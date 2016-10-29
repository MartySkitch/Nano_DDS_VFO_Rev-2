
/*****
  This method is used to format a frequency on the lcd display. The currentFrequqncy variable holds the display
  frequency.

  Argument list:
    void

  Return value:
    void
*****/
void DisplayLCDLine(char *message, int row, int col)
{
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
void NewShowFreq(int row, int col) {
  char part[10];
  dtostrf( (float) currentFrequency, 7,0, temp);
  
  strcpy(part, &temp[1]);
  strcpy(temp, "7.");
  strcat(temp, part);
  strcpy(part, &temp[5]);
  temp[5] = '.';
  strcpy(&temp[6], part);
  strcat(temp, " "); 
  strcat(temp, MEGAHERTZ);
  lcd.setCursor(col, row);
  lcd.print(SPACES);
  lcd.setCursor(col + 2, row);
  lcd.print(temp);
}

/*****
  This method is used to display the current license type

  Argument list:
    char *c       // pointer to the type of license as held in bandWarnings[]

  Return value:
    void
*****/
void ShowMarker(char *c)
{
  lcd.setCursor(9, 1);
  lcd.print(c);
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
void ProcessSteps()
{
  DisplayLCDLine(SPACES, 1, 0);                    // This clears the line

  strcpy(temp, incrementStrings[incrementIndex]);

  if (incrementIndex < 4) {
    strcat(temp, HERTZ);
  } else {
    strcat(temp, KILOHERTZ);
  }

  DisplayLCDLine(temp, 1, 0);
  ShowMarker(bandWarnings[whichLicense]);
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

