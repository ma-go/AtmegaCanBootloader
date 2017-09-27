CAN Bootloader

Mit dem hier vorgestellten Bootloader lassen sich AVRs die an einem MCP2515 angeschlossen sind über den CAN Bus programmieren.

Im Roboterclub verbauen wir eine ganze Menge AVRs, teilweise auch in den Tiefen des Roboters versteckt, so dass man nur sehr schwer mit einem Programmierkabel dorthin kommt. Allerdings haben sie einen CAN Anschluss. Die Idee war also einen Bootloader zu haben mit dem wir auch diese AVRs einfach per CAN programmieren zu können ohne extra Kabel nach außen zu legen und ohne jedesmal den halben Roboter zu zerlegen.

Der Bootloader ist hauptsächlich in C geschrieben, es wurden allerdings schon Teile der Kommunikation mit dem MCP2515 nach Assembler übersetzt um etwas Platz zu sparen. Aktuell belegt die Software knapp über 1200 Bytes und verbraucht daher 1024 Words im Bootloaderbereich. Allerdings sind mit entsprechender Optimierung vermutlich auch noch die 200 Bytes Einsparung möglich um den Bootloader unter 1024 Bytes zu bekommen, damit würde sich der Platzbedarf auf 512 Words halbieren.
UPDATE:

An dieser Stelle einen herzlichen Dank an Adrian Weiler. Er hat sich daran gemacht und die angesprochenen 200 Bytes dann auch wirklich noch eingespart. Dadurch ist der Bootloader jetzt mit LED Unterstützung nur 1010 Bytes groß, ohne sogar nur noch 994 Bytes. Dadurch belegt er auch nur noch einen 512 Words Bootloader-Bereich. Wer auf die neue Version aktualisiert sollte dies beachten und die Fusebits entsprechend auf die neue Größe anpassen!
Funktionsweise

Der AVR wartet nach einem Reset 500ms auf ein für ihn bestimmte CAN Nachricht (siehe Aufbau der Nachrichten). Falls keine empfangen wird, so wird ein Sprung auf Adresse Null ausgeführt und damit das normale Programm gestartet.

Aktuell werden ATMega8/88/168, ATMega16/32 und ATMega644 unterstützt, weitere AVRs können allerdings ohne große Änderungen eingebaut werden.
Aufbau der Nachrichten

Zur Kommunikation auf dem CAN-Bus werden nur Nachrichten mit zwei verschiedenen 11-Bit Identifiern verwendet um so möglichst wenig mit anderen Protokollen die über CAN laufen in Konflikt zu kommen. Verwendet werden 0x7ff (PC zu AVR) und 0x7fe (AVR zu PC), diese beiden haben die geringste Priorität und stören so den Rest der Kommunikation auf dem Bus nicht bzw. nur wenig. Damit ist auch ein aufspielen neuer Software mitten im Normalen Betrieb möglich.

Jeder AVR bekommt eine eigene 8-Bit Nummer zugewiesen die in Netzwerk einmalig sein muss und über die er direkt angesprochen werden kann. Es können also zwei AVRs gleichzeitig im Bootloader Modus sein, es darf aber immer nur einer programmiert werden, da sie ansonsten eventuell gleichzeitig Nachrichten mit dem gleichen CAN-Identifier verschicken würden was zu Kollisionen auf dem Bus führen könnte.

Wie man erkennt sind die ersten vier Bytes jeder Nachricht mit Status-Informationen belegt und müssen in dieser Form immer mitgesendet werden.

Über das erste Byte (Board Identifier) erfolgt eine eindeutige Identifikation des angesprochenen bzw. sendenden AVRs im Netzwerk.

Das zweite Byte codiert das Kommando sowie den Typ der Nachricht:
Wert 	Typ
00 	REQUEST
01 	SUCCESSFULL_RESPONSE
10 	ERROR_RESPONSE
11 	WRONG_NUMBER_REPSONSE
Wert 	Kommando
1 	IDENTIFY
2 	SET_ADDRESS
3 	DATA
4 	START_APP

Die verschieden Kommandos werden in den nächsten Abschnitten genauer vorgestellt.

Mit dem dritten Byte folgt ein fortlaufender Nachrichtenzähler (Message Number). Dieser wird verwendet um verloren gegangene Nachrichten zu erkennen. Detektiert der AVR eine falsche Nachrichtennummer so sendet er auf jede Anfrage Antworten vom Typ WRONG_NUMBER_REPSONSE mit der erwarteten Nummer in diesem Feld. Das PC-Programm muss dann auf die fehlende Nachricht reagieren und den Zähler bei sich anpassen um die Kommunikation wieder aufzunehmen.

Auf den Wert des vierten Bytes (Data Counter) wird bei den DATA-Nachrichten genauer eingegangen, bei allen anderen Nachrichten sollte in diesem Feld eine 0x80 stehen (SoB-Bit gesetzt sein und alle anderen Bits auf Null).
IDENTIFY

Der PC schickt ein IDENTIFY.REQUEST Kommando ohne zusätzliche Datenbytes um den AVR zu identifizieren und um Werte wie den Bootloadertyp, die Flash-Seitengröße usw. abzufragen.

Der AVR antwortet dann mit folgenden Paket:

Bootloader Type gibt den Typ des Bootloaders an und sollte aktuell immer Null sein. Es ist geplant weitere Bootloader Typen hinzuzufügen, die auf kosten eines höheren Speicherbedarfs zum Beispiel auch das Eeprom lesen und schreiben können.
RWW Page Count gibt die Anzahl der beschreibbaren Flash-Seiten und damit die Größe des beschreibbaren Speichers an.

Im Pagesize Identifier ist dann noch die Größe der einzelnen Flash-Seiten kodiert:
Wert 	Seitengröße
0 	32 Byte
1 	64 Byte
2 	128 Byte
3 	256 Byte

Nach der AVR geantwortet hat befindet er sich im Bootloader Modus und wartet auf weitere Befehle. Auch der 500ms Timer wird damit beendet, so dass jetzt nur noch mit einem START_APP-Kommando das normale Programm gestartet werden kann.
SET_ADDRESS

Als nächstes muss festgelegt werden wohin geschrieben werden soll, dazu gibt es das SET_ADDRESS Kommando:

Der Wert für Flash Page entspricht der Seite die beschrieben werden soll und Page Buffer Position gibt dabei die Position innerhalb dieser Seite an.
Über den Wert von Page Buffer Position kann nach einem Fehler wieder an einer definierten Position mit dem Füllen des Puffers angefangen werden.

Es gilt dabei:

Buffer Address = Page Buffer Position * 4

Dies ist so gewählt, da die Daten immer in vier Byte großen Paketen gesendet werden. Es macht also keinen Sinn an Position die nicht durch vier teilbar sind anfangen zu wollen.
DATA

Kommen wir nun zu den eigentlichen Nutzdaten die man in den Flash schreiben möchte. Ist die über die anderen Kommandos eine Kommunikation aufgebaut, so können über das DATA-Kommando die Daten verschickt werden.
Die DATA.REQUEST Nachrichten sind immer acht Byte lang, die hinteren vier Bytes sind dabei die Nutzdaten.

Da es recht lange Latenzzeiten im PC beim Warten auf Antwort und dem Losschicken der nächsten Nachricht gibt und diese die Programmierzeiten stark in die Länge zogen wurden Nachrichtenblöcke eingeführt.

Dafür wird das vierte Byte (Data Counter) des Status-Informations-Feld am Anfang der Nachrichten verwendet.

Ist das SoB-Bit (Start of Block) gesetzt wird ein neuer Nachrichtenblock gestartet. die unteren sieben Bit geben dann an wie viele Nachrichten noch folgenden. Es sind beliebige Blockgrößen möglich.

Um zum Beispiel einen Block der Größe 16 Nachrichten zu senden wird bei der Start-Nachricht das Byte auf den Wert 0x8F (0x80 => Flag für den Start des Blocks + 15 (0x0F) => 15 noch folgende Nachrichten) gesetzt.

Bei den nächsten Nachrichten wird dieser Wert ohne das gesetzte SoB-Bit dann herunter gezählt, also 0x0E, 0x0D usw. bis mit einem 0x00 der Nachrichtenblock endet. Der AVR sendet daraufhin eine DATA.SUCCESSFULL_RESPONSE Nachricht.

Diese enthält im Normalfall außer den Status-Informationen keine weiteren Bytes. Wird mit dem Ende eines Blocks auch gleichzeitig das Ende des Puffers erreicht, so wird der Puffer in die aktuell aktive Flash-Seite geschrieben und die nächste Seite auf aktiv gesetzt. Damit können ohne ein SET_ADDRESS Kommando direkt die nächsten Daten verschickt werden. Tritt dieser Fall ein, so wird eine spezielle DATA.SUCCESSFULL_RESPONSE Nachricht verschickt, diese enthält in zwei zusätzlichen Bytes die Flash-Seite die gerade beschrieben wurde.

Das PC Programm ist dafür zuständig, dass das Ende des Puffers immer mit dem Ende eines Blocks erreicht wird, genauso dafür das immer vollständige Flash-Seiten geschrieben werden. Füllt zum Beispiel ein Programm für den AVR die letzte Flash-Seite nur zur Hälfte, so muss diese um entsprechend viele 0xFFs ergänzt werden bis wieder eine vollständige Flash-Seite daraus geworden ist.
START_APP

Über dieses Kommando wird der Bootloader-Vorgang beendet und das neu geschriebene Programm durch einen Sprung auf die Adresse Null gestartet.
Ablauf der Kommunikation

Sämtliche Kommunikation läuft nach einem Anfrage/Antwort Prinzip ab. Der AVR sendet niemals von sich aus, sondern antwortet immer nur auf Anfragen des PC. Dabei wird mit Ausnahme von Nachrichtenblöcken jede Nachricht des PCs beantwortet.

Nachrichtenblöcke wurden eingeführt um die Kommunikation zu beschleunigen, da die Latenzzeiten auf PC Seite zu lang waren. Es wird dabei ein größerer Block von Nachrichten auf einmal losgeschickt und erst die letzte Nachricht vom AVR beantwortet. Auf den größeren AVRs werden Blöcke mit einer Länge von 64 Nachrichten verwendet. Damit passt genau eine Flash-Seite (256 Byte) in einen Nachrichtenblock.
Bootloader Kommunikation

Nachdem der PC über ein IDENTIFY Kommando die Daten des AVRs wie Flashblock Größe, Anzahl der beschreibbaren Flash-Seite usw. ausgelesen hat setzt er die zu bearbeitende Flash-Seite und die Adresse innerhalb dieser. Danach werden in Blöcken die Daten übertragen. Erreicht ein Block das Ende einer Flash-Seite so wird diese vom Puffer in den Flash geschrieben und auf die nächste umgeschaltet. Dies wird über eine spezielle DATA.SUCESSFULL_RESPONSE Nachricht angezeigt.
Sind alle Daten geschrieben kann das neu geschriebene Programm gestartet und damit die Kommunikation beendet werden.

Es wird keine zusätzliche Verifikation der Daten vorgenommen da der CAN Controller von sich aus schon jede Nachricht mit einer CRC Checksumme absichert. Wird also ein Paket empfangen, so kann mit sehr großer Sicherheit davon ausgegangen werden das es korrekt war.
Bootloader compilieren

Es müssen in der config.h noch einige defines an das eigene Projekt angepasst werden, zum Beispiel:

#define MCP2515_CS              B,4
#define MCP2515_INT             B,2

#define BOOT_LED                B,0

//#define   BOOT_LED_SET_OUTPUT
//#define   BOOT_LED_ON
//#define   BOOT_LED_OFF
//#define   BOOT_LED_TOGGLE

//#define   BOOT_INIT

Hängt die Bootloader LED nicht direkt an einem Port Pin kann man über die Makros BOOT_LED_… die Ausgabe anpassen (ich habe dies zum Beispiel genutzt um eine Duo-LED über zwei Port-Pins anzusteuern). Wird BOOT_LED überhaupt nicht definiert, so läuft der Bootloader ohne die LED-Statusanzeige. Es wird zwar eine Warnung erzeugt, das schränkt jedoch die Funktionalität nicht ein.

BOOT_INIT wird beim Starten des Bootloaders einmal aufgerufen, dort können eigene speziell für die Platine notwendige Initialisierungen untergebracht werden.

Als nächstes muss im Makefile der verwendete AVR, die Taktfrequenz sowie die Adresse des Bootloaders einstellt werden.
Danach kann der Bootloader gebaut werden:

$ make all

Die entstandene Datei bootloader.hex kann jetzt per ISP aufgespielt werden. Außerdem müssen die Fusebits für einen Bootloader-Bereich von 512 Words eingestellt werden.

Für einen ATMega32 wäre das zum Beispiel:

hfuse 0xDC
lfuse 0x94

(entspricht Interner 8Mhz Oszillator, Brown-Out-Detection auf 2,7V, Boot-Reset-Vector aktiviert, 512 Words Boot-Flash-Section, JTAG deaktiviert)
PC Software

Der PC Teil ist in Python geschrieben wird über die Kommandozeile gestartet. Eine entsprechende GUI fehlt noch, vielleicht hat ja jemand Lust daran weiterzumachen?

Bisher habe ich die Software nur unter Linux mit meinem CAN Debugger getestet. Das ganze sollte aber genauso unter Windows lauffähig sein. Allerdings muss das Script vermutlich an den verwendeten CAN Adapter angepasst werden wenn man einen anderen verwendet.
Benutzen der Software

$ python bootloader_can.py -i BOARD_ID -p COM_PORT -f FILE.hex

BOARD_ID, COM_PORT und FILE müssen natürlich an das eigene Projekt angepasst werden. Der Wert von BOARD_ID muss dabei den bei erstellen des Bootloaders unter BOOTLOADER_BOARD_ID angegeben Wert entsprechen.
Geschwindigkeit

Mit CAN Bus von 125 Kbps und einem ATMega32 (Flash-Seitengröße 128 Byte) braucht man für 20 kByte ungefähr 14-15 Sekunden. Für einen ATMega644 (Flash-Seitengröße 256 Byte) braucht man bei der gleichen Menge Daten ungefähr 10-12 Sekunden.

Je größer die Nachrichtenblöcke und je höher die CAN Bitrate um so schneller geht es.


