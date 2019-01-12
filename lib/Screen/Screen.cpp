#include "Screen.h"

Screen::Screen(TFT_eSPI* tft){
	this->tft = tft;
}
TFT_eSPI* Screen::getTft(){
	return this->tft;
}

void MainScreen::activate(){
	dataCollector.registerObserver(this);
	timeEventHandler.registerMinObserver(this);
	timeEventHandler.registerDayObserver(this);
}

void MainScreen::deactivate(){
	dataCollector.removeObserver(this);
	timeEventHandler.removeMinObserver(this);
	timeEventHandler.removeDayObserver(this);
}

void MainScreen::init(){
	tft->setRotation(0);
	tft->setTextDatum(ML_DATUM);

	tft->fillRect(0,		0,		240,	24,	COLOR_BG_STATUSBAR);
	tft->fillRect(0,		24,	240,	100,	COLOR_BG_CLOCK);	

	tft->fillRect(0,		124,	120,	98,	COLOR_BG_BLOCK1);
	tft->setTextColor(TFT_DARKGREY, COLOR_BG_BLOCK1);
	tft->drawString("livingroom", 4, 134, 2);

	tft->fillRect(120,	124,	120,	98,	COLOR_BG_BLOCK2);
	tft->setTextColor(TFT_DARKGREY, COLOR_BG_BLOCK2);
	tft->drawString("child's room", 124, 134, 2);

	tft->fillRect(0,		222,	120,	98,	COLOR_BG_BLOCK2);
	tft->setTextColor(TFT_DARKGREY, COLOR_BG_BLOCK2);
	tft->drawString("outdoor", 4, 232, 2);

	tft->fillRect(120,	222,	120,	98,	COLOR_BG_BLOCK1);
	tft->setTextColor(TFT_DARKGREY, COLOR_BG_BLOCK1);
	tft->drawString("atm.pressure", 124, 232, 2);
}

void MainScreen::displaySensor0(){
	char charBuffer[10];
	tft->setTextPadding(100);
	tft->setTextDatum(MC_DATUM);  
	tft->setTextColor(TFT_GREEN, COLOR_BG_BLOCK1);
	tft->setFreeFont(CF_OL32);
	dtostrf(dataCollector.getSensorData(0).temperature, 1, 1, charBuffer);
	tft->drawString(charBuffer, 60, 159, 1);
	tft->setFreeFont(CF_ORB11);
	dtostrf(dataCollector.getSensorData(0).humidity, 1, 1, charBuffer);
	tft->drawString(charBuffer, 60, 194, 1);
	tft->setTextPadding(72);
	dtostrf(dataCollector.getSensorData(0).airPressure, 1, 1, charBuffer);
	tft->drawString(charBuffer, 180, 252, 1);

	/*display pressure chart hourly*/
	//displayPressureH(pressureHistoryHourly, atoi(sensorDataCache.getPressure(0)));
	tft->setTextPadding(0);
}

void MainScreen::displaySensor1(){
	char charBuffer[10];	
	tft->setTextPadding(100);
	tft->setTextDatum(MC_DATUM);  
	tft->setTextColor(TFT_GREEN, COLOR_BG_BLOCK2);
	tft->setFreeFont(CF_OL32);
	dtostrf(dataCollector.getSensorData(1).temperature, 1, 1, charBuffer);
	tft->drawString(charBuffer, 60, 257, 1);
	tft->setFreeFont(CF_ORB11);
	dtostrf(dataCollector.getSensorData(1).humidity, 1, 1, charBuffer);
	tft->drawString(charBuffer, 60, 292, 1);
	tft->setTextPadding(0);
}

void MainScreen::displaySensor6(){
	char charBuffer[10];	
	tft->setTextPadding(100);
	tft->setTextDatum(MC_DATUM);  
	tft->setTextColor(TFT_GREEN, COLOR_BG_BLOCK2);  
	tft->setFreeFont(CF_OL32);
	dtostrf(dataCollector.getSensorData(6).temperature, 1, 1, charBuffer);
	tft->drawString(charBuffer, 180, 159, 1);
	tft->setFreeFont(CF_ORB11);
	dtostrf(dataCollector.getSensorData(6).humidity, 1, 1, charBuffer);
	tft->drawString(charBuffer, 180, 194, 1);
	tft->setTextPadding(0);
}


void MainScreen::onSensorChange(SensorData* sensorData){
	switch (sensorData->nodeId){
		case 0 :
			displaySensor0();
			break;
		case 1 :
			displaySensor1();
			break;
		case 6 :
			displaySensor6();
			break;
	}
}

void MainScreen::onMinuteChange(time_t currentTime){
	char tftClock[6];
	sprintf(tftClock, "%u:%02u", hour(currentTime), minute(currentTime));
 	tft->setTextColor(TFT_GREEN,TFT_BLACK);
	tft->setTextDatum(MC_DATUM);
	tft->setTextPadding(140);  
	tft->drawString(tftClock, 120, 60, 7);
}

void MainScreen::onDayChange(time_t currentTime){
	char tftDate[6];
	char tftDOW[5];
	sprintf(tftDate, "%02u.%02u", month(currentTime), day(currentTime));
	strncpy(tftDOW, dayShortNames + (weekday() * 4), 4);
	tftDOW[4] = 0;

	tft->setTextColor(TFT_DARKGREY,TFT_BLACK);
	tft->setTextPadding(60); 
	tft->setFreeFont(CF_ORB11);
	tft->setTextDatum(ML_DATUM);
	tft->drawString(tftDate, 4, 100, 1);
	tft->setTextDatum(MR_DATUM);
	tft->drawString(tftDOW, 237, 100, 1);
}











DataField::DataField(uint16_t x, uint16_t y, uint16_t bgColor, uint16_t fgColor, GFXfont* font, uint8_t align, uint16_t padding, uint8_t decimal, Screen* screen){
	this->x = x; 
	this->y = y;
	this->bgColor = bgColor;
	this->fgColor = fgColor;
	this->font = font;
	this->align = align;
	this->padding = padding;
	this->decimal = decimal; 
	this->screen = screen;
}
void DataField::show(char* v){
	TFT_eSPI* tft = screen->getTft();
	tft->setTextPadding(padding);
	tft->setTextDatum(align);  
	tft->setTextColor(fgColor, bgColor);
	tft->setFreeFont(font);
	tft->drawString(v, x, y, 1);
}


void DataField::update(float value){
	char charBuffer[10];
	dtostrf(value, 1, decimal, charBuffer);
	show(charBuffer);
}

void DataField::update(char* value){
	show(value);
}

void DataField::update(int value){
	char charBuffer[10];
	itoa(value, charBuffer, 10);
	show(charBuffer);
}	