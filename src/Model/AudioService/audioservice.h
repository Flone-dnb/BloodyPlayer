﻿// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#pragma once



// STL
#include <vector>
#include <mutex>
#include <random>

// FMOD
#include "../ext/FMOD/inc/fmod.hpp"
#if _WIN32
#pragma comment(lib, "fmod_vc.lib")
#endif





class MainWindow;
class Track;






class AudioService
{

public:

    AudioService(MainWindow* pMainWindow);


    // Main functions

        void    playTrack            (size_t iTrackIndex,           bool bDontLockMutex = false);
        void    nextTrack            (bool bDontLockMutex = false,  bool bRandomNextTrackLocal = false);
        void    pauseTrack           ();
        void    stopTrack            ();
        void    prevTrack            ();


    // Buttons under the volume slider

        void    repeatTrack          ();
        void    randomNextTrack      ();
        void    clearPlaylist        ();


    // Tracklist functions

        void    saveTracklist        (std::wstring pathToTracklist);
        void    openTracklist        (std::wstring pathToTracklist,  bool bClearCurrent);


    // Tracklist management

        void    removeTrack          (size_t    iTrackIndex);
        void    moveDown             (size_t    iTrackIndex);
        void    moveUp               (size_t    iTrackIndex);


    // Search

        void    searchFindPrev       ();
        void    searchFindNext       ();
        void    searchTextSet        (const std::wstring& sKeyword);


    // FX

        void    setPan               (float     fPan);
        void    setPitch             (float     fPitch);
        void    setSpeedByPitch      (float     fSpeed);
        void    setSpeedByTime       (float     fSpeed);
        void    setReverbVolume      (float     fVolume);
        void    setEchoVolume        (float     fEchoVolume);
#if _WIN32
        void    loadVSTPlugin        (wchar_t*  pPathToDll);
        void    unloadVSTPlugin      ();
#endif
        void    systemUpdate         ();


    // Set

        void    addTracks            (std::vector<std::wstring>  paths);
        void    setVolume            (float                  fNewVolume);
        void    setTrackPos          (unsigned int           graphPos);
        void    setRepeatPoint       (unsigned int graphPos);


    // Get

        std::string   getBloodyVersion     ();
        size_t        getPlayingTrackIndex (bool& bSomeTrackIsPlaying);


    // For testing

        FMOD::System* getFMODSystem        ();
        Track*        getCurrentTrack      ();
        size_t        getTracksCount       ();
        bool          isFMODStarted        ();
        bool          isSomeTrackIsPlaying ();
        bool          isCurrentTrackPaused ();




    ~AudioService();

private:

    // First and most important function of our system.
    // If this function fails, the application will terminate.
        bool   FMODinit        ();

    // Functions for execution in a separete thread
        void   threadAddTracks (std::vector<std::wstring> paths, size_t iStart, size_t iStop, bool* done,  int* allCount,  int all);
        bool   addTrack        (const std::wstring& sFilePath);
        std::wstring getTrackName  (const std::wstring& sFilePath);

    // Will switch to next track if one's ended
        void   monitorTrack    ();
        bool   isCurrentTrackEnded();
        void   switchToOtherTrack();

    // Will draw the oscillogram for the current track
        void   drawGraph       (size_t* iTrackIndex);

    // Used in drawGraph()
        float* rawBytesToPCM16_0_1   (char* pBuffer, unsigned int iBufferSizeInBytes);
        float* rawBytesToPCM24_0_1   (char* pBuffer, unsigned int iBufferSizeInBytes);
        int    interpret24bitAsInt32 (char byte0, char byte1, char byte2);

    // Used in search()
        size_t findCaseInsensitive(std::wstring& sText, std::wstring& sKeyword);

    // Used in addTracks()
        void   calcBitrate     ();








    FMOD::System*     pSystem;
    MainWindow*       pMainWindow;
    std::mt19937_64*  pRndGen;


    // FX
    FMOD::DSP*        pPitch;
    FMOD::DSP*        pPitchForTime;
    FMOD::DSP*        pReverb;
    FMOD::DSP*        pEcho;
    FMOD::DSP*        pVST;
    unsigned int      iVSTHandle;


    // Oscillogram  drawing
    std::mutex        mtxDrawGraph;
    std::mutex        mtxGetCurrentDrawingIndex;
    size_t*           iCurrentlyDrawingTrackIndex;
    bool              bDrawing;


    // Search
    std::vector<size_t> vSearchResult;
    size_t              iCurrentPosInSearchVec;
    bool                bFirstSearchAfterKeyChange;


    // Tracks
    std::vector<Track*> vTracks;
    std::vector<Track*> vTracksHistory;


    // Repeat section
    char              cRepeatSectionState;
    unsigned int      iFirstRepeatTimePos;
    unsigned int      iSecondRepeatTimePos;



    std::mutex        mtxTracksVec;
    std::mutex        mtxThreadLoadAddTrack;
    std::mutex        mtxLoadThreadDone;


    std::string       sBloodyVersion;


    size_t            iCurrentlyPlayingTrackIndex;


    float             fCurrentVolume;
    float             fCurrentSpeedByPitch;
    float             fCurrentSpeedByTime;


    bool              bIsSomeTrackPlaying;
    bool              bCurrentTrackPaused;
    bool              bRepeatTrack;
    bool              bRandomNextTrack;
    bool              bMonitorTracks;
    bool              bFMODStarted;
};
