/*
 * ISR.h
 *
 *  Created on: Jan 23, 2025
 *      Author: alexk
 */

#ifndef EXTERNAL_ISR_H_
#define EXTERNAL_ISR_H_

void switchState(void);
void __attribute__((interrupt("IRQ"))) TIM3_IRQHandler(void);
void  __attribute__((interrupt("IRQ"))) TIM5_IRQHandler(void);
void __attribute__((interrupt("IRQ"))) EXTI15_10_IRQHandler(void);


// State of the machine
extern volatile unsigned char state;

#endif /* EXTERNAL_ISR_H_ */
