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



    // Tracklist functions

            // Adds tracks by using private 'threadAddTracks()' and 'addTrack()' functions.
            void addTracks(std::vector<wchar_t*> paths);

            // Starts track playback.
            void playTrack(size_t iTrackIndex, bool bCallFromMonitor = false);

            // Pauses / unpauses the track.
            void pauseTrack();

            // Stops track if one is playing.
            void stopTrack();

            // Switch to the next track in the 'tracks' vector.
            void nextTrack(bool bCallFromMonitor = false, bool bRandomNextTrack = false);

            // Switch to the previous track in the 'tracks' vector.
            void prevTrack();

            // Removes the track from the 'tracks' vector.
            void removeTrack(size_t iTrackIndex);

            // Moves the current track on one position to the right in the 'tracks' vector.
            void moveDown(size_t iTrackIndex);

            // Moves the current track on one position to the left in the 'tracks' vector
            void moveUp(size_t iTrackIndex);

    // Buttons under the volume slider

            // monitorTrack() will repeat current track and not play next.
            void repeatTrack();

            // monitorTrack() will play random next track.
            void randomNextTrack();

            // Removes all tracks.
            void clearPlaylist();

    // set
            void changeVolume(float fNewVolume);

    // get
             size_t getPlayingTrackIndex(bool& bSomeTrackIsPlaying);



    ~AudioService();

private:

    // First and most important function of our system.
    // If this function fails, the application will terminate.
    void FMODinit();

    // Functions for execution as a separete thread
    void threadAddTracks(std::vector<wchar_t*> paths, bool* done, int* allCount, int all);
    void addTrack(const wchar_t* pFilePath);

    // Will switch to next track if one's ended
    void monitorTrack();



    FMOD::System* pSystem;
    MainWindow*   pMainWindow;
    std::mt19937_64* pRndGen;

    std::vector<Track*> tracks;

    bool bIsSomeTrackPlaying;
    bool bRepeatTrack;
    bool bRandomNextTrack;
    bool bMonitorTracks;

    float  fCurrentVolume;
    size_t iCurrentlyPlayingTrackIndex;

    std::mutex mtxTracksVec;
    std::mutex mtxThreadLoadAddTrack;
    std::mutex mtxThreadDone;
};
