/*
 * heartbeat.h
 *
 *  Created on: 2 но€б. 2020 г.
 *      Author: User
 */

#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

#include <stdint.h>

#define MAX_NODE_CNT	8
#define MAX_NET_CNT		8
#define HEARTBEAT_MAX	3

void send_heartbeat();
void increment_modules_heartbeats_counters();
void increment_others_heartbeats_counters();
uint8_t is_internal_node_offline(uint8_t node_num);
void clear_internal_heartbeat(uint8_t node_num);
uint8_t is_external_node_offline(uint8_t node_num);
void clear_external_heartbeat(uint8_t node_num);

#endif /* HEARTBEAT_H_ */
