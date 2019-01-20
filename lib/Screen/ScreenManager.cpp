#include "ScreenManager.h"


//public:
ScreenManager::ScreenManager(Screen* screens[], uint8_t screenArrSize){
	this->screenArr = screens;
	this->screenArrSize = screenArrSize;
};

void ScreenManager::showScreen(char* screenName){
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

//private:

Screen* ScreenManager::getSreenByName(char* screenName){
	for (uint8_t i=0; i < screenArrSize; i++){
		if( (screenArr[i] != NULL) && strcmp(screenName, screenArr[i]->getName()) ){
			return screenArr[i];
		}	
	}	
	return screenArr[0];
}
