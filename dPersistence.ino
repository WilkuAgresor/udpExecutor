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
