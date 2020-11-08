/*
 * leds.h
 *
 *  Created on: 8 нояб. 2020 г.
 *      Author: User
 */

#ifndef LEDS_H_
#define LEDS_H_

#include <stdint.h>
#include "main.h"

#define RELE_LED_QUANTITY	6

enum led_mode {LED_FLASH_BY_CMD, LED_TOGGLE, LED_ON_OFF};

struct led_state{
	enum led_mode mode;	// режим работы (вспышка по команде, мигание, статичный режим)
	uint8_t out_state;	// состояние выхода
	uint8_t on_cmd;		// команда для режима вспышки
	uint8_t off_cmd;	// запрещение работы диода
	uint16_t on_time;	// время во вкл состоянии
	uint16_t off_time;	// время в выкл состоянии
	uint16_t tmr;		// внутр. таймер
	uint8_t inverse;	// инверсный режим
};

void led_cycle(uint8_t add_value);
void init_leds();

#endif /* LEDS_H_ */
