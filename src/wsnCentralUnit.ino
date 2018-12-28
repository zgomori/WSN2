#include "ESP8266WiFi.h"
#include "WsnSensor.h"


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

Adafruit_BME280     bme(BME_CS); // hardware SPI
RF24                radio(RADIO_CE_PIN, RADIO_CSN_PIN);

WiFiClient client;

WsnReceiverConfig cfg;
SensorDataCollector dataCollector;


void setup(){
	readConfig(cfg);

	dataCollector.setRadioSensor(RadioSensorListener(&radio));
	dataCollector.addSensor(BMESensorAdapter(&bme, 200, 0), 60000);

	WsnTSnodeConfig *c =  &cfg.tsNodeConfigArr[0];
	dataCollector.addSensor(ThingSpeakSensor(&client, cfg.thingSpeakAddress, c->nodeID, c->thingSpeakReadKey, c->thingSpeakChannel, c->fieldMapping), c->readFrequencyMs);
}

void loop(){
	
}

void readConfig(WsnReceiverConfig &_cfg){  
  _cfg.radioNetworkAddress = 0xA0A0A0FFLL;
  _cfg.radioChannel = 101;
  strcpy(_cfg.wifiSsid, "wxIoT");
  strcpy(_cfg.wifiPass,"tXgbYPy6DzYaO-U4");

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
