#ifndef TIME_EVENT_NOTIFIER_H
#define  TIME_EVENT_NOTIFIER_H

#include "time.h"

enum timeElement: uint8_t{
	TIME_ELEMENT_MINUTE = 0,
	TIME_ELEMENT_HOUR = 1,
	TIME_ELEMENT_DAY = 2
};

/**********************************
 * TimeObserver 
 * ********************************/
class TimeObserver{
	public:
		virtual void update(timeElement type, time_t currentTime){};
};

/**********************************
 * TimeEventNotifier
 * ********************************/

class TimeEventNotifier{
	protected:
		static const uint8_t MAX_OBSERVERS = 5;	
		TimeObserver* observerArr[MAX_OBSERVERS];
		int8_t cnt=0;
		timeElement type;

	public:
		TimeEventNotifier(timeElement type);
		bool registerObserver(TimeObserver* observer);
		void removeObserver(TimeObserver* observer);
		void notifyObservers(time_t currentTime);
		timeElement getType();
};

class TimeEventHandler{
	private:
		TimeEventNotifier* minNotifier;
		TimeEventNotifier* hourNotifier;
		TimeEventNotifier* dayNotifier;

		int8_t lastMin  = -1;
		int8_t lastHour = -1;
		int8_t lastDay  = -1;

	public:
		TimeEventHandler();
		bool registerMinObserver(TimeObserver* observer);
		bool registerHourObserver(TimeObserver* observer);
		bool registerDayObserver(TimeObserver* observer);

		void removeMinObserver(TimeObserver* observer);
		void removeHourObserver(TimeObserver* observer);
		void removeDayObserver(TimeObserver* observer);

		void process();
};


#endif // TIME_EVENT_NOTIFIER_H
