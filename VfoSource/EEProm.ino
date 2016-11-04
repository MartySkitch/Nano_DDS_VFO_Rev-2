
/*****
  This method is used to read a record from EEPROM. Each record is 4 bytes (sizeof(unsigned long)) and
  is used to calculate where to read from EEPROM.

  Argument list:
    int record                the record to be read. While tuning, it is record 0

  Return value:
    unsigned long            the value of the record,

  CAUTION:  Record 0 is the current frequency while tuning, etc. Record 1 is the number of stored
            frequencies the user has set. Therefore, the stored frequencies list starts with record 23.
*****/
unsigned long readEEPROMRecord(int record)
{
  int offset;
  union {
    byte array[4];
    unsigned long val;
  } myUnion;

  offset = record * sizeof(unsigned long);

  myUnion.array[0] = EEPROM.read(offset);
  myUnion.array[1] = EEPROM.read(offset + 1);
  myUnion.array[2] = EEPROM.read(offset + 2);
  myUnion.array[3] = EEPROM.read(offset + 3);

  return myUnion.val;
}

/*****
  This method is used to test and perhaps write the latest frequency to EEPROM. This routine is called
  every DELTA_TIME_OFFSET (default = 10 seconds). The method tests to see if the current frequency is the
  same as the last frequency (markFrequency). If they are the same, +/- DELTA_FREQ_OFFSET (drift?), no
  write to EEPROM takes place. If the change was larger than DELTA_FREQ_OFFSET, the new frequency is
  written to EEPROM. This is done because EEPROM can only be written/erased 100K times before it gets
  flaky.

  Argument list:
    unsigned long freq        the current frequency of VFO
    int record                the record to be written. While tuning, it is record 0

  Return value:
    void
*****/
void writeEEPROMRecord(unsigned long freq, int record)
{
  int offset;
  union {
    byte array[4];
    unsigned long val;
  } myUnion;

  if (abs(markFrequency - freq) < DELTA_FREQ_OFFSET) {  // Is the new frequency more or less the same as the one last written?
    return;                                           // the same as the one last written? If so, go home.
  }
  myUnion.val = freq;
  offset = record * sizeof(unsigned long);

  EEPROM.write(offset, myUnion.array[0]);
  EEPROM.write(offset + 1, myUnion.array[1]);
  EEPROM.write(offset + 2, myUnion.array[2]);
  EEPROM.write(offset + 3, myUnion.array[3]);
  markFrequency = freq;                               // Save the value just written
}

void UpdateEEProm() {
  static long eepromStartTime;                 // Set when powered up and while tuning
  static long eepromCurrentTime;               // The current time reading
  static int firstTime = 1;

  if(firstTime){
      eepromStartTime = millis();                                     // Need to keep track of EEPROM update time
      return;
  }

  eepromCurrentTime = millis();
  // Only update EEPROM if necessary...both time and currently stored freq.
  if (eepromCurrentTime - eepromStartTime > DELTA_TIME_OFFSET && markFrequency != currentFrequency) {
    writeEEPROMRecord(currentFrequency, READ_EEPROM_FREQ);                  // Update freq
    writeEEPROMRecord((unsigned long) incrementIndex, READ_EEPROM_INCRE);   // Update increment
    eepromStartTime = millis();
    markFrequency = currentFrequency;                                     // Update EEPROM freq.
  }
}

