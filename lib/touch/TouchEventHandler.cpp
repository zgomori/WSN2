#include "TouchEventHandler.h"


TouchEventHandler::TouchEventHandler(TFT_eSPI* tft){
		this->tft = tft;
		tft->setTouch(touchCalibrateData);
	}
	
void TouchEventHandler::listenEvent(){
		uint16_t touchX = 0, touchY = 0; 
		if (tft->getTouch(&touchX, &touchY) && (millis() - this->touchedMillis > 500)) {
			this->touchedMillis = millis();
			this->notifyObservers(touchX, touchY); 
		}
	} 
