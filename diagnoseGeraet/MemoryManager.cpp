#include "MemoryManager.h"
#include "Arduino.h"
#include <EEPROM.h>

void MemoryManager::saveFaultMemoryFast(int* list, int size) {
  saveData(list, size, 0, 20);
}

void MemoryManager::saveFaultMemoryBlink(int* list, int size) {
  saveData(list, size, 40, 20);
}

void MemoryManager::saveSensorData(int* list, int size) {
  saveData(list, size, 80, 10);
}

void MemoryManager::loadFaultMemoryFast(int* buffer, size_t maxSize, size_t &outSize) {
  loadData(0, 20, buffer, maxSize, outSize, true);
}

void MemoryManager::loadFaultMemoryBlink(int* buffer, size_t maxSize, size_t &outSize) {
  loadData(40, 20, buffer, maxSize, outSize, true);
}

void MemoryManager::loadSensorData(int* buffer, size_t maxSize, size_t &outSize) {
  loadData(80, 10, buffer, maxSize, outSize, false);
}

void MemoryManager::saveData(int* list, int sizeOfList, int addrStart, int numberOfElements) {
  for (int i = 0; i < numberOfElements; i++) {Serial.println("save");
    int addr = addrStart + i * 2;
    int value = (i < sizeOfList) ? list[i] : 0;

    int stored;
    EEPROM.get(addr, stored);
    if (stored != value) {
      EEPROM.put(addr, value);
    }
  }
}

void MemoryManager::loadData(int addrStart, int numberOfElements, int* buffer, size_t maxSize, size_t &outSize, bool filterOutZero) {
  outSize = 0;
  for (int i = 0; i < numberOfElements && outSize < maxSize; i++) {
    int addr = addrStart + i * 2;
    int value;
    EEPROM.get(addr, value);

    if (!filterOutZero || value != 0) {Serial.println("load:");Serial.println(value);
      buffer[outSize++] = value;
    }
  }
}
