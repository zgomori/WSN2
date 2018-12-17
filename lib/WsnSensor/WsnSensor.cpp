#include "WsnSensor.h"


/**********************************
 * LocalBMESensor
 * ********************************/
LocalBMESensor::LocalBMESensor(Adafruit_BME280 &bmeReference, float elevation, int8_t nodeId){
	this->sensorType = LOCAL_SENSOR;
	this->bme = bmeReference;
	this->elevation = elevation;
	this->nodeId = nodeId;
	this->sensorSet = 23; // 10111
	this->lastReadStatus = {nodeId, NEW_DATA_ARRIVED, 0};
}

SensorReadStatus LocalBMESensor::read(SensorData &sensorDataOut){
	delay(1);
	bme.takeForcedMeasurement();
	delay(10);

	float temp = round(bme.readTemperature() * 10.0F) / 10.0F;
	float pressure = round((bme.readPressure() / 100.0F) + (1.2 * (elevation/10.0F)));
	float humidity = round(bme.readHumidity() * 10.0F) / 10.0F;

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
 * SensorDataCollector
 * ********************************/
SensorDataCollector::SensorDataCollector(){

}

void SensorDataCollector::setRadioSensor(RadioSensor radioSensor){
	this->radioSensor = radioSensor;
}

void SensorDataCollector::addSensor(Sensor sensor, uint32_t repeatMs){
	if (nextFreeScheduledSensorArrIdx < 4){
		scheduledSensorArr[++nextFreeScheduledSensorArrIdx] = {sensor, 0, repeatMs, 0};
	}
}
