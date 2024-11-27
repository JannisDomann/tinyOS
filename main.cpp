#include "tOS_Kernel.h"
#include "tOS_Mutex.h"

tOS_Kernel kernel;
tOS_Mutex printMtx;

void task1() {
	while (true) {
		printMtx.unlock();
		kernel.delay_ms(1000);
		printMtx.lock();
		printf("t1 ");
		kernel.delay_ms(1000);
	}
}

void task2() {
	while (true) {
		printMtx.wait();
			printf("t2 ");
		kernel.delay_ms(1);
	}
}

void printTask() {

}

int main() {
	kernel.createTask(tOS_Task("task1", task1));
	kernel.createTask(tOS_Task("task2", task2));

	kernel.start();

	return 0;
}
