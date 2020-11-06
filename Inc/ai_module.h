/*
 * aidi_module.h
 *
 *  Created on: 30 ���. 2020 �.
 *      Author: User
 */

#ifndef AI_MODULE_H_
#define AI_MODULE_H_

#include <stdint.h>
#include "ai_calculate.h"

#define MOD_AI_INP_CNT	8	// ����� ������ ������

struct ai_mod{
	uint8_t	addr;	// ����� ������
	uint8_t sens_type[MOD_AI_INP_CNT];	// ��� ������� (�����, 4-20 ��, 0-2.5� � �.�.)
	uint16_t raw_value[MOD_AI_INP_CNT];	// �������������� ��������
	uint8_t sensor_value[MOD_AI_INP_CNT]; // ������������� � 0-255
	uint8_t ai_over[MOD_AI_INP_CNT];	// ���� ������
	uint8_t ai_under[MOD_AI_INP_CNT];	// ���� ������
	uint8_t ai_alarm[MOD_AI_INP_CNT];	// ���� ��� ���� ������� ������
	uint8_t di[MOD_AI_INP_CNT];			// ���������� ����
	uint8_t di_sc[MOD_AI_INP_CNT];		// short circuit (�������� ���������)
	uint8_t di_br[MOD_AI_INP_CNT];		// break (�����)
	uint8_t di_fault[MOD_AI_INP_CNT];	// ������ (�� ��� �����)
	uint8_t link_state;					// �� ����� ��� ���
	uint8_t heartbeat_cnt;				// ����������. ������� (������� ��� ����������� heartbeat)
};

//typedef struct ai_mod *ai_mod_ptr;

void ai_mod_init_values(struct ai_mod *mod);
uint8_t get_input_types(struct ai_mod *mod);

#endif /* AI_MODULE_H_ */
