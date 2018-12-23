#include "ThingSpeakUtil.h"

ThingSpeakUtil::ThingSpeakUtil(){
}

ThingSpeakUtil::ThingSpeakUtil(WiFiClient *wifiClient, char* thingSpeakAddress){
	client = wifiClient;
	this->thingSpeakAddress = thingSpeakAddress;
}

void ThingSpeakUtil::init(WiFiClient *wifiClient, const char* thingSpeakAddress){
	client = wifiClient;
	this->thingSpeakAddress = thingSpeakAddress;
}

bool ThingSpeakUtil::update(char* apiKey, char* params){
	Serial.println(params); 

	//  Serial.print("Connecting to ");
	Serial.println(apiKey);
	if (!client->connect(thingSpeakAddress, 80)) {
		Serial.println(F("[ERROR] ThingSpeakUtil.update: Connection failed!"));
		return false;
	}	

	Serial.println("Wifi client connected to server.");
	client->print(F("POST /update HTTP/1.1\n"));
	client->print(F("Host: api.thingspeak.com\n"));
	client->print(F("Connection: close\n"));
	client->print(F("X-THINGSPEAKAPIKEY: "));
	client->print(apiKey);
	client->print(F("\n"));
	client->print(F("Content-Type: application/x-www-form-urlencoded\n"));
	client->print(F("Content-Length: "));
	client->print(strlen(params));
	client->print(F("\n\n"));
	client->print(params);

	Serial.println("POST request sent.");

	unsigned long clientTimeout = millis();
	while (client->available() == 0) {
		if (millis() - clientTimeout > 5000) {
			Serial.println(F("[ERROR] ThingSpeakUtil.update: Client Timeout!"));
			client->stop();
			return false;
		}
		delay(10);		
	}
	/*
	Serial.println("===RESPONSE BEGIN===");
	while(client.available()){
		String line = client.readStringUntil('\r');
		Serial.println(line);
	}
	Serial.println("===RESPONSE END===");
	*/
	client->stop();
	return true;
	//Serial.println("===CLIENT STOP===");
}

bool ThingSpeakUtil::get(const char* apiKey, const char* channel, char* json){
	char getParam[80] =  "GET /channels/";
	strcat(getParam, channel);
	strcat(getParam, "/feeds/last.json?api_key=");
	strcat(getParam, apiKey);
	strcat(getParam, " HTTP/1.1\n");

	if (!client->connect(thingSpeakAddress, 80)) {
		Serial.println(F("[ERROR] ThingSpeakUtil.get: Connection failed!"));
		return false;
	}
	Serial.println(getParam);
	client->print(getParam);
	client->print(F("Host: api.thingspeak.com\n"));
	client->print(F("Connection: close\n"));
	client->print(F("\n\n"));
 
	unsigned long clientTimeout = millis();
	while (client->available() == 0) {
		if (millis() - clientTimeout > 5000) {
			Serial.println(F("[ERROR] ThingSpeakUtil.get: Client Timeout!"));
			client->stop();
			return false;
		}
		delay(10);    
	}
	//Serial.println("===RESPONSE BEGIN===");
	char c;
	boolean startJson = false;
	boolean endJson = false;
	uint8_t buffIdx = 0;
	while(client->available()){
		c = client->read();
		if (c == '{'){
			startJson = true;
		} 
		if (startJson){
			json[buffIdx] = c;
			buffIdx++;
		}
		if (c == '}'){
			endJson = true;
			break;
		}
	}
	client->stop();
	
	if(!startJson || !endJson){
		json[0] = 0;
		Serial.println(F("[ERROR] ThingSpeakUtil.get: Invalid json response!"));
		return false;
	} 

	json[buffIdx] = 0;
	Serial.println(json);
	return true;
}