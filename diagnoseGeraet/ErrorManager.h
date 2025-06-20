#ifndef ErrorManager_h
#define ErrorManager_h

#include "ErrorStructure.h"

class ErrorManager
{
  public:
    ErrorStructure * getErrorList();
    int getErrorListSize();
    ErrorStructure findErrorStructureByErrorCode(int code, const char* motor);
    int findCodeByVAGCode(int vagCode);
};
#endif