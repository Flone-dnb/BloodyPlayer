#include "track.h"

#include <fstream>
#include <vector>

#include "../src/View/MainWindow/mainwindow.h"
#include "../ext/FMOD/inc/fmod.hpp"
#include "../ext/FMOD/inc/fmod_errors.h"
#include "../src/globalparams.h"

#include <windows.h>

Track::Track(MainWindow *pMainWindow, FMOD::System* pSystem)
{
    pChannel = nullptr;
    pSound   = nullptr;
    this->pMainWindow = pMainWindow;
    this->pSystem    = pSystem;
    iTrackPlayedTimes = 0;
    bPaused = false;
}





bool Track::setTrack(const wchar_t* pFilePath)
{
    char filePath[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, pFilePath, -1, filePath, sizeof(filePath), NULL, NULL);

    FMOD_RESULT result;

    result = pSystem->createStream(filePath, FMOD_DEFAULT, nullptr, &pSound);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::setTrack::FMOD::System::createStream() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }

    this->pFilePath = pFilePath;

    FMOD_SOUND_TYPE type;
    result = pSound->getFormat(&type, nullptr, nullptr, nullptr);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::setTrack::FMOD::Sound::getFormat() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }

    if (type == FMOD_SOUND_TYPE_FLAC) format = "FLAC";
    else if (type == FMOD_SOUND_TYPE_MPEG) format = "MP3";
    else if (type == FMOD_SOUND_TYPE_WAV) format = "WAV";
    else if (type == FMOD_SOUND_TYPE_OGGVORBIS) format = "OGG";

    return true;
}

bool Track::isPlaying()
{
    if (pChannel == nullptr) return false;

    FMOD_RESULT result;
    bool bPlaying;

    result = pChannel->isPlaying(&bPlaying);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::isPlaying::FMOD::Channel::isPlaying() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
    }

    return bPlaying;
}

bool Track::playTrack(float fVolume)
{
    FMOD_RESULT result;

    if (pChannel != nullptr)
    {
        bool bPaused;
        result = pChannel->getPaused(&bPaused);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::getPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        result = pChannel->setVolume(fVolume);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setVolume() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        if (bPaused)
        {
            result = pChannel->setPaused(false);
            if (result)
            {
                pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                return false;
            }
        }
        else
        {
            bool bIsPlaying;
            result = pChannel->isPlaying(&bIsPlaying);
            if (result)
            {
                pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                return false;
            }

            if (bIsPlaying == false)
            {
                result = pChannel->stop();
                if (result)
                {
                    pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::stop() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                    return false;
                }

                result = pSound->release();
                if (result)
                {
                    pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::stop() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                    return false;
                }

                result = pSystem->playSound(pSound, nullptr, false, &pChannel);
                if (result)
                {
                    pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::System::playSound() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                    return false;
                }
            }
        }
    }
    else
    {
        result = pSystem->playSound(pSound, nullptr, true, &pChannel);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::System::playSound() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        result = pChannel->setVolume(fVolume);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setVolume() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        result = pChannel->setPaused(false);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::playTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }
    }

    iTrackPlayedTimes++;

    bPaused = false;

    return true;
}

bool Track::pauseTrack()
{
    FMOD_RESULT result;

    if (pChannel != nullptr)
    {
        bool bPaused;
        result = pChannel->getPaused(&bPaused);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::pauseTrack::FMOD::Channel::getPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        if (bPaused)
        {
            result = pChannel->setPaused(false);
            if (result)
            {
                pMainWindow->showMessageBox( true, std::string("Track::pauseTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                return false;
            }
        }
        else
        {
            result = pChannel->setPaused(true);
            if (result)
            {
                pMainWindow->showMessageBox( true, std::string("Track::pauseTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
                return false;
            }
        }
    }

    bPaused = true;

    return true;
}

bool Track::stopTrack()
{
    FMOD_RESULT result;

    result = pChannel->setPosition(0, FMOD_TIMEUNIT_MS);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::stopTrack::FMOD::Channel::setPosition() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }

    result = pChannel->setPaused(true);
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::stopTrack::FMOD::Channel::setPaused() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        return false;
    }

    bPaused = true;

    return true;
}

bool Track::setPositionInMS(unsigned int iPos)
{
    FMOD_RESULT result;

    if (pChannel != nullptr)
    {
        result = pChannel->setPosition(iPos, FMOD_TIMEUNIT_MS);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::setPositionInMS::FMOD::Channel::setPosition() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }
    }

    return true;
}

bool Track::setVolume(float fNewVolume)
{
    if (pChannel != nullptr)
    {
        FMOD_RESULT result;

        result = pChannel->setVolume(fNewVolume);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::setVolume::FMOD::Channel::setVolume() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }
    }

    return true;
}

unsigned int Track::getLengthInMS()
{
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

unsigned int Track::getPositionInMS()
{
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

std::string Track::getFormat()
{
    return format;
}

bool Track::getChannelsAndBits(int* channels, int* bits)
{
    if (pSound != nullptr)
    {
        FMOD_RESULT result;

        FMOD_SOUND_TYPE type;
        int tchannels;
        int tbits;
        result = pSound->getFormat(&type, nullptr, &tchannels, &tbits);
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::getChannelsAndBits::FMOD::Sound::getFormat() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
            return false;
        }

        *channels = tchannels;
        *bits     = tbits;

        if (type == FMOD_SOUND_TYPE_FLAC) format = "FLAC";
        else if (type == FMOD_SOUND_TYPE_MPEG) format = "MP3";
        else if (type == FMOD_SOUND_TYPE_WAV) format = "WAV";
        else if (type == FMOD_SOUND_TYPE_OGGVORBIS) format = "OGG";

        return true;
    }
    else return false;
}

size_t Track::getTimerPlayed()
{
    return iTrackPlayedTimes;
}

bool Track::getBitRate(int *bitrate)
{
    std::vector<int> framesBitrates;

    // Code below is from my other program so don't really pay attension to some comments


    int amountOfBytes = 0;
    int amountOfBits  = 0;

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

        bool bFoundProtection = false;

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

                if (bit1 && bit2 && bit3)
                {
                    // 11 first bits is set to 'true' seems like it's mp3 frame

                    // Check if format is MPEG-1
                    bit1 = (bool((1 << 4)  &  byte));
                    bit2 = (bool((1 << 3)  &  byte));
                    if (bit1 && bit2)
                    {
                        // MPEG-1

                        // Check if format is Layer III
                             bit3 = (bool((1 << 2)  &  byte));
                        bool bit4 = (bool((1 << 1)  &  byte));
                        if ( (bit3 == false) && (bit4 == true) )
                        {
                            // Layer III

                            // Check if protection is on
                            bit1 = (bool((1 << 0)  &  byte));
                            if (bit1 == false)
                            {
                                // Protection is on
                                bFoundProtection = true;
                                break;
                            }
                            else
                            {
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
                                    // i will think about that later
                                    continue;
                                }

                                // check if padding bit is set
                                bit1 = (bool((1 << 1)  &  byte));

                                amountOfBits += 5;

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
                        }
                        else
                        {
                            // Unsupported format
                            mp3File.close();

                            return false;
                        }
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

        if (bFoundProtection)
        {
            return 0;
        }

        // Minus 8 bytes for the size of a file to hide and it's format
        amountOfBytes -= 8;

        // losing some data here due to int/int - not really important
        amountOfBytes += (amountOfBits / 8);



        std::vector<std::vector<int>> bitrates;

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

        size_t iAverageBitrateIndex = 0;
        for (size_t i = 1; i < bitrates.size(); i++)
        {
            if (bitrates[i][1] > bitrates[iAverageBitrateIndex][1])
            {
                iAverageBitrateIndex = i;
            }
        }

        int iAverageBitrate = bitrates[iAverageBitrateIndex][0];

        *bitrate = iAverageBitrate;

        return true;
    }
    else
    {
        // Can't open file
        return false;
    }
}

bool Track::getPaused()
{
    return bPaused;
}

float Track::getFrequency()
{
    if (pSound != nullptr)
    {
        float freq = 0.0f;
        pSound->getDefaults(&freq, nullptr);
        return freq;
    }

    return 0.0f;
}







Track::~Track()
{
    FMOD_RESULT result;

    if (pChannel != nullptr)
    {
        result = pChannel->stop();
        if (result)
        {
            pMainWindow->showMessageBox( true, std::string("Track::FMOD::Channel::stop() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
        }
    }

    result = pSound->release();
    if (result)
    {
        pMainWindow->showMessageBox( true, std::string("Track::FMOD::Sound::release() failed. Error: ") + std::string(FMOD_ErrorString(result)) );
    }

    delete pFilePath;
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
