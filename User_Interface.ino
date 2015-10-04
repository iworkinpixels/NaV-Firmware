// User Interface Functions

// re_state function - Check for Rotary Encoder change
// Use relookup Table to check if encoder is in valid state
unsigned char re_state() {
  unsigned char pinstate = (digitalRead(ROTARY_PIN2) << 1) | digitalRead(ROTARY_PIN1);
  //state = relookup[state & 0xf][pinstate];
  state = pgm_read_byte(&relookup[state & 0xf][pinstate]);
  return (state & 0x30);
}

// menu function - Check state of Rotary Encoder and Buttons - Update LCD
void menu() {
  unsigned char result = re_state();

  if (result) {
    if (result == DIR_CCW){
      menuState--;
      eventType = ENCODER;
      encoderDirection = false;

      menuLevelSwitch();
    }
    else {
      menuState++;
      //menuMainSwitch();
      eventType = ENCODER;
      encoderDirection = true;
      menuLevelSwitch();
    }  

  }
  // Enter Button
  if ( digitalRead(BUTTON_ENTER) == LOW & enterPress == false ) {
    enterPress = true;
    enterTime = millis();
    eventType = ENTER;
    menuLevelSwitch();
  }
  else if ( digitalRead(BUTTON_ENTER) == HIGH & millis() > enterTime + TIMEOUT ) {
    enterPress = false;
  }
  // Exit Button
  if ( digitalRead(BUTTON_EXIT) == LOW & exitPress == false ) {
    exitPress = true;
    exitTime =millis();
    eventType = EXIT;
    menuLevelSwitch();
  }
  else if ( digitalRead(BUTTON_EXIT) == HIGH & millis() > exitTime + TIMEOUT ) {
    exitPress = false;
  }
}

void menuLevelSwitch(){
  switch(menuLevelState){
  case 0:
    menuPatch();
    break;
  case 1:
    menuMainSwitch();
    break;
  case 2:
    menuOscSwitch();
    break;
  case 3: 
    menuOscWaveform();
    break;
  case 4:
    waveformSelect();
    break;
  case 5:
    patchNameSet();
    break;
  case 6:
    rawEdit();
    break;
  case 7:
    adsr();
    break;
  case 99:
    savePatch();
    break;
  }
}



void menuPatch(){
  // Draw menu if required
  if (eventType == REFRESH){
    editMode = 1;
    menuCursorSet(0,0);
    menuRef("" , "", " Edit", "");
    lcd.setCursor(1,0);
    if (patchNumber < 10) lcd.print("0");
    lcd.print( patchNumber );
    lcd.print(":");
    lcd.print(patchName);
    printCursor(0,0,4);
    eventType = NONE;
  }
  if (editMode == 1) menuState = 1;

  switch(menuState){
  case 0:
    menuState = 2;
    menuDraw(0,2);
    break;
  case 1:
    if (eventType == ENCODER){ 

      if (editMode == 0) {
        menuDraw(0,0);
      }

      else{
        if (encoderDirection == true){
          patchNumber++;
        }
        else if (encoderDirection == false){
          //menuState = 1;
          patchNumber--;
        }
        if (patchNumber == 100) patchNumber = 0;
        else if (patchNumber == 255) patchNumber = 99;
        release();
        lcdPrint(1,0,"  ");
        lcd.setCursor(1,0);
        if (patchNumber < 10) lcd.print("0");
        lcd.print(patchNumber);
        lcd.print(":");
        patchRead(patchNumber);
        lcd.print(patchName);
        burnPatch();
        //eventType = REFRESH;
      }
    }

    else if (eventType == ENTER){
      editMode = 1 - editMode; //Flips 1 to 0 and visa-versa
      if (editMode == 1) {
        printCursor(0,0,4);
      }
      else {
        printCursor(0,0,3);    
      }
    }
    break; 
  case 2://Edit
    if (eventType == ENCODER){
      menuDraw(0,2);
    }
    else if (eventType == ENTER){
      menuChange(MAINMENU);
    }
    break;
  case 3:
    menuState = 1;
    menuDraw(0,0);//call menu drawing function
    break;
  }
}//End menuPatch

void menuMainSwitch(){

  // Draw menu if required
  if (eventType == REFRESH){
    menuCursorSet(3,0);
    //menuRef("Osc A1 A2 A3", "    B1 B2 B3", "Mix A B Ctrl", "    Trig Name");
    lcd.clear();
    lcd.print(F("Osc A1 A2 A3"));
    lcd.setCursor(0,1);
    lcd.print(F("    B1 B2 B3"));
    lcd.setCursor(0,2);
    lcd.print(F("Mix A B Ctrl"));
    lcd.setCursor(0,3);
    lcd.print(F("    Trig Name"));

    //lcd.setCursor (17,3);//TEST ONLY
    //lcd.print(memoryFree()); // print the free memory TEST ONLY

    menuDraw(3,0);
    eventType = NONE;
  }
  if (eventType == EXIT){
    menuChange(PATCHSAVE);
  }
  switch(menuState){
  case 0:
    menuState = 11;
    menuDraw(8,3);//call menu drawing function
    break;
  case 1:
    if (eventType == ENCODER){
      menuDraw(3,0);//call menu drawing function
    }
    else if (eventType == ENTER){
      oscSelect = OSCA1;
      oscName = "A1";
      oscOffset = 16;
      menuChange(OSCMENU);
    }
    else if (eventType == EXIT){

    }
    break;

  case 2:
    if (eventType == ENCODER){
      menuDraw(6,0);
    }
    else if (eventType == ENTER){
      oscSelect = OSCA2;
      oscName = "A2";
      oscOffset = 32;
      menuChange(OSCMENU);
    }
    break;
  case 3:
    if (eventType == ENCODER){
      menuDraw(9,0);
    }
    else if (eventType == ENTER){
      oscSelect = OSCA3;
      oscName = "A3";
      oscOffset = 48;
      menuChange(OSCMENU);
    }
    break;
  case 4:
    if (eventType == ENCODER){
      menuDraw(3,1);
    }
    else if (eventType == ENTER){
      oscSelect = OSCB1;
      oscName = "B1";
      oscOffset = 80;
      menuChange(OSCMENU);
    }
    break;
  case 5:
    if (eventType == ENCODER){
      menuDraw(6,1);
    }
    else if (eventType == ENTER){
      oscSelect = OSCB2;
      oscName = "B2";
      oscOffset = 96;
      menuChange(OSCMENU);
    }
    break;
  case 6:
    if (eventType == ENCODER){
      menuDraw(9,1);
    }
    else if (eventType == ENTER){
      oscSelect = OSCB3;
      oscName = "B3";
      oscOffset = 112;
      menuChange(OSCMENU);
    }
    break;
  case 7:
    if (eventType == ENCODER){
      menuDraw(3,2);
    }
    else if (eventType == ENTER){
      oscName = "MxA";
      oscOffset = 0;
      menuChange(RAW);
    }
    break;
  case 8:
    if (eventType == ENCODER){
      menuDraw(5,2);
    }
    else if (eventType == ENTER){
      oscName = "MxB";
      oscOffset = 64;
      menuChange(RAW);
    }
    break;
  case 9:
    if (eventType == ENCODER){
      menuDraw(7,2);
    }
    else if (eventType == ENTER){
      oscName = "Ctrl";
      oscOffset = 128;
      menuChange(RAW);
    }
    break;
  case 10:
    menuDraw(3,3);//call menu drawing function
    break;
  case 11://Name
    if (eventType == ENCODER){
      menuDraw(8,3);
    }
    else if (eventType == ENTER){
      menuChange(PATCHNAMEMENU);
    }
    break;
  case 12://End
    menuState = 1;//call menu drawing function
    menuDraw(3,0);
    break;
    //
  case 20: //Main Menu Osc A1
    break;
  case 21:
    menuDraw(10,1);//call menu drawing function
    break;
  case 22:
    menuDraw(0,2);//call menu drawing function
    break;
  case 23:
    menuDraw(10,2);//call menu drawing function
    break;

  }
}

void menuOscSwitch(){

  // Draw menu if required
  if (eventType == REFRESH){
    menuCursorSet(0,1);
    //menuRef("[Osc " + oscName + " Menu]", " Amplitude ADSR", " Frequency Waveform", " Raw");
    lcd.clear();
    lcd.print("[Osc " + oscName + " Waveform]");
    lcd.setCursor(0,1);
    lcd.print(F(" Amplitude ADSR"));
    lcd.setCursor(0,2);
    lcd.print(F(" Frequency Waveform"));
    lcd.setCursor(0,3);
    lcd.print(F(" Raw"));

    menuDraw(0,1);
    eventType = NONE;
  }
  if (eventType == EXIT){
    menuChange(MAINMENU);
  }

  switch(menuState){
  case 0:
    menuState = 5;
    menuDraw(0,3);//call menu drawing function
    break;
  case 1:
    if (eventType == ENCODER){
      menuDraw(0,1);//call menu drawing function
    }
    else if (eventType == ENTER){
    }
    break;
  case 2:// ADSR
    if (eventType == ENCODER){
      menuDraw(10,1);//call menu drawing function
    }
    else if (eventType == ENTER){
      menuChange(ADSR);
    }
    break;
  case 3:
    if (eventType == ENCODER){
      menuDraw(0,2);//call menu drawing function
    }
    else if (eventType == ENTER){
    }
    break;
  case 4:
    if (eventType == ENCODER){
      menuDraw(10,2);//call menu drawing function
    }
    else if (eventType == ENTER){
      menuChange(OSCWAVEMENU);
    }
    break;
  case 5:// Raw Edit mode *Development Only*
    if (eventType == ENCODER){
      menuDraw(0,3);//call menu drawing function
    }
    else if (eventType == ENTER){
      menuChange(RAW);
    }
    break;
  case 6:
    menuState = 1;
    menuDraw(0,1);//call menu drawing function
    break;
  }
}




void menuOscWaveform(){
  // Draw menu if required
  if (eventType == REFRESH){
    oscStatus = patch[oscSelect];
    menuCursorSet(0,1);
    //menuRef("[Osc " + oscName + " Waveform]", " Wave=", " On  Abs  Ovf  Bnd", " PMd  PWM=");
    lcd.clear();
    lcd.print("[Osc " + oscName + " Waveform]");
    lcd.setCursor(0,1);
    lcd.print(F(" Wave="));
    lcd.setCursor(0,2);
    lcd.print(F(" On  Abs  Ovf  Bnd"));
    lcd.setCursor(0,3);
    lcd.print(F(" PMd  PWM="));

    menuDraw(0,1);  
    //Update Waveform Type
    waveTypePrint (patch[oscSelect] & 0x07);
    //Update Abs
    printStar(8,2, (oscStatus & 0x08));
    //Update Ovf
    printStar(13,2, (oscStatus & 0x10));
    //Update Bnd
    printStar(18,2, (oscStatus & 0x20));
    //Update On
    printStar(3,2, (oscStatus & 0x80));
    //
    eventType = NONE;
  }
  if (eventType == EXIT){
    menuChange(OSCMENU);
  }

  switch(menuState){
  case 0:
    menuState = 7;
    menuDraw(5,3);
    break;
  case 1: //Wave
    if (eventType == ENCODER){
      menuDraw(0,1);
    }
    else if (eventType == ENTER){
      menuChange(4); //waveformSelect    
    }
    break;
  case 2: //On
    if (eventType == ENCODER){
      menuDraw(0,2);
    }
    else if (eventType == ENTER){
      oscStatus = (oscStatus ^ 0x80); //Bitwize XOR flips bit 8
      writeOneMask(oscSelect, 0x80, oscStatus);
      printStar(3,2, (oscStatus & 0x80));
    }
    break;
  case 3: //Abs
    if (eventType == ENCODER){
      menuDraw(4,2);
    }
    else if (eventType == ENTER){
      oscStatus = (oscStatus ^ 0x08); //Bitwize XOR flips bit 4
      writeOneMask(oscSelect, 0x08, oscStatus);
      printStar(8,2, (oscStatus & 0x08));
    }
    break;
  case 4: //Ovf
    if (eventType == ENCODER){
      menuDraw(9,2);
    }
    else if (eventType == ENTER){
      oscStatus = (oscStatus ^ 0x10); //Bitwize XOR flips bit 5
      writeOneMask(oscSelect, 0x10, oscStatus);
      printStar(13,2, (oscStatus & 0x10));
    }
    break;
  case 5: //Bnd
    if (eventType == ENCODER){
      menuDraw(14,2);
    }
    else if (eventType == ENTER){
      oscStatus = (oscStatus ^ 0x20); //Bitwize XOR flips bit 6
      writeOneMask(oscSelect, 0x20, oscStatus);
      printStar(18,2, (oscStatus & 0x20));
    }
    break;
  case 6: //PMd
    if (eventType == ENCODER){
      menuDraw(0,3);
    }
    else if (eventType == ENTER){
    }
    break;
  case 7: //PWM
    if (eventType == ENCODER){
      menuDraw(5,3);
    }
    else if (eventType == ENTER){
    }
    break;
  case 8:
    menuState = 1;
    menuDraw(0,1);
    break;
  }
}// End menuOscWaveform

void waveformSelect(){
  if (eventType == REFRESH){

    lcd.setCursor(0,1);
    lcd.print(":");//test
    menuState = waveform;
    oldWaveform = waveform;
    eventType = NONE;
  }
  if (eventType == EXIT){
    writeOneMask(oscSelect, 0x07, oldWaveform);
    waveform = oldWaveform;
    menuChange(OSCWAVEMENU);
  }
  if (menuState < 0){
    menuState = 7;
  }
  if (menuState > 7){
    menuState = 0;
  }

  //Select Waveform
  if (eventType == ENCODER){
    waveTypePrint(menuState);
    writeOneMask(oscSelect, 0x07, menuState);     
  }
  else if (eventType == ENTER){
    menuChange(OSCWAVEMENU);
  } 
}

void menuEnterSwitch(){
}

void menuExitSwitch(){
}




void savePatch(){
  if (eventType == REFRESH){
    menuCursorSet(0,0);
    menuRef("", "    Save Patch?", "", "");
    eventType = NONE;
  }
  else if (eventType == ENTER){
    patchWrite(patchNumber);
    menuChange(PATCHMENU);
  }
  else if (eventType == EXIT){
    menuChange(PATCHMENU);
  }  
}

void patchNameSet(){
  if (eventType == REFRESH){
    menuCursorSet(1,2);
    menuRef("[Edit Name]", "", "", "");
    lcd.setCursor(0,1);
    lcd.print("[");
    lcd.print(patchName);
    lcd.print("]");
    printCursor (1, 2, 1);
    eventType = NONE;
    editMode = 0;
  }
  else if (eventType == ENCODER){
    if (editMode == 0){
      if (menuState < 1) menuState = 16;
      if (menuState > 16) menuState = 1;
      printCursor(menuState,2,1);
    }
    if (editMode == 1){
      if (encoderDirection == true){ //CW Direction
        menuState -= 1; //keep menuState from changing
        patchName[menuState -1] ++;
        if (patchName[menuState -1] > 126) patchName[menuState -1] = 32;
        lcd.setCursor(menuState,1);
        lcd.print(patchName[menuState -1]);
      }
      else if (encoderDirection == false){ //CCW Direction
        menuState += 1; //keep menuState from changing
        patchName[menuState -1] --;
        if (patchName[menuState -1] < 32) patchName[menuState -1] = 126;
        lcd.setCursor(menuState,1);
        lcd.print(patchName[menuState -1]);
        //lcd.print(menuState);//test
      }
    }
  }
  else if (eventType == ENTER){
    editMode = 1 - editMode; //Flips 1 to 0 and visa-versa
    printCursor(menuState,2,editMode + 1);
  }
  else if (eventType == EXIT){
    menuChange(MAINMENU);
  }  
}//patchNameSet End

// ADSR Menu Function
void adsr(){
  if (eventType == REFRESH){
    menuCursorSet(0,1);
    lcd.clear();
    lcd.print("[Osc " + oscName + " ADSR]");
    lcd.setCursor(0,1);
    lcd.print(F(" Attack =      Lv=  "));
    lcd.setCursor(0,2);
    lcd.print(F(" Decay  =      Lv=  "));
    lcd.setCursor(0,3);
    lcd.print(F(" Release=      Lv=  "));
    adsrSet(0,1,13,false,0);
    adsrSet(14,1,13,true,0);
    adsrSet(0,2,14,false,1);
    adsrSet(14,2,14,true,0);
    adsrSet(0,3,15,false,1);
    adsrSet(14,3,15,true,0);
    menuDraw(0,1);
    eventType = NONE;
    editMode = 0;
  }
  if (eventType == EXIT){
    menuChange(MAINMENU);
  }

  if (eventType == ENTER) editMode = 1 - editMode;
  if (editMode == 0) {
    backState = menuState;
    printCursor(cursorX, cursorY, 3);
  }
  if (editMode == 1) {
    menuState = backState;
    printCursor(cursorX, cursorY, 4);
  }

 if (menuState < 1) menuState = 6;
 if (menuState > 6) menuState = 1;


  switch(menuState){
    
  case 1: 
    if (eventType == ENCODER){
      adsrSet(0,1,13,false,0);
    }
    break;
  case 2: 
    if (eventType == ENCODER){
      adsrSet(14,1,13,true,0);
    }
    else if (eventType == ENTER){    
    }
    break;
  case 3: 
    if (eventType == ENCODER){
      adsrSet(0,2,14,false,1);
    }
    else if (eventType == ENTER){  
    }
    break;
  case 4: 
    if (eventType == ENCODER){
      adsrSet(14,2,14,true,0);
    }
    else if (eventType == ENTER){   
    }
    break;
  case 5: 
    if (eventType == ENCODER){
      adsrSet(0,3,15,false,1);
    }
    else if (eventType == ENTER){    
    }
    break;
  case 6: 
    if (eventType == ENCODER){
      adsrSet(14,3,15,true,0);
    }
    else if (eventType == ENTER){   
    }
    break;
  }
}//adsr End

/* adsrSet - sets adsr registers on soundgin
 parameters - x,y = coordinates of cursor
 param = memory location of sound gin resister
 byteHigh = The soundgin uses the high nibble of the byte for one parameter
 and the low nibble for the other. This arg lets the function know
 which part of the byte to change
 offset = There is a global int array that stores the time values in milliseconds.
 This is only for the menu display to display the times correctly
 */
void adsrSet(byte x, byte y, byte param, boolean byteHigh, byte offset){

  if (eventType == REFRESH){
    if (byteHigh) {
      lcd.setCursor(x + 4,y);
      lcd.print(patch[param + oscOffset] >> 4);
    }
    else {
      lcd.setCursor(x + 9,y);
      lcd.print(pgm_read_word(&adsrVal[(patch[param + oscOffset] & 0x0F) * 2 + offset]));
    }
    return;
  }

  if (editMode == 0) menuDraw(x,y);

  else if (editMode == 1){
    if (encoderDirection == true){
      if (byteHigh) patch[param + oscOffset] += 16;
      else {
        if ((patch[param + oscOffset] & 0x0F) != 0x0F)
          patch[param + oscOffset] += 1;
      }
      writeOneByte(param + oscOffset, patch[param + oscOffset]);
    }
    else if (encoderDirection == false){
      if (byteHigh) patch[param + oscOffset] -= 16;
      else {
        if ((patch[param + oscOffset] & 0x0F) != 0x00)
          patch[param + oscOffset] -= 1;
      }
      writeOneByte(param + oscOffset, patch[param + oscOffset]);
    }


    if (byteHigh) {
      lcd.setCursor(x + 4,y);
      lcd.print("  ");
      lcd.setCursor(x + 4,y);
      lcd.print(patch[param + oscOffset] >> 4);
    }
    else {
      lcd.setCursor(x + 9,y);
      lcd.print("     ");
      lcd.setCursor(x + 9,y);
      lcd.print(pgm_read_word(&adsrVal[(patch[param + oscOffset] & 0x0F) * 2 + offset]));
    }
  }
}
































