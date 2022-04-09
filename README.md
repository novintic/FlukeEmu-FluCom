# FlukeEmu-FluCom
A HW/SW solution for Fluke 9010 mainframe emulation

The Idea is to use a Raspberry Pi to run a Fluke 9010a emulator in software and interface with the hardware (pods) by means
of an add on board/hat (basically to shift the levels and provide power to the pod). So far we have progressed from a proof
of concept to a pretty solid and stable emulation.

The advantages of using an approach like this are many, amongst them:
* cheap Fluke 9010a mainframe replacement using modern technology and thereby adding stuff like network capability, wireless LAN, remote access/VNC, bluetooth etc ‚for free‘
* the interface board can supply all the power rails to the pod so only one 12V wall adapter is needed to power the RaspberryPi and the pods
* the whole setup is portable and compact, you could in theory put the whole thing inside your cabinet/UUT and comfortably use VNC to remote control the Fluke. You could also run the thing ‚headless‘ making it even smaller.

For further instruction on the fluCom board, please check: FlukeEmu-FluCom/fluCom/readme.md

For further instruction on the flukeEmuWx software, please check: FlukeEmu-FluCom/flukeEmuWx/readme.md

## Credits
We would like to credit Adam Courchesne (@onecircuit) for coming up with the idea of a Fluke emulator and hardware combination
and of course for being the first one to actually write one and make it available.

## See also
https://forums.arcade-museum.com/threads/flukeemu-flucom-fluke-9010a-microsystem-troubleshooter-emulator.503305/ 
