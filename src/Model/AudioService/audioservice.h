#pragma once

// STL
#include <vector>
#include <mutex>


// FMOD
#include "../ext/FMOD/inc/fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")


class MainWindow;
class Track;



class AudioService
{
public:

    AudioService(MainWindow* pMainWindow);

    void addTrack(const wchar_t* pFilePath);
    void addTracks(std::vector<wchar_t*> paths);
    void playTrack(size_t iTrackIndex);
    void pauseTrack();
    void stopTrack();
    void nextTrack();
    void prevTrack();
    void removeTrack(size_t iTrackIndex);

    void changeVolume(float fNewVolume);

    size_t getPlayingTrackIndex(bool& bSomeTrackIsPlaying);

    ~AudioService();

private:

    void threadAddTracks(std::vector<wchar_t*> paths, bool* done, int* allCount, int all);

    void monitorTrack();



    FMOD::System* pSystem;

    MainWindow* pMainWindow;
    bool bSystemReady;

    std::vector<Track*> tracks;

    bool bIsSomeTrackPlaying;
    bool bTrackIsPaused;

    float fCurrentVolume;
    size_t iCurrentlyPlayingTrackIndex;

    std::mutex mtxAddTrack;
    std::mutex mtxThreadDone;
};
