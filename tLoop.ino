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
