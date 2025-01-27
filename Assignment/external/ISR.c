/*
 * ISR.c
 *
 *  Created on: Jan 23, 2025
 *      Author: alexk
 */

#include <stdio.h> // we can now use this
#include <bitwise.h>
#include <stm32f4xx.h>
#include <tasks.h>
#include <ISR.h>

volatile unsigned char state = IDLE;

void switchState(void){
	state++;
	if(state>CONFIG){
		state = IDLE;
	}
	switch (state) {
	case IDLE: // OFF
		// IDLING STUFF -> turn the converter off etc.
		disableTask(MODULATING);
		disableTask(CONFIG);
		disableTask(UART_COMMS);
		enableTask(IDLE);
		printf("IDLE -> LED OFF\r\n");
		break;

	case MODULATING:
		// MODULATING STUFF
		disableTask(MODULATING); // wait for u_ref through uart, enabled in UART task
		disableTask(CONFIG);
		enableTask(UART_COMMS);
		disableTask(IDLE);
		printf("MODULATING -> LED controlled by the PWM output\r\n");
		break;

	case CONFIG: // on
		// Set parameters Kp and Ki
		disableTask(MODULATING);
		enableTask(CONFIG);
		enableTask(UART_COMMS);
		disableTask(IDLE);
		printf("CONFIGURATING -> LED ON\r\n");
		break;
	default:
		__builtin_unreachable();
	}
}
// timer 3 IRQ handler (debounces button and changes state)
void __attribute__((interrupt("IRQ"))) TIM3_IRQHandler(){
	// button controls this state machine
	switchState();
	// re-enable button interrupt
	bit_set(EXTI->IMR, 13);
	TIM3->CR1 &= ~TIM_CR1_CEN;

	// clear interrupt flag
	TIM3->SR &= ~TIM_SR_UIF;
}

void  __attribute__((interrupt("IRQ"))) TIM5_IRQHandler(){
	for(int i=0;i<N_TASKS;i++)
	{
		if(!task[i].timcount) /* should we run this task[i] */
		{
			if(task[i].taskrunning) /* if still running */
				tasktimeout=1;/* timeout, not schedulable */
			task[i].timcount=task[i].timval; /* initialize to total waiting time*/
			if(!task[i].disabled)
			{
				task[i].runtask=1;
				checkPriority(task[i], i);/* tell that this should be executed */
			}
		}
		else
			task[i].timcount--; /* decrement waiting time */
	}
	TIM5->SR &= ~TIM_SR_UIF;
}

void __attribute__((interrupt("IRQ"))) EXTI15_10_IRQHandler(){
	// debounce with timer 3
	bit_clr(EXTI->IMR, 13);
	TIM3->CNT  = 0;
	TIM3->CR1 |= TIM_CR1_CEN;
	// clear interrupt flag
	bit_set(EXTI->PR, 13); // EXTI_PR bits are cleared by setting
}
