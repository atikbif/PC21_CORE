/*
 * modules.c
 *
 *  Created on: 30 окт. 2020 г.
 *      Author: User
 */

#include "ai_module.h"
#include "do_module.h"
#include "ai_calculate.h"
#include "modules.h"

#define AI_MOD_CNT	2	// число аналоговых модулей

uint16_t ai_mod_cnt = AI_MOD_CNT;
const uint8_t ai_mod_addr[AI_MOD_CNT] = {1,2};
const uint8_t ai_sens_type[AI_MOD_CNT][MOD_AI_INP_CNT] = {{SENSDI,SENSDI,SENSDI,SENSDI,SENSDI,SENSDI,SENSDI,SENSDI},
																 {SENSDI,SENSDI,SENSDI,SENSDI,SENSDI,SENSDI,SENSDI,SENSDI}
};

struct ai_mod ai_modules[AI_MOD_CNT];
struct ai_mod* ai_modules_ptr;

#define DO_MOD_CNT	2

uint16_t do_mod_cnt = DO_MOD_CNT;
const uint8_t do_mod_addr[DO_MOD_CNT] = {3,4};

struct do_mod do_modules[DO_MOD_CNT];
struct do_mod* do_modules_ptr;

void init_modules() {
	ai_modules_ptr = ai_modules;
	for(uint8_t i=0;i<AI_MOD_CNT;i++) {
		ai_mod_init_values(&ai_modules[i]);
		ai_modules[i].addr = ai_mod_addr[i];
		for(uint8_t j=0;j<MOD_AI_INP_CNT;j++) ai_modules[i].sens_type[j] = ai_sens_type[i][j];
	}

	do_modules_ptr = do_modules;
	for(uint8_t i=0;i<DO_MOD_CNT;i++) {
		do_mod_init_values(&do_modules[i]);
		do_modules[i].addr = do_mod_addr[i];
	}
}
