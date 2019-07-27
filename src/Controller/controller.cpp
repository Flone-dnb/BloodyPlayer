#include "controller.h"

#include <thread>

// Custom
#include "../src/Model/AudioService/audioservice.h"

Controller::Controller(MainWindow* pMainWindow)
{
    pAudioService = new AudioService(pMainWindow);
}




void Controller::addTrack(const wchar_t *pFilePath)
{
    pAudioService->addTrack(pFilePath);
}

void Controller::addTracks(std::vector<wchar_t*> paths)
{
    std::thread addThread(&AudioService::addTracks, pAudioService, paths);
    addThread.detach();
}

void Controller::playTrack(size_t iTrackIndex)
{
    pAudioService->playTrack(iTrackIndex);
}

void Controller::pauseTrack()
{
    pAudioService->pauseTrack();
}

void Controller::stopTrack()
{
    pAudioService->stopTrack();
}

void Controller::nextTrack()
{
    pAudioService->nextTrack();
}

void Controller::prevTrack()
{
    pAudioService->prevTrack();
}

void Controller::changeVolume(float fNewVolume)
{
    pAudioService->changeVolume(fNewVolume);
}

void Controller::removeTrack(size_t iTrackIndex)
{
    pAudioService->removeTrack(iTrackIndex);
}

size_t Controller::getPlaingTrackIndex(bool& bSomeTrackIsPlaying)
{
    return pAudioService->getPlayingTrackIndex(bSomeTrackIsPlaying);
}




Controller::~Controller()
{
    delete pAudioService;
}
