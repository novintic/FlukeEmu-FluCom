# FlukeEmuWx User Manual

## 1 Introduction
TBD

## 2 Installation
See readme.txt for how to build FlukeEmuWx or check out the manuals on [1] by cyberbarter.

The media folder either needs to be in the same folder as the executable or you have to add the path of the media folder the environment variable PATH.

[1] https://forums.arcade-museum.com/threads/flukeemu-flucom-fluke-9010a-microsystem-troubleshooter-emulator.503305/page-5 

## 3 Features
### 3.1 Tape drive emulation
Clicking the tape icon in the top left corner opens a file dialog where you can select a tape file.
The tape file must exists already  (use touch yourfile.txt on the command line to create an empty file for writing).

### 3.2 Serial port emulation
#### 3.2.1 Host serial port
Setting the serial port flukeEmuWx will route the Fluke 9010B serial port to a real serial port on your rpi. 
The serial port on the RPI GPIO is named /dev/serial0. You will need a  converter board (like FTDI TTL232R) to connect to a USB port on a PC.
The permissions for the current user to access the serial port file must be set. This is done by adding the current user to the group 'tty'.
The port might be used a serial console. In this case you need to  disable the console using "sudo raspi-config" (interface options, disable console, enable serial port hardware).

#### 3.2.1 Serial port to/from file
In order to send a file to the serial port of the Fluke 9010 emulation, select 'send file' in the settings/serial port menu. This will
open a file dialog, where you can choose the file. Once done, the file content will be fed to the Fluke 9010 serial port as soon as
you read form the port. If the file has been fully read, it will be automatically closed.

In order to receive a file from the emulation, select 'receive file' in the settings/serial port menu. This will
open a file dialog, where you can choose the file. Once done, any data sent through the serial port of the Fluke 9010 emulation 
will be written to this file. To detach the file from the serial port, select 'receive file' in the menu and press cancel in the file dialog.

### 3.3 Full screen mode
Enter fullscreen by 
* using the menu "settings/fullscreen"
* Pressing F5
* Left click in the (Fluke emulation) display area and use the pop-up menu
Exit fullscreen by 
* Pressing F5
* Left click in the (Fluke emulation) display area and use the pop-up menu

## Appendix
### A.1 Using FlukeEmuWx over VNC
TBD
