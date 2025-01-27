/*
 * tasks.h
 *
 *  Created on: Jan 23, 2025
 *      Author: alexk
 */

#ifndef EXTERNAL_TASKS_H_
#define EXTERNAL_TASKS_H_

#define PRINTING 0 // Print info through uart
#define UART_COMMS 4 // Ask for parameters through uart


// Actual states
#define IDLE 1
#define MODULATING 2
#define CONFIG 3

// Number of tasks
#define N_TASKS 5

struct tasks {
	char* description;
	unsigned short tid;
	unsigned short timcount; /* Counter Tick of an individual task*/
	unsigned short timval; /* task execution interval */
	unsigned char taskpri;
	unsigned char disabled; /* not to be executed at a moment */
	unsigned char taskrunning; /* active = running now */
	unsigned char runtask; /* pending, in execution turn */
	unsigned char TaskExecuted:1; /* one bit for fun, out from task before new start*/
};

void initTasks( void );
void disableTask(int tid);
void enableTask(int tid);
void setTaskExecuted(int tid);
void checkPriority(struct tasks t, int pos);

extern volatile struct tasks task[N_TASKS];
extern volatile unsigned char tasktimeout;
#endif /* EXTERNAL_TASKS_H_ */
