// Select your target module address:
#define connect_to_module 0x01

// Select your target module speed:
#define module_baud_rate 9600

// Enable/disable printing library debug information on the Serial Monitor.
// You may change the debug level in "KLineKWP1281Lib.h".
#define debug_info false
// Enable/disable printing bus traffic on the Serial Monitor.
#define debug_traffic false

#define is_full_duplex true
#define K_line Serial1
#define TX_pin 18

#ifndef K_line
  #error Please select an option in configuration.h!
#endif
