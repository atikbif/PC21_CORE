#include "ai_module.h"
#include "do_module.h"
#include "rs_module.h"
#include "ai_calculate.h"
#include "modules.h"

#define AI_MOD_CNT		0

uint16_t ai_mod_cnt = AI_MOD_CNT;
struct ai_mod* ai_modules_ptr;

#define DO_MOD_CNT		1

uint16_t do_mod_cnt = DO_MOD_CNT;
const uint8_t do_mod_addr[DO_MOD_CNT] = {3};
struct do_mod do_modules[DO_MOD_CNT];
struct do_mod* do_modules_ptr;

#define RS_MOD_CNT		2

uint16_t rs_mod_cnt = RS_MOD_CNT;
const uint8_t rs_mod_addr[RS_MOD_CNT] = {1,2};
const uint8_t rs_conf[RS_MOD_CNT][CONFIG_LENGTH] = {
	{0,0,10,1,17,2,5,3,13,4,2,5,0|0x80,7,1|0x80,8,3|0x80,9,10|0x80,10,14|0x80,11,15|0x80,0},
	{0,0,2,1,17|0x80,2,17|0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

uint16_t rs_data[RS_MOD_CNT][RS_REG_CNT];
uint8_t rs_dev_state[RS_MOD_CNT][RS_DEVICE_CNT];
struct rs_mod rs_modules[RS_MOD_CNT];
struct rs_mod* rs_modules_ptr;

void init_modules() {
	ai_modules_ptr = 0;
	do_modules_ptr = do_modules;
	for(uint8_t i=0;i<DO_MOD_CNT;i++) {
		do_mod_init_values(&do_modules[i]);
		do_modules[i].addr = do_mod_addr[i];
	}
	rs_modules_ptr = rs_modules;
	for(uint8_t i=0;i<RS_MOD_CNT;i++) {
		rs_modules[i].addr = rs_mod_addr[i];
		rs_modules[i].data = &rs_data[i][0];
		rs_modules[i].dev_state = &rs_dev_state[i][0];
		rs_modules[i].config.buf = &rs_conf[i][0];
		rs_mod_init_values(&rs_modules[i]);
	}
}
