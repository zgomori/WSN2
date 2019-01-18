#ifndef TOUCH_OBSERVER_H
#define TOUCH_OBSERVER_H

#include "Arduino.h"

class TouchObserver{
	public:
		virtual void execute(){};
		bool isContain(uint16_t pX, uint16_t pY){
			if( (pX >= this->x) && (pX < (this->x + this->width)) && (pY >= this->y) && (pY < (this->y + this->height))){
				return true;
			}
			return false;
		}
	private:
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;	
};

#endif