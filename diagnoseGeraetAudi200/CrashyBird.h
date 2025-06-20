#ifndef CrashyBird_h
#define CrashyBird_h

#include "Constants.h"

class CrashyBird
{
  

  public:
    void crashyBird(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx);
  private:  
    bool checkCollisionPixelExact(int carX, int carY, const unsigned char *bitmap, int width, int height, int obstacleX, int gapY, int gapHeight);
    void saveHighscore(int score);
    int loadHighscore();
};
#endif