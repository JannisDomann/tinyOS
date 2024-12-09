#pragma once

#include "tOS_Defines.h"
#include "rnlib.h"

#include <string.h>
#include <stdlib.h>

void tOS_Error(tOS_String error);
void tOS_Out(tOS_String str);

tOS_Size max(tOS_Size a, tOS_Size b);

class tOS_CriticalSection {
	public:
		static void enter() { cli(); } // todo: just disbale timer1A interrupt 
		static void leave() { sei(); } // todo: just disbale timer1A interrupt 
};

template <typename T>
class tOS_Vector {
	public:
		tOS_Vector(tOS_Size capacity=tOS_DEF_VEC_STD_CAP);
		~tOS_Vector();

		tOS_Size capacity() const { return _capacity; }
		tOS_Size size() const { return _size; }
		T& front() const { return _items[0]; }
		void push_front(T& item);
		void pop_front();
		virtual void push(T item);
		virtual void pop();
		T* begin() { return _items; }
		T* end() { return (_items+_size); }

	protected:
		tOS_Size _capacity;
		tOS_Size _size;
		T* _items;
};



template <typename T>
tOS_Vector<T>::tOS_Vector(tOS_Size capacity)
: _capacity(capacity), _size(0) {
	_items = (T*)malloc(sizeof(T)*_capacity);
}

template <typename T>
tOS_Vector<T>::~tOS_Vector() {
	free(_items);
}

template <typename T>
void tOS_Vector<T>::push_front(T& item) {
	if (_size == _capacity) {
		_capacity *= 2;
		T* new_items = (T*)malloc(sizeof(T)*_capacity);

		if (new_items) {
			memcpy(new_items+sizeof(T), _items, sizeof(T)*_size);
		}
		else {
			tOS_Error("could not allocate memory.");
			return;
		}
		new_items[0] = item;

		free(_items);
		_items = new_items;
	}
	else {
		memcpy(_items+sizeof(T), _items, sizeof(T)*_size);
		_items[0] = item;
	}

	_size++;
}

template <typename T>
void tOS_Vector<T>::pop_front() {
	if (_size != 0) {
		_size--;
		memcpy(_items, _items+sizeof(T), sizeof(T)*_size);
	}
}

template <typename T>
void tOS_Vector<T>::push(T item) {
	if (_size == _capacity) {
		_capacity *= 2;
		T* new_items = (T*)malloc(sizeof(T)*_capacity);

		if (new_items) {
			memcpy(new_items, _items, sizeof(T)*_size);
		}
		else {
			tOS_Error("could not allocate memory.");
			return;
		}
		new_items[_size] = item;

		free(_items);
		_items = new_items;
	}
	else {
		_items[_size] = item;
	}

	_size++;
}

template <typename T>
void tOS_Vector<T>::pop() {
	if (_size != 0) {
		_size--;
	}
}

