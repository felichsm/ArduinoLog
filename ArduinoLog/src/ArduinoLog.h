/*
 * ArduinoLog.h
 *
 *  Created on: 03.06.2018
 *      Author: felix
 */

#ifndef MPU9250_ARDUINO_LIBRARY_MPU9250_ARDUINO_DRIVER_ARDUINOLOG_H_
#define MPU9250_ARDUINO_LIBRARY_MPU9250_ARDUINO_DRIVER_ARDUINOLOG_H_

#include <WString.h>
#include <SPI.h>
#include <SD.h>

#define AL_MAXBUFFER 64
#define AL_MAXFILELENGTH 12
#define AL_CARD_DETECT 9
#define AL_CHIP_SELECT 10 //TODO:By using PIN 10 (PB2) as Chip Select. The SPI Cannot be used in Slave Mode. Change this for using Slafe mode

#define LOG_FILE_NAME "ALog.txt" //Note, the Maximum filename length that the SD library can handle is 8.3 (eg: demofile.txt)

class ArduinoLog {

	//private Member Variables:
	//As an alternative, the following could also be dynamically allocated using malloc and free
//	char buff[AL_MAXBUFFER];	//Buffer to format the Data
//	char dynbuff[AL_MAXBUFFER];	//Buffer to dynamically load data from FLASH
	char* buff=NULL;	//Buffer to format the Data
	char* buff_=NULL;	//Buffer to format the Data
	char* formatbuff=NULL;	//Buffer to dynamically load data from FLASH
	char* formatbuff_=NULL;	//Buffer to dynamically load data from FLASH
	//char* resizedBuff = NULL;
	bool sdAlreadyInitialized=false;
	char logFile[AL_MAXFILELENGTH];
	File file;


public:
	//Methods:
	ArduinoLog();
	virtual ~ArduinoLog();
	/*
	 * This Method prints Log Messages to the Serial Console
	 * Use this Method as you would use printf()
	 */
	void logSerial(const char* format,...);

	/*
	 * This Method prints Log Messages to the Serial Console
	 * Use this Method as you would use printf()
	 * In difference to logSerial, the Message is sotred in Flash, and not in RAM.
	 * It needs to be called: logSerial(F("Some %d formated %f string: %s"),x,y,z)
	 */
	void logSerial(const __FlashStringHelper* format,...);

	/*
	 * This Method prints Log Messages to the SD card
	 * Use this Method as you would use printf()
	 * It needs to be called: logSD("Some %d formated %f string: %s",x,y,z)
	 */
	void logSD(const char* format,...);
	/*
	 * This Method prints Log Messages to the SD card
	 * Use this Method as you would use printf()
	 * In difference to logSD, the Message is sotred in Flash, and not in RAM.
	 * It needs to be called: logSD(F("Some %d formated %f string: %s"),x,y,z)
	 */
	void logSD(const __FlashStringHelper* format,...);

	/*
	 * Set the name of the File to be logged to
	 * NOTE: Max file name length that is supported by the SD library is:
	 * 8.3 (eg: demofile.txt will work, longfilename.txt wont)
	 * returns true on success
	 */
	bool setLogFileName(char* name);

private:
	bool initializeSD();
};

extern ArduinoLog ALog;

#endif /* MPU9250_ARDUINO_LIBRARY_MPU9250_ARDUINO_DRIVER_ARDUINOLOG_H_ */
