/*
 * can_task.h
 *
 *  Created on: 22 мар. 2020 г.
 *      Author: User
 */

#ifndef CAN_TASK_H_
#define CAN_TASK_H_

#include <stdint.h>

#define CAN_TX_TMR_GAP	5

void canTask(void const * argument);
void init_can_addr_pins();
uint8_t read_can_addr();

#endif /* CAN_TASK_H_ */
