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
