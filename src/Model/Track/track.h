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

    Track(const wchar_t* pFilePath, MainWindow* pMainWindow, FMOD::System* pSystem);





    // Start/stop functions

        bool           setupTrack             ();
        bool           playTrack              (float fVolume);
        bool           pauseTrack             ();
        bool           stopTrack              ();
        bool           reCreateTrack          (float fVolume);


    // 'FX' functions

        void           setSpeedByFreq         (float fSpeed);
        void           setSpeedByTime         (float fSpeed);


    // 'Dummy Sound' functions (related to oscillogram drawing)

        bool           createDummySound       ();
        bool           releaseDummySound      ();


    // 'Set' functions

        bool           setPositionInMS        (unsigned int  iPos);
        bool           setVolume              (float         fNewVolume);
        void           setMaxPosInGraph       (unsigned int  iMax);


    // 'Get' functions

        // Format

        std::string    getPCMFormat           ();
        std::string    getFormat              ();


        // Time

        std::string    getCurrentTime         ();
        unsigned int   getLengthInMS          ();
        unsigned int   getPositionInMS        ();
        unsigned int   getPositionInPCMBytes  ();


        // Size

        long long      getFileSizeInBytes     ();
        unsigned int   getLengthInPCMbytes    ();
        unsigned int   getMaxValueOnGraph     ();


        // Audio params

        float          getFrequency           ();
        bool           getChannelsAndBits     (int* channels,     int* bits);
        bool           getBitRate             (int* bitrate);


        // State

        bool           getPaused              ();
        bool           getPlaying             ();


        // Other

        char           getPCMSamples          (char* pBuff,  unsigned int amountInBytes,  unsigned int *pActualRead, char* pPcmFormat);
        const wchar_t* getFilePath            ();





    ~Track();

private:

    // Funtions that used by getBitRate() function

        int tellBitRate      (bool bit1,  bool bit2,  bool bit3,  bool bit4);
        int tellSamplingRate (bool bit1,  bool bit2);






    MainWindow*    pMainWindow;


    // FMOD stuff
    FMOD::Sound*   pSound;
    FMOD::Sound*   pDummySound;
    FMOD::Channel* pChannel;
    FMOD::System*  pSystem;


    std::string    format;
    std::string    pcmFormat;


    const wchar_t* pFilePath;


    unsigned int   iMaxValueOnGraph;


    float          fDefaultFrequency;
    float          fSpeedByFreq;
    float          fSpeedByTime;


    bool           bPaused;
};
