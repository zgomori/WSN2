#include "MenuScreen.h"

MenuScreen::MenuScreen(TFT_eSPI* tft){
	this->tft = tft;
}

void MenuScreen::activate(){

}

void MenuScreen::deactivate(){
}

void MenuScreen::init(){
	tft->setRotation(0);
}

const char* MenuScreen::getName(){
	return "menuScreen";
}


void MenuScreen::onValamiTouch(){
	Serial.println("!!!!!!!MenuScreen::onValamiTouch");
}
