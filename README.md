# Inofficial modification of the RF firmware for RF1000 and RF2000 devices
Based on Repetier-Firmware - the fast and user friendly firmware.

## Disclaimer

THIS VERSION IS AN UNOFFICIAL MODIFICATION OF THE ORIGINAL FIRMWARE. It is neither endorsed nor
supported by the developers of the RF1000 firmware. USE AT YOUR OWN RISK.

For the official version, have a look at the upstream repository:
https://github.com/RF1000/Repetier-Firmware


## Version RF.01.33mod

* This version is based on RF.01.33 and introduces a new command M3900 to scan for the heat bed offset.

For more details, have a look at this forum post (in German):
http://www.rf1000.de/viewtopic.php?f=7&t=1504#p14882


## Version RF.01.33mod transcopied to RF2000 by Nibbels

* This version is based on RF.01.33mod and is absolute Alphastage! 
* Still in Testing! 
* At this time I DID NOT EVEN TRY TO RUN IT!!!
* Update 04.12.2016 21:49 : the changes compile well now.
* Update 07.12.2016 13:34 : deleted repository, forked again and remerging files, to get a cleaner changelog

* updated:
* - uilang.h
* - RF2000.h
* - Configuration.h
* - Constants.h
* copied:
* - RF.cpp
* - RF.h
