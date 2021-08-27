/*
 * can_protocol.h
 *
 *  Created on: 3 но€б. 2020 г.
 *      Author: User
 */

#ifndef CAN_PROTOCOL_H_
#define CAN_PROTOCOL_H_

#include <stdint.h>
#include "can_tx_stack.h"

enum Service_type {SRV_Channel, SRV_Heartbeat, SRV_Download, SRV_Spare, SRV_Write, SRV_Read, SRV_Action, SRV_Event};
enum SS_type {SS_Req_not_fragm, SS_Req_fragm, SS_Success_resp_not_fragm, SS_Success_resp_fragm, SS_Err_resp_not_fragm, SS_Err_resp_fragm, SS_Wait_resp_not_fragm, SS_Wait_resp_fragm};

enum ExtCmd {ExtHeartbeat, ExtResponse, ExtGetInfo, ExtDOState, ExtDOWrite, ExtAIState, ExtByteWrite,
			 ExtWriteConf, ExtRegsStateWithValidation, ExtRegsValidation};
enum ExtDir {BroadcastReq, ToNodeReq};
enum ExtFB {NoAnswer, RequireAnswer};
enum ExtModType {Ext_PC21, Ext_AI_Mod, Ext_DO_Mod, Ext_RS_Mod};

struct can_packet_id {
	uint16_t srv: 3;
	uint16_t node_addr: 4;
	uint16_t clust_addr: 3;
	uint16_t dir: 1;
};

struct can_packet_ext_id {
	uint16_t srv: 3;		// always SRV_Channel
	uint16_t mod_type: 6;	// module type (sender)
	uint16_t dir: 1;		// direction (1 - to address, 0 - from address)
	uint16_t req: 1;		// requires an answer
};

#define CAN_DATA_SIZE	8
#define CAN_HEADER_LENGTH	2		// SS EOID INTERN_ADDR
#define CAN_EXT_HEADER_LENGTH	2		// addr, packet signature, num of subpacket (4 bits) and quantity of packets (4 bits), cmd

struct can_packet {
	uint16_t id;
	uint8_t length;
	uint8_t data[CAN_DATA_SIZE];
};

void send_time(uint8_t *cur_time);
uint8_t is_packet_extended(uint32_t can_id);
enum SS_type get_ss(struct can_packet *packet);
uint8_t get_eoid(struct can_packet *packet);
uint8_t get_intern_addr(struct can_packet *packet);
void sendIOName(uint8_t ioNum, uint8_t type, uint8_t req_node, uint8_t req_num);
void sendResponse(tx_stack *stack, uint8_t signature, enum ExtCmd cmd);
void sendByteWrite(tx_stack *stack, uint8_t dst_addr, uint8_t byte_addr, uint8_t byte_value, uint8_t dev_type);
void sendOutState(tx_stack *stack, uint8_t dst_addr, uint8_t outState);
void sendReqDataFromMod(tx_stack *stack, uint8_t dst_addr);

#endif /* CAN_PROTOCOL_H_ */
