
void patchWrite(byte patchNum)
{

  byte i; //Loop Counter
  byte data = 0; //Array Index
  unsigned int address = (patchNum * 192);
  // Write Soundgin data
  do{
    Wire.beginTransmission(EEPROM1);
    Wire.write((int)((address) >> 8));   // High Byte
    Wire.write((int)((address) & 0xFF)); // Low Byte
    for (i=0; i < 16; i++){ // Write 16 Byte block of data
      Wire.write(patch[data]);
      data++;
    }
    Wire.endTransmission();
    address = address + 16; // Move address to next 16 Byte block
    delay(10);
  }
  while (data < 144); // Keep writing 16 Byte blocks untill all 144 registers are sent
  // Write Patch name
  Wire.beginTransmission(EEPROM1);
  Wire.write((int)((address) >> 8));   // High Byte
  Wire.write((int)((address) & 0xFF)); // Low Byte
  for (i=0; i < 16; i++){
    Wire.write((byte) patchName[i]);
  }
  Wire.endTransmission();
  delay(10);
}

void burnPatch(){ 
  byte param = 0;
  do{
    if (digitalRead(CTS)==LOW){
      writeOneByte(param, patch[param]);
      param++;
    }
  }
  while(param < 128);
  release(); 
}

//TEST MEMFREE
// function to return the amount of free RAM
int memoryFree()
{
  int freeValue;
  if((int)__brkval == 0)
    freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)__brkval);
  return freeValue;
}

void patchRead(byte patchNum)
{
  byte data = 0; // Array Index
  byte i; // Loop Counter
  unsigned int address = (patchNum * 192);

  for (i=0; i < 9; i++){ // Read 9 Blocks of 16 Bytes = 144 Bytes
    Wire.beginTransmission(EEPROM1);
    Wire.write((int)(address >> 8)); // High Byte
    Wire.write((int)(address & 0xFF)); // Low Byte
    Wire.endTransmission();

    Wire.requestFrom(EEPROM1, 16);
    while(Wire.available())
    {
      patch[data] = Wire.read();
      data++;
    }
    address = address + 16;
    delay(10);
  }
  //Read Patch Name
  data = 0;
  Wire.beginTransmission(EEPROM1);
  Wire.write((int)(address >> 8)); // High Byte
  Wire.write((int)(address & 0xFF)); // Low Byte
  Wire.endTransmission();

  Wire.requestFrom(EEPROM1, 16);
  while(Wire.available()) 
  {
    patchName[data] = Wire.read();
    data++;
  }
}


void eepromInit(){
  boolean init = true;
  byte i = 0;
  for (i = 0; i < 200; i++){
    if ((digitalRead(BUTTON_ENTER) == HIGH) | (digitalRead(BUTTON_EXIT) == HIGH)) init = false;
    delay(10);
  }

  if (init == true){
    lcd.clear();
    lcd.print(F("Initializing EEPROM"));
    patchWrite(0);
    for (i = 0; i < 15; i++){
      patchName[i] = 32;
    }
    patch[16] = 144;

    for (i = 1; i < 100; i++){
      lcd.setCursor(8,2);
      lcd.print("%");
      lcd.print(i + 1);
      patchWrite(i);
    }
  }
}

