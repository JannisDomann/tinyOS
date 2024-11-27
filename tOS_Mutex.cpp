#include "tOS_Mutex.h"
#include "tOS_Kernel.h"

void tOS_Mutex::lock() {
	_locked = true;
}

void tOS_Mutex::unlock() {
	_locked = false;
}

tOS_Bool tOS_Mutex::locked() {
	return _locked;
}

void tOS_Mutex::wait() {
	while (_locked) {
		if (!_locked)
			return;
	}
}

tOS_Bool tOS_Mutex::wait_for_ms(tOS_Size ms) {
	tOS_Size t = tOS_Kernel::clock() + ms;
	while (tOS_Kernel::clock() < t) {
		if (!_locked)
			return true;
	}
	return false;
}
