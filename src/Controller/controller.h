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



    // Tracklist functions
    void addTracks    (std::vector<wchar_t*> paths);
    void playTrack    (size_t iTrackIndex);
    void pauseTrack   ();
    void stopTrack    ();
    void nextTrack    ();
    void prevTrack    ();
    void removeTrack  (size_t iTrackIndex);

    // set
    void changeVolume (float fNewVolume);

    // get
    size_t getPlaingTrackIndex (bool& bSomeTrackIsPlaying);



    ~Controller();

private:

    AudioService* pAudioService;
};
