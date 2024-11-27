#pragma once

#include "tOS_Util.h"
#include "tOS_Defines.h"
#include "tOS_Task.h"

// #define DEBUG
// #define VECTOR

extern "C" void TIMER0_COMP_vect(void) __attribute__((signal));
extern "C" void TIMER1_COMPA_vect(void) __attribute__((signal, naked));

class tOS_Kernel {
	public:
		tOS_Kernel();
		tOS_Kernel(tOS_Task_Callback idle_task);

		void createTask(tOS_Task& task);
		void createTask(tOS_Task task);
		void start(); 
		void delay_ms(tOS_Size ms);
		static volatile tOS_Size clock() { return _ticks; }
		tOS_Task* curTask() { return _curTask; }

	private:
		inline static volatile tOS_Size _ticks = 0;
			#ifdef VECTOR
				inline static tOS_Vector<tOS_Task> _tasks;
			#else
				inline static tOS_Size _nTasks = 0;
				inline static tOS_Task _tasks[tOS_DEF_VEC_STD_CAP];
			#endif
		inline static tOS_Task* _curTask = nullptr;
		inline static tOS_Task* _nextTask = nullptr;

		inline static tOS_Size _rrToken = 0;

		static void _init();
		static void _scheduleISR(); 
		static void _scheduler(); 
		static void _idle_task();

		friend void ::TIMER0_COMP_vect(void);
		friend void ::TIMER1_COMPA_vect(void);
};

