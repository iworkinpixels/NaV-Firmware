// Raw Edit Mode - **For development only**
void rawEdit(){ 

  if (eventType == REFRESH){
    //oscStatus = readOneByte(oscSelect);
    menuState = 0;
    editMode = 0;
    oscStatus = patch[oscSelect];
    menuCursorSet(4,0);
    menuRef(oscName, "", "", "");
    menuDraw(4,0);
    for (byte i = 0; i < 16; i++){
      rawCurs(i,3);
    }
    rawCurs(0,3);  
    eventType = NONE;
  }

  else if (eventType == ENCODER){
    if (editMode == 0){
      if (menuState < 0) menuState = 15;
      if (menuState > 15) menuState = 0;
      rawCurs(menuState,3);
    }
    else {
      if (encoderDirection == true){
        menuState -= 1; //keep menuState from changing
        patch[menuState + oscOffset]++;
        writeOneByte(menuState + oscOffset, patch[menuState + oscOffset]);
        rawCurs(menuState,4);
      }
      if (encoderDirection == false){ // menuState from changing
        menuState += 1;
        patch[menuState + oscOffset]--;
        writeOneByte(menuState + oscOffset, patch[menuState + oscOffset]);
        rawCurs(menuState,4);
      }
    };
  }

  else if (eventType == ENTER){
    editMode = 1 - editMode; //Flips 1 to 0 and visa-versa
    if (editMode == 1) {
      rawCurs(menuState, 4);
      oldWaveform = patch[menuState + oscOffset];
    }
    else {
      rawCurs(menuState, 3);     
    }
  }

  else if (eventType == EXIT){
    if (editMode == 1){
      patch[menuState + oscOffset] = oldWaveform;
      writeOneByte(menuState + oscOffset, patch[menuState + oscOffset]);
      rawCurs(menuState, 3);
      editMode = 0;
    }
    else menuChange(MAINMENU);
  }
}

void rawCurs(int pos, byte cursType){

  lcd.setCursor(0,1);
  lcd.print("   ");
  lcd.setCursor(0,1);
  lcd.print(menuState + oscOffset);

  if (pos < 4) {
    printCursor(((pos*4))+4,0,cursType);
    lcd.print("   ");
    printCursor(((pos*4))+4,0,cursType);
    lcd.print(patch[pos + oscOffset]);
  }
  if (pos >= 4 && pos < 8) {
    printCursor(((pos-4)*4)+4,1,cursType);
    lcd.print("   ");
    printCursor(((pos-4)*4)+4,1,cursType);
    lcd.print(patch[pos + oscOffset]);
  }
  if (pos >= 8 && pos < 12){
    printCursor(((pos-8)*4)+4,2,cursType);
    lcd.print("   ");
    printCursor(((pos-8)*4)+4,2,cursType);
    lcd.print(patch[pos + oscOffset]);
  } 
  if (pos >= 12) {
    printCursor(((pos-12)*4)+4,3,cursType);
    lcd.print("   ");
    printCursor(((pos-12)*4)+4,3,cursType);
    lcd.print(patch[pos + oscOffset]);
  }
}




