#ifndef SCREEN_H
#define SCREEN_H


#include "TFT_eSPI.h"

class Screen{
	public:
		virtual void activate();
		virtual void deactivate();
		virtual void init();
		virtual const char* getName();
	protected:
		TFT_eSPI* tft;
};

#endif 
