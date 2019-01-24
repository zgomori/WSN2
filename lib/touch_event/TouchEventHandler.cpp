#include "TouchEventHandler.h"

/*************************
 * TouchEventNotifier    *
 * ***********************/
bool TouchEventNotifier::registerObserver(TouchObserver* observer){
	if (cnt < MAX_OBSERVERS - 1){
		observerArr[cnt] = observer;
		++cnt;
		return true;
	}
	else{
		return false;
	}
}

void TouchEventNotifier::removeObserver(TouchObserver* observer){
    uint8_t newIndex = 0;
    for (uint8_t i = 0; i < this->cnt; i++){
        if (this->observerArr[i] != observer) {
            this->observerArr[newIndex] = this->observerArr[i];
				++newIndex;
        }
    }
	 this->cnt = newIndex;
}

void TouchEventNotifier::notifyObservers(uint16_t touchX, uint16_t touchY){
	for (uint8_t i=0; i < cnt; i++){
		if ((observerArr[i] != NULL) && observerArr[i]->isContain(touchX, touchY)){
			observerArr[i]->execute();
		}
	}
}

/*************************
 * TouchEventHandler     *
 * ***********************/
TouchEventHandler::TouchEventHandler(TFT_eSPI* tft){
		this->tft = tft;
		tft->setTouch(touchCalibrateData);
	}
	
void TouchEventHandler::readEvent(){
		uint16_t touchX = 0, touchY = 0; 
		if (tft->getTouch(&touchX, &touchY) && (millis() - this->touchedMillis > 500)) {
			this->touchedMillis = millis();
			this->notifyObservers(touchX, touchY); 
		}
	} 
