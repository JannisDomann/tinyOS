#pragma once

#include "tOS_Util.h"

template <typename T>
class tOS_Queue : public tOS_Vector<T> {
	public:
		tOS_Queue(tOS_Size capacity) : tOS_Vector<T>(capacity) {}
		tOS_Queue() : tOS_Vector<T>(10) {}

        void push(T item) override;
};

template <typename T>
void tOS_Queue<T>::push(T item) {
	if (tOS_Vector<T>::_size >= tOS_Vector<T>::_capacity)
        tOS_Error("queue is full.");
	else
		tOS_Vector<T>::push(item);
}
