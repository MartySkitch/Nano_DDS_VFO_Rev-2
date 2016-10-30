
// Original interrupt service routine, as modified by Jack
ISR(PCINT2_vect) {
  unsigned char result = r.process();

  switch (result) {
    case 0:                                          // Nothing done...
      return;

    case DIR_CW:                                     // Turning Clockwise, going to higher frequencies
        currentFrequency += currentFrequencyIncrement;
      break;

    case DIR_CCW:                                    // Turning Counter-Clockwise, going to lower frequencies
        currentFrequency -= currentFrequencyIncrement;
      break;

    default:                                          // Should never be here
      break;
  }
  if (currentFrequency >= VFOUPPERFREQUENCYLIMIT) {   // Upper band edge?
    currentFrequency = oldFrequency;
  }
  if (currentFrequency <= VFOLOWERFREQUENCYLIMIT) {   // Lower band edge?
    currentFrequency = oldFrequency;
  }
}

/*****
  This method is used to see if the current frequency displayed on line 1 is within a ham band.
  The code does not allow you to use the band edge.

  Argument list:
    void

  Return value:
    int            0 (FREQINBAND) if within a ham band, 1 (FREQOUTOFBAND) if not
*****/
int DoRangeCheck()
{

  if (currentFrequency <= VFOLOWERFREQUENCYLIMIT || currentFrequency >= VFOUPPERFREQUENCYLIMIT) {
    return FREQOUTOFBAND;
  }
  //Setup some VFO band edges
  if (currentFrequency <= VFOLOWALLBUTEXTRA) {
    whichLicense = EXTRA;
  }
  if (currentFrequency > VFOLOWTECH && currentFrequency < VFOHIGHTECH) {
    whichLicense = TECH;
  }

  if ((currentFrequency >= VFOLOWALLBUTEXTRA && currentFrequency < VFOHIGHTECH) ||
      (currentFrequency > VFOGENERALLOWGAP && currentFrequency < VFOUPPERFREQUENCYLIMIT) ) {
    whichLicense = GENERAL;
  }
  return FREQINBAND;
}
