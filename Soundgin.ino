// Soundgin and MIDI functions

void initPatch() // Patch - Initialize Soundgin & set Osc A1 to square wave
{
  // 0 with 10 x 27 = Soundgin Hardware Reset
  soundgin.write(byte(0)); 
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  soundgin.write(27);
  delay (500); // Wait until soundgin is reset
  patchRead(0);
  burnPatch();
  release();
}

/* MyHandleNoteOn is called when The Midi Library receives
 a Midi note. This function uses a lookup table to convert
 the Midi note number to the correct Soundgin PlayNote Number.
 The currentNote variable holds the first key that is pressed.
 The newNote variable is used if another key is pressed while
 the original key is still being held. That way if the new note is 
 released, the original note is retriggered.
 */
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) { 
  if (velocity == 0) {
    MyHandleNoteOff(channel, pitch, velocity);
  } else if (currentNote == 0) {
    digitalWrite(LED, HIGH);
    currentNote = pitch;
    trigger(pgm_read_byte(&lookup[currentNote]));
  }
  else {
    digitalWrite(LED, HIGH);
    newNote = pitch;
    trigger(pgm_read_byte(&lookup[newNote]));
  }
}

void MyHandleNoteOff(byte channel, byte pitch, byte velocity) { 
  if (pitch == newNote){
    newNote = 0;
    trigger(pgm_read_byte(&lookup[currentNote]));
  }
  else if (pitch == currentNote && newNote != 0){
    currentNote = newNote;
    newNote = 0;
  }
  else if (pitch == currentNote && newNote == 0){
    digitalWrite(LED, LOW);
    release();
    currentNote = 0;
    newNote = 0;
  }
}

void trigger(byte note){

  soundgin.write(27); //Soundgin Command
  soundgin.write(88); //Osc A1
  soundgin.write(note);

  soundgin.write(27); //Soundgin Command
  soundgin.write(89); //Osc A2
  soundgin.write(note);

  soundgin.write(27); //Soundgin Command
  soundgin.write(90); //Osc A3
  soundgin.write(note);

  soundgin.write(27); //Soundgin Command
  soundgin.write(120); //Osc B1
  soundgin.write(note);

  soundgin.write(27); //Soundgin Command
  soundgin.write(121); //Osc B2
  soundgin.write(note);

  soundgin.write(27); //Soundgin Command
  soundgin.write(122); //Osc B3
  soundgin.write(note);

  //soundgin.write(27); //Soundgin Command
  //soundgin.write(83); //Mix A

  //soundgin.write(27); //Soundgin Command
  //soundgin.write(115); //Mix B
}


void release(){
  soundgin.write(27); //Soundgin command
  soundgin.write(84); //Osc A1

  soundgin.write(27); //Soundgin command
  soundgin.write(85); //Osc A2

  soundgin.write(27); //Soundgin command
  soundgin.write(86); //Osc A3

  soundgin.write(27); //Soundgin command
  soundgin.write(116); //Osc B1

  soundgin.write(27); //Soundgin command
  soundgin.write(117); //Osc B2

  soundgin.write(27); //Soundgin command
  soundgin.write(118); //Osc B3

  //soundgin.write(27); //Soundgin Command
  //soundgin.write(87); //Mix A

  //soundgin.write(27); //Soundgin Command
  //soundgin.write(119); //Mix B
}



