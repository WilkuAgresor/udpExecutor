
// Arduino Leonardo - Ethernet
#if (defined(__AVR__))
  #include <Ethernet.h>
  EthernetUDP UDP;
  #define PACKET_MAX_SIZE 250

// ESP8266, WEMOS D1 R2
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  #include <IPAddress.h>
  WiFiUDP UDP;
#else
  #error unsupported board
#endif

//persistent settings
#include <EEPROM.h>
