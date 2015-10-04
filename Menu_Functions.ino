void menuDraw(int cursX, int cursY){
  lcd.setCursor (cursorX, cursorY);
  lcd.print(" "); //Erase Old Cursor Position
  cursorX = cursX;
  cursorY = cursY;
  lcd.setCursor (cursorX,cursorY);
  lcd.print(">"); //Draw New Cursor Position
}

void menuRef(String line1, String line2, String line3, String line4){
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  lcd.setCursor(0,2);
  lcd.print(line3);
  lcd.setCursor(0,3);
  lcd.print(line4);
}

void menuCursorSet(int x, int y){
  cursorX = x;
  cursorY = y;
}

byte readOneByte(byte arg){
  byte inByte = 0;
  soundgin.write(0x1B);
  soundgin.write((byte)0x00);
  soundgin.write(arg);

  while (soundgin.available()){
    inByte = soundgin.read();
    //lcd.print(inByte);
  }
  return inByte;
}

void writeOneMask(byte location, byte mask, byte arg){
  soundgin.write(0x1B);
  soundgin.write(0x04);
  soundgin.write(location);
  soundgin.write(mask);
  soundgin.write(arg);

  arg = mask & arg;
  mask = ~mask;
  byte result = mask & patch[location];
  patch[location] = arg | result;
}

void writeOneByte(byte location, byte arg){
  soundgin.write(0x1B);
  soundgin.write(0x01);
  soundgin.write(location);
  soundgin.write(arg);

  patch[location] = arg;
}


void lcdPrint (int x, int y, char text[]){
  lcd.setCursor(x,y);
  lcd.print(text);
}

void waveTypePrint (byte index){

  waveform = index; //global

  lcd.setCursor (6,1);
  switch(index){
  case 0:
    lcd.print(F("Sine    "));
    break;
  case 1:
    lcd.print(F("Triangle"));
    break;
  case 2:
    lcd.print(F("Saw     "));
    break;
  case 3:
    lcd.print(F("Ramp    "));
    break;
  case 4:
    lcd.print(F("Pulse   "));
    break;
  case 5:
    lcd.print(F("Noise   "));
    break;
  case 6:
    lcd.print(F("Level   "));
    break;
  case 7:
    lcd.print(F("Vocal   "));
    break;
  }
}

void printStar (byte x, byte y, byte star){
  lcd.setCursor (x,y);
  if (star){
    lcd.print("*");
  }
  else {
    lcd.print(" ");
  }
}

void printCursor (byte cursX, byte cursY, byte cursorType){
  lcd.setCursor (cursorX, cursorY);
  lcd.print(" "); //Erase Old Cursor Position
  cursorX = cursX;
  cursorY = cursY;
  lcd.setCursor (cursorX,cursorY);
  if (cursorType == 1){
    lcd.print("^");
  }
  else if (cursorType == 2){
    lcd.print("-");
  }
  else if (cursorType == 3){
    lcd.print(">");
  }
  else if (cursorType == 4){
    lcd.print(":");
  }
}

void menuChange(byte level){
  menuLevelState = level;
  menuState = 1;
  eventType = REFRESH;
  menuLevelSwitch();

}


