#pragma once

#include <string>


class MainWindow;
namespace FMOD
{
    class System;
    class Sound;
    class Channel;
}



class Track
{
public:

    Track(MainWindow* pMainWindow, FMOD::System* pSystem);

    // start/stop
    bool setTrack(const wchar_t* pFilePath);
    bool playTrack(float fVolume);
    bool pauseTrack();
    bool stopTrack();

    // set
    bool setPositionInMS(unsigned int iPos);
    bool setVolume(float fNewVolume);

    // get
    unsigned int getLengthInMS();
    unsigned int getPositionInMS();
    std::string getFormat();
    bool getChannelsAndBits(int* channels, int* bits);
    size_t getTimerPlayed();
    bool getBitRate(int* bitrate);
    bool getPaused();
    float getFrequency();
    bool isPlaying();

    ~Track();

private:

    int tellBitRate(bool bit1, bool bit2, bool bit3, bool bit4);

    int tellSamplingRate(bool bit1, bool bit2);


    MainWindow* pMainWindow;

    FMOD::Sound*   pSound;
    FMOD::Channel* pChannel;
    FMOD::System*  pSystem;

    const wchar_t* pFilePath;
    size_t iTrackPlayedTimes;
    bool bPaused;
    std::string format;
};
