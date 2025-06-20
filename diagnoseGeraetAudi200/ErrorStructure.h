#ifndef ErrorStructure_h
#define ErrorStructure_h

typedef struct {  
  int errorCode;         // Fehlercode Ausblinken
  int errorCodeVAG1551;  // Fehlercode VAG1551
  char * motor[3];       // Array für Motorkennungen (maximal 3 verschiedene Buchstaben)
  char * shortText;      // FehlertextKurz
  char * longText;       // FehlertextLang
} ErrorStructure;

#endif