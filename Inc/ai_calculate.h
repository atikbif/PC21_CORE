/*
 * ai_calculate.h
 *
 *  Created on: 2 но€б. 2020 г.
 *      Author: User
 */

#ifndef AI_CALCULATE_H_
#define AI_CALCULATE_H_

#include <stdint.h>
#include "ai_module.h"

enum sens_type {SENSDI, SENS0p4_2V, SENS0_20mA, SENS4_20mA, SENS2_10mA, SENS0_2p5V};

typedef struct ai_mod *ai_mod_ptr;

void handle_adc(uint16_t raw_value, enum sens_type sensor, ai_mod_ptr mod, uint8_t ai_num);

#endif /* AI_CALCULATE_H_ */
