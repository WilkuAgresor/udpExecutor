
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
