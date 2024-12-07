#include "tOS_Kernel.h"
// #include "tOS_Queue.h"

tOS_Kernel kernel;
// tOS_Queue<tOS_String> q;

// void task1() {
// 	for (;;) {
// 		RNLIB::setLed(0x01);
// 		printf("led1 on\n");
// 		kernel.delay_ms(2356);
// 		RNLIB::clearLed(0x01);
// 		printf("led1 off\n");
// 		kernel.delay_ms(1856);
// 	}
// }

// void task2() {
// 	for (;;) {
// 		RNLIB::setLed(0x02);
// 		printf("led2 on\n");
// 		kernel.delay_ms(345);
// 		RNLIB::clearLed(0x02);
// 		printf("led2 off\n");
// 		kernel.delay_ms(635);
// 	}
// }

void task3() {
	for (;;) {
		printf("led3 on\n");
		kernel.delay_ms(1000);
	}
}

void task4() {
	for (;;) {
		printf("led4 on\n");
		kernel.delay_ms(963);
	}
}

// void printTask() {
// 	for (;;) {
// 		if (q.size() > 0) {
// 			printf(q.front());
// 			q.pop();
// 		}
// 	}
// }

int main() {
	// kernel.createTask(tOS_Task("task1", task1, tOS_Prio_UPPERNORMAL));
	// kernel.createTask(tOS_Task("task2", task2));
	kernel.createTask(tOS_Task("task3", task3, tOS_Prio_UPPERNORMAL));
	kernel.createTask(tOS_Task("task4", task4, tOS_Prio_UPPERNORMAL));
	// kernel.createTask(tOS_Task("print", printTask));

	kernel.start();
 
	return 0;
}
