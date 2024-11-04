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
