#!/bin/bash
printf "*** Installing dependencies\n"
sudo apt install codeblocks build-essential libwxgtk3.0-gtk3-dev libpulse-dev
sudo apt install libwxgtk-webview3.0-gtk3-0v5 libwxgtk-media3.0-gtk3-0v5

mkdir -p ~/fluke/downloads
cd ~/fluke/downloads
wget https://github.com/WiringPi/WiringPi/releases/download/2.61-1/wiringpi-2.61-1-armhf.deb
sudo dpkg -i wiringpi-2.61-1-armhf.deb

printf "\n*** Downloading source files\n"
wget https://github.com/novintic/FlukeEmu-FluCom/archive/refs/heads/main.zip
unzip main.zip

cd ~/fluke
cp -a downloads/FlukeEmu-FluCom-main/flukeEmuWx/* .

cd media
wget "https://arcarc.xmission.com/archive/Test Equipment/Fluke/9010A-Base/128C900.U9"
wget "https://arcarc.xmission.com/archive/Test Equipment/Fluke/9010A-Base/128C901.U19"
wget "https://arcarc.xmission.com/archive/Test Equipment/Fluke/9010A-Base/128C902.U21"

printf "\n*** Building flukeEmuWx\n"
# This part needs to be run in the gui, not over ssh
cd ~/fluke
codeblocks --build FlukeEmuWx.cbp --target='Release'
ln -s bin/Release/FlukeEmuWx .

printf "\n*** Creating menu item\n"
# Create a menu item
cat <<EOF > ~/.local/share/applications/fluke.desktop
[Desktop Entry]
Name=Fluke
Type=Application
Exec=sh -c 'cd $HOME/fluke;$HOME/fluke/FlukeEmuWx'
Icon=/usr/share/pixmaps/codeblocks.png
EOF

printf "\n*** Register for auto-start\n"
# Make it start automatically when booted (optional)
mkdir -p ~/.config/autostart
ln -s ~/.local/share/applications/fluke.desktop ~/.config/autostart

printf "\nDone.\n"
