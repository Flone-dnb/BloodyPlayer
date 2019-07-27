#pragma once

#include <vector>
#include <string>

class MainWindow;
class AudioService;

class Controller
{
public:

    Controller(MainWindow* pMainWindow);

    void addTrack(const wchar_t* pFilePath);

    void addTracks(std::vector<wchar_t*> paths);

    void playTrack(size_t iTrackIndex);
    void pauseTrack();
    void stopTrack();
    void nextTrack();
    void prevTrack();

    void changeVolume(float fNewVolume);

    void removeTrack(size_t iTrackIndex);

    size_t getPlaingTrackIndex(bool& bSomeTrackIsPlaying);

    ~Controller();

private:

    AudioService* pAudioService;
};
