/**********************************************************************
 *                 NaV-1 Arduino Soundgin Synth V1.0                  *
 *               Visit - notesandvolts.com for tutorial               *
 *                                                                    *
 *               Requires Arduino Version 1.0 or Later                *
 **********************************************************************/

#include <SoftwareSerial.h>
#include <MIDI.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#define CTS 2 
#define txPin 3
#define rxPin 4
#define ROTARY_PIN1 5
#define ROTARY_PIN2 7
#define LCD_RS 6
#define LCD_EN 8
#define LCD_DB4 A0
#define LCD_DB5 A1
#define LCD_DB6 A2
#define LCD_DB7 A3
#define LED 13
#define BUTTON_ENTER 9
#define BUTTON_EXIT 10

#define NumNotes 96

#define TIMEOUT 300
#define DIR_CCW 0x10
#define DIR_CW 0x20

#define ENCODER 1
#define ENTER 2
#define EXIT 3
#define REFRESH 4
#define NONE 5

#define PATCHMENU 0
#define MAINMENU 1
#define OSCMENU 2
#define OSCWAVEMENU 3
#define PATCHNAMEMENU 5
#define PATCHSAVE 99
#define RAW 6
#define ADSR 7
#define EEPROM1 0x50 //Address of eeprom chip

#define OSCA1 0x10
#define OSCA2 0x20
#define OSCA3 0x30
#define OSCB1 0x50
#define OSCB2 0x60
#define OSCB3 0x70

MIDI_CREATE_DEFAULT_INSTANCE();

// Create a SoftwareSerial object named "Soundgin"
SoftwareSerial soundgin = SoftwareSerial(rxPin, txPin);

// Initialize the LCD and Assign pins
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);

//TEST ONLY
// variables created by the build process when compiling the sketch
extern int __bss_end;//Memory test
extern void *__brkval;//Memory test
// Global Variables
byte currentNote = 0;
byte newNote = 0;
volatile unsigned char state = 0;
boolean enterPress = false;
boolean exitPress = false;
unsigned long enterTime;
unsigned long exitTime;
int myCursor = 10;

int menuState = 1;
int menuLevelState = 0; //Level of Menu
//boolean menuRefresh = true;
int cursorX = 0;
int cursorY = 0;
byte editMode = 0; //Used by name func to edit letters

int backState = 1;
byte eventType = 0;
boolean encoderDirection; // True for CW. False for CCW
byte oscSelect = 0; // Stores selected Oscillator
String oscName;
byte oscOffset = 0; //Used by RAW EDIT
byte oscStatus = 0; // Stores Osc status byte
byte waveform = 0; // Stores wave type
byte oldWaveform = 0; // old wave for restore

// Array of Patch parameters
byte patchNumber = 0;
byte patch[128] = {//Holds 128 Byte Soundgin Parameters
  7,0,0,0,0,0,0,0,127,0,0,0,0,0,0,0, //Mix A
  146,0,0,0,0,0,0,0,0,0,0,0,0,240,204,9, //Osc A1
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //Osc A2
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //Osc A3
  7,0,0,0,0,0,0,0,127,0,0,0,0,0,0,0, //Mix B
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //Osc B1
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //Osc B2
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //Osc B3
};
char patchName[16] = "Notes and Volts";//Holds patch name

// Lookup Table for Rotary Encoder function
const unsigned char relookup[7][4] PROGMEM = {
  {
    0x0, 0x2, 0x4,  0x0    }
  , 
  {
    0x3, 0x0, 0x1, 0x10    }
  ,
  {
    0x3, 0x2, 0x0,  0x0    }
  , 
  {
    0x3, 0x2, 0x1,  0x0    }
  ,
  {
    0x6, 0x0, 0x4,  0x0    }
  , 
  {
    0x6, 0x5, 0x0, 0x20    }
  ,
  {
    0x6, 0x5, 0x4,  0x0    }
  ,
};

// Lookup Table - Coverts incoming Midi Note Number to Soundgin note number
const byte lookup[NumNotes] PROGMEM = {
  0,1,2,3,4,5,6,7,8,9,10,11,
  16,17,18,19,20,21,22,23,24,25,26,27,
  32,33,34,35,36,37,38,39,40,41,42,43,
  48,49,50,51,52,53,54,55,56,57,58,59,
  64,65,66,67,68,69,70,71,72,73,74,75,
  80,81,82,83,84,85,86,87,88,89,90,91,
  96,97,98,99,100,101,102,103,104,105,106,107,
  112,113,114,115,116,117,118,119,120,121,122,123
};

// Soundgin Initialization Commands
// **27 = Command Charater (Precedes all commands)**
// **27,1,Memory,Param (Stores Parameter in Memory Location)**
const byte sgInit[] PROGMEM = {
  27,6, // (6 = Clear Mixers A & B)
  27,84, 27,85, 27,86, // (84,85,86 = Release Osc A1,A2,A3)
  27,116, 27,117, 27,118,  // (116,117,118 = Release Osc B1,B2,B3)
  27,1,136,127, // (136 = Set Master Volume to 127 - FULL)
  27,1,8,127, // (8 = Set Mixer A Volume to 127 - FULL)
  27,1,72,127, // (72 = Set Mixer B Volume to 127 - FULL)
  27,1,0,1, // Send Osc A1 to Mixer A
  27,1,1,0, // Turn Off PWM for Osc A1
  27,1,16,146, // 16 = Osc A Control = OvF Mode on, Saw Wave set
  27,1,29,240 ,27,1,30,204, 27,1,31,9 };// 29,30,31 = Osc A1 Atack,Decay,Release


//Lookup Table ADSR Values
const unsigned int adsrVal[] PROGMEM = {
  2,6,8,24,16,48,24,72,38,114,56,168,68,204,80,240,100,300,
  250,750,500,1500,800,2400,1000,3000,2800,8400,5600,16800,11200,33600
};

void setup()
{
  //Set Arduino Pins
  pinMode (LED, OUTPUT);
  pinMode (rxPin, INPUT);
  pinMode (txPin, OUTPUT);
  pinMode (CTS, INPUT);

  pinMode(ROTARY_PIN1, INPUT); //Set Rotary Encoder Pin 1 to Input
  digitalWrite(ROTARY_PIN1, HIGH); //Enable on board resistor
  pinMode(ROTARY_PIN2, INPUT);
  digitalWrite(ROTARY_PIN2, HIGH);

  pinMode(BUTTON_ENTER,INPUT);            // default mode is INPUT
  digitalWrite(BUTTON_ENTER, HIGH);     // Turn on the internal pull-up resistor
  pinMode(BUTTON_EXIT,INPUT);            // default mode is INPUT
  digitalWrite(BUTTON_EXIT, HIGH);     // Turn on the internal pull-up resistor

  Wire.begin();
  soundgin.begin(9600); //Set Software Serial Baud Rate to 9600
  //initPatch(); //Run patch function - Initializes and sets up sound on Soundgin Chip
  MIDI.begin(); // Initialize the Midi Library.
  MIDI.setHandleNoteOn(MyHandleNoteOn); //set MyHandleNoteOn as the callback function

    // Print to LCD
  lcd.begin(20, 4);
  delay(500);
  lcd.print(F(" NOTESANDVOLTS.COM "));
  lcd.print(F("         1.0 "));
  eepromInit();
  initPatch(); //Run patch function - Initializes and sets up sound on Soundgin Chip
  menuChange(PATCHMENU);
  eventType = REFRESH;
}

void loop() // Main Loop
{
  MIDI.read(); // Continually check RX for incoming Midi Commands
  menu(); // Check state of Rotary Encoder and Buttons - Update LCD
}

