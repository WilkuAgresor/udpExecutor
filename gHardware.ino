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
