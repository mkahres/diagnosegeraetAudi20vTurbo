#include "KLineManager.h"
#include "Constants.h"
#include "communication.h"
#include "configuration.h"

// Debugging can be enabled in "configuration.h" in order to print connection-related info on the Serial Monitor.
#if debug_info
  KLineKWP1281Lib diag(beginFunction, endFunction, sendFunction, receiveFunction, TX_pin, is_full_duplex, &Serial);
#else
  KLineKWP1281Lib diag(beginFunction, endFunction, sendFunction, receiveFunction, TX_pin, is_full_duplex);
#endif

uint8_t measurement_buffer[80];

// Each fault code takes up 3 bytes.
// You can increase the value below if you get "Too many faults for the given buffer size" during the test.
#define DTC_BUFFER_MAX_FAULT_CODES 20
uint8_t faults[3 * DTC_BUFFER_MAX_FAULT_CODES];

int sensorList[10];


void KLineManager::connect()
{
  // If debugging bus traffic was enabled, attach the debugging function.
  #if debug_traffic
    diag.KWP1281debugFunction(KWP1281debugFunction);
  #endif
  
  // Connect to the module.
  diag.connect(connect_to_module, module_baud_rate, false);

  //Workaround da beim 3B wohl mehr Serialnummer-Daten kommen und es sonst nicht funktioniert
  diag.update();
  diag.update();
}

void KLineManager::getSensorData(Adafruit_SH1106G &display, 
                                  U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, 
                                  DisplayManager &displayManager,
                                  int* buffer, size_t maxSize, size_t &outSize)
{
  outSize = 0;

  uint8_t amount_of_measurements = 0;
  KLineKWP1281Lib::executionStatus readGroup_status = 
      diag.readGroup(amount_of_measurements, GROUP_TO_READ, measurement_buffer, sizeof(measurement_buffer));
  
  if (readGroup_status == KLineKWP1281Lib::GROUP_BASIC_SETTINGS) 
  {
    size_t copyCount = min((size_t)10, maxSize); // Begrenzen auf maximalen Buffer
    for (size_t i = 0; i < copyCount; ++i) 
    {
      buffer[outSize++] = measurement_buffer[i];
    }
  } 
  else 
  {
    // Fehlerbehandlung – Buffer mit Nullen füllen
    size_t fillCount = min((size_t)10, maxSize);
    for (size_t i = 0; i < fillCount; ++i) 
    {
      buffer[i] = 0;
    }
    outSize = fillCount;
  }
}


void KLineManager::getFaultMemory(Adafruit_SH1106G &display, U8G2_FOR_ADAFRUIT_GFX &u8g2_for_adafruit_gfx, DisplayManager &displayManager, 
  int* buffer, size_t maxSize, size_t &outSize)
{
  uint8_t amount_of_fault_codes = 0;

  connect();

  if (diag.readFaults(amount_of_fault_codes, faults, sizeof(faults)) == KLineKWP1281Lib::SUCCESS)
  {
    uint8_t available_fault_codes = amount_of_fault_codes;
    if (sizeof(faults) < available_fault_codes * 3)
    {
      displayManager.showError(display, u8g2_for_adafruit_gfx, "Mehr als 20 Fehler!");
      disconnect();
    }

    outSize = amount_of_fault_codes;
    for (uint8_t i = 0; i < available_fault_codes; i++)
    {
      buffer[i] = (int) KLineKWP1281Lib::getFaultCode(i, available_fault_codes, faults, sizeof(faults));
    }
    disconnect();
  }
}


uint16_t current_output_test;

int KLineManager::performOutputTests()
{
  KLineKWP1281Lib::executionStatus outputTests_status = diag.outputTests(current_output_test);

  return current_output_test;
}

void KLineManager::clearFaultMemory()
{
  connect();
  diag.clearFaults();
  disconnect();
}

void KLineManager::update()
{
  diag.update();
}

void KLineManager::disconnect()
{
  diag.disconnect(false);
  Serial1.end();
}

void KLineManager::convertUint8ToInt(const uint8_t* input, int* output, size_t length) 
{
  for (size_t i = 0; i < length; i++) 
  { 
    output[i] = (int)input[i];
  }
}
