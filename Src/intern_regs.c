/*
 * intern_regs.c
 *
 *  Created on: 5 февр. 2020 г.
 *      Author: User
 */

#include <stdint.h>
#include "os_conf.h"

extern unsigned short ireg[IREG_CNT];
extern uint8_t answer_95[128];
extern uint8_t answer_94[28];
extern unsigned char ibit[IBIT_CNT];
extern unsigned char scada_bits[16];
extern unsigned short scada_regs[16];
extern uint8_t answer_99[4];
extern uint8_t answer_9a[34];
extern uint8_t ob_99_upd;
extern uint8_t ob_9a_upd;

void update_ethip_intern_regs() {
	uint8_t i = 0;
	for(i=0;i<64;i++) {
		answer_95[i*2] = ireg[i]&0xFF;
		answer_95[i*2+1] = ireg[i]>>8;
	}
}

void update_ethip_intern_bits() {
	uint8_t i = 0;
	uint8_t byte_num;
	for(i=0;i<64;i++) {
		byte_num = i/8;
		if(ibit[i]) answer_94[byte_num] |= 1<<(i%8);
		else answer_94[byte_num] &= ~(1<<(i%8));
	}
}

void update_ethip_scada_bits() {
	uint8_t i = 0;
	if(ob_99_upd) {
		for(i=0;i<16;i++) {
			if(i<8) {
				if(answer_99[0]&(1<<i)) scada_bits[i]=1;else scada_bits[i]=0;
			}else {
				if(answer_99[1]&(1<<(i-8))) scada_bits[i]=1;else scada_bits[i]=0;
			}
		}
		ob_99_upd = 0;
	}else {
		for(i=0;i<16;i++) {
			if(scada_bits[i]) {
				if(i<8) answer_99[0]|=1<<i;
				else answer_99[1]|=1<<(i-8);
			}else {
				if(i<8) answer_99[0]&=~(1<<i);
				else answer_99[1]&=~(1<<(i-8));
			}
		}
	}

}

void update_ethip_scada_regs() {
	uint8_t i = 0;
	if(ob_9a_upd) {
		for(i=0;i<16;i++) {
			scada_regs[i] = ((uint16_t)answer_9a[i*2+1]<<8) | answer_9a[i*2];
		}
		ob_9a_upd=0;
	}else {
		for(i=0;i<16;i++) {
			answer_9a[i*2] = scada_regs[i] & 0xFF;
			answer_9a[i*2+1] = scada_regs[i] >> 8;
		}
	}
}
