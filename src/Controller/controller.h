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
    void setTrackPos  (unsigned int graphPos);
    void stopTrack    ();
    void nextTrack    ();
    void prevTrack    ();
    void removeTrack  (size_t iTrackIndex);
    void moveDown     (size_t iTrackIndex);
    void moveUp       (size_t iTrackIndex);

    // FX
    void setPan          (float fPan);
    void setPitch        (float fPitch);
    void setSpeedByPitch (float fSpeed);
    void setSpeedByTime  (float fSpeed);
    void setReverbVolume (float fVolume);
    void setEchoVolume   (float fEchoVolume);
    void loadVSTPlugin   (wchar_t *pPathToDll);
    void unloadVSTPlugin ();
    void systemUpdate    ();

    // Buttons under the volume slider
    void repeatTrack    ();
    void randomNextTrack();
    void clearPlaylist  ();

    // set
    void changeVolume (float fNewVolume);

    // get
    size_t getPlaingTrackIndex (bool& bSomeTrackIsPlaying);



    ~Controller();

private:

    AudioService* pAudioService;

    bool bRepeatTrack;
    bool bRandomTrack;
};
