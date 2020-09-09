/*
 * elements.c
 *
 *  Created on: 18 окт. 2018 г.
 *      Author: Roman
 */

#include "elements.h"
#include <math.h>

unsigned char open_contact(unsigned char contact, unsigned char inpState){if(contact) return inpState;else return 0;}
unsigned char clos_contact(unsigned char contact, unsigned char inpState){if(contact==0) return inpState;else return 0;}

unsigned char plus_bool(unsigned char a, unsigned char b){return a+b;}
unsigned short plus_unsigned_short(unsigned short a, unsigned short b){return a+b;}
unsigned long plus_unsigned_long(unsigned long a, unsigned long b){return a+b;}
double plus_double(double a, double b){return a+b;}

unsigned char minus_bool(unsigned char a, unsigned char b){if(a<b) return 0;return a-b;}
unsigned short minus_unsigned_short(unsigned short a, unsigned short b){return a-b;}
unsigned long minus_unsigned_long(unsigned long a, unsigned long b){return a-b;}
double minus_double(double a, double b){return a-b;}

unsigned char relay_bool(unsigned char *v, unsigned char inpState){*v=inpState; return inpState;}
unsigned short relay_unsigned_short(unsigned short *v,unsigned short inpState){*v=inpState; return inpState;}
unsigned long relay_unsigned_long(unsigned long *v, unsigned long inpState){*v=inpState; return inpState;}
double relay_double(double *v, double inpState){*v=inpState; return inpState;}

unsigned char relay_enabled_bool(unsigned char *v, unsigned char inpState, unsigned short en) {if(en) *v=inpState; return *v;}
unsigned short relay_enabled_unsigned_short(unsigned short *v,unsigned short inpState, unsigned short en) {if(en) *v=inpState; return *v;}
unsigned long relay_enabled_unsigned_long(unsigned long *v, unsigned long inpState, unsigned short en) {if(en) *v=inpState; return *v;}
double relay_enabled_double(double *v, double inpState, unsigned short en) {if(en) *v=inpState; return *v;}

unsigned char const_bool(unsigned char value, unsigned short inpState){if(inpState) return value;else return 0;}
unsigned short const_unsigned_short(unsigned short value, unsigned short inpState){if(inpState) return value;else return 0;}
unsigned long const_unsigned_long(unsigned long value, unsigned short inpState){if(inpState) return value;else return 0;}
double const_double(double value, unsigned short inpState){if(inpState) return value;else return 0;}

unsigned char variable_bool(unsigned char v, unsigned short inpState){if(inpState) return v; else return 0;}
unsigned short variable_unsigned_short(unsigned short v, unsigned short inpState){if(inpState) return v; else return 0;}
unsigned long variable_unsigned_long(unsigned long v, unsigned short inpState){if(inpState) return v; else return 0;}
double variable_double(double v, unsigned short inpState){if(inpState) return v; else return 0;}

unsigned short sr_trig(unsigned short prev_value,unsigned short a, unsigned short b){
	if(a==0 && b==0) return prev_value;
	if(a==0 && b!=0) return 0;
	return 1;
}
unsigned short rs_trig(unsigned short prev_value,unsigned short a, unsigned short b){
	if(a==0 && b==0) return prev_value;
	if(a==0 && b!=0) return 1;
	return 0;
}

unsigned short counter(struct counter_state *state, unsigned char clk, unsigned char dir, unsigned char load, unsigned long preset) {
	if(load && load!=state->load) {
		state->counter = preset;
	}
	state->load = load;
	if(clk && clk!=state->clock) {
		if(dir) {
			if(state->counter < 0xFFFFFFFF) state->counter++;
		}else {
			if(state->counter) state->counter--;
		}
	}
	state->clock = clk;

	return state->counter;
}

unsigned short delay_on(unsigned short *ms_tmr, unsigned short *filter_on, unsigned short a, unsigned short b){
	/*if(a==0) {*ms_tmr=0;*filter_on=1;return 0;}
	else
	{
		if(*filter_on) {
			if(*ms_tmr>=b) {*filter_on=0;return 1;}
			else return 0;
		}else return 1;
	}*/

	if(a==0) {*ms_tmr=0;*filter_on=0;return 0;}
	else
	{
		if((*filter_on==0) && ((*ms_tmr<b) || (b==0))) *filter_on=1;

		if(*filter_on) {
			if(*ms_tmr>=b) {*filter_on=0;return 1;}
			else return 0;
		}else return 1;
	}
	return 0;
}
unsigned short delay_off(unsigned short *ms_tmr, unsigned short *filter_on, unsigned short a, unsigned short b){
	/*if(a) {*ms_tmr=0;*filter_on=1;return 1;}
	else
	{
		if(*filter_on) {
			if(*ms_tmr>=b) {*filter_on=0;return 0;}
			else return 1;
		}else return 0;
	}*/

	if(a) {*ms_tmr=0;*filter_on=0;return 1;}
	else
	{
		if((*filter_on==0) && ((*ms_tmr<b) || (b==0))) *filter_on=1;

		if(*filter_on) {
			if(*ms_tmr>=b) {*filter_on=0;return 0;}
			else return 1;
		}else return 0;
	}
	return 0;
}

unsigned short empty(unsigned short v){return 0;}

unsigned char greater_bool(unsigned char a, unsigned char b){return a>b;}
unsigned char greater_unsigned_short(unsigned short a,unsigned short b){return a>b;}
unsigned char greater_unsigned_long(unsigned long a, unsigned long b){return a>b;}
unsigned char greater_double(double a, double b){return a>b;}

unsigned char greater_or_equal_bool(unsigned char a, unsigned char b){return a>=b;}
unsigned char greater_or_equal_unsigned_short(unsigned short a,unsigned short b){return a>=b;}
unsigned char greater_or_equal_unsigned_long(unsigned long a, unsigned long b){return a>=b;}
unsigned char greater_or_equal_double(double a, double b){return a>=b;}

unsigned char less_bool(unsigned char a, unsigned char b){return a<b;}
unsigned char less_unsigned_short(unsigned short a, unsigned short b){return a<b;}
unsigned char less_unsigned_long(unsigned long a, unsigned long b){return a<b;}
unsigned char less_unsigned_double(double a, double b){return a<b;}

unsigned char less_or_equal_bool(unsigned char a, unsigned char b){return a<=b;}
unsigned char less_or_equal_unsigned_short(unsigned short a, unsigned short b){return a<=b;}
unsigned char less_or_equal_unsigned_long(unsigned long a, unsigned long b){return a<=b;}
unsigned char less_or_equal_double(double a, double b){return a<=b;}

unsigned char multiple_bool(unsigned char a, unsigned char b){return a*b;}
unsigned short multiple_unsigned_short(unsigned short a,unsigned short b){return a*b;}
unsigned long multiple_unsigned_long(unsigned long a, unsigned long b){return a*b;}
double multiple_double(double a, double b){return a*b;}

unsigned char divide_bool(unsigned char a, unsigned char b){if(b!=0) return a/b;else return 0;}
unsigned short divide_unsigned_short(unsigned short a, unsigned short b){if(b!=0) return a/b;else return 0;}
unsigned long divide_unsigned_long(unsigned long a, unsigned long b){if(b!=0) return a/b;else return 0;}
double divide_double(double a, double b){if(b!=0) return a/b;else return 0;}

unsigned char equal_bool(unsigned char a, unsigned char b){return a==b;}
unsigned short equal_unsigned_short(unsigned short a,unsigned short b){return a==b;}
unsigned long equal_unsigned_long(unsigned long a, unsigned long b){return a==b;}
unsigned char equal_double(double a, double b){return fabs(a-b)<=1e-5;}

unsigned char not_equal_bool(unsigned char a, unsigned char b){return a!=b;}
unsigned short not_equal_unsigned_short(unsigned short a,unsigned short b){return a!=b;}
unsigned long not_equal_unsigned_long(unsigned long a, unsigned long b){return a!=b;}
unsigned char not_equal_double(double a, double b){return fabs(a-b)>1e-5;}

unsigned char not_operation_bool(unsigned char a) {if(a) return 0;return 1;}
unsigned char not_operation_unsigned_short(unsigned short a) {if(a) return 0;return 1;}
unsigned char not_operation_unsigned_long(unsigned long a) {if(a) return 0;return 1;}
unsigned char not_operation_double(double a) {if(a>1e-5) return 0;return 1;}
