#include "tOS_Kernel.h"
#include "tOS_Queue.h"

tOS_Kernel kernel;
tOS_Queue<tOS_String> q;

void task1() {
	for (;;) {
		RNLIB::setLed(0x00);
		q.put("led1 on\n");
		kernel.delay_ms(2356);
		RNLIB::clearLed(0x00);
		q.put("led1 off\n");
		kernel.delay_ms(1856);
	}
}

void task2() {
	for (;;) {
		RNLIB::setLed(0x02);
		q.put("led2 on\n");
		kernel.delay_ms(345);
		RNLIB::clearLed(0x02);
		q.put("led2 off\n");
		kernel.delay_ms(635);
	}
}

void task3() {
	for (;;) {
		RNLIB::setLed(0x03);
		q.put("led3 on\n");
		kernel.delay_ms(1000);
		RNLIB::clearLed(0x03);
		q.put("led3 off\n");
		kernel.delay_ms(1000);
	}
}

void task4() {
	for (;;) {
		RNLIB::setLed(0x01);
		q.put("led4 on\n");
		kernel.delay_ms(120);
		RNLIB::clearLed(0x01);
		q.put("led4 off\n");
		kernel.delay_ms(200);
	}
}

void printTask() {
	for (;;) {
		if (q.pending())
			printf(q.get());
		kernel.delay_ms(1);
	}
}

int main() {
	kernel.createTask(tOS_Task("print", printTask));
	kernel.createTask(tOS_Task("task2", task2));
	kernel.createTask(tOS_Task("task4", task4, tOS_Prio_LOWERNORMAL));
	kernel.createTask(tOS_Task("task1", task1, tOS_Prio_REALTIME));
	kernel.createTask(tOS_Task("task3", task3, tOS_Prio_UPPERNORMAL));

	kernel.start();
 
	return 0;
}
