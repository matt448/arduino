/*

  Matthew McMillan
  @matthewmcmillan
  http://matthewcmcmillan.blogspot.com

  Created 12 Mar 2014

  Digital Pot Control (MCP4251)
  
  This example controls a Microchip MCP4251 digital potentiometer.
  The MCP4251 has 2 potentiometer channels. Each channel's pins are labeled:
      A - connect this to voltage
      W - this is the pot's wiper, which changes when you set it
      B - connect this to ground.
  The MCP4251 also has Terminal Control Registers (TCON) which allow you to
  individually connect and disconnect the A,W,B terminals which can be useful
  for reducing power usage or motor controls.
  
  A value of 255 is no resistance
  A value of 128 is approximately 5k ohms
  A value of 0 is 10k ohm resistance
 
  The MCP4251 is SPI-compatible. To command it you send two bytes, 
  one with the memory address and one with the value to set at that address.
  The MCP4251 has few different memory addresses for wipers and tcon (Terminal Control)
     *Wiper 0 write
     *Wiper 0 read
     *Wiper 1 write
     *Wiper 1 read
     *TCON write
     *TCON read
 
 The circuit:
     * All A pins of MCP4251 connected to +5V
     * All B pins of MCP4251 connected to ground
     * An LED and a 220-ohm resistor in series connected from each W pin to ground
     * VSS - to GND
     * VDD - to +5v
     * SHDN - to digital pin 7 and a 4.7k pull down resistor
     * CS - to digital pin 10  (SS pin)
     * SDI - to digital pin 11 (MOSI pin)
     * SDO - to digital pin 12 (MISO pin)
     * CLK - to digital pin 13 (SCK pin)
 
 created 12 Mar 2014 
 
 Thanks to Heather Dewey-Hagborg and Tom Igoe for their original tutorials
 
*/


// inslude the SPI library:
#include <SPI.h>

// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
const int shutdownPin = 7;
const int wiper0writeAddr = B00000000;
const int wiper1writeAddr = B00010000;
const int tconwriteAddr = B01000000;
const int tcon_0off_1on = B11110000;
const int tcon_0on_1off = B00001111;
const int tcon_0off_1off = B00000000;
const int tcon_0on_1on = B11111111;

void setup() {
  Serial.begin(9600);
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  // set the shutdownPin as an output:
  pinMode (shutdownPin, OUTPUT);
  // start with all the pots shutdown
  digitalWrite(shutdownPin,LOW);
  // initialize SPI:
  SPI.begin();
  Serial.println("Setup complete");
}

// This loop adjusts the brightness of two LEDs and
// uses the TCON registers to indvidually disconnect
// the wipers which turns off the LEDs.
void loop() {
  Serial.println("Starting loop");
  digitalWrite(shutdownPin,HIGH); //Turn off shutdown
  
  for(int x=255; x> 0; x--){
    Serial.print("Value sent to pot: ");
    Serial.println(x);
    digitalPotWrite(wiper0writeAddr, x);
    delay(200);
    int wiper0pos = digitalPotRead(B00001100);
    Serial.print("wiper0 position: ");
    Serial.println(wiper0pos);
    wiper0pos = digitalPotRead(B00001100);
    Serial.print("wiper0 position 2nd read: ");
    Serial.println(wiper0pos);
    Serial.println();
    delay(200);
  }
  

  int wiper0pos = digitalPotRead(B00001100);
  Serial.print("wiper0 position: ");
  Serial.println(wiper0pos);
  delay(1000);
  digitalPotWrite(wiper0writeAddr, 150); // Set wiper 0 to 200
  wiper0pos = digitalPotRead(B000011);
  Serial.print("wiper0 position: ");
  Serial.println(wiper0pos);
  digitalPotWrite(wiper1writeAddr, 200); // Set wiper 1 to 200
  delay(1000);
  digitalPotWrite(tconwriteAddr, tcon_0off_1on); // Disconnect wiper 0 with TCON register
  delay(1000);
  digitalPotWrite(tconwriteAddr, tcon_0off_1off); // Disconnect both wipers with TCON register
  digitalPotWrite(wiper0writeAddr, 250); //Set wiper 0 to 255
  delay(1000);
  digitalPotWrite(tconwriteAddr, tcon_0on_1on); // Connect both wipers with TCON register
  Serial.println("Finished loop");
  Serial.println();
}

// This function takes care of sending SPI data to the pot.
void digitalPotWrite(int address, int value) {
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address and value via SPI:
  SPI.transfer(address);
  SPI.transfer(value);
  delay(30);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH); 
}

int digitalPotRead(int address) {
  // take the SS pin low to select the chip:
  digitalWrite(slaveSelectPin,LOW);
  //  send in the address via SPI:
  SPI.transfer(address);
  // send zero to read data from the address
  int data1=SPI.transfer(B00000000);
  delay(30);
  // take the SS pin high to de-select the chip:
  digitalWrite(slaveSelectPin,HIGH);
  Serial.print("Data1 digitalPotRead: ");
  Serial.println(data1, BIN);
  return(data1);
}



