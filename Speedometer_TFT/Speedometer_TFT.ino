#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include <SD.h>
#include <OneWire.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// These are the pins used for the Mega
// for Due/Uno/Leonardo use the hardware SPI pins (which are different)
#define _sclk 52
#define _miso 50
#define _mosi 51
#define _cs 53
#define _rst 7
#define _dc 6
#define SD_CS 5

// Set pin for OneWire comm
OneWire ds(3);

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
int potPin=0;
int count=0;
int prevCount=1;
int countdigits[] = {0, 0, 0};
int prevdigits[] = {0, 0, 0};
int digitpos[] = {30, 90, 150};
int maxcurrdigits[] = {0, 0, 0};
int maxprevdigits[] = {0, 0, 0};
int maxdigitpos[] = {240, 252, 264};
int maxspeed=0;
int avgspeed=0;
int x=0;

void setup() {
  Serial.begin(9600);
  
  //Init the SD Card
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
  
  //Start the TFT screen and paint it black
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9340_BLACK);
  
  //Draw TRD Logo before turing on backlight
  bmpDraw("trdlogo.bmp", 40, 60);
  
  //fade in back lighting
  for(int b=0; b < 230; b++){
    analogWrite(2, b);
    delay(20);
  }
  
  //Sleep a while to display the logo
  delay(2000);
  tft.fillScreen(ILI9340_BLACK);
  //tft.drawRect(1, 1, 319, 239, ILI9340_RED);
  //             x,  y,  w,  h, color
  //tft.drawRect(150, 70, 50, 70, ILI9340_BLUE);
  //tft.drawRect(90, 70, 50, 70, ILI9340_RED);
  //tft.drawRect(30, 70, 50, 70, ILI9340_GREEN);
  //tft.drawRect(230, 90, 70, 33, ILI9340_RED);
  //tft.fillRect(120, 70, 115, 70, ILI9340_RED);
  
  tft.drawFastHLine(0, 180, 320, ILI9340_RED);

  //Draw static screen elements
  bmpDraw("trdsml24.bmp", 110, 195);
  tft.setTextColor(ILI9340_WHITE);
  tft.setTextSize(4);
  tft.setCursor(230, 90);
  tft.print("mph");
  tft.setTextSize(1);
  tft.setCursor(30, 190);
  tft.print("Temp");

  tft.setCursor(240, 190);
  tft.print("Max Speed");
  tft.setCursor(280, 215);
  tft.print("mph");
  tft.drawRect(240, 210, 11, 15, ILI9340_GREEN);
  tft.drawRect(252, 210, 11, 15, ILI9340_BLUE);
  tft.drawRect(264, 210, 11, 15, ILI9340_YELLOW);
}

void loop(void) {
  if(prevCount > maxspeed){
    tft.setTextSize(2);
    maxcurrdigits[2] = maxspeed % 10;
    if(maxspeed > 99){
      maxcurrdigits[1] = (maxspeed / 10) % 10;
    }else{
      maxcurrdigits[1] = maxspeed / 10;
    }
    maxcurrdigits[0] = maxspeed / 100;

    maxprevdigits[2] = prevCount % 10;
    if(prevCount > 99){
      maxprevdigits[1] = (prevCount / 10) % 10;
    }else{
     maxprevdigits[1] = prevCount / 10;
    }
    maxprevdigits[0] = prevCount / 100;
    
    for(x=0; x < 3; x++){
      if(maxcurrdigits[x] != maxprevdigits[x]){
        //black out old value first
        tft.setCursor(maxdigitpos[x], 210);
        tft.setTextColor(ILI9340_BLACK);
        tft.print(maxprevdigits[x]);
        //print new value in white
        if((x == 0) and (maxspeed > 99) and (maxcurrdigits[x] > 0)){
          tft.setCursor(maxdigitpos[x], 210);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(maxcurrdigits[x]);
        }
        if((x == 1) and (count >= 99)){
          tft.setCursor(maxdigitpos[x], 210);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(maxcurrdigits[x]);
        }else if((x == 1) and (maxspeed < 99) and (maxcurrdigits[x] > 0)){
          tft.setCursor(maxdigitpos[x], 210);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(maxcurrdigits[x]);
        }
        if(x == 2){
          tft.setCursor(maxdigitpos[x], 210);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(maxcurrdigits[x]);
        }
      }
    }
    
    
    //tft.setTextSize(2);
    //tft.setTextColor(ILI9340_BLACK);
    //tft.setCursor(240, 210);
    //tft.print(maxspeed);
    //maxspeed = prevCount;
    //tft.setTextColor(ILI9340_WHITE);
    //tft.setCursor(240, 210);
    //tft.print(maxspeed);
  }
  
  //int newavgspeed = (avgspeed + prevCount)/2;
  //if(avgspeed != newavgspeed){
  //  tft.setTextSize(2);
  //  tft.setTextColor(ILI9340_BLACK);
  //  tft.setCursor(145, 210);
  //  tft.print(avgspeed);
  //  avgspeed = newavgspeed;
  //  tft.setTextColor(ILI9340_WHITE);
  //  tft.setCursor(145, 210);
  //  tft.print(avgspeed);
  //}
  
  //This simulates input of a VSS
  count = analogRead(potPin) / 8;
  
  //Split each digit of the speed into an array
  //This will allow printing of just numbers that have changed.
  countdigits[2] = count % 10;
  if(count > 99){
    countdigits[1] = (count / 10) % 10;
  }else{
    countdigits[1] = count / 10;
  }
  countdigits[0] = count / 100;

  prevdigits[2] = prevCount % 10;
  if(prevCount > 99){
    prevdigits[1] = (prevCount / 10) % 10;
  }else{
   prevdigits[1] = prevCount / 10;
  }
  prevdigits[0] = prevCount / 100;  
  
  
  if(count != prevCount){
    tft.setTextSize(10);
    for(x=0; x < 3; x++){
      if(countdigits[x] != prevdigits[x]){
        //black out old value first
        tft.setCursor(digitpos[x], 70);
        tft.setTextColor(ILI9340_BLACK);
        tft.print(prevdigits[x]);
        //print new value in white
        if((x == 0) and (count > 99) and (countdigits[x] > 0)){
          tft.setCursor(digitpos[x], 70);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(countdigits[x]);
        }
        if((x == 1) and (count >= 99)){
          tft.setCursor(digitpos[x], 70);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(countdigits[x]);
        }else if((x == 1) and (count < 99) and (countdigits[x] > 0)){
          tft.setCursor(digitpos[x], 70);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(countdigits[x]);
        }
        if(x == 2){
          tft.setCursor(digitpos[x], 70);
          tft.setTextColor(ILI9340_WHITE);
          tft.print(countdigits[x]);
        }
      }
    }
  }
  delay(999);

  //count += 1;
  //if(count > 110){
  //  count = 0;
 // }
 prevCount = count;
}





// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 50

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}



#define DS18S20_ID 0x10
 #define DS18B20_ID 0x28
 float temp;
 boolean getTemperature(){
   byte i;
   byte present = 0;
   byte data[12];
   byte addr[8];
   //find a device
   if (!ds.search(addr)) {
     ds.reset_search();
     return false;
   }
   if (OneWire::crc8( addr, 7) != addr[7]) {
     return false;
   }
   if (addr[0] != DS18S20_ID && addr[0] != DS18B20_ID) {
     return false;
   }
   ds.reset();
   ds.select(addr);
   // Start conversion
   ds.write(0x44, 1);
   // Wait some time...
   delay(850);
   present = ds.reset();
   ds.select(addr);
   // Issue Read scratchpad command
   ds.write(0xBE);
   // Receive 9 bytes
   for ( i = 0; i < 9; i++) {
     data[i] = ds.read();
   }
   // Calculate temperature value
   temp = ( (data[1] << 8) + data[0] )*0.0625;
   return true;
 }
