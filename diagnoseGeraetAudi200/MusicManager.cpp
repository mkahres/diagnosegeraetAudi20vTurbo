#include "MusicManager.h"

int tempoStarWars = 120;
int wholenoteStarWars = (60000 * 4) / tempoStarWars;
int divider = 0;
int noteDuration = 0;

MusicManager::MusicManager()
{

}

void MusicManager::playStarWars()
{
  
    int melody[] = {

      NOTE_A4, 4, NOTE_A4, 4, NOTE_A4, 4, NOTE_F4, -8, NOTE_C5, 16,

      NOTE_A4, 4, NOTE_F4, -8, NOTE_C5, 16, NOTE_A4, 2, //4
    /*   NOTE_E5, 4, NOTE_E5, 4, NOTE_E5, 4, NOTE_F5, -8, NOTE_C5, 16,
      NOTE_A4, 4, NOTE_F4, -8, NOTE_C5, 16, NOTE_A4, 2,

     NOTE_A5, 4, NOTE_A4, -8, NOTE_A4, 16, NOTE_A5, 4, NOTE_GS5, -8, NOTE_G5, 16, //7
      NOTE_DS5, 16, NOTE_D5, 16, NOTE_DS5, 8, REST, 8, NOTE_A4, 8, NOTE_DS5, 4, NOTE_D5, -8, NOTE_CS5, 16,

      NOTE_C5, 16, NOTE_B4, 16, NOTE_C5, 16, REST, 8, NOTE_F4, 8, NOTE_GS4, 4, NOTE_F4, -8, NOTE_A4, -16, //9
      NOTE_C5, 4, NOTE_A4, -8, NOTE_C5, 16, NOTE_E5, 2,

      NOTE_A5, 4, NOTE_A4, -8, NOTE_A4, 16, NOTE_A5, 4, NOTE_GS5, -8, NOTE_G5, 16, //7
      NOTE_DS5, 16, NOTE_D5, 16, NOTE_DS5, 8, REST, 8, NOTE_A4, 8, NOTE_DS5, 4, NOTE_D5, -8, NOTE_CS5, 16,

      NOTE_C5, 16, NOTE_B4, 16, NOTE_C5, 16, REST, 8, NOTE_F4, 8, NOTE_GS4, 4, NOTE_F4, -8, NOTE_A4, -16, //9
      NOTE_A4, 4, NOTE_F4, -8, NOTE_C5, 16*/

    };
    playIt(melody, sizeof(melody) / sizeof(melody[0]) / 2, wholenoteStarWars);
  }

void MusicManager::playIt(int *melody, int notes, int wholenote)
{
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) 
  {
    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) 
    {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } 
    else if (divider < 0) 
    {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(BUZZER);
  }
}