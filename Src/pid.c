/*
 * pid.c
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: User
 */

#include "pid.h"

extern volatile unsigned short plc_cycle;

double calculate_pid(struct pid_data *state, double real_value, double target, double offset, double k_pr, double ti, double td, double min, double max) {
	double res = 0;
	if(plc_cycle==0) return offset;
	if(state->init==0) {
		state->init = 1;
		state->prev_err = target - real_value;
	}else {
		double err = target - real_value;
		state->int_sum += err * plc_cycle/1000;
		double dy = 1000*(err-state->prev_err)*td/plc_cycle;
		if(k_pr==0) res=offset;
		else {
			if(ti==0) res=max;
			else res = offset + k_pr*(err + (state->int_sum)/ti + dy);
		}

		if(res<min) res=min;
		if(res>max) res=max;
		state->prev_err = err;
	}
	return res;
}
