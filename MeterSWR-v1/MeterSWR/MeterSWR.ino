/*
 *
 *
 *    MeterSWR.ino
 *
 *    SWR meter application base.
 *
 *    License: GNU General Public License Version 3.0.
 *    
 *    Copyright (C) 2014-2016 by Matthew K. Roberts, KK5JY. All rights reserved.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see: http://www.gnu.org/licenses/
 *
 *
 */

// ================== START CONFIGURATION SETTINGS ==================

///////////////////////////////////////////////////////////////////////////////
//
//  REQUIRED: I/O pin definitions
//

// blinks once per second to indicate healthy main loop; (13) for built-in LED on UNO
#define HEARTBEAT_LED (13)

// analog input pin for FORWARD power reading
#define FWD_PIN (0)

// analog input pin for REFLECTED power reading
#define REF_PIN (1)


///////////////////////////////////////////////////////////////////////////////
//
//  REQUIRED: numeric limits for SWR hardware sensor
//

// define power limits (W); this is the power reading corresponding
//    to a full-scale A/D reading; to report the power as a percentage
//    of full scale, set this to 100.0.
#define FULL_SCALE_FORWARD (20.0)
#define FULL_SCALE_REFLECTED (20.0)

// MINIMUM A/D for SWR - this is the minimum A/D reading that will produce an
//   SWR != 1.0; this helps to prevent unnecessarily high SWR readings at low
//   power levels where there is insufficient A/D resolution to make accurate
//   SWR calculations.  This value is the RAW value used as a minimum FORWARD
//   power limit.  Below this RAW value, the SWR will be reported as 1.0.
#define MIN_POWER (5)

//
//  Serial Port - assign a specific hardware port to be the interface to
//	  the host; by default, the USB connection on most boards is 'Serial',
//	  which is also the hardware TTL serial port on the UNO board.  Make
//    sure to only select ONE option.
//

///////////////////////////////////////////////////////////////////////////////
//
//  OPTIONAL: select host serial port
//

// default for most boards' USB port
#define HostSerial Serial

// default for Leonardo UART, and second MEGA port
//#define HostSerial Serial1

// default for third MEGA port
//#define HostSerial Serial2

// default for fourth MEGA port
//#define HostSerial Serial3

// the 'baud rate' for the serial port
#define SerialRate (9600)

///////////////////////////////////////////////////////////////////////////////
//
//  OPTIONAL: enable persistence of settings
//
//  If this is set to 'true', the firmware will store the values of 'alpha'
//  into the EEPROM whenever the value is changed by user commands.
//

#define PERSIST_SETTINGS false

///////////////////////////////////////////////////////////////////////////////
//
//  Items below this level are OPTIONAL, and can enable display and lighting
//  features that were shown as examples in the website.  You can use these
//  examples as-is, or you can add your own display options
//

///////////////////////////////////////////////////////////////////////////////
//
//  OPTIONAL: Enable LED bar graph for SWR; LED pin definitions below
//
#define SWR_BAR_GRAPH false

#if SWR_BAR_GRAPH == true
#include "LightBar.h"
//
//  BAR - Configuration for the 'soft' bar graph (if enabled)
//    For each LED in the bar, you will need to specify a
//    threshold and digital output pin number (use A# macros
//    for analog pins), and an inversion flag, which should
//    probably always be false.
//

// specify the total number of bar elements
const int BarLength = 10;

// specify the configuration for the individual elements
const LightBarItem barItems[BarLength] = {
	// GREEN LEDs
	{ 1.01,  2, false },
	{ 1.25,  4, false },
	{ 1.50,  5, false },
	{ 1.75,  6, false },
	
	// YELLOW LEDs
	{ 2.00,  7, false },
	{ 2.33,  8, false },
	{ 2.66,  9, false },
	
	// RED LEDs
	{ 3.0,  10, false },
	{ 4.0,  11, false },
	{ 5.0,  12, false },
};

///////////////////////////////////////////////////////////////////////////////
//
//  OPTIONAL: Enable PWM follower, to control LED bar brightness using a
//            hardware potentiometer
//
#define SWR_BAR_FOLLOWER false

#if SWR_BAR_FOLLOWER == true
#include "FollowPWM.h"
//
//  PWM Brightness Control - if you have enabled SWR_BAR_FOLLOWER, this
//    will set up a 'PWM follower' that will adjust a PWM output according
//    to the value of an A/D input.  You can connect a potentiometer to an
//    A/D input, and the output PWM value will follow the A/D voltage at
//    runtime.  You can use this on the common side of the LEDs of your
//    bar graph to control their brightness.  CAUTION: Make sure that you
//    don't consume too much current on the PWM pin; if you need more net
//    current than the PWM line can supply, use a buffer transistor.
//
// PWM follower (output pin, input pin)
FollowPWM brightness(3, 5);

#endif // SWR_BAR_FOLLOWER
#endif // SWR_BAR_GRAPH

///////////////////////////////////////////////////////////////////////////////
//
//  OPTIONAL: Enable serial 7-segment display
//
#define SERIAL_SEGMENTED_DISPLAY false

// 7-segment SWR display support
#if SERIAL_SEGMENTED_DISPLAY == true
#include <SoftwareSerial.h>
#include "SegmentSWR.h"
//
//  Serial 7-Segment Display Support - this is an example of a digital
//    display for showing SWR.
//
SegmentSWR segment(7, 8);		// 7-segment object (rx pin, tx pin)
#endif // SERIAL_SEGMENTED_DISPLAY

///////////////////////////////////////////////////////////////////////////////
//
//  OPTIONAL: Enable LCD/OLED character display
//
//  Set this to 'true' to enable 16x2 display; otherwise leave 'false'; if set
//  to 'true', you will need to configure a display object, below.
//
#define ENABLE_DISPLAY false

#if ENABLE_DISPLAY == true
//
//  Configure display
//
//  Add a 'display' object below.  Examples are provided for standard LCD and
//  OLED displays in 16x2 format, using parallel interface.
//
//  You can use types other than 'LiquidCrystal' as long as the display object
//  is called 'display' and implements the same interface as the 'LiquidCrystal'
//  type.
//

//
//  LCD example
//
//  This example is for a 16x2 LCD display connected to D6 through D12, using
//  4-bit data transfer mode.
//
//#include <LiquidCrystal.h>
//LiquidCrystal display(6, 7, 8, 9, 10, 11, 12);

//
//  OLED example
//
//  This example uses the AdaFruit enhanced Adafruit_CharacterOLED type
//  that is optimized for OLEDs. This type works with both AdaFruit and
//  SparkFun displays based on the RS0010 chip.
//
//  See this hookup guide for an example:
//     https://learn.sparkfun.com/tutorials/oled-display-hookup-guide/all
//
// This is the display type used in the web article.
//#include <Adafruit_CharacterOLED.h>
//Adafruit_CharacterOLED display(OLED_V2, 6, 7, 8, 9, 10, 11, 12);

//
//   Configure display size (default is 16x2)
// 
#define DISPLAY_ROWS (2)
#define DISPLAY_COLS (16)

//
//   Backlight - set this to 'true' if your display supports a
//               'setBacklight' option and you want to enable
//               the backlight; otherwise leave this set to 'false'
//
#define DISPLAY_BACKLIGHT false
#endif // ENABLE_DISPLAY

//
// ================== END OF CONFIGURATION SETTINGS ==================
//

// The version string
#define VERSION_DATE "20160920b"
#define VERSION_STRING "MeterSWR 1.0 (beta-" VERSION_DATE ")"

// local headers
#include "Elapsed.h"
#include "Heartbeat.h"
#include "Bounce2.h"
#include "SWR.h"
#include "utils.h"

//
//  Constants and Globals
//

// Maximum Arduino A/D reading; used only to scale the power readings.
const int ARDUINO_AD_MAX = 1023;

// maximum auto-poll interval (msec)
const int MaxAutoPoll = 1000;

// compute scaling constants for power readings
const float FORWARD_SCALE = (float)FULL_SCALE_FORWARD / (float)ARDUINO_AD_MAX;
const float REFLECT_SCALE = (float)FULL_SCALE_REFLECTED / (float)ARDUINO_AD_MAX;

#if SERIAL_SEGMENTED_DISPLAY == true
const uint16_t SegmentUpdateInterval = 500; // msec between segmented display updates
unsigned long lastSegment = 0;	// used to track the update interval
#endif

#if SWR_BAR_FOLLOWER == true
const uint16_t BrightnessUpdateInterval = 500; // set the msec between follower control A/D readings
unsigned long lastBrightness = 0; // used to track the update inteval
#endif

#if ENABLE_DISPLAY == true
const uint16_t DisplayUpdateInterval = 150; // set the msec between follower control A/D readings
unsigned long lastDisplay = 0;
#endif

#if SWR_BAR_GRAPH == true
// the light bar
LightBar Bar(barItems, BarLength);
#endif

// the SWR calculator
SWR swr(FWD_PIN, REF_PIN);

// a string to hold incoming data
String inputString;

// and another for command data
String cmdText = "";

// command parsing
String command = "";
String argument = "";

// whether the string is complete
boolean inputReady;

// maximum string lengths
const int InputLength = 16;
const int CommandLength = 16;
const int BufferLength = 24;

// I/O buffer
char ioBuffer[BufferLength];

#ifdef CMD_LED
// the last RX data from the host
unsigned long lastRxData;
bool lastCmdLED = false;
#endif

// echo USB RX data back to host
bool echo = false;

// currently parsing a command
bool cmd = false;

// autopolling
unsigned autoPoll = 0;
unsigned long lastPoll = 0;
bool autoRaw = false;

// global timer (amoritize clock read to one/loop)
unsigned long now;

#if PERSIST_SETTINGS == true
#include "persist.h"
#endif

//
//  UpdateDisplays() - update user displays, if any.
//
//  This is where you can update any bar graphs, digital displays, etc.
//  to show readings to the user.
//
static void UpdateDisplays() {
	//
	//  NOTE: the 'swr' object will always be updated immediately prior
	//        to calling UpdateDisplays(), so we can use it directly to
	//        get the values we need.
	//
	//  Current SWR is read using swr.Value()
	//  Current Power Forward is read using swr.Forward()
	//  Current Power Reflected is read using swr.Reflected()
	//

	// refresh the clock
	unsigned long now = millis();

	//
	//  EXAMPLE: update bar graph
	//
	#if SWR_BAR_GRAPH == true
	Bar.Update(swr.Value());
	#if SWR_BAR_FOLLOWER == true
	if (Elapsed(now, lastBrightness) >= BrightnessUpdateInterval) {
		brightness.Process();
		lastBrightness = now;
	}
	#endif
	#endif

	//
	//  EXAMPLE: update segmented display
	//
	#if SERIAL_SEGMENTED_DISPLAY == true
	// update 7-segment display
	if (Elapsed(now, lastSegment) >= SegmentUpdateInterval) {
		segment.Update(swr.Value());
		lastSegment = now;
	}
	#endif
	
	#if ENABLE_DISPLAY == true
	// Example: line 1 - display the forward power level
	display.setCursor(0, 0);
	uint8_t count = 0;
	// the label
	display.print(strcpy_P(ioBuffer, PSTR("F: ")));
	count += strlen(ioBuffer);
	// the number
	FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Forward());
	display.print(ioBuffer);
	count += strlen(ioBuffer);
	// spaces
	for (uint8_t i = count; i < (DISPLAY_COLS - 4); ++i) {
		display.print(' ');
	}
	// SWR label
	display.print(strcpy_P(ioBuffer, PSTR("SWR ")));

	// Example: line 2 - display the reflected power and SWR
	display.setCursor(0, 1);
	// the label
	count = 0;
	snprintf(ioBuffer, sizeof(ioBuffer), strcpy_P(ioBuffer, PSTR("R: ")));
	display.print(ioBuffer);
	count += strlen(ioBuffer);
	// the number
	FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Reflected());
	display.print(ioBuffer);
	count += strlen(ioBuffer);
	// spaces and SWR value
	FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Value());
	for (uint8_t i = count; i < (DISPLAY_COLS - strlen(ioBuffer)); ++i) {
		display.print(' ');
	}
	display.print(ioBuffer);
	#endif

	//
	//  OPTIONAL: update other items
	//
	//  If you have added other display hardware, perform updates to
	//  it here.
	//
}


//
//  setup()
//
void setup() {
	// start the serial port
	HostSerial.begin(SerialRate);
	
	// allocate string storage
	inputString.reserve(InputLength);
	cmdText.reserve(CommandLength);

	// prime the command pipeline with a version command
	inputString = strcpy_P(ioBuffer, PSTR("#VERSION;"));
	inputReady = true;

	// the A/D inputs
	pinMode(FWD_PIN, INPUT);
	pinMode(REF_PIN, INPUT);

	// configure the SWR algorithm
	swr.MinPower(MIN_POWER);
	swr.ScaleForward(FORWARD_SCALE);
	swr.ScaleReflected(REFLECT_SCALE);
	
	#if SWR_BAR_GRAPH == true
	// initialize the bar graph
	Bar.Initialize();
	#endif

	#if SERIAL_SEGMENTED_DISPLAY == true
	// initialize the 7-segment display
	segment.Initialize();
	#endif

	// initialize the display
	#if ENABLE_DISPLAY == true
	// initialize the display
	display.begin(DISPLAY_COLS, DISPLAY_ROWS);
	delay(1000);
	display.clear();
	display.home();
	display.clear();
	display.home();
	#if DISPLAY_BACKLIGHT == true
	// enable backlight
	display.setBacklight(HIGH);
	#endif
	// print banner including version number
	strcpy_P(ioBuffer, PSTR("SWR Meter"));
	const uint8_t x = (DISPLAY_COLS - strlen(ioBuffer)) / 2;
	display.setCursor(x, 0);
	display.print(ioBuffer);
	display.setCursor(x, 1);
	display.print(strcpy_P(ioBuffer, PSTR(VERSION_DATE)));
	// pause to let user actually *read* the banner
	delay(1500);
	#endif

	// set up the HB LED
	HeartbeatSetup();

	// load EEPROM settings
	#if PERSIST_SETTINGS == true
	load_eeprom();
	#endif
}


//
//  loop()
//
void loop() {
	// service auto-polling
	now = millis();

	// HB update
	HeartbeatLoop();

	// spend all our spare time reading the transducer
	swr.Poll();

	// auto-polling
	if (autoPoll && (Elapsed(now, lastPoll) >= autoPoll)) {
		lastPoll = now;
		
		if (autoRaw) {
			HostSerial.write(strcpy_P(ioBuffer, PSTR("#RAW=")));
			snprintf(ioBuffer, sizeof(ioBuffer), "%d", swr.ForwardRaw());
			HostSerial.write(ioBuffer);
			HostSerial.write(",");
			snprintf(ioBuffer, sizeof(ioBuffer), "%d", swr.ReflectedRaw());
			HostSerial.write(ioBuffer);
			HostSerial.write(";");
			if (echo) {
				HostSerial.write("\r\n");
			}
		} else {
			HostSerial.write(strcpy_P(ioBuffer, PSTR("#SWR=")));
			FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Value());
			HostSerial.write(ioBuffer);
			HostSerial.write(';');
			if (echo) {
				HostSerial.write(strcpy_P(ioBuffer, PSTR("\r\n")));
			}
		}
	}
	
	#if ENABLE_DISPLAY == true
	// update user displays, bar graphs, etc.
	if (Elapsed(now, lastDisplay) > DisplayUpdateInterval) {
		UpdateDisplays();
		lastDisplay = now;
	}
	#endif // ENABLE_DISPLAY == true
	
	// if user input ready
	if (inputReady) {
		// for each char received from the USB...
		for (int i = 0; i != inputString.length(); ++i) {
			char ch = inputString.charAt(i);
			
			// if we are in command mode...
			if (cmd) {
				if (ch == '#') {
					// if a '#' sent mid-command, start over
					cmdText = "";
				} else if (ch == ';') {
					// terminate and process the command string
					cmd = false;
					bool ok = false;
					if (echo) HostSerial.print(';');

					// parse the command and argument
					SplitCommand(cmdText, command, argument);
					if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("SWR")))) {
						//
						//  SWR: read A/D and compute SWR
						//
						
						// add the SWR value to the response
						cmdText += "=";
						FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Value());
						cmdText += ioBuffer;
						ok = true;
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("RAW")))) {
						//
						//  RAW: read A/D and output raw values
						//						
						cmdText += "=";
						snprintf(ioBuffer, sizeof(ioBuffer), "%d", swr.ForwardRaw());
						cmdText += ioBuffer;
						cmdText += ",";
						snprintf(ioBuffer, sizeof(ioBuffer), "%d", swr.ReflectedRaw());
						cmdText += ioBuffer;
						ok = true;
					} else if (command.equalsIgnoreCase("PWR")) {
						//
						//  POWER: read A/D and output scaled values
						//						
						cmdText += "=";
						FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Forward());
						cmdText += ioBuffer;
						cmdText += ",";
						FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Reflected());
						cmdText += ioBuffer;
						ok = true;
					} else if (command.equalsIgnoreCase("ALL")) {
						//
						//  ALL: read A/D and output scaled values + SWR
						//						
						cmdText += "=";
						FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Value());
						cmdText += ioBuffer;
						cmdText += ",";
						FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Forward());
						cmdText += ioBuffer;
						cmdText += ",";
						FormatFloat(ioBuffer, sizeof(ioBuffer), swr.Reflected());
						cmdText += ioBuffer;
						ok = true;
					} else if (command.equalsIgnoreCase("ECHO")) {
						//
						//  QUERY/SET ECHO
						//
						if (argument.length() == 0) {
							ok = true;
							cmdText += "=";
							cmdText += echo ? "1" : "0";
						} else {
							int newEcho = argument.toInt();
							echo = newEcho ? true : false;
							ok = true;
						}
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("AUTO")))) {
						//
						//  QUERY/SET AUTO-POLL
						//
						if (argument.length() == 0) {
							ok = true;
							cmdText += "=";
							cmdText += autoPoll;
						} else {
							int newAuto = argument.toInt();
							if (newAuto >= 0 && newAuto < MaxAutoPoll) {
								autoPoll = newAuto;
								ok = true;
							}
						}
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("AUTORAW")))) {
						//
						//  QUERY/SET AUTO-POLL RAW MODE
						//
						if (argument.length() == 0) {
							ok = true;
							cmdText += "=";
							cmdText += autoRaw ? "1" : "0";
						} else {
							int newAuto = argument.toInt();
							autoRaw = newAuto ? true : false;
							ok = true;
						}
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("VERSION")))) {
						//
						//  QUERY VERSION NUMBER
						//
						ok = true;
						cmdText += "=";
						cmdText += VERSION_STRING;
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("ALPHAFWD")))) {
						//
						//  QUERY/SET ALPHA - FORWARD
						//
						if (argument.length() == 0) {
							ok = true;
							cmdText += "=";
							cmdText += swr.AlphaForward();
						} else {
							// read the value (float)
							for (uint8_t i = 0; i != argument.length(); ++i) {
								ioBuffer[i] = argument.charAt(i);
							}
							ioBuffer[argument.length()] = 0;
							float alpha = atof(ioBuffer);
							// set the value
							swr.AlphaForward(alpha);
							// read-back to see what actually got set
							cmdText = command + "=" + swr.AlphaForward();
							ok = true;
#if PERSIST_SETTINGS == true
							save_eeprom();
#endif
						}
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("ALPHAREF")))) {
						//
						//  QUERY/SET ALPHA - REFLECTED
						//
						if (argument.length() == 0) {
							ok = true;
							cmdText += "=";
							cmdText += swr.AlphaReflected();
						} else {
							// read the value (float)
							for (uint8_t i = 0; i != argument.length(); ++i) {
								ioBuffer[i] = argument.charAt(i);
							}
							ioBuffer[argument.length()] = 0;
							float alpha = atof(ioBuffer);
							// set the value
							swr.AlphaReflected(alpha);
							// read-back to see what actually got set
							cmdText = command + "=" + swr.AlphaReflected();
							ok = true;
#if PERSIST_SETTINGS == true
							save_eeprom();
#endif
						}
#if PERSIST_SETTINGS == true
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("EREAD")))) {
						//
						//  READ: get startup settings from EEPROM
						//
						load_eeprom();
						ok = true;
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("EWRITE")))) {
						//
						//  WRITE: commit settings to EEPROM
						//
						save_eeprom();
						ok = true;
					} else if (command.equalsIgnoreCase(strcpy_P(ioBuffer, PSTR("ECLEAR")))) {
						//
						//  FACTORY: reset EEPROM data
						//
						clear_eeprom();
						ok = true;
#endif
					}

					//
					//  TODO: process other command contents
					//

					// respond with #OK or #ERR
					if (echo)
						HostSerial.print(strcpy_P(ioBuffer, PSTR("\r\n")));
					HostSerial.print(ok ? strcpy_P(ioBuffer, PSTR("#OK:")) : strcpy_P(ioBuffer, PSTR("#ERR:")));
					HostSerial.print(cmdText);
					HostSerial.print(';');
					if (echo)
						HostSerial.print(strcpy_P(ioBuffer, PSTR("\r\n")));
					cmdText = "";
				} else {
					if (cmdText.length() < CommandLength) {
						cmdText += ch;
					}
					if (echo) {
						HostSerial.print(ch);
					}
				}
				continue;
			}
			
			// start command mode??
			if ((!cmd) && (ch == '#')) {
				cmd = true;
				if (echo) HostSerial.print('#');
				continue;
			}
		}
		
		// clear the string:
		inputString = "";
		inputReady = false;
	}

	// service the serial port
	while (HostSerial.available()) {
		// get a new byte
		char inChar = (char)HostSerial.read();

		// add it to the inputString
		if (inputString.length() < InputLength)
			inputString += inChar;
		inputReady = true;
	}
}

// EOF
