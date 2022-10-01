String processGetVal(int expander, int pin)
{  
  if(expander == -1)
  {
    return createReply(200, digitalRead(pin));
  }
  else
  {
    return createReply(200, pwm[expander].getPWM(pin));
  }
}

String processSetIp(int addr0, int addr1, int addr2, int addr3)
{
  ip = IPAddress(addr0, addr1, addr2, addr3);
  saveIpAddress();    
  return createReply(200);
}

bool needReply()
{
  String message(packetBuffer);  

  Serial.println(message);

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
  if(expander >= 0 && expander < PWM_EXPANDERS_COUNT && port >= 0 && port < PWM_PORTS_PER_EXPANDER)
  {
    setPwm(expander, port, dutyCycle);
  }
  else if (expander == -1)
  {
    if(dutyCycle == 1 || dutyCycle == 0)
    {
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
  
  if(msgType != "getVal" && msgType != "reset" && msgType != "setIp" && msgType != "getSessionId")
  {
    return createReply(500);
  }

  if(msgType == "reset")
  {
    pwmReset();
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
  else if(msgType == "setIp")
  {
    curSep = message.indexOf(separator, lastSep+1);
    int addr2 = message.substring(lastSep+1, curSep).toInt();
    lastSep = curSep;
    
    curSep = message.indexOf(separator, lastSep+1);
    int addr3 = message.substring(lastSep+1, curSep).toInt();
    lastSep = curSep;  

    return processSetIp(expander, port, addr2, addr3);
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

  Serial.print("Setting value: ");
  Serial.println(value);  
    
    processSetVal(expander, port, value);
  }
}
