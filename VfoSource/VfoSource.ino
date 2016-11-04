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

#define MY_TUNING_CONSTANT     34.35910479585483    // Replace with your calculated TUNING CONSTANT. See article

#define SPACES      "                "
#define HERTZ       "Hz"
#define KILOHERTZ   "kHz"
#define MEGAHERTZ   "MHz"

#define GENERAL     2
#define TECH        1
#define EXTRA       0

#define ELEMENT_COUNT(x) (sizeof(x) / sizeof(x[0]))  // A macro that calculates the number
// of elements in an array.
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); } // Write macro

//Setup some VFO band edges
#define VFO_UPPER_FREQUENCY_LIMIT  7300000L            // Upper band edge
#define VFO_LOWER_FREQUENCY_LIMIT  7000000L            // Lower band edge
#define VFO_LOW_ALL_BUT_EXTRA       7025000L            // Frequency of Extra licensees only
#define VFO_HIGH_TECH             7125000L            // Hi Tech cutoff
#define VFO_LOW_TECH              7100000L            // Low Tech cutoff
#define VFO_GENERAL_LOW_GAP        7175000L            // General class edge


#define W_CLK             8               // Pin  8 - connect to AD9850 module word load clock pin (CLK)
#define FQ_UD             9               // Pin  9 - connect to freq update pin (FQ)
#define DAT              10               // Pin 10 - connect to serial data load pin (DAT)
#define RESET            11               // Pin 11 - connect to reset pin (RST) 

#define LCD_COLS          16               // LCD stuff
#define LCD_ROWS           2
#define SPLASH_DELAY    4000               // Hold splash screen for 4 seconds

#define ROTARY_SWITCH_PIN   7               // Used by switch for rotary encoder
#define RXTX_PIN          12               // When HIGH, the xcvr is in TX mode

#define FREQ_IN_BAND        0               // Used with range checking
#define FREQ_OUT_OF_BAND     1

// ===================================== EEPROM Offsets and data ==============================================
#define READ_EEPROM_FREQ        0       // The EEPROM record address of the last written frequency
#define READ_EEPROM_INCRE       1       // The EEPROM record address of last frequency increment

#define DELTA_FREQ_OFFSET      25       // Must move frequency more than this to call it a frequency change
#define DELTA_TIME_OFFSET   60000       // If not change in frequency within 1 minute, update the frequency

// ==================================== Display ==============================
#define DISPLAY_SWITCH_PIN    5
#define LINE_ONE    0
#define LINE_TWO    1
#define NUMBER_OF_DISPLAYS    3
#define VOLTAGE_DELTA     0.1

// ==================================== EEPROM data ==============================================
unsigned long markFrequency;          // The frequency just written to EEPROM

// ============================ ISR variables: ======================================
volatile int_fast32_t currentFrequency;     // Starting frequency of VFO
volatile long currentFrequencyIncrement;
volatile long ritOffset;

// ============================ General Global Variables ============================
int incrementIndex = 0;           // variable to index into increment arrays (see below)
int_fast32_t oldFrequency = 1;    // variable to hold the updated frequency

static char const *bandWarnings[]     = {"  Extra", "   Tech", "General"};
static int whichLicense;
static char const *incrementStrings[] = {"1", "10", "20", "100", "1", "5", "10",   "100"};     // These two allign
static  long incrementTable[]   =       { 1,   10,   20,   100, 1000, 5000, 10000, 100000};
static  long memory[]           = {VFO_LOWER_FREQUENCY_LIMIT, VFO_UPPER_FREQUENCY_LIMIT};

// ----- Second Line Display  -----
int displayMode = 1;

// -- FUNCTION PROTOTYPES for second line of display--
String ShowFreqStepAndLicence();
String ShowSupplyVoltage();
String ShowClockDisplay();
String func3();
String func4();
String func5();
// -- ENDS --
// notice the prototype
String (*Line2Display[5])();

Rotary r = Rotary(2, 3);       // Create encoder object and set the pins the rotary encoder uses.  Must be interrupt pins.
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Create LCD object and set the LCD I2C address


void setup() {
  // ===================== arrays are made to point ======================================
  // at the respective functions
  Line2Display[0] = ShowFreqStepAndLicence;
  Line2Display[1] = ShowSupplyVoltage;
  Line2Display[2] = ShowClockDisplay;

  // ===================== Set up from EEPROM memory ======================================
  currentFrequency = readEEPROMRecord(READ_EEPROM_FREQ);            // Last frequency read while tuning
  if (currentFrequency < 7000000L || currentFrequency > 7300000L) // New EEPROM usually initialized with 0xFF
    currentFrequency = 7030000L;                                  // Default QRP freq if no EEPROM recorded yet
  markFrequency = currentFrequency;                               // Save EEPROM freq.


  incrementIndex = (int) readEEPROMRecord(READ_EEPROM_INCRE);       // Saved increment as written to EEPROM
  if (incrementIndex < 0 || incrementIndex > 9)                   // If none stored in EEPROM yet...
    incrementIndex = 0;                                           // ...set to 10Hz
  currentFrequencyIncrement = incrementTable[incrementIndex];     // Store working freq variables
  markFrequency = currentFrequency;


  pinMode(ROTARY_SWITCH_PIN, INPUT);
  pinMode(RXTX_PIN, LOW);              // Start in RX mode
  pinMode(DISPLAY_SWITCH_PIN, INPUT_PULLUP);

  lcd.begin(LCD_COLS, LCD_ROWS);        // Display LCD
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

  int updateLineOne = 0;
  int updateLineTwo = 0;

  updateLineOne = CheckDeltaFrequency();
  updateLineTwo = CheckDisplaySwitch() + CheckStepSwitch() + CheckLicence() + CheckVoltage();

  UpdateEEProm();

  if (updateLineOne)
    DisplayLCDLine(ShowFrequency(), LINE_ONE, 2, 1);                    // Nope, so update display.

  if (updateLineTwo)
    DisplayLCDLine( (*Line2Display[displayMode])(), LINE_TWO, 0, 1);  // This is for the second line
}





