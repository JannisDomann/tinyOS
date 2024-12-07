#include "tOS_Task.h"
#include "tOS_Defines.h"
#include "tOS_Kernel.h"
#include "tOS_Context.h"

volatile uint16_t pxCurrentTCB;

tOS_Task::tOS_Task()
: _name(""), _callback(nullptr), _priority(tOS_Prio_NONE), _state(tOS_State_SUSPENDED) {}

tOS_Task::tOS_Task(tOS_String name, tOS_Task_Callback callback, tOS_Task_Priority priority, tOS_Task_State state)
: _name(name), _callback(callback), _priority(priority), _state(state) {
	_sp = (uint16_t)(tOS_DEF_MIN_STACK_ADR + (++_cTasks)*tOS_DEF_LOCAL_STACK_SIZE);
}

void tOS_Task::setDelayed(tOS_Size delay) {
	_delayed_until = tOS_Kernel::clock()+delay;
}

void tOS_Task::run() {
	_state = tOS_State_RUNNING;
	
	pxCurrentTCB = sp();
	INIT_CONTEXT();
	_callback();

	if (_state == tOS_State_RUNNING)
		_state = tOS_State_TERMINATED;
}
