#ifndef LOG_WRITER_H
#define LOG_WRITER_H

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "stdarg.h"
#include "time.h"


class LogWriter{

	protected:
		Print* _logOutput;

		LogWriter(){};

		virtual void print(const char *format, va_list args);
		virtual void printFormat(const char format, va_list *args);
		virtual void printDateTime();

		virtual void beginLogEntry(){};
		virtual void endLogEntry(){};

	public:
		template <class T> 
		void writeLog(char* levelName, T msg, ...){
			beginLogEntry();
			
			printDateTime();
			_logOutput->print(levelName);
			_logOutput->print(' ');
			va_list args;
			va_start(args, msg);
			print(msg,args);

			_logOutput->print('\n');

			endLogEntry();
		}

};

class DummyLogWriter : public LogWriter{
	public:
		DummyLogWriter(){};
		template <class T> 
		void writeLog(char* levelName, T msg, ...){
		}
};

class SerialLogWriter : public LogWriter{
	public:
		SerialLogWriter(Stream* logOutput);
};	

class UdpLogWriter : public LogWriter{
	protected:
		char* _ipAddress;
		uint _remotePort;

		void beginLogEntry();
		void endLogEntry();	
	public:
		UdpLogWriter(WiFiUDP* logOutput, char* ipAddress, uint remotePort);
};
#endif
