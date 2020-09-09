/*
 * ain.c
 *
 *  Created on: 8 џэт. 2020 у.
 *      Author: User
 */

#include "ain.h"
#include <stdint.h>
#include "os_conf.h"

extern unsigned short ain_raw[AI_CNT];
extern unsigned short ain[AI_CNT];
extern unsigned char ain_under[AI_CNT];
extern unsigned char ain_over[AI_CNT];
extern unsigned char ain_alarm[AI_CNT];
extern uint16_t ai_type;
extern uint8_t answer_90[246];

void update_ethip_ain() {
	uint8_t i = 0;
	for(i=0;i<AI_CNT;i++) {
		answer_90[i*2] = ain[i] & 0xFF;
		//if(ai_type & ((uint16_t)1<<i)) {answer_90[i*2] = ain_raw[i]/100;}	// mA
		//else { answer_90[i*2] = ain_raw[i]/10; }	// mV
		if(ain_alarm[i]) answer_90[i*2+1] = 0xFD;
		else if(ain_under[i]) answer_90[i*2+1] = 0xEF;
		else if(ain_over[i]) answer_90[i*2+1] = 0xF7;
		else answer_90[i*2+1] = 0xFF;
	}
}
