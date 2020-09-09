/*
 * can_task.h
 *
 *  Created on: 22 мар. 2020 г.
 *      Author: User
 */

#ifndef CAN_TASK_H_
#define CAN_TASK_H_

#include <stdint.h>

#define MAX_NODE_CNT	8
#define MAX_NET_CNT		8
#define HEARTBEAT_MAX	3

void canTask(void const * argument);
void sendIOName(uint8_t ioNum, uint8_t type, uint8_t req_node, uint8_t req_num);
void init_can_addr_pins();
uint8_t read_can_addr();

#endif /* CAN_TASK_H_ */
