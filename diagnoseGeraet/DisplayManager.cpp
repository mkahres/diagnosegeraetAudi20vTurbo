#include "DisplayManager.h"
#include "Constants.h"

const int NUM_ENTRIES = 10;
int minValues[NUM_ENTRIES] = { 184, 20, 77, 121, 80,  123, 41, 123, 115, 35 };
int maxValues[NUM_ENTRIES] = { 215, 25, 83, 135, 125, 137, 55, 133, 141, 37 };

const uint8_t checkmark_bitmap[] PROGMEM = {
0x00, 0x01, 0x03, 0x06, 0x8c, 0xd8, 0x70, 0x20
};

const uint8_t x_bitmap[] PROGMEM = {
0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3
};

int errorTextWidth;
String longText;
bool initFaultMemory = false;

int xOffset = -64;

char buffer32[32];
char bufferRow2[40];

DisplayManager::DisplayManager()
{
}


const char msgHauptMenueWeiter[] PROGMEM = "< Hauptmenue > Weiter";
const char msgHauptMenueOk[] PROGMEM = "< Hauptmenue     > Ok";
const char msgHauptMenue[] PROGMEM = "< Hauptmenue";
const char msgAuswahlOk[] PROGMEM = "< Auswahl        > Ok";

const char msgVerbindung[] PROGMEM = "Ggf. Verbindung trennen,";
const char msgZuendung[] PROGMEM = "Zündung aus und nochmal";
const char msgVersuchen[] PROGMEM = "versuchen...";

void DisplayManager::showError(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* error)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, error);
  strcpy_P(buffer32, msgVerbindung);
  u8g2_for_adafruit_gfx.drawStr(0, 21, buffer32);
  strcpy_P(buffer32, msgZuendung);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgVersuchen);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.display();
}

const char msgDiagnosegeraet[] PROGMEM = "Diagnosegerät";
const char msg20VTurbo[] PROGMEM = "    20v Turbo";
const char msgCyber[] PROGMEM = "-Cyberdinosaurus Systems-";

void DisplayManager::displayStartScreen(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR14_tf);
  strcpy_P(buffer32, msgDiagnosegeraet);
  u8g2_for_adafruit_gfx.drawUTF8(0, 20, buffer32);
  strcpy_P(buffer32, msg20VTurbo);
  u8g2_for_adafruit_gfx.drawUTF8(0, 42, buffer32);  
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgCyber);
  u8g2_for_adafruit_gfx.drawUTF8(0, 62, buffer32);
  display.display();
}



void DisplayManager::displayProgramTypeChoice(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* programTypes[], int &programTypeIndex,
 bool blinkingOperation)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  int verticalCirclePos = 3 + 13 * programTypeIndex;
  display.fillCircle(3, verticalCirclePos, 3, SH110X_WHITE);
  u8g2_for_adafruit_gfx.drawUTF8(13, 8, programTypes[0]);
  u8g2_for_adafruit_gfx.drawUTF8(13, 21, programTypes[1]);
  if(!blinkingOperation)
  {
    u8g2_for_adafruit_gfx.drawUTF8(13, 34, programTypes[2]);
    u8g2_for_adafruit_gfx.drawUTF8(13, 47, programTypes[3]);
  }
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgAuswahlOk);
  display.print(buffer32);
  display.display();
}

void DisplayManager::displayProgramModeChoice(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, 
  char* programModes[], int &programModeIndex, char* programType, int &programTypeIndex, bool blinkingOperation)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  int verticalCirclePos = 16 + 13 * programModeIndex;
  display.fillCircle(3, verticalCirclePos, 3, SH110X_WHITE);
  u8g2_for_adafruit_gfx.drawUTF8(7, 8, programType);
  u8g2_for_adafruit_gfx.drawUTF8(13, 21, programModes[0]);
  if((!blinkingOperation && programTypeIndex != 2) || (blinkingOperation && programTypeIndex == 0))
  {
    u8g2_for_adafruit_gfx.drawUTF8(13, 34, programModes[1]);
  }
  if(!blinkingOperation && programTypeIndex == 0)
  {
    u8g2_for_adafruit_gfx.drawUTF8(13, 47, programModes[2]);
  }  
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgAuswahlOk);
  display.print(buffer32);
  display.display();
}

void DisplayManager::displaySensorDetails(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int index, int value)
{
  switch (index) 
  {
  case 1:
    displaySensorDetails1(display, u8g2_for_adafruit_gfx, value);
    break;
  case 2:
    displaySensorDetails2(display, u8g2_for_adafruit_gfx, value);
    break;
  case 3:
    displaySensorDetails3(display, u8g2_for_adafruit_gfx, value);
    break;
  case 4:
    displaySensorDetails4(display, u8g2_for_adafruit_gfx, value);
    break;
  case 5:
    displaySensorDetails5(display, u8g2_for_adafruit_gfx, value);
    break;
  case 6:
    displaySensorDetails6(display, u8g2_for_adafruit_gfx, value);
    break;
  case 7:
    displaySensorDetails7(display, u8g2_for_adafruit_gfx, value);
    break;
  case 8:
    displaySensorDetails8(display, u8g2_for_adafruit_gfx, value);
    break;
  case 9:
    displaySensorDetails9(display, u8g2_for_adafruit_gfx, value);
    break;     
  case 10:
    displaySensorDetails10(display, u8g2_for_adafruit_gfx, value);
    break;   
  }      
}



const char msgUeberschrift1[] PROGMEM = "1: Temperatur Kühlerwasser";
const char msgSpec1[] PROGMEM = "Spec:184-215 (=85-105°C)";
const char msgZusatz1[] PROGMEM = "Voraussetzung f. Sollwerte";

void DisplayManager::displaySensorDetails1(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift1);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, getFormattedEngineTemp(value, bufferRow2)); 
  strcpy_P(buffer32, msgSpec1);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz1);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);  
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift2[] PROGMEM = "2: Motorlast";
const char msgSpec2[] PROGMEM = "Spec:20-25";
const char msgZusatz2[] PROGMEM = "ohne Klima u. Verbraucher";

void DisplayManager::displaySensorDetails2(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  sprintf(bufferRow2, "Wert:%d", value);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift2);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, bufferRow2);
  strcpy_P(buffer32, msgSpec2);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz2);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift3[] PROGMEM = "3: Motordrehzahl";
const char msgSpec3[] PROGMEM = "Spec:77-83(= 770-830)";
const char msgZusatz3[] PROGMEM = "ohne Klima u. Verbraucher";

void DisplayManager::displaySensorDetails3(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  sprintf(bufferRow2, "Wert:%d (= %d U/min)", value, value * 10);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift3);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, bufferRow2);
  strcpy_P(buffer32, msgSpec3);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz3);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift4[] PROGMEM = "4: LL-Stab. Arbeitsbereich";
const char msgSpec4[] PROGMEM = "Spec:121-135";
const char msgZusatz4[] PROGMEM = "Luftmengenänderung N71";

void DisplayManager::displaySensorDetails4(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  sprintf(bufferRow2, "Wert:%d", value);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift4);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, bufferRow2);
  strcpy_P(buffer32, msgSpec4);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz4);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift5[] PROGMEM = "5: LL-Stab. Nullpunkt";
const char msgSpec5[] PROGMEM = "Spec:80-125";
const char msgZusatz5[] PROGMEM = "Interner Rechenwert";

void DisplayManager::displaySensorDetails5(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  sprintf(bufferRow2, "Wert:%d", value);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift5);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, bufferRow2);
  strcpy_P(buffer32, msgSpec5);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz5);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift6[] PROGMEM = "6: LL-Stab. Lastanpassung";
const char msgSpec6[] PROGMEM = "Spec:123-137";
const char msgZusatz6[] PROGMEM = "Luftmengenänderung N71";

void DisplayManager::displaySensorDetails6(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  sprintf(bufferRow2, "Wert:%d", value);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift6);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, bufferRow2);
  strcpy_P(buffer32, msgSpec6);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz6);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift7[] PROGMEM = "7: LL-Stab. Kennl.-Strg.";
const char msgSpec7[] PROGMEM = "Spec:41-55";
const char msgZusatz7[] PROGMEM = "Interner Rechenwert";

void DisplayManager::displaySensorDetails7(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  sprintf(bufferRow2, "Wert:%d", value);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift7);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, bufferRow2);
  strcpy_P(buffer32, msgSpec7);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz7);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift8[] PROGMEM = "8: Lambdaregelung aktiv";
const char msgSpec8[] PROGMEM = "Spec:123-133 (0,5-0,9V%)";
const char msgZusatz8[] PROGMEM = "Nach ca. 1.5 min.";

void DisplayManager::displaySensorDetails8(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift8);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, getLambdaStatus(value, bufferRow2));  
  strcpy_P(buffer32, msgSpec8);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz8);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift9[] PROGMEM = "9: Lambdareg. Anpassung";
const char msgSpec9[] PROGMEM = "Spec:115-141";
const char msgZusatz9[] PROGMEM = "hoch -> Runde fahren";

void DisplayManager::displaySensorDetails9(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  sprintf(bufferRow2, "Wert:%d", value);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift9);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, bufferRow2);  
  strcpy_P(buffer32, msgSpec9);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);  
  strcpy_P(buffer32, msgZusatz9);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}

const char msgUeberschrift10[] PROGMEM = "10: Zündwinkel";
const char msgSpec10[] PROGMEM = "Spec:35-37(8-12° v.OT)";
const char msgZusatz10[] PROGMEM = "von J220 berechnet";

void DisplayManager::displaySensorDetails10(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgUeberschrift10);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, getIgnitionAngle(value, bufferRow2));
  strcpy_P(buffer32, msgSpec10);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZusatz10);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueWeiter);
  display.print(buffer32);
  display.display();
}


const char msgLadeSensordaten[] PROGMEM = "Lade Sensordaten...";

void DisplayManager::showLoadSensorData(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgLadeSensordaten);
  u8g2_for_adafruit_gfx.drawUTF8(12, 25, buffer32);
  display.display();
}

const char msgStandGespeichert[] PROGMEM = "Stand gespeichert!";

void DisplayManager::showSaveSensorData(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgStandGespeichert);
  u8g2_for_adafruit_gfx.drawUTF8(12, 25, buffer32);
  display.display();
}

void DisplayManager::displaySensorDataFinal(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int* list, bool continuously)
{  

  char** stringArray = intArrayToStringArray(list, 10);
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, "1:");
  u8g2_for_adafruit_gfx.drawUTF8(10, 8, stringArray[0]);
  display.drawBitmap(30, 0, getStatusBitmap(1, list[0]), 8, 8, SH110X_WHITE);
  u8g2_for_adafruit_gfx.drawUTF8(45, 8, "2:");
  u8g2_for_adafruit_gfx.drawUTF8(55, 8, stringArray[1]);
  display.drawBitmap(74, 0, getStatusBitmap(2, list[1]), 8, 8, SH110X_WHITE);
  u8g2_for_adafruit_gfx.drawUTF8(90, 8, "3:");
  u8g2_for_adafruit_gfx.drawUTF8(100, 8, stringArray[2]);
  display.drawBitmap(119, 0, getStatusBitmap(3, list[2]), 8, 8, SH110X_WHITE);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, "4:");
  u8g2_for_adafruit_gfx.drawUTF8(10, 21, stringArray[3]);
  display.drawBitmap(30, 13, getStatusBitmap(4, list[3]), 8, 8, SH110X_WHITE);
  u8g2_for_adafruit_gfx.drawUTF8(45, 21, "5:");
  u8g2_for_adafruit_gfx.drawUTF8(55, 21, stringArray[4]);
  display.drawBitmap(74, 13, getStatusBitmap(5, list[4]), 8, 8, SH110X_WHITE);
  u8g2_for_adafruit_gfx.drawUTF8(90, 21, "6:");
  u8g2_for_adafruit_gfx.drawUTF8(100, 21, stringArray[5]);
  display.drawBitmap(119, 13, getStatusBitmap(6, list[5]), 8, 8, SH110X_WHITE);  
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, "7:");
  u8g2_for_adafruit_gfx.drawUTF8(10, 34, stringArray[6]);
  display.drawBitmap(30, 26, getStatusBitmap(7, list[6]), 8, 8, SH110X_WHITE);  
  u8g2_for_adafruit_gfx.drawUTF8(45, 34, "8:");
  u8g2_for_adafruit_gfx.drawUTF8(55, 34, stringArray[7]);
  display.drawBitmap(74, 26, getStatusBitmap(8, list[7]), 8, 8, SH110X_WHITE);  
  u8g2_for_adafruit_gfx.drawUTF8(90, 34, "9:");
  u8g2_for_adafruit_gfx.drawUTF8(100, 34, stringArray[8]);
  display.drawBitmap(119, 26, getStatusBitmap(9, list[8]), 8, 8, SH110X_WHITE);  
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, "10:");
  u8g2_for_adafruit_gfx.drawUTF8(16, 47, stringArray[9]);
  display.drawBitmap(30, 39, getStatusBitmap(10, list[9]), 8, 8, SH110X_WHITE);  

  display.setCursor(0, 55);

  if(continuously)
  {
    strcpy_P(buffer32, msgHauptMenueOk);
    display.print(buffer32);
  }
  else
  {
    strcpy_P(buffer32, msgHauptMenueWeiter);
    display.print(buffer32);
  }
  display.display();
}

const char msgFehlerspeicherAusblinken[] PROGMEM = "Fehlerspeicher ausblinken";
const char msgFehlerspeicher[] PROGMEM = "Fehlerspeicher";
const char msgZuendungAnMotorAn[] PROGMEM = "- Zündung an, Motor an!";

void DisplayManager::prepareFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, boolean blink)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  if(blink)
  {
    strcpy_P(buffer32, msgFehlerspeicherAusblinken);
    u8g2_for_adafruit_gfx.drawUTF8(7, 8, buffer32);
  }
  else
  {
    strcpy_P(buffer32, msgFehlerspeicher);
    u8g2_for_adafruit_gfx.drawUTF8(7, 8, buffer32);
  }
  strcpy_P(buffer32, msgZuendungAnMotorAn);
  u8g2_for_adafruit_gfx.drawUTF8(0, 30, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueOk);
  display.print(buffer32);
  display.display();
}  


const char msgLeseFehlerspeicher[] PROGMEM = "Lese Fehlerspeicher...";

void DisplayManager::showLoadFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgLeseFehlerspeicher);
  u8g2_for_adafruit_gfx.drawUTF8(12, 25, buffer32);
  display.display();
}

const char msgCode[] PROGMEM = "Code:";

void DisplayManager::showAnalyzeFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* actualCode)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgFehlerspeicherAusblinken);
  u8g2_for_adafruit_gfx.drawUTF8(7, 8, buffer32);
  strcpy_P(buffer32, msgCode);
  u8g2_for_adafruit_gfx.drawUTF8(0, 30, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(30, 30, actualCode);
  display.display();
}  

const char msgAufPedal[] PROGMEM = "optional: auf Pedal tippen";
const char msgAutomat[] PROGMEM = "-> automat. nächstes Ventil";
const char msgSollte[] PROGMEM = "Sollte hörbares Geräusch";
const char msgMachen[] PROGMEM = "machen";
const char msgWeiterschalten[] PROGMEM = "     > Weiterschalten";

void DisplayManager::showAnalyzeActuators(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* actualCode, ErrorStructure errorStructure,
 boolean kLine)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgCode);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  u8g2_for_adafruit_gfx.drawUTF8(30, 8, actualCode);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, errorStructure.shortText);
  if(!kLine && (errorStructure.errorCode == 4411 || errorStructure.errorCode == 4412 || errorStructure.errorCode == 4414 
    || errorStructure.errorCode == 4421 || errorStructure.errorCode == 4413))
  {
    strcpy_P(buffer32, msgAufPedal);
    u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
    strcpy_P(buffer32, msgAutomat);
    u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  }
  else if(errorStructure.errorCode > 0)
  {
    strcpy_P(buffer32, msgSollte);
    u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
    strcpy_P(buffer32, msgMachen);
    u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);   
  }
  
  display.setCursor(0, 55);
  if(kLine)
  {
    strcpy_P(buffer32, msgHauptMenueWeiter);
    display.print(buffer32);
  }
  else
  {
    strcpy_P(buffer32, msgWeiterschalten);
    display.print(buffer32);
  }
  display.display();
}  

const char msgStellglieddiagnoseFertig[] PROGMEM = "Stellglieddiagnose fertig";
const char msgZumWiederholen[] PROGMEM = "Zum Wiederholen Zündung";
const char msgFuerCa20[] PROGMEM = "für ca. 20 Sekunden aus";

void DisplayManager::showActuatorFinished(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  strcpy_P(buffer32, msgStellglieddiagnoseFertig);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  strcpy_P(buffer32, msgZumWiederholen);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, buffer32);
  strcpy_P(buffer32, msgFuerCa20);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenue);
  display.print(buffer32);
  display.display();
}

const char msgNichtMoeglich[] PROGMEM = "nicht möglich. Evtl. wurde";
const char msgEsSchon[] PROGMEM = "es schon ausgeführt. Bitte";
const char msgZuendungAusEin[] PROGMEM = "Zündung aus / ein.";

void DisplayManager::showFaultMemoryNotPossible(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  strcpy_P(buffer32, msgFehlerspeicherAusblinken);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  strcpy_P(buffer32, msgNichtMoeglich);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, buffer32);
  strcpy_P(buffer32, msgEsSchon);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZuendungAusEin);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenue);
  display.print(buffer32);
  display.display();
}

const char msgStellglieddiagnoseNicht[] PROGMEM = "Stellglieddiagnose nicht";
const char msgMoeglichEvtl[] PROGMEM = "möglich. Evtl. wurde sie";
const char msgSchonAusgefuehrt[] PROGMEM = "schon ausgeführt. Bitte";

void DisplayManager::showActuatorsNotPossible(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  strcpy_P(buffer32, msgStellglieddiagnoseNicht);
  u8g2_for_adafruit_gfx.drawUTF8(0, 8, buffer32);
  strcpy_P(buffer32, msgMoeglichEvtl);
  u8g2_for_adafruit_gfx.drawUTF8(0, 21, buffer32);
  strcpy_P(buffer32, msgSchonAusgefuehrt);
  u8g2_for_adafruit_gfx.drawUTF8(0, 34, buffer32);
  strcpy_P(buffer32, msgZuendungAusEin);
  u8g2_for_adafruit_gfx.drawUTF8(0, 47, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenue);
  display.print(buffer32);
  display.display();
}

const char msgStellglieddiagnose[] PROGMEM = "Stellglieddiagnose";
const char msgJetztZuendungAus[] PROGMEM = "- Jetzt Zündung aus!";

void DisplayManager::prepareActuatorDiagnostics1(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgStellglieddiagnose);
  u8g2_for_adafruit_gfx.drawUTF8(7, 8, buffer32);
  strcpy_P(buffer32, msgJetztZuendungAus);
  u8g2_for_adafruit_gfx.drawUTF8(0, 30, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueOk);
  display.print(buffer32);
  display.display();
} 


const char msgZuendungAnMotorAus[] PROGMEM = "- Zündung an, Motor aus!";

void DisplayManager::prepareActuatorDiagnostics2(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgStellglieddiagnose);
  u8g2_for_adafruit_gfx.drawUTF8(7, 8, buffer32);
  strcpy_P(buffer32, msgZuendungAnMotorAus);
  u8g2_for_adafruit_gfx.drawUTF8(0, 30, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgHauptMenueOk);
  display.print(buffer32);
  display.display();
}  

const char msgWirdGestartet[] PROGMEM = "wird gestartet...";

void DisplayManager::showWaitingActuatorDiagnostics(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgStellglieddiagnose);
  u8g2_for_adafruit_gfx.drawUTF8(7, 8, buffer32);
  strcpy_P(buffer32, msgWirdGestartet);
  u8g2_for_adafruit_gfx.drawUTF8(12, 30, buffer32);
  display.display();
} 

const char msgGefundeneFehler[] PROGMEM = "Gefundene Fehler:";
const char msgKeineFehler[] PROGMEM = "Keine Fehler!";

void DisplayManager::displayFaultMemoryOverview(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int* list, int size)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgGefundeneFehler);
  u8g2_for_adafruit_gfx.drawUTF8(7, 8, buffer32);
  if(size == 0)
  {
    strcpy_P(buffer32, msgKeineFehler);
    u8g2_for_adafruit_gfx.drawUTF8(0, 27, buffer32);
  }
  else
  {
    printFixedIntList(u8g2_for_adafruit_gfx, list, size, 0, 21);
  }
  display.setCursor(0, 55);
  if(size == 0)
  {
    strcpy_P(buffer32, msgHauptMenue);
    display.print(buffer32);
  }
  else
  {    
    strcpy_P(buffer32, msgHauptMenueWeiter);
    display.print(buffer32);
  }
  display.display();
}  


const char msgWirdGeloescht[] PROGMEM = "wird gelöscht!";

void DisplayManager::showDeleteFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgFehlerspeicher);
  u8g2_for_adafruit_gfx.drawUTF8(25, 18, buffer32);
  strcpy_P(buffer32, msgWirdGeloescht);
  u8g2_for_adafruit_gfx.drawUTF8(25, 36, buffer32);
  display.display();
}

const char msgFehlerspeicherLoeschen[] PROGMEM = "Fehlerspeicher löschen?";
const char msgJaNein[] PROGMEM = "< Ja           > Nein";

void DisplayManager::showDeleteFaultMemoryQuestion(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
  strcpy_P(buffer32, msgFehlerspeicherLoeschen);
  u8g2_for_adafruit_gfx.drawUTF8(0, 18, buffer32);
  display.setCursor(0, 55);
  strcpy_P(buffer32, msgJaNein);
  display.print(buffer32);
  display.display();
}

const char msgDatentransfer[] PROGMEM = "Datentransfer";
const char msgBereit[] PROGMEM = "- bereit -";
const char msgAktiv[] PROGMEM = "- aktiv -";
const char msgPausiert[] PROGMEM = "- pausiert -";

void DisplayManager::showDatentransferBereit(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR14_tf); 
  strcpy_P(buffer32, msgDatentransfer);
  u8g2_for_adafruit_gfx.drawUTF8(5, 24, buffer32);
  strcpy_P(buffer32, msgBereit);
  u8g2_for_adafruit_gfx.drawUTF8(32, 50, buffer32);
  display.display();
}

void DisplayManager::showDatentransferAktiv(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR14_tf); 
  strcpy_P(buffer32, msgDatentransfer);
  u8g2_for_adafruit_gfx.drawUTF8(5, 24, buffer32);
  strcpy_P(buffer32, msgAktiv);
  u8g2_for_adafruit_gfx.drawUTF8(32, 50, buffer32);
  display.display();
}

void DisplayManager::showDatentransferPausiert(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx)
{
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR14_tf); 
  strcpy_P(buffer32, msgDatentransfer);
  u8g2_for_adafruit_gfx.drawUTF8(5, 24, buffer32);
  strcpy_P(buffer32, msgPausiert);
  u8g2_for_adafruit_gfx.drawUTF8(17, 50, buffer32);
  display.display();
}

void DisplayManager::displayFaultMemoryDetails(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, 
  int code, int sizeFaultMemory, int indexFaultMemory, ErrorManager &errorManager)
{  
  ErrorStructure match;

  if(initFaultMemory)
  {
    initFaultMemory = false;
    display.clearDisplay();
    xOffset = -64;
    match = errorManager.findErrorStructureByErrorCode(code, ENGINE);
    longText = readProgmemString(match.longText);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_helvR08_tf); 
    char bufferRow1[40];
    sprintf(bufferRow1, "%d / %d - Fehler %d", indexFaultMemory, sizeFaultMemory, code);
    u8g2_for_adafruit_gfx.drawUTF8(0, 8, bufferRow1);
    u8g2_for_adafruit_gfx.drawUTF8(0, 21, match.shortText);
    display.setCursor(0, 55);
    strcpy_P(buffer32, msgHauptMenueWeiter);
    display.print(buffer32);
  }
  display.fillRect(0, 32, 128, 10, SH110X_BLACK); 
  scrollText(display, u8g2_for_adafruit_gfx, 40);
  display.display();
}

void DisplayManager::initFaultMemoryDetails()
{
  initFaultMemory = true;
}

void DisplayManager::printFixedIntList(U8G2_FOR_ADAFRUIT_GFX &u8g2, int* values, int count, int startX, int startY) {
  const int charWidth = 6;       // Approx. for helvR08
  const int numWidth = 5 * charWidth; // 4 digits + comma/space
  const int maxWidth = 128;
  const int maxPerLine = maxWidth / numWidth; // should be 4
  const int lineHeight = 13;
  const int maxLines = 3;

  u8g2.setFont(u8g2_font_helvR08_tf);

  for (int i = 0; i < count && i < maxPerLine * maxLines; i++) {
    int line = i / maxPerLine;
    int col = i % maxPerLine;
    int x = startX + col * numWidth;
    int y = startY + line * lineHeight;

    char part[8];
    sprintf(part, "%04d%s", values[i], (i < count - 1 ? "," : ""));

    u8g2.setCursor(x, y);
    u8g2.print(part);
  }
}

char** DisplayManager::intArrayToStringArray(int* inputArray, int size) 
{
  // Each int can be up to 5 digits + null terminator
  static char strStorage[20][6];  // Max 20 entries, each max 6 chars
  static char* result[20];        // Array of pointers to the strings

  if (size > 20) size = 20;       // Safety limit

  for (int i = 0; i < size; i++) {
    itoa(inputArray[i], strStorage[i], 10);  // Convert to string
    result[i] = strStorage[i];               // Store pointer
  }

  return result;
}

const uint8_t* DisplayManager::getStatusBitmap(int index, int value) 
{
  if (index < 1 || index > NUM_ENTRIES) return "?";  // Invalid index

  int minVal = minValues[index - 1];
  int maxVal = maxValues[index - 1];

  if (value >= minVal && value <= maxVal) 
  {
    return checkmark_bitmap;
  } 
  else 
  {
    return x_bitmap;
  }
}  

int DisplayManager::getMin(int index) {
  return (index >= 1 && index <= NUM_ENTRIES) ? minValues[index - 1] : -1;
}

int DisplayManager::getMax(int index) {
  return (index >= 1 && index <= NUM_ENTRIES) ? maxValues[index - 1] : -1;
}

float DisplayManager::convertSensorLambdaToO2Vol(int sensorValue) {
  // 123 → 0.5 Vol.%
  // 133 → 0.9 Vol.%
  return map(sensorValue, 123, 133, 50, 90) / 100.0;  // 0.50 - 0.90
}

const char* DisplayManager::getLambdaStatus(int raw, char* buffer) {
  if (raw < 123) {
    sprintf(buffer, "Wert: %d (Gemisch fett)", raw);
  } else if (raw > 133) {
    sprintf(buffer, "Wert: %d (Gemisch mager)", raw);
  } else {
    // Integer-basierte Berechnung: 0.5–0.9 → 5–9
    int volX10 = 5 + (raw - 123) * 4 / 10;  // 4 = 0.4 * 10
    sprintf(buffer, "Wert: %d (≈ 0.%d Vol.%%)", raw, volX10);
  }

  return buffer;
}

const char* DisplayManager::getFormattedEngineTemp(int rawValue, char* buffer) {
  if (rawValue < 184) {
    sprintf(buffer, "Wert: %d (Motor kalt)", rawValue);
  } else if (rawValue > 215) {
    sprintf(buffer, "Wert: %d (Motor überhitzt)", rawValue);
  } else {
    float tempC = 85.0 + (rawValue - 184) * 20.0 / 31.0;
    char tempStr[8];
    dtostrf(tempC, 4, 0, tempStr);  // rounded, no decimal

    sprintf(buffer, "Wert: %d (=%s°C)", rawValue, tempStr);
  }
  return buffer;
}

const char* DisplayManager::getIgnitionAngle(int rawValue, char* buffer) {
  if (rawValue < 35) {
    sprintf(buffer, "Wert: %d (Zündung früh?)", rawValue);
  } else if (rawValue > 37) {
    sprintf(buffer, "Wert: %d (Zündung spät?)", rawValue);
  } else {
    int degX10 = 80 + (rawValue - 35) * (120 - 80) / (37 - 35);  // Zehntelgrad
    sprintf(buffer, "Wert: %d (≈ %d.%d° v.OT)", rawValue, degX10 / 10, degX10 % 10);
  }
  return buffer;
}

char* DisplayManager::progmemToRam(const char* progmemStr) {
  static char buffer[500];  // Achtung: max. Länge! (anpassen wenn nötig)
  strncpy_P(buffer, progmemStr, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';  // Sicherheit: Nullterminierung
  return buffer;
}

String DisplayManager::readProgmemString(const char* ptr) {
  if (ptr == nullptr) {
    Serial.println("readProgmemString: nullptr empfangen");
    return "";
  }

  size_t len = strlen_P(ptr);
  if (len == 0 || len > 499) {
    Serial.print("readProgmemString: ungültige Länge: ");
    Serial.println(len);
    return "";
  }

  char buffer[500];
  strncpy_P(buffer, ptr, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  return String(buffer);
}

void DisplayManager::scrollText(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2, int y)
{
  int currentPixel = 0;
  int currentX = 0;
  int maxWidth = 128;

  String visibleText = "";
  bool started = false;

  // Zeichenweise UTF-8-sicher iterieren
  for (int i = 0; i < longText.length(); ) {
    // UTF-8 korrekt extrahieren
    char ch[5] = {0};
    int len = 0;
    unsigned char c = longText[i];

    if (c < 0x80) len = 1;
    else if ((c & 0xE0) == 0xC0) len = 2;
    else if ((c & 0xF0) == 0xE0) len = 3;
    else if ((c & 0xF8) == 0xF0) len = 4;

    longText.substring(i, i + len).toCharArray(ch, 5);
    int cw = u8g2.getUTF8Width(ch);

    currentPixel += cw;

    if (!started && currentPixel >= xOffset) {
      started = true;
      currentX = currentPixel - cw; // tatsächlicher X-Start des sichtbaren Texts
    }

    if (started) {
      visibleText += ch;
      if (currentPixel - xOffset >= maxWidth) break; // sichtbar genug
    }

    i += len;
  }

  // Text zeichnen mit korrekt berechnetem X-Offset
  u8g2.setCursor(-(xOffset - currentX), y);
  u8g2.print(visibleText);
 // display.display();

  // Scroll weiter
  xOffset+=2;
  int totalWidth = u8g2.getUTF8Width(longText.c_str());
  if (xOffset >= totalWidth) xOffset = -64;
}