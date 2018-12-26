// If things do not work after changing DAC settings then take few more readings.
// It takes few cycles for changes to take effect.

// By default on powerup this chip is set to make continious conversions on channel 1 at 12bit
// To begin interaction with this ADC it can simply be asked for two bytes (Wire.requestFrom(MCP3422_ADDRESS, 2);)
// if to ask for 3 or more bytes it will display contents of it's 'config' register.

// For 12, 14 and 16 bits a reading is made up of two bytes and it can be stored as an int type variable.
// In each case first byte has 8, 6 or 4 meaningful bits on left side (LSB).
// So bytes on the right side are cleared (or'ed) with a mask.
// For 18 bit reading 3 bytes are returend and they have to be stored as long type variable.
// Of those 3 bits two left bits are fill with meaningful data, but left bit has meaninful data only it's last (right) two bits.

// convenient list of multipliers: https://github.com/uChip/MCP342X/blob/master/MCP342X.cpp

#include  <Wire.h>
#define MCP3422_ADDRESS 0X68



void setup() {
  Wire.begin();  
  Serial.begin(9600); 
  Serial.println("Start");


  float multiplier = 0;
  byte mask = 0;
  int adcConfig = 0;
  uint8_t TakeReading = 0, channel = 0, continious = 1, resolution = 3, gain = 0;
  adcConfig |= TakeReading << 7 | channel << 5 | continious << 4 | resolution << 2 | gain;


  if (resolution == 0) {
    multiplier = 0.001;
    mask = 0b00001111;
  }
  if (resolution == 1) {
    multiplier = 0.00025;
    mask = 0b00111111;
  }
  if (resolution == 2) {
    multiplier = 0.0000625;
    mask = 0b11111111;
  }
  if (resolution == 3) {
    mask = 0b00000011;
    if (gain == 0) multiplier = 0.000015625;
    if (gain == 1) multiplier = 0.0000078125;
    if (gain == 2) multiplier = 0.00000390625;
    if (gain == 3) multiplier = 0.000001953125;
  }


  Serial.println(" ");
  Serial.print("Volts per step: ");
  Serial.println(multiplier, 12);
  Serial.println("adcConfig:");
  Serial.println(adcConfig, BIN);
  Serial.println(" ");

  Wire.beginTransmission(MCP3422_ADDRESS);
  Wire.write(adcConfig);  // sampling rate 128SPS
  Wire.endTransmission();

  Wire.requestFrom(MCP3422_ADDRESS, 6);

  byte DACa = Wire.read() & mask;
  byte DACb = Wire.read();
  byte DACc = Wire.read();

  long DAC_raw = 0b0;

  //if 12, 14 or 16 bit reading join first two bytes
  DAC_raw = DACa;
  DAC_raw = DAC_raw << 8 | DACb;
  //if 18 bit reading add third bit
  if (resolution == 3) {
    DAC_raw = DAC_raw << 8 | DACc;
  }


  Serial.print("ADC reading: 0b");
  Serial.println(DAC_raw, BIN);

  Serial.println("data bits:");
  Serial.println(DACa, BIN);
  Serial.println(DACb, BIN);
  Serial.println(DACc, BIN);
  Serial.println("config bits:");
  Serial.println(Wire.read(), BIN);
  Serial.println(Wire.read(), BIN);
  Serial.println(Wire.read(), BIN);

  Wire.endTransmission();

  double convert = DAC_raw * multiplier;
  Serial.println(convert, DEC);
  Serial.println(" ");

}



void loop() {

}
