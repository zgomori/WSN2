#ifndef TIME_EVENT_NOTIFIER_H
#define  TIME_EVENT_NOTIFIER_H

#include "time.h"
#include "TimeObserver.h"

namespace TimeEventType{
	enum timeElement: uint8_t{
		MINUTE = 0,
		HOUR = 1,
		DAY = 2
	};
}


/************************
 * TimeEventNotifier    *
 * **********************/
class TimeEventNotifier{
	public:
		TimeEventNotifier(TimeEventType::timeElement type);
		bool registerObserver(TimeObserver* observer);
		void removeObserver(TimeObserver* observer);
		void notifyObservers(time_t currentTime);
		TimeEventType::timeElement getType();

	protected:
		static const uint8_t MAX_OBSERVERS = 5;	
		TimeObserver* observerArr[MAX_OBSERVERS] = {NULL};
		int8_t cnt=0;
		TimeEventType::timeElement type;
};


/************************
 * TimeEventHandler     *
 * **********************/
class TimeEventHandler{
	public:
		TimeEventHandler();
		bool registerMinObserver(TimeObserver* observer);
		bool registerHourObserver(TimeObserver* observer);
		bool registerDayObserver(TimeObserver* observer);

		void removeMinObserver(TimeObserver* observer);
		void removeHourObserver(TimeObserver* observer);
		void removeDayObserver(TimeObserver* observer);

		void readEvent();

	private:
		TimeEventNotifier* minNotifier;
		TimeEventNotifier* hourNotifier;
		TimeEventNotifier* dayNotifier;

		int8_t lastMin  = -1;
		int8_t lastHour = -1;
		int8_t lastDay  = -1;
};


#endif // TIME_EVENT_NOTIFIER_H
