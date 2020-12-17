/*
 * rs_module.h
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: User
 */

#ifndef RS_MODULE_H_
#define RS_MODULE_H_

#include <stdint.h>
#include "can_tx_stack.h"

#define RS_REG_CNT		80
#define CONFIG_LENGTH	24
#define RS_DEVICE_CNT	10
#define NUM_OF_BYTES_IN_WR_BUF_PACKET	4

enum rs_config_state {RS_WAIT_CMD, RS_START_WR_CONFIG, RS_WR_CONFIG, RS_WAIT_RESPONSE};
enum rs_dev_state {RS_NO_CAN_LINK, RS_NO_485_LINK, RS_WRONG_TYPE, RS_CORRECT_DATA};

struct rs_config {
	const uint8_t* 	buf;
	uint8_t sent_id_value;
	uint8_t rcv_id_value;
	uint8_t rcv_id_flag;
	enum rs_config_state write_state;
	uint16_t tmr;
	uint8_t last_packet_num;
};

struct rs_mod {
	struct rs_config config;
	uint16_t* data;
	uint8_t* dev_state;
	uint8_t	addr;
	uint8_t link_state;
	uint8_t heartbeat_cnt;
};

void rs_mod_init_values(struct rs_mod *mod);
void rs_module_write_config(struct rs_mod *mod, tx_stack *stack);

#endif /* RS_MODULE_H_ */
