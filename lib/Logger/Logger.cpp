/*
Based on  https://github.com/thijse/Arduino-Log
*/
#include "Logger.h"

Logger::Logger(){
	_level = -1;
	_logWriter = new DummyLogWriter();
}

void Logger::init(int level, Stream* logOutput){
    _level     = level;
	 _logWriter = new SerialLogWriter(logOutput);
}
 
void Logger::init(int level, WiFiUDP* logOutput, char* ipAddress, uint remotePort){
    _level     = level;
	 _logWriter = new UdpLogWriter(logOutput, ipAddress, remotePort);
}

void Logger::setLevel(int level){
	this->_level = level;
}

int Logger::getLevel(){
	return _level;
}