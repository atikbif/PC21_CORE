/*
 * do_module.h
 *
 *  Created on: 13 ����. 2020 �.
 *      Author: User
 */

#ifndef DO_MODULE_H_
#define DO_MODULE_H_

#include <stdint.h>

#define MOD_DO_OUT_CNT	4	// ����� ������� ������

struct do_mod{
	uint8_t	addr;	// ����� ������
	uint8_t do_err[MOD_DO_OUT_CNT];
	uint8_t do_state[MOD_DO_OUT_CNT];
	uint8_t prev_do_state[MOD_DO_OUT_CNT];
	uint8_t update_data;
	uint8_t link_state;					// �� ����� ��� ���
	uint8_t heartbeat_cnt;				// ����������. ������� (������� ��� ����������� heartbeat)
};

void do_mod_init_values(struct do_mod *mod);
void do_mod_cycle();

#endif /* DO_MODULE_H_ */
