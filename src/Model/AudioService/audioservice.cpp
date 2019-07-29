#include "audioservice.h"

//STL
#include <string>
#include <thread>
#include <iomanip>
#include <sstream>

// Custom
#include "../src/View/MainWindow/mainwindow.h"
#include "../ext/FMOD/inc/fmod_errors.h"
#include "../src/Model/Track/track.h"
#include "../src/globalparams.h"

#include <windows.h>


AudioService::AudioService(MainWindow* pMainWindow)
{
    this->pMainWindow   = pMainWindow;
    pSystem             = nullptr;

    bMonitorTracks      = false;
    bIsSomeTrackPlaying = false;

    fCurrentVolume = DEFAULT_VOLUME;
    iCurrentlyPlayingTrackIndex = 0;

    FMODinit();
}




void AudioService::FMODinit()
{
    // FMOD initialization
    FMOD_RESULT result;
    result = FMOD::System_Create(&pSystem);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System_Create() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pMainWindow->showMessageBox( false, "The audio system has not been started and the application will be closed.");
        // Look main.cpp (isSystemReady() function)
        // app will be closed.
        pMainWindow->markAnError();
        return;
    }

    result = pSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, nullptr);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::init() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pMainWindow->showMessageBox( false, "The audio system has not been started and the application will be closed.");
        // Look main.cpp (isSystemReady() function)
        // app will be closed.
        pMainWindow->markAnError();
        return;
    }

    result = pSystem->update();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pMainWindow->showMessageBox( false, "The audio system has not been started and the application will be closed.");
        // Look main.cpp (isSystemReady() function)
        // app will be closed.
        pMainWindow->markAnError();
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
        if (wPathStr[i] == L'/' || wPathStr[i] == L'\\')
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
    trackInfo += L", ";
    std::wstringstream stream;
    stream << std::fixed << std::setprecision(2) << pNewTrack->getSize() / 1024.0 / 1024;
    trackInfo += stream.str();
    trackInfo += L" MB";

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

    mtxThreadLoadAddTrack.lock();


    tracks.push_back(pNewTrack);

    pMainWindow->addNewTrack(trackName, trackInfo, trackTime);

    if (tracks.size() == 1)
    {
        bMonitorTracks = true;
        std::thread monitor(&AudioService::monitorTrack, this);
        monitor.detach();
    }


    mtxThreadLoadAddTrack.unlock();
}

void AudioService::addTracks(std::vector<wchar_t*> paths)
{
    // This function adds tracks by using private 'threadAddTracks()' and 'addTrack()' functions.

    mtxTracksVec.lock();

    // Remove some of the tracks if we exceed MAX_CHANNELS (i.e. max amount of tracks)
    if (paths.size() + tracks.size() > MAX_CHANNELS)
    {
        size_t removeCount = paths.size() + tracks.size() - MAX_CHANNELS;

        for (size_t i = 0; i < removeCount; i++)
        {
            delete[] paths.back();
            paths.pop_back();
        }

        pMainWindow->showMessageBox(true, std::to_string(removeCount) + std::string(" tracks were rejected because with them we will exceed maximum"
                                                                                    " amount of tracks in the tracklist which is " + std::to_string(MAX_CHANNELS)));
    }


    pMainWindow->showWaitWindow();

    // Get amount of CPU threads
    // In every CPU thread we will add tracks
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
        pMainWindow->setFocusOnTrack(tracks.size() - 1);

        mtxTracksVec.unlock();
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
    pMainWindow->setFocusOnTrack(tracks.size() - 1);

    mtxTracksVec.unlock();
}

void AudioService::playTrack(size_t iTrackIndex, bool bCallFromMonitor)
{
    // This function starts track playback.

    if (!bCallFromMonitor) mtxTracksVec.lock();

    if ( iTrackIndex < tracks.size() )
    {
        if (bIsSomeTrackPlaying && (iCurrentlyPlayingTrackIndex != iTrackIndex))
        {
            // Stop currently playing track
            if ( tracks[iCurrentlyPlayingTrackIndex]->stopTrack() )
            {
                // Start new track
                if ( tracks[iTrackIndex]->playTrack(fCurrentVolume) )
                {
                    pMainWindow->removePlayingOnTrack(iCurrentlyPlayingTrackIndex);
                    iCurrentlyPlayingTrackIndex = iTrackIndex;
                    bIsSomeTrackPlaying = true;
                }
                else
                {
                    bIsSomeTrackPlaying = false;
                }
            }
        }
        else
        {
            if (bIsSomeTrackPlaying == false)
            {
                // We will get here if some track is paused and we need to play a new one
                pMainWindow->removePlayingOnTrack(iCurrentlyPlayingTrackIndex);
            }

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

    if (!bCallFromMonitor) mtxTracksVec.unlock();
}

void AudioService::pauseTrack()
{
    // This function pauses / unpauses the track.

    mtxTracksVec.lock();

    if ( tracks.size() > 0 )
    {
        if ( tracks[iCurrentlyPlayingTrackIndex]->pauseTrack() )
        {
            if (bIsSomeTrackPlaying)
            {
                bIsSomeTrackPlaying = false;
            }
            else
            {
                bIsSomeTrackPlaying = true;
            }
        }
    }

    mtxTracksVec.unlock();
}

void AudioService::stopTrack()
{
    // This function stops track if one is playing.

    mtxTracksVec.lock();

    if ( (tracks.size() > 0) && (bIsSomeTrackPlaying) )
    {
        tracks[iCurrentlyPlayingTrackIndex]->stopTrack();
        bIsSomeTrackPlaying = false;
    }

    mtxTracksVec.unlock();
}

void AudioService::nextTrack(bool bCallFromMonitor)
{
    // This function switches to the next track in the 'tracks' vector.

    if (!bCallFromMonitor) mtxTracksVec.lock();

    if ( tracks.size() > 0 )
    {
        if (iCurrentlyPlayingTrackIndex == (tracks.size() - 1))
        {
            if (bCallFromMonitor)
            {
                playTrack(0, true);
            }
            else
            {
                mtxTracksVec.unlock();
                playTrack(0);
            }
        }
        else
        {
            if (bCallFromMonitor)
            {
                playTrack(iCurrentlyPlayingTrackIndex + 1, true);
            }
            else
            {
                mtxTracksVec.unlock();
                playTrack(iCurrentlyPlayingTrackIndex + 1);
            }
        }
    }
    else if (!bCallFromMonitor) mtxTracksVec.unlock();
}

void AudioService::prevTrack()
{
    // This function switches to the previous track in the 'tracks' vector.

    mtxTracksVec.lock();

    if ( tracks.size() > 0 )
    {
        if (iCurrentlyPlayingTrackIndex == 0)
        {
            mtxTracksVec.unlock();

            playTrack( tracks.size() - 1 );
        }
        else
        {
            mtxTracksVec.unlock();

            playTrack(iCurrentlyPlayingTrackIndex - 1);
        }
    }
    else
    {
        mtxTracksVec.unlock();
    }
}

void AudioService::removeTrack(size_t iTrackIndex)
{
    // This function removes the track from the 'tracks' vector.

    mtxTracksVec.lock();

    if ( iTrackIndex < tracks.size() )
    {
        delete tracks[iTrackIndex];
        tracks.erase( tracks.begin() + iTrackIndex );

        if ( tracks.size() == 0)
        {
            bMonitorTracks      = false;
            bIsSomeTrackPlaying = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS));
        }
        else if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex == iCurrentlyPlayingTrackIndex)
            {
                bIsSomeTrackPlaying = false;
                iCurrentlyPlayingTrackIndex = 0;
            }
            else if (iCurrentlyPlayingTrackIndex != 0)
            {
                // Move 'iCurrentlyPlayingTrackIndex' to the left in the 'tracks' vector because of the delete.
                iCurrentlyPlayingTrackIndex--;
            }
        }
    }
    else
    {
        pMainWindow->showMessageBox( false, "Something went wrong. Сan't find this file in the system." );
    }

    mtxTracksVec.unlock();
}

void AudioService::moveDown(size_t iTrackIndex)
{
    mtxTracksVec.lock();

    if (iTrackIndex == tracks.size() - 1)
    {
        Track* pTemp = tracks[iTrackIndex];
        tracks.pop_back();
        tracks.insert( tracks.begin(), pTemp );

        if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex == iCurrentlyPlayingTrackIndex)
            {
                iCurrentlyPlayingTrackIndex = 0;
            }
            else
            {
                iCurrentlyPlayingTrackIndex++;
            }
        }
    }
    else
    {
        Track* pTemp = tracks[iTrackIndex + 1];
        tracks[iTrackIndex + 1] = tracks[iTrackIndex];
        tracks[iTrackIndex] = pTemp;

        if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex == iCurrentlyPlayingTrackIndex)
            {
                iCurrentlyPlayingTrackIndex++;
            }
            else if (iCurrentlyPlayingTrackIndex != 0)
            {
                if (iTrackIndex == iCurrentlyPlayingTrackIndex - 1)
                {
                    iCurrentlyPlayingTrackIndex--;
                }
            }
        }
    }

    mtxTracksVec.unlock();
}

void AudioService::moveUp(size_t iTrackIndex)
{
    mtxTracksVec.lock();

    if (iTrackIndex == 0)
    {
        Track* pTemp = tracks[iTrackIndex];
        tracks.erase( tracks.begin() );
        tracks.push_back( pTemp );

        if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex == iCurrentlyPlayingTrackIndex)
            {
                iCurrentlyPlayingTrackIndex = tracks.size() - 1;
            }
            else
            {
                iCurrentlyPlayingTrackIndex--;
            }
        }
    }
    else
    {
        Track* pTemp = tracks[iTrackIndex - 1];
        tracks[iTrackIndex - 1] = tracks[iTrackIndex];
        tracks[iTrackIndex] = pTemp;

        if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex == iCurrentlyPlayingTrackIndex)
            {
                iCurrentlyPlayingTrackIndex--;
            }
            else if (iTrackIndex == iCurrentlyPlayingTrackIndex + 1)
            {
                iCurrentlyPlayingTrackIndex++;
            }
        }
    }

    mtxTracksVec.unlock();
}

void AudioService::changeVolume(float fNewVolume)
{
    mtxTracksVec.lock();

    if (tracks.size() > 0)
    {
        tracks[iCurrentlyPlayingTrackIndex]->setVolume(fNewVolume);
        fCurrentVolume = fNewVolume;
    }

    mtxTracksVec.unlock();
}

size_t AudioService::getPlayingTrackIndex(bool &bSomeTrackIsPlaying)
{
    mtxTracksVec.lock();

    bSomeTrackIsPlaying = bIsSomeTrackPlaying;

    mtxTracksVec.unlock();
    return iCurrentlyPlayingTrackIndex;
}

void AudioService::monitorTrack()
{
    while (bMonitorTracks)
    {
        mtxTracksVec.lock();

        if (bIsSomeTrackPlaying)
        {
            if (
                    (tracks[iCurrentlyPlayingTrackIndex]->getPositionInMS() >= (tracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() - MAX_TIME_ERROR_MS))
                    &&
                    (tracks[iCurrentlyPlayingTrackIndex]->getPositionInMS() <= (tracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() + MAX_TIME_ERROR_MS))
                )
            {
                // The track is ended, play next
                tracks[iCurrentlyPlayingTrackIndex]->reCreateTrack(fCurrentVolume);
                nextTrack(true);
            }
        }

        mtxTracksVec.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS));
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
    bMonitorTracks = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS));

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
