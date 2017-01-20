/*****
  This method will check if the Step swithch has been pressed and update the
    incrementTable.

  Return value:
    int         1 indicates that the switch was pressed

*****/

int CheckStepSwitch() {
  static int state = 1;      // 1 because of pull-ups on encoder switch
  static int oldState = 1;
  int ret = 0;

  state = digitalRead(ROTARY_SWITCH_PIN);    // See if they pressed encoder switch

  if (state != oldState) {                 // Only if it's been changed...
    if (state == 1) {                      // Only adjust increment when HIGH
      if (incrementIndex < ELEMENT_COUNT(incrementTable) - 1) {    // Adjust the increment size
        incrementIndex++;
      } else {
        incrementIndex = 0;                // Wrap around to zero
      }
      currentFrequencyIncrement = incrementTable[incrementIndex];
    }
    oldState = state;
    ret = 1;
  }
  return ret;
}

/*****
  This method will check if the Display swithch has been pressed and update the

  Return value:
    int         1 indicates that the switch was pressed

*****/
int CheckDisplaySwitch() {
  static int state = 1;      // 1 because of pull-ups on encoder switch
  static int oldState = 1;
  int ret = 0;

  state = digitalRead(DISPLAY_SWITCH_PIN);    // See if they pressed encoder switch

  if (state != oldState) {                 // Only if it's been changed...
    if (state == 1) {                      // Only adjust increment when HIGH
      if (displayMode < NUMBER_OF_DISPLAYS - 1) {    // Adjust the Display
        displayMode++;
      } else {
        displayMode = 0;                // Wrap around to zero
      }
    }
    oldState = state;
    ret = 1;
  }
  return ret;
}

/*****
  This method will check if the Frequency has been changed.

  Return value:
    int         1 indicates that the switch was pressed

*****/
int CheckDeltaFrequency() {
  int ret = 0;
  if (currentFrequency != oldFrequency) { // Are we still looking at the same frequency?
    sendFrequency(currentFrequency);      // Send frequency to chip
    oldFrequency = currentFrequency;
    ret = 1;
  }
  return ret;
}

/*****
  This method will check if the License needs to be updated
  
  Return value:
    int         1 indicates that the license needs to updated

*****/
int CheckLicence() {
  static int oldLicense;
  int ret = 0;

  //Setup some VFO band edges
  if (currentFrequency <= VFO_LOW_ALL_BUT_EXTRA) {
    whichLicense = EXTRA;
  }
  if (currentFrequency > VFO_LOW_TECH && currentFrequency < VFO_HIGH_TECH) {
    whichLicense = TECH;
  }

  if ((currentFrequency >= VFO_LOW_ALL_BUT_EXTRA && currentFrequency < VFO_HIGH_TECH) ||
      (currentFrequency > VFO_GENERAL_LOW_GAP && currentFrequency < VFO_UPPER_FREQUENCY_LIMIT) ) {
    whichLicense = GENERAL;
  }
  if( whichLicense != oldLicense) {
    oldLicense = whichLicense;
    ret = 1;
  }
  return ret;
}

/*****
  This method will check if he Voltage has changed by more than

  Return value:
    int         1 indicates that the switch was pressed

*****/
int CheckVoltage() {
  int ret = 0;
  static float oldVoltage;
  static float currentVoltage;

  currentVoltage = GetSupplyVoltage();
  if ( abs(currentVoltage - oldVoltage) > VOLTAGE_DELTA) {
    oldVoltage = currentVoltage;
    ret = 1;
  }
  return ret;
}

/*****
  This method will check if the uptime has increased by Set Amount

  Return value:
    int         1 indicates that the uptime has increased 

*****/
int CheckUpTime() {
  long currentmillis;
  static long oldmillis;
  int ret = 0;

  currentmillis = millis();

  if ( (currentmillis - oldmillis) > 1000) {
    oldmillis = millis();
    ret = 1;
  }
  return ret;
}

