#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H


#include "Screen.h"
#include "TouchControl.h"
#include "WsnSensor.h"
#include "TimeEventHandler.h"
#include "TouchEventHandler.h"

#include "Fonts/orbitron_light_11_2.h"



#define CF_OL32 &Orbitron_Light_32
#define CF_ORB11 &orbitron_light11pt7b

#define COLOR_BG_STATUSBAR 0x18C3
#define COLOR_BG_CLOCK     0x0000
#define COLOR_BG_BLOCK1		0x18C3
#define COLOR_BG_BLOCK2		0x10A2
#define COLOR_ICON_NORMAL	0x7BEF
#define COLOR_ICON_ERROR	0xF800

class MainScreen: public Screen, public SensorObserver, public TimeObserver{
	private:
//		const char *dayShortNames = " Err Sun Mon Tue Wed Thu Fri Sat";
		const char *dayShortNames = " Err Vas HetKedd SzeCsut Pen Szo"; 



		void displaySensor0();
		void displaySensor1();
		void displaySensor6();
		void displayClock(time_t currentTime);
		void displayDate(time_t currentTime);
		void onValamiTouch();
		

	public:
		TouchControl<MainScreen> valamiTouchControl = TouchControl<MainScreen>(this, &MainScreen::onValamiTouch);

		MainScreen(TFT_eSPI* tft); 
		void activate() override;  //Screen interface
		void deactivate() override; //Screen interface
		void init() override; //Screen interface
		const char* getName() override; //Screen interface
		void onSensorChange(SensorData* sensorData) override; // SensorObserver interface
		void onMinuteChange(time_t currentTime) override; // TimeObserver interface
		void onDayChange(time_t currentTime) override; // TimeObserver interface
}; 

extern SensorDataCollector dataCollector;
extern TimeEventHandler timeEventHandler;
extern TouchEventHandler touchEventHandler; 

#endif