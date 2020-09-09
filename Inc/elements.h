/*
 * elements.h
 *
 *  Created on: 18 окт. 2018 г.
 *      Author: Roman
 */

#ifndef ELEMENTS_H_
#define ELEMENTS_H_

struct counter_state{
	unsigned char clock;
	unsigned char load;
	unsigned long counter;
};

unsigned short counter(struct counter_state *state, unsigned char clk, unsigned char dir, unsigned char load, unsigned long preset);

unsigned char open_contact(unsigned char contact, unsigned char inpState);
unsigned char clos_contact(unsigned char contact, unsigned char inpState);

unsigned char plus_bool(unsigned char a, unsigned char b);
unsigned short plus_unsigned_short(unsigned short a, unsigned short b);
unsigned long plus_unsigned_long(unsigned long a, unsigned long b);
double plus_double(double a, double b);

unsigned char minus_bool(unsigned char a, unsigned char b);
unsigned short minus_unsigned_short(unsigned short a, unsigned short b);
unsigned long minus_unsigned_long(unsigned long a, unsigned long b);
double minus_double(double a, double b);

unsigned char relay_bool(unsigned char *v, unsigned char inpState);
unsigned short relay_unsigned_short(unsigned short *v,unsigned short inpState);
unsigned long relay_unsigned_long(unsigned long *v, unsigned long inpState);
double relay_double(double *v, double inpState);

unsigned char relay_enabled_bool(unsigned char *v, unsigned char inpState, unsigned short en);
unsigned short relay_enabled_unsigned_short(unsigned short *v,unsigned short inpState, unsigned short en);
unsigned long relay_enabled_unsigned_long(unsigned long *v, unsigned long inpState, unsigned short en);
double relay_enabled_double(double *v, double inpState, unsigned short en);

unsigned char const_bool(unsigned char value, unsigned short inpState);
unsigned short const_unsigned_short(unsigned short value, unsigned short inpState);
unsigned long const_unsigned_long(unsigned long value, unsigned short inpState);
double const_double(double value, unsigned short inpState);

unsigned char variable_bool(unsigned char v, unsigned short inpState);
unsigned short variable_unsigned_short(unsigned short v, unsigned short inpState);
unsigned long variable_unsigned_long(unsigned long v, unsigned short inpState);
double variable_double(double v, unsigned short inpState);

unsigned short sr_trig(unsigned short prev_value,unsigned short a, unsigned short b);
unsigned short rs_trig(unsigned short prev_value,unsigned short a, unsigned short b);

unsigned short delay_on(unsigned short *ms_tmr, unsigned short *prev_ip, unsigned short a, unsigned short b);
unsigned short delay_off(unsigned short *ms_tmr, unsigned short *prev_ip, unsigned short a, unsigned short b);

unsigned short empty(unsigned short v);

unsigned char greater_bool(unsigned char a, unsigned char b);
unsigned char greater_unsigned_short(unsigned short a,unsigned short b);
unsigned char greater_unsigned_long(unsigned long a, unsigned long b);
unsigned char greater_double(double a, double b);

unsigned char greater_or_equal_bool(unsigned char a, unsigned char b);
unsigned char greater_or_equal_unsigned_short(unsigned short a,unsigned short b);
unsigned char greater_or_equal_unsigned_long(unsigned long a, unsigned long b);
unsigned char greater_or_equal_double(double a, double b);

unsigned char less_bool(unsigned char a, unsigned char b);
unsigned char less_unsigned_short(unsigned short a, unsigned short b);
unsigned char less_unsigned_long(unsigned long a, unsigned long b);
unsigned char less_double(double a, double b);

unsigned char less_or_equal_bool(unsigned char a, unsigned char b);
unsigned char less_or_equal_unsigned_short(unsigned short a, unsigned short b);
unsigned char less_or_equal_unsigned_long(unsigned long a, unsigned long b);
unsigned char less_or_equal_double(double a, double b);

unsigned char multiple_bool(unsigned char a, unsigned char b);
unsigned short multiple_unsigned_short(unsigned short a,unsigned short b);
unsigned long multiple_unsigned_long(unsigned long a, unsigned long b);
double multiple_double(double a, double b);

unsigned char divide_bool(unsigned char a, unsigned char b);
unsigned short divide_unsigned_short(unsigned short a, unsigned short b);
unsigned long divide_unsigned_long(unsigned long a, unsigned long b);
double divide_double(double a, double b);

unsigned char equal_bool(unsigned char a, unsigned char b);
unsigned short equal_unsigned_short(unsigned short a,unsigned short b);
unsigned long equal_unsigned_long(unsigned long a, unsigned long b);
unsigned char equal_double(double a, double b);

unsigned char not_equal_bool(unsigned char a, unsigned char b);
unsigned short not_equal_unsigned_short(unsigned short a,unsigned short b);
unsigned long not_equal_unsigned_long(unsigned long a, unsigned long b);
unsigned char not_equal_double(double a, double b);

unsigned char not_operation_bool(unsigned char a);
unsigned char not_operation_unsigned_short(unsigned short a);
unsigned char not_operation_unsigned_long(unsigned long a);
unsigned char not_operation_double(double a);


#endif /* ELEMENTS_H_ */
