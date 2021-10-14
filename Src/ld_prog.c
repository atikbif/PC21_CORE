#include "ld_prog.h"
#include "elements.h"
#include "iodef.h"
#include "os_conf.h"
#include <stdint.h>
#include "modbus_master.h"
#include "heartbeat.h"
#include "moddef.h"
#include "modules.h"
#include "ai_module.h"
#include "do_module.h"
#include "rs_module.h"
volatile unsigned short plc_cycle = 10;

unsigned short ain[AI_CNT];
unsigned short ain_raw[AI_CNT];
unsigned char ain_under[AI_CNT];
unsigned char ain_over[AI_CNT];
unsigned char ain_alarm[AI_CNT];
unsigned char din[DI_CNT];
unsigned char dinr[DI_CNT];
unsigned char dinf[DI_CNT];
unsigned char dout[DO_CNT];
extern unsigned char din_break[4];
extern unsigned char din_short_circuit[4];
extern unsigned char din_fault[4];
unsigned char ibit[IBIT_CNT];
unsigned short tmrms[TMRMS_CNT];
unsigned short tmrs[TMRS_CNT];
unsigned short ireg[IREG_CNT];
unsigned short frsh[FRSH_CNT];
double frd[FRD_CNT];
unsigned long frl[FRL_CNT];
extern unsigned short work_time;

uint16_t cluster_regs[64]={0};
uint16_t prev_cluster_regs[64]={0};
uint8_t cluster_bits[224]={0};
uint8_t prev_cluster_bits[224]={0};
uint8_t net_bits[128]={0};
uint8_t net_bits_tx[16]={0};
uint8_t prev_net_bits_tx[16]={0};
uint16_t net_regs[128]={0};
uint16_t net_regs_tx[16]={0};
uint16_t prev_net_regs_tx[16]={0};

// SS1..SS8 (nodes online offline)
extern uint16_t node_link[8];
// SS9 PC21 Network Can
extern uint16_t can_link;
// SS10 startup impulse
extern uint16_t start_up;
// SS11 seconds since power up
extern uint32_t seconds;
// SS12 minutes since power up
extern uint32_t minutes;
// SS13 cluster status (not used)
extern uint16_t cluster_state;
// SS14 telemetry status
extern uint16_t telemetry_state;
// SS15 cluster number
extern uint16_t cluster_num;
// SS16..SS23 clusters online offline
extern uint16_t cluster_link[8];
// SS24 aidi link
extern uint8_t adc_link;


const char* app_name = "тестовый проект 1234567890";
const char* app_build_date = "00.00.00 00:00:00";
const char* app_version = "1.0";

uint16_t app_id = 0;
uint8_t can_addr = 0x00;
const char* di_names[14] = {"","","","","","","","","","","","","","",};
const char* do_names[6] = {"","","","","","",};
extern unsigned char scada_bits[16];
extern unsigned short scada_regs[16];

uint16_t mmb[128]={0};
uint8_t err_mod_can1[256]={0};
uint8_t err_mod_can2[256]={0};
const uint16_t canal1_req_count=0;
const uint16_t canal1_modbus_delay = 100;


const mvar_reqs canal1_mvar_reqs[] = {
};
const uint16_t canal2_req_count=0;
const uint16_t canal2_modbus_delay = 100;


const mvar_reqs canal2_mvar_reqs[] = {
};
extern struct ai_mod* ai_modules_ptr;
extern struct do_mod* do_modules_ptr;
extern struct rs_mod* rs_modules_ptr;

void init_vars()
{
}

void inc_timers(){
}

uint16_t used_ai = 0x0;

const char* adc_names[14] = {"","","","","","","","","","","","","","",};

uint8_t tdu[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

void calculate_adc(){
	unsigned char i=0;
	float v=0;
	for(i=0;i<14;i++) {
		v=ain_raw[i];
		switch(i) {
			case 0:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 1:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 2:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 3:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 4:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 5:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 6:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 7:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 8:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 9:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 10:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 11:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 12:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
			case 13:
				ain[i]=0;
				ain_under[i]=0;
				ain_over[i]=0;
				ain_alarm[i]=0;
				break;
		};
	}
}

void ld_process(void) {
}
