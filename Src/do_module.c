/*
 * do_module.c
 *
 *  Created on: 13 но€б. 2020 г.
 *      Author: User
 */

#include "do_module.h"

extern uint16_t do_mod_cnt;
extern struct do_mod* do_modules_ptr;

void do_mod_init_values(struct do_mod *mod) {
	for(uint8_t i=0;i<MOD_DO_OUT_CNT;++i) {
		mod->do_state[i] = 0;
		mod->prev_do_state[i] = 0;
		mod->do_err[i] = 0;
	}
	mod->update_data = 0;
	mod->link_state = 0;
	mod->heartbeat_cnt = 0;
}

void do_mod_cycle() {
	for(uint16_t i=0;i<do_mod_cnt;++i) {
		for(uint8_t j=0;j<MOD_DO_OUT_CNT;++j) {
			if(do_modules_ptr[i].prev_do_state[j] != do_modules_ptr[i].do_state[j]) {
				do_modules_ptr[i].prev_do_state[j] = do_modules_ptr[i].do_state[j];
				do_modules_ptr[i].update_data = 1;
			}
		}
	}
}
