/*
 * setup.h
 *
 *  Created on: Jan 22, 2025
 *      Author: alexk
 */

#ifndef EXTERNAL_SETUP_H_
#define EXTERNAL_SETUP_H_

void setupGPIO(void);
void setupUSART(void);
void setupClocks(void);
void setupEXTI(void);
void setupTimers(void);
#define prescaler (SystemCoreClock/1000000)-1 // 1 MHz clock
#define period 100

#endif /* EXTERNAL_SETUP_H_ */
