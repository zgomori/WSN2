#include "TouchEventNotifier.h"
#include "TouchObserver.h"

bool TouchEventNotifier::registerObserver(TouchObserver* observer){
	if (cnt < MAX_OBSERVERS - 1){
		observerArr[cnt++] = observer;
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
            this->observerArr[newIndex++] = this->observerArr[i];
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

