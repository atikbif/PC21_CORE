/*
 * heartbeat.c
 *
 *  Created on: 2 но€б. 2020 г.
 *      Author: User
 */

#include "heartbeat.h"
#include "can_tx_stack.h"
#include "update_plc_data.h"
#include "ai_module.h"

extern uint8_t can_addr;
extern uint8_t cluster_addr;
extern tx_stack can1_tx_stack;
extern tx_stack can2_tx_stack;

static uint8_t heartbeat_cnt[MAX_NODE_CNT] = {HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX};
static uint8_t net_heartbeat_cnt[MAX_NET_CNT] = {HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX};

extern uint16_t ai_mod_cnt;
extern struct ai_mod* ai_modules_ptr;

void send_heartbeat() {
	static tx_stack_data packet;
	static uint16_t cnt = 0;
	static uint16_t net_cnt = 0;
	static uint8_t heartbeat_value = 1;
	static uint8_t net_heartbeat_value = 0;
	if(can_addr==0) {
		net_cnt++;
		if(net_cnt>=2000) {
			net_cnt = 0;
			packet.id = 0x0400 | 0x01 | (cluster_addr << 7);
			packet.length = 2;
			packet.data[0] = 0x49;
			packet.data[1] = net_heartbeat_value++;
			add_tx_can_packet(&can2_tx_stack,&packet);
		}
	}
	cnt++;
	if(cnt>=500) {
		update_net_status();
		cnt = 0;
		packet.id = 0x0400 | 0x01 | (can_addr<<3) | (cluster_addr << 7);
		packet.length = 2;
		packet.data[0] = 0x49;
		packet.data[1] = heartbeat_value++;
		add_tx_can_packet(&can1_tx_stack,&packet);
		if(can_addr<MAX_NODE_CNT) heartbeat_cnt[can_addr]=0;
	}
}

void increment_modules_heartbeats_counters() {
	static uint16_t mod_heartbeat_tmr = 0;
	mod_heartbeat_tmr++;
	if(mod_heartbeat_tmr>=1000) {
		mod_heartbeat_tmr = 0;
		for(uint8_t i=0;i<ai_mod_cnt;++i) {
			if(ai_modules_ptr[i].heartbeat_cnt<HEARTBEAT_MAX) {
				ai_modules_ptr[i].heartbeat_cnt++;
				ai_modules_ptr[i].link_state = 1;
			}else {
				ai_modules_ptr[i].link_state = 0;
			}
		}
	}
}

void increment_others_heartbeats_counters() {
	static uint16_t heartbeat_tmr = 0;
	static uint16_t net_heartbeat_tmr = 0;
	heartbeat_tmr++;
	if(heartbeat_tmr>=1000) {
		heartbeat_tmr = 0;
		for(uint8_t i=0;i<sizeof(heartbeat_cnt);i++) {
			if(heartbeat_cnt[i]<HEARTBEAT_MAX) heartbeat_cnt[i]++;
		}
	}

	net_heartbeat_tmr++;
	if(net_heartbeat_tmr>=2000) {
		net_heartbeat_cnt[cluster_addr]=0;
		net_heartbeat_tmr = 0;
		for(uint8_t i=0;i<sizeof(net_heartbeat_cnt);i++) {
			if(net_heartbeat_cnt[i]<HEARTBEAT_MAX) net_heartbeat_cnt[i]++;
		}
	}
}

uint8_t is_internal_node_offline(uint8_t node_num) {
	if(node_num>=MAX_NODE_CNT) return 0;
	if(heartbeat_cnt[node_num]==HEARTBEAT_MAX) return 1;
	return 0;
}

void clear_internal_heartbeat(uint8_t node_num) {
	if(node_num>=MAX_NODE_CNT) return;
	heartbeat_cnt[node_num] = 0;
}

uint8_t is_external_node_offline(uint8_t node_num) {
	if(node_num>=MAX_NET_CNT) return 0;
	if(net_heartbeat_cnt[node_num]==HEARTBEAT_MAX) return 1;
	return 0;
}

void clear_external_heartbeat(uint8_t node_num) {
	if(node_num>=MAX_NET_CNT) return;
	net_heartbeat_cnt[node_num] = 0;
}
