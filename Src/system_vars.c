/*
 * system_vars.c
 *
 *  Created on: 15 апр. 2020 г.
 *      Author: User
 */

#include "system_vars.h"
#include "can_task.h"

extern uint8_t heartbeat_cnt[MAX_NODE_CNT];
extern uint8_t net_heartbeat_cnt[MAX_NET_CNT];
uint8_t can1_tmr = 30;
uint8_t telemetry_tmr = 30;
extern uint8_t can_addr;
extern uint8_t cluster_addr;

// SS1..SS8 (nodes online offline)
uint16_t node_link[8]={0};

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
uint16_t cluster_link[8]={0};

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
	if(MAX_NODE_CNT>=8) {
		for(i=0;i<8;i++) {
			if(heartbeat_cnt[i]<HEARTBEAT_MAX) node_link[i]=1;else node_link[i]=0;
		}
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
	if(MAX_NET_CNT>=8) {
			for(i=0;i<8;i++) {
				if(net_heartbeat_cnt[i]<HEARTBEAT_MAX) cluster_link[i]=1;else cluster_link[i]=0;
			}
		}
}
