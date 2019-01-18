#ifndef SCREEN_H
#define  SCREEN_H


#include "TFT_eSPI.h"

class Screen{
	protected:
		TFT_eSPI* tft;

	public:
		TFT_eSPI* getTft();
		virtual void activate();
		virtual void deactivate();
		virtual void init();
		virtual const char* getName();
};

#endif 
