#include "../ext/Catch2/catch.hpp"

#include "View/MainWindow/mainwindow.h"
#include "Model/AudioService/audioservice.h"
#include "Model/Track/track.h"



TEST_CASE("Track object is created without errors.", "[ModelTests::TrackTests::setupTrack]")
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

    wchar_t* pTrackName = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Create track

    Track* pTrackObject = new Track(pTrackName,
                                    L"Flone - Magic Store (cut)",
                                    pMainWindow,
                                    pAudioService ->getFMODSystem());



    // Act

    bool bResult = pTrackObject ->setupTrack();

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pTrackObject; // delete track before audio service because track will call FMOD::releaseSound().
    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("Track object can be played without errors.", "[ModelTests::TrackTests::playTrack]")
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

    wchar_t* pTrackName = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Create track

    Track* pTrackObject = new Track(pTrackName,
                                    L"Flone - Magic Store (cut)",
                                    pMainWindow,
                                    pAudioService ->getFMODSystem());




    // Prepare for test

    if ( pTrackObject ->setupTrack() != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Act

    bool bResult = pTrackObject ->playTrack(0.75f);

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pTrackObject; // delete track before audio service because track will call FMOD::releaseSound().
    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("Track object can be stopped without errors.", "[ModelTests::TrackTests::stopTrack]")
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

    wchar_t* pTrackName = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Create track

    Track* pTrackObject = new Track(pTrackName,
                                    L"Flone - Magic Store (cut)",
                                    pMainWindow,
                                    pAudioService ->getFMODSystem());




    // Prepare for test

    if ( pTrackObject ->setupTrack() != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    if ( pTrackObject ->playTrack(0.75f) != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Act

    bool bResult = pTrackObject ->stopTrack();

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pTrackObject; // delete track before audio service because track will call FMOD::releaseSound().
    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("Track's position can be changed without errors.", "[ModelTests::TrackTests::setPositionInMS]")
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

    wchar_t* pTrackName = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Create track

    Track* pTrackObject = new Track(pTrackName,
                                    L"Flone - Magic Store (cut)",
                                    pMainWindow,
                                    pAudioService ->getFMODSystem());




    // Prepare for test

    if ( pTrackObject ->setupTrack() != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    if ( pTrackObject ->playTrack(0.0f) != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Act

    bool bResult = pTrackObject ->setPositionInMS(2000);

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pTrackObject; // delete track before audio service because track will call FMOD::releaseSound().
    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("Track object can be paused without errors.", "[ModelTests::TrackTests::pauseTrack]")
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

    wchar_t* pTrackName = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Create track

    Track* pTrackObject = new Track(pTrackName,
                                    L"Flone - Magic Store (cut)",
                                    pMainWindow,
                                    pAudioService ->getFMODSystem());




    // Prepare for test

    if ( pTrackObject ->setupTrack() != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    if ( pTrackObject ->playTrack(0.75f) != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    if ( pTrackObject ->setPositionInMS(2000) != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Act

    bool bResult = pTrackObject ->pauseTrack();

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pTrackObject; // delete track before audio service because track will call FMOD::releaseSound().
    delete pAudioService;
    delete pMainWindow;
}


TEST_CASE("Track object can be recreated without errors.", "[ModelTests::TrackTests::reCreateTrack]")
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

    wchar_t* pTrackName = new wchar_t[sTrackName .size() + 1];
    memset(pTrackName, 0, sTrackName .size() * sizeof(wchar_t) + 2);
    std::memcpy(pTrackName, sTrackName .c_str(), sTrackName .size() * sizeof(wchar_t));

    // Create track

    Track* pTrackObject = new Track(pTrackName,
                                    L"Flone - Magic Store (cut)",
                                    pMainWindow,
                                    pAudioService ->getFMODSystem());




    // Prepare for test

    if ( pTrackObject ->setupTrack() != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    if ( pTrackObject ->playTrack(0.75f) != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    if ( pTrackObject ->setPositionInMS(2000) != true )
    {
        delete pTrackObject;
        delete pAudioService;
        delete pMainWindow;

        REQUIRE( false );
        return;
    }

    // Act

    bool bResult = pTrackObject ->reCreateTrack(0.0f);

    // Assert

    REQUIRE( bResult == true );




    // Cleanup

    delete pTrackObject; // delete track before audio service because track will call FMOD::releaseSound().
    delete pAudioService;
    delete pMainWindow;
}
