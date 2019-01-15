#ifndef TOUCH_CONTROL_H
#define TOUCH_CONTROL_H

#include "TouchObserver.h"

template<typename T> 
class TouchControl: public TouchObserver{
	typedef void (T::*MemberFn)(); 

	public:
		TouchControl(){};
		TouchControl(T* objInstance, MemberFn memberFn){
			this->memberFn = memberFn;
			this->objInstance = objInstance;
		};
		void setCallbackFunction(T* objInstance, MemberFn memberFn){
			this->memberFn = memberFn;
			this->objInstance = objInstance;
		}

		void execute(){
			(objInstance->*memberFn)();
		}

	private:
		MemberFn memberFn;
		T* objInstance; 
};

#endif