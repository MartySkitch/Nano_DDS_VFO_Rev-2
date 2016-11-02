void sendFrequency(int32_t frequency) {
  /*
  Formula: int32_t adjustedFreq = frequency * 4294967295/125000000;

  Note the 125 MHz clock on 9850.  You can make 'slight' tuning
  variations here by adjusting the clock frequency. The constants
  factor to 34.359
  */
  int32_t freq = (int32_t) (((float) frequency * MYTUNINGCONSTANT));  // Redefine your constant if needed

  for (int b = 0; b < 4; b++, freq >>= 8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}

// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i = 0; i < 8; i++, data >>= 1) {
    digitalWrite(DAT, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

