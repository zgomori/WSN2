#include "LogWriter.h"

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


void LogWriter::print(const char *format, va_list args) {
	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			printFormat(*format, &args);
		} else {
			_logOutput->print(*format);
		}
		yield();
	}
}

void LogWriter::printFormat(const char format, va_list *args) {
	if (format == '\0') return;

	if (format == '%') {
		_logOutput->print(format);
		return;
	}

	if( format == 's' ) {
		register char *s = (char *)va_arg( *args, int );
		_logOutput->print(s);
		return;
	}

	if( format == 'S' ) {
		register __FlashStringHelper *s = (__FlashStringHelper *)va_arg( *args, int );
		_logOutput->print(s);
		return;
	}
  
	if( format == 'd' || format == 'i') {
		_logOutput->print(va_arg( *args, int ),DEC);
		return;
	}

	if( format == 'D' || format == 'F') {
		_logOutput->print(va_arg( *args, double ));
		return;
	}

	if( format == 'x' ) {
		_logOutput->print(va_arg( *args, int ),HEX);
		return;
	}

	if( format == 'X' ) {
		_logOutput->print("0x");
		_logOutput->print(va_arg( *args, int ),HEX);
		return;
	}

	if( format == 'b' ) {
		_logOutput->print(va_arg( *args, int ),BIN);
		return;
	}

	if( format == 'B' ) {
		_logOutput->print("0b");
		_logOutput->print(va_arg( *args, int ),BIN);
		return;
	}

	if( format == 'l' ) {
		_logOutput->print(va_arg( *args, long ),DEC);
		return;
	}

	if( format == 'c' ) {
		_logOutput->print((char) va_arg( *args, int ));
		return;
	}

	if( format == 't' ) {
		if (va_arg( *args, int ) == 1) {
			_logOutput->print("T");
		}
		else {
			_logOutput->print("F");
		}
		return;
	}

	if( format == 'T' ) {
		if (va_arg( *args, int ) == 1) {
		_logOutput->print(F("true"));
	}
	else {
		_logOutput->print(F("false"));
	}
		return;
	}
}

void LogWriter::printDateTime(){
	char timeBuffer[20];
	time_t ct = now();

	sprintf(timeBuffer, "%u.%02u.%02u %02u:%02u:%02u ", year(ct), month(ct), day(ct), hour(ct), minute(ct), second(ct) );

	_logOutput->print(timeBuffer);
}

SerialLogWriter::SerialLogWriter(Stream* logOutput){
	_logOutput = logOutput;
}

UdpLogWriter::UdpLogWriter(WiFiUDP* udp, char* ipAddress, uint remotePort){
	this->_logOutput = udp;
	this->_ipAddress = ipAddress;
	this->_remotePort = remotePort;
}

void UdpLogWriter::beginLogEntry(){
	static_cast<WiFiUDP*>(_logOutput)->beginPacket(_ipAddress, _remotePort);
	yield();
}

void UdpLogWriter::endLogEntry(){
	static_cast<WiFiUDP*>(_logOutput)->endPacket();
	delay(2);
}
