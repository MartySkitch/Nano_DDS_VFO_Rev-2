
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
  if (currentFrequency >= VFO_UPPER_FREQUENCY_LIMIT) {   // Upper band edge?
    currentFrequency = oldFrequency;
  }
  if (currentFrequency <= VFO_LOWER_FREQUENCY_LIMIT) {   // Lower band edge?
    currentFrequency = oldFrequency;
  }
}

/*****
  This method is used to see if the current frequency displayed on line 1 is within a ham band.
  The code does not allow you to use the band edge.

  Argument list:
    void

  Return value:
    int            0 (FREQ_IN_BAND) if within a ham band, 1 (FREQ_OUT_OF_BAND) if not
*****/
int DoRangeCheck()
{

  if (currentFrequency <= VFO_LOWER_FREQUENCY_LIMIT || currentFrequency >= VFO_UPPER_FREQUENCY_LIMIT) {
    return FREQ_OUT_OF_BAND;
  }
  return FREQ_IN_BAND;
}
