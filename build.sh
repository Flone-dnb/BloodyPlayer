#!/bin/bash
mkdir build
cd build
/usr/bin/qmake ../ide/BloodyPlayer.pro -spec linux-g++ CONFIG+=qtquickcompiler
make -j$(grep -c ^processor /proc/cpuinfo | awk '{s= $1 + 1} END {print s}')
