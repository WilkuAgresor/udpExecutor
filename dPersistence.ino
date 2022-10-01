// This function takes a 6 byte array, and will fill every byte that is 0x00 with device unique data.
// The data is retrieved from a preset place in the Atmega EEPROM.
// If the particular EEPROM bytes are 0x00 or 0xFF, the data is ranomly generated and stored in EEPROM.

void getMacAddress(byte* macAddr) {
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
}

void clearEEPROM()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

const int ipEEPROMAddress[]       = {300, 301, 302, 303};

void saveIpAddress() 
{
  for (int i = 0; i <= 3; i++) 
  {
    EEPROM.write(ipEEPROMAddress[i], ip[i]);
  }
}

void readIpAddress()
{
  IPAddress readIp(EEPROM.read(ipEEPROMAddress[0]), EEPROM.read(ipEEPROMAddress[1]), EEPROM.read(ipEEPROMAddress[2]), EEPROM.read(ipEEPROMAddress[3]));
  ip = readIp;  
}
