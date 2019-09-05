#pragma once

// STL
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



    // Start/stop functions
    // This functions return 'false' on fail.

            bool setTrack   (const wchar_t* pFilePath);
            bool playTrack  (float fVolume);
            bool pauseTrack ();
            bool stopTrack  ();
            bool reCreateTrack(float fVolume);

    // 'Set' functions
    // This functions return 'false' on fail.

            bool setPositionInMS (unsigned int iPos);
            bool setVolume       (float fNewVolume);
            bool setPosForDummy  (unsigned int pcm);
            void setMaxPosInGraph(unsigned int iMax);
            void setSpeedByFreq(float fSpeed);
            void setSpeedByTime(float fSpeed);


    // 'Get' functions

            std::string  getPCMFormat       ();
            std::string  getFormat          ();
            std::string  getCurrentTime     ();
            unsigned int getLengthInMS      ();
            unsigned int getLengthInPCMbytes();
            unsigned int getPositionInMS    ();
            unsigned int getPositionInPCMBytes();
            float        getFrequency       ();
            bool         getChannelsAndBits (int* channels, int* bits);
            bool         getBitRate         (int* bitrate);
            long long    getSize            ();
            bool         getPaused          ();
            bool         getPlaying         ();
            bool         createDummySound   ();
            char         getPCMSamples (short int* pBuff, unsigned int lengthInBytes, unsigned int *pActualRead);
            bool         releaseDummySound  ();
            const wchar_t* getFilePath();
            unsigned int getMaxValueOnGraph();



    ~Track();

private:

    // Funtions that used by getBitRate() function
        int tellBitRate      (bool bit1, bool bit2, bool bit3, bool bit4);
        int tellSamplingRate (bool bit1, bool bit2);


    MainWindow* pMainWindow;

    // FMOD stuff
    FMOD::Sound*   pSound;
    FMOD::Sound*   pDummySound;
    FMOD::Channel* pChannel;
    FMOD::System*  pSystem;

    const wchar_t* pFilePath;

    std::string    format;
    std::string    pcmFormat;

    unsigned int iMaxValueOnGraph;

    float fDefaultFrequency;
    float fSpeedByFreq;
    float fSpeedByTime;

    bool           bPaused;
};
