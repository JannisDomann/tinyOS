#pragma once

#include "tOS_Defines.h"
#include "tOS_Task.h"

class tOS_Kernel;

class tOS_Task {
	public:
		tOS_Task();
		tOS_Task(tOS_String name, tOS_Task_Callback callback, tOS_Task_Priority priority=tOS_Prio_NORMAL, tOS_Task_State state=tOS_State_READY);
		
		tOS_String name() const { return _name; }
		tOS_Task_Priority priority() const { return _priority; }
		tOS_Task_State state() const { return _state; }
		uint16_t sp() const { return _sp; }
		void setPriority(tOS_Task_Priority priority) { _priority = priority; }
		void setPriority(tOS_Size priority) { _priority = (tOS_Task_Priority)priority; }
		void setState(tOS_Task_State state) { _state = state; }
		void setSp(uint16_t sp) { _sp = sp; }
		void setDelayed(tOS_Size delay);
		tOS_Size delayed_until() const { return _delayed_until; }
		void run();

	private:
		inline static tOS_Size _cTasks = 0;
		uint16_t _sp;
		tOS_String _name;
		tOS_Task_Priority _priority;
		tOS_Task_State _state;
		tOS_Task_Callback _callback;
		tOS_Size _delayed_until = 0;
};
