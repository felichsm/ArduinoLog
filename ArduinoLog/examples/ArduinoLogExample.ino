/*
 * Arduino Log Example
 * by Felix Mitter
 * Use this Library as explained in this sketch
 * It can Log Data in printf() format to the Serial Console
 * Or the data can be logged to a SD card
 * 
 * This Library is ditributed under the 
 */

#include <ArduinoLog.h>

void setup()
{
  Serial.begin(115200);
  while(!Serial){

  }
  Serial.println(F("Serial started..."));

  //Now start Logging:
  //first over Serial
  ALog.logSerial(F("A formated String with a Number: %d\n"), 3);
  ALog.logSerial(F("Such a Long %s string that you can log as if %s printf\n") ,"formatted" ,"you were using");
  ALog.logSerial("Such a Long %s string that it just fits into the Buffer!\n" ,"formatted");

  //Then to SD card
  //The SD card is Connected over SPI
  /*
   * Arduino  <->   SD card
   * Pin 13         SCK
   * Pin 12 (MISO)  MOSI
   * Pin 11 (MOSI)  MISO
   * Pin 10         CS (active low)
   * Pin 9          CD (= chip detect, active high) this is used to detect if a SD card is inserted, The SDcard Adapter needs to support this
   *                In case the SDcard adapter does not support CD, use a pullup resistor on PIN 9 to do the trick 
   */
  ALog.logSD(F("A formated String with a Number: %d\n"), 3);
  ALog.logSD(F("Such a Long %s string that it just %s fit into the Buffer\n") ,"formatted" ,"wont");
  ALog.logSD("Such a Long %s string that it just fits into the Buffer!\n" ,"formatted");

}

void loop() {
  // put your main code here, to run repeatedly:

}
