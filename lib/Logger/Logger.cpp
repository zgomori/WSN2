/*
Based on  https://github.com/thijse/Arduino-Log
*/
#include "Logger.h"

const char* LOG_LEVEL_NAMES[] = {"FATAL", "ERROR", "WARN ", "INFO ", "DEBUG"};

Logger::Logger(){
	this->level = -1;
	this->logWriter = new DummyLogWriter();
}

void Logger::init(int level, Stream* logOutput){
    this->level     = level;
	 this->logWriter = new SerialLogWriter(logOutput);
}
 
void Logger::init(int level, WiFiUDP* logOutput, char* ipAddress, uint remotePort){
    this->level     = level;
	 this->logWriter = new UdpLogWriter(logOutput, ipAddress, remotePort);
}

void Logger::setLevel(int level){
	this->level = level;
}

int Logger::getLevel(){
	return this->level;
}