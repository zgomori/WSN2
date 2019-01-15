#ifndef SCREEN_H
#define  SCREEN_H

#include "TFT_eSPI.h"
#include "WsnSensor.h"
#include "TimeEventNotifier.h"
#include "Custom/orbitron_light_11_2.h"

class ScreenArea{
	public:
		bool isContain(uint16_t pX, uint16_t pY);
	protected:
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
};

class TouchObserver: public ScreenArea{
	public:
		virtual void execute(){};
};

template<typename T> 
class TouchControl: public TouchObserver{
	typedef void (T::*MemberFn)(); 

	public:
		TouchControl(){};
		TouchControl(T* objInstance, MemberFn memberFn){
			this->memberFn = memberFn;
			this->objInstance = objInstance;
		};
		void setCallbackFunction(T* objInstance, MemberFn memberFn){
			this->memberFn = memberFn;
			this->objInstance = objInstance;
		}

		void execute(){
			(objInstance->*memberFn)();
		}

	private:
		MemberFn memberFn;
		T* objInstance; 
};


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
	private:
		uint16_t touchCalibrateData[5] =  { 213, 3571, 377, 3516, 4 };
		TFT_eSPI* tft;
		uint32_t touchedMillis = millis();

	public:
		TouchEventHandler(TFT_eSPI* tft);
		void listenEvent(); 	
};
/*

class TouchHelperInterface{
	public:
		virtual void execute(){};
};

template<typename T> 
class TouchHelper: public TouchHelperInterface{
	typedef void (T::*MemberFn)(); 

	public:
		TouchHelper(){};
		TouchHelper(T* objInstance, MemberFn memberFn){
			this->memberFn = memberFn;
			this->objInstance = objInstance;
		};
		void setCallbackFunction(T* objInstance, MemberFn memberFn){
			this->memberFn = memberFn;
			this->objInstance = objInstance;
		}

		void execute(){
			(objInstance->*memberFn)();
		}

	private:
		MemberFn memberFn;
		T* objInstance; 

};
*/

class Screen{
	protected:
		TFT_eSPI* tft;

	public:
//		Screen(TFT_eSPI* tft);
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
		void init() override ; //Screen interface
		void onSensorChange(SensorData* sensorData) override; // SensorObserver interface
		void onMinuteChange(time_t currentTime) override; // TimeObserver interface
		void onDayChange(time_t currentTime) override; // TimeObserver interface
}; 

/*
class TouchHelper{
	typedef void (MainScreen::*MemberFn)(); 


	public:
		TouchHelper();
		void setCallbackFunction(MainScreen* mainScreen, MemberFn memberFn);
		void execute();

	private:
		MemberFn memberFn;
		MainScreen* mainScreen; 

};
*/





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
extern TimeEventHandler timeEventHandler;


#endif 
