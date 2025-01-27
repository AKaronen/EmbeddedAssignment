/*
 * setup.c
 *
 *  Created on: Jan 22, 2025
 *      Author: alexk
 */


#include <stdlib.h>
#include <bitwise.h>
#include <stm32f4xx.h>
#include <setup.h>
//#include <uart_utils.h>
#include <ISR.h>



void setupGPIO(){
	// enable GPIOC & GPIOA clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;


	// configure user LED (PA5) as alternate
	bits_val(GPIOA->MODER , 2, 5, 2); // PA5 -> alternate function mode
	bits_val(GPIOA->AFR[0], 4, 5, 1); // Set PA5 to TIM1/TIM2 alt mode

	// configure user button (PC13) as pull-up input
	bits_val(GPIOC->PUPDR, 2, 13, 1);

}


void setupEXTI(){
	// route external interrupt 13 to GPIOC
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;    // clear EXTI13
	SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC; // set EXTI13 to GPIOC

	// configure external interrupt 13
	bit_set(EXTI->IMR , 13);        // enable interrupt
	bit_clr(EXTI->RTSR, 13);        // disable rising edge
	bit_set(EXTI->FTSR, 13);        // enable falling edge
	NVIC_EnableIRQ(EXTI15_10_IRQn); // enable IRQ
}

void setupClocks(){
	// enable timer 2, 3 and 5
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM5EN;;
	// enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

void setupTimers(){

	// configure timer 2 (PWM capture compare mode)
	TIM2->PSC   = prescaler;
	TIM2->ARR   = period;
	TIM2->CR1 |= TIM_CR1_ARPE; // Auto-reload preload
	TIM2->CCR1 = 0;

	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // Preload enabled

	TIM2->EGR |= TIM_EGR_UG; // update event generation
	TIM2->CCER |= TIM_CCER_CC1E;
	TIM2->CR1 |= TIM_CR1_CEN;


	// configure timer 3 for external interrupts
	TIM3->PSC   = 1 << 10;
	TIM3->ARR   = 1 << 10;
	TIM3->DIER |= TIM_DIER_UIE; // enable interrupt
	TIM3->CR1  |= TIM_CR1_URS;  // set counter overflow as only event source
	NVIC_EnableIRQ(TIM3_IRQn);  // enable IRQ



	// Configure timer 5 for PI controller timing
	TIM5->PSC   = (SystemCoreClock/100000)-1; // 100kHz clock
	TIM5->ARR   = 100; // 1ms ticker
	TIM5->DIER |= TIM_DIER_UIE; // enable interrupt
	TIM5->CR1  |= TIM_CR1_URS;  // set counter overflow as only event source
	NVIC_EnableIRQ(TIM5_IRQn);  // enable IRQ
	TIM5->CR1 |= TIM_CR1_CEN;
}

//void setupUSART(){
//	// enable USART2 clock
//	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
//
//	// set PA2 & PA3 alternate functions to AF7 (USART2 RX/TX)
//	bits_val(GPIOA->AFR[0], 4, 2, 7); // PA2 -> USART2_TX
//	bits_val(GPIOA->AFR[0], 4, 3, 7); // PA3 -> USART2_RX
//	bits_val(GPIOA->MODER , 2, 2, 2); // PA2 -> alternate function mode
//	bits_val(GPIOA->MODER , 2, 3, 2); // PA3 -> alternate function mode
//
//	// configure UART as 8N1 at 115200bps
//	USART2->BRR  = baud(115200);
//	USART2->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;
//}
