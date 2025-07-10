#ifndef Constants_h
#define Constants_h

#include <avr/pgmspace.h>
#include "Arduino.h"
#include <KLineKWP1281Lib.h>
#include <Adafruit_SH110X.h>
#include <U8g2_for_Adafruit_GFX.h>

const char ENGINE[] = "3B"; //mögliche Werte: "3B", "MC", "NF"  - für RR, AAN, etc. verwende "3B"
#define USE_RELAIS false //für MC, NF, etc. auf true setzen
#define PIN_RELAY 12 //für MC, NF, etc. den Relais PIN setzen


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_ADDRESS 0x3C

#define BUTTON_LEFT_PIN 5
#define BUTTON_RIGHT_PIN 6

//K-Line

// Change which measurement to read, from which group.
#define GROUP_TO_READ 0 // valid range: 0-255
#define INDEX_TO_READ 0 // valid range: 0-3


const char errorNotFound[] PROGMEM =  "Fehlertext nicht gefunden";

const char error_1111_3B[] PROGMEM = "Schaltkreise defekt, falsche Checksumme der EPROMs (je nach Software) -> Masseverbindung und Verdrahtung überprüfen, Steuergerät ersetzen";
const char error_1111_MC[] PROGMEM = "Bauteil defekt -> Steuergerät reparieren bzw. austauschen";
const char error_1111_NF[] PROGMEM = "Motorsteuergerät, oder Zündsteuergerät defekt, bzw schadhaftes Bauteil -> Elektrik überprüfen, Steuergerät reparieren, bzw ersetzen";
const char error_1231_3B_NF[] PROGMEM = "Drehzahlsensor am Getriebe (G68), Drehzahlsignal für Zündsteuergerät (ROV), Sensor überprüfen";
const char error_2111_3B_MC[] PROGMEM = "Fehler kommt immer wenn Motor bei der Fehlerdiagnose aus war! Drehzahlsensor, Leitung unterbrochen, oder Kurzschluss, Geber und/oder Leitung defekt, Schwungradzähne verschmutzt / defekt";
const char error_2112_3B_MC[] PROGMEM = "OT Geber auf dem Schwungrad defekt / fehlt, Bezugsmarkengeber verstellt, Stift auf dem Schwungrad, Hallsensor im Verteiler dejustiert";
const char error_2113_3B_MC[] PROGMEM = "Befestigung des Verteilers, Hallsensor fehlerhaft, Verteiler, bzw Hallgeber lose / falsch befestigt";
const char error_2121_3B[] PROGMEM = "Kontakt öffnet nicht, Drosselklappenschalter defekt, Leitungsunterbrechung, Schalter ersetzen";
const char error_2121_MC[] PROGMEM = "Drosselklappenschalter / Leerlaufkontakt, Kontakt öffnet nicht -> Drosselklappenschalter defekt, Leitungsunterbrechung, Schalter ersetzen";
const char error_2121_NF[] PROGMEM = "Kontakt defekt, öffnet nicht -> Schalter defekt, Befestigung schadhaft, Leitungsunterbrechung";
const char error_2122_NF[] PROGMEM = "Drehzahlsignal für Zündsteuergerät, Unterbrechung, oder Kurzschluss, Sensor, oder Kabel defekt, Zahnkranz auf dem Schwungrad beschädigt";
const char error_2123_MC_NF[] PROGMEM = "Drosselklappenschalter / Volllastkontakt, Kontakt öffnet nicht -> Drosselklappenschalter defekt, Leitungsunterbrechung, Schalter ersetzen";
const char error_2132_3B_NF[] PROGMEM = "Datenübertragung (Abgas) unterbrochen, Leitung überprüfen (California Vehicles only)";
const char error_2141_3B_MC[] PROGMEM = "Motorklopfen, Zündungsspätverstellung um 7,8 grad, Ladedruck reduziert (20 V), Kraftstoff mit zu niedriger Oktanzahl, zu hohe Kompression, Ladedruck zu hoch";
const char error_2141_NF[] PROGMEM = "Regelbereich überschritten, Motorklopfen, max Spätverstellung, Kraftstoff mit zu niedriger Oktanzahl, zu starke Kompression, Verkokung des Brennraums";
const char error_2142_3B[] PROGMEM = "Klopfsensor defekt, Leitung unterbrochen, Klopfsensor / Leitung überprüfen, bzw austauschen - Symptom: Zündung wird auf spät verstellt, hoher Kraftstoffverbrauch, Leistungsverlust, Ladedruck bleibt unter 1,3 bar";
const char error_2142_MC[] PROGMEM = "Sensor defekt, Leitungsunterbrechung, Sensor und Leitung überprüfen - Symptom: Zündspätverstellung, Leistungsverlust, erhöhter Verbrauch";
const char error_2142_NF[] PROGMEM = "Sensor defekt, Leitungsunterbrechung, Sensor und Leitung überprüfen - Symptom: Zündspätverstellung, Leistungsverlust, erhöhter Verbrauch";
const char error_2143_3B[] PROGMEM = "Klopfsensor Nr. 2, (Zylinder 3, 4, 5) Motorklopfen, Zündungsspätverstellung um 7,8 grad, Ladedruck reduziert (20 V), Kraftstoff mit zu niedriger Oktanzahl, zu hohe Kompression, Ladedruck zu hoch";
const char error_2143_MC[] PROGMEM = "Klopfsensor Nr. 2, (Zylinder 3, 4, 5) MC Motor mit 2 Klopfsensoren ab Fg. Nr. KN 038262, Motorklopfen, Zündungsspätverstellung um 7,8 grad. Kraftstoff mit zu niedriger Oktanzahl, zu hohe Kompression, Ladedruck zu hoch";
const char error_2144_3B[] PROGMEM = "Klopfsensor Nr. 2, (Zylinder 3, 4, 5) Klopfsensor defekt, Leitung unterbrochen, Klopfsensor / Leitung überprüfen, bzw austauschen, Symptom: Zündung wird auf spät verstellt, hoher Kraftstoffverbrauch, Leistungsverlust, Ladedruck bleibt unter 1,3 bar";
const char error_2144_MC[] PROGMEM = "Klopfsensor Nr. 2, (Zylinder 3, 4, 5) MC Motor mit 2 Klopfsensoren ab Fg. Nr. KN 038262, Klopfsensor defekt, Leitung unterbrochen, Klopfsensor / Leitung überprüfen, bzw austauschen, Symptom: Zündung wird auf spät verstellt, hoher Kraftstoffverbrauch, Leistungsverlust, Ladedruck bleibt unter 1,3 bar";
const char error_2212_3B[] PROGMEM = "Kurzschluss an +, oder an Masse, Feuchter, bzw korrodierter Steckkontakt, Drosselklappenpotentiometer Widerstand zw. Vollast/Leerlauf-Kontakt und mittlerem Anschluss prüfen.";
const char error_2214_3B[] PROGMEM = "Motor wurde über 7440 Umdrehungen gedreht (20 V Motor), Fahrer war schnell unterwegs.";
const char error_2214_MC[] PROGMEM = "(MC Motor mit 2 Klopfsensoren, ab MJ 89) - Motor wurde überdreht (Drehzahllimit 6700-6900 UPM bei MC Motor), Fahrer war wohl in Eile.";
const char error_2221_3B[] PROGMEM = "Druck im Ansaugrohr zu niedrig, zu hoch, Messdifferenz. Schlauch zum Drucksensor im Motorsteuergerät, fehlender, oder schadhafter Unterdruckschlauch -> Schlauch überprüfen - Symptom: Leistungsverlust";
const char error_2221_MC[] PROGMEM = "Druck im Ansaugrohr zu hoch oder zu niedrig, Ladedruckmessung fehlerhaft, grüner Schlauch zum Ladedrucksensor im Steuergerät, Schlauch undicht, abgerissen oder fehlend -> überprüfen bzw. erneuern - Symptom: Leistungsverlust";
const char error_2222_3B_MC[] PROGMEM = "Ladedrucksensor im Steuergerät defekt, Steuergerät austauschen, bzw reparieren";
const char error_2223_3B[] PROGMEM = "Kabel unterbrochen, oder Kurzschluss an +, oder an Masse - Symptom: Leistungsverlust, Ladedruck wird nicht erreicht";
const char error_2223_NF[] PROGMEM = "defekt -> Ausgangsspannung und Leitung überprüfen";
const char error_2224_3B[] PROGMEM = "Over boost (zu hoher Ladedruck) - Waste gate (Ladedruckregelventil), oder Druckschlauch, Waste Gate Ventil / Schlauch Problem";
const char error_2224_MC[] PROGMEM = "Ladedruck zu hoch - Overboost (MC Motor ab 1989) - Ladedruck überschreitet 1.6 bar bei MC Motor, Steuerung geht auf Notprogramm, Ladedruckregelventil (Wastegate) bzw. Druckschlauch, auf Lecks / Funktion überprüfen";
const char error_2231_3B_MC[] PROGMEM = "adaptive Leerlaufanpassung Werte außer Toleranz, Leerlaufsteuerventil verschmutzt, Falschluft.";
const char error_2232_NF[] PROGMEM = "Ausgangsspannung und Leitung überprüfen";
const char error_2233_NF[] PROGMEM = "falsche Spannung";
const char error_2234_3B[] PROGMEM = "Versorgungsspannung für Steuergerät zu hoch / zu niedrig";
const char error_2241_3B[] PROGMEM = "starkes Klopfsensor, Regelbereich überschritten, Symptom: reduzierter Ladedruck";
const char error_2312_3B_MC_NF[] PROGMEM = "Unterbrechung, oder Kurzschluss, Geber oder Kabel defekt - Symptom: Kaltstartschwierigkeiten, Leerlauf und Gasannahme in der Warmlaufphase schlecht";
const char error_2322_3B_MC[] PROGMEM = "Unterbrechung oder Kurzschluss, Geber oder Leitung defekt - Symptom: leichter Leistungsverlust, erhöhter Verbrauch";
const char error_2324_3B_MC[] PROGMEM = "Unterbrechung, oder Kurzschluss, Sensor defekt, System geht in den Notlauf"; 
const char error_2341_3B[] PROGMEM = "Lambda Sonde Regelbereich überschritten, Kraftstoffdruck zu niedrig, oder zu hoch, Leck im Auspuff, oder Ansaugtrakt, Defekt in der Zündung, Falschluft hinter dem Luftmassenmesser, Lambdasonde schadhaft - Symptom: CO Gehalt vor Kat ist unter 0.3% oder über 1.0%, Abgas viel zu mager, oder zu fett, Zündkerzen schadhaft";
const char error_2341_NF[] PROGMEM = "Regelbereich über-, bzw unterschritten";
const char error_2342_3B[] PROGMEM = "Leitung, oder Sonde schadhaft, Heizelement defekt, Leitung hat Kurzschluss an Masse, Benzintank leer, schadhaftes Bauteil in Zündung, Falschluft am Luftmassensensor, undichter Auspuff vor Katalysator, mangelhafter Massekontakt am Motor. Symptom: Abgaswerte außer Toleranz"; 
const char error_2342_MC[] PROGMEM = "Sonde bzw. Leitung defekt bzw. Sonde gealtert - Symptom: Abgaswerte zu hoch";
const char error_2411_NF[] PROGMEM = "(nur bei Exportfahrzeugen)";
const char error_2413_3B[] PROGMEM = "Adaptionslimit überschritten - Symptom: Gemisch zu fett, hoher Verbrauch"; 
const char error_3424_3B_MC[] PROGMEM = "Kurzschluss an +, oder Masse - Symptom: Kontrolleuchte Motorelektronik leuchtet NICHT bei Zündung ein";
const char error_4343_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss - Symptom: schlechtes Ansprechen auf Gemischanpassung, Benzingeruch, unruhiger Leerlauf"; 
const char error_4411_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss - Symptom: Motor läuft rauh, oder stirbt ab"; 
const char error_4412_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss - Symptom: Motor läuft rauh, oder stirbt ab"; 
const char error_4413_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss - Symptom: Motor läuft rauh, oder stirbt ab"; 
const char error_4414_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss - Symptom: Motor läuft rauh, oder stirbt ab"; 
const char error_4421_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss - Symptom: Motor läuft rauh, oder stirbt ab"; 
const char error_4431_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss, Symptom: LL-Drehzahl außer Toleranz, Motor kann im kalten Zustand ausgehen, unrunder Leerlauf, Anmerkung: wenn Motor im Notlauf, LL-Drehzahl zwischen 1100 and 1200/min"; 
const char error_4431_NF[] PROGMEM = "Stromaufnahme ist Maß für LL-Drehzahl"; 
const char error_4442_3B[] PROGMEM = "Leitungsunterbrechung, oder Kurzschluss - Symptom: Ladedruck zu niedrig, oder zu hoch, Fehlzündungen unter Vollast";
const char error_4444_3B_MC_NF[] PROGMEM = "keine Fehler";


#define BUZZER 3

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define REST      0


const unsigned char carBitmap [] PROGMEM = {
	0x1f, 0x00, 0x38, 0x80, 0x78, 0x40, 0xff, 0xf0, 0x8f, 0x10, 0x26, 0x40, 0x50, 0xa0, 0x20, 0x40
};


#endif