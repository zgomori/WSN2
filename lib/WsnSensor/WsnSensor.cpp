#include "WsnSensor.h"

/**********************************
 * Sensor
 * ********************************/
Sensor::Sensor(){
}

SensorReadStatus Sensor::getStatus(){
	return this->lastReadStatus;
}

SensorType Sensor::getSensorType(){
	return this->sensorType;
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
	Log.debug("BMESensorAdapter.read() - start reading...");
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
		Log.error("BMESensorAdapter.read() - read error");
	} 
	else{
		sensorDataOut.temperature = temp;
		sensorDataOut.humidity = humidity;
		sensorDataOut.airPressure = pressure;
		sensorDataOut.messageCount = this->messageCount++;

		lastReadStatus.statusCode = NEW_DATA_ARRIVED;
		strcpy(lastReadStatus.statusMessage, "");
		Log.debug("BMESensorAdapter.read() - result= t:%F, h:F%, p:%F",temp, pressure, humidity);
	}

	return lastReadStatus;
}

/**********************************
 * RadioSensorAdapter
 * ********************************/
RadioSensorListener::RadioSensorListener(RF24 *radio){
	this->sensorType = RADIO_SENSOR;
	this->radio = radio;
}

SensorReadStatus RadioSensorListener::read(SensorData &sensorDataOut){
	uint8_t rfPipeNum;
	if (radio->available(&rfPipeNum)) {
		Log.debug("RadioSensorListener.read() - start reading.  Pipe number: %d", rfPipeNum);

		WsnSensorNodeMessage sensorNodeMessage;
	 	radio->read(&sensorNodeMessage, sizeof(WsnSensorNodeMessage));

		if(sensorNodeMessage.nodeID == rfPipeNum){
			sensorMessage2sensorData(sensorNodeMessage, sensorDataOut);

			lastReadStatus.nodeId = sensorNodeMessage.nodeID;
			lastReadStatus.statusCode = NEW_DATA_ARRIVED;
			strcpy(lastReadStatus.statusMessage, "");

//			sysStatus.set(sysStatus.RADIO, true);

			Log.debug("RadioSensorListener.read() - Message received from node %d", sensorNodeMessage.nodeID);

		}
		else{  
//			sysStatus.set(sysStatus.RADIO, false);
			lastReadStatus.nodeId = -1;
			lastReadStatus.statusCode = RADIO_CONFIG_ERROR;
			char errMsg[80];
			sprintf(errMsg, "Radio config error. PipeNumber:%u nodeId:%u");
			strcpy(lastReadStatus.statusMessage, errMsg);

			Log.error("RadioSensorListener.read() - config error! PipeNumber: %d, nodeID: %d", rfPipeNum, sensorNodeMessage.nodeID);
		}

	}
	else{
			lastReadStatus.nodeId = -1;
			lastReadStatus.statusCode = NO_DATA_CHANGED;
			strcpy(lastReadStatus.statusMessage, "");
	}
	
	return lastReadStatus;
}

void RadioSensorListener::sensorMessage2sensorData(WsnSensorNodeMessage &in, SensorData &out){
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
	Log.debug("ThingSpeakSensor::read() - key:%s start reading...", readKey);
	char jsonResponse[255];
	bool tsGetStatus = thingSpeakUtil.get(readKey, channel, jsonResponse); 
	Log.debug("ThingSpeakSensor::read() - GET response: %s", jsonResponse);
	if(tsGetStatus){
		Log.debug("ThingSpeakSensor::read() - parsing JSON response...", readKey);
		json2SensorData(jsonResponse, sensorDataOut);
		sensorDataOut.nodeId = this->nodeId;
		sensorDataOut.sensorSet = 	this->sensorSet;
		sensorDataOut.lastUpdateTime = now();

		lastReadStatus.nodeId = this->nodeId;
		lastReadStatus.statusCode = NEW_DATA_ARRIVED;
		strcpy(lastReadStatus.statusMessage, "");
		Log.debug("ThingSpeakSensor::read() - key:%s read OK");
	}
	else{
		lastReadStatus.nodeId = this->nodeId;
		lastReadStatus.statusCode = THING_SPEAK_READ_ERROR;
		strcpy(lastReadStatus.statusMessage, "Thing Speak read error");
		Log.debug("ThingSpeakSensor::read() - read error");
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
 * SensorScheduler
 * ********************************/
SensorScheduler::SensorScheduler(){
}

bool SensorScheduler::addTask(Sensor* sensor, uint32_t repeatMs){
	if (taskCnt < MAX_SCHEDULED_SENSORS - 1){
		taskArr[++taskCnt] = {sensor, repeatMs, 0};
		return true;
	}
	else{
		return false;
	}
}
	
SensorReadStatus SensorScheduler::execute(SensorData &sensorDataOut){
	SensorReadStatus status = {-1, NO_DATA_CHANGED, '\0'};
	if(taskCnt != 0){
		uint32_t currentMillis = millis();
		if (currentMillis - taskArr[execIdx].lastRead  >= taskArr[execIdx].repeatMs){
			// read sensor	
			status = taskArr[execIdx].sensor->read(sensorDataOut);
			// update lastRead millis
			taskArr[execIdx].lastRead = currentMillis;
			// calculate nexe natsIdx to run
			recalcExecIdx();
		}   
	}
	return status;
}

void SensorScheduler::recalcExecIdx(){
	const uint32_t safetyTime = 100000; // biztonsági okból, hogy a kivonás eredménye biztosan ne legyen negatív, ha az utolsó olvasás régebben volt mint a repeatMS értéke
	uint32_t currentMillis = millis();

	uint8_t _taskIdx = 0;
	uint32_t _minMillis = (taskArr[0].repeatMs + safetyTime) - (currentMillis - taskArr[0].lastRead);
	uint32_t _wrk;

	for (int i=1; i < taskCnt; i++){
		_wrk = (taskArr[i].repeatMs + safetyTime) - (currentMillis - taskArr[i].lastRead); 	
		if (_wrk < _minMillis){
			_minMillis = _wrk;
			_taskIdx = i;
		}
	}

	this->execIdx = _taskIdx;
	Log.debug("SensorScheduler::recalcExecIdx() - Next sensorIdx: %d, sensorType: %s, millis: %d", _taskIdx, taskArr[_taskIdx].sensor->getSensorType(), (_minMillis - safetyTime)); 
	return;
}

uint8_t SensorScheduler::getTaskCnt(){
	return taskCnt;
}
/**********************************
 * SensorDataCollector
 * ********************************/
SensorDataCollector::SensorDataCollector(){
}

void SensorDataCollector::setRadioSensor(RadioSensorListener* radioSensor){
	this->radioSensor = radioSensor;
}

bool SensorDataCollector::addSensor(Sensor* sensor, uint32_t repeatMs){
	return this->sensorScheduler.addTask(sensor, repeatMs);
}

SensorReadStatus SensorDataCollector::process(){
	SensorData sensorData;
	lastSensorReadStatus = {-1, NO_DATA_CHANGED, '\0'};

	if(radioSensor->getSensorType() != UNDEFINED_SENSOR){
		this->lastSensorReadStatus = radioSensor->read(sensorData);
		if (this->lastSensorReadStatus.statusCode == NEW_DATA_ARRIVED){
			sensorDataArr[sensorData.nodeId] = sensorData;
			return this->lastSensorReadStatus; 
		}
	}

	if(sensorScheduler.getTaskCnt() != 0){
		this->lastSensorReadStatus = sensorScheduler.execute(sensorData);
		if (this->lastSensorReadStatus.statusCode == NEW_DATA_ARRIVED){
			sensorDataArr[sensorData.nodeId] = sensorData;
		}
	}
	return this->lastSensorReadStatus;
}

SensorReadStatus SensorDataCollector::getStatus(){
	return this->lastSensorReadStatus;
}

SensorData SensorDataCollector::getSensorData(uint8_t nodeId){
	return this->sensorDataArr[nodeId];
}


