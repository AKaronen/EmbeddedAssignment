/*
 * PI_controller.c
 *
 *  Created on: Jan 22, 2025
 *      Author: alexk
 */



#define U_MAX 3.3
#define U_MIN 0

float* update_states(volatile float u_in, volatile float* x) {
	float next_x[6] = {0, 0, 0, 0, 0, 0};
	//float C[6] = {0, 0, 0, 0, 0, 1};
	float B[6] = {0.0471, 0.0377, 0.0404, 0.0485, 0.0373, 0.0539};
	float A[6][6] = {
			{0.9652, -0.0172, 0.0057, -0.0058, 0.0052, -0.251},
			{0.7732, 0.1252, 0.2315, 0.07, 0.1282, 0.7754},
			{0.8278, -0.7522, -0.0956, 0.3299, -0.4855, 0.3915},
			{0.9948, 0.2655, -0.3848, 0.4212, 0.3927, 0.2899},
			{0.7648, -0.4165, -0.4855, -0.3366, -0.0986, 0.7281},
			{1.1056, 0.7587, 0.1179, 0.0748, -0.2192, 0.1491}
	};
	// Matrix multiplication: x(k+h) = A*x(k) + B*u_in
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			next_x[i] += A[i][j] * x[j];
		}
		next_x[i] += B[i] * u_in;
	}

	// Update state vector
	for (int i = 0; i < 6; i++) {
		x[i] = next_x[i];
	}

	return x;
}


float calculate_pi_controller(volatile float u_ref, volatile float u_act , volatile float Ki, volatile float Kp, volatile float *pu1){
	float u1_old, error, u1_new, u_in;
	u1_old = *pu1;
	// Compute error
	error = u_ref - u_act;

	u1_new = u1_old + Ki * error;

	if (fabs(u1_new)>= U_MAX){
		u1_new = u1_old;
	}

	u_in = Kp * error + u1_new;
	u1_old = u1_new;
	*pu1 = u1_old;

	return u_in;

}

float runPI(volatile float u_ref, volatile float u_out,
		volatile float Ki, volatile float Kp, volatile float* pu1, volatile float* x){
	// TODO: Capture UART input for u_ref
	float u_in;
	u_in = calculate_pi_controller(u_ref, u_out, Ki, Kp, pu1);
	// Update state-space model
	x = update_states(u_in, x);

	u_out = x[5];
	u_out = (u_out>U_MAX) ? U_MAX : u_out;
	u_out = (u_out<U_MIN) ? U_MIN: u_out;
	return u_out; // y = Cx = [0 0 0 0 0 1]*x -> y = u_out = x[5]
}


