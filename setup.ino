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
