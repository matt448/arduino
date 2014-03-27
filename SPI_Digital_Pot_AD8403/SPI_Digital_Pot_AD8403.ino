/*
  Digital Pot Control
  
  This example controls an Analog Devices AD8403 digital potentiometer.
  The AD8403 has 4 potentiometer channels. Each channel's pins are labeled
  A - connect this to voltage
  W - this is the pot's wiper, which changes when you set it
  B - connect this to ground.
 
 The AD8403 is SPI-compatible. To command it you send two bytes. 
 The first byte is the channel number starting at zero: (0 - 3)
 The second byte is the resistance value for the channel: (0 - 255).
 
 The AD8403 also contains shutdown (SHDN) and reset (RS) pins.  The shutdown
 pin disables all four of the potentiometers when taken low and enables all
 pots when high. The values of the pots can be adjusted while shutdown is active.
 The reset pin sets all of the pots back to their center value when taken low.
 Reset may or may not be useful for your particular application. In this circuit
 reset is simply tied to +5v to keep it inactive.
 
 The circuit:
  * All A pins of AD8403 connected to +5V
  * All B pins of AD8403 connected to ground
  * An LED and a 220-ohm resisor in series connected from each W pin to ground
  * RS - to +5v
  * SHDN - to digital pin 7 and a pull down resistor
  * CS - to digital pin 10  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin)
 
 Thanks to Tom Igoe and Heather Dewey-Hagborg for their code which this was built on.
 
*/


// inslude the SPI library:
#include <SPI.h>


// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
const int shutDownPin = 7;

void setup() {
  // set the slaveSelectPin as an output
  pinMode (slaveSelectPin, OUTPUT);
  // set the shutDownPin as an output
  pinMode (shutDownPin, OUTPUT);
  // initialize SPI
  SPI.begin();
  //Shutdown the pots to start with
  digitalWrite(shutDownPin, LOW);
  //Set all pots to zero as a starting point
  for (int channel = 0; channel < 4; channel++) {
    digitalPotWrite(channel, 0);
  }
}

void loop() {
  digitalWrite(shutDownPin, LOW);
  int channel = 0;
  // Loop through the four channels of the digital pot.
  for (int channel = 0; channel < 4; channel++) { 
    // Change the resistance on this channel from min to max.
    // Starting at 50 because the LED doesn't visibly change
    // before that point.
    digitalWrite(shutDownPin, HIGH);
    for (int level = 50; level < 255; level++) {
      digitalPotWrite(channel, level);
      delay(2);
    }
    // wait a bit at the top
    delay(5);
    digitalWrite(shutDownPin, LOW);
    // change the resistance on this channel from max to min:
    digitalWrite(shutDownPin, HIGH);
    for (int level = 0; level < 205; level++) {
      digitalPotWrite(channel, 255 - level);
      delay(2);
    }
   
  }
  digitalWrite(shutDownPin, LOW);

}

void digitalPotWrite(int address, int value) {
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin, LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin, HIGH); 
}
