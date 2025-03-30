
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
// This function takes a 6 byte array, and will fill every byte that is 0x00 with device unique data.
// The data is retrieved from a preset place in the Atmega EEPROM.
// If the particular EEPROM bytes are 0x00 or 0xFF, the data is randomly generated and stored in EEPROM.

void getMacAddress(byte* macAddr) {

#if defined(ARDUINO_ARCH_ESP8266)
  EEPROM.begin(EEPROM_SIZE);  
#endif

  int eepromOffset = 128;
  int b = 0; 
  for (int c = 0; c < 6; c++) {
    b = 0;
    if(macAddr[c] == 0) {
      b = EEPROM.read(eepromOffset + c);
      if(b == 0 || b == 255) {
         b = random(0, 255);
         EEPROM.write(eepromOffset + c, b);
      }
    macAddr[c] = b;
    }
  }
  
#if defined(ARDUINO_ARCH_ESP8266)
  EEPROM.commit();
#endif
}

void clearEEPROM()
{
#if defined(ARDUINO_ARCH_ESP8266)
  EEPROM.begin(EEPROM_SIZE);
#endif

  for (unsigned int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  
#if defined(ARDUINO_ARCH_ESP8266)
  EEPROM.commit();
#endif
}
void setDebugLedOn()
{
    if(!DEBUG_LED_ON)
    {
      pinMode(LED_BUILTIN, OUTPUT);
      DEBUG_LED_ON = true;
    }
}

void setPinValue(int pin, int state)
{
  if(state != 0)
  {
    digitalWrite(pin, HIGH);   
  }
  else
  {
    digitalWrite(pin, LOW);   
  }
}

void blinkDebugLed(int millis)
{
  if(DEBUG_LED_ON)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(millis);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(millis);    
  }
}

#ifdef SPECIAL_PURPOSE_PWM
void setPwm(int pwmDriver, int pin, int dutyCycle)
{
    pwm[pwmDriver].setPin(pin, dutyCycle);
}

void pwmReset()
{
  for(int i=0 ; i < PWM_EXPANDERS_COUNT ; i++)
  {   
    for(int port = 0; port < PWM_PORTS_PER_EXPANDER; port++)
    {
      setPwm(i, port, 0);
    }    
  }
}
#endif

void generateSessionId()
{
#if defined(__AVR__)
  randomSeed(analogRead(0));
#elif defined(ARDUINO_ARCH_ESP8266)
  randomSeed(ESP.getCycleCount());
#endif  

  sessionId = random(1,2000000);
}
String createReply(int retCode)
{
  String reply = "sts";
  reply.concat(separator);
  reply.concat(String(retCode));
  return reply;  
}

String createReply(int retCode, int retVal)
{
  String reply = "rtv";
  reply.concat(separator);
  reply.concat(String(retCode));
  reply.concat(separator);
  reply.concat("val");
  reply.concat(separator);
  reply.concat(String(retVal));
  return reply;  
}

String createSessionReply()
{
  String reply = "sessionId";
  reply.concat(separator);
  reply.concat(String(sessionId));
  reply.concat(separator);
  return reply;
}

#ifdef SPECIAL_PURPOSE_OPENTHERM

#endif

void networkBegin()
{
  #if defined(__AVR__)
  {
    getMacAddress(mac);
    // start the Ethernet connection:
    //Serial.print("My MAC address: ");
    //Serial.println(String(mac[0], HEX)+":"+String(mac[1], HEX)+":"+String(mac[2], HEX)+":"+String(mac[3], HEX)+":"+String(mac[4], HEX)+":"+String(mac[5], HEX));
    
    //Serial.println("Initialize Ethernet with DHCP:");
    while (Ethernet.begin(mac) == 0) {
      //Serial.println("Failed to configure Ethernet using DHCP. Retrying");
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        //Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      } else if (Ethernet.linkStatus() == LinkOFF) {
        //Serial.println("Ethernet cable is not connected.");
      }
//      setDebugLedOn();
//      blinkDebugLed(5000);    
    }
    //Serial.print("Connected! My IP address: ");
    //Serial.println(Ethernet.localIP());


//    Ethernet.setRetransmissionTimeout(200);
//    Ethernet.setRetransmissionCount(2);
  }
  #elif defined(ARDUINO_ARCH_ESP8266)
  {
    WiFi.hostname(HOSTNAME);
    
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    //Serial.print("Connecting to ");
    //Serial.print(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(100);
      //Serial.print(".");
    }  
    //Serial.print("\nConnected! My IP address: ");
    //Serial.println(WiFi.localIP());
    //Serial.print("My MAC address: ");
    //Serial.println(WiFi.macAddress());    
  }
  #endif
}

void ethernetMaintain()
{
#if defined(__AVR__)
{     
  switch (Ethernet.maintain()) {
    case 1:
      //renewed fail
      //Serial.println("Error: renewed fail");
      break;

    case 2:
      //renewed success
      //Serial.println("Renewed success");
      //print your local IP address:
      //Serial.print("My IP address: ");
      //Serial.println(Ethernet.localIP());
      break;

    case 3:
      //rebind fail
      //Serial.println("Error: rebind fail");
      break;

    case 4:
      //rebind success
      //Serial.println("Rebind success");
      //print your local IP address:
      //Serial.print("My IP address: ");
      //Serial.println(Ethernet.localIP());
      break;

    default:
      //nothing happened
      break;
  }
}
#elif defined(ARDUINO_ARCH_ESP8266)
{
  ////Serial.println("No implementation of dhcp maintain for esp8266 yet");
}
#endif


}
#if (defined(SPECIAL_PURPOSE_OPENTHERM))

#include <OpenTherm.h>

int openthermRefreshCounter = 0;
int maintainCounter = 0;

const int inPin = 2;  //for Arduino, 4 for ESP8266 (D2), 21 for ESP32
const int outPin = 3; //for Arduino, 5 for ESP8266 (D1), 22 for ESP32
OpenTherm ot(inPin, outPin);
void IRAM_ATTR handleInterrupt() {
    ot.handleInterrupt();
}
//in
bool enableCentralHeating = true;
bool enableHotWater = true;
float centralHeatingTempSet = 25;
float maxCentralHeatingTempSet = 35;
float hotWaterTempSet = 40;

//out
bool isCentralHeatingOn = false;
bool isHotWaterOn = false;
bool isFlameOn = false;

float centralHeatingTemp = 25;
float maxCentralHeatingTemp = 25;
float hotWaterTemp = 25;
int connectionStatus = 1;
float returnTemp = 0;
float pressure = 0;

void openthermMaintain()
{
    //get boiler status
    unsigned long response = ot.setBoilerStatus(enableCentralHeating, enableHotWater);
    OpenThermResponseStatus responseStatus = ot.getLastResponseStatus();            
    if (responseStatus == OpenThermResponseStatus::SUCCESS) {
      connectionStatus = 0;
      isCentralHeatingOn = ot.isCentralHeatingActive(response);
      isHotWaterOn = ot.isHotWaterActive(response);
      isFlameOn = ot.isFlameOn(response);       
    }
    if (responseStatus == OpenThermResponseStatus::NONE) {
      connectionStatus = 1;
      return;
    }
    else if (responseStatus == OpenThermResponseStatus::INVALID) {
      connectionStatus = 2;
      return;
    }
    else if (responseStatus == OpenThermResponseStatus::TIMEOUT) {
      connectionStatus = 3;
      return;
    }

    switch(maintainCounter)
    {
      case 0:
          getMaxCentralHeatingTemp();
          getCentralHeatingTemp();
        break;
      case 1:
          getHotWaterTemp();
          getReturnTemp();
        break;
      case 2:
          getPressure();
        break;
      case 3:
          setCentralHeatingTemp();
        break;
      case 4: 
          setHotWaterTemp();
        break;
      case 5:
          setMaxCentralHeatingTemp();
        break;
      default:
        break;     
    }

    maintainCounter++;    
    if(maintainCounter > 5)
    {
      maintainCounter = 0;
    }
}

void getMaxCentralHeatingTemp()
{
  unsigned long request = ot.buildRequest(OpenThermMessageType::READ_DATA, OpenThermMessageID::MaxTSet, 0);
  unsigned long response = ot.sendRequest(request);
  if(ot.isValidResponse(response))
  {
    maxCentralHeatingTemp = ot.getFloat(response);
  }
  else
  {
    maxCentralHeatingTemp = 0;
  }
}

void setMaxCentralHeatingTemp()
{    
  unsigned int data = ot.temperatureToData(maxCentralHeatingTempSet);
  unsigned long request = ot.buildRequest(OpenThermMessageType::WRITE_DATA, OpenThermMessageID::MaxTSet, data);
  unsigned long response = ot.sendRequest(request);
  
  if(!ot.isValidResponse(response))
  {
    //Serial.println("set room temp failed");
  } 
}

void setCentralHeatingTemp()
{
  ot.setBoilerTemperature(centralHeatingTempSet);
}

void getCentralHeatingTemp()
{
  centralHeatingTemp = ot.getBoilerTemperature();
}

void getHotWaterTemp()
{
  hotWaterTemp = ot.getDHWTemperature(); 
}

void setHotWaterTemp()
{
    ot.setDHWSetpoint(hotWaterTempSet);
}

void getReturnTemp()
{
   returnTemp = ot.getReturnTemperature();
}

void getPressure()
{
  pressure = ot.getPressure();
}

bool isOpenthermMessage(const String& messageType)
{
  return messageType.equals("opentherm");  
}

//read-only properties
#define JSON_IS_CENTRAL_HEATING_ON "icho"
#define JSON_IS_HOT_WATER_ON "ihw"
#define JSON_IS_FLAME_ON "ifo"
#define JSON_CENTRAL_HEATING_TEMP "cht"
#define JSON_MAX_CENTRAL_HEATING_TEMP "mcht"
#define JSON_HOT_WATER_TEMP "hwt"
#define JSON_CONNECTION_STATUS "cons"
#define JSON_RETURN_TEMP "ret"
#define JSON_PRESSURE "pres"

String createHeatingReply()
{
  String reply = JSON_CENTRAL_HEATING_TEMP;
  reply.concat(separator);
  reply.concat(centralHeatingTemp);
  reply.concat(separator);
  
  reply.concat(JSON_MAX_CENTRAL_HEATING_TEMP);
  reply.concat(separator);
  reply.concat(maxCentralHeatingTemp);
  reply.concat(separator);
  
  reply.concat(JSON_HOT_WATER_TEMP);
  reply.concat(separator);
  reply.concat(hotWaterTemp);
  reply.concat(separator);
  
  reply.concat(JSON_IS_CENTRAL_HEATING_ON);
  reply.concat(separator);
  reply.concat(isCentralHeatingOn);
  reply.concat(separator);

  reply.concat(JSON_IS_HOT_WATER_ON);
  reply.concat(separator);
  reply.concat(isHotWaterOn);
  reply.concat(separator);

  reply.concat(JSON_IS_FLAME_ON);
  reply.concat(separator);
  reply.concat(isFlameOn);
  reply.concat(separator);
  
  reply.concat(JSON_CONNECTION_STATUS);
  reply.concat(separator);
  reply.concat(connectionStatus);
  reply.concat(separator);

  reply.concat(JSON_RETURN_TEMP);
  reply.concat(separator);
  reply.concat(returnTemp);
  reply.concat(separator);

  reply.concat(JSON_PRESSURE);
  reply.concat(separator);
  reply.concat(pressure);
  reply.concat(separator);  

  return reply;
}

//settable properties
#define JSON_ENABLE_CENTRAL_HEATING "ec"
#define JSON_ENABLE_HOT_WATER "ehw"
#define JSON_CENTRAL_HEATING_TEMP_SET "chts"
#define JSON_MAX_CENTRAL_HEATING_TEMP_SET "mchts"
#define JSON_HOT_WATER_TEMP_SET "hwts"

String handleOpenthermMessage(const String& message)
{
  //ignore the first attribute - opentherm type
  int lastSep = 0;
  int curSep = message.indexOf(separator, lastSep);
  lastSep = curSep;

  curSep = message.indexOf(separator, lastSep+1);
  String commandType = message.substring(lastSep+1, curSep);
  lastSep = curSep;

  //Serial.print("opentherm message type: ");
  //Serial.println(commandType);
  
  if(commandType.equals("set"))
  {
    curSep = message.indexOf(separator, lastSep+1);
    String attributeType = message.substring(lastSep+1, curSep);
    lastSep = curSep;

    if(attributeType.equals(JSON_CENTRAL_HEATING_TEMP_SET))
    {
      curSep = message.indexOf(separator, lastSep+1);
      int value = message.substring(lastSep+1, curSep).toFloat();
      lastSep = curSep;

      centralHeatingTempSet = value;     
      return createReply(200);   
    }
    else if (attributeType.equals(JSON_MAX_CENTRAL_HEATING_TEMP_SET))
    {
      curSep = message.indexOf(separator, lastSep+1);
      int value = message.substring(lastSep+1, curSep).toFloat();
      lastSep = curSep;

      maxCentralHeatingTempSet = value;  
      return createReply(200);   

    }
    else if (attributeType.equals(JSON_HOT_WATER_TEMP_SET))
    {
      curSep = message.indexOf(separator, lastSep+1);
      int value = message.substring(lastSep+1, curSep).toFloat();
      lastSep = curSep;

      hotWaterTempSet = value;
      return createReply(200); 
    }
    else if(attributeType.equals(JSON_ENABLE_CENTRAL_HEATING))
    {
      curSep = message.indexOf(separator, lastSep+1);
      int value = message.substring(lastSep+1, curSep).toInt();
      lastSep = curSep;

      enableCentralHeating = (value == 0) ? false: true;
      return createReply(200);
    }
    else if(attributeType.equals(JSON_ENABLE_HOT_WATER))
    {
      curSep = message.indexOf(separator, lastSep+1);
      int value = message.substring(lastSep+1, curSep).toInt();
      lastSep = curSep;

      enableHotWater = (value == 0) ? false: true;
      return createReply(200);
    }    
  }
  else if(commandType.equals("get"))
  {
    return createHeatingReply();
  }
  
   return createReply(500);  
}


#endif
String processGetVal(int expander, int pin)
{  
  if(expander == -1)
  {
    return createReply(200, digitalRead(pin));
  }
  #ifdef SPECIAL_PURPOSE_PWM
  else
  {
    return createReply(200, pwm[expander].getPWM(pin));
  }
  #endif
  return createReply(404);
}

bool needReply()
{
  String message(packetBuffer);  

  //Serial.println(message);

  int lastSep = 0;  
  int curSep = message.indexOf(separator, lastSep);;
  String msgType = message.substring(lastSep, curSep);

  if(msgType == "setVal")
  {
    return false;  
  }
  
  return true;
}

void processSetVal(int expander, int port, int dutyCycle)
{
  #ifdef SPECIAL_PURPOSE_PWM
  if(expander >= 0 && expander < PWM_EXPANDERS_COUNT && port >= 0 && port < PWM_PORTS_PER_EXPANDER)
  {
    setPwm(expander, port, dutyCycle);
  }
  #endif
  if (expander == -1) //digital pins (2-9) and analog pins A1-A5 (15-19). A0 is used for analog random seed
  {
    if(dutyCycle == 1 || dutyCycle == 0)
    {
      pinMode(port, OUTPUT);
      setPinValue(port, dutyCycle); 
    } 
  }
}

String processMessage()
{
  String message(packetBuffer);  
  
  int lastSep = 0;

  int curSep = message.indexOf(separator, lastSep);;
  String msgType = message.substring(lastSep, curSep);
  lastSep = curSep;

  #ifdef SPECIAL_PURPOSE_OPENTHERM
  if(isOpenthermMessage(msgType))
  {
    return handleOpenthermMessage(message);
  }
  #endif

  if(msgType.equals("eepromClear"))
  {
    clearEEPROM();
    return createReply(200);
  }
  
  if(!msgType.equals("getVal") && !msgType.equals("reset") && !msgType.equals("getSessionId"))
  {
    return createReply(500);
  }

  if(msgType == "reset")
  {
#ifdef SPECIAL_PURPOSE_PWM
    pwmReset();
#endif
    return createReply(200);
  }

  if(msgType == "getSessionId")
  {
    return createSessionReply();
  }

  curSep = message.indexOf(separator, lastSep+1);
  int expander = message.substring(lastSep+1, curSep).toInt();
  lastSep = curSep;

  curSep = message.indexOf(separator, lastSep+1);
  int port = message.substring(lastSep+1, curSep).toInt();
  lastSep = curSep;

  if(msgType == "getVal")
  {
    return processGetVal(expander, port);
  }
  
  return createReply(500);
}

void processMessageNoReply()
{
  String message(packetBuffer);  
  int lastSep = 0;
  
  int curSep = message.indexOf(separator, lastSep);;
  String msgType = message.substring(lastSep, curSep);
  lastSep = curSep;

  curSep = message.indexOf(separator, lastSep+1);
  int expander = message.substring(lastSep+1, curSep).toInt();
  lastSep = curSep;

  curSep = message.indexOf(separator, lastSep+1);
  int port = message.substring(lastSep+1, curSep).toInt();
  lastSep = curSep;

  if(msgType == "setVal")
  {
    curSep = message.indexOf(separator, lastSep+1);
    int value = message.substring(lastSep+1, curSep).toInt();

  //Serial.print("Setting value: ");
  //Serial.println(value);  
    
    processSetVal(expander, port, value);
  }
}
void setup() 
{
//   //Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  randomSeed(analogRead(0));

  //digital pins output default HIGH
  for(int i= 2; i< 10; i++)
  {
    digitalWrite(i, HIGH);
    pinMode(i,OUTPUT);
  }

  for (int i = A1; i <= A5; i++) 
  {
    digitalWrite(i, HIGH);
    pinMode(i, OUTPUT); 
  }
  
  networkBegin(); 
  generateSessionId();
  
  UDP.begin(localPort);
#ifdef SPECIAL_PURPOSE_PWM
  for(int i=0 ; i < PWM_EXPANDERS_COUNT ; i++)
  {
    //Serial.println("Initializing pwm expander");

    pwm[i].begin();
    pwm[i].setOscillatorFrequency(27000000);
    pwm[i].setPWMFreq(900);
  }
  pwmReset();
#endif

#ifdef SPECIAL_PURPOSE_OPENTHERM
  ot.begin(handleInterrupt);
#endif

  if(DEBUG_LED_ON)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  //Serial.println("ready");  
}
void loop() {

  
//  //refresh the DHCP lease
//  ethernetMaintain();

#if (defined(SPECIAL_PURPOSE_OPENTHERM))
 openthermRefreshCounter++;
 if(openthermRefreshCounter > 900)
 {
    //Serial.println("Opentherm refresh");
    openthermRefreshCounter = 0;
    openthermMaintain();
    //Serial.println("Opentherm refresh end");
 }
#endif
  
  // if there's data available, read a packet
  int packetSize = UDP.parsePacket();
  
  if(packetSize > PACKET_MAX_SIZE)
  {
    //Serial.println("Received packet too big");
      while(UDP.available())
      UDP.read();
  }
  else if (packetSize != 0)
  {       
    // read the packet into packetBufffer
    int len = UDP.read(packetBuffer, PACKET_MAX_SIZE);
    
    if (len > 0)
    {
      packetBuffer[len] = '\0';
    }
    
    //Serial.println(packetBuffer);

    if(needReply())
    {
      String reply = processMessage();
  
      // send a reply, to the IP address and port that sent us the packet we received
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.print(reply);
      UDP.endPacket();
    }
    else
    {
      processMessageNoReply();
    }
  }
   
//  delay(20);
}
