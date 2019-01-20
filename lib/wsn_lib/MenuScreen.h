#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H


#include "Screen.h"
#include "TouchControl.h"

class MenuScreen: public Screen{
	private:
		void onValamiTouch();
		

	public:
		TouchControl<MenuScreen> valamiTouchControl = TouchControl<MenuScreen>(this, &MenuScreen::onValamiTouch);

		MenuScreen(TFT_eSPI* tft); 
		void activate() override;  //Screen interface
		void deactivate() override; //Screen interface
		void init() override; //Screen interface
		const char* getName() override; //Screen interface
}; 

 
#endif