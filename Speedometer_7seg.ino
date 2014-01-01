#include "Adafruit_GFX.h"    // Core graphics library
#include "Adafruit_LEDBackpack.h" // Seven Segment display library
#include <SPI.h>
#include <Wire.h>

void setBrightness(uint8_t b, byte segment_address) {
  if (b > 15) b = 15;
  Wire.beginTransmission(segment_address);
  Wire.write(0xE0 | b);
  Wire.endTransmission();  
}

const int lightPin = 0;
const int hardwareCounterPin = 5;
const int samplePeriod = 1000; //in milliseconds
const float pulsesPerMile = 4000; // this is pulses per mile for Toyota. Other cars are different.
const float convertMph = pulsesPerMile/3600;
unsigned int count;
float mph;
unsigned int imph;
int roundedMph;
Adafruit_7segment matrix = Adafruit_7segment();
byte segment_address = 0x70; //This is hex address of the 7 segment display
boolean drawDots = true;

void setup(void) {
  Serial.begin(9600);
  matrix.begin(segment_address);
  setBrightness(3, segment_address);
  matrix.println(0);
  matrix.writeDisplay();

  TCCR1A=0; //Configure hardware counter 
  TCNT1 = 0;  // Reset hardware counter to zero  
}

void loop() {
  int reading  = analogRead(lightPin);
  int brightness = (reading / 2) / 15;
  if(brightness > 15){
    brightness = 15;
  }
  setBrightness(brightness, segment_address);
  
  bitSet(TCCR1B, CS12); // start counting pulses
  bitSet(TCCR1B, CS11); // Clock on rising edge
  delay(samplePeriod); // Allow pulse counter to collect for samplePeriod
  TCCR1B = 0; // stop counting
  count = TCNT1; // Store the hardware counter in a variable
  TCNT1 = 0;     // Reset hardware counter to zero
  mph = (count/convertMph)*10;

  imph = (unsigned int) mph;

  int x = imph / 10;
  int y = imph % 10;
  
  if(y >= 5){
    roundedMph = x + 1;
  }else{
    roundedMph = x;
  }
    
  matrix.println(roundedMph);
  matrix.writeDisplay();
}

