/*
 * can_tx_stack.h
 *
 *  Created on: 30 ???. 2019 ?.
 *      Author: User
 */

#ifndef CAN_TX_STACK_H_
#define CAN_TX_STACK_H_

#include <stdint.h>

#define CAN_TX_DATA_SIZE		8
#define CAN_TX_STACK_LENGTH		80

#define EMPTY_PACKET	0
#define BUSY_PACKET		1
#define READY_PACKET	2


typedef struct  __attribute__((packed, aligned(4))){
	uint8_t data[CAN_TX_DATA_SIZE];
	uint32_t id;
	uint8_t length;
	uint8_t state;
} tx_stack_data;

typedef struct  __attribute__((packed, aligned(4))){
	tx_stack_data packet[CAN_TX_STACK_LENGTH];
	uint16_t read_position;
	uint16_t write_position;
} tx_stack;

void init_can_tx_stack(tx_stack *stack);
void add_tx_can_packet(tx_stack *stack,tx_stack_data *packet);
uint8_t get_tx_can_packet(tx_stack *stack, tx_stack_data *packet);

#endif /* CAN_TX_STACK_H_ */
