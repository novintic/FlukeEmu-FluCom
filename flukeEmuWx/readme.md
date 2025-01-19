## Instructions for setting up FlukeEmuWx by Clemens (Novintic)

## Easy method:

1) Get a Raspberry PI 2, 3 or 4 with Raspi OS installed
2) Download the install script: https://raw.githubusercontent.com/novintic/FlukeEmu-FluCom/refs/heads/main/flukeEmuWx/build-flukeEmuWx.sh
3) make it executable: chmod u+x build-flukeEmuWx.sh
4) execute the script and wait.
5) Start flukeEmuWx from the menu/others folder


## Advanced method
You need:
* A Raspberry PI 2, 3 or 4 with Raspi OS installed
* Codeblocks IDE (if not installed, use add/remove software)
* Fluke 9010A ROMs 
* To connect a POD you need a FluCom HAT board. KiCad files are in folder: kicad

### Get FlukeEmuWx source files
* Download sources from: https://github.com/novintic/FlukeEmu-FluCom
* Create path: FlukeEmuWx
* Copy source files to path FlukeEmuWx

### Install wxWidgets (Grab a few beers, takes 1-2h!)
* Follow instructions: https://wiki.wxwidgets.org/Compiling_and_getting_started

### Install wiringPi (http://wiringpi.com/news/)
* wget https://unicorn.drogon.net/wiringpi-2.46-1.deb
* sudo dpkg -i wiringpi-2.46-1.deb

### Install pulseAudio
* sudo apt-get install -y libpulse-dev

### Install Fluke ROMs (we cannot supply these due to copyright)
* Copy the 3 ROM files to FlukeEmuWx/media
   * Files have to be named: 128C900.U9, 128C901.U19, 128C902.U21

### Start Codeblocks IDE
* Open project file: FlukeEmuWx/FlukeEmuWx.cbp
* Build the project

### Run flukeEmuWx
* Press the run button in codeblocks
* In order to run flukeEmuWx standalone, you need to place the 'media' folder in the same folder as the executable
   * The executable file 'FlukeEmuWx' is in the bin/Release or bin/Debug folder, depending on which target was built
