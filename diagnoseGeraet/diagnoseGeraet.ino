#include "Constants.h"
#include "configuration.h"
#include "DisplayManager.h"
#include "MusicManager.h"
#include "MemoryManager.h"
#include "KLineManager.h"
#include "ErrorManager.h"
#include "CrashyBird.h"

DisplayManager displayManager;
MusicManager musicManager;
MemoryManager memoryManager;
KLineManager kLineManager;
ErrorManager errorManager;
CrashyBird crashyBird;

//for memory check
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

long currentMillis = 0;
long measureTime = 0;
int voltageBoundary = 7;
boolean isBlinking = false;
long lastBlinkingChange = 0;
long lastInterval = 0;
long voltageTime = 0;

uint8_t START_SCREEN = 0;
uint8_t SELECT_OPERATION = 1;
uint8_t SELECT_MODE = 2;
uint8_t LOAD_FAULT_MEMORY_FAST = 7;
uint8_t LOAD_FAULT_MEMORY_BLINK = 8;
uint8_t LOAD_SENSOR_DATA = 10;
uint8_t PREPARE_FAULT_MEMORY_FAST = 11;
uint8_t PREPARE_FAULT_MEMORY_BLINK = 12;
uint8_t PREPARE_ACTUATOR_DIAGNOSTICS_1 = 13;
uint8_t PREPARE_ACTUATOR_DIAGNOSTICS_2 = 14;
uint8_t EXECUTE_FAULT_MEMORY_FAST = 15;
uint8_t EXECUTE_FAULT_MEMORY_BLINK = 16;
uint8_t ANALYZE_FAULT_MEMORY_BLINK = 17;
uint8_t EXECUTE_ACTUATOR_DIAGNOSTICS = 18;
uint8_t ANALYZE_ACTUATOR_DIAGNOSTICS = 19;
uint8_t EXECUTE_SENSOR_DATA = 20;
uint8_t SHOW_FAULT_MEMORY = 21;
uint8_t SHOW_SENSOR_DATA_TEMP = 22;
uint8_t SHOW_SENSOR_DATA_FINAL = 23;
uint8_t DELETE_FAULT_MEMORY_FAST = 24;
uint8_t ANALYZE_ACTUATOR_FINISHED = 25;
uint8_t ANALYZE_ACTUATOR_NOT_POSSIBLE = 26;
uint8_t ANALYZE_FAULT_MEMORY_NOT_POSSIBLE = 27;
uint8_t DATA_TO_SERIAL = 28;
uint8_t SELECT_OPERATION_BLINK = 29;
uint8_t PREPARE_ACTUATOR_DIAGNOSTICS_KLINE = 30;
uint8_t EXECUTE_ACTUATOR_DIAGNOSTICS_KLINE = 31;
uint8_t CRASHY_BIRD = 99;

uint8_t phase = START_SCREEN;

#define MAX_SENSORS 10
int sensorData[MAX_SENSORS];
size_t sensorCount = 0;

int indexSensorData = 0;

#define MAX_FAULTS 20
int faultMemory[MAX_FAULTS];
size_t faultCount = 0;
int indexFaultMemory = 0;
int faultsVAGCodes[MAX_FAULTS];
int codeCount = 0;

unsigned long lastChangeTime = 0;
boolean lastState = false;
int codeForDisplay;
int codeActuator = -1;
int counter2500ms = 0;

int blinkCount = 0;
int digitIndex = 0;
int digits[4] = {0, 0, 0, 0};
boolean ignoreFirstLongBreak = true;
boolean faultMemoryBlinkFinished = false;
boolean actuatorBlinkFinished = false;
char displayBufferBlink[8] = "?-?-?-?";
char bufferCode[10];
char* unknownCode = "?-?-?-?";
boolean noBlinkingFor12Seconds = false;

ErrorStructure errorStructure;
ErrorStructure errorStructureEmpty;

boolean newPhase = false;
boolean buttonLeftPressed = false;
boolean buttonRightPressed = false;

const int ledPin = 4; 

const char* programTypes[] = {"Fehlerspeicher", "Sensordaten", "Stellglieddiagnose", "Ausblinken"};
const char* programTypesBlink[] = {"Fehlerspeicher ausblinken", "Stellglieddiagnose (blink)"};
int programTypeIndex = 0;
int programTypeIndexBlink = 0;
bool blinkingOperation = false;

const char* programModes[] = {"Ausführen", "Letztes Ergebnis"};
const char* programModesOnlyExecute[] = {"Ausführen"};
const char* programModesWithDelete[] = {"Ausführen", "Letztes Ergebnis", "Ecu-Speicher löschen"};
int programModeIndex = 0;

boolean bothButtonsPressed = false;

char inputStringPython[32];  
byte inputIndex = 0;
bool stringPythonCommandComplete = false;
bool isSendingToPython = false;     
unsigned long sendIntervalTestdaten = 200; 
unsigned long lastSendTimeTestdaten = 0;

void setup(void) 
{   
  Serial.begin (115200);

  pinMode(PIN_RELAY, OUTPUT);
  pinMode(TX_pin, OUTPUT);
  setPinState(HIGH);

  display.begin(I2C_ADDRESS, true);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setTextWrap(false);
  
  u8g2_for_adafruit_gfx.begin(display);  

  errorStructureEmpty.shortText[0] = '\0'; 
  
  randomSeed(analogRead(0));

  Serial.println("READY");
} 

void loop(void)
{  
       
      currentMillis = millis();

      if(phase != CRASHY_BIRD)
      {
        //auf Python Programm reagieren
        while (Serial.available()) {
          char inChar = (char)Serial.read();

          if (inChar == '>') {
            inputStringPython[inputIndex++] = inChar;
            inputStringPython[inputIndex] = '\0';  // Null-terminieren
            stringPythonCommandComplete = true;
          }
          else if (inputIndex < sizeof(inputStringPython) - 1) {
            inputStringPython[inputIndex++] = inChar;
          }
          else {
            // Zu lang -> zurücksetzen
            inputIndex = 0;
            inputStringPython[0] = '\0';
          }
        }
      }
      if (stringPythonCommandComplete) {
        handlePythonCommand(inputStringPython);  // C-String Übergabe
        inputStringPython[0] = '\0';             // Zurücksetzen
        inputIndex = 0;
        stringPythonCommandComplete = false;
      }
      if(phase == DATA_TO_SERIAL)
      {
        if(isSendingToPython)
        {
          sendSensorDataViaSerial();
        }
      }
      // zum Testen
      /*
      if (isSendingToPython && currentMillis - lastSendTimeTestdaten >= sendIntervalTestdaten) 
      {
        lastSendTimeTestdaten = currentMillis;
        sendeZufallswerte();
      }    
      */
      else if(phase == START_SCREEN)
      {
        displayManager.displayStartScreen(display, u8g2_for_adafruit_gfx);
        musicManager.playStarWars();        
        phase = SELECT_OPERATION;   
      }
      else if(phase == CRASHY_BIRD)
      {
        bothButtonsPressed = false;
        crashyBird.crashyBird(display, u8g2_for_adafruit_gfx);
        if(digitalRead(BUTTON_LEFT_PIN) == HIGH && digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        {
          if(!bothButtonsPressed)
          {
            bothButtonsPressed = true;
            phase = SELECT_OPERATION;
            delay(1000);
          }
          else
          {
            bothButtonsPressed = false;
          }          
        }        
      }
      else if(phase == SELECT_OPERATION)
      {
        if(digitalRead(BUTTON_LEFT_PIN) == HIGH && digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        {
          if(!bothButtonsPressed)
          {
            bothButtonsPressed = true;
            phase = CRASHY_BIRD;
          }
          else
          {
            bothButtonsPressed = false;
          }          
        }
        else
        {
          if (digitalRead(BUTTON_LEFT_PIN) == HIGH) 
          { 
            if (!buttonLeftPressed) 
            { 
              tone(BUZZER, NOTE_D5, 200);
              buttonLeftPressed = true;
              programTypeIndex = (programTypeIndex + 1) % 4;
            }
          } 
          else 
          {
            buttonLeftPressed = false; 
          }
          if(programTypeIndex == 3)
          {
            manageRightButtonForNextPhase(SELECT_OPERATION_BLINK);
          }
          else
          {
            manageRightButtonForNextPhase(SELECT_MODE);
          }          
          displayManager.displayProgramTypeChoice(display, u8g2_for_adafruit_gfx, programTypes, programTypeIndex, blinkingOperation);
        }
      }    
      else if(phase == SELECT_OPERATION_BLINK)
      {
        if (digitalRead(BUTTON_LEFT_PIN) == HIGH) 
        { 
          if (!buttonLeftPressed) 
          { 
            tone(BUZZER, NOTE_D5, 200);
            buttonLeftPressed = true;
            programTypeIndexBlink = (programTypeIndexBlink + 1) % 2;
          }
        } 
        else 
        {
          buttonLeftPressed = false; 
        }
        manageRightButtonForNextPhase(SELECT_MODE);
        displayManager.displayProgramTypeChoice(display, u8g2_for_adafruit_gfx, programTypesBlink, programTypeIndexBlink, blinkingOperation);
      }         
      else if(phase == SELECT_MODE)
      {
        if (digitalRead(BUTTON_LEFT_PIN) == HIGH) 
        { 
          if (!buttonLeftPressed) 
          { 
            tone(BUZZER, NOTE_D5, 200);
            buttonLeftPressed = true;
            if(!blinkingOperation && programTypeIndex == 0)
            {
              programModeIndex = (programModeIndex + 1) % 3;
            }
            else if(!blinkingOperation && programTypeIndex == 2)
            {
              programModeIndex = (programModeIndex + 1) % 1;
            }
            else if(!blinkingOperation)
            {
              programModeIndex = (programModeIndex + 1) % 2;
            }
            else if(blinkingOperation && programTypeIndexBlink == 0)
            {
              programModeIndex = (programModeIndex + 1) % 2;
            }
            else if(blinkingOperation && programTypeIndexBlink == 1)
            {
              programModeIndex = (programModeIndex + 1) % 1;
            }           
          }
        } 
        else 
        {
          buttonLeftPressed = false; 
        }

        if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        { 
          if (!buttonRightPressed) 
          { 
            tone(BUZZER, NOTE_E5, 200);
            buttonRightPressed = true;

            if(blinkingOperation)
            {
              if(programModeIndex == 0)
              {
                if(programTypeIndexBlink == 0) { phase = PREPARE_FAULT_MEMORY_BLINK; }
                else if(programTypeIndexBlink == 1) { phase = PREPARE_ACTUATOR_DIAGNOSTICS_1; }
              }
              else if(programModeIndex == 1)
              {
                if(programTypeIndexBlink == 0) { phase = LOAD_FAULT_MEMORY_BLINK; }
              }
            }            
            else
            {
              if(programModeIndex == 0)
              {
                if(programTypeIndex == 0) { phase = PREPARE_FAULT_MEMORY_FAST; }
                else if(programTypeIndex == 1) { phase = EXECUTE_SENSOR_DATA; }
                else if(programTypeIndex == 2) { phase = PREPARE_ACTUATOR_DIAGNOSTICS_KLINE; }
              }
              else if(programModeIndex == 1)
              {
                if(programTypeIndex == 0) { phase = LOAD_FAULT_MEMORY_FAST; }
                else if(programTypeIndex == 1) { phase = LOAD_SENSOR_DATA; }
              }
              else if(programModeIndex == 2)
              {
                if(programTypeIndex == 0) { phase = DELETE_FAULT_MEMORY_FAST; }
              }
            }
            newPhase = true;
          }
        } 
        else 
        {
          buttonRightPressed = false; 
        }
        if(!blinkingOperation && programTypeIndex == 0)
        {
          displayManager.displayProgramModeChoice(display, u8g2_for_adafruit_gfx, programModesWithDelete, programModeIndex, programTypes[programTypeIndex], programTypeIndex, blinkingOperation);
        }
        else if(!blinkingOperation && programTypeIndex == 2)
        {
          displayManager.displayProgramModeChoice(display, u8g2_for_adafruit_gfx, programModesOnlyExecute, programModeIndex, programTypes[programTypeIndex], programTypeIndex, blinkingOperation);
        }
        else if(!blinkingOperation) 
        {
          displayManager.displayProgramModeChoice(display, u8g2_for_adafruit_gfx, programModes, programModeIndex, programTypes[programTypeIndex], programTypeIndex, blinkingOperation);
        }
        else if(blinkingOperation && programTypeIndexBlink == 0)
        {
          displayManager.displayProgramModeChoice(display, u8g2_for_adafruit_gfx, programModes, programModeIndex, programTypesBlink[programTypeIndexBlink], programTypeIndexBlink, blinkingOperation);
        }
        else if(blinkingOperation && programTypeIndexBlink == 1)
        {
          displayManager.displayProgramModeChoice(display, u8g2_for_adafruit_gfx, programModesOnlyExecute, programModeIndex, programTypesBlink[programTypeIndexBlink], programTypeIndexBlink, blinkingOperation);
        }        
      }     
      //Laden
      else if(phase == LOAD_FAULT_MEMORY_FAST)
      {
        memoryManager.loadFaultMemoryFast(faultMemory, MAX_FAULTS, faultCount);
        phase = SHOW_FAULT_MEMORY;
        displayManager.initFaultMemoryDetails();
      }
      else if(phase == LOAD_SENSOR_DATA)
      {
        memoryManager.loadSensorData(sensorData, MAX_SENSORS, sensorCount);
        phase = SHOW_SENSOR_DATA_FINAL;
      } 
      else if(phase == LOAD_FAULT_MEMORY_BLINK)
      {
        memoryManager.loadFaultMemoryBlink(faultMemory, MAX_FAULTS, faultCount);
        phase = SHOW_FAULT_MEMORY;
        displayManager.initFaultMemoryDetails();
      }      
      //Anzeigen
      else if(phase == SHOW_SENSOR_DATA_FINAL)
      {
        manageLeftButtonForMenu(false);

        if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        { 
          if (!buttonRightPressed) 
          { 
            tone(BUZZER, NOTE_E5, 200);
            buttonRightPressed = true;
                        
            indexSensorData ++;
            if(indexSensorData > 10)
            {
              indexSensorData = 0;
            }
          }
        } 
        else 
        {
          buttonRightPressed = false; 
        }
        if(indexSensorData == 0)
        {
          displayManager.displaySensorDataFinal(display, u8g2_for_adafruit_gfx, sensorData, false);
        }
        else
        {
          displayManager.displaySensorDetails(display, u8g2_for_adafruit_gfx, indexSensorData, sensorData[indexSensorData-1]);
        }
      }   
      else if(phase == SHOW_FAULT_MEMORY)
      {
        manageLeftButtonForMenu(false);

        if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        { 
          if (!buttonRightPressed) 
          { 
            displayManager.initFaultMemoryDetails();
            tone(BUZZER, NOTE_E5, 200);
            buttonRightPressed = true;
                        
            indexFaultMemory ++;
            if(indexFaultMemory > faultCount)
            {
              indexFaultMemory = 0;
            }
          }
        } 
        else 
        {
          buttonRightPressed = false; 
        }
        if(indexFaultMemory == 0)
        {
          displayManager.displayFaultMemoryOverview(display, u8g2_for_adafruit_gfx, faultMemory, faultCount);
        }
        else
        {       
          displayManager.displayFaultMemoryDetails(display, u8g2_for_adafruit_gfx, faultMemory[indexFaultMemory-1], faultCount, indexFaultMemory, errorManager);
        }
      }   
     
      //Ausführen
      else if(phase == PREPARE_FAULT_MEMORY_FAST)
      {
        displayManager.prepareFaultMemory(display, u8g2_for_adafruit_gfx, false);       
        manageLeftButtonForMenu(false);
        manageRightButtonForNextPhase(EXECUTE_FAULT_MEMORY_FAST);      
      }    
      else if(phase == EXECUTE_FAULT_MEMORY_FAST)
      {
        if(newPhase)
        {
          displayManager.showLoadFaultMemory(display, u8g2_for_adafruit_gfx);
          newPhase = false;
          kLineManager.connect();
          kLineManager.getFaultMemory(display, u8g2_for_adafruit_gfx, displayManager, faultsVAGCodes, MAX_FAULTS, faultCount);
          for(int i = 0; i < faultCount; i++)
          {
            faultMemory[i] = errorManager.findCodeByVAGCode(faultsVAGCodes[i]);
          }
          memoryManager.saveFaultMemoryFast(faultMemory, faultCount);
          phase = LOAD_FAULT_MEMORY_FAST;
        }
      }      
      else if(phase == DELETE_FAULT_MEMORY_FAST)
      {
        displayManager.showDeleteFaultMemory(display, u8g2_for_adafruit_gfx);
        kLineManager.clearFaultMemory();
        goToMenu();
      }
      else if(phase == EXECUTE_SENSOR_DATA)
      {
        if(newPhase)
        {
          displayManager.showLoadSensorData(display, u8g2_for_adafruit_gfx);
          newPhase = false;
          kLineManager.connect();
        }
        else
        {
          kLineManager.getSensorData(display, u8g2_for_adafruit_gfx, displayManager, sensorData, MAX_SENSORS, sensorCount); 
          displayManager.displaySensorDataFinal(display, u8g2_for_adafruit_gfx, sensorData, true);
        }
        manageLeftButtonForMenu(true);
       
        if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        { 
          if (!buttonRightPressed) 
          { 
            tone(BUZZER, NOTE_E5, 200);
            buttonRightPressed = true;
           
            kLineManager.disconnect();
            memoryManager.saveSensorData(sensorData, 10);
            displayManager.showSaveSensorData(display, u8g2_for_adafruit_gfx);
            delay(2000);
            phase = SHOW_SENSOR_DATA_FINAL;
          }
        } 
        else 
        {
          buttonRightPressed = false; 
        }        
      }        
      else if(phase == PREPARE_ACTUATOR_DIAGNOSTICS_KLINE)
      {
        displayManager.prepareActuatorDiagnostics2(display, u8g2_for_adafruit_gfx);
        manageLeftButtonForMenu(false);
        newPhase = true;
        measureTime = 0;
        manageRightButtonForNextPhase(EXECUTE_ACTUATOR_DIAGNOSTICS_KLINE);      
      } 
      else if(phase == EXECUTE_ACTUATOR_DIAGNOSTICS_KLINE)
      {
        if(newPhase)
        {
          displayManager.showWaitingActuatorDiagnostics(display, u8g2_for_adafruit_gfx);
          newPhase = false;
          kLineManager.connect();
          int vagCode = kLineManager.performOutputTests();     
          int errorCode = errorManager.findCodeByVAGCode(vagCode);
          errorStructure = errorManager.findErrorStructureByErrorCode(errorCode, ENGINE);
          sprintf(bufferCode, "%d", errorCode); 
          displayManager.showAnalyzeActuators(display, u8g2_for_adafruit_gfx, bufferCode, errorStructure, true);
          measureTime = currentMillis;
        }
        else
        {
          if(currentMillis > measureTime)
          {            
            kLineManager.update();
            measureTime = currentMillis + 10;
          }
          manageLeftButtonForMenu(false);
          if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
          {
            if (!buttonRightPressed) 
            { 
              tone(BUZZER, NOTE_E5, 200);
              buttonRightPressed = true;
              int vagCode = kLineManager.performOutputTests();     
              int errorCode = errorManager.findCodeByVAGCode(vagCode);
              errorStructure = errorManager.findErrorStructureByErrorCode(errorCode, ENGINE);
              sprintf(bufferCode, "%d", errorCode); 
              displayManager.showAnalyzeActuators(display, u8g2_for_adafruit_gfx, bufferCode, errorStructure, true);
            }
          } 
          else 
          {
            buttonRightPressed = false; 
          }        
        
        }
      }             
      else if(phase == PREPARE_ACTUATOR_DIAGNOSTICS_1)
      {
        displayManager.prepareActuatorDiagnostics1(display, u8g2_for_adafruit_gfx);
        manageLeftButtonForMenu(false);
        manageRightButtonForNextPhase(PREPARE_ACTUATOR_DIAGNOSTICS_2);      
      }    
      else if(phase == PREPARE_ACTUATOR_DIAGNOSTICS_2)
      {         
        delay(100);
        if(newPhase)
        {
          displayManager.prepareActuatorDiagnostics2(display, u8g2_for_adafruit_gfx);
          newPhase = false;
          setPinState(LOW);          
        }
        manageLeftButtonForMenu(false);

        if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        {
          if (!buttonRightPressed) 
          { 
            tone(BUZZER, NOTE_E5, 200);
            buttonRightPressed = true;
            measureTime = currentMillis + 4100;
            displayManager.showAnalyzeActuators(display, u8g2_for_adafruit_gfx, displayBufferBlink, errorStructureEmpty, false);
            phase = EXECUTE_ACTUATOR_DIAGNOSTICS;
          }
        } 
        else 
        {
          buttonRightPressed = false; 
        }        
        
      }       
      else if(phase == EXECUTE_ACTUATOR_DIAGNOSTICS)
      {
        if(newPhase)
        {
          newPhase = false;          
          setPinState(LOW);     
        }
        else if(currentMillis > measureTime)
        {
          setPinState(HIGH);  
          measureTime = currentMillis;
          phase = ANALYZE_ACTUATOR_DIAGNOSTICS;
          newPhase = true;
        }
      }
      else if(phase == ANALYZE_ACTUATOR_DIAGNOSTICS)
      {
        if(!actuatorBlinkFinished)
        {
          if(noBlinkingFor12Seconds)
          {          
            phase = ANALYZE_ACTUATOR_NOT_POSSIBLE;
          }
          else
          {
            analyzeActuators(currentMillis);
          }
        }
        else
        {
          phase = ANALYZE_ACTUATOR_FINISHED;
        }
        if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
        { 
          if (!buttonRightPressed) 
          { 
            resetBlinkVariables(true);
            displayManager.showAnalyzeActuators(display, u8g2_for_adafruit_gfx, unknownCode, errorStructureEmpty, false);
            tone(BUZZER, NOTE_E5, 200);
            buttonRightPressed = true;
            setPinState(LOW);  
            delay(4100);
            setPinState(HIGH);  
          }   
        }
        else 
        {
          buttonRightPressed = false; 
        } 
      }     
      else if(phase == ANALYZE_ACTUATOR_FINISHED)
      {
        displayManager.showActuatorFinished(display, u8g2_for_adafruit_gfx);
        manageLeftButtonForMenu(false);       
      }   
      else if(phase == ANALYZE_ACTUATOR_NOT_POSSIBLE)
      {
        displayManager.showActuatorsNotPossible(display, u8g2_for_adafruit_gfx);
        manageLeftButtonForMenu(false);       
      }            
      else if(phase == PREPARE_FAULT_MEMORY_BLINK)
      {
        displayManager.prepareFaultMemory(display, u8g2_for_adafruit_gfx, true);
        manageLeftButtonForMenu(false);
        manageRightButtonForNextPhase(EXECUTE_FAULT_MEMORY_BLINK);            
      } 
      else if(phase == EXECUTE_FAULT_MEMORY_BLINK)
      {
        if(newPhase)
        {
          measureTime = currentMillis + 4100;
          newPhase = false;
          setPinState(LOW);            
          displayManager.showAnalyzeFaultMemory(display, u8g2_for_adafruit_gfx, unknownCode);
        }
        else if(currentMillis > measureTime)
        {
          setPinState(HIGH);  
          measureTime = currentMillis;
          phase = ANALYZE_FAULT_MEMORY_BLINK;
          newPhase = true;
        }
      }   
      else if(phase == ANALYZE_FAULT_MEMORY_BLINK)
      {
        if(!faultMemoryBlinkFinished)
        {
          if(noBlinkingFor12Seconds)
          {          
            phase = ANALYZE_FAULT_MEMORY_NOT_POSSIBLE;
          }
          else
          {
            analyzeBlinkCodes(currentMillis);
          }   
        }
        else
        {
          memoryManager.saveFaultMemoryBlink(faultMemory, faultCount);
          phase = LOAD_FAULT_MEMORY_BLINK;
        }
      }         
      else if(phase == ANALYZE_FAULT_MEMORY_NOT_POSSIBLE)
      {
        displayManager.showFaultMemoryNotPossible(display, u8g2_for_adafruit_gfx);
        manageLeftButtonForMenu(false);       
      }     
  
       
}

//TODO kann später alles weg, led brauch man auch net
void manageVoltage(long currentMillis)
{
    if(voltageTime < currentMillis)
    {        //memory check for debugging
    //int freeMemory2 = freeMemory();
    //Serial.print("free:");Serial.println(freeMemory2);
      voltageTime = currentMillis + 20;
      double voltage = map(analogRead(A0), 0, 1023, 0, 2500);
      voltage /= 100;
      if(voltage > voltageBoundary)
      {        
        if(isBlinking)
        {
          lastInterval = currentMillis - lastBlinkingChange;
          lastBlinkingChange = currentMillis;
        }
        isBlinking = false;
        analogWrite(ledPin, 0);
      }
      else if(voltage <= voltageBoundary)
      {
        if(!isBlinking)
        {
          lastInterval = currentMillis - lastBlinkingChange;
          lastBlinkingChange = currentMillis;
        }        
        isBlinking = true;
        analogWrite(ledPin, 5);
      }
    }
}

boolean getLightOn()
{
  double voltage = map(analogRead(A0), 0, 1023, 0, 2500);
  voltage /= 100;
  if(voltage > voltageBoundary)
  {
    analogWrite(ledPin, 0);    
    return false;
  }
  else
  {
    analogWrite(ledPin, 5);    
    return true;
  }       
}

void goToMenu()
{
  phase = SELECT_OPERATION;
  indexSensorData = 0;
  indexFaultMemory = 0;
  faultMemoryBlinkFinished = false;
  actuatorBlinkFinished = false;
  programModeIndex = 0;
  programTypeIndex = 0;
  programTypeIndexBlink = 0;
  blinkingOperation = false;
  resetBlinkVariables(true);
  faultCount = 0;  
  noBlinkingFor12Seconds = false;
  analogWrite(ledPin, 0);
}

void manageLeftButtonForMenu(boolean disconnect)
{
  if (digitalRead(BUTTON_LEFT_PIN) == HIGH) 
  { 
    if (!buttonLeftPressed) 
    { 
      tone(BUZZER, NOTE_D5, 200);
      buttonLeftPressed = true;
      goToMenu();
      if(disconnect)
      {
        kLineManager.disconnect();
      }
    }
  } 
  else 
  {
    buttonLeftPressed = false; 
  }
}

void manageRightButtonForNextPhase(int nextPhase)
{
  if (digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
  {
    if (!buttonRightPressed) 
    { 
      tone(BUZZER, NOTE_E5, 200);
      buttonRightPressed = true;                        
      phase = nextPhase;            
      newPhase = true;
      if(nextPhase == SELECT_OPERATION_BLINK)
      {
        blinkingOperation = true;
      }
    }
  } 
  else 
  {
    buttonRightPressed = false; 
  }
}

void analyzeBlinkCodes(long now)
{
  //kein Blinken, Zündung aus/an
  if(now - measureTime >= 12000)
  {
    noBlinkingFor12Seconds = true;
  }

  boolean currentState = getLightOn();
  
  // Flanke erkannt
  if (currentState != lastState) 
  {
    unsigned long duration = now - lastChangeTime;
    measureTime = now;

    // Kurzes AN = Blink erkannt
    if (lastState && duration >= 400 && duration <= 600) 
    {
      blinkCount++;
    }

    // Lange AUS = Ziffer abgeschlossen
    if (!lastState && duration >= 2400 && duration <= 2600) 
    {
      if(ignoreFirstLongBreak)
      {
        ignoreFirstLongBreak = false;
      }
      else 
      {
        if (digitIndex < 4) 
        {
          digits[digitIndex++] = blinkCount;
          getCurrentCodeString(displayBufferBlink, digits, digitIndex);
          displayManager.showAnalyzeFaultMemory(display, u8g2_for_adafruit_gfx, displayBufferBlink);
        }
        blinkCount = 0;
      }
    }

    //Ende Diagnose erkennen
    if(duration >= 2480 && duration <= 2520)
    {
      counter2500ms++;
      if(counter2500ms > 4)
      {
        faultMemoryBlinkFinished = true;
        counter2500ms  = 0;
      }
    }
    else
    {
      counter2500ms  = 0;
    }
    

    // 2.5s AN = Fehlercode vollständig
    if (lastState && duration >= 2400 && duration <= 2600 && digitIndex == 4) 
    {
      int code = digits[0] * 1000 + digits[1] * 100 + digits[2] * 10 + digits[3];
      if(code == 0 || code == 4444)
      {
        code = 0;
        faultMemoryBlinkFinished = true;
      }

      faultMemory[faultCount++] = code;
      
      // Brücke setzen, um nächsten Code zu bekommen
      setPinState(LOW);  
      delay(4100);
      setPinState(HIGH);  
      displayManager.showAnalyzeFaultMemory(display, u8g2_for_adafruit_gfx, unknownCode);

      resetBlinkVariables(true);
    }

    lastChangeTime = now;
    lastState = currentState;
  }
}


void analyzeActuators(long now)
{
  //kein Blinken, Zündung aus/an
  if(now - measureTime >= 12000)
  {
    noBlinkingFor12Seconds = true;
  }

  boolean currentState = getLightOn();
  
  // Flanke erkannt
  if (currentState != lastState) 
  {

    unsigned long duration = now - lastChangeTime;
    measureTime = now;

    // Kurzes AN = Blink erkannt
    if (lastState && duration >= 480 && duration <= 520) 
    {
      blinkCount++;
    }

    // Lange AUS = Ziffer abgeschlossen
    if (!lastState && duration >= 2480 && duration <= 2520) 
    {
      if(ignoreFirstLongBreak)
      {
        ignoreFirstLongBreak = false;
      }
      else 
      {
        if (digitIndex < 4) 
        {
          digits[digitIndex++] = blinkCount;
          getCurrentCodeString(displayBufferBlink, digits, digitIndex);
          if(codeActuator == -1)
          {
            displayManager.showAnalyzeActuators(display, u8g2_for_adafruit_gfx, displayBufferBlink, errorStructureEmpty, false);
          }
          else
          {
            displayManager.showAnalyzeActuators(display, u8g2_for_adafruit_gfx, displayBufferBlink, errorStructure, false);
          }
        }
        blinkCount = 0;
      }
    }

    //Ende Diagnose erkennen
    if(duration >= 2480 && duration <= 2520)
    {
      counter2500ms++;
      if(counter2500ms > 4)
      {
        actuatorBlinkFinished = true;
        counter2500ms  = 0;
      }
    }
    else
    {
      counter2500ms  = 0;
    }
    
    //Ungültige Intervalle
    if(duration < 480 || (duration > 520 && duration < 2480) || duration > 2520)
    {
      resetBlinkVariables(true);
    }

    //Es wurde mittendrin weitergeschaltet, nicht nach der letzten Ziffer
    if (lastState && duration >= 2480 && duration <= 2520 && digitIndex != 4) 
    {
      resetBlinkVariables(true);
    }    

    //Alle Ziffern durch, 
    if (lastState && duration >= 2480 && duration <= 2520 && digitIndex == 4) 
    {
      codeActuator = digits[0] * 1000 + digits[1] * 100 + digits[2] * 10 + digits[3];
      if(codeActuator == 0 || codeActuator == 4444)
      {
        codeActuator = 0;
        actuatorBlinkFinished = true;
      }
      errorStructure = errorManager.findErrorStructureByErrorCode(codeActuator, ENGINE);
      displayManager.showAnalyzeActuators(display, u8g2_for_adafruit_gfx, displayBufferBlink, errorStructure, false);
      resetBlinkVariables(false);
    }

    lastChangeTime = now;
    lastState = currentState;
  }
}

void getCurrentCodeString(char* buffer, const int* digits, int digitCount) {
  for (int i = 0; i < 4; i++) {
    if (i < digitCount) {
      buffer[i * 2] = '0' + digits[i];  // Zahl in Zeichen wandeln
    } else {
      buffer[i * 2] = '?';
    }
    if (i < 3) {
      buffer[i * 2 + 1] = '-';
    }
  }
  buffer[7] = '\0'; // Null-Terminierung
}


void resetBlinkVariables(boolean resetHard)
{
  ignoreFirstLongBreak = true;
  digitIndex = 0;
  blinkCount = 0;
  for (int i = 0; i < 4; i++) digits[i] = 0;
  strcpy(displayBufferBlink, "?-?-?-?");

  if(resetHard)
  {
    codeActuator = -1;
  }
}

int freeMemory()
{
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

String letzteWerte = "";

void handlePythonCommand(const char* cmd) {
  
  if (strcmp(cmd, "<H>") == 0) 
  {
    phase = DATA_TO_SERIAL;
    displayManager.showDatentransferBereit(display, u8g2_for_adafruit_gfx);
  }
  else if (strcmp(cmd, "<S>") == 0)
  {
    kLineManager.connect();
    isSendingToPython = true;
    Serial.println("START OK");
    displayManager.showDatentransferAktiv(display, u8g2_for_adafruit_gfx);
  }
  else if (strcmp(cmd, "<X>") == 0)
  {
    isSendingToPython = false;
    kLineManager.disconnect();
    Serial.println("STOP OK");
    displayManager.showDatentransferPausiert(display, u8g2_for_adafruit_gfx);
  } 
  else 
  {
    displayManager.showError(display, u8g2_for_adafruit_gfx, "Unbekannter Befehl");
  }
}


void sendSensorDataViaSerial()
{
  kLineManager.getSensorData(display, u8g2_for_adafruit_gfx, displayManager, sensorData, MAX_SENSORS, sensorCount);
  char zeile[128];
  int pos = 0;

  for (size_t i = 0; i < sensorCount && pos < sizeof(zeile) - 6; i++) 
  {
    pos += sprintf(zeile + pos, "%d", sensorData[i]);
    if (i < sensorCount - 1) 
    {
      zeile[pos++] = ',';
    }
  }
  zeile[pos] = '\0';
  Serial.println(zeile);
}

void setPinState(uint8_t level)
{
  if (USE_RELAIS) 
  {
    digitalWrite(PIN_RELAY, level == HIGH ? LOW : HIGH);
  } 
  else 
  {
    digitalWrite(TX_pin, level);
  }
}

/*
void sendeZufallswerte() { 
  int werte[10];
  String zeile = "";

  werte[0] = random(184, 216);   // Motortemp
  werte[1] = random(20, 26);     // Motorlast
  werte[2] = random(77, 84);     // Drehzahl
  werte[3] = random(121, 136);   // Leerlaufstabilisierung
  werte[4] = random(80, 126);    // Luftmenge
  werte[5] = random(123, 138);   // Lastanpassung
  werte[6] = random(41, 56);     // Kennliniensteuerung
  werte[7] = random(123, 134);   // Lambda intern
  werte[8] = random(115, 142);   // Lambda Ist
  werte[9] = random(35, 38);     // Zündwinkel

  for (int i = 0; i < 10; i++) {
    Serial.print(werte[i]);
    zeile += String(werte[i]);
    if (i < 9) {
      Serial.print(",");
      zeile += ",";
    }
  }
  Serial.println();

  letzteWerte = zeile;  // Nur setzen, wenn die ganze Zeile vollständig ist
}
*/