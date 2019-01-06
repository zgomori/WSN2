#ifndef SCREEN_H
#define  SCREEN_H

#include "TFT_eSPI.h"
#include "WsnSensor.h"
#include <Fonts/Custom/orbitron_light_11_2.h>

class Screen{
	protected:
		TFT_eSPI* tft;

	public:
		Screen(TFT_eSPI* tft);
		TFT_eSPI* getTft();
		virtual void activate() = 0;
		virtual void deactivate() = 0;
		virtual void init() = 0;
};


#define CF_OL32 &Orbitron_Light_32
#define CF_ORB11 &orbitron_light11pt7b

#define COLOR_BG_STATUSBAR 0x18C3
#define COLOR_BG_CLOCK     0x0000
#define COLOR_BG_BLOCK1		0x18C3
#define COLOR_BG_BLOCK2		0x10A2
#define COLOR_ICON_NORMAL	0x7BEF
#define COLOR_ICON_ERROR	0xF800

class MainScreen: public Screen, public SensorObserver{
	private:
		void displaySensor0();
		void displaySensor1();
		void displaySensor6();

	public:
		void activate();
		void deactivate();
		void init();
		void update(SensorData* sensorData);
}; 




class DataField{
	private:
		uint16_t   x;
		uint16_t  y;
		uint16_t  bgColor;
		uint16_t  fgColor;
		GFXfont* font;
		uint8_t align;
		uint16_t padding;
		uint8_t decimal = 0;
		Screen* screen;

		void show(char* value);

	public:
		DataField(uint16_t x, uint16_t y, uint16_t bgColor, uint16_t fgColor, GFXfont* font, uint8_t align, uint16_t padding, uint8_t decimal, Screen* screen);

		void update(char* value);
		void update(float value);
		void update(int value);

};

extern SensorDataCollector dataCollector;

#endif 
