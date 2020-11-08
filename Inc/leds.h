/*
 * leds.h
 *
 *  Created on: 8 ����. 2020 �.
 *      Author: User
 */

#ifndef LEDS_H_
#define LEDS_H_

#include <stdint.h>
#include "main.h"

#define RELE_LED_QUANTITY	6

enum led_mode {LED_FLASH_BY_CMD, LED_TOGGLE, LED_ON_OFF};

struct led_state{
	enum led_mode mode;	// ����� ������ (������� �� �������, �������, ��������� �����)
	uint8_t out_state;	// ��������� ������
	uint8_t on_cmd;		// ������� ��� ������ �������
	uint8_t off_cmd;	// ���������� ������ �����
	uint16_t on_time;	// ����� �� ��� ���������
	uint16_t off_time;	// ����� � ���� ���������
	uint16_t tmr;		// �����. ������
	uint8_t inverse;	// ��������� �����
};

void led_cycle(uint8_t add_value);
void init_leds();

#endif /* LEDS_H_ */
