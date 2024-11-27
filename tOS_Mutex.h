#pragma once

#include "tOS_Types.h"

class tOS_Mutex {
	public:
		void lock();
		void unlock();
		tOS_Bool locked();

		void wait();
		tOS_Bool wait_for_ms(tOS_Size ms);

	private:
		tOS_Bool _locked = false;
};
