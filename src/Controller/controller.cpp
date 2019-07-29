#include "controller.h"

// STL
#include <thread>

// Custom
#include "../src/Model/AudioService/audioservice.h"

Controller::Controller(MainWindow* pMainWindow)
{
    pAudioService = new AudioService(pMainWindow);
}





void Controller::addTracks(std::vector<wchar_t*> paths)
{
    // We run this method (AudioService::addTracks) in another thread, because this call is coming from the main thread and
    // the interface will stop updating if you do not create a separate thread here.
    // Of course, we can use QApplication::processEvents(), but I want to create a separate thread.
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

void Controller::moveDown(size_t iTrackIndex)
{
    pAudioService->moveDown(iTrackIndex);
}

void Controller::moveUp(size_t iTrackIndex)
{
    pAudioService->moveUp(iTrackIndex);
}

size_t Controller::getPlaingTrackIndex(bool& bSomeTrackIsPlaying)
{
    return pAudioService->getPlayingTrackIndex(bSomeTrackIsPlaying);
}





Controller::~Controller()
{
    delete pAudioService;
}
