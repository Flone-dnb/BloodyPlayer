// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#include "audioservice.h"

//STL
#include <string>
#include <thread>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

// Custom
#include "View/MainWindow/mainwindow.h"
#include "Model/Track/track.h"
#include "globalparams.h"
#include "../ext/FMOD/inc/fmod_errors.h"

// Other
#include <windows.h>
#include <shlobj.h>

#pragma comment(lib, "Shell32.lib") // for <shlobj.h>


AudioService::AudioService(MainWindow* pMainWindow)
{
    sBloodyVersion       = BLOODY_PLAYER_VERSION;


    this ->pMainWindow   = pMainWindow;
    pSystem              = nullptr;
    pRndGen              = new std::mt19937_64( std::random_device{}() );
    iCurrentlyDrawingTrackIndex = new size_t(0);


    bMonitorTracks      = false;
    bIsSomeTrackPlaying = false;
    bRepeatTrack        = false;
    bRandomNextTrack    = false;
    bDrawing            = false;
    bCurrentTrackPaused = false;
    bFMODStarted        = false;


    cRepeatSectionState = 0;


    // FX
    pPitch        = nullptr;
    pPitchForTime = nullptr;
    pReverb       = nullptr;
    pEcho         = nullptr;
    pVST          = nullptr;


    fCurrentVolume = DEFAULT_VOLUME;
    iCurrentlyPlayingTrackIndex = 0;
    fCurrentSpeedByPitch = 1.0f;
    fCurrentSpeedByTime  = 1.0f;


    if ( FMODinit() == false )
    {
        showTutorial();
    }
}

void AudioService::doNotShowTutorialAgain()
{
    TCHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

    if (result == S_OK)
    {
        std::wstring adressToSettings    = std::wstring(my_documents);
        std::wstring adressToOldSettings = std::wstring(my_documents);
        adressToSettings    += L"\\BloodyPlayerSettings.data";
        adressToOldSettings += L"\\BloodyPlayerSettings_.data";

        _wrename(adressToSettings.c_str(), adressToOldSettings.c_str());

        std::ofstream settingsFileNew (adressToSettings, std::ios::binary);

        char cNeverShowTutorialAgain = 1;
        char cTutorialFinished       = 0;

        settingsFileNew.write ( &cNeverShowTutorialAgain, sizeof(cNeverShowTutorialAgain) );
        settingsFileNew.write ( &cTutorialFinished,       sizeof(cTutorialFinished)       );

        settingsFileNew.close();

        _wremove(adressToOldSettings.c_str());
    }
    else
    {
        pMainWindow->showMessageBox(true, "AudioService::doNotShowTutorialAgain::SHGetFolderPathW() failed.");
    }
}

void AudioService::tutorialEnd()
{
    TCHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

    if (result == S_OK)
    {
        std::wstring adressToSettings    = std::wstring(my_documents);
        std::wstring adressToOldSettings = std::wstring(my_documents);
        adressToSettings    += L"\\BloodyPlayerSettings.data";
        adressToOldSettings += L"\\BloodyPlayerSettings_.data";

        _wrename(adressToSettings.c_str(), adressToOldSettings.c_str());

        std::ofstream settingsFileNew (adressToSettings, std::ios::binary);

        char cNeverShowTutorialAgain = 0;
        char cTutorialFinished       = 1;

        settingsFileNew.write ( &cNeverShowTutorialAgain, sizeof(cNeverShowTutorialAgain) );
        settingsFileNew.write ( &cTutorialFinished,       sizeof(cTutorialFinished)       );

        settingsFileNew.close();

        _wremove(adressToOldSettings.c_str());
    }
    else
    {
        pMainWindow->showMessageBox(true, "AudioService::doNotShowTutorialAgain::SHGetFolderPathW() failed.");
    }
}




bool AudioService::FMODinit()
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
        return true;
    }

    result = pSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, nullptr);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::init() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pMainWindow->showMessageBox( false, "The audio system has not been started and the application will be closed.");
        // Look main.cpp (isSystemReady() function)
        // app will be closed.
        pMainWindow->markAnError();
        return true;
    }

    result = pSystem->update();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::AudioService::FMOD::System::update() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        pMainWindow->showMessageBox( false, "The audio system has not been started and the application will be closed.");
        // Look main.cpp (isSystemReady() function)
        // app will be closed.
        pMainWindow->markAnError();
        return true;
    }



    // FX

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
        pReverb->setBypass(true);
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
        pEcho->setBypass(true);
    }

    pMaster->setPan(0.0f);


    bFMODStarted = true;
    return false;
}

void AudioService::showTutorial()
{
    TCHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

    if (result == S_OK)
    {
        std::wstring adressToSettings = std::wstring(my_documents);
        adressToSettings += L"\\BloodyPlayerSettings.data";

        std::ifstream settingsFile (adressToSettings, std::ios::binary);

        if ( settingsFile.is_open() )
        {
            // Check if tutorial finished

            char cNeverShowTutorialAgain = 0;
            char cTutorialFinished       = 0;

            settingsFile.read ( &cNeverShowTutorialAgain, sizeof(cNeverShowTutorialAgain) );
            settingsFile.read ( &cTutorialFinished,       sizeof(cTutorialFinished)       );

            settingsFile.close();



            if (cNeverShowTutorialAgain == 0 && cTutorialFinished == 0)
            {
                pMainWindow->showTutorialWindow();
            }
        }
        else
        {
            // First time opened app

            std::ofstream settingsFileNew (adressToSettings, std::ios::binary);

            char cNeverShowTutorialAgain = 0;
            char cTutorialFinished       = 0;

            settingsFileNew.write ( &cNeverShowTutorialAgain, sizeof(cNeverShowTutorialAgain) );
            settingsFileNew.write ( &cTutorialFinished,       sizeof(cTutorialFinished)       );

            settingsFileNew.close();

            pMainWindow->showTutorialWindow();
        }
    }
    else
    {
        pMainWindow->showMessageBox(true, "AudioService::showTutorial::SHGetFolderPathW() failed.");
    }
}

void AudioService::addTrack(const wchar_t *pFilePath)
{
    Track* pNewTrack = new Track(pFilePath, getTrackName(pFilePath), pMainWindow, pSystem);
    if ( !pNewTrack->setupTrack() )
    {
        delete pNewTrack;
        return;
    }

    std::wstring wPathStr(pFilePath);



    // Get track name

    std::wstring trackName = getTrackName(pFilePath);



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
    std::string trackTime = "";
    unsigned int iMS = pNewTrack->getLengthInMS();
    unsigned int iSeconds = iMS / 1000;
    unsigned int iMinutes = iSeconds / 60;
    iSeconds -= (iMinutes * 60);
    trackTime += std::to_string(iMinutes);
    trackTime += ":";
    if (iSeconds < 10) trackTime += "0";
    trackTime += std::to_string(iSeconds);

    pNewTrack->setSpeedByFreq(fCurrentSpeedByPitch);
    pNewTrack->setSpeedByTime(fCurrentSpeedByTime);

    mtxThreadLoadAddTrack.lock();


    vTracks.push_back(pNewTrack);

    pMainWindow->addNewTrack(trackName, trackInfo, trackTime);

    if (vTracks.size() == 1)
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

std::wstring AudioService::getTrackName(const wchar_t *pFilePath)
{
    std::wstring wPathStr(pFilePath);

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


    return trackName;
}

void AudioService::addTracks(std::vector<wchar_t*> paths)
{
    // This function adds tracks by using private 'threadAddTracks()' and 'addTrack()' functions.

    mtxTracksVec.lock();

    // Remove some of the tracks if we exceed MAX_CHANNELS (i.e. max amount of tracks)
    if (paths.size() + vTracks.size() > MAX_CHANNELS)
    {
        size_t removeCount = paths.size() + vTracks.size() - MAX_CHANNELS;

        for (size_t i = 0; i < removeCount; i++)
        {
            delete[] paths.back();
            paths.pop_back();
        }

        pMainWindow->showMessageBox(true, "Maximum tracks exceeded. " + std::to_string(removeCount) + " tracks have not been added.");
    }



    if (paths .size() >= MIN_TRACKS_TO_SHOW_LOADING)
    {
        pMainWindow->showWaitWindow("Please wait...\n"
                                    "Adding your music.");
    }


    // Get amount of CPU threads
    // In every CPU thread we will add tracks
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    size_t threads = info.dwNumberOfProcessors;

    std::vector<bool*> threadsDoneFlags;
    int* allCount = new int(0);


    if (paths.size() >= threads * 2)
    {
        size_t iPathsOnOneThread = paths.size() / threads;
        size_t iCurrentPos = 0;

        size_t iStart, iStop;
        for (size_t i = 0; i < paths.size(); i++)
        {
            if (iCurrentPos < iPathsOnOneThread)
            {
                if (iCurrentPos == 0) iStart = i;
                iCurrentPos++;
            }

            if (iCurrentPos == iPathsOnOneThread)
            {
                iStop = i;

                threadsDoneFlags.push_back(new bool(false));
                std::thread t (&AudioService::threadAddTracks, this, &paths, iStart, iStop, threadsDoneFlags.back(), allCount, static_cast<int>(paths.size()));
                t.detach();

                iCurrentPos = 0;
            }
        }

        if (iCurrentPos > 0)
        {
            iStop = paths.size() - 1;

            threadsDoneFlags.push_back(new bool(false));
            std::thread t (&AudioService::threadAddTracks, this, &paths, iStart, iStop, threadsDoneFlags.back(), allCount, static_cast<int>(paths.size()));
            t.detach();
        }
    }
    else
    {
        // Just add them in one thread

        for (size_t i = 0; i < paths.size(); i++)
        {
            addTrack(paths[i]);

            if (paths .size() >= MIN_TRACKS_TO_SHOW_LOADING)
            {
                pMainWindow->setProgress( static_cast<int>(100.0f * ( i + 1 ) / paths.size()) );
            }
        }
    }

    if (paths.size() >= threads * 2)
    {
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
    }

    delete allCount;

    for (size_t i = 0; i < threadsDoneFlags.size(); i++)
    {
        delete threadsDoneFlags[i];
    }

    if (paths .size() >= MIN_TRACKS_TO_SHOW_LOADING)
    {
        pMainWindow->hideWaitWindow();
    }

    // Wait until MainWindow added all tracks
    do
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(WAIT_FOR_UI_IN_MS) );
    } while ( pMainWindow->getTracksCount() != vTracks.size() );

    pMainWindow->showAllTracks();

    // Wait to show all tracks
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );

    pMainWindow->setFocusOnTrack( vTracks.size() - 1 );

    mtxTracksVec.unlock();
}

void AudioService::playTrack(size_t iTrackIndex, bool bDontLockMutex)
{
    // This function starts track playback.

    if (!bDontLockMutex) mtxTracksVec.lock();


    size_t iOldPlayingTrackIndex = iCurrentlyPlayingTrackIndex;
    bool   bFirstTrack           = false;

    if ( iTrackIndex < vTracks.size() )
    {
        if ( ((bCurrentTrackPaused == false) && (bIsSomeTrackPlaying == false)) || (iTrackIndex != iCurrentlyPlayingTrackIndex) )
        {
            // If I'm correct then (bCurrentTrackPaused == false) && (bIsSomeTrackPlaying == false) should happend only when
            // we play first track after player's started.

            bFirstTrack = true;

            if (bDrawing)
            {
                // Some thread is drawing on the oscillogram, stop it.

                bDrawing = false;

                // A way to wait for that thread to end.
                mtxDrawGraph.lock();
                mtxDrawGraph.unlock();
            }

            // Draw new oscillogram.
            *iCurrentlyDrawingTrackIndex = iTrackIndex;
            std::thread drawGraphThread(&AudioService::drawGraph, this, iCurrentlyDrawingTrackIndex);
            drawGraphThread.detach();
        }

        if (bDontLockMutex == false)
        {
            // This function is called not from monitorTrack() function.
            // Check if the current track is ended.

            if ( isCurrentTrackEnded() )
            {
                vTracks[iCurrentlyPlayingTrackIndex]->reCreateTrack(fCurrentVolume);
            }
        }

        // Stop currently playing track
        if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex != iCurrentlyPlayingTrackIndex)
            {
                vTracks[iCurrentlyPlayingTrackIndex]->stopTrack();
                pMainWindow->removePlayingOnTrack(iCurrentlyPlayingTrackIndex);
            }
        }
        else if (bCurrentTrackPaused && (iTrackIndex != iCurrentlyPlayingTrackIndex))
        {
            vTracks[iCurrentlyPlayingTrackIndex]->stopTrack();
            pMainWindow->removePlayingOnTrack(iCurrentlyPlayingTrackIndex);
        }


        bool         bOldTrackWasPaused = bCurrentTrackPaused;
        unsigned int iTrackOldPos       = vTracks[iCurrentlyPlayingTrackIndex]->getPositionInMS();


        // Play track
        if ( vTracks[iTrackIndex]->playTrack(fCurrentVolume) )
        {
            bIsSomeTrackPlaying  = true;
            bCurrentTrackPaused  = false;


            iCurrentlyPlayingTrackIndex  = iTrackIndex;



            // Add to history.

            vTracksHistory.push_back(vTracks[iTrackIndex]);

            if (vTracksHistory.size() > MAX_HISTORY_SIZE)
            {
                vTracksHistory.erase( vTracksHistory.begin() );
            }



            if ( (cRepeatSectionState != 0) && (iCurrentlyPlayingTrackIndex != iOldPlayingTrackIndex) )
            {
                pMainWindow->eraseRepeatSection();

                cRepeatSectionState = 0;
            }
            else if ( (cRepeatSectionState != 0) && (iCurrentlyPlayingTrackIndex == iOldPlayingTrackIndex) )
            {
                if (bOldTrackWasPaused)
                {
                    if ( vTracks[iTrackIndex]->getPositionInMS() >= iFirstRepeatTimePos )
                    {
                        vTracks[iTrackIndex]->setPositionInMS(iTrackOldPos);
                    }
                    else
                    {
                        vTracks[iTrackIndex]->setPositionInMS(iFirstRepeatTimePos);
                    }
                }
                else
                {
                    vTracks[iTrackIndex]->setPositionInMS(iFirstRepeatTimePos);
                }
            }
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



    if ( (bFirstTrack) )
    {
        if ( (vTracks[iCurrentlyPlayingTrackIndex] ->isBitrateCalculated() == false)
             &&
             (vTracks[iCurrentlyPlayingTrackIndex] ->getFormat() == "MP3") )
        {
            std::thread tCalcBitrate(&AudioService::calcBitrate, this);
            tCalcBitrate .detach();
        }
    }
}

void AudioService::setTrackPos(unsigned int graphPos)
{
    mtxTracksVec.lock();

    if ( (vTracks.size() > 0) && (bIsSomeTrackPlaying || bCurrentTrackPaused) )
    {
        // track->getMaxValueOnGraph() - 100%
        // graphPos                    - x%

        double fPosMult = graphPos / static_cast<double>(vTracks[iCurrentlyPlayingTrackIndex]->getMaxValueOnGraph());
        // cast to avoid overflow
        unsigned int iPosInMS = static_cast<unsigned int>(vTracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() * fPosMult);

        if (cRepeatSectionState == 2)
        {
            if ( (iPosInMS > iFirstRepeatTimePos) && (iPosInMS < iSecondRepeatTimePos) )
            {
                if ( vTracks[iCurrentlyPlayingTrackIndex]->setPositionInMS(iPosInMS) )
                {
                    pMainWindow->setCurrentPos(fPosMult, vTracks[iCurrentlyPlayingTrackIndex]->getCurrentTime());
                }
            }
        }
        else
        {
            if ( vTracks[iCurrentlyPlayingTrackIndex]->setPositionInMS(iPosInMS) )
            {
                pMainWindow->setCurrentPos(fPosMult, vTracks[iCurrentlyPlayingTrackIndex]->getCurrentTime());
            }
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

void AudioService::setRepeatPoint(unsigned int graphPos)
{
    mtxTracksVec.lock();

    if ( (vTracks.size() > 0) && (bIsSomeTrackPlaying || bCurrentTrackPaused) )
    {
        // track->getMaxValueOnGraph() - 100%
        // graphPos                    - x%

        double fPosMult = graphPos / static_cast<double>(vTracks[iCurrentlyPlayingTrackIndex]->getMaxValueOnGraph());

        if (cRepeatSectionState == 0)
        {
            // Set the first point
            pMainWindow->setRepeatPoint(true, fPosMult);

            cRepeatSectionState = 1;

            unsigned int iPosInMS = static_cast<unsigned int>(vTracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() * fPosMult);
            iFirstRepeatTimePos = iPosInMS;


            // Set track pos
            if ( vTracks[iCurrentlyPlayingTrackIndex]->getPositionInMS() < iFirstRepeatTimePos )
            {
                if ( vTracks[iCurrentlyPlayingTrackIndex]->setPositionInMS(iFirstRepeatTimePos) )
                {
                    pMainWindow->setCurrentPos(fPosMult, vTracks[iCurrentlyPlayingTrackIndex]->getCurrentTime());
                }
            }
        }
        else if (cRepeatSectionState == 1)
        {
            unsigned int iPosInMS = static_cast<unsigned int>(vTracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() * fPosMult);
            iSecondRepeatTimePos = iPosInMS;

            if ( iPosInMS
                 < (vTracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() - MAX_SECOND_REPEAT_BOUND_FROM_END_MS) )
            {
                if ( iPosInMS > iFirstRepeatTimePos + MAX_SECOND_REPEAT_BOUND_FROM_END_MS )
                {
                    // One point already on graph, set the second one
                    pMainWindow->setRepeatPoint(false, fPosMult);

                    // Done
                    cRepeatSectionState = 2;


                    // Set track pos
                    if ( vTracks[iCurrentlyPlayingTrackIndex]->getPositionInMS() > iSecondRepeatTimePos )
                    {
                        if ( vTracks[iCurrentlyPlayingTrackIndex]->setPositionInMS(iFirstRepeatTimePos) )
                        {
                            pMainWindow->setCurrentPos(fPosMult, vTracks[iCurrentlyPlayingTrackIndex]->getCurrentTime());
                        }
                    }
                }
            }
        }
        else
        {
            // User pressed RMB again, erase section
            pMainWindow->eraseRepeatSection();

            cRepeatSectionState = 0;
        }
    }

    mtxTracksVec.unlock();
}

std::string AudioService::getBloodyVersion()
{
    return sBloodyVersion;
}

void AudioService::pauseTrack()
{
    // This function pauses / unpauses the track.

    mtxTracksVec.lock();

    if ( vTracks.size() > 0 )
    {
        if ( isCurrentTrackEnded() )
        {
            vTracks[iCurrentlyPlayingTrackIndex] ->reCreateTrack(fCurrentVolume);
        }

        if ( vTracks[iCurrentlyPlayingTrackIndex]->pauseTrack() )
        {
            if (bIsSomeTrackPlaying)
            {
                bIsSomeTrackPlaying = false;
                bCurrentTrackPaused = true;
            }
            else
            {
                bIsSomeTrackPlaying = true;
                bCurrentTrackPaused = false;
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

    if ( (vTracks.size() > 0) && (bIsSomeTrackPlaying || bCurrentTrackPaused) )
    {
        vTracks[iCurrentlyPlayingTrackIndex]->stopTrack();
        bIsSomeTrackPlaying = false;
        bCurrentTrackPaused = true;
        pMainWindow->clearGraph(true);
    }

    mtxTracksVec.unlock();
}

void AudioService::nextTrack(bool bDontLockMutex, bool bRandomNextTrackLocal)
{
    // This function switches to the next track in the 'tracks' vector.

    if (!bDontLockMutex) mtxTracksVec.lock();

    if ( vTracks.size() > 0 )
    {
        if ( isCurrentTrackEnded() )
        {
            vTracks[iCurrentlyPlayingTrackIndex]->reCreateTrack(fCurrentVolume);
        }

        if (bRandomNextTrackLocal)
        {
            size_t iNewTrackIndex = iCurrentlyPlayingTrackIndex;

            if (vTracks.size() != 1)
            {
                std::uniform_int_distribution<> uid(0, static_cast<int>(vTracks.size()) - 1);

                do
                {
                    iNewTrackIndex = static_cast<size_t>(uid(*pRndGen));

                }while (iNewTrackIndex == iCurrentlyPlayingTrackIndex);
            }

            playTrack(iNewTrackIndex, true);
        }
        else
        {
            if (iCurrentlyPlayingTrackIndex == (vTracks.size() - 1))
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

    if ( vTracks.size() > 0 )
    {
        if ( isCurrentTrackEnded() )
        {
            vTracks[iCurrentlyPlayingTrackIndex]->reCreateTrack(fCurrentVolume);
        }

        if (vTracksHistory.size() > 1)
        {
            size_t iTrackIndex = 0;

            Track* pPrevTrack = vTracksHistory[ vTracksHistory.size() - 2 ];

            for (size_t i = 0; i < vTracks.size(); i++)
            {
                if (pPrevTrack == vTracks[i])
                {
                    iTrackIndex = i;
                    break;
                }
            }


            mtxTracksVec.unlock();

            vTracksHistory.pop_back();
            vTracksHistory.pop_back();

            playTrack ( iTrackIndex );
        }
        else
        {
            mtxTracksVec.unlock();

            playTrack( iCurrentlyPlayingTrackIndex );
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

    if ( iTrackIndex < vTracks.size() )
    {
        mtxGetCurrentDrawingIndex.lock();

        if ( vTracks.size() == 0)
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


        // Erase from vTracksHistory
        for (size_t i = 0; i < vTracksHistory.size(); i++)
        {
            if (vTracksHistory[i] == vTracks[iTrackIndex])
            {
                vTracksHistory.erase ( vTracksHistory.begin() + i );
                break;
            }
        }

        delete vTracks[iTrackIndex];
        vTracks.erase( vTracks.begin() + iTrackIndex );

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

    for (size_t i = 0; i < vTracks.size(); i++)
    {
        delete vTracks[i];
    }
    vTracks.clear();

    //fCurrentVolume = DEFAULT_VOLUME;


    vTracksHistory.clear();

    mtxTracksVec.unlock();
}

void AudioService::saveTracklist(std::wstring pathToTracklist)
{
    mtxTracksVec.lock();

    //pMainWindow->showWaitWindow("Saving...");

    std::ofstream tracklistFile(pathToTracklist, std::ios::binary);

    short iTrackCount = static_cast<short>(vTracks.size());
    tracklistFile.write(reinterpret_cast<char*>(&iTrackCount), 2);

    for (size_t i = 0; i < vTracks.size(); i++)
    {
        std::wstring trackPath( vTracks[i]->getFilePath() );
        short iPathSize = static_cast<short>(trackPath.size()) * 2;

        // Write path size
        tracklistFile.write(reinterpret_cast<char*>(&iPathSize), sizeof(iPathSize));

        tracklistFile.write(reinterpret_cast<char*>( const_cast<wchar_t*>(trackPath.c_str()) ), iPathSize);
    }

    tracklistFile.close();

    //pMainWindow->hideWaitWindow();

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

        addTracks(newTracks);
        //std::thread addThread(&AudioService::addTracks, this, newTracks);
        //addThread.detach();
    }
    else
    {
        pMainWindow->showMessageBox(true, "Can't open file.");
    }
}

void AudioService::setPan(float fPan)
{
    mtxTracksVec.lock();

    if (vTracks.size() > 0)
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

    if (vTracks.size() > 0)
    {
        for (size_t i = 0; i < vTracks.size(); i++)
        {
            vTracks[i]->setSpeedByFreq(fSpeed);
        }
    }

    pSystem->update();

    mtxTracksVec.unlock();
}

void AudioService::setSpeedByTime(float fSpeed)
{
    mtxTracksVec.lock();

    fCurrentSpeedByTime = fSpeed;

    if (vTracks.size() > 0)
    {
        for (size_t i = 0; i < vTracks.size(); i++)
        {
            vTracks[i]->setSpeedByTime(fSpeed);
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
        if (fVolume <= -80.0f)
        {
            pReverb->setBypass(true);
        }
        else
        {
            pReverb->setBypass(false);
        }

        pReverb->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, fVolume);
        pSystem->update();
    }
}

void AudioService::setEchoVolume(float fEchoVolume)
{
    if (pEcho)
    {
        if (fEchoVolume <= -80.0f)
        {
            pEcho->setBypass(true);
        }
        else
        {
            pEcho->setBypass(false);
        }

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

    if (iTrackIndex == vTracks.size() - 1)
    {
        Track* pTemp = vTracks[iTrackIndex];
        vTracks.pop_back();
        vTracks.insert( vTracks.begin(), pTemp );

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
        Track* pTemp = vTracks[iTrackIndex + 1];
        vTracks[iTrackIndex + 1] = vTracks[iTrackIndex];
        vTracks[iTrackIndex] = pTemp;

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
        Track* pTemp = vTracks[iTrackIndex];
        vTracks.erase( vTracks.begin() );
        vTracks.push_back( pTemp );

        if (bIsSomeTrackPlaying)
        {
            if (iTrackIndex == iCurrentlyPlayingTrackIndex)
            {
                iCurrentlyPlayingTrackIndex = vTracks.size() - 1;
                *iCurrentlyDrawingTrackIndex = vTracks.size() - 1;
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
        Track* pTemp = vTracks[iTrackIndex - 1];
        vTracks[iTrackIndex - 1] = vTracks[iTrackIndex];
        vTracks[iTrackIndex] = pTemp;

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

void AudioService::searchFindPrev()
{
    if (vSearchResult.size() > 0)
    {
        if (bFirstSearchAfterKeyChange == false)
        {
            if (iCurrentPosInSearchVec == 0)
            {
                iCurrentPosInSearchVec = vSearchResult.size() - 1;
            }
            else
            {
                iCurrentPosInSearchVec--;
            }
        }


        if ( !(bFirstSearchAfterKeyChange == false && vSearchResult.size() == 1) ) // do not select again if matches == 1 && already selected
        {
            pMainWindow->setFocusOnTrack   ( vSearchResult[iCurrentPosInSearchVec] );
            pMainWindow->searchSetSelected ( vSearchResult[iCurrentPosInSearchVec] );
        }


        bFirstSearchAfterKeyChange = false;
    }
}

void AudioService::searchFindNext()
{
    if (vSearchResult.size() > 0)
    {
        if (bFirstSearchAfterKeyChange == false)
        {
            if (iCurrentPosInSearchVec == vSearchResult.size() - 1)
            {
                iCurrentPosInSearchVec = 0;
            }
            else
            {
                iCurrentPosInSearchVec++;
            }
        }


        if ( !(bFirstSearchAfterKeyChange == false && vSearchResult.size() == 1) ) // do not select again if matches == 1 && already selected
        {
            pMainWindow->setFocusOnTrack   ( vSearchResult[iCurrentPosInSearchVec] );
            pMainWindow->searchSetSelected ( vSearchResult[iCurrentPosInSearchVec] );
        }


        bFirstSearchAfterKeyChange = false;
    }
}

void AudioService::searchTextSet(const std::wstring &sKeyword)
{
    mtxTracksVec.lock();


    vSearchResult.clear();
    iCurrentPosInSearchVec = 0;

    if (sKeyword != L"")
    {
        for (size_t i = 0; i < vTracks.size(); i++)
        {
            if ( findCaseInsensitive( vTracks[i]->getTrackName(), const_cast<std::wstring&>(sKeyword)) != std::string::npos )
            {
                vSearchResult.push_back(i);
            }
        }
    }

    bFirstSearchAfterKeyChange = true;


    mtxTracksVec.unlock();


    pMainWindow->setSearchMatchCount (vSearchResult.size());
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

    if (vTracks.size() > 0)
    {
        vTracks[iCurrentlyPlayingTrackIndex]->setVolume(fNewVolume);
    }

    fCurrentVolume = fNewVolume;

    mtxTracksVec.unlock();
}

size_t AudioService::getPlayingTrackIndex(bool &bSomeTrackIsPlaying)
{
    mtxTracksVec.lock();

    bSomeTrackIsPlaying = bIsSomeTrackPlaying;

    mtxTracksVec.unlock();
    return iCurrentlyPlayingTrackIndex;
}

bool AudioService::isFMODStarted()
{
    return bFMODStarted;
}

bool AudioService::isSomeTrackIsPlaying()
{
    return bIsSomeTrackPlaying;
}

bool AudioService::isCurrentTrackPaused()
{
    return bCurrentTrackPaused;
}

FMOD::System *AudioService::getFMODSystem()
{
    return pSystem;
}

Track *AudioService::getCurrentTrack()
{
    mtxTracksVec .lock();
    mtxTracksVec .unlock();

    if ( bCurrentTrackPaused == false && bIsSomeTrackPlaying )
    {
        return vTracks[ iCurrentlyPlayingTrackIndex ];
    }
    else
    {
        return nullptr;
    }
}

size_t AudioService::getTracksCount()
{
    mtxTracksVec .lock();
    mtxTracksVec .unlock();

    return vTracks .size();
}

void AudioService::monitorTrack()
{
    while (bMonitorTracks)
    {
        mtxTracksVec.lock();

        if (bIsSomeTrackPlaying)
        {
            if ( isCurrentTrackEnded() )
            {
                switchToOtherTrack();
            }
            else
            {
                if (cRepeatSectionState == 2)
                {
                    if ( vTracks[iCurrentlyPlayingTrackIndex]->getPositionInMS() >= iSecondRepeatTimePos - MAX_TIME_ERROR_MS )
                    {
                        for (float i = fCurrentVolume; i >= 0.0f; i-= 0.01f)
                        {
                            vTracks[iCurrentlyPlayingTrackIndex]->setVolume(i);

                            std::this_thread::sleep_for (std::chrono::milliseconds(TRANSITION_SLEEP_MS));
                        }

                        vTracks[iCurrentlyPlayingTrackIndex]->setPositionInMS (iFirstRepeatTimePos);

                        for (float i = 0; i <= fCurrentVolume; i+= 0.01f)
                        {
                            vTracks[iCurrentlyPlayingTrackIndex]->setVolume(i);

                            std::this_thread::sleep_for (std::chrono::milliseconds(TRANSITION_SLEEP_MS));
                        }
                    }
                }


                // track->getLengthInMS()   - 1.0
                // track->getPositionInMS() - x

                double x = static_cast<double>(vTracks[iCurrentlyPlayingTrackIndex]->getPositionInMS()) / vTracks[iCurrentlyPlayingTrackIndex]->getLengthInMS();

                pMainWindow->setCurrentPos(x, vTracks[iCurrentlyPlayingTrackIndex]->getCurrentTime());
            }
        }

        pSystem->update();

        mtxTracksVec.unlock();

        if (bMonitorTracks) std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS));
    }
}

bool AudioService::isCurrentTrackEnded()
{
    bool bError = false;

    unsigned int iCurrentTimeInMS = vTracks[iCurrentlyPlayingTrackIndex]->getPositionInMS(&bError);

    if (bError)
    {
        // The track is ended?
        return true;
    }

    if ( iCurrentTimeInMS >= (vTracks[iCurrentlyPlayingTrackIndex]->getLengthInMS() - MAX_TIME_ERROR_MS) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void AudioService::switchToOtherTrack()
{
    // This function is executed in mtxTracksVec.lock();

    // The track is ended
    // Play next
    vTracks[iCurrentlyPlayingTrackIndex]->reCreateTrack(fCurrentVolume);

    FMOD_RESULT result = pSystem->update();
    if (result)
    {
        pMainWindow->showMessageBox(true, std::string("AudioService::monitorTrack::FMOD::System::update() failed. Error: ") + FMOD_ErrorString(result));
    }

    if (bRepeatTrack || cRepeatSectionState == 1)
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

    // this value combines 'iOnlySamplesInOneRead' samples in one to store less points for graph in memory
    // more than '200' on a track that is about 5 minutes long looks bad
    // for example 2.5 min track with 'iSamplesInOne' = 100, adds like 3 MB to RAM
    // but less value can fill RAM very bad
    unsigned int iOnlySamplesInOneRead = 150;
    // so we calculate 'iSamplesInOne' like this:
    // 3000 ('iSamplesInOne') - 6000 (sec.)
    // x    ('iSamplesInOne') - track length (in sec.)

    mtxGetCurrentDrawingIndex.lock();



    // here we do: 3000 * (tracks[iTrackIndex]->getLengthInMS() / 1000) / 6000, but we can replace this with just:
    iOnlySamplesInOneRead = vTracks[*iTrackIndex]->getLengthInMS() * 3 / 6000;
    if (iOnlySamplesInOneRead == 0) iOnlySamplesInOneRead = 1;


    // Set max on graph
    std::string format = vTracks[*iTrackIndex]->getPCMFormat();
    unsigned int iTempMax;
    if (format == "PCM16")
    {
        iTempMax = vTracks[*iTrackIndex]->getLengthInPCMbytes() / 4 / iOnlySamplesInOneRead;
    }
    else
    {
        iTempMax = vTracks[*iTrackIndex]->getLengthInPCMbytes() / 6 / iOnlySamplesInOneRead;
    }

    pMainWindow->setXMaxToGraph(iTempMax);
    vTracks[*iTrackIndex]->setMaxPosInGraph(iTempMax);


    vTracks[*iTrackIndex]->createDummySound();
    mtxGetCurrentDrawingIndex.unlock();

    // Buffer Size = 2 MB
    unsigned int iBufferSize = 2097152;
    unsigned int iGraphMax = 0;
    char pcmFormat = 0;
    char result = 1;

    do
    {
        char* pSamplesBuffer = new char[iBufferSize];
        unsigned int iActuallyReadBytes;




        // Read

        mtxGetCurrentDrawingIndex.lock();

        result = vTracks[*iTrackIndex]->getPCMSamples(pSamplesBuffer, iBufferSize, &iActuallyReadBytes, &pcmFormat);

        mtxGetCurrentDrawingIndex.unlock();




        // Check for errors

        if (result == 0)
        {
            delete[] pSamplesBuffer;
            break;
        }
        else if ((result == -1) && (iActuallyReadBytes == 0))
        {
            delete[] pSamplesBuffer;
            break;
        }




        // Convert to proper massive format

        if (pcmFormat == 16)
        {
            // PCM16

            if (iActuallyReadBytes % 4 != 0)
            {
                // Wil probably appear on end of file

                while (iActuallyReadBytes % 4 != 0)
                {
                    iActuallyReadBytes--;
                }
            }



            float* pSamples = rawBytesToPCM16_0_1 (pSamplesBuffer, iActuallyReadBytes);


            // Delete raw buffer

            delete[] pSamplesBuffer;


            // Send to Main Window new buffer

            iGraphMax += iActuallyReadBytes / 4 / iOnlySamplesInOneRead;

            pMainWindow->addDataToGraph(pSamples, iActuallyReadBytes / 2, iOnlySamplesInOneRead);
        }
        else
        {
            // PCM24

            if (iActuallyReadBytes % 6 != 0)
            {
                // Wil probably appear on end of file

                while (iActuallyReadBytes % 6 != 0)
                {
                    iActuallyReadBytes--;
                }
            }



            float* pSamples = rawBytesToPCM24_0_1 (pSamplesBuffer, iActuallyReadBytes);


            // Delete raw buffer

            delete[] pSamplesBuffer;


            // Send to Main Window new buffer

            iGraphMax += iActuallyReadBytes / 6 / iOnlySamplesInOneRead;

            pMainWindow->addDataToGraph(pSamples, iActuallyReadBytes / 3, iOnlySamplesInOneRead);
        }

    }while ( (result == 1) && (bDrawing) );



    mtxGetCurrentDrawingIndex.lock();


    if (bDrawing)
    {
        pMainWindow->setXMaxToGraph(iGraphMax);
        vTracks[*iTrackIndex]->setMaxPosInGraph(iGraphMax);
    }

    vTracks[*iTrackIndex]->releaseDummySound();



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

float *AudioService::rawBytesToPCM16_0_1(char *pBuffer, unsigned int iBufferSizeInBytes)
{
    float* pSamples     = new float[ iBufferSizeInBytes / 2 ];

    size_t iPosInSamples = 0;

    for (size_t i = 0; i < iBufferSizeInBytes; i += 4)
    {
        short int iSampleL = 0;

        std::memcpy(reinterpret_cast<char*>(&iSampleL),     &pBuffer[i],     1);
        std::memcpy(reinterpret_cast<char*>(&iSampleL) + 1, &pBuffer[i + 1], 1);

        unsigned short int iSampleLUnsigned = static_cast<unsigned short int> (iSampleL + USHRT_MAX / 2.0f);

        pSamples[iPosInSamples] = static_cast<float>(iSampleLUnsigned) / USHRT_MAX;
        iPosInSamples++;



        short int iSampleR = 0;

        std::memcpy(reinterpret_cast<char*>(&iSampleR),     &pBuffer[i + 2], 1);
        std::memcpy(reinterpret_cast<char*>(&iSampleR) + 1, &pBuffer[i + 3], 1);

        unsigned short int iSampleRUnsigned = static_cast<unsigned short int> (iSampleR + USHRT_MAX / 2.0f);

        pSamples[iPosInSamples] = static_cast<float>(iSampleRUnsigned) / USHRT_MAX;
        iPosInSamples++;
    }

    return pSamples;
}

float *AudioService::rawBytesToPCM24_0_1(char *pBuffer, unsigned int iBufferSizeInBytes)
{
    float* pSamples     = new float [ iBufferSizeInBytes / 3 ];

    size_t iPosInSamples = 0;

    // 8388607 is 2^24 / 2
    int iMin24BitSigned = 8388608;

    for (size_t i = 0; i < iBufferSizeInBytes; i += 6)
    {
        int iSampleL = interpret24bitAsInt32 (pBuffer[i + 2], pBuffer[i + 1], pBuffer[i]);

        unsigned int iSampleLUnsigned = static_cast<unsigned int> (iSampleL + iMin24BitSigned);

        pSamples[iPosInSamples] = static_cast<float>(iSampleLUnsigned / pow(2, 24));
        iPosInSamples++;



        int iSampleR = interpret24bitAsInt32 (pBuffer[i + 5], pBuffer[i + 4], pBuffer[i + 3]);

        unsigned int iSampleRUnsigned = static_cast<unsigned int> (iSampleR + iMin24BitSigned);

        pSamples[iPosInSamples] = static_cast<float>(iSampleRUnsigned / pow(2, 24));
        iPosInSamples++;
    }


    return pSamples;
}

int AudioService::interpret24bitAsInt32(char byte0, char byte1, char byte2)
{
    // copy-paste from stackoverflow
    return ( (byte0 << 24) | (byte1 << 16) | (byte2 << 8) ) >> 8;
}

size_t AudioService::findCaseInsensitive(std::wstring& sText, std::wstring& sKeyword)
{
    // All to lower case

    std::transform(sText.begin(), sText.end(), sText.begin(), ::tolower);

    std::transform(sKeyword.begin(), sKeyword.end(), sKeyword.begin(), ::tolower);

    return sText.find(sKeyword);
}

void AudioService::calcBitrate()
{
    mtxTracksVec .lock();

    int iBitrate = 0;

    bool bResult = vTracks[iCurrentlyPlayingTrackIndex] ->getBitRate(&iBitrate);

    if (bResult)
    {
        pMainWindow ->setTrackBitrate(iCurrentlyPlayingTrackIndex, std::to_string(iBitrate));
    }


    mtxTracksVec .unlock();
}

void AudioService::threadAddTracks(std::vector<wchar_t*>* paths, size_t iStart, size_t iStop, bool* done, int* allCount, int all)
{
    for (size_t i = iStart; i <= iStop; i++)
    {
        addTrack(paths->operator[](i));


        mtxLoadThreadDone.lock();

        *allCount += 1;
        if (paths ->size() >= MIN_TRACKS_TO_SHOW_LOADING)
        {
            pMainWindow->setProgress( *allCount *  100 / all);
        }

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
    if ( (pPitch != nullptr) || (pPitchForTime != nullptr) || (pReverb != nullptr) || (pEcho != nullptr) || (pVST != nullptr) )
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

    for (size_t i = 0; i < vTracks.size(); i++)
    {
        delete vTracks[i];
    }
    vTracks.clear();

    result = pSystem->release();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("AudioService::FMOD::System::release::~AudioService() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
    }
}
