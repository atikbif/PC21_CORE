/*
 * aidi_module.c
 *
 *  Created on: 30 окт. 2020 г.
 *      Author: User
 */

#include <ai_module.h>
#include "modules.h"
#include "heartbeat.h"

void ai_mod_init_values(struct ai_mod *mod) {
	for(uint8_t i=0;i<MOD_AI_INP_CNT;++i) {
		mod->raw_value[i] = 0;
		mod->sensor_value[i] = 0;
		mod->ai_alarm[i] = 0;
		mod->ai_over[i] = 0;
		mod->ai_under[i] = 0;
		mod->di[i] = 0;
		mod->di_br[i] = 0;
		mod->di_sc[i] = 0;
		mod->di_fault[i] = 0;
		mod->sens_type[i] = SENSDI;
	}
	mod->heartbeat_cnt = HEARTBEAT_MAX;
	mod->link_state = 0;
}

uint8_t get_input_types(struct ai_mod *mod) {
	uint8_t res = 0;
	for(uint8_t i=0;i<MOD_AI_INP_CNT;++i) {
		if(mod->sens_type[i]==SENSDI || mod->sens_type[i]==SENS0_20mA || mod->sens_type[i]==SENS2_10mA || mod->sens_type[i]==SENS4_20mA) res |= 1<<i;
	}
	return res;
}


