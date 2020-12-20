/*
 * rs_module.c
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: User
 */

#include "rs_module.h"
#include "heartbeat.h"
#include "can_protocol.h"

extern uint8_t can_addr;

void rs_mod_init_values(struct rs_mod *mod) {
	mod->heartbeat_cnt = HEARTBEAT_MAX;
	mod->link_state = 0;
	for(uint8_t i=0;i<RS_REG_CNT;i++) mod->data[i] = 0;
	mod->config.sent_id_value = 0;
	mod->config.rcv_id_value = 0;
	mod->config.rcv_id_flag = 0;
	mod->config.tmr = 0;
	mod->config.write_state = RS_WAIT_CMD;
	mod->config.last_packet_num = 0;
}

void rs_module_write_config(struct rs_mod *mod, tx_stack *stack) {
	if(mod->link_state==0) return;
	tx_stack_data tx_packet;
	switch(mod->config.write_state) {
		case RS_WAIT_CMD:
			break;
		case RS_START_WR_CONFIG:
			mod->config.tmr = 0;
			mod->config.last_packet_num = 0;
			mod->config.sent_id_value = 0;
			mod->config.rcv_id_value = 0;
			mod->config.rcv_id_flag = 0;
			mod->config.write_state = RS_WR_CONFIG;
			break;
		case RS_WR_CONFIG:
			if(mod->config.last_packet_num == 0) {	// send header
				tx_packet.id = 0;
				struct can_packet_ext_id *tx_can_id = (struct can_packet_ext_id *)&tx_packet.id;
				tx_can_id->mod_type = Ext_PC21;
				tx_can_id->req = RequireAnswer;
				tx_can_id->dir = ToNodeReq;
				tx_can_id->srv = SRV_Channel;
				tx_packet.data[0] = mod->addr;
				tx_packet.data[1] = ExtWriteConf;
				tx_packet.data[2] = can_addr;
				tx_packet.data[3] = 0;	// packet num (0 - header)
				uint8_t pack_quantity = CONFIG_LENGTH/NUM_OF_BYTES_IN_WR_BUF_PACKET; // число пакетов для передачи буфера
				if(CONFIG_LENGTH%NUM_OF_BYTES_IN_WR_BUF_PACKET) pack_quantity++;
				tx_packet.data[4] = pack_quantity;
				tx_packet.data[5] = CONFIG_LENGTH >> 8;
				tx_packet.data[6] = CONFIG_LENGTH &0xFF;
				tx_packet.data[7] = Ext_RS_Mod;	// type of device
				tx_packet.length = 8;
				add_tx_can_packet(stack,&tx_packet);
				mod->config.sent_id_value = can_addr;
				mod->config.rcv_id_value = 0;
				mod->config.rcv_id_flag = 0;
			}else {	// send packet
				tx_packet.id = 0;
				struct can_packet_ext_id *tx_can_id = (struct can_packet_ext_id *)&tx_packet.id;
				tx_can_id->mod_type = Ext_PC21;
				tx_can_id->req = RequireAnswer;
				tx_can_id->dir = ToNodeReq;
				tx_can_id->srv = SRV_Channel;
				tx_packet.data[0] = mod->addr;
				tx_packet.data[1] = ExtWriteConf;
				tx_packet.data[2] = can_addr;
				tx_packet.data[3] = mod->config.last_packet_num;
				uint8_t offset = NUM_OF_BYTES_IN_WR_BUF_PACKET*(mod->config.last_packet_num-1);
				for(uint8_t i=0;i<NUM_OF_BYTES_IN_WR_BUF_PACKET;i++) {
					if(offset+i<CONFIG_LENGTH) tx_packet.data[4+i] = mod->config.buf[offset+i];
					else tx_packet.data[4+i] = 0;
				}
				tx_packet.length = 8;
				add_tx_can_packet(stack,&tx_packet);
				mod->config.sent_id_value = can_addr;
				mod->config.rcv_id_value = 0;
				mod->config.rcv_id_flag = 0;
			}
			mod->config.tmr = 0;
			mod->config.write_state = RS_WAIT_RESPONSE;
			break;
		case RS_WAIT_RESPONSE:
			if(mod->config.rcv_id_flag) {
				mod->config.rcv_id_flag = 0;
				if(mod->config.rcv_id_value == mod->config.sent_id_value) {
					uint8_t pack_quantity = CONFIG_LENGTH/NUM_OF_BYTES_IN_WR_BUF_PACKET; // число пакетов для передачи буфера
					if(CONFIG_LENGTH%NUM_OF_BYTES_IN_WR_BUF_PACKET) pack_quantity++;
					if(pack_quantity==mod->config.last_packet_num) {	// last packet
						mod->config.write_state = RS_WAIT_CMD;
						mod->config.last_packet_num = 0;
						break;
					}else {
						mod->config.last_packet_num++;
						mod->config.write_state = RS_WR_CONFIG;
					}
				}
			}
			mod->config.tmr++;
			if(mod->config.tmr>=100) mod->config.write_state = RS_WR_CONFIG; // повторная отправка
			break;
	}
}
