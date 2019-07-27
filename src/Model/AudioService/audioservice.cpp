#include "audioservice.h"

//STL
#include <string>
#include <thread>

// Custom
#include "../src/View/MainWindow/mainwindow.h"
#include "../ext/FMOD/inc/fmod_errors.h"
#include "../src/Model/Track/track.h"
#include "../src/globalparams.h"

#include <windows.h>

AudioService::AudioService(MainWindow* pMainWindow)
{
    this->pMainWindow = pMainWindow;
    pSystem           = nullptr;
    bSystemReady      = false;
    bTrackIsPaused    = false;
    bIsSomeTrackPlaying = false;
    fCurrentVolume = DEFAULT_VOLUME;
    iCurrentlyPlayingTrackIndex = 0;

    // FMOD init
    FMOD_RESULT result;
    result = FMOD::System_Create(&pSystem);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System_Create() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return;
    }

    result = pSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, nullptr);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::init() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return;
    }

    result = pSystem->update();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return;
    }
}




void AudioService::addTrack(const wchar_t *pFilePath)
{
    Track* pNewTrack = new Track(pMainWindow, pSystem);
    if ( !pNewTrack->setTrack(pFilePath) )
    {
        delete pNewTrack;
        return;
    }

    std::wstring wPathStr(pFilePath);

    // Get track name
    size_t iNameStartIndex = 0;
    for (size_t i = wPathStr.size() - 1; i >= 1; i--)
    {
        if (wPathStr[i] == L'/')
        {
            iNameStartIndex = i + 1;
            break;
        }
    }
    std::wstring trackName = L"";
    for (size_t i = iNameStartIndex; i < wPathStr.size() - 4; i++)
    {
        trackName += wPathStr[i];
    }
    if (trackName.back() == L'.') trackName.pop_back();

    // Get track info
    std::wstring trackInfo = L"";
    for (size_t i = wPathStr.size() - 4; i < wPathStr.size(); i++)
    {
        if (wPathStr[i] != L'.')
        {
            trackInfo += wPathStr[i];
        }
    }
    trackInfo += L", ";
    trackInfo += std::to_wstring( static_cast<int>(pNewTrack->getFrequency()) );
    trackInfo += L"hz";
    if (pNewTrack->getFormat() == "MP3")
    {
        int bitrate;
        bool ok = pNewTrack->getBitRate(&bitrate);
        if (ok)
        {
            trackInfo += L", ";
            trackInfo += std::to_wstring(bitrate);
            trackInfo += L" kbit/s";
        }
    }
    int channels, bits;
    if (pNewTrack->getChannelsAndBits(&channels, &bits))
    {
        trackInfo += L", channels: ";
        trackInfo += std::to_wstring(channels);
        trackInfo += L", ";
        trackInfo += std::to_wstring(bits);
        trackInfo += L" bits";
    }

    // Get track time
    unsigned int iMS = pNewTrack->getLengthInMS();
    unsigned int iSeconds = iMS / 1000;
    unsigned int iMinutes = iSeconds / 60;
    iSeconds -= (iMinutes * 60);
    std::string trackTime = "";
    trackTime += std::to_string(iMinutes);
    trackTime += ":";
    if (iSeconds < 10) trackTime += "0";
    trackTime += std::to_string(iSeconds);

    mtxAddTrack.lock();


    tracks.push_back(pNewTrack);

    pMainWindow->addNewTrack(trackName, trackInfo, trackTime);

    if (tracks.size() == 1)
    {
        bSystemReady = true;
        std::thread monitor(&AudioService::monitorTrack, this);
        monitor.detach();
    }


    mtxAddTrack.unlock();
}

void AudioService::addTracks(std::vector<wchar_t*> paths)
{
    if (paths.size() + tracks.size() > MAX_CHANNELS)
    {
        size_t removeCount = paths.size() + tracks.size() - MAX_CHANNELS;

        for (size_t i = 0; i < removeCount; i++)
        {
            delete[] paths.back();
            paths.pop_back();
        }
    }


    pMainWindow->showWaitWindow();

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    size_t threads = info.dwNumberOfProcessors;

    std::vector<bool*> threadsDoneFlags;
    int* allCount = new int(0);

    if (paths.size() >= threads*2)
    {
        size_t iPathsOnOneThread = paths.size() / threads;
        size_t iCurrentPos = 0;

        std::vector<wchar_t*> onThread;
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (iCurrentPos < iPathsOnOneThread)
            {
                onThread.push_back(paths[i]);
                iCurrentPos++;
            }

            if (iCurrentPos == iPathsOnOneThread)
            {
                threadsDoneFlags.push_back(new bool(false));
                std::thread t (&AudioService::threadAddTracks, this, onThread, threadsDoneFlags.back(), allCount, static_cast<int>(paths.size()));
                t.detach();

                onThread.clear();
                iCurrentPos = 0;
            }
        }

        if (onThread.size() > 0)
        {
            threadsDoneFlags.push_back(new bool(false));
            std::thread t (&AudioService::threadAddTracks, this, onThread, threadsDoneFlags.back(), allCount, static_cast<int>(paths.size()));
            t.detach();
        }
    }
    else
    {
        for (size_t i = 0; i < paths.size(); i++)
        {
            addTrack(paths[i]);
        }

        delete allCount;
        pMainWindow->hideWaitWindow();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pMainWindow->setFocusOnLastTrack();
        return;
    }

    bool bDone;
    do
    {
        bDone = true;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        for (size_t i = 0; i < threadsDoneFlags.size(); i++)
        {
            if (*threadsDoneFlags[i] == false)
            {
                bDone = false;
                break;
            }
        }

    }while(bDone == false);

    delete allCount;

    for (size_t i = 0; i < threadsDoneFlags.size(); i++)
    {
        delete threadsDoneFlags[i];
    }

    pMainWindow->hideWaitWindow();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pMainWindow->setFocusOnLastTrack();
}

void AudioService::playTrack(size_t iTrackIndex)
{
    if ( iTrackIndex < tracks.size() )
    {
        if (bIsSomeTrackPlaying || bTrackIsPaused)
        {
            if (iCurrentlyPlayingTrackIndex == iTrackIndex)
            {
                if (!bTrackIsPaused) tracks[iTrackIndex]->setPositionInMS(0);

                if ( tracks[iTrackIndex]->playTrack(fCurrentVolume) )
                {
                    bIsSomeTrackPlaying = true;
                    bTrackIsPaused = false;
                    iCurrentlyPlayingTrackIndex = iTrackIndex;
                }
            }
            else
            {
                if ( tracks[iCurrentlyPlayingTrackIndex]->stopTrack() )
                {
                    if ( tracks[iTrackIndex]->playTrack(fCurrentVolume) )
                    {
                        pMainWindow->removePlayingOnTrack(iCurrentlyPlayingTrackIndex);
                        bIsSomeTrackPlaying = true;
                        bTrackIsPaused = false;
                        iCurrentlyPlayingTrackIndex = iTrackIndex;
                    }
                }
            }
        }
        else
        {
            if ( tracks[iTrackIndex]->playTrack(fCurrentVolume) )
            {
                bIsSomeTrackPlaying = true;
                iCurrentlyPlayingTrackIndex = iTrackIndex;
            }
        }

        pMainWindow->setPlayingOnTrack(iCurrentlyPlayingTrackIndex);
    }
    else
    {
        pMainWindow->showMessageBox( false, "Something went wrong. Сan't find this file in the system." );
    }
}

void AudioService::pauseTrack()
{
    if ( tracks.size() > 0 )
    {
        if ( tracks[iCurrentlyPlayingTrackIndex]->pauseTrack() )
        {
            if (bIsSomeTrackPlaying)
            {
                bIsSomeTrackPlaying = false;
                bTrackIsPaused = true;
            }
            else
            {
                bIsSomeTrackPlaying = true;
                bTrackIsPaused = false;
            }
        }
    }
}

void AudioService::stopTrack()
{
    if ( (tracks.size() > 0) && (bIsSomeTrackPlaying) )
    {
        tracks[iCurrentlyPlayingTrackIndex]->stopTrack();
        bIsSomeTrackPlaying = false;
        bTrackIsPaused = true;
    }
}

void AudioService::nextTrack()
{
    if ( tracks.size() > 0 )
    {
        if (iCurrentlyPlayingTrackIndex == (tracks.size() - 1))
        {
            playTrack(0);
        }
        else
        {
            playTrack(iCurrentlyPlayingTrackIndex + 1);
        }
    }
}

void AudioService::prevTrack()
{
    if ( tracks.size() > 0 )
    {
        if (iCurrentlyPlayingTrackIndex == 0)
        {
            playTrack( tracks.size() - 1 );
        }
        else
        {
            playTrack(iCurrentlyPlayingTrackIndex - 1);
        }
    }
}

void AudioService::removeTrack(size_t iTrackIndex)
{
    if ( iTrackIndex < tracks.size() )
    {
        if ( (tracks.size() - 1) == 0)
        {
            bSystemReady = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        delete tracks[iTrackIndex];
        tracks.erase( tracks.begin() + iTrackIndex );
    }
    else
    {
        pMainWindow->showMessageBox( false, "Something went wrong. Сan't find this file in the system." );
    }
}

void AudioService::changeVolume(float fNewVolume)
{
    if (tracks.size() > 0)
    {
        tracks[iCurrentlyPlayingTrackIndex]->setVolume(fNewVolume);
        fCurrentVolume = fNewVolume;
    }
}

size_t AudioService::getPlayingTrackIndex(bool &bSomeTrackIsPlaying)
{
    bSomeTrackIsPlaying = bIsSomeTrackPlaying;
    return iCurrentlyPlayingTrackIndex;
}

void AudioService::monitorTrack()
{
    while (bSystemReady)
    {
        if (bIsSomeTrackPlaying)
        {
            if ( tracks[iCurrentlyPlayingTrackIndex]->getPositionInMS() >= (tracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() - 100) )
            {
                // Track will end now, play next
                nextTrack();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void AudioService::threadAddTracks(std::vector<wchar_t*> paths, bool* done, int* allCount, int all)
{
    for (size_t i = 0; i < paths.size(); i++)
    {
        addTrack(paths[i]);

        mtxThreadDone.lock();
        *allCount += 1;
        pMainWindow->setProgress( *allCount *  100 / all);
        mtxThreadDone.unlock();
    }

    *done = true;
}


AudioService::~AudioService()
{
    bSystemReady = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    FMOD_RESULT result;

    for (size_t i = 0; i < tracks.size(); i++)
    {
        delete tracks[i];
    }
    tracks.clear();

    result = pSystem->release();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::FMOD::System::release::~AudioService() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
    }
}
