#include "rnlib.h"
#include "tOS_Kernel.h"
#include "tOS_Context.h"

extern volatile uint16_t pxCurrentTCB;

tOS_Kernel::tOS_Kernel() {
	createTask(tOS_Task("idle", _idle_task, tOS_Prio_IDLE));
	#ifdef VECTOR
		_curTask = _tasks.begin();
	#else
		_curTask = _tasks;
	#endif
}

tOS_Kernel::tOS_Kernel(tOS_Task_Callback idle_task) {
	createTask(tOS_Task("idle", idle_task, tOS_Prio_IDLE));
	#ifdef VECTOR
		_curTask = _tasks.begin();
	#else
		_curTask = _tasks;
	#endif
}

void tOS_Kernel::createTask(tOS_Task task) {
	#ifdef VECTOR
		_tasks.push(task);
	#else
		_tasks[_nTasks++] = task;
	#endif
}

void tOS_Kernel::createTask(tOS_Task& task) {
	#ifdef VECTOR
		_tasks.push(task);
	#else
		_tasks[_nTasks++] = task;
	#endif
}

void tOS_Kernel::start() {
	_init();
	_scheduler();
}

void tOS_Kernel::_rollTasks() {
	#ifdef VECTOR
		_tasks.push(_tasks.front());
		_tasks.pop_front();
	#else
		tOS_Task temp = _tasks[0];
		for (tOS_Size k=1; k<_nTasks; k++)
			_tasks[k-1] = _tasks[k];
		_tasks[_nTasks-1] = temp;
	#endif
}

void tOS_Kernel::delay_ms(tOS_Size ms) {
	#ifdef DEBUG	
		printf("block %s (SP: 0x%04x)\n", _curTask->name(), _curTask->sp());
	#endif
	_curTask->setState(tOS_State_BLOCKED);
	_curTask->setDelayed(ms);
	_scheduleISR();
}

void tOS_Kernel::_init() {
	RNLIB::initUSART(115200, 0x08, 0x01, PARNONE);
	// RNLIB::initTimer0(PRESC064, CTC0, NORMAL, 0x19-0x01);
	// RNLIB::initTimer1A(PRESC164, CTC1, NORMAL, 0xFA-0x01); 
	RNLIB::initTimer0(PRESC064, CTC0, NORMAL, 0xFA-0x01);
	RNLIB::initTimer1A(PRESC1256, CTC1, NORMAL, 0x271-0x01); 
}

void tOS_Kernel::_scheduler() {
	tOS_Task_Priority min_task_prio = tOS_Prio_NONE;

	if (_curTask->state() == tOS_State_RUNNING) {
		#ifdef DEBUG	
			printf("suspend %s (SP: 0x%04x)\n", _curTask->name(), _curTask->sp());
		#endif
		_curTask->setState(tOS_State_SUSPENDED);
	}

	for (auto& task : _tasks) {
		if (task.state() == tOS_State_BLOCKED && clock() >= task.delayed_until())
			task.setState(tOS_State_UNBLOCKED);


		if ((task.state() == tOS_State_READY || task.state() == tOS_State_SUSPENDED || task.state() == tOS_State_UNBLOCKED) && task.priority() < min_task_prio) {
			min_task_prio = task.priority();
			_nextTask = &task;
		}
	}

	if (_nextTask->state() == tOS_State_READY) {
		_curTask = _nextTask;
		#ifdef DEBUG	
			printf("run %s (SP: 0x%04x)\n", _curTask->name(), _curTask->sp());
		#endif
		_curTask->run();
	}
	else if (_nextTask->state() == tOS_State_SUSPENDED || _nextTask->state() == tOS_State_UNBLOCKED) {
		_curTask = _nextTask;
		_curTask->setState(tOS_State_RUNNING);
		#ifdef DEBUG
			printf("restore %s\n", _curTask->name());
		#endif
	}
}

void tOS_Kernel::_idle_task() {
	while (true) 
		;
	/* todo:
		- enter sleepmode
		- watchdog wake on isr
	*/
}

void tOS_Kernel::_scheduleISR() {
	SAVE_CONTEXT()
	_curTask->setSp(pxCurrentTCB);
	_scheduler();
	pxCurrentTCB = _curTask->sp();
	RESTORE_CONTEXT()
}

ISR(TIMER0_COMP_vect) {
	++tOS_Kernel::_ticks;
}

ISR(TIMER1_COMPA_vect, ISR_NAKED) {
	tOS_Kernel::_scheduleISR();
}
