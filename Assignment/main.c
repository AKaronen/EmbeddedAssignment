#include <stdio.h> // we can now use this
#include <sleep.h>
#include <stdlib.h>
#include <bitwise.h>
#include <stm32f4xx.h>
#include <setup.h>
#include <PI_controller.h>
#include <ISR.h>
#include <tasks.h>
#include <math.h>
#include <float.h>

// Global Variables


static volatile float duty_cycle;
static volatile float x[6] = {};
static volatile float Kp = 1.0;           // Default proportional gain
static volatile float Ki = 0.1;// Default integral gain
static volatile float u1 = 0;
static volatile float *pu1 = &u1;
static volatile float u_ref;     // Default output voltage
static volatile float u_out = 0;

// Semaphore for when uart takes over and button shouldn't work
static volatile int uart_sem = 0;

void resetModulation(void){
	for(int i=0; i<6;i++){
		x[i] = 0;
	}
	u1 = 0;
	u_out = 0;
}


void idle(){
	TIM2->CCR1 = 0; // set the led timer duty to 0%
	setTaskExecuted(IDLE);
}

void modulate(){ // Run the PI controller and set the duty cycle for the LED
	u_out = runPI(u_ref, u_out, Ki, Kp, pu1, x);
	duty_cycle = (fabs(u1)/U_MAX);
	if(isnan(u1)){
		printf("Converter parameters reset due to NaN output \r\n");
		resetModulation();
	}
	TIM2->CCR1 = ((float)period) * duty_cycle;
	setTaskExecuted(MODULATING);
}

void configurate(){
	resetModulation();
	TIM2->CCR1 = period; // set the led timer duty to 100%
	setTaskExecuted(CONFIG);
}



int PassingSemaphore(int sem){ // Enter Critical (disable timer/button interrupts)
	NVIC_DisableIRQ(EXTI15_10_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	NVIC_DisableIRQ(TIM5_IRQn);
	if(!sem){
		sem = 1;
	}
	return sem;
}
int ReleaseSemaphore(int sem){ // Exit critical (enable interrupts and clear pending external interrupt requests)

	while (NVIC_GetPendingIRQ(EXTI15_10_IRQn)) {
		bit_set(EXTI->PR, 13); // clear
		NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	}

	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM5_IRQn);


	sem = 0;
	return sem;
}



void askForParams(){
	int ret;
	if(state == MODULATING){ // If in modulating state, ask for u_ref


		printf("(input = u_ref): ");
		fflush(stdout);
		ret = scanf("%f", &u_ref);
		while (getchar() != '\r')
			;
		printf("\r\n");

		// sanity check
		if (ret != 1) {
			printf("Invalid input.\r\n");
			askForParams(); // Ask again
		}
		printf("U_ref = %f \r\n", u_ref);
	}
	else if (state == CONFIG){ //configurating (not called ever when IDLE), ask for Kp and Ki

		// Capture Kp and Ki through UART
		printf("(input = Kp Ki): ");
		fflush(stdout);
		ret = scanf("%f %f", &Kp, &Ki);
		while (getchar() != '\r')
			;
		printf("\r\n");

		// sanity check
		if (ret != 2) {
			printf("Invalid input.\r\n");
			askForParams(); // Ask again
		}

		printf("Kp = %f \r\n", Kp);
		printf("Ki = %f \r\n", Ki);
		printf("Modulation reset due to parameter change\r\n");

	}

	disableTask(UART_COMMS); // Disabled after first use within a mode
	enableTask(state); // enable CONFIG/MODULATION after parameters have been set
	setTaskExecuted(UART_COMMS);
}

void printInfo(){
	char* mode;
	switch (state) {
	case IDLE:
		mode = "IDLE";
		break;
	case MODULATING:
		printf("U: %.3f \r\n", fabs(u1));
		printf("(Kp, Ki): %.3f , %.3f\r\n", Kp, Ki);
		mode = "MODULATING";
		break;
	case CONFIG:
		printf("U: %.3f \r\n", fabs(u1));
		printf("(Kp, Ki): %.3f , %.3f\r\n", Kp, Ki);
		mode = "CONFIGURATING";
		break;
	default:
		__builtin_unreachable();
	}
	printf("MODE: %s \r\n", mode);
	setTaskExecuted(PRINTING);
}


void LoopTasks(void){
	for(int i = 0;i<N_TASKS;i++){
		//checkPriority(task[i], i);
		if(task[i].runtask)// Selected inside ISR to be executed
		{
			task[i].runtask=0;
			if(task[i].taskrunning){
				tasktimeout = 1;
				break;
			}

			if(!tasktimeout)
				switch(task[i].tid){// select task code function
				case PRINTING:
					printInfo();
					task[i].TaskExecuted=0;
					task[i].taskrunning=0;
					break;
				case IDLE:
					idle();
					task[i].TaskExecuted=0;
					task[i].taskrunning=0;
					break;
				case MODULATING:
					modulate();
					task[i].TaskExecuted=0;
					task[i].taskrunning=0;
					break;
				case CONFIG:
					configurate();
					task[i].TaskExecuted=0;
					task[i].taskrunning=0;
					break;
				case UART_COMMS:
					uart_sem = PassingSemaphore(uart_sem); // Enter critical
					askForParams();
					uart_sem = ReleaseSemaphore(uart_sem); 	// Exit Critical
					task[i].TaskExecuted=0;
					task[i].taskrunning=0;
					break;
				}

		}
	}

}


void setup(void){
	setupGPIO();
	setupClocks();
	setupEXTI();
	setupTimers();
}




int main()
{
	initTasks();
	setup();
	while (1)
	{
		LoopTasks();
	}
}








