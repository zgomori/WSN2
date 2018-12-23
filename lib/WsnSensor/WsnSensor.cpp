#include "WsnSensor.h"

/**********************************
 * Sensor
 * ********************************/
Sensor::Sensor(){

}
SensorReadStatus Sensor::getStatus(){
	return this->lastReadStatus;
}

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
			sensorMessage2sensorData(sensorNodeMessage, sensorDataOut);

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

void RadioSensorAdapter::sensorMessage2sensorData(WsnSensorNodeMessage &in, SensorData &out){
	out.nodeId = 		in.nodeID;
	out.sensorSet = 	in.sensorSet;
	out.temperature = in.temperature;
	out.humidity = 	in.humidity;
	out.airPressure = in.pressure;
	out.batteryVoltage = in.batteryVoltage;
	out.messageCount = in.messageCnt;
	out.lastUpdateTime = now();	
}

/**********************************
 * ThingSpeakSensor
 * ********************************/
ThingSpeakSensor::ThingSpeakSensor(WiFiClient *client, const char* thingSpeakAddress, const int8_t nodeId, const char* readKey, const char *channel, const int8_t *fieldMapping){
	this->sensorType = THING_SPEAK_SENSOR;
	this->thingSpeakAddress = thingSpeakAddress;
	this->nodeId = nodeId;
	this->readKey = readKey;
	this->channel = channel;
	this->fieldMapping = fieldMapping;
	this->thingSpeakUtil.init(client, this->thingSpeakAddress);

	for(uint8_t i=0; i < 5; i++){
		if (fieldMapping[i] != -1){
            bitSet(this->sensorSet,i);
		}
	}
}


SensorReadStatus ThingSpeakSensor::read(SensorData &sensorDataOut){
	char jsonResponse[255];
	bool tsGetStatus = thingSpeakUtil.get(readKey, channel, jsonResponse); 
	if(tsGetStatus){
		json2SensorData(jsonResponse, sensorDataOut);
		sensorDataOut.nodeId = this->nodeId;
		sensorDataOut.sensorSet = 	this->sensorSet;
		sensorDataOut.lastUpdateTime = now();

		lastReadStatus.nodeId = this->nodeId;
		lastReadStatus.statusCode = NEW_DATA_ARRIVED;
		strcpy(lastReadStatus.statusMessage, '\0');
	}
	else{
		lastReadStatus.nodeId = this->nodeId;
		lastReadStatus.statusCode = THING_SPEAK_READ_ERROR;
		strcpy(lastReadStatus.statusMessage, "Thing Speak read error");
	}

	return lastReadStatus;
}

void ThingSpeakSensor::json2SensorData(char* jsonString, SensorData &sensorDataOut){
	char jsonValue[10];
	for(uint8_t i=0; i < 5; i++){
		int8_t fieldNo = fieldMapping[i];
		if (fieldNo != -1){
			getJsonFieldValue(jsonString, fieldNo,  jsonValue);

			switch(i) {
   			case WSN_TEMPERATURE :
      			sensorDataOut.temperature = atof(jsonValue);
      			break;
   			case WSN_HUMIDITY :
      			sensorDataOut.humidity = atof(jsonValue);
      			break;
   			case WSN_PRESSURE :
      			sensorDataOut.airPressure = atof(jsonValue);
      			break;
   			case WSN_BATTERY :
      			sensorDataOut.batteryVoltage = atof(jsonValue);
      			break;
   			case WSN_MESSAGES :
      			sensorDataOut.messageCount = atof(jsonValue);
      			break;
			}	 
		}
	}
}  

void ThingSpeakSensor::getJsonFieldValue(char* jsonString, int8_t fieldNo, char* dst){
	char key[]="\"fieldX\"";
	key[6] = '0' + fieldNo;
	char* src = strstr(jsonString+45, key);
	dst[0] = 0;
	if(src){
		//  "fieldX":"
		//            ^ position src + 10
		src = src + 10;
		uint8_t i = 0;
		while( (src[i] != 0) && (src[i] != '\"') && (i < 9)) {
			dst[i] = src[i];
			i++;  
		}
		dst[i] = 0;
	}
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
