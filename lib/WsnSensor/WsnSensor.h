#ifndef WSN_SENSOR_H
#define  WSN_SENSOR_H

#include "time.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "SPI.h"
#include "nRF24L01.h"
#include "RF24.h"

#include "ThingSpeakUtil.h"
#include "WsnCommon.h"


enum SensorStatusCode: int8_t{
	NO_DATA_CHANGED = 0,
	NEW_DATA_ARRIVED = 1,
	BME_SENSOR_ERROR = -1,
	RADIO_CONFIG_ERROR = -2
	// TODO - error codes
}; 

enum SensorType: uint8_t{
	LOCAL_SENSOR = 1,
	RADIO_SENSOR = 2,
	THING_SPEAK_SENSOR = 3	
};

struct SensorReadStatus{
	int8_t nodeId;
	SensorStatusCode statusCode;
	char statusMessage[50]; 
};

struct SensorData{
	int8_t nodeId = -1;
	byte sensorSet = 0;
	float temperature = 0.0;
	float humidity = 0.0;
	float airPressure = 0.0;
	float batteryVoltage = 0.0;
	uint32_t messageCount = 0;
	time_t lastUpdateTime = 0;	
};	

class Sensor{
	protected:
	SensorType sensorType;
	SensorReadStatus lastReadStatus;
	
	public:
	virtual SensorReadStatus read(SensorData &sensorDataOut);
	SensorReadStatus getStatus();
};


class BMESensorAdapter : public Sensor{
	private:
	Adafruit_BME280 *bme;
	float elevation;
	int8_t nodeId;
	uint32_t messageCount;
	byte sensorSet; 

	public:
	BMESensorAdapter(Adafruit_BME280 *bme, const float elevation, const int8_t nodeId);
	SensorReadStatus read(SensorData &sensorDataOut);
};	

class RadioSensorAdapter : public Sensor{
	private:
	RF24 *radio;

	public:
	RadioSensorAdapter(RF24 *radio);
	SensorReadStatus read(SensorData &sensorDataOut);
};	

class ThingSpeakSensor : public Sensor{
	private:

	WiFiClient *client;
	int8_t nodeId;
	char *readKey;
	char channel[8];
	uint8_t fieldMapping[5];
	byte sensorSet = 0; 
//	ThingSpeakUtil thingSpeakUtil;

	public:
	ThingSpeakSensor(WiFiClient *client, const int8_t nodeId, const char* readKey, const char *channel, const uint8_t *fieldMapping);
	SensorReadStatus read(SensorData &sensorDataOut);
};	

class SensorDataCollector{
	struct ScheduledSensor{
		Sensor sensor;
		uint32_t lastRead;
		uint32_t repeatMs;
		uint32_t nextRead;
	};

	private:
	Sensor radioSensor;
	ScheduledSensor scheduledSensorArr[5];
	SensorData sensorDataArr[10];
	SensorReadStatus lastSensorReadStatus;
	uint8_t nextFreeScheduledSensorArrIdx = 0;
	uint8_t nextReadIdx; 

	public:
	SensorDataCollector();
	void setRadioSensor(RadioSensorAdapter radioSensor);
	void addSensor(Sensor sensor, uint32_t repeatMs);
	void process();
	SensorReadStatus getStatus();
	SensorData getSensorData(int8_t nodeId);
};	


#endif // WSN_SENSOR_H



