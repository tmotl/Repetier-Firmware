[<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/ae/Flag_of_the_United_Kingdom.svg/100px-Flag_of_the_United_Kingdom.svg.png" height="30">](README.md)
[<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/a/a4/Flag_of_the_United_States.svg/100px-Flag_of_the_United_States.svg.png" height="30">](README.md)
[<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/b/ba/Flag_of_Germany.svg/100px-Flag_of_Germany.svg.png" height="30">](README.de_DE.md)

# Inoffizielle Modifikation der RF Betriebssoftware für RF1000 und RF2000 Geräte

Basierend auf der Repetier-Betriebssoftware - die schnelle und benutzerfreundliche Betriebssoftware  

## Warnhinweis
Diese Modifikation stellt eine Weiterentwicklung/Ergänzung von mhier's inoffizieller Modifikation der orginal Betriebssoftware dar. Sie wird weder vom Vertreiber der RF1000 und RF2000 Geräte, noch vom Entwickler des ursprünglichen "Z-Offset-Scan" unterstützt.  
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

mhier Z-offset Scan  
Nibbels/Wessix SensiblePressure Anpassung  

## Liste der Funktionen und zusätzlichen GCODEs für den RF2000 und den RF1000

_von mhier entwickelt_:  
M3900 - Messe den Abstand/Versatz des Heizbetts und korrigiere die aktive Z-Matrix des Heizbettes und speichere sie im RAM Speicher.   (Angabe von X, Y Wert der Matrix möglich, Zufallsbestimmung der Messtelle,  Lernfaktor, Lineare Gewichtung in Abhängigkeit von der Entfernung)  
RF2000: Z_OVERRIDE_MAX = 1.0mm, anstelle von 0.5mm.  
RF1000: Z_OVERRIDE_MAX = 0.5mm, weil mehr mit dem Originalschalter nicht zwingend gut ist.  

_von StarTony_:
Lüfter-Geschwindigkeits-Patch, siehe unter:  
http://www.rf1000.de/viewtopic.php?f=7&t=1638  

_von Wessix_:  
M355 Sx - Port X19 an und abschalten / schalten um dort angeschlossene Verbraucher wie z.b. die LED Leiste zu steuern (24V MOSFET @Anschluss X19)  
grundlegende Idee eine Sensible / kontinuierliche Druckmessung zu implentieren,
weiteres unter der Erklärung des MCODES M3909  
und Idee die Zahl der aktuellen Digits zusammen mit der Status-Temperaturabfrage abzugreifen.  

_von Nibbels_:  
Abschaltung des Fräs-Modus! Wenn Sie diesen benötigen kann er innerhalb der Konfigurationsdateien wieder angeschaltet werden.  
M3900 Xn Ym Px Sy - Scanne den Abstand des Heizbetts und korrigiere die aktive Z-Matrix innerhalb des RAM Speichers des Druckers.   (Angabe von X, Y Wert der Matrix möglich, Zufallsbestimmung der Messtelle,  Lernfaktor, Lineare Gewichtung in Abhängigkeit von der Entfernung)  
M3901 Xn Ym Px Sy - um den M3900 GCODE vor zu konfigurieren.  

M3902 R1 - um ein einzelnes Loch im Heizbett in der Heatbed-Matrix zu korrigieren.  

M3902 Zn.n - Um den aktiven Z-Offset zur hinterlegten Z-Matrix zu addieren. Der M3006 Z - Offset wird danach auf 0 gesetzt. Die Matrix wird innerhalb des RAMs verändert und kann danach mit dem Befehl M3902 S [n] an der Position [n] im EEPROM abgelegt werden.  
M3902 Sn - Speichere die gerade im RAM aktivierte Matrix unter der Postion n = {1..9}  
M3902 Z0 S1 - Verschiebe des Z-Offset auf den Matrixwert und speichere die Matrix an der Position 1 im EEPROM - Dies ist ein Beispiel um zu zeigen, dass die Optionen von M3902 kombiniert werden können.  

M3903 Pt Smin - um einen sehr langsamen und schrittweisen Abfall der Betttemperatur einzustellen. Ein Schritt dauert t Sekunden. Die Endtemperatur wird in °C angegeben.  

M3939 Fn St1 Pt2 Ex Iy Rm - um ein Diagramm über die Filamentextrusionsgeschwindigkeit und die korrelierende Digit Zahl aufzuzeichnen -> ermöglicht Rückschlüsse zur Viskosität des Filaments.  

M3920 Sb - Flüstermodus ein oder ausschalten(die Funktion vermindert den Strom der Steppermotoren auf ein in der Firmware definiertes anderes Profil mit niedrigeren Werten).  

Es wurden alle Fehler die in der Orginalbetriebssoftware zu Compilerwarnungen oder Fehlern führten eliminiert. Weiter wurden einige Verbesserungen, die für die allem zugrunde liegenden Repetier Software eingebaut wurden ebenfalls implementiert da von Conrad noch nicht umgesetzt.  

Mit dem Stand vom 20.01.2017 wurde der ganze Mod auf den neuesten Firmwarstand RF.01.37 angehoben.  


_von Nibbels und Wessix entwickelt_:  
M3909 Pn Sm - siehe unter "SensibleDruckmessung / Sensible Pressure"  

## mhier Z-Offset-Scan

* M3900 Xn Ym Sy Px - Mache einen Z-Offset-Scan an einer bestimmten Position.  
Zuvor einstellbare Optionen für M3900:  
* M3901 Xn Ym Sy Px  
[X]/[Y] legt die Position des Scans innerhalb der möglichen HBS-Matrix Positionen fest.  
X = {1..9}, {0 = zufällig}  
Y = {2..12}, {0 = zufällig}  
Mit zwei Extrudern ist X=1 ein erlaubter Wert, mit nur einem ein ungültiger Wert, sodass normalerweise X=2 der niedrigste Matrixwert in X Ausrichtung für eine Scanposition ist. Diese Limitierung wird von Begrenzungen/Begrenzungswerten in jeder Z-Matrix verursacht. Man kann aber nichts falsch einstellen, sollten vom Benutzer unzulässige Werte eingestellt werden, ändert die Firmware diese auf den letzten gültigen Wert.  

[S] legt die Lernrate von M3900 fest  
S=100: Die Matrix wird beim Beginn des Scans komplett vom EEPROM geladen.  

S={0..99}: Die Matrix im RAM wird von 0 - 99% Penetranz des gemessenen Offsets angepasst. Dies ist eine tolle Möglichkeit mit mehreren kleinen Korrekturen eine insgesamt gute Korrekturquote zu erreichen im Falle dass man keinen kompletten HBS machen will oder man den ersten Messwerten nicht zu 100% trauen kann (Stichwort Popel). (Man will letztlich die Korrekturen von Verschiedenen Positionen gemeinsam nutzen)  

[P] mhier Z-Offset-Scan Entfernungsgewichtung  
* P = {0..100}  
Dies ist eine Option mit der sich ein, an einer definierten Position gemessener Offset, nur in einem gewissen Radius um diese Messposition auswirkt. Wenn sie diese Option verwenden und die Mitte des Bettes sowie danach die vier Ecken des Druckbettes mit einer hohen Entfernungsgewichtung scanne wirkt sich dieses Feature wie ein "Auto-Bed-Levelling" auf die Z-Matrix aus. Die Matrix wird dann quasie "verbogen" und nicht mit einem konstanten Wert verändert. (Dies Option könnte für Heizbetten bei denen sich die Ecken bei unterschiedlichen Temperaturen anheben/senken gut funktionieren um etwaige Verdrehungen herauszukorrigieren)  

Sie können den mhier Z-Offset-Scan im Druckermenü unter:  
-> Konfiguration  
   ->  Z-Konfiguration  
      -> Z-Offset Scan  
starten. Dann werden immer die vorkonfigurierten oder bereits geänderten (siehe unter M3901) Einstellungen vom RAM des Druckers verwendet. Diese M3900/M3901 Einstellungen werden nicht im EEPROM des Druckers gespeichert.  

## kontinuierliche sensible Druckmessung

* M3909 Pn Sm  
P = maximale Digitgrenze = {1....14999} [digits]  
S = maximaler Mess/Korrektur-Abstand = {1...300} Standardeinstellung wenn nicht expliziet definiert.: 180 [um]  

Der Drucker wird wenn das Feature aktiviert ist automatisch den in Digits gemessenen Druck unter der vorgegebenen Grenze halten indem er den Abstand zwischen Düse und Bett vergrößert und dadurch den entstehenden Druck verringert. Die Anpassung die vom Drucker vorgenommen wird ist auf positive Werte beschränkt (das bedeutet dass das Bett wird niemals näher an die Düse fahren wie ohne Aktivierung des Mods durch M3909). Die vorgenommenen Änderungen werden den Wert "max. SenseOffset = maximaler Mess/Korrektur-Abstand" nicht überschreiten um ungewollt hohe Offsets/Abstände (falls nicht durch eine zu nah am Bett befindliche Düse bedingt) zu vermeiden.  

Bei der Wahl des Wertes für "max. SenseOffset = maximaler Mess/Korrektur-Abstand" sollten Sie bereits anhand vorangegangener "guter" Drucke wissen welchen Digitwert der Drucker normalerweise bei zufriedenstellendem Druckergebnis einnimmt (Basierend auf gleichem Material, gleichen Drucktemperaturen, gleiche Druckgeschwindigkeit, gleiche Düse) und dann ca 20% oder 1000 Digits an Krafttoleranz dazu addieren.  
Wird die Z-Kompensation ausgeschalten wird die "SensiblePressure = kontinuierliche sensible Druckmessung" ebenfalls ausgeschaltet
Benutzen Sie "M3909 P0" für die manuelle Abschaltung der Funktion, dies sollte aber normalerweise nicht nötig werden. (Die Funktion schaltet sich nach vollenden des kritischen Ersten Layers von selbst ab)  
Sie können die Funktion nicht aktivieren wenn die Z-Kompensation der Grund Firmware zuvor nicht aktiviert wurde.  

Während des Drucks können sie die Funktion aktivieren oder einstellen indem sie die "links" und "rechts" Taste an Bedienfeld drücken, solange sie im "Mod-Menü" des Druckers (Seite 2 auf dem Display des Druckers) sind (Seite 2 kann über die Pfeil nach unten Taste vom Hauptbildschirm aus aufgerufen werden).  

## SensibleViskositätsmessung = SensibleViscosity  
* M3939 Fn St1 Pt2 Ex Iy Rm  
S = Starttemperatur für den Test in °C  
P = Enddemperatur für den Test in °C  
F = maximaler Wert für die Digits = 10000 oder {1...12000} [digits]  
E = maximale Extrusionsgeschwindigkeit = 5 oder {0,05 .... max.StartFeedrate} [mm/s]  

optional:  
I = Extrusionsgeschwindigkeitssteigerungsrate = 0.1 oder {0.02.....0.4} [mm/s]  
R= maximaler Digitunterschied welche angenommen wird um eine "gefüllte" Düse anzunehmen = 800 [digits]  

Der Drucker wird automatisch in die Luft extrudieren und  die dabei entsehende Kraft messen. Nach jeder Messung wirde die Geschwindigkeit der Extrusion erhöht.  
Sie werden eine CSV-formatierte Ausgabe im Konsolenfenster ihres Slicers oder Printservers angezeigt bekommen, die sin dann einfach per kopieren und einfügen in eine Textdatei kopieren können. Diese kann dann z.B. in MS Exel importiert und in einen Graphen umgewandelt werden. Sie werden sich nach dem Sinn dieses Features fragen, aber wenn sie ihr Filament gut kennen können sie die resultierenden Werte vergleichen insbesondere unter sich verändernden Bedingungen und was z.B. passiert wenn die Düse verstopft oder kurz davor ist.  

(Mein) länger angebratenes ABS: http://www.rf1000.de/viewtopic.php?f=62&t=1549&p=16617#p16617  
 ![ ](https://downfight.de/picproxy.php?url=http://image.prntscr.com/image/3508699873f7431489d1df9344110e7f.png "Angebratenes ABS")  
(Mein) ABS: http://www.rf1000.de/viewtopic.php?f=23&t=1620&p=16613#p16605  
 ![ ](https://downfight.de/picproxy.php?url=http://image.prntscr.com/image/610de7720945474590668b20c05d7652.png "ABS")  
 Das ist ABS bei mehr und mehr verstopfter Düse:
 ![ ](https://downfight.de/picproxy.php?url=http://image.prntscr.com/image/c00ac11b3a384994b37ce8bc3cf03bd9.png "Clogged Nozzle ABS")  
 Das ist rotes PLA:
 ![ ](https://downfight.de/picproxy.php?url=http://image.prntscr.com/image/2a3253c930794afc81e4fa4d4b2a4261.png "Clogged Nozzle ABS")  



## FlüsterModus = SilentMode
* M3920 Sb
S = {0,1} = Flüstermodus an und abschalten und Verlust der Nullpunktjustierung 

Diese Funktion kann nützlich sein wenn sie eine zu den von Conrad vorgegebenen Motorströmen abweichende Einstellung dieser definieren wollen, diese Funktion aber an und abschaltbar machen wollen.

Warnung! Wenn Sie zu niedrige Werte/Motorströme einstellen werden Sie Schrittverluste der Motoren riskieren. Der Motor kann dan den enstehenden Fliehkräften nicht ausreichend Kraft engegensetzen und Schritte verlieren. Dies resultiert dann in (in X oder Y) verschobenen Layern. Sollte dies auftreten können sie entweder die Beschleunigungen während des Druckens vermindern oder aber die Motorströme so weit anheben bis dies nichtmehr auftritt.

Wenn Sie die Motorströme verringern, werden Sie eine große Verbesserung der Steppermotorentemperatur und der Lämtproduktion der Motoren erreichen. Der Ton der Motoren wird eine tiefere Tonlage anschlagen und nicht in der unangenehmen Höhe der orginalen Firmware sein.
Man sollte diesen MCODE nicht absetzen solange die Z-Kompensation aktiv ist und genutzt wird, da M3920 diese abschalten wird und der Drucker seine Nullpunktjustierung verlieren wird. Sie müssen danach dann den Drucker auf allen Achsen Nullen/homen und die Z-Kompensation danach wieder anschalten. Wir empfehlen diesen MCODE ganz am Anfang des Startcodes einzubauen.

## RF2000: Zusätzlicher Temperatursensor

Diese Option aktiviert einen weiteren Temperatursensor T3, dessen Messwerte automatisch mit der Statusabfrage ausgegeben werden.
Beispiel: ``` 15:16:14.637: T:204.67 /205 B:28.60 /20 B@:0 @:143 T0:28.60 /0 @0:0 T1:204.67 /205 @1:143 T3:28.23 F:322 ```  
Weitere Konfiguartionsoptionen findet man RESERVE_ANALOG_SENSOR_TYPE in der RF2000.h.
Sucht man in der Firmware nach RESERVE_ANALOG_INPUTS sieht man den Code.
Man muss, um den Temperatursensor zu nutzen, ein zusätzliches Kabel mit einem Temperatursensor am Port X35 an der Druckerplatine des RF2000-Druckers anschliessen. Anschließend kann der optionale Sensor an einem beliebigen Punkt im Drucker positioniert werden.
Es könnte z.B. die Temperatur in der Nähe der Hauptplatine oder im Druckraum gemessen werden.

## Wessix's Hilfe-Video:
[![ScreenShot](https://downfight.de/picproxy.php?url=http://image.prntscr.com/image/d7b7fade0c7343eeb67b680339478894.png)](http://youtu.be/iu9Nft7SXD8)

## !! 03.02.2017: An diesem Projekt wird kontinuierlich weitergearbeitet, spontane Änderungen sind jederzeit möglich.
## !! 28.02.2017: Dieser Mod sollte sich mit Arduino.cc Version 1.8.1 compilieren lassen. Frühere Probleme sind nicht mehr vorhanden.
