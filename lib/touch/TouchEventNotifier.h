#ifndef TOUCH_EVENT_NOTIFIER_H
#define TOUCH_EVENT_NOTIFIER_H

#include "Arduino.h"
#include "TFT_eSPI.h"
#include "TouchObserver.h"

class TouchEventNotifier{
	protected:
		static const uint8_t MAX_OBSERVERS = 10;	
		TouchObserver* observerArr[MAX_OBSERVERS];
		int8_t cnt=0;

		void notifyObservers(uint16_t touchX, uint16_t touchY);
		
	public:
		bool registerObserver(TouchObserver* observer);
		void removeObserver(TouchObserver* observer);
};

#endif