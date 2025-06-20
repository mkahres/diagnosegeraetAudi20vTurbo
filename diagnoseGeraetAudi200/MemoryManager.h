#ifndef MemoryManager_h
#define MemoryManager_h
#include "Constants.h"

class MemoryManager {
public:
  void saveFaultMemoryFast(int* list, int size);
  void saveFaultMemoryBlink(int* list, int size);
  void saveSensorData(int* list, int size);

  void loadFaultMemoryFast(int* buffer, size_t maxSize, size_t &outSize);
  void loadFaultMemoryBlink(int* buffer, size_t maxSize, size_t &outSize);
  void loadSensorData(int* buffer, size_t maxSize, size_t &outSize);

private:
  void saveData(int* list, int sizeOfList, int addrStart, int numberOfElements);
  void loadData(int addrStart, int numberOfElements, int* buffer, size_t maxSize, size_t &outSize, bool filterOutZero);
};

#endif