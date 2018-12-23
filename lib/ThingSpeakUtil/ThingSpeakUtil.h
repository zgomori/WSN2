#ifndef THING_SPEAK_UTIL_H
	#define  THING_SPEAK_UTIL_H

	#include "ESP8266WiFi.h"  

	class ThingSpeakUtil{
		private:
			WiFiClient *client; 
			const char* thingSpeakAddress;
		public:
			ThingSpeakUtil();
			ThingSpeakUtil(WiFiClient *wifiClient, char* thingSpeakAddress);
			void init(WiFiClient *wifiClient, const char* thingSpeakAddress);
			bool update(char* apiKey, char* params);
			bool get(const char* apiKey, const char* channel, char* json);
	};

#endif // WS_DISPLAY_H