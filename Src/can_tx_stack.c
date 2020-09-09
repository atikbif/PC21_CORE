#include "can_tx_stack.h"
#include <cmsis_gcc.h>

void init_can_tx_stack(tx_stack *stack) {
	uint8_t i = 0;
	stack->read_position = 0;
	stack->write_position = 0;
	for(i=0;i<CAN_TX_STACK_LENGTH;i++) {
		stack->packet[i].length = 0;
		stack->packet[i].state = EMPTY_PACKET;
	}
}

void add_tx_can_packet(tx_stack *stack,tx_stack_data *packet) {
	uint8_t i = 0;
	uint16_t wr_pos = stack->write_position;
	if(wr_pos>=CAN_TX_STACK_LENGTH) {
		return;
	}
	if(packet->length > CAN_TX_DATA_SIZE) return;
	stack->packet[wr_pos].state = BUSY_PACKET;
	for(i=0;i<packet->length;++i) stack->packet[wr_pos].data[i] = packet->data[i];
	stack->packet[wr_pos].id = packet->id;
	stack->packet[wr_pos].length = packet->length;
	stack->packet[wr_pos].state = READY_PACKET;
	wr_pos++;
	if(wr_pos>=CAN_TX_STACK_LENGTH) wr_pos=0;
	stack->write_position = wr_pos;
}

uint8_t get_tx_can_packet(tx_stack *stack, tx_stack_data *packet) {
	uint8_t i = 0;
	uint16_t rd_pos = stack->read_position;
	if(rd_pos>=CAN_TX_STACK_LENGTH) {
		return 0;
	}
	if(stack->packet[rd_pos].state==READY_PACKET) {
		packet->id = stack->packet[rd_pos].id;
		packet->length = stack->packet[rd_pos].length;
		if(packet->length > CAN_TX_DATA_SIZE) {
			stack->packet[rd_pos].state = EMPTY_PACKET;
			rd_pos++;
			if(rd_pos>=CAN_TX_STACK_LENGTH) rd_pos = 0;
			stack->read_position=rd_pos;
			return 0;
		}
		for(i=0;i<packet->length;++i) packet->data[i] = stack->packet[rd_pos].data[i];
		stack->packet[rd_pos].state = EMPTY_PACKET;
		rd_pos++;
		if(rd_pos>=CAN_TX_STACK_LENGTH) rd_pos = 0;
		stack->read_position=rd_pos;
		return 1;
	}
	return 0;
}
