## Instructions for setting up FlukeEmuWx by Clemens (Novintic)
You need:
* A Raspberry PI 2, 3 or 4 with Raspi OS installed
* Codeblocks IDE (if not installed, use add/remove software)
* Fluke 9010A ROMs 
* To connect a POD you need a FluCom HAT board. KiCad files are in folder: kicad

### Get FlukeEmuWx source files
* Download sources from: github...
* Create path: FlukeEmuWx
* Copy source files to path FlukeEmuWx

### Install wxWidgets (Grab a few beers, takes 1-2h!)
* Follow instructions: https://wiki.wxwidgets.org/Compiling_and_getting_started

### Install wiringPi (http://wiringpi.com/news/)
* wget https://unicorn.drogon.net/wiringpi-2.46-1.deb
* sudo dpkg -i wiringpi-2.46-1.deb

### Install pulseAudio
* sudo apt-get install -y libpulse-dev

### Install Z80 emulator source code by Marat Fayzullin
Note the license! This code is not for commercial use and you are not allowed to distribute it yourself.
* Download Z80 Emulator : http://fms.komkon.org/EMUL8/Z80-081707.zip
* Copy the source files into the project folder FlukeEmuWx/Z80

### Install Fluke ROMs (we cannot supply these due to copyright)
* Copy the 3 ROM files to FlukeEmuWx/media
   * Files have to be named: 128C900.U9, 128C901.U19, 128C902.U21

### Start Codeblocks IDE
* Open project file: FlukeEmuWx/FlukeEmuWx.cbp
* Build the project
