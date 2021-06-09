TEMPLATE = app
QT       += core
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += \
    ../tests/ModelTests/AudioServiceTests/AudioServiceTests.cpp \
    ../tests/main.cpp \
    ../tests/ModelTests/TrackTests/TrackTests.cpp

LIBS += -L"$$_PRO_FILE_PWD_/../tests" -lBloodyPlayer
win32:
{
    LIBS += -L"$$_PRO_FILE_PWD_/../ext/FMOD/lib/x64"
}
unix: LIBS += -L/usr/local/lib -lfmod

INCLUDEPATH += "../src"
INCLUDEPATH += "../tests"
