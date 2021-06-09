// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#include "../ext/Catch2/catch.hpp"

#include <vector>
#include <thread>

#include "View/MainWindow/mainwindow.h"
#include "Model/AudioService/audioservice.h"
#include "Controller/controller.h"
#include "Model/Track/track.h"
#include "globalparams.h"

#if __linux__
#include <unistd.h>
#endif


TEST_CASE("The FMOD system is created without any errors.", "[ModelTests::AudioServiceTests::FMODinit]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);


	// Act

	bool bResult = pAudioService->isFMODStarted();

	// Assert

	REQUIRE(bResult == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}

TEST_CASE("AudioService is able to add multiple tracks without errors.", "[ModelTests::AudioServiceTests::addTracks]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}


	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";
	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	// Assert

	REQUIRE(iTrackCount == vPathsToTracks.size());


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService is able to play track without errors.", "[ModelTests::AudioServiceTests::playTrack]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}


	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";
	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	pAudioService->setVolume(0.0f); // don't need to hear music while testing
	pAudioService->playTrack(0);
	bool bResult = pAudioService->isSomeTrackIsPlaying();


	// Assert

	REQUIRE(bResult == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService is able to pause a track without errors.", "[ModelTests::AudioServiceTests::pauseTrack]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing
	pAudioService->playTrack(0);

	if (pAudioService->isSomeTrackIsPlaying() == false) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->pauseTrack();

	bool bPaused  = pAudioService->isCurrentTrackPaused();
	bool bPlaying = pAudioService->isSomeTrackIsPlaying();

	bool bResult = false;

	if ((bPaused == true) && (bPlaying == false)) {
		bResult = true;
	}


	// Assert

	REQUIRE(bResult == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService: 'monitorTrack()' function is switching to the next track without errors.", "[ModelTests::AudioServiceTests::monitorTrack]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing
	pAudioService->playTrack(0);

	if (pAudioService->isSomeTrackIsPlaying() == false) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	Track* pCurrentTrack = pAudioService->getCurrentTrack();

	if (pCurrentTrack == nullptr) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pCurrentTrack->setPositionInMS(pCurrentTrack->getLengthInMS() - MAX_TIME_ERROR_MS);

	std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS * 2));

	bool bSomethingIsPlaying = false;
	size_t iCurrentTrackIndex = pAudioService->getPlayingTrackIndex(bSomethingIsPlaying);

	bool bResult = false;

	if (bSomethingIsPlaying && (iCurrentTrackIndex == 1)) {
		bResult = true;
	}


	// Assert

	REQUIRE(bResult == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}

TEST_CASE("AudioService: 'monitorTrack()' function is switching to the same track when 'repeat track' is on.", "[ModelTests::AudioServiceTests::repeatTrack]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing
	pAudioService->playTrack(0);

	if (pAudioService->isSomeTrackIsPlaying() == false) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	Track* pCurrentTrack = pAudioService->getCurrentTrack();

	if (pCurrentTrack == nullptr) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->repeatTrack();

	pCurrentTrack->setPositionInMS(pCurrentTrack->getLengthInMS() - MAX_TIME_ERROR_MS);

	std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS * 2));

	bool bSomethingIsPlaying = false;
	size_t iCurrentTrackIndex = pAudioService->getPlayingTrackIndex(bSomethingIsPlaying);

	bool bResult = false;

	if (bSomethingIsPlaying && (iCurrentTrackIndex == 0)) {
		bResult = true;
	}


	// Assert

	REQUIRE(bResult == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService: 'monitorTrack()' function is switching to the other track when 'random track' is on.", "[ModelTests::AudioServiceTests::randomNextTrack]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing
	pAudioService->playTrack(0);

	if (pAudioService->isSomeTrackIsPlaying() == false) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	Track* pCurrentTrack = pAudioService->getCurrentTrack();

	if (pCurrentTrack == nullptr) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->randomNextTrack();

	pCurrentTrack->setPositionInMS(pCurrentTrack->getLengthInMS() - MAX_TIME_ERROR_MS);

	std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS * 2));

	bool bSomethingIsPlaying = false;
	size_t iCurrentTrackIndex = pAudioService->getPlayingTrackIndex(bSomethingIsPlaying);

	bool bResult = false;

	if (bSomethingIsPlaying && (iCurrentTrackIndex != 0)) {
		bResult = true;
	}


	// Assert

	REQUIRE(bResult == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService: played tracks history is working.", "[ModelTests::AudioServiceTests::tracksHistory]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Collect history

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing
	pAudioService->playTrack(0);

	if (pAudioService->isSomeTrackIsPlaying() == false) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	std::vector<size_t> vPlayedTracksIndexes;
	vPlayedTracksIndexes.push_back(0);

	for (size_t i = 0; i < 9; i++) {
		pAudioService->nextTrack(false, true);

		bool bSomethingIsPlaying = false;
		size_t iCurrentTrackIndex = pAudioService->getPlayingTrackIndex(bSomethingIsPlaying);

		if (bSomethingIsPlaying == false) {
			delete pAudioService;
			delete pMainWindow;

			REQUIRE(false);
			return;
		}

		vPlayedTracksIndexes.push_back(iCurrentTrackIndex);
	}


	// Act

	bool bWorking = true;

	for (int i = vPlayedTracksIndexes.size() - 2; i >= 0; i--) {
		pAudioService->prevTrack();

		bool bPlaying = false;
		size_t iCurrentTrack = pAudioService->getPlayingTrackIndex(bPlaying);

		if ((bPlaying == false) || (iCurrentTrack != vPlayedTracksIndexes[i])) {
			bWorking = false;
			break;
		}
	}


	// Assert

	REQUIRE(bWorking == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService: played tracks history is working when 'random track' is enabled.", "[ModelTests::AudioServiceTests::tracksHistory (r)]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Collect history

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing
    pAudioService->randomNextTrack();
	pAudioService->playTrack(0);

	if (pAudioService->isSomeTrackIsPlaying() == false) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	std::vector<size_t> vPlayedTracksIndexes;
	vPlayedTracksIndexes.push_back(0);

	for (size_t i = 0; i < 9; i++) {
		pAudioService->nextTrack(false, true);

		bool bSomethingIsPlaying = false;
		size_t iCurrentTrackIndex = pAudioService->getPlayingTrackIndex(bSomethingIsPlaying);

		if (bSomethingIsPlaying == false) {
			delete pAudioService;
			delete pMainWindow;

			REQUIRE(false);
			return;
		}

		vPlayedTracksIndexes.push_back(iCurrentTrackIndex);
	}


	// Act

	bool bWorking = true;

	for (int i = vPlayedTracksIndexes.size() - 2; i >= 0; i--) {
		pAudioService->prevTrack();

		bool bPlaying = false;
		size_t iCurrentTrack = pAudioService->getPlayingTrackIndex(bPlaying);

		if ((bPlaying == false) || (iCurrentTrack != vPlayedTracksIndexes[i])) {
			bWorking = false;
			break;
		}
	}


	// Assert

	REQUIRE(bWorking == true);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService: 'clear playlist' is working without errors.", "[ModelTests::AudioServiceTests::clearPlaylist]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName, sTrackName, sTrackName};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing


	// Assert

	SECTION("'Clear playlist' when nothing is playing") {
		pAudioService->clearPlaylist();
		size_t iTracksCount = pAudioService->getTracksCount();

		REQUIRE(iTracksCount == 0);
	}
	SECTION("'Clear playlist' when something is playing") {
		pAudioService->playTrack(0);

		pAudioService->clearPlaylist();
		size_t iTracksCount = pAudioService->getTracksCount();

		REQUIRE(iTracksCount == 0);
	}


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService: save/open tracklist is working without errors.", "[ModelTests::AudioServiceTests::saveTracklist/openTracklist]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
	const std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName1, sTrackName2, sTrackName1};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	size_t iTrackCount = pAudioService->getTracksCount();

	if (iTrackCount != vPathsToTracks.size()) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	pAudioService->setVolume(0.0f); // don't need to hear music while testing

	pAudioService->saveTracklist(L"test_tracklist.bpt");
	pAudioService->openTracklist(L"test_tracklist.bpt", true);
	pAudioService->setVolume(0.0f); // don't need to hear music while testing

#if _WIN32
	_wremove(L"test_tracklist.bpt");
#elif __linux__
    unlink("test_tracklist.bpt");
#endif

	iTrackCount = pAudioService->getTracksCount();

	bool bResult = false;

	if (iTrackCount == vPathsToTracks.size()) {
		pAudioService->playTrack(1);
		std::wstring sCurrentTrackName = pAudioService->getCurrentTrack()->getTrackName();
		sCurrentTrackName += L".mp3";

		if (sCurrentTrackName == sTrackName2) {
			bResult = true;
		}
	}

	// Assert

	REQUIRE(bResult);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}

TEST_CASE("AudioService is able to remove a track without errors.", "[ModelTests::AudioServiceTests::removeTrack]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
	const std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

    const std::vector<std::wstring> vPathsToTracks = {sTrackName1, sTrackName2, sTrackName1};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	pAudioService->removeTrack(1);

	pAudioService->setVolume(0.0f); // don't need to hear music while testing

	// Get left tracks' names
	pAudioService->playTrack(0);
	std::wstring sFirstTrackName = pAudioService->getCurrentTrack()->getTrackName();
	pAudioService->playTrack(1);
	std::wstring sSecondTrackName = pAudioService->getCurrentTrack()->getTrackName();
	pAudioService->stopTrack();

	size_t iLeftTracks = pAudioService->getTracksCount();

	bool bResult = false;

	if (iLeftTracks == (vPathsToTracks.size() - 1)) {
		sFirstTrackName += L".mp3";
		sSecondTrackName += L".mp3";

		if ((sFirstTrackName == sTrackName1) && (sSecondTrackName == sFirstTrackName)) {
			bResult = true;
		}
	}

	// Assert

	REQUIRE(bResult);


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}

TEST_CASE("AudioService is able to move up a track without errors.", "[ModelTests::AudioServiceTests::moveUp]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
	const std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

    const std::vector<std::wstring> vPathsToTracks = {sTrackName1, sTrackName2, sTrackName1};

    
	// Act

	pAudioService->addTracks(vPathsToTracks);
	pAudioService->setVolume(0.0f);

	SECTION("Move the track on index 1 up") {
		pAudioService->moveUp(1);

		bool bResult = false;

		pAudioService->playTrack(0);

		std::wstring sCurrentTrackName = pAudioService->getCurrentTrack()->getTrackName();
		sCurrentTrackName += L".mp3";

		if (sCurrentTrackName == sTrackName2) {
			bResult = true;
		}

		REQUIRE(bResult);
	}

	SECTION("Move the track on index 0 up") {
		pAudioService->moveUp(0);

		bool bResult = false;

		pAudioService->playTrack(0);

		std::wstring sCurrentTrackName = pAudioService->getCurrentTrack()->getTrackName();
		sCurrentTrackName += L".mp3";

		if (sCurrentTrackName == sTrackName2) {
			bResult = true;
		}

		REQUIRE(bResult);
	}


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}


TEST_CASE("AudioService is able to move down a track without errors.", "[ModelTests::AudioServiceTests::moveDown]") {
	// Arrange

	MainWindow*   pMainWindow = new MainWindow();
	AudioService* pAudioService = new AudioService(pMainWindow);

	// Check if the FMOD is even started
	// (we have a test for this)
	if (pAudioService->isFMODStarted() != true) {
		delete pAudioService;
		delete pMainWindow;

		REQUIRE(false);
		return;
	}

	// Setup path to track

	const std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
	const std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

	const std::vector<std::wstring> vPathsToTracks = {sTrackName1, sTrackName2, sTrackName1};


	// Act

	pAudioService->addTracks(vPathsToTracks);
	pAudioService->setVolume(0.0f); // don't need to hear music while testing

	SECTION("Move the track on index 1 down") {
		pAudioService->moveDown(1);

		bool bResult = false;

		pAudioService->playTrack(2);

		std::wstring sCurrentTrackName = pAudioService->getCurrentTrack()->getTrackName();
		sCurrentTrackName += L".mp3";

		if (sCurrentTrackName == sTrackName2) {
			bResult = true;
		}

		REQUIRE(bResult);
	}

	SECTION("Move the track on index 2 down") {
		pAudioService->moveDown(2);

		bool bResult = false;

		pAudioService->playTrack(2);

		std::wstring sCurrentTrackName = pAudioService->getCurrentTrack()->getTrackName();
		sCurrentTrackName += L".mp3";

		if (sCurrentTrackName == sTrackName2) {
			bResult = true;
		}

		REQUIRE(bResult);
	}


	// Cleanup

	delete pAudioService;
	delete pMainWindow;
}
