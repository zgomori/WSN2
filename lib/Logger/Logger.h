/*
Based on https://github.com/thijse/Arduino-Log
*/

#ifndef LOGGER_H
#define LOGGER_H
#include <stdarg.h>
#include "Arduino.h"
#include "LogWriter.h"


#define LOG_LEVEL_FATAL   0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4


/*!

**** Wildcards

* %s	replace with an string (char*)
* %c	replace with an character
* %d	replace with an integer value
* %l	replace with an long value
* %x	replace and convert integer value into hex
* %X	like %x but combine with 0x123AB
* %b	replace and convert integer value into binary
* %B	like %x but combine with 0b10100011
* %t	replace and convert boolean value into "t" or "f"
* %T	like %t but convert into "true" or "false"
*/

class Logger {
	private:
		int _level;
		LogWriter* _logWriter;
		template <class T, typename... Args> void writeLog(int level, T msg, Args... args){
			if (level <= _level) {
				_logWriter->writeLog(levelNames[level], msg, args...);
				yield();
			}
		}

	public:
		char levelNames[5][6] = {"FATAL", "ERROR", "WARN ", "INFO ", "DEBUG"};

		Logger();
		void init(int level, Stream* logOutput);
		void init(int level, WiFiUDP* logOutput, char* ipAddress, uint remotePort);

		void setLevel(int level);
		int getLevel();

		template <class T, typename... Args> void fatal(T msg, Args... args){
			writeLog(LOG_LEVEL_FATAL, msg, args...);
		}
		template <class T, typename... Args> void error(T msg, Args... args){
			writeLog(LOG_LEVEL_ERROR, msg, args...);
		}
		template <class T, typename... Args> void warning(T msg, Args...args){
			writeLog(LOG_LEVEL_WARNING, msg, args...);
		}
		template <class T, typename... Args> void info(T msg, Args...args){
			writeLog(LOG_LEVEL_INFO, msg, args...);
		}
		template <class T, typename... Args> void debug(T msg, Args... args){
			writeLog(LOG_LEVEL_DEBUG, msg, args...);
		}

};

#endif
