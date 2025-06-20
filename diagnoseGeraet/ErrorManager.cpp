#include "ErrorStructure.h"
#include "ErrorManager.h"
#include "Constants.h"

const ErrorStructure errorList[] PROGMEM = {
  {1111, 65535, {"3B"}, "Steuergerät", error_1111_3B},
  {1111, 0, {"MC"}, "Motorsteuergerät", error_1111_MC},
  {1111, 0, {"NF"}, "Motorsteuergerät", error_1111_NF},
  //Der Code 1231 existiert für den AAN (wird wie 3B behandelt) auf http://www.sjmautotechnik.com/trouble_shooting/20vcodes.htm aber nicht in Rep-Anl-AAN-Motronic.pdf)
  {1231, 281, {"NF", "3B"}, "Drehzahlsensor", error_1231_3B_NF},
  {2111, 513, {"3B", "MC"}, "Drehzahlsensor", error_2111_3B_MC},
  {2112, 514, {"3B", "MC"}, "Bezugsmarkengeber", error_2112_3B_MC},
  {2113, 515, {"3B", "MC"}, "Hallgeber", error_2113_3B_MC},
  {2121, 516, {"3B"}, "Leerlaufschalter", error_2121_3B},
  {2121, 0, {"MC"}, "Drosselklappenschalter", error_2121_MC},
  {2121, 0, {"NF"}, "Leerlaufschalter", error_2121_NF},
  {2122, 0, {"NF"}, "Drehzahlsensor", error_2122_NF},
  {2123, 0, {"MC", "NF"}, "Drosselklappenschalter", error_2123_MC_NF},
  {2132, 0, {"3B", "NF"}, "Abgas", error_2132_3B_NF},
  {2141, 535, {"3B", "MC"}, "Klopfsensor Nr. 1", error_2141_3B_MC},
  {2141, 0, {"NF"}, "Klopfsensor", error_2141_NF},
  {2142, 524, {"3B"}, "Klopfsensor Nr. 1", error_2142_3B},
  {2142, 0, {"MC"}, "Klopfsensor Nr. 1", error_2142_MC},
  {2142, 0, {"NF"}, "Klopfsensor", error_2142_NF},
  {2143, 536, {"3B"}, "Klopfsensor Nr. 2", error_2143_3B},
  {2143, 0, {"MC"}, "Klopfsensor Nr. 2", error_2143_MC},
  {2144, 540, {"3B"}, "Klopfsensor Nr. 2", error_2144_3B},
  {2144, 0, {"MC"}, "Klopfsensor Nr. 2", error_2144_MC},
  {2212, 518, {"3B"}, "Drosselklappenpoti", error_2212_3B},
  {2214, 543, {"3B"}, "Drehzahl überschritten", error_2214_3B},
  {2214, 0, {"MC"}, "Drehzahl überschritten", error_2214_MC},
  {2221, 575, {"3B"}, "Druck im Ansaugrohr", error_2221_3B},
  {2221, 0, {"MC"}, "Druck im Ansaugrohr", error_2221_MC},
  {2222, 519, {"3B", "MC"}, "Drucksensor", error_2222_3B_MC},
  {2223, 528, {"3B"}, "Höhengeber", error_2223_3B},
  {2223, 0, {"NF"}, "Höhengeber", error_2223_NF},
  {2224, 544, {"3B"}, "Overboost", error_2224_3B},
  {2224, 0, {"MC"}, "Overboost", error_2224_MC},
  {2231, 533, {"3B", "MC"}, "Leerlaufregelung", error_2231_3B_MC},
  {2232, 520, {"3B", "MC"}, "Luftmassenmesser ", error_2324_3B_MC}, //(ECU 447 907 404A)
  {2232, 0, {"NF"}, "Stauscheibenpoti", error_2232_NF},
  {2233, 0, {"NF"}, "Stauscheibenpoti", error_2233_NF},
  {2234, 532, {"3B"}, "Spannungsversorgung", error_2234_3B},
  {2241, 538, {"3B"}, "Klopfsensor 2", error_2241_3B},
  {2312, 522, {"3B", "MC", "NF"}, "Geber für Kühlmitteltemp.", error_2312_3B_MC_NF},
  {2322, 523, {"3B", "MC"}, "Ansauglufttemperatursensor", error_2322_3B_MC},
  {2324, 553, {"3B", "MC"}, "Luftmassenmesser ", error_2324_3B_MC},//(ECU 857 907 404B)
  {2341, 537, {"3B"}, "Lambdasonde", error_2341_3B},
  {2341, 0, {"NF"}, "Lambdasonde", error_2341_NF},
  {2342, 525, {"3B"}, "Lambdasonde", error_2342_3B}, 
  {2342, 0, {"MC"}, "Lambdasonde", error_2342_MC},
  {2411, 0, {"NF"}, "Abgasrückführventil", error_2411_NF},
  {2413, 561, {"3B"}, "Gemischanpassung", error_2413_3B}, 
  {3424, 824, {"3B", "MC"}, "Motorfehlerlampe", error_3424_3B_MC},
  {4343, 1247, {"3B"}, "Aktivkohlebehälter", error_4343_3B}, 
  {4411, 1249, {"3B"}, "Einspritzventil Zyl. 1", error_4411_3B}, 
  {4412, 1250, {"3B"}, "Einspritzventil Zyl. 2", error_4412_3B}, 
  {4413, 1251, {"3B"}, "Einspritzventil Zyl. 3", error_4413_3B}, 
  {4414, 1252, {"3B"}, "Einspritzventil Zyl. 4", error_4414_3B}, 
  {4421, 1253, {"3B"}, "Einspritzventil Zyl. 5", error_4421_3B}, 
  {4431, 1257, {"3B"}, "Leerlaufsteuerventil", error_4431_3B}, 
  {4431, 0, {"NF"}, "Leerlaufsteuerventil", error_4431_NF}, 
  {4442, 1262, {"3B"}, "Ladedruck Taktventil", error_4442_3B},
  {4444, 0, {"3B"}, "keine Fehler", error_4444_3B_MC_NF}
};

static constexpr size_t errorListSize = sizeof(errorList) / sizeof(errorList[0]);

int ErrorManager::getErrorListSize()
{
  return errorListSize;
}

ErrorStructure * ErrorManager::getErrorList()
{
  return errorList;
}


ErrorStructure ErrorManager::findErrorStructureByErrorCode(int code, const char* motor) 
{
  static ErrorStructure temp;

  for (int i = 0; i < errorListSize; i++) 
  {
    memcpy_P(&temp, &errorList[i], sizeof(ErrorStructure));
    if (temp.errorCode == code) 
    {
      int size = sizeof(temp.motor) / sizeof(temp.motor[0]);
      for (int j = 0; j < size; j++) 
      {
        if (strcmp(temp.motor[j], motor) == 0) 
        {
          return temp;  // return pointer into PROGMEM
        }
      }
    }
  }
  temp.errorCode = code;
  temp.motor[0] = motor;
  temp.shortText = "Fehlertext nicht gefunden"; 
  temp.longText  = errorNotFound;
  return temp; // not found
}

int ErrorManager::findCodeByVAGCode(int vagCode) 
{
  static ErrorStructure temp;

  for (int i = 0; i < errorListSize; i++) 
  {
    memcpy_P(&temp, &errorList[i], sizeof(ErrorStructure));
    if (temp.errorCodeVAG1551 == vagCode && vagCode != 0) 
    {
      return temp.errorCode;  // return pointer into PROGMEM
    }
  }
  return 0; // not found
}