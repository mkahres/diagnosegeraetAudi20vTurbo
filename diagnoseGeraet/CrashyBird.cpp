#include "CrashyBird.h"
#include <EEPROM.h>

const int obstacleWidth = 10;  // Breite des Hindernisses

const int carWidth = 12;
const int carHeight = 8;

const int HIGHSCORE_ADDRESS = 100;
int highscore = 0;

void CrashyBird::crashyBird(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx) 
{
  
  display.clearDisplay();
  u8g2_for_adafruit_gfx.setFont(u8g2_font_6x10_tr);
  u8g2_for_adafruit_gfx.setCursor(0, 10);
  u8g2_for_adafruit_gfx.print("Crashy Bird!");
  display.display();
  delay(1000);
  highscore = loadHighscore();

  int carY = SCREEN_HEIGHT / 2;
  int velocity = 0;
  const int gravity = 1;
  const int jumpStrength = -4;

  int obstacleX = SCREEN_WIDTH;
  int gapY = random(15, SCREEN_HEIGHT - 25);
  const int gapHeight = 25;

  unsigned long lastFrame = millis();
  bool gameOver = false;

  int score = 0;

  while (true) {
    unsigned long now = millis();
    if (now - lastFrame < 50) continue;  // ~20 FPS
    lastFrame = now;

    if (digitalRead(BUTTON_LEFT_PIN) == HIGH && digitalRead(BUTTON_RIGHT_PIN) == HIGH) 
    {
      return;
    }

    // Steuerung
    else if (digitalRead(BUTTON_RIGHT_PIN) == HIGH && !gameOver) {
      velocity = jumpStrength;
    }

    else if (digitalRead(BUTTON_LEFT_PIN) == HIGH && gameOver) {
      return;  // Spiel beenden
    }

    // Physik
    velocity += gravity;
    carY += velocity;



    // Hindernis bewegen
    obstacleX -= 2;
    if (obstacleX + obstacleWidth < 0) {
      obstacleX = SCREEN_WIDTH;
      gapY = random(15, SCREEN_HEIGHT - 25);
    }

    // Kollision
    const int carX = 8;  // X-Position des Autos (wo es gezeichnet wird)

    if (carY < 0 || carY + carHeight > SCREEN_HEIGHT) {
      gameOver = true;
    }

    if (checkCollisionPixelExact(carX, carY, carBitmap, carWidth, carHeight, obstacleX, gapY, gapHeight)) {
      gameOver = true;
      saveHighscore(score);
    }

    if (!gameOver) {
    // Hindernis vorbei?
    if (obstacleX + obstacleWidth == carX) {
      score++;
    }

  }
  
    // Zeichnen
    display.clearDisplay();
    u8g2_for_adafruit_gfx.setCursor(0, 10);
    if (gameOver) {
      display.setCursor(0, 0);
      display.print("Score: ");
      display.print(score);
      display.setCursor(0, 10);
      display.print("Highscore: ");
      display.print(highscore);
    } else {
      u8g2_for_adafruit_gfx.print("Crashy Bird!");
      display.setCursor(110, 0);
      display.print(score);
    }

    // Auto zeichnen (links)
    display.drawBitmap(8, carY, carBitmap, 12, 8, SH110X_WHITE);

    // Hindernis
    display.fillRect(obstacleX, 0, obstacleWidth, gapY, SH110X_WHITE);
    display.fillRect(obstacleX, gapY + gapHeight, obstacleWidth, SCREEN_HEIGHT - gapY - gapHeight, SH110X_WHITE);

    display.display();
  }
}


bool CrashyBird::checkCollisionPixelExact(int carX, int carY, const unsigned char *bitmap, int width, int height,
                              int obstacleX, int gapY, int gapHeight) {
  // Frühzeitiger Check: Berühren sich Auto und Hindernis überhaupt horizontal?
  if (obstacleX > carX + width || obstacleX + obstacleWidth < carX) {
    return false;
  }

  // Bereich der horizontalen Überlappung berechnen
  int startX = max(0, obstacleX - carX);
  int endX = min(width, obstacleX + obstacleWidth - carX);

  for (int y = 0; y < height; y++) {
    for (int x = startX; x < endX; x++) {
      // Pixel im Bitmap gesetzt?
      int byteIndex = (y * ((width + 7) / 8)) + (x / 8);
      uint8_t bit = 7 - (x % 8);
      bool pixelOn = (pgm_read_byte(&bitmap[byteIndex]) >> bit) & 1;

      if (!pixelOn) continue;

      int screenY = carY + y;

      // Pixel liegt außerhalb der Lücke?
      if (screenY < gapY || screenY >= gapY + gapHeight)
      {        
        return true;  // echte Pixelkollision
      }
    }
  }
  return false;
}

void CrashyBird::saveHighscore(int score) {
  if(score > highscore)
  {
    EEPROM.put(HIGHSCORE_ADDRESS, score);
  }
}

int CrashyBird::loadHighscore() {
  int score;
  EEPROM.get(HIGHSCORE_ADDRESS, score);
  return score;
}