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
#include "Logger.h"


enum SensorStatusCode: int8_t{
	NO_DATA_CHANGED = 0,
	NEW_DATA_ARRIVED = 1,
	BME_SENSOR_ERROR = -1,
	RADIO_CONFIG_ERROR = -2,
	THING_SPEAK_READ_ERROR = -3
}; 

enum SensorType: uint8_t{
	UNDEFINED_SENSOR = 0,
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

/****************************************
 * Sensor (Abstract Class)
 ****************************************/
class Sensor{
	protected:
	SensorType sensorType = UNDEFINED_SENSOR;
	SensorReadStatus lastReadStatus;
	
	public:
	Sensor();
	virtual SensorReadStatus read(SensorData &sensorDataOut);
	SensorReadStatus getStatus();
	SensorType getSensorType();
};

/****************************************
 * BMESensorAdapter
 ****************************************/
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

/****************************************
 * RadioSensorListener
 ****************************************/
class RadioSensorListener : public Sensor{
	private:
	RF24 *radio = NULL;
	void sensorMessage2sensorData(WsnSensorNodeMessage &in, SensorData &out);

	public:

	RadioSensorListener(RF24 *radio);
	SensorReadStatus read(SensorData &sensorDataOut);
};	

/****************************************
 * ThingSpeakSensor
 ****************************************/
class ThingSpeakSensor : public Sensor{
	private:

	const char* thingSpeakAddress;
	int8_t nodeId;
	const char *readKey;
	const char *channel;
	const int8_t *fieldMapping;
	byte sensorSet = 0; 
	ThingSpeakUtil thingSpeakUtil;

	void json2SensorData(char* jsonString, SensorData &sensorDataOut);
	void getJsonFieldValue(char* jsonString, int8_t fieldNo, char* dst);

	public:
	ThingSpeakSensor(WiFiClient *client, const char* thingSpeakAddress, const int8_t nodeId, const char* readKey, const char *channel, const int8_t *fieldMapping);
	SensorReadStatus read(SensorData &sensorDataOut);
};	

/****************************************
 * SensorScheduler
 ****************************************/
class SensorScheduler{
	private:
	struct ScheduledSensor{
		Sensor* sensor;
		uint32_t repeatMs;
		uint32_t lastRead;
	};

	static const uint8_t MAX_SCHEDULED_SENSORS = 5; 
	ScheduledSensor taskArr[MAX_SCHEDULED_SENSORS];
	uint8_t taskCnt = 0;
	uint8_t execIdx = 0; 

	void recalcExecIdx();

	public:
	SensorScheduler();
	bool addTask(Sensor* sensor, uint32_t repeatMs);
	SensorReadStatus execute(SensorData &sensorDataOut);
	uint8_t getTaskCnt();
};


/**********************************
 * SensorObserver 
 * ********************************/
class SensorObserver{
  public:
	 SensorObserver();
    virtual void onSensorChange(SensorData* sensorData) = 0;
};


/**********************************
 * SensorEventNotifier
 * ********************************/

class SensorEventNotifier{
	protected:
		static const uint8_t MAX_OBSERVERS = 5;	
		SensorObserver* observerArr[MAX_OBSERVERS];
		int8_t cnt=0;

		void notifyObservers(SensorData* sensorData);
		
	public:
		bool registerObserver(SensorObserver* observer);
		void removeObserver(SensorObserver* observer);
};

/****************************************
 * SensorDataCollector
 ****************************************/
class SensorDataCollector : public SensorEventNotifier{
	struct ScheduledSensor{
		Sensor* sensor;
		uint32_t repeatMs;
		uint32_t lastRead;
	};

	private:
	RadioSensorListener* radioSensor;
	SensorScheduler sensorScheduler;

	SensorData sensorDataArr[10];
	SensorReadStatus lastSensorReadStatus;
	
	public:
	SensorDataCollector();
	void setRadioSensor(RadioSensorListener* radioSensor);
	bool addSensor(Sensor* sensor, uint32_t repeatMs);
	SensorReadStatus process();
	SensorReadStatus getStatus();
	SensorData getSensorData(uint8_t nodeId);
};	

extern time_t now(); 
extern Logger Log;

#endif // WSN_SENSOR_H



