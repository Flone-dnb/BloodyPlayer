#include "controller.h"

// STL
#include <thread>

// Custom
#include "../src/Model/AudioService/audioservice.h"

Controller::Controller(MainWindow* pMainWindow)
{
    pAudioService = new AudioService(pMainWindow);

    bRepeatTrack = false;
    bRandomTrack = false;
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

void Controller::setTrackPos(unsigned int graphPos)
{
    pAudioService->setTrackPos(graphPos);
}

void Controller::stopTrack()
{
    pAudioService->stopTrack();
}

void Controller::nextTrack()
{
    if (bRandomTrack) pAudioService->nextTrack(false, true);
    else              pAudioService->nextTrack();
}

void Controller::prevTrack()
{
    pAudioService->prevTrack();
}

void Controller::setVolume(float fNewVolume)
{
    pAudioService->setVolume(fNewVolume);
}

void Controller::removeTrack(size_t iTrackIndex)
{
    pAudioService->removeTrack(iTrackIndex);
}

void Controller::clearPlaylist()
{
    pAudioService->clearPlaylist();
}

void Controller::moveDown(size_t iTrackIndex)
{
    pAudioService->moveDown(iTrackIndex);
}

void Controller::moveUp(size_t iTrackIndex)
{
    pAudioService->moveUp(iTrackIndex);
}

void Controller::openTracklist(std::wstring pathToTracklist, bool bClearCurrent)
{
    pAudioService->openTracklist(pathToTracklist, bClearCurrent);
}

void Controller::saveTracklist(std::wstring pathToTracklist)
{
    pAudioService->saveTracklist(pathToTracklist);
}

void Controller::setPan(float fPan)
{
    pAudioService->setPan(fPan);
}

void Controller::setPitch(float fPitch)
{
    pAudioService->setPitch(fPitch);
}

void Controller::setSpeedByPitch(float fSpeed)
{
    pAudioService->setSpeedByPitch(fSpeed);
}

void Controller::setSpeedByTime(float fSpeed)
{
    pAudioService->setSpeedByTime(fSpeed);
}

void Controller::setReverbVolume(float fVolume)
{
    pAudioService->setReverbVolume(fVolume);
}

void Controller::setEchoVolume(float fEchoVolume)
{
    pAudioService->setEchoVolume(fEchoVolume);
}

void Controller::loadVSTPlugin(wchar_t *pPathToDll)
{
    pAudioService->loadVSTPlugin(pPathToDll);
}

void Controller::unloadVSTPlugin()
{
    pAudioService->unloadVSTPlugin();
}

void Controller::systemUpdate()
{
    pAudioService->systemUpdate();
}

void Controller::repeatTrack()
{
    pAudioService->repeatTrack();

    bRepeatTrack = !bRepeatTrack;
    if (bRepeatTrack && bRandomTrack) bRandomTrack = false;
}

void Controller::randomNextTrack()
{
    pAudioService->randomNextTrack();

    bRandomTrack = !bRandomTrack;
    if (bRandomTrack && bRepeatTrack) bRepeatTrack = false;
}

size_t Controller::getPlaingTrackIndex(bool& bSomeTrackIsPlaying)
{
    return pAudioService->getPlayingTrackIndex(bSomeTrackIsPlaying);
}





Controller::~Controller()
{
    delete pAudioService;
}
