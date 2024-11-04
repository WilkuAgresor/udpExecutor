
//special purpose selector. Select the needed feature set depending on the purpose. 

// PWM
// Support for Adafruit PWM Servo driver (16 PWM outputs on each module)
// You can specify the number of modules and addresses in the defines below
//#define SPECIAL_PURPOSE_PWM

// Opentherm 
// Support for OpenTherm controller for the boiler.
#define SPECIAL_PURPOSE_OPENTHERM




char separator = ',';
bool DEBUG_LED_ON = false;

//byte mac[] = { 0xDA, 0xAD, 0xBE, 0x00, 0x00, 0x00}; 

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[PACKET_MAX_SIZE]; //buffer to hold incoming packet,

#ifdef SPECIAL_PURPOSE_PWM
const int PWM_EXPANDERS_COUNT = 1;
const int PWM_PORTS_PER_EXPANDER = 16;

#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
//PWM drivers
Adafruit_PWMServoDriver pwm[PWM_EXPANDERS_COUNT] = 
{
 Adafruit_PWMServoDriver(0x40)//,
// Adafruit_PWMServoDriver(0x41)
};
#endif

uint32_t sessionId = 0;

#define HOSTNAME "KuchniaIPExecutor"

// The sketch supports AVR based ethernet connection and ESP8266 based WiFi connection
#if defined(__AVR__)
  byte mac[] = { 0xDA, 0xAD, 0xBE, 0x00, 0x00, 0x00}; 
#elif defined(ARDUINO_ARCH_ESP8266)
  byte mac[] = { 0xDD, 0xDD, 0xBE, 0x00, 0x00, 0x00}; 
  #define EEPROM_SIZE 512 

  #define WIFI_SSID "Bosman"
  #define WIFI_PASS "gepardLuna"
#endif
