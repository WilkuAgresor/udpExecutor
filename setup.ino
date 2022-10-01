void setup() 
{
   Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  getMacAddress(mac); 

  randomSeed(analogRead(0));
  sessionId = random(1,2000000);

  Serial.print("Device MAC address: ");
  Serial.println(String(mac[0], HEX)+":"+String(mac[1], HEX)+":"+String(mac[2], HEX)+":"+String(mac[3], HEX)+":"+String(mac[4], HEX)+":"+String(mac[5], HEX));

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP. Retrying");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    setDebugLedOn();
    blinkDebugLed(5000);    
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  Udp.begin(localPort);

  for(int i=0 ; i < PWM_EXPANDERS_COUNT ; i++)
  {
    Serial.print("Initializing pwm expander");

    pwm[i].begin();
    pwm[i].setOscillatorFrequency(27000000);
    pwm[i].setPWMFreq(900);
  }
  pwmReset();

  if(DEBUG_LED_ON)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  Serial.println("ready");  
}
