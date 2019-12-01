#include "track.h"

// STL
#include <fstream>
#include <vector>
#include <codecvt>

// Custom
#include "../src/View/MainWindow/mainwindow.h"
#include "../ext/FMOD/inc/fmod.hpp"
#include "../ext/FMOD/inc/fmod_errors.h"
#include "../src/globalparams.h"

// Other
#include <windows.h>

Track::Track(const wchar_t* pFilePath, const std::wstring& sTrackName, MainWindow *pMainWindow, FMOD::System* pSystem)
{
    pChannel          = nullptr;
    pSound            = nullptr;
    this->pFilePath   = pFilePath;
    this->sTrackName  = sTrackName;

    this->pMainWindow = pMainWindow;
    this->pSystem     = pSystem;

    iMaxValueOnGraph  = 0;

    bPaused           = false;
    bBitrateCalculated= false;

    fSpeedByFreq = 1.0f;
    fSpeedByTime = 1.0f;
}






bool Track::setupTrack()
{
    // This function creates a track (pSound) in the FMOD system.

    // wchar_t is 16 bits and holds UTF-16 code units
    // FMOD accepts UTF-8 strings
    // convert wchar_t* (UTF-16) to char* (UTF-8)
    char filePathInUTF8[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, pFilePath, -1, filePathInUTF8, sizeof(filePathInUTF8), nullptr, nullptr);


    FMOD_RESULT result;

    result = pSystem->createStream(filePathInUTF8, FMOD_DEFAULT | FMOD_LOOP_OFF | FMOD_ACCURATETIME, nullptr, &pSound);
    if (result)
    {
        pMainWindow->showWMessageBox( true, std::wstring(L"Track::setupTrack::FMOD::System::createStream() failed.\n\n"
                                                       "Can't load the file \"" + std::wstring(pFilePath) + L"\".\n\n"
                                                       "Error: ") + stringToWString(std::string(FMOD_ErrorString(result))) );
        return false;
    }


    // Get audio format (mp3, wav, flac, ogg and etc.)
    FMOD_SOUND_TYPE type;
    FMOD_SOUND_FORMAT formatType;
    result = pSound->getFormat(&type, &formatType, nullptr, nullptr);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::setupTrack::FMOD::Sound::getFormat() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }


    // Save format to 'std::string' and not 'FMOD_SOUND_TYPE' because we will use it later
    // and can't save 'FMOD_SOUND_TYPE' in .h file (it does not include FMOD).
    if      (type == FMOD_SOUND_TYPE_FLAC)      format = "FLAC";
    else if (type == FMOD_SOUND_TYPE_MPEG)      format = "MP3";
    else if (type == FMOD_SOUND_TYPE_WAV)       format = "WAV";
    else if (type == FMOD_SOUND_TYPE_OGGVORBIS) format = "OGG";

    if      (formatType == FMOD_SOUND_FORMAT_PCM8)  pcmFormat = "PCM8";
    else if (formatType == FMOD_SOUND_FORMAT_PCM16) pcmFormat = "PCM16";
    else if (formatType == FMOD_SOUND_FORMAT_PCM24) pcmFormat = "PCM24";
    else                                            pcmFormat = "NULL";

    return true;
}

bool Track::getPlaying()
{
    // This function returns 'true' if the track is plaing right now.

    if (pChannel == nullptr)
    {
        // If we got here then it means that 'playTrack()' function was not called.
        return false;
    }

    FMOD_RESULT result;
    bool bPlaying;

    result = pChannel->isPlaying(&bPlaying);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::isPlaying::FMOD::Channel::isPlaying() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }

    return bPlaying;
}

bool Track::createDummySound()
{
    // wchar_t is 16 bits and holds UTF-16 code units
    // FMOD accepts UTF-8 strings
    // convert wchar_t* (UTF-16) to char* (UTF-8)
    char filePathInUTF8[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, pFilePath, -1, filePathInUTF8, sizeof(filePathInUTF8), nullptr, nullptr);


    FMOD_RESULT result;

    result = pSystem->createStream(filePathInUTF8, FMOD_DEFAULT, nullptr, &pDummySound);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::createDummySound::FMOD::System::createStream() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }

    return true;
}

char Track::getPCMSamples(char *pBuff, unsigned int amountInBytes, unsigned int *pActualRead, char* pPcmFormat)
{
    // return value:
    // -1 == end of file
    // 0  == error
    // 1  == ok


    if (pcmFormat == "PCM16")
    {
        *pPcmFormat = 16;
    }
    else if (pcmFormat == "PCM24")
    {
        *pPcmFormat = 24;
    }
    else
    {
        pMainWindow->showMessageBox(true, "Track::getPCMSamples() error. Unsupported PCM format. "
                                          "This version of Bloody Player supports only 16 bit and 24 bit audio. This is not a critical error.");
        return 0;
    }




    FMOD_RESULT result;

    if (pcmFormat == "PCM16")
    {
        if (amountInBytes % 4 != 0)
        {
            while (amountInBytes % 4 != 0)
            {
                amountInBytes--;
            }
        }
    }
    else if (pcmFormat == "PCM24")
    {
        if (amountInBytes % 6 != 0)
        {
            while (amountInBytes % 6 != 0)
            {
                amountInBytes--;
            }
        }
    }




    result = pDummySound->readData(pBuff, amountInBytes, pActualRead);

    if ( (result) && (result != FMOD_ERR_FILE_EOF))
    {
        pMainWindow->showMessageBox( true, std::string("Track::getAudioData::FMOD::Sound::readData() failed. Error: ") + std::string(FMOD_ErrorString(result)) );

        return 0;
    }
    else
    {
        if (result == FMOD_ERR_FILE_EOF)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
}

bool Track::releaseDummySound()
{
    FMOD_RESULT result;
    result = pDummySound->release();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::releaseDummySound::FMOD::Sound::release() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }

    return true;
}

const wchar_t* Track::getFilePath()
{
    return pFilePath;
}

std::wstring &Track::getTrackName()
{
    return sTrackName;
}

unsigned int Track::getMaxValueOnGraph()
{
    return iMaxValueOnGraph;
}

bool Track::playTrack(float fVolume)
{
    // This function starts track playback under various conditions, for example, no track is created, track is stopped, or ended.

    if (pChannel == nullptr)
    {
        // If we got here then it's our first time calling this function (playTrack()).

        FMOD_RESULT result;

        result = pSystem->playSound(pSound, nullptr, true, &pChannel);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::System::playSound() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        float fFrequency;
        result = pChannel->getFrequency(&fFrequency);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::getFrequency() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }

        fDefaultFrequency = fFrequency;

        result = pChannel->setVolume(fVolume);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setVolume() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        if ( (fSpeedByFreq == 1.0f) && (fSpeedByTime == 1.0f) )
        {
            pChannel->setFrequency( fDefaultFrequency );
        }
        else
        {
            if (fSpeedByFreq != 1.0f) pChannel->setFrequency( fDefaultFrequency * fSpeedByFreq );
            else                      pChannel->setFrequency( fDefaultFrequency * fSpeedByTime );
        }

        result = pChannel->setPaused(false);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        bPaused = false;

        return true;
    }
    else
    {
        // If we got here then it's not our first time calling this function (playTrack()).

        FMOD_RESULT result;

        result = pChannel->setVolume(fVolume);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setVolume() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }


        // Check if it's actually playing
        bool bIsPlaying;
        result = pChannel->isPlaying(&bIsPlaying);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        if (bPaused)
        {
            pauseTrack();
        }
        else if (bIsPlaying)
        {
            // The track is plaing and user pressed Play so now we just need to start track from the beginning.
            setPositionInMS(0);
        }

        if ( (fSpeedByFreq == 1.0f) && (fSpeedByTime == 1.0f) )
        {
            pChannel->setFrequency( fDefaultFrequency );
        }
        else
        {
            if (fSpeedByFreq != 1.0f) pChannel->setFrequency( fDefaultFrequency * fSpeedByFreq );
            else                      pChannel->setFrequency( fDefaultFrequency * fSpeedByTime );
        }

        result = pChannel->setPaused(false);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        bPaused = false;

        return true;
    }
}

bool Track::pauseTrack()
{
    // This function pauses / unpauses the track.

    if (pChannel != nullptr)
    {
        // If we got here then it means that 'playTrack()' function was called.

        FMOD_RESULT result;

        bool bPausedTrack;
        result = pChannel->getPaused(&bPausedTrack);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::pauseTrack::FMOD::Channel::getPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        // Unpause the track is it's paused.
        // Pause the track is it's unpaused.
        if (bPausedTrack)
        {
            result = pChannel->setPaused(false);
            if (result)
            {
                pMainWindow->showMessageBox( true, std::string("Track::pauseTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                return false;
            }

            bPaused = false;
        }
        else
        {
            result = pChannel->setPaused(true);
            if (result)
            {
                pMainWindow->showMessageBox( true, std::string("Track::pauseTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                return false;
            }

            bPaused = true;
        }

        return true;
    }

    return false;
}

bool Track::stopTrack()
{
    // This function pauses the track and sets its position at the beginning.

    if (pChannel != nullptr)
    {
        // If we got here then it means that 'playTrack()' function was called.

        FMOD_RESULT result;

        result = pChannel->setPaused(true);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::stopTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        result = pChannel->setPosition(0, FMOD_TIMEUNIT_MS);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::stopTrack::FMOD::Channel::setPosition() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        bPaused = true;

        return true;
    }

    return false;
}

bool Track::reCreateTrack(float fVolume)
{
    FMOD_RESULT result;

    if (pChannel != nullptr)
    {
        result = pChannel->stop();
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::reCreateTrack::FMOD::Channel::stop() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }

        result = pSystem->playSound(pSound, nullptr, true, &pChannel);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::reCreateTrack::FMOD::System::playSound() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        result = pChannel->setVolume(fVolume);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::reCreateTrack::FMOD::Channel::setVolume() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool Track::setPositionInMS(unsigned int iPos)
{
    // This function sets the track position in milliseconds.

    if (pChannel != nullptr)
    {
        // If we got here then it means that 'playTrack()' function was called.

        FMOD_RESULT result;

        result = pChannel->setPosition(iPos, FMOD_TIMEUNIT_MS);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::setPositionInMS::FMOD::Channel::setPosition() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        return true;
    }

    return false;
}

bool Track::setVolume(float fNewVolume)
{
    // This function sets track volume.

    if (pChannel != nullptr)
    {
        // If we got here then it means that 'playTrack()' function was called.

        FMOD_RESULT result;

        result = pChannel->setVolume(fNewVolume);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::setVolume::FMOD::Channel::setVolume() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        return true;
    }

    return false;
}

void Track::setMaxPosInGraph(unsigned int iMax)
{
    iMaxValueOnGraph = iMax;
}

void Track::setSpeedByFreq(float fSpeed)
{
    // Save the value even if pChannel is not created
    fSpeedByFreq = fSpeed;

    if (pChannel != nullptr)
    {
        if (fSpeedByFreq == 1.0f)
        {
            pChannel->setFrequency( fDefaultFrequency );
        }
        else
        {
            pChannel->setFrequency( fDefaultFrequency * fSpeedByFreq );
        }
    }
}

void Track::setSpeedByTime(float fSpeed)
{
    // Save the value even if pChannel is not created
    fSpeedByTime = fSpeed;

    if (pChannel != nullptr)
    {
        if (fSpeedByTime == 1.0f)
        {
            pChannel->setFrequency( fDefaultFrequency );
        }
        else
        {
            pChannel->setFrequency( fDefaultFrequency * fSpeedByTime );
        }
    }
}

std::string Track::getPCMFormat()
{
    return pcmFormat;
}

unsigned int Track::getLengthInMS()
{
    // This function returns the length of the track.

    if (pSound != nullptr)
    {
        FMOD_RESULT result;
        unsigned int iLength;

        result = pSound->getLength(&iLength, FMOD_TIMEUNIT_MS);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::getLengthInMS::FMOD::Sound::getLength() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return 0;
        }

        return iLength;
    }
    else
    {
        return 0;
    }
}

unsigned int Track::getLengthInPCMbytes()
{
    FMOD_RESULT result;
    unsigned int iLength;
    result = pSound->getLength(&iLength, FMOD_TIMEUNIT_PCMBYTES);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::getLengthInPCMBytes::FMOD::Sound::getLength() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return 0;
    }
    else
    {
        return iLength;
    }
}

unsigned int Track::getPositionInMS()
{
    // This function returns the current position of the track in milliseconds.

    if (pChannel != nullptr)
    {
        FMOD_RESULT result;

        unsigned int pos = 0;
        result = pChannel->getPosition(&pos, FMOD_TIMEUNIT_MS);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::getPositionInMS::FMOD::Channel::getPosition() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return 0;
        }

        return pos;
    }

    return 0;
}

unsigned int Track::getPositionInPCMBytes()
{
    // This function returns the current position of the track in milliseconds.

    if (pChannel != nullptr)
    {
        FMOD_RESULT result;

        unsigned int pos = 0;
        result = pChannel->getPosition(&pos, FMOD_TIMEUNIT_PCMBYTES);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::getPositionInPCMBytes::FMOD::Channel::getPosition() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return 0;
        }

        return pos;
    }

    return 0;
}

std::string Track::getCurrentTime()
{
    unsigned int iMS = getPositionInMS();
    if (iMS == 0) return "";

    unsigned int iSeconds = iMS / 1000;
    unsigned int iMinutes = iSeconds / 60;
    iSeconds -= (iMinutes * 60);
    std::string trackTime = "";
    trackTime += std::to_string(iMinutes);
    trackTime += ":";
    if (iSeconds < 10) trackTime += "0";
    trackTime += std::to_string(iSeconds);

    return trackTime;
}

std::string Track::getFormat()
{
    // This function returns private 'std::string format' variable.
    // It contains the audio file format like mp3, ogg, wav, flac.

    return format;
}

bool Track::getChannelsAndBits(int* channels, int* bits)
{
    // This function returns the amount of channels and quantization bit depth of the track

    if (pSound != nullptr)
    {
        FMOD_RESULT result;

        int trackChannels;
        int trackBits;

        result = pSound->getFormat(nullptr, nullptr, &trackChannels, &trackBits);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::getChannelsAndBits::FMOD::Sound::getFormat() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        *channels = trackChannels;
        *bits     = trackBits;

        return true;
    }

    return false;
}

bool Track::getBitRate(int *bitrate)
{
    // This function returns tracks bitrate.

    std::vector<int> framesBitrates;

    // Some of the code below is from my other program so don't really pay attension to some of the comments


    // Open selected file in binary mode
    std::ifstream mp3File (pFilePath, std::ios::binary);

    if (mp3File.is_open())
    {
        // Will be 'true' if we passed the first mp3 frame
        // We skip it because it is usually contains some VBR or CBR info
        // I don't wanna touch it for now, maybe later
        bool bFirstMP3FramePassed = false;

        // Get file size
        int endPos = 0;
        mp3File.seekg(0,std::ios::end);
        // tellg() returns 'long long' value so we can't put it in endPos (int) yet
        long long tempSize = mp3File.tellg();

        // File is more than 256 MB?
        if (tempSize > 268435450)
        {
            // We close file
            // Because 'amountOfBits' can hold 256 MB max (2147483648 numbers - in our case it's bits)
            // But will it ever contain so much???
            mp3File.close();
            pMainWindow->showMessageBox(true, "File is larger than 256 MB, can't determine the bitrate.");
            return false;
        }

        // Now we know that the size of the selected file is less than 256 MB and we can safely put it to 'endPos'
        endPos = static_cast<int>(tempSize);

        // Set position to the beginning of this file
        mp3File.seekg(0,std::ios::beg);

        // 'pos' - current read position
        int pos = 0;
        // 'byte' will contain bytes that we read
        unsigned char byte = 0;

        // Find the size of metadata
        char metaBuffer[4];
        memset(metaBuffer, 0, 4);

        mp3File.read(metaBuffer, 3);

        if (std::string(metaBuffer) == "ID3")
        {
            // Skip to tag size
            mp3File.seekg(3, std::ios::cur);
            mp3File.read(metaBuffer, 4);

            int iTagSize = 0;

            int iCurrentBit = 0;

            for (int i = 3; i >= 0; i--)
            {
                // last bit does not contain data
                // so we read 7 bits and not 8
                for (int j = 0; j < 7; j++)
                {
                    bool bit = (bool((1 << j)  &  metaBuffer[i]));

                    if (bit)
                    {
                        // write true
                        iTagSize |= (1 << iCurrentBit);
                    }
                    else
                    {
                        // write false
                        iTagSize &= ~(1 << iCurrentBit);
                    }

                    iCurrentBit++;
                }
            }

            mp3File.seekg(iTagSize, std::ios::cur);
        }
        else
        {
            mp3File.seekg(0, std::ios::beg);
        }



        do
        {
            mp3File.read(reinterpret_cast<char*>(&byte),sizeof(byte));
            pos++;

            // Every mp3 frame starts with 11 (from 0 to 10) 'true' bits
            if (byte != 255)
            {
                continue;
            }

            if ((static_cast<double>(pos)/endPos) * 100 > 99)
            {
                continue;
            }

            mp3File.read(reinterpret_cast<char*>(&byte),sizeof(byte));
            pos++;

            // Last bit (right bit) can be 0 and byte value will not be 251 but 250.
            // Last bit holds protection info
            if (byte == 251 || byte == 250)
            {
                // read 8, 9, 10th bit (starting from 0 bit) of mp3 frame
                bool bit1 = (bool((1 << 7)  &  byte));
                bool bit2 = (bool((1 << 6)  &  byte));
                bool bit3 = (bool((1 << 5)  &  byte));
                bool bit4 = false;

                if (bit1 && bit2 && bit3)
                {
                    // 11 first bits is set to 'true' seems like it's mp3 frame

                    // Check if format is MPEG-1
                    bit1 = (bool((1 << 4)  &  byte));
                    bit2 = (bool((1 << 3)  &  byte));
                    if (bit1 && bit2)
                    {
                        // MPEG-1

                        // Starting to read third byte of mp3 frame
                        mp3File.read(reinterpret_cast<char*>(&byte),sizeof(byte));
                        pos++;

                        bit1 = (bool((1 << 7)  &  byte));
                        bit2 = (bool((1 << 6)  &  byte));
                        bit3 = (bool((1 << 5)  &  byte));
                        bit4 = (bool((1 << 4)  &  byte));

                        int iBitRate = tellBitRate(bit1, bit2, bit3, bit4);

                        if ( iBitRate == -1 )
                        {
                            // bitrate code is 1111 or 0000 - this is not valid
                            continue;
                        }

                        framesBitrates.push_back(iBitRate);

                        // check if next 2 bits != 11
                        bit1 = (bool((1 << 3)  &  byte));
                        bit2 = (bool((1 << 2)  &  byte));

                        int iSamplingRate = tellSamplingRate(bit1,bit2);

                        if ( iSamplingRate == -1 )
                        {
                            // sampling rate index == 11 - this is not valid
                            continue;
                        }

                        if (bFirstMP3FramePassed == false)
                        {
                            bFirstMP3FramePassed = true;
                            // It will probably contain some VBR and LAME stuff which we do not want to change
                            continue;
                        }

                        // check if padding bit is set
                        bit1 = (bool((1 << 1)  &  byte));

                        // Calculate the size of this mp3 frame
                        // And we don't count padding byte here (we will do that later)
                        // Not 1024 but 1000!
                        iBitRate *= 1000;

                        // We need to floor() it
                        int iFrameSize = static_cast<int>(floor(144 * iBitRate / static_cast<double>(iSamplingRate)));

                        // Minus header size
                        iFrameSize -= 4;

                        // read last (4th) byte of mp3 frame header
                        mp3File.read(reinterpret_cast<char*>(&byte),sizeof(byte));
                        pos++;

                        // If padding bit is set
                        if (bit1 == true)
                        {
                            iFrameSize++;
                        }

                        // Jump right to the end of this mp3 frame - the beginning of a new mp3 frame
                        mp3File.seekg(iFrameSize,std::ios::cur);
                        pos += iFrameSize;
                    }
                    else
                    {
                        // Unsupported format
                        mp3File.close();

                        return false;
                    }
                }
            }
        }while(pos < endPos);

        mp3File.close();



        // On each index this vector will contain:
        // (on 0 index) bitrate number
        // (on 1 index) the number of times this bitrate was encountered in the file.
        // So it will look something like this:
        // [128, 4034] [224, 549] [256, 533] [320, 325].
        std::vector<std::vector<int>> bitrates;

        // Fill vector
        for (size_t i = 0; i < framesBitrates.size(); i++)
        {
            bool bExists = false;
            for (size_t j = 0; j < bitrates.size(); j++)
            {
                if (bitrates[j][0] == framesBitrates[i])
                {
                    bExists = true;

                    bitrates[j][1] += 1;

                    break;
                }
            }

            if (bExists == false)
            {
                bitrates.resize( bitrates.size() + 1 );
                bitrates[ bitrates.size() - 1 ].resize(2);

                bitrates[ bitrates.size() - 1 ][0] = framesBitrates[i];
                bitrates[ bitrates.size() - 1 ][1] = 0;
            }
        }

        // Find bitrate that was encountered most of the times.
        size_t iAverageBitrateIndex = 0;
        for (size_t i = 1; i < bitrates.size(); i++)
        {
            if (bitrates[i][1] > bitrates[iAverageBitrateIndex][1])
            {
                iAverageBitrateIndex = i;
            }
        }

        *bitrate = bitrates[iAverageBitrateIndex][0];

        bBitrateCalculated = true;

        return true;
    }
    else
    {
        // Can't open file
        return false;
    }
}

bool Track::isBitrateCalculated()
{
    return bBitrateCalculated;
}

long long Track::getFileSizeInBytes()
{
    // This function returns tracks file size in bytes.

    // Open selected file in binary mode
    std::ifstream mp3File (pFilePath, std::ios::binary);

    if (mp3File.is_open())
    {
        // Get file size
        long long endPos = 0;
        mp3File.seekg(0, std::ios::end);
        // tellg() returns 'long long' value so we can't put it in endPos (int) yet
        endPos = mp3File.tellg();

        mp3File.close();

        return endPos;
    }
    else
    {
        // Can't open file
        return 0;
    }
}

bool Track::getPaused()
{
    // This function returns private 'bool bPaused' variable - 'true' if track is paused.
    return bPaused;
}

float Track::getFrequency()
{
    // This function returns the sampling rate of the track.

    if (pSound != nullptr)
    {
        float freq = 0.0f;

        pSound->getDefaults(&freq, nullptr);
        return freq;
    }

    return 0.0f;
}

int Track::tellBitRate(bool bit1, bool bit2, bool bit3, bool bit4)
{
    // bit1 is first bit in third byte of mp3 header

    QString bitString = "";

    if (bit1) bitString+="1";
    else      bitString+="0";

    if (bit2) bitString+="1";
    else      bitString+="0";

    if (bit3) bitString+="1";
    else      bitString+="0";

    if (bit4) bitString+="1";
    else      bitString+="0";




         if (bitString == "0001") return 32;
    else if (bitString == "0010") return 40;
    else if (bitString == "0011") return 48;
    else if (bitString == "0100") return 56;
    else if (bitString == "0101") return 64;
    else if (bitString == "0110") return 80;
    else if (bitString == "0111") return 96;
    else if (bitString == "1000") return 112;
    else if (bitString == "1001") return 128;
    else if (bitString == "1010") return 160;
    else if (bitString == "1011") return 192;
    else if (bitString == "1100") return 224;
    else if (bitString == "1101") return 256;
    else if (bitString == "1110") return 320;

         return -1;
}

int Track::tellSamplingRate(bool bit1, bool bit2)
{
    // bit1 is bit right after last bitrate bit

    QString bitString = "";

    if (bit1) bitString+="1";
    else      bitString+="0";

    if (bit2) bitString+="1";
    else      bitString+="0";


         if (bitString == "00") return 44100;
    else if (bitString == "01") return 48000;
    else if (bitString == "10") return 32000;

         return -1;
}

std::wstring Track::stringToWString(const std::string &str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.from_bytes(str);
}







Track::~Track()
{
    FMOD_RESULT result;

    if (pChannel != nullptr)
    {
        result = pChannel->stop();
        if (result)
        {
            // FMOD_ERR_INVALID_HANDLE can occur if the track is currently playing, it's very short and it's just ended
            // stop() will return FMOD_ERR_INVALID_HANDLE because after the sound is ended every operation on the channel will be invalid (that's an FMOD thing)
            // in monitorTrack() we will recreate track (recreate channel) but user is closing app.
            if (result != FMOD_ERR_INVALID_HANDLE)
            {
               pMainWindow->showMessageBox( true, std::string("~Track::FMOD::Channel::stop() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            }
        }
    }

    if (pSound != nullptr)
    {
        result = pSound->release();
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("~Track::FMOD::Sound::release() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }
    }

    if (pFilePath) delete[] pFilePath;
}
