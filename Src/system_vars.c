/*
 * system_vars.c
 *
 *  Created on: 15 апр. 2020 г.
 *      Author: User
 */

#include "system_vars.h"
#include "can_task.h"
#include "heartbeat.h"
#include <iodef.h>

uint8_t can1_tmr = 30;
uint8_t telemetry_tmr = 30;
extern uint8_t can_addr;
extern uint8_t cluster_addr;

// SS1..SS8 (nodes online offline)
uint16_t node_link[MAX_NODE_CNT]={0};

// SS9 PC21 Network Can
uint16_t can_link=0;

// SS10 startup impulse
uint16_t start_up=0;

// SS11 seconds since power up
uint32_t seconds = 0;

// SS12 minutes since power up
uint32_t minutes = 0;

// SS13 cluster status (not used)
uint16_t cluster_state=0;

// SS14 telemetry status
uint16_t telemetry_state=1;

// SS15 cluster number
uint16_t cluster_num=0;

// SS16..SS23 clusters online offline
uint16_t cluster_link[MAX_NET_CNT]={0};

// SS24 aidi link
extern uint8_t adc_link;

// должна вызываться каждые 100 мс
void update_system_vars() {
	static uint16_t ms_tmr = 0;
	static uint8_t sec_tmr = 0;
	ms_tmr+=100;
	if(ms_tmr==1000) {
		start_up = 0;
		ms_tmr = 0;
		seconds++;
		sec_tmr++;
		if(sec_tmr>=60) {
			sec_tmr = 0;
			minutes++;
		}
	}
	uint8_t i = 0;
	// ss1..ss8 (node link: 1 - online, 0 - offline)

	for(i=0;i<MAX_NODE_CNT;i++) {
		if(is_internal_node_offline(i)) node_link[i]=0;else node_link[i]=1;
	}

	// SS9 PC21 network CAN (1 - online, 0 - offline)
	if(can1_tmr<30) can1_tmr++;
	if(can1_tmr<30) can_link = 1;else can_link = 0;

	// SS14 telemetry status (0 - healthy, 1 - failed)
	if(telemetry_tmr<30) telemetry_tmr++;
	if(telemetry_tmr<30) telemetry_state = 0; else telemetry_state = 1;

	// SS15 cluster number (0 - not networked, 1..8 - number)
	if(can_addr) cluster_num = 0;else {
		cluster_num = cluster_addr+1;
	}

	// SS16..SS23 clusters online offline
	for(i=0;i<MAX_NET_CNT;i++) {
		if(is_external_node_offline(i)) cluster_link[i]=0;else cluster_link[i]=1;
	}
}

uint16_t getSSVar(uint8_t num) {
	uint16_t res = 0;
	switch(num) {
		case 0:res=SS1;break;
		case 1:res=SS2;break;
		case 2:res=SS3;break;
		case 3:res=SS4;break;
		case 4:res=SS5;break;
		case 5:res=SS6;break;
		case 6:res=SS7;break;
		case 7:res=SS8;break;
		case 8:res=SS9;break;
		case 9:res=SS10;break;
		case 10:res=SS11;break;
		case 11:res=SS12;break;
		case 12:res=SS13;break;
		case 13:res=SS14;break;
		case 14:res=SS15;break;
		case 15:res=SS16;break;
		case 16:res=SS17;break;
		case 17:res=SS18;break;
		case 18:res=SS19;break;
		case 19:res=SS20;break;
		case 20:res=SS21;break;
		case 21:res=SS22;break;
		case 22:res=SS23;break;
		case 23:res=SS24;break;
	}
	return res;
}
