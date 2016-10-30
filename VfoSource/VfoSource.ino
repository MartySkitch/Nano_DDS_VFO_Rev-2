/*
  The heart of the code that manages the AD9850 was written by
  Richard Visokey AD7C - www.ad7c.com

 Modifications were made by Jack Purdum and Dennis Kidder:
   Rev 4.00:  Feb.  2, 2015
   Rev 5.00:  July  8, 2015, Jack Purdum
   Rev.6.00:  Aug.  1, 2015, Jack Purdum
	 Rev.6.10:  Feb. 27, 2016, Jack Purdum, decreased the number of elements in the increment array
   Rev.7.00:  Oct. 30, 2016, Marty Squicciarini modified code to allow second line to be changed on the fly
*/

#include <Rotary.h>   // From Brian Low: https://github.com/brianlow/Rotary
#include <EEPROM.h>   // Shipped with IDE
#include <Wire.h>     //         "



// Get the LCD I2C Library here:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move all *_I2C files into a new folder named LiquidCrystal_I2C
// in the Arduino library folder
#include <LiquidCrystal_I2C.h>

#define MYTUNINGCONSTANT     34.35910479585483    // Replace with your calculated TUNING CONSTANT. See article

#define SPACES      "                "
#define HERTZ       "Hz"
#define KILOHERTZ   "kHz"
#define MEGAHERTZ   "MHz"

#define GENERAL     2
#define TECH        1
#define EXTRA       0

#define ELEMENTCOUNT(x) (sizeof(x) / sizeof(x[0]))  // A macro that calculates the number
                                                    // of elements in an array.
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); } // Write macro

//Setup some VFO band edges
#define VFOUPPERFREQUENCYLIMIT  7300000L            // Upper band edge
#define VFOLOWERFREQUENCYLIMIT  7000000L            // Lower band edge
#define VFOLOWALLBUTEXTRA       7025000L            // Frequency of Extra licensees only
#define VFOHIGHTECH             7125000L            // Hi Tech cutoff
#define VFOLOWTECH              7100000L            // Low Tech cutoff
#define VFOGENERALLOWGAP        7175000L            // General class edge


#define W_CLK             8               // Pin  8 - connect to AD9850 module word load clock pin (CLK)
#define FQ_UD             9               // Pin  9 - connect to freq update pin (FQ)
#define DAT              10               // Pin 10 - connect to serial data load pin (DAT)
#define RESET            11               // Pin 11 - connect to reset pin (RST) 

#define LCDCOLS          16               // LCD stuff
#define LCDROWS           2
#define SPLASHDELAY    4000               // Hold splash screen for 4 seconds

#define ROTARYSWITCHPIN   7               // Used by switch for rotary encoder
#define RXTXPIN          12               // When HIGH, the xcvr is in TX mode

#define FREQINBAND        0               // Used with range checking
#define FREQOUTOFBAND     1

// ===================================== EEPROM Offsets and data ==============================================
#define READEEPROMFREQ        0       // The EEPROM record address of the last written frequency
#define READEEPROMINCRE       1       // The EEPROM record address of last frequency increment

#define DELTAFREQOFFSET      25       // Must move frequency more than this to call it a frequency change
#define DELTATIMEOFFSET   60000       // If not change in frequency within 1 minute, update the frequency

unsigned long markFrequency;          // The frequency just written to EEPROM
long eepromStartTime;                 // Set when powered up and while tuning
long eepromCurrentTime;               // The current time reading

// ============================ ISR variables: ======================================
volatile int_fast32_t currentFrequency;     // Starting frequency of VFO
volatile long currentFrequencyIncrement;
volatile long ritOffset;

// ============================ General Global Variables ============================
char temp[17];         
int incrementIndex = 0;           // variable to index into increment arrays (see below)
int_fast32_t oldFrequency = 1;    // variable to hold the updated frequency

static char const *bandWarnings[]     = {"  Extra", "   Tech", "General"};
static int whichLicense;
static char const *incrementStrings[] = {"1", "10", "20", "100", "1", "5", "10",   "100"};     // These two allign
static  long incrementTable[]   =       { 1,   10,   20,   100, 1000, 5000, 10000, 100000};
static  long memory[]           = {VFOLOWERFREQUENCYLIMIT, VFOUPPERFREQUENCYLIMIT};

// ----- Second Line Display  -----
int displayMode = 0;

// -- FUNCTION PROTOTYPES for second line of display--
const char * ShowFreqStepAndLicence();
const char * ShowClockDisplay();
const char * func3();
const char * func4();
const char * func5();
// -- ENDS --
// notice the prototype
const char * (*ptr[5])();   

Rotary r = Rotary(2, 3);       // Create encoder object and set the pins the rotary encoder uses.  Must be interrupt pins.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Create LCD object and set the LCD I2C address


void setup() {
   // ===================== arrays are made to point ======================================
   // at the respective functions
   ptr[0]=ShowFreqStepAndLicence;
   ptr[1]=ShowClockDisplay;
  
  // ===================== Set up from EEPROM memory ======================================
  currentFrequency = readEEPROMRecord(READEEPROMFREQ);            // Last frequency read while tuning
  if (currentFrequency < 7000000L || currentFrequency > 7300000L) // New EEPROM usually initialized with 0xFF
    currentFrequency = 7030000L;                                  // Default QRP freq if no EEPROM recorded yet
  markFrequency = currentFrequency;                               // Save EEPROM freq.


  incrementIndex = (int) readEEPROMRecord(READEEPROMINCRE);       // Saved increment as written to EEPROM
  if (incrementIndex < 0 || incrementIndex > 9)                   // If none stored in EEPROM yet...
    incrementIndex = 0;                                           // ...set to 10Hz
  currentFrequencyIncrement = incrementTable[incrementIndex];     // Store working freq variables
  markFrequency = currentFrequency;
  eepromStartTime = millis();                                     // Need to keep track of EEPROM update time

  pinMode(ROTARYSWITCHPIN, INPUT);
  pinMode(RXTXPIN, LOW);              // Start in RX mode

  lcd.begin(LCDCOLS, LCDROWS);        // Display LCD
  Splash();                           // Tell 'em were here...

  PCICR |= (1 << PCIE2);              // Interrupt service code
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
  pinMode(FQ_UD, OUTPUT);             // Tied to AD9850 board
  pinMode(W_CLK, OUTPUT);
  pinMode(DAT, OUTPUT);
  pinMode(RESET, OUTPUT);

  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);  // this pulse enables serial mode on the AD9850 - Datasheet page 12.`

  DisplayLCDLine(ShowFreqStepAndLicence(), 1, 0, 1);  
  DoRangeCheck();
  sendFrequency(currentFrequency);
}

void loop() {
  static int state = 1;      // 1 because of pull-ups on encoder switch
  static int oldState = 1;

  int flag;

  state = digitalRead(ROTARYSWITCHPIN);    // See if they pressed encoder switch
  
  if (state != oldState) {                 // Only if it's been changed...
    if (state == 1) {                      // Only adjust increment when HIGH
      if (incrementIndex < ELEMENTCOUNT(incrementTable) - 1) {    // Adjust the increment size
        incrementIndex++;
      } else {
        incrementIndex = 0;                // Wrap around to zero
      }
      currentFrequencyIncrement = incrementTable[incrementIndex];
        
      DisplayLCDLine( (*ptr[displayMode])(), 1, 0, 1);  // This is for the second line
    }
    oldState = state;
  }

  if (currentFrequency != oldFrequency) { // Are we still looking at the same frequency?
    flag = DoRangeCheck();
    if (flag == FREQOUTOFBAND) {          // Tell user if out of band; should not happen
      lcd.setCursor(0, 0);
      lcd.print("* Out of Band *");
    }
    sendFrequency(currentFrequency);      // Send frequency to chip
    oldFrequency = currentFrequency;
    
    DisplayLCDLine(ShowFrequency(), 0, 2, 1);                    // Nope, so update display.
    DisplayLCDLine( (*ptr[displayMode])(), 1, 0, 1);  // This is for the second line
  }
    
  eepromCurrentTime = millis();
  // Only update EEPROM if necessary...both time and currently stored freq.
  if (eepromCurrentTime - eepromStartTime > DELTATIMEOFFSET && markFrequency != currentFrequency) {
    writeEEPROMRecord(currentFrequency, READEEPROMFREQ);                  // Update freq
    writeEEPROMRecord((unsigned long) incrementIndex, READEEPROMINCRE);   // Update increment
    eepromStartTime = millis();
    markFrequency = currentFrequency;                                     // Update EEPROM freq.
  }

  if (eepromCurrentTime - eepromStartTime > 10000)
    displayMode = 1;
   else
    displayMode =0;  
}






