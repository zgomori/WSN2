#ifndef TOUCH_EVENT_HANDLER_H
#define TOUCH_EVENT_HANDLER_H

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

class TouchEventHandler: public TouchEventNotifier{
	public:
		TouchEventHandler(TFT_eSPI* tft);
		void listenEvent(); 	

	private:
		uint16_t touchCalibrateData[5] =  { 213, 3571, 377, 3516, 4 };
		TFT_eSPI* tft;
		uint32_t touchedMillis = millis();

};

#endif