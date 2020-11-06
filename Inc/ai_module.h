/*
 * aidi_module.h
 *
 *  Created on: 30 окт. 2020 г.
 *      Author: User
 */

#ifndef AI_MODULE_H_
#define AI_MODULE_H_

#include <stdint.h>
#include "ai_calculate.h"

#define MOD_AI_INP_CNT	8	// число входов модуля

struct ai_mod{
	uint8_t	addr;	// адрес модуля
	uint8_t sens_type[MOD_AI_INP_CNT];	// тип датчика (дискр, 4-20 мА, 0-2.5В и т.д.)
	uint16_t raw_value[MOD_AI_INP_CNT];	// необработанное значение
	uint8_t sensor_value[MOD_AI_INP_CNT]; // пересчитанное в 0-255
	uint8_t ai_over[MOD_AI_INP_CNT];	// выше порога
	uint8_t ai_under[MOD_AI_INP_CNT];	// ниже порога
	uint8_t ai_alarm[MOD_AI_INP_CNT];	// выше или ниже второго порога
	uint8_t di[MOD_AI_INP_CNT];			// дискретный вход
	uint8_t di_sc[MOD_AI_INP_CNT];		// short circuit (короткое замыкание)
	uint8_t di_br[MOD_AI_INP_CNT];		// break (обрыв)
	uint8_t di_fault[MOD_AI_INP_CNT];	// ошибка (кз или обрыв)
	uint8_t link_state;					// на связи или нет
	uint8_t heartbeat_cnt;				// автоинкрем. счётчик (сбрасыв при обнаружении heartbeat)
};

//typedef struct ai_mod *ai_mod_ptr;

void ai_mod_init_values(struct ai_mod *mod);
uint8_t get_input_types(struct ai_mod *mod);

#endif /* AI_MODULE_H_ */
