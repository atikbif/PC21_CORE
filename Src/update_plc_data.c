/*
 * can_cmd.c
 *
 *  Created on: 2 нояб. 2020 г.
 *      Author: User
 */

#include "update_plc_data.h"
#include "can_tx_stack.h"
#include "iodef.h"
#include "os_conf.h"
#include "can_task.h"
#include "heartbeat.h"

extern uint8_t can_addr;
extern uint8_t cluster_addr;
extern tx_stack can1_tx_stack;
extern tx_stack can2_tx_stack;
extern uint8_t cluster_bits[CLUSTER_BITS_NUM];
extern uint8_t prev_cluster_bits[CLUSTER_BITS_NUM];
extern uint16_t cluster_regs[CLUST_REG_NUM];
extern uint16_t prev_cluster_regs[CLUST_REG_NUM];
extern uint16_t net_regs[NET_REG_NUM];
extern uint16_t net_regs_tx[NET_TX_REG_NUM];
extern uint16_t prev_net_regs_tx[NET_TX_REG_NUM];
extern uint8_t net_bits[NET_BIT_NUM];
extern uint8_t net_bits_tx[NET_TX_BIT_NUM];
extern uint8_t prev_net_bits_tx[NET_TX_BIT_NUM];
extern unsigned char din[DI_CNT];
extern uint8_t din_fault[DI_CNT];
extern unsigned short ain_raw[AI_CNT];
extern unsigned short ain[AI_CNT];
extern unsigned char ain_alarm[AI_CNT];

extern uint16_t ai_type;
extern uint16_t used_ai;
extern uint8_t tdu[AI_CNT];
extern uint16_t app_id;
extern uint8_t update_all;

uint16_t update_di = 0x0000;
uint16_t update_ai = 0x0000;
uint16_t update_tmr = 0;

static uint8_t net_status = 0;

void update_cluster_bits() {

	if(can_addr>7) return;
	tx_stack_data packet;
	uint8_t offset = (CLUSTER_BITS_NUM/8)*can_addr;
	for(uint8_t i=0;i<CLUSTER_BITS_NUM/8;i++) {
		if(cluster_bits[offset+i]!=prev_cluster_bits[offset+i])
		{
			uint8_t mask = 0;
			uint8_t state = 0;
			for(uint8_t j=0;j<8;j++) {
				if(i+j>=CLUSTER_BITS_NUM/8) break;
				if(cluster_bits[offset+i+j]!=prev_cluster_bits[offset+i+j]) {
					mask|=1<<j;
					if(cluster_bits[offset+i+j]) state|=1<<j;
					prev_cluster_bits[offset+i+j] = cluster_bits[offset+i+j];
				}
			}
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x03; // packed deduced digitals
			packet.data[1] = offset+16+i; // bit num
			packet.data[2] = state;
			packet.data[3] = mask;
			add_tx_can_packet(&can1_tx_stack,&packet);
			i+=7; // +1 добавит i++ цикла
		}
	}
}

void update_cluster_regs() {
	tx_stack_data packet;
	uint8_t i =0;
	while(1) {
		if(cluster_regs[i]!=prev_cluster_regs[i]) {
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x06; // global integer values
			packet.data[1] = 17+i; // reg num
			packet.data[2] = cluster_regs[i] & 0xFF; // value low byte
			packet.data[3] = cluster_regs[i] >> 8; // fault high byte
			add_tx_can_packet(&can1_tx_stack,&packet);
			prev_cluster_regs[i]=cluster_regs[i];
			i++;if(i>=CLUST_REG_NUM) i=0;
			break;
		}
		i++;
		if(i>=CLUST_REG_NUM) {i=0;break;}
	}
}

void update_net_regs() {
	tx_stack_data packet;
	static uint8_t i =0;
	if(can_addr>7 || cluster_addr>7) return;
	while(1) {
		if(net_regs_tx[i]!=prev_net_regs_tx[i]) {

			// external net
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x0f; // Network Integer Values
			packet.data[1] = (NET_TX_REG_NUM*cluster_addr) + 1 + i; // starting point
			packet.data[2] = net_regs_tx[i] & 0xFF; // value low byte
			packet.data[3] = net_regs_tx[i] >> 8; // fault high byte
			add_tx_can_packet(&can2_tx_stack,&packet);
			prev_net_regs_tx[i]=net_regs_tx[i];

			// inside cluster
			add_tx_can_packet(&can1_tx_stack,&packet);
			net_regs[cluster_addr*NET_TX_REG_NUM+i] = net_regs_tx[i];
			i++;if(i>=NET_TX_REG_NUM) i=0;
			break;
		}
		i++;
		if(i>=NET_TX_REG_NUM) {i=0;break;}
	}
}

void update_net_bits() {
	tx_stack_data packet;
	if(can_addr>7 || cluster_addr>7) return;
	for(uint8_t i=0;i<NET_TX_BIT_NUM;i++) {
		if(net_bits_tx[i]!=prev_net_bits_tx[i])
		{
			uint8_t mask = 0;
			uint8_t state = 0;
			for(uint8_t j=0;j<8;j++) {
				if(i+j>=NET_TX_BIT_NUM) break;
				if(net_bits_tx[i+j]!=prev_net_bits_tx[i+j]) {
					net_bits[cluster_addr*NET_TX_BIT_NUM+i+j] = net_bits_tx[i+j];
					mask|=1<<j;
					if(net_bits_tx[i+j]) state|=1<<j;
					prev_net_bits_tx[i+j] = net_bits_tx[i+j];
				}
			}

			// external net
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x0E; // network packed deduced digitals
			packet.data[1] = cluster_addr*16+i+1; // bit num
			packet.data[2] = state;
			packet.data[3] = mask;
			add_tx_can_packet(&can2_tx_stack,&packet);

			// inside the cluster
			add_tx_can_packet(&can1_tx_stack,&packet);
			i+=7; // +1 добавит i++ цикла
		}
	}
}

void update_di_data() {
	static uint16_t di_tmr=0;
	uint8_t di_mask = 0x00;
	uint8_t send_fault = 0x00;
	uint8_t send_state = 0x00;
	uint16_t di_fitted = 0x0000;
	uint8_t i = 0;

	tx_stack_data packet;

	di_fitted = ai_type & 0x3FFF;
	di_fitted &= ~used_ai;
	di_tmr++;
	if(di_tmr==20) {
		di_mask = update_di & di_fitted & 0xFF;
		if(di_mask)  {
			for(i=0;i<8;i++) {
				if(di_mask & (1<<i)) {
					if(din[i]) send_state |= 1<<i;
					if(din_fault[i]) send_fault |= 1<<i;
				}
			}

			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x01; // packed physical digits
			packet.data[1] = 0x01; // start bit
			packet.data[2] = send_state; // state
			packet.data[3] = send_fault; // fault
			packet.data[4] = di_mask; // mask
			add_tx_can_packet(&can1_tx_stack,&packet);
			update_di &= 0xFF00;
		}
	}else if(di_tmr==40) {
		di_mask = ((update_di & di_fitted)>>8) & 0xFF;
		if(di_mask)  {
			for(i=0;i<8;i++) {
				if(i+8>=DI_CNT) break;
				if(di_mask & (1<<i)) {
					if(din[i+8]) send_state |= 1<<i;
					if(din_fault[i+8]) send_fault |= 1<<i;
				}
			}

			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x01; // packed physical digits
			packet.data[1] = 0x09; // start bit
			packet.data[2] = send_state; // state
			packet.data[3] = send_fault; // fault
			packet.data[4] = di_mask; // mask
			add_tx_can_packet(&can1_tx_stack,&packet);
			update_di &= 0x00FF;
		}
		di_tmr=0;
	}
}

void update_ai_data() {
	static uint16_t ai_tmr=0;
	static uint8_t ai_num = 0;
	static const uint8_t ai_max_num = 14;

	tx_stack_data packet;

	ai_tmr++;
	if(ai_tmr>=50) {
		ai_tmr=0;
		for(;;) {
			if((used_ai & (1<<ai_num)) && (update_ai & (1<<ai_num))) {

				packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
				packet.length = 8;
				packet.data[0] = 0x05; // Analogue data scaled with status
				packet.data[1] = ai_num + 1; // inputs number
				packet.data[2] = ain[ai_num]; // ain value
				packet.data[3] = tdu[ai_num]; // tdu type
				if(ain_alarm[ai_num]) {
					packet.data[4] = 0x02; // alarm bits
					packet.data[5] = 0x00;
				}else {
					packet.data[4] = 0x00; // alarm bits
					packet.data[5] = 0x00;
				}
				packet.data[6] = ain_raw[ai_num] & 0xFF; // raw value
				packet.data[7] = ain_raw[ai_num] >> 8;
				add_tx_can_packet(&can1_tx_stack,&packet);
				update_ai &= ~(1<<ai_num);
				break;
			}else {
				ai_num++;
				if(ai_num>=ai_max_num) {ai_num=0;break;}
			}
		}
	}
}

void update_all_data() {
	uint16_t di_fitted = 0;
	tx_stack_data packet;
	update_tmr++;
	switch(update_tmr) {
		case 10:	// network status
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 3;
			packet.data[0] = 0x0D; // request not fragmented, eoid - 0x0D
			if(can_addr==0) {
				packet.data[1] = cluster_addr+1; // cluster number
				packet.data[2] = 0x00; // network_status
				for(uint8_t i=0;i<MAX_NODE_CNT;i++) {
					if(is_internal_node_offline(i)==0) packet.data[2]|=1<<i;
				}
			}else {
				packet.data[1] = 0x00; // not networked
				packet.data[2] = 0x00;
			}
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 20:	// module data, internal faults
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x09; // internal faults
			packet.data[2] = 0x00; // faults value
			packet.data[3] = 0x00; // faults value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 30:	// module data, analogue o/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x08; // analogue o/p fitted
			packet.data[2] = 0x00; // value
			packet.data[3] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 40:	// module data, relay o/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x07; // relay o/p fitted
			packet.data[2] = 0x3F; // value
			packet.data[3] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 50:	// module data, switch i/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x06; // switch i/p fitted
			packet.data[2] = 0x00; // value
			packet.data[3] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 60:	// module data, digital i/p fitted
			di_fitted = ai_type & 0x3FFF;
			di_fitted &= ~used_ai;
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x05; // digital i/p fitted
			packet.data[2] = di_fitted & 0xFF; // value
			packet.data[3] = di_fitted >> 8; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 70:	// module data, analogue i/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x04; // analogue i/p fitted
			packet.data[2] = 0xFF; // value
			packet.data[3] = 0x3F; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 80:	// module data, os version
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 8;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x02; // os version
			packet.data[2] = 0x30; // value
			packet.data[3] = 0x31; // value
			packet.data[4] = 0x2E; // value
			packet.data[5] = 0x34; // value
			packet.data[6] = 0x30; // value
			packet.data[7] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 90:	// module data, bootloader version
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 8;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x01; // bootloader version
			packet.data[2] = 0x30; // value
			packet.data[3] = 0x31; // value
			packet.data[4] = 0x2E; // value
			packet.data[5] = 0x30; // value
			packet.data[6] = 0x37; // value
			packet.data[7] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 100:	// module data, application cn
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x03; // application cn
			packet.data[2] = app_id & 0xFF; // value
			packet.data[3] = app_id >> 8; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 110:	// module data, module type
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 3;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x00; // module type
			packet.data[2] = 0x01; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 120:	// display tx mask digital i/p
			di_fitted = ai_type & 0x3FFF;
			di_fitted &= ~used_ai;
			packet.id = 0x0400 | 0x06 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x0A; // tx mask, eoid - 0x0A
			packet.data[1] = 0x00; // display tx mask
			packet.data[2] = di_fitted & 0xFF; // value
			packet.data[3] = di_fitted >> 8; // value
			packet.data[4] = 0x01; // io type - digital i/p
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 130:	// display tx mask analogue i/p
			packet.id = 0x0400 | 0x06 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x0A; // tx mask, eoid - 0x0A
			packet.data[1] = 0x00; // display tx mask
			packet.data[2] = used_ai & 0xFF; // value
			packet.data[3] = used_ai >> 8; // value
			packet.data[4] = 0x00; // io type - analogue i/p
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
	}
	if(update_tmr>=200) {
		update_tmr=0;update_all=0;
		update_di=0x3FFF;
		update_ai=0x3FFF;
		uint8_t i=0;
		for(i=0;i<CLUST_REG_NUM;i++) {
			if(cluster_regs[i]) prev_cluster_regs[i]=0; // provocate an update
		}
		if(can_addr<8) {
			uint8_t offset = CLUSTER_BITS_NUM/8*can_addr;
			for(i=0;i<CLUSTER_BITS_NUM/8;i++) {
				if(cluster_bits[offset+i]) prev_cluster_bits[offset+i] = 0; // provocate an update
			}
		}
	}
}

void send_changed_data() {
	static uint16_t inp_tmr=0;
	static uint16_t clust_tmr = 0;

	clust_tmr++;
	if(clust_tmr>=15) {
		clust_tmr=0;
		update_cluster_regs();
		update_cluster_bits();
		if(can_addr==0) {
			update_net_regs();
			update_net_bits();
		}
	}

	update_ai_data();
	update_di_data();
	inp_tmr++;
	if(inp_tmr==30000) {
		update_di=0x3FFF;
	}
	else if(inp_tmr>=60000) {
		inp_tmr = 0;
		update_ai=0x3FFF;
	}
}

void update_net_status() {
	if(can_addr==0) {
		uint8_t state = 0;
		for(uint8_t i=0;i<MAX_NET_CNT;i++) {
			if(is_external_node_offline(i)==0) state|=1<<i;
		}
		if(state!=net_status) {
			tx_stack_data packet;
			packet.id = 0x0400 | 0x07 | (cluster_addr << 7);	// event
			packet.length = 3;
			packet.data[0] = 0x0D; // request not fragmented, eoid - 0x0D
			packet.data[1] = cluster_addr+1; // cluster number
			packet.data[2] = state; // network_status
			add_tx_can_packet(&can1_tx_stack,&packet);
			net_status = state;
		}
	}
}
