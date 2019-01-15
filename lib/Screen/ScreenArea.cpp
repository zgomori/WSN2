#include "ScreenArea.h"

bool ScreenArea::isContain(uint16_t pX, uint16_t pY){
	if( (pX >= this->x) && (pX < (this->x + this->width)) && (pY >= this->y) && (pY < (this->y + this->height))){
		return true;
	}
	return false;
}
