#include "WsnSensor.h"


/**********************************
 * BMESensorAdapter
 * ********************************/
BMESensorAdapter::BMESensorAdapter(Adafruit_BME280 *bme, const float elevation, const int8_t nodeId){
	this->sensorType = LOCAL_SENSOR;
	this->bme = bme;
	this->elevation = elevation;
	this->nodeId = nodeId;
	this->sensorSet = 23; // 10111
	this->lastReadStatus = {nodeId, NEW_DATA_ARRIVED, 0};
}

SensorReadStatus BMESensorAdapter::read(SensorData &sensorDataOut){
	delay(1);
	bme->takeForcedMeasurement();
	delay(10);

	float temp = round(bme->readTemperature() * 10.0F) / 10.0F;
	float pressure = round((bme->readPressure() / 100.0F) + (1.2 * (elevation/10.0F)));
	float humidity = round(bme->readHumidity() * 10.0F) / 10.0F;

	sensorDataOut.nodeId = this->nodeId;
	sensorDataOut.sensorSet = this->sensorSet;
	sensorDataOut.lastUpdateTime = now();	

	if ((temp == 0.0) && (pressure = 0.0) && (humidity = 0.0)){
		lastReadStatus.statusCode = BME_SENSOR_ERROR;
		strcpy(lastReadStatus.statusMessage, "BME280 READ ERROR");
	} 
	else{
		sensorDataOut.temperature = temp;
		sensorDataOut.humidity = humidity;
		sensorDataOut.airPressure = pressure;
		sensorDataOut.messageCount = this->messageCount++;

		lastReadStatus.statusCode = NEW_DATA_ARRIVED;
		strcpy(lastReadStatus.statusMessage, '\0');
	}

	return lastReadStatus;
}

/**********************************
 * RadioSensor
 * ********************************/

RadioSensorAdapter::RadioSensorAdapter(RF24 *radio){
	this->sensorType = RADIO_SENSOR;
	this->radio = radio;
}

SensorReadStatus RadioSensorAdapter::read(SensorData &sensorDataOut){
	uint8_t rfPipeNum;
	if (radio->available(&rfPipeNum)) {
		WsnSensorNodeMessage sensorNodeMessage;
	 	radio->read(&sensorNodeMessage, sizeof(WsnSensorNodeMessage));

		if(sensorNodeMessage.nodeID == rfPipeNum){

			sensorDataOut.nodeId = sensorNodeMessage.nodeID;
			sensorDataOut.sensorSet = sensorNodeMessage.sensorSet;
			sensorDataOut.temperature = sensorNodeMessage.temperature;
			sensorDataOut.humidity = sensorNodeMessage.humidity;
			sensorDataOut.airPressure = sensorNodeMessage.pressure;
			sensorDataOut.batteryVoltage = sensorNodeMessage.batteryVoltage;
			sensorDataOut.messageCount = sensorNodeMessage.messageCnt;
			sensorDataOut.lastUpdateTime = now();	

			lastReadStatus.nodeId = sensorNodeMessage.nodeID;
			lastReadStatus.statusCode = NEW_DATA_ARRIVED;
			strcpy(lastReadStatus.statusMessage, '\0');

//			sysStatus.set(sysStatus.RADIO, true);

			Serial.println(F("----------------------------"));
			Serial.print(F("Message received from node "));
			Serial.println(sensorNodeMessage.nodeID);
			Serial.println(F("----------------------------"));
		}
		else{  
//			sysStatus.set(sysStatus.RADIO, false);

			lastReadStatus.nodeId = -1;
			lastReadStatus.statusCode = RADIO_CONFIG_ERROR;
			char errMsg[80];
			sprintf(errMsg, "Radio config error. PipeNumber:%u nodeId:%u");
			strcpy(lastReadStatus.statusMessage, errMsg);

			Serial.print(F("[ERROR] RadioSensorAdapter.read : PipeNumber != nodeID "));
			Serial.print(F("PipeNumber: "));
			Serial.print(rfPipeNum);
			Serial.print(F(" nodeID: "));
			Serial.println(sensorNodeMessage.nodeID);
		}

	}
	else{
			lastReadStatus.nodeId = -1;
			lastReadStatus.statusCode = NO_DATA_CHANGED;
			strcpy(lastReadStatus.statusMessage, '\0');
	}
	
	return lastReadStatus;
}

/**********************************
 * ThingSpeakSensor
 * ********************************/
ThingSpeakSensor::ThingSpeakSensor(WiFiClient *client, const int8_t nodeId, const char* readKey, const char *channel, const uint8_t *fieldMapping){

	this->sensorType = THING_SPEAK_SENSOR;
	this->client = client;
	this->nodeId = nodeId;
	*(this->readKey) = *readKey;
	*(this->channel) = *channel;
	*(this->fieldMapping) = *fieldMapping;

	for(uint8_t i=0; i < 5; i++){
		if (fieldMapping[i] != -1){
            bitSet(this->sensorSet,i);
		}
	}
}


SensorReadStatus ThingSpeakSensor::read(SensorData &sensorDataOut){

}
/**********************************
 * SensorDataCollector
 * ********************************/
SensorDataCollector::SensorDataCollector(){

}

void SensorDataCollector::setRadioSensor(RadioSensorAdapter radioSensor){
	this->radioSensor = radioSensor;
}

void SensorDataCollector::addSensor(Sensor sensor, uint32_t repeatMs){
	if (nextFreeScheduledSensorArrIdx < 4){
		scheduledSensorArr[++nextFreeScheduledSensorArrIdx] = {sensor, 0, repeatMs, 0};
	}
}
