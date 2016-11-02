/*****
 * This method is used to read the value from the R7, R8, C7 voltage divider circuit going to pin 26/A7 of the Arduino, 
 *    process the required calculations, then display the voltage on the LCD.
 * by Hank Ellis K5HDE.
 * V1.0 14 June 2016 Implementation of the basic voltmeter
 * V1.1 23 June 2016 Fixed bug that blanks the voltmeter when the frequency step is changed via the encoder pushbutton
 * V2.0 16 July 2016 Added voltage low and high cautions and warnings
*****/

float GetSupplyVoltage()
{
  float analog_value = 12.023; //value from Arduino pin 26/A7
  float tmp = 0.0; //temporary number used in calculations
  float r7 = 20000.0; //value of R7
  float r8 = 8000.0; //arbitrary value assigned to R8. intent is to provide a 3.5 voltage divider.
  float supplyVoltage = 0.0; 

  analog_value = analogRead(A7); //read the value from the voltage divider on pin A7
  tmp = (analog_value * 5.0) / 1023.0; //convert it to a usable number
  supplyVoltage = tmp / (r8 / (r7 + r8)); //formula of a voltage divider circuit 

  return supplyVoltage;
}
