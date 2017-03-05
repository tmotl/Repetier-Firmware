# Inoffizielle Modifikation der RF Betriebssoftware für RF1000 und RF 2000 Geräte

Basierend auf der Repetier-Betriebssoftware - die schnelle und benutzerfreundliche Betriebssoftware

## Warnhinweis
Diese Modifikation stellt eine Weiterentwicklung/Ergänzung von mhier's inoffizieller Modifikation der orginal Betriebssoftware dar. Sie wird weder vom Vertreiber der RF 1000 und RD 2000 Geräte, noch vom Entwickler des ursprünglichen "Z-Offset-Scan" unterstützt.
Benutzung auf eigene Gefahr. Für etwaige Schäden die an Ihrem Gerät entstehen können kann keine Verantwortung übernommen werden.

Für die offizielle Version der Betriebssoftware bitten wir Sie das zugrundeliegende Repository unter:
https://github.com/RF1000/Repetier-Firmware
aufzurufen. 

Für die modifizierte Version von mhier schauen Sie bitte unter:
https://github.com/mhier/Repetier-Firmware

## Version RF 1.37mod

http://www.rf1000.de/viewtopic.php?f=74&t=1674 (Nibbels/Wessix SenseOffset-Thread)

http://www.rf1000.de/viewtopic.php?f=7&t=1504#p14882 (mhier Mod)

http://www.rf1000.de/viewtopic.php?f=7&t=1504&start=60#p16397 (added feature)

http://www.rf1000.de/viewtopic.php?f=7&t=1610 (rf1000s developement version thread to 1.35)

http://www.rf1000.de/viewtopic.php?f=7&t=1665 (rf1000s developement version thread to 1.37)

https://github.com/Nibbels/Repetier-Firmware/commits/heat_bed_z_offset_scan-%26-heat_bed_decrease (all the commits)

## Haupt Funktionen dieser Modifikation:

* mhier Z-offset Scan
* Nibbels/Wessix SensiblePressure Anpassung

Liste der Funktionen und zusätzlichen GCODEs für den RF2000 und den RF1000

von mhier implementiert:
M3900 - Messe den Abstand/Versatz des Heizbetts und korrigiere die aktive Z-Matrix des Heizbettes und speichere sie im RAM Speicher. (Angabe von X, Y Wert der Matrix möglich, Zufallsbestimmung der Messtelle,  Lernfaktor, Lineare Gewichtung in Abhängigkeit von der Entfernung)
--------- Z overrider je nach stand

von StarTony implementiert:
Lüfter-Geschwindigkeits-Patch, siehe unter:
http://www.rf1000.de/viewtopic.php?f=7&t=1638

von Wessix implementiert:
M355 Sx - Port X19 an und abschalten / schalten um dort angeschlossene Verbraucher wie z.b. die LED Leiste zu steuern (24V MOSFET @Anschluss X19)
grundlegende Idee eine Sensible / kontinuierliche Druckmessung zu implentieren,
weiteres unter der Erklärung des MCODES M3909
und Idee die Zahl der aktuellen Digits zusammen mit der Status-Temperaturabfrage abzugreifen.

von Nibbles implementiert:
Abschaltung des Fräs-Modus! Wenn Sie diesen benötigen kann er innerhalb der Konfigurationsdateien wieder angeschaltet werden.
M3900 Xn Ym Px Sy - Scanne den Abstand des Heizbetts und korrigiere die aktive Z-Matrix innerhalb des RAM Speichers des Druckers.(Angabe von X, Y Wert der Matrix möglich, Zufallsbestimmung der Messtelle,  Lernfaktor, Lineare Gewichtung in Abhängigkeit von der Entfernung)
M3901 Xn Ym Px Sy - um den M3900 GCODE vor zu konfigurieren.

M3902 R1 - um ein einzelnes Loch im Heizbett in der Heatbed-Matrix zu korrigieren.

M3902 Zn.n - Um den aktiven Z-Offset zur hinterlegten Z-Matrix zu addieren. Der M3006 Z - Offset wird danach auf 0 gesetzt. Die Matrix wird innerhalb des RAMs verändert und kann danach mit dem Befehl M3902 S [n] an der Position [n] im EEPROM abgelegt werden.
M3902 Sn - Speichere die gerade im RAM aktivierte Matrix unter der Postion n = {1..9}
M3902 Z0 S1 - Verschiebe des Z-Offset auf den Matrixwert und speichere die Matrix an der Position 1 im EEPROM - Dies ist ein Beispiel um zu zeigen, dass die Optionen von M3902 kombiniert werden können.

M3903 Pt Smin - um einen sehr langsamen und schrittweisen Abfall der Betttemperatur einzustellen. Ein Schritt dauert t Sekunden. Die Endtemperatur wird in °C angegeben.

M3939 Fn St1 Pt2 Ex Iy Rm - um ein Diagramm über die Filamentextrusionsgeschwindigkeit und die korrelierende Digit Zahl aufzuzeichnen -> ermöglicht Rückschlüsse zur Viskosität des Filaments

M3920 Sb - Flüstermodus ein oder ausschalten(die Funktion vermindert den Strom der Steppermotoren auf ein in der Firmware definiertes anderes Profil mit niedrigeren Werten)

Es wurden alle Fehler die in der Orginalbetriebssoftware zu Compilerwarnungen oder Fehlern führten eliminiert. Weiter wurden einige Verbesserungen, die für die allem zugrunde liegenden Repetier Software eingebaut wurden ebenfalls implementiert da von Conrad noch nicht umgesetzt.

Mit dem Stand vom 20.01.2017 wurde der ganze Mod auf den neuesten Firmwarstand RF.01.37 angehoben.


Von Nibbels und Wessix entwickelt:
M3909 Pn Sm - siehe unter "SensibleDruckmessung / Sensible Pressure"

Z-Offset-Scan

M3900 Xn Ym Sy Px - Mache einen Z-Offset-Scan an einer bestimmten Position.
Zuvor einstellbare Optionen für M3900:
M3901 Xn Ym Sy Px
[X]/[Y] legt die Position des Scans innerhalb der möglichen HBS-Matrix Positionen fest.
X = {1..9}, {0 = zufällig} Y = {2..12}, {0 = zufällig}
Mit zwei Extrudern ist X=1 ein erlaubter Wert, mit nur einem ein ungültiger Wert, sodass normalerweise X=2 der niedrigste Matrixwert in X Ausrichtung für eine Scanposition ist. Diese Limitierung wird von Begrenzungen/Begrenzungswerten in jeder Z-Matrix verursacht. Man kann aber nichts falsch einstellen, sollten vom Benutzer unzulässige Werte eingestellt werden, ändert die Firmware diese auf den letzten gültigen Wert.

[S] legt die Lernrate von M3900 fest
S=100: Die Matrix wird beim Beginn des Scans komplett vom EEPROM geladen. 

S={0..99}: Die Matrix im RAM wird von 0 - 99% Penetranz des gemessenen Offsets angepasst. Dies ist eine tolle Möglichkeit mit mehreren kleinen Korrekturen eine insgesamt gute Korrekturquote zu erreichen im Falle dass man keinen kompletten HBS machen will oder man den ersten Messwerten nicht zu 100% trauen kann (Stichwort Popel). (Man will letztlich die Korrekturen von Verschiedenen Positionen gemeinsam nutzen)

[P] mhier Z-Offset-Scan Entfernungsgewichtung
P = {0..100}
Dies ist eine Option mit der sich ein, an einer definierten Position gemessener Offset, nur in einem gewissen Radius um diese Messposition auswirkt. Wenn sie diese Option verwenden und die Mitte des Bettes sowie danach die vier Ecken des Druckbettes mit einer hohen Entfernungsgewichtung scanne wirkt sich dieses Feature wie ein "Auto-Bed-Levelling" auf die Z-Matrix aus. Die Matrix wird dann quasie "verbogen" und nicht mit einem konstanten Wert verändert. (Dies Option könnte für Heizbetten bei denen sich die Ecken bei unterschiedlichen Temperaturen anheben/senken gut funktionieren um etwaige Verdrehungen herauszukorrigieren)  





  

 

  
 

 
 


