/*
 * PI_controller.h
 *
 *  Created on: Jan 22, 2025
 *      Author: alexk
 */

#ifndef EXTERNAL_PI_CONTROLLER_H_
#define EXTERNAL_PI_CONTROLLER_H_

#define U_MAX 3.3
#define U_MIN 0

float* update_states(float u_in, volatile float* x);

float calculate_pi_controller(volatile float u_ref, volatile float u_act ,volatile float
								Ki, volatile float Kp, volatile float *pu1);

float runPI(volatile float u_ref, volatile float u_out,
		volatile float Ki, volatile float Kp,
		volatile float* pu1, volatile float* x);


#endif /* EXTERNAL_PI_CONTROLLER_H_ */
