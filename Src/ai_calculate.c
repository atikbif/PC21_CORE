/*
 * ai_calculate.c
 *
 *  Created on: 2 но€б. 2020 г.
 *      Author: User
 */

#include "ai_calculate.h"

#define DI_ON_LEVEL		950
#define DI_BREAK_LEVEL	300
#define DI_SC_LEVEL		2000

// 0.4 - 2 V
#define	SENS1_LOW_LOW_LEVEL		200
#define SENS1_LOW_LEVEL			340
#define SENS1_HIGH_LEVEL		2050
#define SENS1_HIGH_HIGH_LEVEL	2200

// 4 - 20 mA
#define	SENS2_LOW_LOW_LEVEL		2000
#define SENS2_LOW_LEVEL			3400
#define SENS2_HIGH_LEVEL		20500
#define SENS2_HIGH_HIGH_LEVEL	22000

// 2 - 10 mA
#define	SENS3_LOW_LOW_LEVEL		1000
#define SENS3_LOW_LEVEL			1700
#define SENS3_HIGH_LEVEL		10250
#define SENS3_HIGH_HIGH_LEVEL	11000

static uint8_t ai = 0;
static uint8_t ai_over = 0;
static uint8_t ai_under = 0;
static uint8_t ai_alarm = 0;
static uint8_t di = 0;
static uint8_t di_br = 0;
static uint8_t di_sc = 0;
static uint8_t di_fault = 0;
static volatile uint8_t deb_ai_num = 0;
static volatile uint8_t deb_sens = 0;
static uint16_t deb_cnt = 0;

extern struct ai_mod* ai_modules_ptr;

void handle_adc(uint16_t raw_value, enum sens_type sensor, ai_mod_ptr mod, uint8_t ai_num) {
	float val = raw_value;
	if(ai_num>=MOD_AI_INP_CNT) return;
	ai = 0;
	ai_over = 0;
	ai_under = 0;
	ai_alarm = 0;
	di = 0;
	di_br = 0;
	di_sc = 0;
	di_fault = 0;
	deb_sens = sensor;
	deb_ai_num = ai_num;

	switch(sensor) {
		case SENSDI:
			if(raw_value==0) deb_cnt++;
			raw_value = val*500/62+0.5;
			if(raw_value>=DI_ON_LEVEL) di = 1;
			if(raw_value<DI_BREAK_LEVEL) di_br = 1;
			if(raw_value>=DI_SC_LEVEL) di_sc = 1;
			break;
		case SENS0p4_2V:
			if(val<400) val = 400;
			if(val>2000) val = 2000;
			val = 255*val/1600-255/4.0+0.5;
			if(val>255) val=255;
			if(raw_value<=SENS1_LOW_LOW_LEVEL) {
				ai_under=1;
				ai_alarm=1;
			}else if(raw_value<=SENS1_LOW_LEVEL) {
				ai_under = 1;
			}else if(raw_value>=SENS1_HIGH_HIGH_LEVEL) {
				ai=255;
				ai_alarm = 1;
				ai_over = 1;
			}else if(raw_value>=SENS1_HIGH_LEVEL) {
				ai = 255;
				ai_over = 1;
			}else {
				ai = (uint8_t)val;
			}
			break;
		case SENS0_20mA:
			raw_value = val*500/62+0.5;
			if(val>20000) val = 20000;
			val = 255*val/20000+0.5;
			if(val>255) val = 255;
			ai = (uint8_t)val;
			break;
		case SENS4_20mA:
			raw_value = val*500/62+0.5;
			if(val>20000) val = 20000;
			if(val<4000) val = 4000;
			val = 255*val/16000-255/4.0+0.5;
			if(val>255) val = 255;
			if(raw_value<=SENS2_LOW_LOW_LEVEL) {
				ai_alarm = 1;
				ai_under = 1;
			}else if(raw_value<=SENS2_LOW_LEVEL) {
				ai_under = 1;
			}else if(raw_value>=SENS2_HIGH_HIGH_LEVEL) {
				ai = 255;
				ai_alarm = 1;
				ai_over = 1;
			}else if(raw_value>=SENS2_HIGH_LEVEL) {
				ai = 255;
				ai_over = 1;
			}else {
				ai = (uint8_t)val;
			}
			break;
		case SENS2_10mA:
			raw_value = val*500/62+0.5;
			if(val>20000) val = 20000;
			if(val<4000) val = 4000;
			val = 255*val/8000-255/4.0+0.5;
			if(val>255) val = 255;
			if(raw_value<=SENS3_LOW_LOW_LEVEL) {
				ai_alarm = 1;
				ai_under = 1;
			}
			else if(raw_value<=SENS3_LOW_LEVEL) {
				ai_under = 0;
			}
			else if(raw_value>=SENS3_HIGH_HIGH_LEVEL) {
				ai = 255;
				ai_alarm = 1;
				ai_over = 1;
			}else if(raw_value>=SENS3_HIGH_LEVEL) {
				ai = 255;
				ai_over = 1;
			}
			else {
				ai = (uint8_t)val;
			}
			break;
		case SENS0_2p5V:
			if(val>2500) val = 2500;
			val = 255*val/2500+0.5;
			if(val>255) val=255;
			ai = (uint8_t)val;
			break;
	}
	if(sensor!=SENSDI) {
		deb_cnt++;
	}
	if(raw_value != mod->raw_value[ai_num]) {
		mod->raw_value[ai_num] = raw_value;
	}

	mod->sensor_value[ai_num] = ai;
	mod->ai_alarm[ai_num] = ai_alarm;
	mod->ai_under[ai_num] = ai_under;
	mod->ai_over[ai_num] = ai_over;
	mod->di[ai_num] = di;
	mod->di_br[ai_num] = di_br;
	mod->di_sc[ai_num] = di_sc;
	if(di_br || di_sc) mod->di_fault[ai_num] = 1;
	else mod->di_fault[ai_num] = 0;
}
