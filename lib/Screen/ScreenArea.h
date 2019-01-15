#ifndef SCREEN_AREA_H
#define  SCREEN_AREA_H

#include "Arduino.h"

class ScreenArea{
	public:
		bool isContain(uint16_t pX, uint16_t pY);
	protected:
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
};

#endif