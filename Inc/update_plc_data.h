/*
 * can_cmd.h
 *
 *  Created on: 2 ����. 2020 �.
 *      Author: User
 */

#ifndef UPDATE_PLC_DATA_H_
#define UPDATE_PLC_DATA_H_

#include <stdint.h>

void update_cluster_bits();
void update_cluster_regs();
void update_net_regs();
void update_net_bits();
void update_di_data();
void update_ai_data();
void update_all_data();
void send_changed_data();
void update_net_status();

#endif /* UPDATE_PLC_DATA_H_ */
