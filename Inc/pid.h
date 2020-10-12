/*
 * pid.h
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: User
 */

#ifndef PID_H_
#define PID_H_

struct pid_data {
	double int_sum;
	double prev_err;
	unsigned char init;
};

double calculate_pid(struct pid_data *state, double real_value, double target, double offset, double k_pr, double ti, double td, double min, double max);

#endif /* PID_H_ */
