#include "TimeEventNotifier.h"

TimeEventNotifier::TimeEventNotifier(timeElement type){
	this->type = type;
}

bool TimeEventNotifier::registerObserver(TimeObserver* observer){
	if (cnt < MAX_OBSERVERS - 1){
		observerArr[cnt++] = observer;
		return true;
	}
	else{
		return false;
	}
}

void TimeEventNotifier::removeObserver(TimeObserver* observer){
    uint8_t newIndex = 0;
    for (uint8_t i = 0; i < this->cnt; i++){
        if (this->observerArr[i] != observer) {
            this->observerArr[newIndex++] = this->observerArr[i];
        }
    }
	 this->cnt = newIndex;
}

void TimeEventNotifier::notifyObservers(time_t currentTime){
	for (uint8_t i=0; i < cnt; i++){
		if (observerArr[i] != NULL){
			observerArr[i]->update(this->type, currentTime);
		}
	}
}

timeElement TimeEventNotifier::getType(){
	return this->type;
}

TimeEventHandler::TimeEventHandler(){
	 *minNotifier = TimeEventNotifier(TIME_ELEMENT_MINUTE);
	 *hourNotifier = TimeEventNotifier(TIME_ELEMENT_HOUR);
	 *dayNotifier = TimeEventNotifier(TIME_ELEMENT_DAY);

}

bool TimeEventHandler::registerMinObserver(TimeObserver* observer){
	return this->minNotifier->registerObserver(observer);	
}

bool TimeEventHandler::registerHourObserver(TimeObserver* observer){
	return this->hourNotifier->registerObserver(observer);	
}

bool TimeEventHandler::registerDayObserver(TimeObserver* observer){
	return this->dayNotifier->registerObserver(observer);	
}

void TimeEventHandler::removeMinObserver(TimeObserver* observer){
	this->minNotifier->removeObserver(observer);
}

void TimeEventHandler::removeHourObserver(TimeObserver* observer){
	this->hourNotifier->removeObserver(observer);
}

void TimeEventHandler::removeDayObserver(TimeObserver* observer){
	this->dayNotifier->removeObserver(observer);
}

void TimeEventHandler::process(){
	time_t ct = now();

	if (minute(ct) != this->lastMin){
		this->lastMin = minute(ct);
		this->minNotifier->notifyObservers(ct);
		if (hour(ct) != this->lastHour){
			this->lastHour = hour(ct);
			this->hourNotifier->notifyObservers(ct);
			if (day(ct) != this->lastDay){
				this->lastDay = day(ct);
				this->dayNotifier->notifyObservers(ct);
			}		
		}	
	}
}