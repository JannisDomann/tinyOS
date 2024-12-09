#pragma once

#include "tOS_Util.h"

template <typename T>
class tOS_Queue : private tOS_Vector<T> {
	public:
		tOS_Queue(tOS_Size capacity) : tOS_Vector<T>(capacity) {}
		tOS_Queue() : tOS_Vector<T>(10) {}

		tOS_Bool pending();
        void put(T item);
		T get();
};

template <typename T>
tOS_Bool tOS_Queue<T>::pending() {
	return this->size()>0;
}

template <typename T>
void tOS_Queue<T>::put(T item) {
	tOS_CriticalSection::enter();
	{
		if (tOS_Vector<T>::_size >= tOS_Vector<T>::_capacity)
			tOS_Error("full queue");
		else
			tOS_Vector<T>::push(item);
	}
	tOS_CriticalSection::leave();
}

template <typename T>
T tOS_Queue<T>::get() {
	T item;

	tOS_CriticalSection::enter();
	{
		item = this->front();
		tOS_Vector<T>::pop_front();
	}
	tOS_CriticalSection::leave();
	return item;
}
