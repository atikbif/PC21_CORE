/*
 * do_module.c
 *
 *  Created on: 13 нояб. 2020 г.
 *      Author: User
 */

#include "do_module.h"

extern uint16_t do_mod_cnt;
extern struct do_mod* do_modules_ptr;

static uint16_t upd_next_do_lim = 0xFFFF;
static uint16_t upd_do_mod_num = 0;
static uint16_t upd_do_tmr = 0;


void do_mod_init_values(struct do_mod *mod) {
	for(uint8_t i=0;i<MOD_DO_OUT_CNT;++i) {
		mod->do_state[i] = 0;
		mod->prev_do_state[i] = 0;
		mod->do_err[i] = 0;
	}
	mod->update_data = 0;
	mod->link_state = 0;
	mod->heartbeat_cnt = 0;

	if(do_mod_cnt) upd_next_do_lim = 1000/do_mod_cnt;
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

	// в течение одной секунды с равными промежутками постоянно передаются данные
	// всем модулям чтобы они контролировал связь с контроллером (на случай когда данные не меняются)
	if(do_mod_cnt) {
		upd_do_tmr++;
		if(upd_do_tmr>=upd_next_do_lim) {
			upd_do_tmr = 0;
			do_modules_ptr[upd_do_mod_num].update_data = 1;
			upd_do_mod_num++;
			if(upd_do_mod_num>=do_mod_cnt) upd_do_mod_num=0;
		}
	}
}
