#ifndef TOUCH_EVENT_HANDLER_H
#define TOUCH_EVENT_HANDLER_H

#include "TouchEventNotifier.h"

class TouchEventHandler: public TouchEventNotifier{
	private:
		uint16_t touchCalibrateData[5] =  { 213, 3571, 377, 3516, 4 };
		TFT_eSPI* tft;
		uint32_t touchedMillis = millis();

	public:
		TouchEventHandler(TFT_eSPI* tft);
		void listenEvent(); 	
};

#endif