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

unsigned short p1_v1=1;unsigned char p1_v2=0;unsigned char p1_v9=0;unsigned char p1_v10=0;unsigned char p1_v17=0;unsigned char p1_v18=0;unsigned char p1_v25=0;unsigned char p1_v26=0;unsigned char p1_v33=0;unsigned char p1_v34=0;unsigned char p1_v41=0;unsigned char p1_v42=0;unsigned char p1_v49=0;unsigned char p1_v50=0;unsigned char p1_v57=0;unsigned char p1_v58=0;unsigned char p1_v65=0;unsigned char p1_v66=0;unsigned char p1_v73=0;uint16_t cluster_regs[64]={0};
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

uint16_t app_id = 0;
uint8_t can_addr = 0x00;
const char* di_names[14] = {"","","","","","","","","","","","","","",};
const char* do_names[6] = {"","","","","","",};
extern unsigned char scada_bits[16];
extern unsigned short scada_regs[16];

uint16_t mmb[64]={0};
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
	p1_v2=open_contact(IB1,p1_v1);
	p1_v9=relay_bool(&MDO1,p1_v2);
	p1_v10=open_contact(IB2,p1_v1);
	p1_v17=relay_bool(&MDO2,p1_v10);
	p1_v18=open_contact(IB3,p1_v1);
	p1_v25=relay_bool(&MDO3,p1_v18);
	p1_v26=open_contact(IB4,p1_v1);
	p1_v33=relay_bool(&MDO4,p1_v26);
	p1_v34=open_contact(MDO1_FAULT,p1_v1);
	p1_v41=relay_bool(&IB5,p1_v34);
	p1_v42=open_contact(MDO2_FAULT,p1_v1);
	p1_v49=relay_bool(&IB6,p1_v42);
	p1_v50=open_contact(MDO3_FAULT,p1_v1);
	p1_v57=relay_bool(&IB7,p1_v50);
	p1_v58=open_contact(MDO4_FAULT,p1_v1);
	p1_v65=relay_bool(&IB8,p1_v58);
	p1_v66=open_contact(MDO1_LINK,p1_v1);
	p1_v73=relay_bool(&IB9,p1_v66);
}
