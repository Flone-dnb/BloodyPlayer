// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#pragma once




//STL
#include <vector>
#include <string>





class MainWindow;
class AudioService;





class Controller
{

public:

    Controller(MainWindow* pMainWindow);


    // Tutorial

        void   doNotShowTutorialAgain();
        void   tutorialEnd       ();


    // Main buttons

        void    playTrack        (size_t iTrackIndex);
        void    pauseTrack       ();
        void    stopTrack        ();
        void    nextTrack        ();
        void    prevTrack        ();


    // Buttons under the volume slider

        void    repeatTrack      ();
        void    randomNextTrack  ();
        void    clearPlaylist    ();


    // Tracklist management

        void    removeTrack      (size_t iTrackIndex);
        void    moveDown         (size_t iTrackIndex);
        void    moveUp           (size_t iTrackIndex);


    // Tracklist functions

        void    openTracklist    (std::wstring pathToTracklist,  bool bClearCurrent);
        void    saveTracklist    (std::wstring pathToTracklist);


    // Search

        void    searchFindPrev   ();
        void    searchFindNext   ();
        void    searchTextSet    (const std::wstring& sKeyword);


    // FX

        void    setPan           (float    fPan);
        void    setPitch         (float    fPitch);
        void    setSpeedByPitch  (float    fSpeed);
        void    setSpeedByTime   (float    fSpeed);
        void    setReverbVolume  (float    fVolume);
        void    setEchoVolume    (float    fEchoVolume);
        void    loadVSTPlugin    (wchar_t* pPathToDll);
        void    unloadVSTPlugin  ();
        void    systemUpdate     ();


    // Set

        void    addTracks        (std::vector<wchar_t*> paths);
        void    setVolume        (float fNewVolume);
        void    setTrackPos      (unsigned int graphPos);
        void    setRepeatPoint   (unsigned int graphPos);


    // Get

        std::string  getBloodyVersion     ();
        size_t       getPlaingTrackIndex  (bool& bSomeTrackIsPlaying);




    ~Controller();

private:

    AudioService* pAudioService;


    bool bRepeatTrack;
    bool bRandomTrack;
};
