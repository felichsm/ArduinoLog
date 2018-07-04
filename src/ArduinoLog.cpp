/*
 * ArduinoLog.cpp
 *
 *  Created on: 03.06.2018
 *      Author: felix
 */

#include "ArduinoLog.h"
#include <Arduino.h>


#define HAVE_SERIAL
#define HAVE_SD
//#undef HAVE_SD

#ifdef HAVE_SERIAL
#define LogSerial Serial
#endif

#ifdef HAVE_SD
static const char PROGMEM errorMessage[] = "ArduinoLog: Could not Log a Message!!!";
//static const char errorMessage[] = "ArduinoLog: Could not Log a Message!!!";
	//static const char errorMessage[] = {"ArduinoLog: Could not Log a Message!!!"};
#else
	static const char PROGMEM errorMessage[] = {"ArduinoLog: Could not Log a Message!!!"};
#endif

#define DEBUG_SD logSerial
//#define DEBUG_SD

#define DEBUG_SERIAL Serial

ArduinoLog::ArduinoLog() {
	strcpy(logFile,LOG_FILE_NAME);
	sdAlreadyInitialized=false;
}

ArduinoLog::~ArduinoLog() {
#ifdef HAVE_SD
	file.close();
	//Destructors are rather rare called in uController programming so we dont do te following
	// And instead safe some Flash space:)
//	//uninitialize the SD card
//	if(sdAlreadyInitialized){
//		file = SD.open(logFile, FILE_WRITE);
//		memset(this->buff,'-',AL_MAXBUFFER);
//		file.write(this->buff, AL_MAXBUFFER);
//		file.println();
//		file.close();
//	}
#endif
}

#ifdef HAVE_SERIAL
void ArduinoLog::logSerial(const char* format,...){
	if(!LogSerial) return;	//Check if the Serial is available

	//check if the buffer is available
	if(!this->buff) {
		if(!(this->buff_= (char*) realloc((void*)this->buff,AL_MAXBUFFER*sizeof(char)))){
			free(this->buff);
			this->buff=NULL;
			LogSerial.println(errorMessage);
			return;
		}
		this->buff=this->buff_;
	}
	//LogSerial.println("Buffer is available");
	int done;
	va_list arg;
	va_start(arg, format);
	done = vsnprintf(this->buff,AL_MAXBUFFER, format, arg);	//This formats the "format" string and saves it in buff

	//Check if buff was too small:
	if(done>=AL_MAXBUFFER){
		done+=1;
		//va_end(arg);
		//Was to small, realloc and try again
		if((this->buff_ = (char*) realloc((void*)this->buff, done*sizeof(char)))){
			this->buff=this->buff_;
			vsnprintf(this->buff,done, format, arg);
			LogSerial.print(this->buff);
			free(this->buff);
			this->buff=NULL;
		}
		else{
			//could not realloc enough memory
			free(this->buff);
			this->buff=NULL;
			LogSerial.println(errorMessage);
		}
	}
	//Buffer was big enough, log it:
	else{
		LogSerial.print(this->buff);
	}
	va_end(arg);
}


void ArduinoLog::logSerial(const __FlashStringHelper* format,...){
	if(!LogSerial) return;	//Check if the Serial is available

	//check if the buffer is available
	if(!this->buff) {
		if(!(this->buff_= (char*) realloc((void*)this->buff,AL_MAXBUFFER*sizeof(char)))){
			free(this->buff);
			this->buff=NULL;
			LogSerial.println(errorMessage);
			return;
		}
		this->buff=this->buff_;
	}
	//LogSerial.println("Buffer is available");
	int done;
	va_list arg;
	va_start(arg, format);
	done = vsnprintf_P(this->buff,AL_MAXBUFFER, (const char*)format, arg);	//This formats the "format" string and saves it in buff

	//Check if buff was too small:
	if(done>=AL_MAXBUFFER){
		done+=1;
		//Was to small, realloc and try again
		if((this->buff_ = (char*) realloc((void*)this->buff, done*sizeof(char)))){
			this->buff=this->buff_;
			vsnprintf_P(this->buff,done, (const char*)format, arg);
			LogSerial.print(this->buff);
			free(this->buff);
			this->buff=NULL;
		}
		else{
			//could not realloc enough memory
			free(this->buff);
			this->buff=NULL;
			LogSerial.println(errorMessage);
		}
	}
	//Buffer was big enough, log it:
	else{
		LogSerial.print(this->buff);
	}
	va_end(arg);
}



#else
void ArduinoLog::logSerial(const char* format,...){

}
void ArduinoLog::logSerial(const __FlashStringHelper* format,...){

}
#endif

#ifdef HAVE_SD
void ArduinoLog::logSD(const char* format,...){
	//Check if the SD card is available:
	if(!initializeSD()) return;

	//open the file on the SD card to write to it:
	//this->file.open(this->logFile,  O_CREAT | O_WRITE | O_EXCL);
	//if(!this->file) return;

	//check if the buffer is available
	if(!this->buff) {
		if(!(this->buff_= (char*)realloc((void*)this->buff,AL_MAXBUFFER*sizeof(char)))){
			free(this->buff);
			this->buff=NULL;
			this->file.println(errorMessage);
			//this->file.close();
			if (!file.sync() || file.getWriteError()) {
			    DEBUG_SD("write error");
			}
			return;
		}
		this->buff=this->buff_;
	}
	int done;
	va_list arg;
	va_start(arg, format);
	done = vsnprintf(this->buff,AL_MAXBUFFER, format, arg);	//This formats the "format" string and saves it in buff

	//Check if buff was too small:
	if(done>=AL_MAXBUFFER){
		done+=1;
		//Was to small, realloc and try again
		if((this->buff_ = (char*) realloc((void*)this->buff, done*sizeof(char)))){
			this->buff=this->buff_;
			vsnprintf(this->buff,done, format, arg);
			this->file.print(this->buff);
			free(this->buff);
			this->buff=NULL;
		}
		else{
			//could not realloc enough memory
			free(this->buff);
			this->buff=NULL;
			this->file.println(errorMessage);
		}
	}
	//Buffer was big enough, log it:
	else{
		this->file.print(this->buff);
	}
	va_end(arg);
	//this->file.close();
}

void ArduinoLog::logSD(const __FlashStringHelper* format,...){
	//Check if the SD card is available:
	if(!initializeSD()) return;

	//open the file on the SD card to write to it:
	//file.open(this->logFile,  O_CREAT | O_WRITE | O_EXCL);
	//if(!this->file) return;

	//check if the buffer is available
	if(!this->buff) {
		if(!(this->buff_= (char*)realloc((void*)this->buff,AL_MAXBUFFER*sizeof(char)))){
			free(this->buff);
			this->buff=NULL;
			this->file.println(errorMessage);
			//this->file.close();
			if (!file.sync() || file.getWriteError()) {
			    DEBUG_SD("write error");
			}
			return;
		}
		this->buff=this->buff_;
	}
	int done;
	va_list arg;
	va_start(arg, format);
	done = vsnprintf_P(this->buff,AL_MAXBUFFER, (const char*)format, arg);	//This formats the "format" string and saves it in buff

	//Check if buff was too small:
	if(done>=AL_MAXBUFFER){
		done+=1;
		//Was to small, realloc and try again
		if((this->buff_ = (char*) realloc((void*)this->buff, done*sizeof(char)))){
			this->buff=this->buff_;
			vsnprintf_P(this->buff,done, (const char*)format, arg);
			this->file.print(this->buff);
			free(this->buff);
			this->buff=NULL;
		}
		else{
			//could not realloc enough memory
			free(this->buff);
			this->buff=NULL;
			this->file.println(errorMessage);
		}
	}
	//Buffer was big enough, log it:
	else{
		this->file.print(this->buff);
	}
	va_end(arg);
	//this->file.close();
}


bool ArduinoLog::flushSD(){
	if (!file.sync() || file.getWriteError()) {
	    DEBUG_SD("write error");
	    return false;
	}
	return true;
}

bool ArduinoLog::initializeSD(){
	if(!digitalRead(this->cardDetect)){
		//No Card Detected
		DEBUG_SD("No SD Card detected\n");
		return false;
	}
	else if(!SD.begin(this->chipSelect, SD_SCK_MHZ(50))&& !sdAlreadyInitialized){
		//SD card is not Yet initialized & begin did not work
		DEBUG_SD("Could not Initialize SD card\n");
		return false;
	}
	else{
		//SD card is initialized
		//DEBUG_SD("Initializing SD...");
		if(!sdAlreadyInitialized){
			DEBUG_SD("Opening %s\n", this->logFile);
			if(file.open(logFile, sdMode))
			DEBUG_SD("file Opened\n");
			else return false;
//			memset(this->buff,'-',AL_MAXBUFFER);
//			file.write(this->buff, AL_MAXBUFFER);
			for(int i=0; i<AL_MAXBUFFER;i++){
				file.write('-');
			}
			file.println();
			//file.close();
			if (!file.sync() || file.getWriteError()) {
			    DEBUG_SD("write error");
			}
			DEBUG_SD("SD card initialized\n");
		}
		return sdAlreadyInitialized = true;
	}

}


#else
void ArduinoLog::logSD(const char* format,...){
}
void ArduinoLog::logSD(const __FlashStringHelper* format,...){

}
bool ArduinoLog::initializeSD(){
	return false;
}


#endif


bool ArduinoLog::setLogFileName(const char* name){
	//close old file first:
	this->file.close();
	size_t len = strlen(name);
	if(len > AL_MAXFILELENGTH || len < 1) return false;
	strncpy(this->logFile, name, AL_MAXFILELENGTH);
	this->sdAlreadyInitialized=false;
	return true;
}

bool ArduinoLog::beginSD(uint8_t CS, uint8_t CD){
#ifdef HAVE_SD
	//Initialize the SD card
	this->cardDetect = CD;
	this->chipSelect = CS;
	pinMode(this->cardDetect, INPUT);
	pinMode(this->chipSelect, OUTPUT);
	//initializeSD();
#endif
	return true;
}

void ArduinoLog::logError(){
	LogSerial.print("Random");
}


bool ArduinoLog::setSDmode(uint8_t sdMode){
	this->sdMode = sdMode;
	return true;
}


ArduinoLog ALog = ArduinoLog();
