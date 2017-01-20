/*****
  This method is used to format a frequency on the lcd display. The currentFrequqncy variable holds the display
  frequency.

  Argument list:
    void

  Return value:
    void
*****/
void DisplayLCDLine(String message, int row, int col, int clearLine)
{
  if (clearLine) {
    lcd.setCursor(0, row);
    lcd.print(SPACES);
  }
  lcd.setCursor(col, row);
  lcd.print(message);
}

/*****
  This method is used to format a frrequency on the lcd display. The currentFrequqncy variable holds the display
  frequency. This is kinda clunky...

  Argument list:
    void

  Return value:
    void
*****/
String ShowFrequency() {
  String part;
  String tmp;
  int flag;

  flag = DoRangeCheck();
  if (flag == FREQ_OUT_OF_BAND) {          // Tell user if out of band; should not happen
    tmp = "* Out of Band *";
  }
  else {
    part = currentFrequency;
    tmp = "7." + part.substring(1, 4)
          + "." + part.substring(4)
          + " "  MEGAHERTZ;
  }
  return tmp;
}

/*****
  This method is used to display the current license type

  Argument list:
    String       // String the type of license as held in bandWarnings[]

  Return value:
    void
*****/
String ShowLicenseType()
{
  return bandWarnings[whichLicense];
}

/*****
  This method is used to change the number of hertz associated with one click of the rotary encoder. Ten step
  levels are provided and increasing or decreasing beyonds the array limits rolls around to the start/end
  of the array.

  Argument list:
    void

  Return value:
    void
*****/
String ShowFreqStep()
{
  String tmp;
  tmp +=  incrementStrings[incrementIndex];

  if (incrementIndex < 4) {
    tmp += HERTZ;
  } else {
    tmp += KILOHERTZ;
  }
  return tmp;
}

/*****
  This method displays the Frequency and Licence

*****/
String ShowFreqStepAndLicence()
{
  String tmp;
  tmp =  ShowFreqStep();
  for (int indx = tmp.length(); indx < 9; indx++) {
    tmp += " ";
  }
  tmp += ShowLicenseType();
  return tmp;
}

/*****
  This method displays the time

*****/
String ShowClockDisplay() {
  String tmp;
  tmp = "This is the time";
  return tmp;
}

/*****
  This method displays the supply voltage
*****/
String  ShowSupplyVoltage() {
  float volt = GetSupplyVoltage();

  String buf = String(volt, 3);
  buf = "Volt: " + buf;

  return buf;
}

/*****
  This method displays the uptime
*****/
//############################################ UPTIME vvvvv  #################################
String ShowUpTime()
{
  long days = 0;
  long hours = 0;
  long mins = 0;
  long secs = 0;
  String temp;

  secs =  millis() / 1000; //convect milliseconds to seconds
  mins = secs / 60; //convert seconds to minutes
  hours = mins / 60; //convert minutes to hours
  days = hours / 24; //convert hours to days
  secs = secs - (mins * 60); //subtract the coverted seconds to minutes in order to display 59 secs max
  mins = mins - (hours * 60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours = hours - (days * 24); //subtract the coverted hours to days in order to display 23 hours max

  String secStr = String(secs, DEC);
  String minStr = String(mins, DEC);
  String hourStr = String(hours, DEC);
  
  //Display results

  temp = "Uptime: " + hourStr + ":" + minStr + ":" + secStr;

  return temp;
}

/*****
  This method simply displays a sign-on message

*****/
void Splash()
{
  lcd.setCursor(2, 0);
  lcd.print("Forty-9er DDS ");
  lcd.setCursor(5, 1);
  lcd.print("NR3Z");
  delay(SPLASH_DELAY);
}

