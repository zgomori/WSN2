#include "TimeEventHandler.h"

/************************
 * TimeEventNotifier    *
 ************************/ 
TimeEventNotifier::TimeEventNotifier(TimeEventType::timeElement type){
	this->type = type;
}

bool TimeEventNotifier::registerObserver(TimeObserver* observer){
	if (cnt < MAX_OBSERVERS - 1){
		observerArr[cnt] = observer;
		++cnt;
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
            this->observerArr[newIndex] = this->observerArr[i];
				++newIndex;
        }
    }
	 this->cnt = newIndex;
}

void TimeEventNotifier::notifyObservers(time_t currentTime){
	for (uint8_t i=0; i < cnt; i++){
		if (observerArr[i] != NULL){
			switch (this->type){
				case TimeEventType::MINUTE :
					observerArr[i]->onMinuteChange(currentTime);
					break;
				case TimeEventType::HOUR :
					observerArr[i]->onHourChange(currentTime);
					break;
				case TimeEventType::DAY :
					observerArr[i]->onDayChange(currentTime);
					break;
			}	
		}
	}
}

TimeEventType::timeElement TimeEventNotifier::getType(){
	return this->type;
}

/************************
 * TimeEventHandler     *
 ************************/ 
TimeEventHandler::TimeEventHandler(){
	 this->minNotifier = new TimeEventNotifier(TimeEventType::MINUTE);
	 this->hourNotifier = new TimeEventNotifier(TimeEventType::HOUR);
	 this->dayNotifier = new TimeEventNotifier(TimeEventType::DAY);
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