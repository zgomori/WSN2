#include "ESP8266WiFi.h"
#include <WiFiUdp.h>

#include "WsnSensor.h"
#include "Logger.h"
#include "MainScreen.h"
#include "TouchControl.h"
#include "TimeEventHandler.h"
#include "ScreenManager.h"
#include <TFT_eSPI.h>


/*  tft_eSPI library settings*/
#define TFT_CS   PIN_D8  // Chip select control pin D8
#define TFT_DC   PIN_D4  // Data Command control pin
#define TFT_RST  -1  // Reset pin (could connect to NodeMCU RST, see next line)
#define TOUCH_CS PIN_D2     // Chip select pin (T_CS) of touch screen


#define RADIO_CE_PIN   D3
#define RADIO_CSN_PIN  D0

/*  BME280      NodeMCU
 *  VCC
 *  GND
 *  SCL (CLK)   D5 (GPIO14)
 *  SDA (MOSI)  D7 (GPIO13)
 *  CSB (CS)    D1 
 *  SDO (MISO)  D6 (GPIO12)
 */

#define BME_CS D1

WsnReceiverConfig cfg;

WiFiClient client;
WiFiUDP udp;
Logger Log;

Adafruit_BME280     bme(BME_CS); // hardware SPI
RF24                radio(RADIO_CE_PIN, RADIO_CSN_PIN);


SensorDataCollector dataCollector;

TimeEventHandler timeEventHandler;



TFT_eSPI tft = TFT_eSPI();
MainScreen*  mainScreen = new MainScreen(&tft);

void setup(){
    Serial.begin(115200);
	Serial.println("START");	
	 
	 readConfig(cfg);
	checkWifiConnection(cfg);

//	 Log.init(LOG_LEVEL::DEBUG, &Serial);
	 Log.init(LOG_LEVEL::DEBUG, &udp, "10.100.1.25", 5678);

	
/*
	dataCollector.setRadioSensor(RadioSensorListener(&radio));
	dataCollector.addSensor(BMESensorAdapter(&bme, 200, 0), 60000);

	WsnTSnodeConfig *c =  &cfg.tsNodeConfigArr[0];
	dataCollector.addSensor(ThingSpeakSensor(&client, cfg.thingSpeakAddress, c->nodeID, c->thingSpeakReadKey, c->thingSpeakChannel, c->fieldMapping), c->readFrequencyMs);
*/

//timeEventHandler.process();


TouchObserver* thi;

thi = &(mainScreen->valamiTouchControl);

thi->execute();



Serial.println("START LOG");
delay(500);
/*
int          intValue1  , intValue2;
long         longValue1, longValue2;
bool         boolValue1, boolValue2;
const char * charArray                 = "this is a string";
const char   flashCharArray1[] PROGMEM = "this is a string";
String       stringValue1              = "this is a string";
float        floatValue;
double       doubleValue;


    intValue1  = random(100);
    intValue2  = random(10000);
    longValue1 = random(1000000);
    longValue2 = random(100000000);
    boolValue1 = random(2)==0;
    boolValue2 = random(2)==1;
    floatValue = 12.34;
    doubleValue= 1234.56789;

	uint32_t m = micros();

	     Log.debug   (  "Log as Info with integer values : %d, %d"                   , intValue1,  intValue2);


//    Log.debug   (F("Log as Info with hex values     : %x, %X"                  ), intValue1,  intValue1);
    Log.debug   (  "Log as Info with hex values     : %x, %X"                   , intValue2,  intValue2);
//    Log.debug   (F("Log as Info with binary values  : %b, %B"                  ), intValue1,  intValue1);
    Log.debug   (  "Log as Info with binary values  : %b, %B"                   , intValue2,  intValue2);
//    Log.debug   (F("Log as Info with long values    : %l, %l"                  ), longValue1, longValue2);
    Log.debug   (  "Log as Info with bool values    : %t, %T"                   , boolValue1, boolValue2);
//    Log.debug   (F("Log as Info with string value   : %s"                      ), charArray);
    Log.debug   (  "Log as Info with Flash string value   : %S"                 , flashCharArray1);
    Log.debug   (  "Log as Info with string value   : %s"                       , stringValue1.c_str());
//    Log.debug   (F("Log as Info with float value   : %F"                       ), floatValue);
    Log.debug   (  "Log as Info with float value   : %F"                        , floatValue);
//    Log.debug   (F("Log as Info with double value   : %D"                      ), doubleValue);
    Log.debug   (  "Log as Info with double value   : %D"                       , doubleValue);
//    Log.debug   (F("Log as Debug with mixed values  : %d, %d, %l, %l, %t, %T"  ), intValue1 , intValue2,longValue1, longValue2, boolValue1, boolValue2);
    Log.info    (  "Log as Trace with bool value    : %T"                       , boolValue1);
    Log.warning  (  "Log as Warning with bool value  : %T"                       , boolValue1);
    Log.error    (  "Log as Error with bool value    : %T"                       , boolValue1);
    Log.fatal    (  "Log as Fatal with bool value    : %T"                       , boolValue1);

    Log.error    (  "ERROR    : %x"                       , 1);
    Log.error    (  "ERROR    : %x"                       , 2);
    Log.error    (  "ERROR    : %x"                       , 3);

    
Serial.println(micros()-m);
*/
}

void loop(){
	
}

void readConfig(WsnReceiverConfig &_cfg){  
  _cfg.radioNetworkAddress = 0xA0A0A0FFLL;
  _cfg.radioChannel = 101;
//  strcpy(_cfg.wifiSsid, "wxIoT");
//  strcpy(_cfg.wifiPass,"tXgbYPy6DzYaO-U4");
  strcpy(_cfg.wifiSsid, "wx");
  strcpy(_cfg.wifiPass,"BuMeu8238UPC");

	strcpy(_cfg.ntpServerName, "time.google.com");
	_cfg.timeZone = 1;
	_cfg.localUdpPort = 8760; 

  strcpy(_cfg.thingSpeakAPIKeyArr[0],"JXWWMBZMQZNRMOJK");  
  strcpy(_cfg.thingSpeakAPIKeyArr[1],"5LXV4LVUS2D6OEJA");
  strcpy(_cfg.thingSpeakAPIKeyArr[2],"5LXV4LVUS2D6OEJA");
  _cfg.sensorSet = 23;
  _cfg.sensorReadFrequencyMs = 60000L;
  _cfg.elevation = 126;

  strcpy(_cfg.tsNodeConfigArr[0].name, "Peti");
  strcpy(_cfg.tsNodeConfigArr[0].thingSpeakReadKey, "9ZTPTLMLNFU8VZU3");
  strcpy(_cfg.tsNodeConfigArr[0].thingSpeakChannel, "340091");
  _cfg.tsNodeConfigArr[0].fieldMapping[WSN_TEMPERATURE] = 1;
  _cfg.tsNodeConfigArr[0].fieldMapping[WSN_HUMIDITY] = 2;
  _cfg.tsNodeConfigArr[0].nodeID = 6;
  _cfg.tsNodeConfigArr[0].readFrequencyMs = 61000L;

  strcpy(_cfg.tsNodeConfigArr[1].name, "Central");
  strcpy(_cfg.tsNodeConfigArr[1].thingSpeakReadKey, "JXWWMBZMQZNRMOJK");
  strcpy(_cfg.tsNodeConfigArr[1].thingSpeakChannel, "528401");
  _cfg.tsNodeConfigArr[1].fieldMapping[WSN_TEMPERATURE] = 1;
  _cfg.tsNodeConfigArr[1].fieldMapping[WSN_HUMIDITY] = 2;
  _cfg.tsNodeConfigArr[1].fieldMapping[WSN_PRESSURE] = 3;
  _cfg.tsNodeConfigArr[1].fieldMapping[WSN_MESSAGES] = 4;
  _cfg.tsNodeConfigArr[1].nodeID = 7;
  _cfg.tsNodeConfigArr[1].readFrequencyMs = 60000L;

}


bool checkWifiConnection(WsnReceiverConfig &_cfg){
	if (WiFi.status() == WL_CONNECTED){
		return true;
	}

	WiFi.mode(WIFI_STA); 
	WiFi.begin(_cfg.wifiSsid, _cfg.wifiPass);

	uint32_t startMillis = millis();
	while (millis() - startMillis < 5000){
		Serial.print("Connecting to SSID...");
		Serial.print(_cfg.wifiSsid);
		Serial.print(" Status: ");
		Serial.println(WiFi.status());
		if (WiFi.status() == WL_CONNECTED){
			break;	
		} 
		delay(1000);
	}

	if (WiFi.status() == WL_CONNECTED){
		Serial.println("WiFi Connected to SSID.");
		return true;
	}	
	else{
		Serial.println("WiFi Connection failed.");
		return false;
	}
}
