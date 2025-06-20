#ifndef DisplayManager_h
#define DisplayManager_h

#include "ErrorManager.h"
#include "Constants.h"

class DisplayManager
{
  public:
    DisplayManager();
    void showError(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* error);
    void displayStartScreen(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void displayProgramTypeChoice(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* programTypes[], int &programTypeIndex, bool blinkingOperation);
    void displayProgramModeChoice(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, 
      char* programModes[], int &programModeIndex, char* programType, int &programTypeIndex, bool blinkingOperation);
    void displaySensorDetails(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int index, int value);
    void displaySensorDetails1(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails2(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails3(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails4(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails5(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails6(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails7(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails8(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails9(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void displaySensorDetails10(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int value);
    void showLoadSensorData(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showSaveSensorData(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showLoadFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void displaySensorDataFinal(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int* list, bool continuously);
    void prepareFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, boolean blink);
    void showAnalyzeFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* actualCode);
    void showAnalyzeActuators(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, char* actualCode, ErrorStructure errorStructure, boolean kLine);
    void showActuatorFinished(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showFaultMemoryNotPossible(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showActuatorsNotPossible(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void prepareActuatorDiagnostics1(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void prepareActuatorDiagnostics2(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showWaitingActuatorDiagnostics(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void displayFaultMemoryOverview(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int* list, int size);
    void displayFaultMemoryDetails(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int code, int sizeFaultMemory, int indexFaultMemory, ErrorManager &errorManager);
    void initFaultMemoryDetails();
    void showDeleteFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showDatentransferBereit(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showDatentransferAktiv(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void showDatentransferPausiert(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    String readProgmemString(const char *ptr);
    void DisplayManager::test(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
    void loadScrollSliceFromPROGMEM(const char* progmemStr, int byteOffset);
    void drawScrollingProgmemText(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, const char* progmemStr);
    void scrollText(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, int y);

  private:
    char** intArrayToStringArray(int* inputArray, int size);
    const uint8_t* getStatusBitmap(int index, int value);
    int getMin(int index);
    int getMax(int index);
    float convertSensorLambdaToO2Vol(int sensorValue);
    const char* getLambdaStatus(int raw, char* buffer);
    const char* getFormattedEngineTemp(int rawValue, char* buffer);
    const char* getIgnitionAngle(int rawValue, char* buffer);
    void printFixedIntList(U8G2_FOR_ADAFRUIT_GFX &u8g2, int* values, int count, int startX, int startY);
    char* progmemToRam(const char* progmemStr);
};

#endif