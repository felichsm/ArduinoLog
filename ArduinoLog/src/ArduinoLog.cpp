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
#ifdef HAVE_SD
	//Initialize the SD card
	pinMode(AL_CARD_DETECT, INPUT);
	pinMode(AL_CHIP_SELECT, OUTPUT);
	//initializeSD();
#endif
}

ArduinoLog::~ArduinoLog() {
#ifdef HAVE_SD
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



//void ArduinoLog::logSerial(const __FlashStringHelper* format,...){
//	if(!LogSerial) return; //Check if the Serial is available
//	PGM_P p = reinterpret_cast<PGM_P>(format); 	//Cast the Flash Address
//	int len = strlen_P(p);						//check the Length of the String in Flash
//
//	DEBUG_SERIAL.println("Length to Print:");
//	DEBUG_SERIAL.println(len);
//	//Check if the Buffer we need are allocated
//	if(!(this->buff_= (char*)realloc((void*)this->buff,(AL_MAXBUFFER-1>len?AL_MAXBUFFER:len+1)*sizeof(char)))){
//		free(this->buff);
//		this->buff=NULL;
//		LogSerial.println(errorMessage);
//		return;
//	}
//	this->buff = this->buff_;
//	if(this->buff)
//	DEBUG_SERIAL.println("Buffer is allocated");
//	if(!(this->formatbuff_= (char*)realloc((void*)this->formatbuff,(AL_MAXBUFFER-1>len?AL_MAXBUFFER:len+1)*sizeof(char)))){
//		free(this->formatbuff);
//		this->formatbuff=NULL;
//		LogSerial.println(errorMessage);
//		return;
//	}
//	this->formatbuff = this->formatbuff_;
//	if(this->formatbuff)
//	DEBUG_SERIAL.println("Second buffer too");
//	memset(this->buff,0,(AL_MAXBUFFER-1>len?AL_MAXBUFFER:len+1));
//	memset(this->formatbuff,0,(AL_MAXBUFFER-1>len?AL_MAXBUFFER:len+1));
//
//	//Buffer is available so Read the String into the Buffer:
//	for(int i =0; i<len; i++){
//		this->formatbuff[i] = pgm_read_byte_near(p+i);
//	}
//	DEBUG_SERIAL.println(formatbuff);
//	//Now Format it:
//	int done;
//	va_list arg;
//	va_start(arg, format);
//	DEBUG_SERIAL.print("Buffer size is:");
//	DEBUG_SERIAL.println((AL_MAXBUFFER-1>len?AL_MAXBUFFER:len+1)*sizeof(char));
//	done = vsnprintf(this->buff,max(AL_MAXBUFFER,len+1), this->formatbuff, arg);	//HERE is the PROBLEM
//	DEBUG_SERIAL.println(done);
//	DEBUG_SERIAL.println(buff);
//	if(done>=(AL_MAXBUFFER>len+1?AL_MAXBUFFER:len+1)){
//		va_end(arg);
//		done+=1;
//		this->buff_ = (char*) realloc((void*)this->buff, done*sizeof(char));
//		this->buff_=NULL;
//		if(this->buff_){
//			this->buff = this->buff_;
//			va_start(arg, format); 	//TODO might be the solution
//			vsnprintf(this->buff,done, this->formatbuff, arg);
//			LogSerial.print(this->buff);
//			free(this->buff);
//			this->buff=NULL;
//		}
//		else{
//			//Allocating Memory did not work:
//			free(this->buff);
//			this->buff=NULL;
//			LogSerial.println(errorMessage);
//		}
//	}
//	else{
//		LogSerial.print(this->buff);
//	}
//	if(len>AL_MAXBUFFER){
//		if(this->buff){
//			free(this->buff);
//		}
//		if(this->formatbuff){
//			free(this->formatbuff);
//		}
//	}
//	va_end(arg);
//}

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
	this->file = SD.open(this->logFile, FILE_WRITE);
	if(!this->file) return;

	//check if the buffer is available
	if(!this->buff) {
		if(!(this->buff_= (char*)realloc((void*)this->buff,AL_MAXBUFFER*sizeof(char)))){
			free(this->buff);
			this->buff=NULL;
			this->file.println(errorMessage);
			this->file.close();
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
	this->file.close();
}

void ArduinoLog::logSD(const __FlashStringHelper* format,...){
	//Check if the SD card is available:
	if(!initializeSD()) return;

	//open the file on the SD card to write to it:
	this->file = SD.open(this->logFile, FILE_WRITE);
	if(!this->file) return;

	//check if the buffer is available
	if(!this->buff) {
		if(!(this->buff_= (char*)realloc((void*)this->buff,AL_MAXBUFFER*sizeof(char)))){
			free(this->buff);
			this->buff=NULL;
			this->file.println(errorMessage);
			this->file.close();
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
	this->file.close();
}

//
//void ArduinoLog::logSD(const __FlashStringHelper* format,...){
//	//Check if the SD card is available:
//	if(!initializeSD()) return;
//
//	//open the file on the SD card to write to it:
//	this->file = SD.open(this->logFile, FILE_WRITE);
//	if(!this->file) return;
//
//
//	PGM_P p = reinterpret_cast<PGM_P>(format); 	//Cast the Flash Address
//	int len = strlen_P(p);						//check the Length of the String in Flash
//
//	//Check if the Buffer we need are allocated
//	if(!(this->buff_= (char*)realloc((void*)this->buff,(AL_MAXBUFFER>len?AL_MAXBUFFER:len)))){
//		free(this->buff);
//		this->buff=NULL;
//		this->file.println(errorMessage);
//		this->file.close();
//		return;
//	}
//	this->buff = this->buff_;
//
//	if(!(this->formatbuff_= (char*)realloc((void*)this->formatbuff,(AL_MAXBUFFER>len?AL_MAXBUFFER:len)))){
//		free(this->formatbuff);
//		this->formatbuff=NULL;
//		this->file.println(errorMessage);
//		this->file.close();
//		return;
//	}
//	this->formatbuff = this->formatbuff_;
//
//	memset(this->buff,0,(AL_MAXBUFFER>len?AL_MAXBUFFER:len));
//	memset(this->formatbuff,0,(AL_MAXBUFFER>len?AL_MAXBUFFER:len));
//
//	//Buffer is available so Read the String into the Buffer:
//	for(int i =0; i<len; i++){
//		this->formatbuff[i] = pgm_read_byte_near(p+i);
//	}
//
//	//Now Format it:
//	int done;
//	va_list arg;
//	va_start(arg, format);
//	done = vsnprintf(this->buff,(AL_MAXBUFFER>len?AL_MAXBUFFER:len), this->formatbuff, arg);
//
//	if(done>=(AL_MAXBUFFER>len?AL_MAXBUFFER:len)){
//		done+=1;
//		this->buff_ = (char*) realloc((void*)this->buff, done);
//		if(this->buff_){
//			this->buff = this->buff_;
//			vsnprintf(this->buff,done, this->formatbuff, arg);
//			this->file.print(this->buff);
//			free(this->buff);
//			this->buff=NULL;
//		}
//		else{
//			//Allocating Memory did not work:
//			free(this->buff);
//			this->buff=NULL;
//			this->file.println(errorMessage);
//		}
//	}
//	else{
//		this->file.print(this->buff);
//	}
//	if(len>AL_MAXBUFFER){
//		if(this->buff){
//			free(this->buff);
//		}
//		if(this->formatbuff){
//			free(this->formatbuff);
//		}
//	}
//	va_end(arg);
//	this->file.close();
//}


bool ArduinoLog::initializeSD(){
	if(!digitalRead(AL_CARD_DETECT)){
		//No Card Detected
		DEBUG_SD("No SD Card detected\n");
		return false;
	}
	else if(!SD.begin(AL_CHIP_SELECT)&& !sdAlreadyInitialized){
		//SD card is not Yet initialized & begin did not work
		DEBUG_SD("Could not Initialize SD card\n");
		return false;
	}
	else{
		//SD card is initialized
		//DEBUG_SD("Initializing SD...");
		if(!sdAlreadyInitialized){
			DEBUG_SD("Opening %s\n", this->logFile);
			file = SD.open(logFile, FILE_WRITE);
			DEBUG_SD("file Opened\n");
//			memset(this->buff,'-',AL_MAXBUFFER);
//			file.write(this->buff, AL_MAXBUFFER);
			for(int i=0; i<AL_MAXBUFFER;i++){
				file.write('-');
			}
			file.println();
			file.close();
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


bool ArduinoLog::setLogFileName(char* name){
	size_t len = strlen(name);
	if(len > AL_MAXFILELENGTH || len < 1) return false;
	strncpy(this->logFile, name, AL_MAXFILELENGTH);
	return true;
}

void ArduinoLog::logError(){
	LogSerial.print("Random");
}





ArduinoLog ALog = ArduinoLog();
