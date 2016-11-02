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
  if(clearLine) {
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
  part = currentFrequency;
  tmp = "7." + part.substring(1,4) 
        + "." + part.substring(4)
        + " "  MEGAHERTZ;
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

String ShowFreqStepAndLicence()
{
  String tmp;  
  tmp =  ShowFreqStep();
  for(int indx = tmp.length(); indx < 9; indx++) {
      tmp += " ";
  }
  tmp += ShowLicenseType();
  return tmp;
}


String ShowClockDisplay(){
  String tmp;   
  tmp = "This is the time";
  return tmp;
}

/*****
  This method displays the supply voltage

  Argument list:
    void

  Return value:
    void
*****/
String  ShowSupplyVoltage(){
  String part;
  String tmp;   

  part = GetSupplyVoltage();
  tmp =  "Voltage: ";
  tmp += part;
  return tmp;
}


/*****
  This method simply displays a sign-on message

  Argument list:
    void

  Return value:
    void
*****/
void Splash()
{
  lcd.setCursor(0, 0);
  lcd.print("Forty-9er DDS ");
  lcd.setCursor(3, 1);
  lcd.print("NR3Z");
  delay(SPLASHDELAY);
}

