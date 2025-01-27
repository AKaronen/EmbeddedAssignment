/*
 * tasks.c
 *
 *  Created on: Jan 23, 2025
 *      Author: alexk
 */

#include <stdio.h> // we can now use this
#include <sleep.h>
#include <stdlib.h>
#include <bitwise.h>
#include <stm32f4xx.h>
#include <setup.h>
#include <PI_controller.h>
//#include <uart_utils.h>
#include <ISR.h>
#include <tasks.h>
#include <math.h>

volatile struct tasks task[N_TASKS];
volatile unsigned char tasktimeout;
void initTasks( void )
{

	task[0].timval = 1000; // Print information every second
	task[0].taskpri = 100;
	task[0].tid = PRINTING;
	task[0].description = "PRINT";

	task[1].timval= 100; // IDLING
	task[1].taskpri = 50;
	task[1].tid = IDLE;
	task[1].description = "IDLE";


	task[2].timval = 100; // Run the PI-controller (100Hz)
	task[2].taskpri = 2;
	task[2].tid = MODULATING;
	task[2].description = "MODULATE";

	task[3].timval= 100; // Set config
	task[3].taskpri = 10;
	task[3].tid = CONFIG;
	task[3].description = "CONFIG";


	task[4].timval = 10; // Ask for params
	task[4].taskpri = 1;
	task[4].tid = UART_COMMS;
	task[4].description = "UART";

	for(int ii=0;ii<N_TASKS;ii++)
	{
		task[ii].timcount=task[ii].timval; // set counter to execution cycle (waiting time)
		task[ii].taskrunning=0;
		task[ii].runtask=0;
		task[ii].disabled=0;
		task[ii].TaskExecuted=0;
	}
	task[2].disabled = 1;
	task[3].disabled = 1;
	task[4].disabled = 1;
}

void disableTask(int tid){
	for(int i=0;i<N_TASKS;i++){
		if(task[i].tid == tid)
			task[i].disabled = 1;
	}
}

void enableTask(int tid){
	for(int i=0;i<N_TASKS;i++){
		if(task[i].tid == tid)
			task[i].disabled = 0;
	}
}

void setTaskExecuted(int tid){
	for(int i=0;i<N_TASKS;i++){
			if(task[i].tid == tid)
				task[i].TaskExecuted = 1;
		}
}


void checkPriority(struct tasks t, int pos){
	struct tasks temp;
	for(int i = 0; i< N_TASKS; i++){
		if((t.taskpri < task[i].taskpri)){
			temp = task[i];
			task[i] = task[pos];
			task[pos] = temp;
		}
	}
}








