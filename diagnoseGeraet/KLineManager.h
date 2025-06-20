#ifndef KLineManager_h
#define KLineManager_h

#include <Arduino.h>
#include "DisplayManager.h"

class KLineManager
{
  public:
    void connect();
    void getSensorData(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, DisplayManager &displayManager, int* buffer, size_t maxSize, size_t &outSize);
    void getFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, DisplayManager &displayManager, int* buffer, size_t maxSize, size_t &outSize);
    void clearFaultMemory();
    int performOutputTests();
    void update();
    void disconnect();
  private:  
    void convertUint8ToInt(const uint8_t* input, int* output, size_t length);
};
#endif