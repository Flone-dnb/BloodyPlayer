#!/bin/bash

# Copy binary.
sudo cp build/BloodyPlayer /usr/local/bin/
sudo chmod +x /usr/local/bin/BloodyPlayer

# Copy FMOD libs.
if [ ! -d "/usr/local/lib/BloodyPlayer" ]; then
    sudo mkdir /usr/local/lib/BloodyPlayer/
fi
sudo cp -Prv ext/FMOD/lib/x64_linux/* /usr/local/lib/BloodyPlayer/

# Copy icon.
if [ ! -d "/usr/share/pixmaps" ]; then
    sudo mkdir /usr/share/pixmaps/
fi
sudo cp res/bloodyLogo2.png /usr/share/pixmaps/

# Set ldconfig to look for our folder.
if [ ! -d "/etc/ld.so.conf.d" ]; then
    sudo mkdir /etc/ld.so.conf.d
fi
echo '/usr/local/lib/BloodyPlayer' | sudo tee -a /etc/ld.so.conf.d/bloodyplayer.conf
sudo ldconfig

# Create desktop entry.
cp BloodyPlayer.desktop ~/.local/share/applications/
