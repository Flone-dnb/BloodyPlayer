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

            // Creates a track (pSound) in the FMOD system.
            bool setTrack   (const wchar_t* pFilePath);

            // Starts track playback under various conditions, for example, no track is created, track is stopped, or ended.
            bool playTrack  (float fVolume);

            // Pauses / unpauses the track.
            bool pauseTrack ();

            // Pauses the track and sets its position at the beginning.
            bool stopTrack  ();

            // If track is ended then we need to recreate it because we can't use pChannel.
            bool reCreateTrack(float fVolume);

    // 'Set' functions
    // This functions return 'false' on fail.

            // Sets the track position in milliseconds.
            bool setPositionInMS (unsigned int iPos);

            // Sets track volume.
            bool setVolume       (float fNewVolume);


    // 'Get' functions

            // Returns private 'std::string format' variable. It contains the audio file format like mp3, ogg, wav, flac.
            std::string  getFormat          ();

            // Returns the length of the track.
            unsigned int getLengthInMS      ();

            // Returns the current position of the track in milliseconds.
            unsigned int getPositionInMS    ();

            // Returns the sampling rate of the track.
            float        getFrequency       ();

            // Returns the amount of channels and quantization bit depth of the track.
            bool         getChannelsAndBits (int* channels, int* bits);

            // Returns tracks bitrate.
            bool         getBitRate         (int* bitrate);

            // Returns tracks file size in bytes.
            long long    getSize            ();

            // Returns private 'bool bPaused' variable - 'true' if track is paused.
            bool         getPaused          ();

            // Returns 'true' if the track is plaing right now.
            bool         getPlaying         ();



    ~Track();

private:

    // Funtions that used by getBitRate() function
        int tellBitRate      (bool bit1, bool bit2, bool bit3, bool bit4);
        int tellSamplingRate (bool bit1, bool bit2);


    MainWindow* pMainWindow;

    // FMOD stuff
    FMOD::Sound*   pSound;
    FMOD::Channel* pChannel;
    FMOD::System*  pSystem;

    const wchar_t* pFilePath;

    std::string    format;

    bool           bPaused;
};
