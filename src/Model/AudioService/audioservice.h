#pragma once



// STL
#include <vector>
#include <mutex>
#include <random>

// FMOD
#include "../ext/FMOD/inc/fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")






class MainWindow;
class Track;






class AudioService
{

public:

    AudioService(MainWindow* pMainWindow);





    // Main functions

        void   playTrack             (size_t iTrackIndex,           bool bDontLockMutex = false);
        void   nextTrack             (bool bDontLockMutex = false,  bool bRandomNextTrack = false);
        void   pauseTrack            ();
        void   stopTrack             ();
        void   prevTrack             ();


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


    // FX

        void    setPan               (float     fPan);
        void    setPitch             (float     fPitch);
        void    setSpeedByPitch      (float     fSpeed);
        void    setSpeedByTime       (float     fSpeed);
        void    setReverbVolume      (float     fVolume);
        void    setEchoVolume        (float     fEchoVolume);
        void    loadVSTPlugin        (wchar_t*  pPathToDll);
        void    unloadVSTPlugin      ();
        void    systemUpdate         ();


    // Set

        void    addTracks            (std::vector<wchar_t*>  paths);
        void    setVolume            (float                  fNewVolume);
        void    setTrackPos          (unsigned int           graphPos);


    // Get

        size_t  getPlayingTrackIndex (bool& bSomeTrackIsPlaying);





    ~AudioService();

private:

    // First and most important function of our system.
    // If this function fails, the application will terminate.
    void   FMODinit        ();

    // Functions for execution in a separete thread
    void   threadAddTracks (std::vector<wchar_t*> paths,  bool* done,  int* allCount,  int all);
    void   addTrack        (const wchar_t* pFilePath);

    // Will switch to next track if one's ended
    void   monitorTrack    ();

    // Will draw the oscillogram for the current track
    void   drawGraph       (size_t* iTrackIndex);

    // Used in drawGraph()
        float* rawBytesToPCM16_0_1 (char* pBuffer, unsigned int iBufferSizeInBytes);
        float* rawBytesToPCM24_0_1 (char* pBuffer, unsigned int iBufferSizeInBytes);
        int interpret24bitAsInt32(char byte0, char byte1, char byte2);








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


    size_t            iCurrentlyPlayingTrackIndex;


    float             fCurrentVolume;
    float             fCurrentSpeedByPitch;
    float             fCurrentSpeedByTime;


    bool              bIsSomeTrackPlaying;
    bool              bCurrentTrackPaused;
    bool              bRepeatTrack;
    bool              bRandomNextTrack;
    bool              bMonitorTracks;


    std::mutex        mtxTracksVec;
    std::mutex        mtxThreadLoadAddTrack;
    std::mutex        mtxLoadThreadDone;


    std::vector<Track*> tracks;
};
