#ifndef WSN_SENSOR_H
#define  WSN_SENSOR_H

#include "time.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

enum SensorStatusCode: int8_t{
	NO_DATA_CHANGED = 0,
	NEW_DATA_ARRIVED = 1
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
	int8_t nodeId;
	byte sensorSet;
	float temperature;
	float humidity;
	float airPressure;
	float batteryVoltage;
	uint32_t messageCount;
	time_t lastUpdateTime;	
};	

class Sensor{
	private:
	SensorType sensorType;
	SensorReadStatus lastReadStatus;
	
	public:
	virtual SensorReadStatus read(SensorData &sensorDataOut);
	SensorReadStatus getStatus();
};


class LocalBMESensor : public Sensor{
	private:
	Adafruit_BME280 bme;
	int8_t nodeId;
	uint32_t messageCount;
	byte sensorSet; 

	public:
	LocalBMESensor(SensorType sensorType, Adafruit_BME280 &bmeReference, int8_t nodeId, byte sensorSet);
	SensorReadStatus read(SensorData &sensorDataOut);
};	

#endif // WSN_SENSOR_H



