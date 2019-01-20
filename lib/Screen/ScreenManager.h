#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "Screen.h"

class ScreenManager{
	public:
		ScreenManager(Screen* screens[], uint8_t screenArrSize);
		void showScreen(char* screenName);

	private:
		Screen** screenArr;
		uint8_t screenArrSize;
		Screen* activeScreen = NULL;

		Screen* getSreenByName(char* screenName);
};

#endif