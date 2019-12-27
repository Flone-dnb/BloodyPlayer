#include "../ext/Catch2/catch.hpp"

#include <vector>
#include <thread>

#include "View/MainWindow/mainwindow.h"
#include "Model/AudioService/audioservice.h"
#include "Controller/controller.h"
#include "Model/Track/track.h"
#include "globalparams.h"


TEST_CASE("The FMOD system is created without any errors.", "[ModelTests::AudioServiceTests::FMODinit]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);




    // Act

    bool bResult = pAudioService ->isFMODStarted();

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}

TEST_CASE("AudioService is able to add multiple tracks without errors.", "[ModelTests::AudioServiceTests::addTracks]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    // Assert

    REQUIRE( iTrackCount == vPathsToTracks .size() );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService is able to play track without errors.", "[ModelTests::AudioServiceTests::playTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    pAudioService ->setVolume(0.0f);
    pAudioService ->playTrack(0);
    bool bResult = pAudioService ->isSomeTrackIsPlaying();

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService is able to switch to the next track without errors.", "[ModelTests::AudioServiceTests::nextTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    pAudioService ->setVolume(0.0f);


    // We've loaded 3 tracks
    SECTION( "Switch from index 0 to 1" )
    {
        pAudioService ->playTrack(0);

        if ( pAudioService ->isSomeTrackIsPlaying() == false )
        {
            delete pAudioService;
            delete pMainWindow;

            REQUIRE( false );
            return;
        }

        pAudioService ->nextTrack();
        bool bSomethingIsPlaying = false;
        size_t iIndexOfTheCurrentTrack = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);

        bool bResult = false;

        if ( (iIndexOfTheCurrentTrack == 1) && bSomethingIsPlaying )
        {
            bResult = true;
        }

        // Assert

        REQUIRE( bResult );
    }
    SECTION( "Switch from index 2 to 0" )
    {
        pAudioService ->playTrack(2);

        if ( pAudioService ->isSomeTrackIsPlaying() == false )
        {
            delete pAudioService;
            delete pMainWindow;

            REQUIRE( false );
            return;
        }

        pAudioService ->nextTrack();
        bool bSomethingIsPlaying = false;
        size_t iIndexOfTheCurrentTrack = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);

        bool bResult = false;

        if ( (iIndexOfTheCurrentTrack == 0) && bSomethingIsPlaying )
        {
            bResult = true;
        }

        // Assert

        REQUIRE( bResult );
    }



    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService is able to switch to the previous track without errors.", "[ModelTests::AudioServiceTests::prevTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    pAudioService ->setVolume(0.0f);


    // We've loaded 3 tracks
    SECTION( "Switch from index 1 to 0" )
    {
        pAudioService ->playTrack(1);

        if ( pAudioService ->isSomeTrackIsPlaying() == false )
        {
            delete pAudioService;
            delete pMainWindow;

            REQUIRE( false );
            return;
        }

        pAudioService ->prevTrack();
        bool bSomethingIsPlaying = false;
        size_t iIndexOfTheCurrentTrack = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);

        bool bResult = false;

        if ( (iIndexOfTheCurrentTrack == 0) && bSomethingIsPlaying )
        {
            bResult = true;
        }

        // Assert

        REQUIRE( bResult );
    }
    SECTION( "Switch from index 0 to 2" )
    {
        pAudioService ->playTrack(0);

        if ( pAudioService ->isSomeTrackIsPlaying() == false )
        {
            delete pAudioService;
            delete pMainWindow;

            REQUIRE( false );
            return;
        }

        pAudioService ->prevTrack();
        bool bSomethingIsPlaying = false;
        size_t iIndexOfTheCurrentTrack = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);

        bool bResult = false;

        if ( (iIndexOfTheCurrentTrack == 2) && bSomethingIsPlaying )
        {
            bResult = true;
        }

        // Assert

        REQUIRE( bResult );
    }



    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService is able to pause a track without errors.", "[ModelTests::AudioServiceTests::pauseTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    if ( iTrackCount != vPathsToTracks .size() )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->setVolume(0.0f);
    pAudioService ->playTrack(0);

    if ( pAudioService ->isSomeTrackIsPlaying() == false )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->pauseTrack();

    bool bPaused  = pAudioService ->isCurrentTrackPaused();
    bool bPlaying = pAudioService ->isSomeTrackIsPlaying();

    bool bResult = false;

    if ( (bPaused == true) && (bPlaying == false) )
    {
        bResult = true;
    }


    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService: 'monitorTrack()' function is switching to the next track without errors.", "[ModelTests::AudioServiceTests::monitorTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    if ( iTrackCount != vPathsToTracks .size() )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->setVolume(0.0f);
    pAudioService ->playTrack(0);

    if ( pAudioService ->isSomeTrackIsPlaying() == false )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    Track* pCurrentTrack = pAudioService ->getCurrentTrack();

    if (pCurrentTrack == nullptr)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pCurrentTrack ->setPositionInMS( pCurrentTrack ->getLengthInMS() - MAX_TIME_ERROR_MS );

    std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS * 2));

    bool bSomethingIsPlaying = false;
    size_t iCurrentTrackIndex = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);

    bool bResult = false;

    if ( bSomethingIsPlaying && (iCurrentTrackIndex == 1) )
    {
        bResult = true;
    }


    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}

TEST_CASE("AudioService: 'monitorTrack()' function is switching to the same track when 'repeat track' is on.", "[ModelTests::AudioServiceTests::repeatTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    if ( iTrackCount != vPathsToTracks .size() )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->setVolume(0.0f);
    pAudioService ->playTrack(0);

    if ( pAudioService ->isSomeTrackIsPlaying() == false )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    Track* pCurrentTrack = pAudioService ->getCurrentTrack();

    if (pCurrentTrack == nullptr)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->repeatTrack();

    pCurrentTrack ->setPositionInMS( pCurrentTrack ->getLengthInMS() - MAX_TIME_ERROR_MS );

    std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS * 2));

    bool bSomethingIsPlaying = false;
    size_t iCurrentTrackIndex = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);

    bool bResult = false;

    if ( bSomethingIsPlaying && (iCurrentTrackIndex == 0) )
    {
        bResult = true;
    }


    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService: 'monitorTrack()' function is switching to the other track when 'random track' is on.", "[ModelTests::AudioServiceTests::randomNextTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    if ( iTrackCount != vPathsToTracks .size() )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->setVolume(0.0f);
    pAudioService ->playTrack(0);

    if ( pAudioService ->isSomeTrackIsPlaying() == false )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    Track* pCurrentTrack = pAudioService ->getCurrentTrack();

    if (pCurrentTrack == nullptr)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->randomNextTrack();

    pCurrentTrack ->setPositionInMS( pCurrentTrack ->getLengthInMS() - MAX_TIME_ERROR_MS );

    std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_TRACK_INTERVAL_MS * 2));

    bool bSomethingIsPlaying = false;
    size_t iCurrentTrackIndex = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);

    bool bResult = false;

    if ( bSomethingIsPlaying && (iCurrentTrackIndex != 0) )
    {
        bResult = true;
    }


    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService: played tracks history is working when 'random track' is on.", "[ModelTests::AudioServiceTests::tracksHistory]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Collect history

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    if ( iTrackCount != vPathsToTracks .size() )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->setVolume(0.0f);
    pAudioService ->randomNextTrack();
    pAudioService ->playTrack(0);

    if ( pAudioService ->isSomeTrackIsPlaying() == false )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    std::vector<size_t> vPlayedTracksIndexes;
    vPlayedTracksIndexes .push_back(0);

    for (size_t i = 0;   i < 9;   i++)
    {
        pAudioService ->nextTrack(false, true);

        bool bSomethingIsPlaying = false;
        size_t iCurrentTrackIndex = pAudioService ->getPlayingTrackIndex(bSomethingIsPlaying);



        if ( bSomethingIsPlaying == false )
        {
            delete pAudioService;
            delete pMainWindow;

            REQUIRE( false );
            return;
        }

        vPlayedTracksIndexes .push_back(iCurrentTrackIndex);
    }



    // Act

    bool bWorking = true;

    for (int i = vPlayedTracksIndexes .size() - 2;   i >= 0;   i--)
    {
        pAudioService ->prevTrack();

        bool bPlaying = false;
        size_t iCurrentTrack = pAudioService ->getPlayingTrackIndex(bPlaying);

        if( (bPlaying == false) || (iCurrentTrack != vPlayedTracksIndexes[i]) )
        {
            bWorking = false;
            break;
        }
    }




    // Assert

    REQUIRE( bWorking == true );




    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService: 'clear playlist' is working without errors.", "[ModelTests::AudioServiceTests::clearPlaylist]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName = L"Flone - Magic Store (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName1, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName1, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName2, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName2, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName3, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName3, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    if ( iTrackCount != vPathsToTracks .size() )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->setVolume(0.0f);


    // Assert

    SECTION( "'Clear playlist' when nothing is playing" )
    {
        pAudioService ->clearPlaylist();
        size_t iTracksCount = pAudioService ->getTracksCount();

        REQUIRE( iTracksCount == 0 );
    }
    SECTION( "'Clear playlist' when something is playing" )
    {
        pAudioService ->playTrack(0);

        pAudioService ->clearPlaylist();
        size_t iTracksCount = pAudioService ->getTracksCount();

        REQUIRE( iTracksCount == 0 );
    }


    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService: save/open tracklist is working without errors.", "[ModelTests::AudioServiceTests::saveTracklist/openTracklist]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
    std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName1, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName1, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName2 .size() + 1];
    memset(pTrackName2, 0, sTrackName2 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName2, sTrackName2 .c_str(), sTrackName2 .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName3, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName3, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    size_t iTrackCount = pAudioService ->getTracksCount();

    if ( iTrackCount != vPathsToTracks .size() )
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    pAudioService ->setVolume(0.0f);

    pAudioService ->saveTracklist(L"test_tracklist.bpt");
    pAudioService ->openTracklist(L"test_tracklist.bpt", true);
    pAudioService ->setVolume(0.0f);

    _wremove(L"test_tracklist.bpt");

    iTrackCount = pAudioService ->getTracksCount();

    bool bResult = false;

    if ( iTrackCount == vPathsToTracks .size() )
    {
        pAudioService ->playTrack(1);
        std::wstring sCurrentTrackName = pAudioService ->getCurrentTrack() ->getTrackName();
        sCurrentTrackName += L".mp3";

        if ( sCurrentTrackName == sTrackName2 )
        {
            bResult = true;
        }
    }

    // Assert

    REQUIRE( bResult );


    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}

TEST_CASE("AudioService is able to remove a track without errors.", "[ModelTests::AudioServiceTests::removeTrack]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
    std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName1, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName1, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName2 .size() + 1];
    memset(pTrackName2, 0, sTrackName2 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName2, sTrackName2 .c_str(), sTrackName2 .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName3, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName3, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    pAudioService ->removeTrack(1);

    pAudioService ->setVolume(0.0f);

    // Get left tracks' names
    pAudioService ->playTrack(0);
    std::wstring sFirstTrackName = pAudioService ->getCurrentTrack() ->getTrackName();
    pAudioService ->playTrack(1);
    std::wstring sSecondTrackName = pAudioService ->getCurrentTrack() ->getTrackName();
    pAudioService ->stopTrack();

    size_t iLeftTracks = pAudioService ->getTracksCount();

    bool bResult = false;

    if ( iLeftTracks == (vPathsToTracks .size() - 1) )
    {
        sFirstTrackName += L".mp3";
        sSecondTrackName += L".mp3";

        if ( (sFirstTrackName == sTrackName1) && (sSecondTrackName == sFirstTrackName) )
        {
            bResult = true;
        }
    }

    // Assert

    REQUIRE( bResult );


    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}

TEST_CASE("AudioService is able to move up a track without errors.", "[ModelTests::AudioServiceTests::moveUp]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
    std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName1, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName1, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName2 .size() + 1];
    memset(pTrackName2, 0, sTrackName2 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName2, sTrackName2 .c_str(), sTrackName2 .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName3, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName3, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    pAudioService ->setVolume(0.0f);

    SECTION( "Move the track on index 1 up" )
    {
        pAudioService ->moveUp(1);

        bool bResult = false;

        pAudioService ->playTrack(0);

        std::wstring sCurrentTrackName = pAudioService ->getCurrentTrack() ->getTrackName();
        sCurrentTrackName += L".mp3";

        if ( sCurrentTrackName == sTrackName2 )
        {
            bResult = true;
        }

        REQUIRE( bResult );
    }

    SECTION( "Move the track on index 0 up" )
    {
        pAudioService ->moveUp(0);

        bool bResult = false;

        pAudioService ->playTrack(0);

        std::wstring sCurrentTrackName = pAudioService ->getCurrentTrack() ->getTrackName();
        sCurrentTrackName += L".mp3";

        if ( sCurrentTrackName == sTrackName2 )
        {
            bResult = true;
        }

        REQUIRE( bResult );
    }


    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("AudioService is able to move down a track without errors.", "[ModelTests::AudioServiceTests::moveDown]")
{
    // Arrange

    MainWindow*   pMainWindow = new MainWindow();
    AudioService* pAudioService = new AudioService(pMainWindow);

    // Check if the FMOD is even started
    // (we have a test for this)
    if (pAudioService ->isFMODStarted() != true)
    {
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Setup path to track

    std::wstring sTrackName1 = L"Flone - Magic Store (cut).mp3";
    std::wstring sTrackName2 = L"Flone - Little Creature In The Night (cut).mp3";

    // Path 1
    wchar_t* pTrackName1 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName1, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName1, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    // Path 2
    wchar_t* pTrackName2 = new wchar_t[sTrackName2 .size() + 1];
    memset(pTrackName2, 0, sTrackName2 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName2, sTrackName2 .c_str(), sTrackName2 .size() * sizeof(wchar_t));

    // Path 3
    wchar_t* pTrackName3 = new wchar_t[sTrackName1 .size() + 1];
    memset(pTrackName3, 0, sTrackName1 .size() * sizeof(wchar_t) + sizeof(wchar_t));
    std::memcpy(pTrackName3, sTrackName1 .c_str(), sTrackName1 .size() * sizeof(wchar_t));

    std::vector<wchar_t*> vPathsToTracks;
    vPathsToTracks .push_back(pTrackName1);
    vPathsToTracks .push_back(pTrackName2);
    vPathsToTracks .push_back(pTrackName3);




    // Act

    pAudioService ->addTracks(vPathsToTracks);
    pAudioService ->setVolume(0.0f);

    SECTION( "Move the track on index 1 down" )
    {
        pAudioService ->moveDown(1);

        bool bResult = false;

        pAudioService ->playTrack(2);

        std::wstring sCurrentTrackName = pAudioService ->getCurrentTrack() ->getTrackName();
        sCurrentTrackName += L".mp3";

        if ( sCurrentTrackName == sTrackName2 )
        {
            bResult = true;
        }

        REQUIRE( bResult );
    }

    SECTION( "Move the track on index 2 down" )
    {
        pAudioService ->moveDown(2);

        bool bResult = false;

        pAudioService ->playTrack(2);

        std::wstring sCurrentTrackName = pAudioService ->getCurrentTrack() ->getTrackName();
        sCurrentTrackName += L".mp3";

        if ( sCurrentTrackName == sTrackName2 )
        {
            bResult = true;
        }

        REQUIRE( bResult );
    }


    // Cleanup

    delete pAudioService;
    delete pMainWindow;
}
