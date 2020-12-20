/*
 * can_protocol.c
 *
 *  Created on: 3 но€б. 2020 г.
 *      Author: User
 */

#include "can_protocol.h"
#include "main.h"
#include <string.h>

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

extern tx_stack can1_tx_stack;
extern tx_stack can2_tx_stack;

extern const char* di_names[14];
extern const char* do_names[6];
extern const char* adc_names[14];

extern uint8_t can_addr;
extern uint8_t cluster_addr;

uint8_t is_packet_extended(uint32_t can_id) {
	if((can_id & 0x07) == SRV_Channel) return 1;
	return 0;
}

enum SS_type get_ss(struct can_packet *packet) {
	return packet->data[0] >> 5;
}

uint8_t get_eoid(struct can_packet *packet) {
	return packet->data[0] & 0b00011111;
}

uint8_t get_intern_addr(struct can_packet *packet) {
	return packet->data[1];
}

void sendIOName(uint8_t ioNum, uint8_t type, uint8_t req_node, uint8_t req_num) {
	uint8_t name[20]={0};
	uint8_t lngth = 0;
	uint8_t i = 0;
	switch(type) {
		case 0:	// analogue input
			if(ioNum>=1 && ioNum<=14) {
				lngth = strlen(adc_names[ioNum-1]);
				if(lngth>20) lngth = 20;
				for(i=0;i<lngth;i++) name[i] = adc_names[ioNum-1][i];
			}
			break;
		case 1: // digital input
			if(ioNum>=1 && ioNum<=14) {
				lngth = strlen(di_names[ioNum-1]);
				if(lngth>20) lngth = 20;
				for(i=0;i<lngth;i++) name[i] = di_names[ioNum-1][i];
			}
			break;
		case 3: // relay output
			if(ioNum>=1 && ioNum<=6) {
				lngth = strlen(do_names[ioNum-1]);
				if(lngth>20) lngth = 20;
				for(i=0;i<lngth;i++) name[i] = do_names[ioNum-1][i];
			}
			break;
	}

	tx_stack_data packet;
	packet.id = (req_node<<3) | 0x05 | (can_addr<<3) | (cluster_addr << 7);
	packet.length = 8;
	packet.data[0] = (0x03<<5) | 0x0b; // ss-0x05 (fragmented response) eoid-0x0b(read name)
	packet.data[1] = req_num;
	packet.data[2] = 1; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i];
	add_tx_can_packet(&can1_tx_stack,&packet);

	packet.data[1] = req_num;
	packet.data[2] = 2; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i+5];
	add_tx_can_packet(&can1_tx_stack,&packet);

	packet.data[1] = req_num;
	packet.data[2] = 3; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i+10];
	add_tx_can_packet(&can1_tx_stack,&packet);

	packet.data[1] = req_num;
	packet.data[2] = 4; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i+15];
	add_tx_can_packet(&can1_tx_stack,&packet);
}

void sendResponse(tx_stack *stack, uint8_t signature, enum ExtCmd cmd) {
	tx_stack_data tx_packet;
	tx_packet.id = 0;
	struct can_packet_ext_id *tx_can_id = (struct can_packet_ext_id *)&tx_packet.id;
	tx_can_id->mod_type = Ext_PC21;
	tx_can_id->req = NoAnswer;
	tx_can_id->dir = BroadcastReq;
	tx_can_id->srv = SRV_Channel;
	tx_packet.data[0] = can_addr;
	tx_packet.data[1] = ExtResponse;
	tx_packet.data[2] = signature;
	tx_packet.data[3] = cmd;
	tx_packet.length = 4;
	add_tx_can_packet(stack,&tx_packet);
}

void sendByteWrite(tx_stack *stack, uint8_t dst_addr, uint8_t byte_addr, uint8_t byte_value, uint8_t dev_type) {
	tx_stack_data tx_packet;
	tx_packet.id = 0;
	struct can_packet_ext_id *tx_can_id = (struct can_packet_ext_id *)&tx_packet.id;
	tx_can_id->mod_type = Ext_PC21;
	tx_can_id->req = NoAnswer;
	tx_can_id->dir = ToNodeReq;
	tx_can_id->srv = SRV_Channel;	// extended protocol
	tx_packet.data[0] = dst_addr;
	tx_packet.data[1] = ExtByteWrite;
	tx_packet.data[2] = byte_addr;
	tx_packet.data[3] = byte_value;
	tx_packet.data[4] = can_addr;
	tx_packet.data[5] = dev_type;
	tx_packet.length = 6;
	add_tx_can_packet(stack,&tx_packet);
}

void sendOutState(tx_stack *stack, uint8_t dst_addr, uint8_t outState) {
	tx_stack_data tx_packet;
	tx_packet.id = 0;
	struct can_packet_ext_id *tx_can_id = (struct can_packet_ext_id *)&tx_packet.id;
	tx_can_id->mod_type = Ext_PC21;
	tx_can_id->req = NoAnswer;
	tx_can_id->dir = ToNodeReq;
	tx_can_id->srv = SRV_Channel;	// extended protocol
	tx_packet.data[0] = dst_addr;
	tx_packet.data[1] = ExtDOWrite;
	tx_packet.data[2] = 0;	// out num
	tx_packet.data[3] = outState;
	tx_packet.data[4] = can_addr;
	tx_packet.length = 5;
	add_tx_can_packet(stack,&tx_packet);
}

void sendReqDataFromMod(tx_stack *stack, uint8_t dst_addr) {
	tx_stack_data tx_packet;
	tx_packet.id = 0;
	struct can_packet_ext_id *tx_can_id = (struct can_packet_ext_id *)&tx_packet.id;
	tx_can_id->mod_type = Ext_PC21;
	tx_can_id->req = NoAnswer;
	tx_can_id->dir = ToNodeReq;
	tx_can_id->srv = SRV_Channel;	// extended protocol
	tx_packet.data[0] = dst_addr;
	tx_packet.data[1] = ExtGetInfo;
	tx_packet.length = 2;
	add_tx_can_packet(stack,&tx_packet);
}
