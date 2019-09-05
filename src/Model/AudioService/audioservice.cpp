#include "audioservice.h"

//STL
#include <string>
#include <thread>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

// Custom
#include "../src/View/MainWindow/mainwindow.h"
#include "../ext/FMOD/inc/fmod_errors.h"
#include "../src/Model/Track/track.h"
#include "../src/globalparams.h"

#include <windows.h>


AudioService::AudioService(MainWindow* pMainWindow)
{
    this->pMainWindow   = pMainWindow;
    pSystem              = nullptr;
    pRndGen              = new std::mt19937_64( static_cast<unsigned long long>(time(nullptr)) );
    iCurrentlyDrawingTrackIndex = new size_t(0);

    bMonitorTracks      = false;
    bIsSomeTrackPlaying = false;
    bRepeatTrack        = false;
    bRandomNextTrack    = false;
    bDrawing            = false;
    bCurrentTrackPaused = false;

    // FX
    pPitch        = nullptr;
    pPitchForTime = nullptr;
    pFaderForTime = nullptr;
    pReverb       = nullptr;
    pEcho         = nullptr;
    pVST          = nullptr;


    fCurrentVolume = DEFAULT_VOLUME;
    iCurrentlyPlayingTrackIndex = 0;
    fCurrentSpeedByPitch = 1.0f;
    fCurrentSpeedByTime  = 1.0f;

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

    FMOD::ChannelGroup* pMaster;
    pSystem->getMasterChannelGroup(&pMaster);


    result = pSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pPitch);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::createDSPByType() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pPitch = nullptr;
    }
    else
    {
        pPitch->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 4096);
        pPitch->setBypass(true);
        pMaster->addDSP(0, pPitch);
    }

    result = pSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pPitchForTime);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::createDSPByType() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pPitchForTime = nullptr;
    }
    else
    {
        pPitchForTime->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 4096);
        pPitchForTime->setBypass(true);
        pMaster->addDSP(1, pPitchForTime);
    }

    result = pSystem->createDSPByType(FMOD_DSP_TYPE_FADER, &pFaderForTime);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::createDSPByType() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pFaderForTime = nullptr;
    }
    else
    {
        pMaster->addDSP(2, pFaderForTime);
    }

    result = pSystem->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &pReverb);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::createDSPByType() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pReverb = nullptr;
    }
    else
    {
        pMaster->addDSP(3, pReverb);
        pReverb->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, -80.0f);
    }

    result = pSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &pEcho);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::createDSPByType() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pEcho = nullptr;
    }
    else
    {
        pMaster->addDSP(4, pEcho);
        pEcho->setParameterFloat(FMOD_DSP_ECHO_DELAY, 1000);
        pEcho->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL, -80.0f);
    }

    pMaster->setPan(0.0f);
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
    stream << std::fixed << std::setprecision(2) << pNewTrack->getFileSizeInBytes() / 1024.0 / 1024;
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

    pNewTrack->setSpeedByFreq(fCurrentSpeedByPitch);
    pNewTrack->setSpeedByTime(fCurrentSpeedByTime);

    mtxThreadLoadAddTrack.lock();


    tracks.push_back(pNewTrack);

    pMainWindow->addNewTrack(trackName, trackInfo, trackTime);

    if (tracks.size() == 1)
    {
        bMonitorTracks = true;
        std::thread monitor(&AudioService::monitorTrack, this);
        monitor.detach();
    }

    FMOD_RESULT result = pSystem->update();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::addTrack::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
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


    pMainWindow->showWaitWindow("Please wait.\nAdding your music.");

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
        // Just add them in one thread

        for (size_t i = 0; i < paths.size(); i++)
        {
            addTrack(paths[i]);
        }

        delete allCount;
        pMainWindow->hideWaitWindow();

        // Wait a little for all track widgets to show
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (tracks.size() > 0) pMainWindow->setFocusOnTrack(tracks.size() - 1);

        mtxTracksVec.unlock();
        return;
    }

    // Check loop if all threads are done
    bool bDone;
    do
    {
        bDone = true;

        // Wait a little for all track widgets to show
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

    // Wait a little for all track widgets to show
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (tracks.size() > 0) pMainWindow->setFocusOnTrack(tracks.size() - 1);

    mtxTracksVec.unlock();
}

void AudioService::playTrack(size_t iTrackIndex, bool bDontLockMutex)
{
    // This function starts track playback.

    if (!bDontLockMutex) mtxTracksVec.lock();

    if ( iTrackIndex < tracks.size() )
    {
        if ( ((bCurrentTrackPaused == false) && (bIsSomeTrackPlaying == false)) || (iTrackIndex != iCurrentlyPlayingTrackIndex) )
        {
            if (bDrawing)
            {
                // Some thread is drawing, stop it
                bDrawing = false;
                mtxDrawGraph.lock();
                mtxDrawGraph.unlock();
            }

            // Add new graph
            *iCurrentlyDrawingTrackIndex = iTrackIndex;
            std::thread drawGraphThread(&AudioService::drawGraph, this, iCurrentlyDrawingTrackIndex);
            drawGraphThread.detach();
        }


        // Stop currently playing track
        if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex != iCurrentlyPlayingTrackIndex)
            {
                tracks[iCurrentlyPlayingTrackIndex]->stopTrack();
                pMainWindow->removePlayingOnTrack(iCurrentlyPlayingTrackIndex);
            }
        }
        else if (bCurrentTrackPaused && (iTrackIndex != iCurrentlyPlayingTrackIndex))
        {
            tracks[iCurrentlyPlayingTrackIndex]->stopTrack();
            pMainWindow->removePlayingOnTrack(iCurrentlyPlayingTrackIndex);
        }

        // Play track
        if ( tracks[iTrackIndex]->playTrack(fCurrentVolume) )
        {
            bIsSomeTrackPlaying = true;
            bCurrentTrackPaused = false;
            iCurrentlyPlayingTrackIndex = iTrackIndex;
        }
        else
        {
            bIsSomeTrackPlaying = false;
            bCurrentTrackPaused = false;
            iCurrentlyPlayingTrackIndex = 0;
            pMainWindow->showMessageBox(false, "Something went wrong and we could not play the track.");
        }

        FMOD_RESULT result = pSystem->update();
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("AudioService::playTrack::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }

        pMainWindow->setPlayingOnTrack(iCurrentlyPlayingTrackIndex);
    }
    else
    {
        pMainWindow->showMessageBox( false, "Something went wrong. Сan't find this file in the system." );
    }

    if (!bDontLockMutex) mtxTracksVec.unlock();
}

void AudioService::setTrackPos(unsigned int graphPos)
{
    mtxTracksVec.lock();

    if ( (tracks.size() > 0) && (bIsSomeTrackPlaying || bCurrentTrackPaused) )
    {
        // track->getMaxValueOnGraph() - 100%
        // graphPos                    - x%

        // cast to unsigned long long to avoid overflow
        double fPosMult = graphPos / static_cast<double>(tracks[iCurrentlyPlayingTrackIndex]->getMaxValueOnGraph());
        unsigned int iPosInMS = static_cast<unsigned int>(tracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() * fPosMult);
        if ( tracks[iCurrentlyPlayingTrackIndex]->setPositionInMS(iPosInMS) )
        {
            pMainWindow->setCurrentPos(fPosMult, tracks[iCurrentlyPlayingTrackIndex]->getCurrentTime());
        }
    }

    FMOD_RESULT result;
    result = pSystem->update();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::setTrackPos::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
    }

    mtxTracksVec.unlock();
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
                bCurrentTrackPaused = true;
            }
            else
            {
                bIsSomeTrackPlaying = true;
            }
        }
    }

    FMOD_RESULT result;
    result = pSystem->update();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::pauseTrack::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
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
        bCurrentTrackPaused = true;
        pMainWindow->clearGraph(true);
    }

    mtxTracksVec.unlock();
}

void AudioService::nextTrack(bool bDontLockMutex, bool bRandomNextTrack)
{
    // This function switches to the next track in the 'tracks' vector.

    if (!bDontLockMutex) mtxTracksVec.lock();

    if ( tracks.size() > 0 )
    {
        if (bRandomNextTrack)
        {
            size_t iNewTrackIndex = iCurrentlyPlayingTrackIndex;

            if (tracks.size() != 1)
            {
                std::uniform_int_distribution<> uid(0, static_cast<int>(tracks.size()) - 1);

                do
                {
                    iNewTrackIndex = static_cast<size_t>(uid(*pRndGen));

                }while (iNewTrackIndex == iCurrentlyPlayingTrackIndex);
            }

            playTrack(iNewTrackIndex, true);
        }
        else
        {
            if (iCurrentlyPlayingTrackIndex == (tracks.size() - 1))
            {
                playTrack(0, true);
            }
            else
            {
                playTrack(iCurrentlyPlayingTrackIndex + 1, true);
            }
        }
    }

    if (!bDontLockMutex) mtxTracksVec.unlock();
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
        mtxGetCurrentDrawingIndex.lock();

        if ( tracks.size() == 0)
        {
            bMonitorTracks      = false;
            bIsSomeTrackPlaying = false;
            bCurrentTrackPaused = false;
            iCurrentlyPlayingTrackIndex = 0;
            pMainWindow->clearGraph();
        }
        else if (bIsSomeTrackPlaying || bCurrentTrackPaused)
        {
            if (iTrackIndex == iCurrentlyPlayingTrackIndex)
            {
                mtxGetCurrentDrawingIndex.unlock();
                bDrawing = false;
                mtxDrawGraph.lock();
                mtxDrawGraph.unlock();
                mtxGetCurrentDrawingIndex.lock();

                bIsSomeTrackPlaying = false;
                bCurrentTrackPaused = false;
                iCurrentlyPlayingTrackIndex = 0;

                // Clear Track Name and Track Info
                pMainWindow->setPlayingOnTrack(0, true);

                pMainWindow->clearGraph();
            }
            else if ( (iCurrentlyPlayingTrackIndex != 0) && (iTrackIndex < iCurrentlyPlayingTrackIndex) )
            {
                // Move 'iCurrentlyPlayingTrackIndex' to the left in the 'tracks' vector because of the delete.
                iCurrentlyPlayingTrackIndex--;
                *iCurrentlyDrawingTrackIndex -= 1;
            }
        }

        delete tracks[iTrackIndex];
        tracks.erase( tracks.begin() + iTrackIndex );

        FMOD_RESULT result;
        result = pSystem->update();
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("AudioService::removeTrack::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }

        mtxGetCurrentDrawingIndex.unlock();
    }
    else
    {
        pMainWindow->showMessageBox( false, "Something went wrong. Сan't find this file in the system." );
    }

    mtxTracksVec.unlock();
}

void AudioService::clearPlaylist()
{
    mtxTracksVec.lock();

    bDrawing = false;
    mtxDrawGraph.lock();
    mtxDrawGraph.unlock();

    bMonitorTracks = false;
    bIsSomeTrackPlaying = false;

    iCurrentlyPlayingTrackIndex = 0;

    for (size_t i = 0; i < tracks.size(); i++)
    {
        delete tracks[i];
    }
    tracks.clear();

    fCurrentVolume = DEFAULT_VOLUME;

    mtxTracksVec.unlock();
}

void AudioService::saveTracklist(std::wstring pathToTracklist)
{
    mtxTracksVec.lock();

    pMainWindow->showWaitWindow("Saving...");

    std::ofstream tracklistFile(pathToTracklist, std::ios::binary);

    short iTrackCount = static_cast<short>(tracks.size());
    tracklistFile.write(reinterpret_cast<char*>(&iTrackCount), 2);

    for (size_t i = 0; i < tracks.size(); i++)
    {
        std::wstring trackPath( tracks[i]->getFilePath() );
        short iPathSize = static_cast<short>(trackPath.size()) * 2;

        // Write path size
        tracklistFile.write(reinterpret_cast<char*>(&iPathSize), sizeof(iPathSize));

        tracklistFile.write(reinterpret_cast<char*>( const_cast<wchar_t*>(trackPath.c_str()) ), iPathSize);
    }

    tracklistFile.close();

    pMainWindow->hideWaitWindow();

    mtxTracksVec.unlock();
}

void AudioService::openTracklist(std::wstring pathToTracklist, bool bClearCurrent)
{
    if (bClearCurrent)
    {
        pMainWindow->clearCurrentPlaylist();
        clearPlaylist();
    }

    std::ifstream tracklistFile(pathToTracklist, std::ios::binary);
    if (tracklistFile.is_open())
    {
        mtxTracksVec.lock();

        short iTrackPathSize = 0;
        std::vector<wchar_t*> newTracks;

        short iTrackCount = 0;
        tracklistFile.read(reinterpret_cast<char*>(&iTrackCount), 2);

        for (short i = 0; i < iTrackCount; i++)
        {
            tracklistFile.read(reinterpret_cast<char*>(&iTrackPathSize), 2);

            wchar_t* pNewTrack = new wchar_t[static_cast<size_t>(iTrackPathSize + 2)];
            memset(pNewTrack, 0, static_cast<size_t>(iTrackPathSize + 2));

            tracklistFile.read(reinterpret_cast<char*>(pNewTrack), iTrackPathSize);
            newTracks.push_back(pNewTrack);
        }

        tracklistFile.close();
        mtxTracksVec.unlock();

        std::thread addThread(&AudioService::addTracks, this, newTracks);
        addThread.detach();
    }
    else
    {
        pMainWindow->showMessageBox(true, "Can't open file.");
    }
}

void AudioService::setPan(float fPan)
{
    mtxTracksVec.lock();

    if (tracks.size() > 0)
    {
        FMOD::ChannelGroup* pMaster;
        pSystem->getMasterChannelGroup(&pMaster);
        pMaster->setPan(fPan);
    }

    pSystem->update();

    mtxTracksVec.unlock();
}

void AudioService::setPitch(float fPitch)
{
    if (pPitch)
    {
        pPitch->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, fPitch);

        if (fPitch != 1.0f)
        {
            pPitch->setBypass(false);
        }
        else
        {
            pPitch->setBypass(true);
        }
    }
}

void AudioService::setSpeedByPitch(float fSpeed)
{
    mtxTracksVec.lock();

    fCurrentSpeedByPitch = fSpeed;

    if (tracks.size() > 0)
    {
        for (size_t i = 0; i < tracks.size(); i++)
        {
            tracks[i]->setSpeedByFreq(fSpeed);
        }
    }

    pSystem->update();

    mtxTracksVec.unlock();
}

void AudioService::setSpeedByTime(float fSpeed)
{
    mtxTracksVec.lock();

    fCurrentSpeedByTime = fSpeed;

    if (tracks.size() > 0)
    {
        for (size_t i = 0; i < tracks.size(); i++)
        {
            tracks[i]->setSpeedByTime(fSpeed);
        }
    }

    pPitchForTime->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 1.0f / fSpeed);

    if (fSpeed != 1.0f)
    {
        pPitchForTime->setBypass(false);
    }
    else
    {
        pPitchForTime->setBypass(true);
    }

    pSystem->update();

    mtxTracksVec.unlock();
}

void AudioService::setReverbVolume(float fVolume)
{
    if (pReverb)
    {
        pReverb->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, fVolume);
        pSystem->update();
    }
}

void AudioService::setEchoVolume(float fEchoVolume)
{
    if (pEcho)
    {
        pEcho->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL, fEchoVolume);
        pSystem->update();
    }
}

void AudioService::loadVSTPlugin(wchar_t *pPathToDll)
{
    // wchar_t is 16 bits and holds UTF-16 code units
    // FMOD accepts UTF-8 strings
    // convert wchar_t* (UTF-16) to char* (UTF-8)
    char filePathInUTF8[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, pPathToDll, -1, filePathInUTF8, sizeof(filePathInUTF8), nullptr, nullptr);

    delete[] pPathToDll;

    FMOD_RESULT result;

    result = pSystem->loadPlugin(reinterpret_cast<const char*>(&filePathInUTF8), &iVSTHandle);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::loadVSTPlugin::FMOD::System::loadPlugin() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return;
    }
    result = pSystem->createDSPByPlugin(iVSTHandle, &pVST);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::loadVSTPlugin::FMOD::System::createDSPByPlugin() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pVST = nullptr;
        pSystem->unloadPlugin(iVSTHandle);
        return;
    }

    FMOD::ChannelGroup* pMaster;
    pSystem->getMasterChannelGroup(&pMaster);

    pMaster->addDSP(5, pVST);

    char name[MAX_PATH];
    memset(name, 0, MAX_PATH);
    pVST->getInfo(name, nullptr, nullptr, nullptr, nullptr);

    result = pVST->showConfigDialog(pMainWindow->getVSTWindowHWND(), true);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::loadVSTPlugin::FMOD::DSP::showConfigDialog() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
    }

    pMainWindow->setVSTName(name);

    pSystem->update();
}

void AudioService::unloadVSTPlugin()
{
    if (pVST)
    {
        FMOD::ChannelGroup* pMaster;
        pSystem->getMasterChannelGroup(&pMaster);

        FMOD_RESULT result;

        result = pVST->showConfigDialog(pMainWindow->getVSTWindowHWND(), false);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("AudioService::unloadVSTPlugin::FMOD::DSP::showConfigDialog() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }

        result = pMaster->removeDSP(pVST);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("AudioService::unloadVSTPlugin::FMOD::ChannelGroup::removeDSP() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }
        result = pVST->release();
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("AudioService::unloadVSTPlugin::FMOD::DSP::release() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }

        do
        {
            result = pSystem->update();
            if (result)
            {
                pMainWindow->showMessageBox( true, std::string("AudioService::unloadVSTPlugin::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            }
            result = pSystem->unloadPlugin(iVSTHandle);
        } while (result == FMOD_ERR_DSP_INUSE);

        pVST = nullptr;

        pMainWindow->hideVSTWindow();
    }
}

void AudioService::systemUpdate()
{
    pSystem->update();
}

void AudioService::moveDown(size_t iTrackIndex)
{
    mtxTracksVec.lock();

    mtxGetCurrentDrawingIndex.lock();

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
                *iCurrentlyDrawingTrackIndex = 0;
            }
            else
            {
                iCurrentlyPlayingTrackIndex++;
                *iCurrentlyDrawingTrackIndex += 1;
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
                *iCurrentlyDrawingTrackIndex += 1;
            }
            else if (iCurrentlyPlayingTrackIndex != 0)
            {
                if (iTrackIndex == iCurrentlyPlayingTrackIndex - 1)
                {
                    iCurrentlyPlayingTrackIndex--;
                    *iCurrentlyDrawingTrackIndex -= 1;
                }
            }
        }
    }

    mtxGetCurrentDrawingIndex.unlock();

    mtxTracksVec.unlock();
}

void AudioService::moveUp(size_t iTrackIndex)
{
    mtxTracksVec.lock();

    mtxGetCurrentDrawingIndex.lock();

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
                *iCurrentlyDrawingTrackIndex = tracks.size() - 1;
            }
            else
            {
                iCurrentlyPlayingTrackIndex--;
                *iCurrentlyDrawingTrackIndex -= 1;
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
                *iCurrentlyDrawingTrackIndex -= 1;
            }
            else if (iTrackIndex == iCurrentlyPlayingTrackIndex + 1)
            {
                iCurrentlyPlayingTrackIndex++;
                *iCurrentlyDrawingTrackIndex += 1;
            }
        }
    }

    mtxGetCurrentDrawingIndex.unlock();

    mtxTracksVec.unlock();
}

void AudioService::repeatTrack()
{
    bRepeatTrack = !bRepeatTrack;

    if (bRepeatTrack && bRandomNextTrack)
    {
        bRandomNextTrack = false;
        pMainWindow->uncheckRandomTrackButton();
    }
}

void AudioService::randomNextTrack()
{
    bRandomNextTrack = !bRandomNextTrack;

    if (bRandomNextTrack && bRepeatTrack)
    {
        bRepeatTrack = false;
        pMainWindow->uncheckRepeatTrackButton();
    }
}

void AudioService::setVolume(float fNewVolume)
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

                FMOD_RESULT result = pSystem->update();
                if (result)
                {
                    pMainWindow->showMessageBox(true, std::string("AudioService::monitorTrack::FMOD::System::update() failed. Error: ") + FMOD_ErrorString(result));
                }

                if (bRepeatTrack)
                {
                    // Or not
                    playTrack(iCurrentlyPlayingTrackIndex, true);
                }
                else
                {
                    if (bRandomNextTrack)
                    {
                        nextTrack(true, true);
                    }
                    else
                    {
                        nextTrack(true);
                    }
                }
            }
            else
            {
                // track->getLengthInMS()   - 1.0
                // track->getPositionInMS() - x

                double x = static_cast<double>(tracks[iCurrentlyPlayingTrackIndex]->getPositionInMS()) / tracks[iCurrentlyPlayingTrackIndex]->getLengthInMS();

                pMainWindow->setCurrentPos(x, tracks[iCurrentlyPlayingTrackIndex]->getCurrentTime());
            }
        }

        pSystem->update();

        mtxTracksVec.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS));
    }
}

void AudioService::drawGraph(size_t* iTrackIndex)
{
    mtxDrawGraph.lock();
    bDrawing = true;


    // default: 16384
    // "If FMOD_TIMEUNIT_RAWBYTES is used, the memory allocated is two times the size passed in, because fmod allocates a double buffer."
    // 131072 * 2 = 256 kB (x8 times bigger than default) to speed up our graph draw (to speed up our readData() calls)
    FMOD_RESULT fresult = pSystem->setStreamBufferSize(131072, FMOD_TIMEUNIT_RAWBYTES);
    if (fresult)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::setStreamBufferSize() failed. Error: ") + std::string(FMOD_ErrorString(fresult)) );
    }


    pMainWindow->clearGraph();

    // this value combines 'iSamplesInOne' samples in one to store less points for graph in memory
    // more than '200' on a track that is about 5 minutes long looks bad
    // for example 2.5 min track with 'iSamplesInOne' = 100, adds like 3 MB to RAM
    // but less value can fill RAM very bad
    unsigned int iSamplesInOne = 150;
    // so we calculate 'iSamplesInOne' like this:
    // 3000 ('iSamplesInOne') - 6000 (sec.)
    // x    ('iSamplesInOne') - track length (in sec.)

    mtxGetCurrentDrawingIndex.lock();
    // here we do: 3000 * (tracks[iTrackIndex]->getLengthInMS() / 1000) / 6000, but we can replace this with just:
    iSamplesInOne = tracks[*iTrackIndex]->getLengthInMS() * 3 / 6000;
    if (iSamplesInOne == 0) iSamplesInOne = 1;

    unsigned int iTempMax = tracks[*iTrackIndex]->getLengthInPCMbytes() / 4 / iSamplesInOne;
    pMainWindow->setXMaxToGraph(iTempMax);
    tracks[*iTrackIndex]->setMaxPosInGraph(iTempMax);

    tracks[*iTrackIndex]->createDummySound();
    mtxGetCurrentDrawingIndex.unlock();

    // 2 MB because you need to multiply this value by 2 (because of short int type of buffer).
    unsigned int iBufferSize = 1048576;
    unsigned int iGraphMax = 0;
    char result = 1;

    do
    {
        short int* pSamples = new short int[iBufferSize];
        unsigned int iActuallyRead;

        mtxGetCurrentDrawingIndex.lock();
        result = tracks[*iTrackIndex]->getPCMSamples(pSamples, iBufferSize * 2, &iActuallyRead);
        mtxGetCurrentDrawingIndex.unlock();

        if (result == 0)
        {
            // error
            delete[] pSamples;
            break;
        }
        else if ((result == -1) && (iActuallyRead == 0))
        {
            delete[] pSamples;
            break;
        }


        // Because 'iActuallyRead' is amount of L and R samples and we in MainWindow mix L and R channels in one.
        iGraphMax += iActuallyRead / 2 / iSamplesInOne;

        pMainWindow->addDataToGraph(pSamples, iActuallyRead, iSamplesInOne);

    }while ( (result == 1) && (bDrawing) );

    mtxGetCurrentDrawingIndex.lock();

    if (bDrawing)
    {
        pMainWindow->setXMaxToGraph(iGraphMax);
        tracks[*iTrackIndex]->setMaxPosInGraph(iGraphMax);
    }

    tracks[*iTrackIndex]->releaseDummySound();

    mtxGetCurrentDrawingIndex.unlock();



    // set to default: 16384
    fresult = pSystem->setStreamBufferSize(16384, FMOD_TIMEUNIT_RAWBYTES);
    if (fresult)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::setStreamBufferSize() failed. Error: ") + std::string(FMOD_ErrorString(fresult)) );
    }


    bDrawing = false;
    mtxDrawGraph.unlock();
}

void AudioService::threadAddTracks(std::vector<wchar_t*> paths, bool* done, int* allCount, int all)
{
    for (size_t i = 0; i < paths.size(); i++)
    {
        addTrack(paths[i]);

        mtxLoadThreadDone.lock();
        *allCount += 1;
        pMainWindow->setProgress( *allCount *  100 / all);
        mtxLoadThreadDone.unlock();
    }

    *done = true;
}


AudioService::~AudioService()
{
    bDrawing = false;
    mtxDrawGraph.lock();
    mtxDrawGraph.unlock();

    delete iCurrentlyDrawingTrackIndex;

    delete pRndGen;

    bMonitorTracks = false;
    mtxTracksVec.lock();
    mtxTracksVec.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS));

    // FX
    if ( (pPitch != nullptr) || (pPitchForTime != nullptr) || (pFaderForTime != nullptr) || (pReverb != nullptr) || (pEcho != nullptr) || (pVST != nullptr) )
    {
        FMOD::ChannelGroup* pMaster;
        pSystem->getMasterChannelGroup(&pMaster);

        if (pPitch)
        {
            pMaster->removeDSP(pPitch);
            pPitch->release();
        }
        if (pPitchForTime)
        {
            pMaster->removeDSP(pPitchForTime);
            pPitchForTime->release();
        }
        if (pFaderForTime)
        {
            pMaster->removeDSP(pFaderForTime);
            pFaderForTime->release();
        }
        if (pReverb)
        {
            pMaster->removeDSP(pReverb);
            pReverb->release();
        }
        if (pEcho)
        {
            pMaster->removeDSP(pEcho);
            pEcho->release();
        }
        if (pVST)
        {
            pMaster->removeDSP(pVST);
            pVST->release();
            pSystem->unloadPlugin(iVSTHandle);
        }

        pSystem->update();
    }

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
