# Inofficial modification of the RF firmware for RF1000 and RF2000 devices
Based on Repetier-Firmware - the fast and user friendly firmware.

## Disclaimer

THIS VERSION IS AN UNOFFICIAL MODIFICATION OF THE ORIGINAL FIRMWARE. It is neither endorsed nor
supported by the developers of the RF1000 firmware. USE AT YOUR OWN RISK.

For the official version, have a look at the upstream repository:
https://github.com/RF1000/Repetier-Firmware


## Version RF.01.35mod (2016-12-23) by mhier

* This version is based on RF.01.35 and introduces a new command M3900 to scan for the heat bed offset.

For more details, have a look at this forum post (in German):
http://www.rf1000.de/viewtopic.php?f=7&t=1504#p14882


## Version RF.01.35mod forked transcopied to RF2000 by Nibbels

* Version in Testing-Phase!
* This Version is tested on RF2000 only, but should work on RF1000 as well.

* introduceing a new command M3901 to configure M3900 (X-Y-Pos, Learning-Factor, linear distance weight.)
* M116 Temperature tolerance changed to 2°K
* Z_OVERRIDE_MAX = 1.0mm, instead of 0.5mm
* introduceing a new command M3902 R1 to fix a HeatBeds Hole within the HBS Matrix.
* introduceing a new command M3902 Zn.n to add an Offset [mm] to the Matrix.
* introduceing a new command M3902 Sn to save the active Matrix to position n={1..9}
* introduceing a new command M3903 to configure a very slow heat bed temperature decrease
* included Wessixs Licht-Mod as a new command M355 to configure 24V-MOSFET @Connector X19
* included StarTonys Fan-Speed-Patch, see http://www.rf1000.de/viewtopic.php?f=7&t=1638

## Version RF.01.35d.mod by Wessix and Nibbels

* added a new Feature called "SensiblePressure" 
* It is a pressure-sense-function ment for the first layer. Wessix told me about this idea.
* Syntax: M3909 P[max.digits] S[max.offset]
* The printer will adjust the heat-bed if z < g_minZCompensationSteps, whenever the digits rise atop [max.digits]. 
* The adjustment-offset is fixed to positive values (bed does never go closer to the nozzle than without M3909). the adjustment will not * violate the [max.offset] restriction in order to avoid unwanted offsets (not caused by too close nozzle).
* When choosing [max.digits] you should already know how much digits your printer normally has (Same Material, same Temperatures) and * then add some 20%+.
* When the z-Compensation gets deactivated the pressure-sense-function is deactivated as well.
* Use "M3909 P0" for manual shutdown of the feature, but normally this is not necessary.
* You cannot activate the Feature if zCompensation is not active already.


## !! 31.12.2016: -> Compile with Arduino.cc 1.6.5, otherwise the OutputObject-Command is not 100% stable at Commands::waitUntilEndOfAllMoves();
see http://www.rf1000.de/viewtopic.php?f=7&t=1610&p=16082#p16082
## !! 11.01.2017: Project is Work in Progress and untested changes are possible.
## !! 12.01.2017: ZOS is working good now! 
