#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "Screen.h"

template<uint8_t SIZE> 
class ScreenManager{
	public:
		ScreenManager();
		void show(char* screenName){
			Screen* screen = this->getSreenByName(screenName);
			if(screen != NULL){
				if(this->activeScreen != NULL){
					this->activeScreen->deactivate();
				}	
				this->activeScreen = screen;
				screen->init();
				screen->activate();
			}
		}

	private:
		Screen* screenArr[SIZE] = {NULL};
		Screen* activeScreen = NULL;

		Screen* getSreenByName(char* screenName){
			for (uint8_t i=0; i < SIZE; i++){
				if( (screenArr[i] != NULL) && strcmp(screenName, screenArr[i]->getName()) ){
					return screenArr[i];
				}	
			}	
			return screenArr[0];
		}
};

#endif