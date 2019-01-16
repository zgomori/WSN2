#ifndef TIME_OBSERVER_H
#define TIME_OBSERVER_H


class TimeObserver{
	public:
		virtual void onMinuteChange(time_t currentTime){};
		virtual void onHourChange(time_t currentTime){};
		virtual void onDayChange(time_t currentTime){};
};

#endif