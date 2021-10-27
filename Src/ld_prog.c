#include "ld_prog.h"
#include "elements.h"
#include "iodef.h"
#include "os_conf.h"
#include "lib_elements.h"
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
extern unsigned char din_break[DI_CNT];
extern unsigned char din_short_circuit[DI_CNT];
extern unsigned char din_fault[DI_CNT];
unsigned char ibit[IBIT_CNT];
unsigned short tmrms[TMRMS_CNT];
unsigned short tmrs[TMRS_CNT];
unsigned short ireg[IREG_CNT];
unsigned short frsh[FRSH_CNT];
double frd[FRD_CNT];
unsigned long frl[FRL_CNT];
extern unsigned short work_time;

unsigned short p1_v1=1;unsigned char p1_v9=0;unsigned char p1_v17=0;unsigned char p1_v25=0;unsigned char p1_v33=0;unsigned char p1_v41=0;unsigned char p1_v49=0;unsigned char p1_v57=0;unsigned char p1_v65=0;unsigned char p1_v73=0;unsigned char p1_v81=0;unsigned short p1_v89=0;unsigned short p1_v97=0;unsigned short p1_v105=0;unsigned short p1_v113=0;unsigned short p1_v121=0;unsigned short p1_v129=0;unsigned char p1_v137=0;unsigned char p1_v145=0;unsigned char p1_v153=0;unsigned char p1_v161=0;unsigned char p1_v169=0;unsigned char p1_v177=0;unsigned char p1_v178=0;unsigned char p1_v184=0;unsigned char p1_v185=0;unsigned char p1_v191=0;unsigned char p1_v192=0;unsigned char p1_v198=0;unsigned char p1_v199=0;unsigned char p1_v205=0;unsigned char p1_v213=0;unsigned short p1_v221=0;unsigned short p1_v229=0;unsigned short p1_v237=0;unsigned short p1_v245=0;unsigned short p1_v253=0;unsigned char p1_v254=0;unsigned char p1_v255=0;unsigned char p1_v256=0;unsigned char p1_v257=0;unsigned char p1_v258=0;unsigned char p1_v259=0;unsigned char p1_v260=0;unsigned char p1_v261=0;unsigned char p1_v262=0;unsigned char p1_v263=0;unsigned short p1_v264=0;unsigned short p1_v265=0;unsigned short p1_v266=0;unsigned short p1_v267=0;unsigned short p1_v268=0;unsigned short p1_v269=0;unsigned char p1_v270=0;unsigned char p1_v271=0;unsigned char p1_v272=0;unsigned char p1_v273=0;unsigned char p1_v274=0;unsigned char p1_v275=0;unsigned char p1_v276=0;unsigned char p1_v277=0;unsigned char p1_v278=0;unsigned char p1_v279=0;unsigned char p1_v280=0;unsigned short p1_v281=0;unsigned short p1_v282=0;unsigned short p1_v283=0;unsigned short p1_v284=0;unsigned char p1_v285=0;uint16_t cluster_regs[64]={0};
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

const char* app_name = "test";
const char* app_build_date = "19.10.2021 15:43:54";
const char* app_version = "1.0";
uint16_t app_id = 10000;
uint8_t can_addr = 0x00;
const char* di_names[14] = {"DI-1","DI-2","DI-3","DI-4","DI-5","DI-6","DI-7","","","","","","","",};
const char* do_names[6] = {"DO-1","DO-2","DO-3","DO-4","DO-5","DO-6",};
extern unsigned char scada_bits[16];
extern unsigned short scada_regs[16];

uint16_t mmb[128]={0};
uint8_t err_mod_can1[256]={0};
uint8_t err_mod_can2[256]={0};
const uint16_t canal1_req_count=1;
const uint16_t canal1_modbus_delay = 100;

const mvar canal1_req1_vars[] = {
	{0,-1,&MODB1}
};

const mvar_reqs canal1_mvar_reqs[] = {
	{"\x01\x03\x00\x00\x00\x01\x84\x0a", 8, 7, canal1_req1_vars, 1, 0}
};
const uint16_t canal2_req_count=1;
const uint16_t canal2_modbus_delay = 100;

const mvar canal2_req1_vars[] = {
	{0,-1,&MODB2}
};

const mvar_reqs canal2_mvar_reqs[] = {
	{"\x01\x03\x00\x00\x00\x01\x84\x0a", 8, 7, canal2_req1_vars, 1, 0}
};
extern struct ai_mod* ai_modules_ptr;
extern struct do_mod* do_modules_ptr;
extern struct rs_mod* rs_modules_ptr;

void init_vars() 
{
}

void inc_timers(){
}

uint16_t used_ai = 0x3f80;

const char* adc_names[14] = {"","","","","","","","AI8","AI9","AI10","AI11","AI12","AI13","AI14",};

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
				if(v<400) v=400;
				if(v>2000) v=2000;
				v = 255*v/1600-255/4.0+0.5;
				if(v>255) v=255;
				if(ain_raw[i]<=200) {ain[i]=0;ain_alarm[i]=1;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]<=340) {ain[i]=0;ain_alarm[i]=0;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]>=2200) {ain[i]=255;ain_alarm[i]=1;ain_under[i]=0;ain_over[i]=1;}
				else if(ain_raw[i]>=2050) {ain[i]=255;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=1;}
				else {ain[i]=(unsigned char)v;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=0;}
				break;
			case 8:
				if(v<400) v=400;
				if(v>2000) v=2000;
				v = 255*v/1600-255/4.0+0.5;
				if(v>255) v=255;
				if(ain_raw[i]<=200) {ain[i]=0;ain_alarm[i]=1;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]<=340) {ain[i]=0;ain_alarm[i]=0;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]>=2200) {ain[i]=255;ain_alarm[i]=1;ain_under[i]=0;ain_over[i]=1;}
				else if(ain_raw[i]>=2050) {ain[i]=255;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=1;}
				else {ain[i]=(unsigned char)v;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=0;}
				break;
			case 9:
				if(v<400) v=400;
				if(v>2000) v=2000;
				v = 255*v/1600-255/4.0+0.5;
				if(v>255) v=255;
				if(ain_raw[i]<=200) {ain[i]=0;ain_alarm[i]=1;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]<=340) {ain[i]=0;ain_alarm[i]=0;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]>=2200) {ain[i]=255;ain_alarm[i]=1;ain_under[i]=0;ain_over[i]=1;}
				else if(ain_raw[i]>=2050) {ain[i]=255;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=1;}
				else {ain[i]=(unsigned char)v;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=0;}
				break;
			case 10:
				if(v<400) v=400;
				if(v>2000) v=2000;
				v = 255*v/1600-255/4.0+0.5;
				if(v>255) v=255;
				if(ain_raw[i]<=200) {ain[i]=0;ain_alarm[i]=1;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]<=340) {ain[i]=0;ain_alarm[i]=0;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]>=2200) {ain[i]=255;ain_alarm[i]=1;ain_under[i]=0;ain_over[i]=1;}
				else if(ain_raw[i]>=2050) {ain[i]=255;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=1;}
				else {ain[i]=(unsigned char)v;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=0;}
				break;
			case 11:
				if(v<400) v=400;
				if(v>2000) v=2000;
				v = 255*v/1600-255/4.0+0.5;
				if(v>255) v=255;
				if(ain_raw[i]<=200) {ain[i]=0;ain_alarm[i]=1;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]<=340) {ain[i]=0;ain_alarm[i]=0;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]>=2200) {ain[i]=255;ain_alarm[i]=1;ain_under[i]=0;ain_over[i]=1;}
				else if(ain_raw[i]>=2050) {ain[i]=255;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=1;}
				else {ain[i]=(unsigned char)v;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=0;}
				break;
			case 12:
				if(v<400) v=400;
				if(v>2000) v=2000;
				v = 255*v/1600-255/4.0+0.5;
				if(v>255) v=255;
				if(ain_raw[i]<=200) {ain[i]=0;ain_alarm[i]=1;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]<=340) {ain[i]=0;ain_alarm[i]=0;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]>=2200) {ain[i]=255;ain_alarm[i]=1;ain_under[i]=0;ain_over[i]=1;}
				else if(ain_raw[i]>=2050) {ain[i]=255;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=1;}
				else {ain[i]=(unsigned char)v;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=0;}
				break;
			case 13:
				if(v<400) v=400;
				if(v>2000) v=2000;
				v = 255*v/1600-255/4.0+0.5;
				if(v>255) v=255;
				if(ain_raw[i]<=200) {ain[i]=0;ain_alarm[i]=1;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]<=340) {ain[i]=0;ain_alarm[i]=0;ain_under[i]=1;ain_over[i]=0;}
				else if(ain_raw[i]>=2200) {ain[i]=255;ain_alarm[i]=1;ain_under[i]=0;ain_over[i]=1;}
				else if(ain_raw[i]>=2050) {ain[i]=255;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=1;}
				else {ain[i]=(unsigned char)v;ain_alarm[i]=0;ain_under[i]=0;ain_over[i]=0;}
				break;
		};
	}
}

void ld_process(void) {
	p1_v9=open_contact(MDI1,p1_v1);
	p1_v254=relay_bool(&DO1,p1_v9);
	p1_v17=open_contact(MDI2,p1_v1);
	p1_v255=relay_bool(&DO2,p1_v17);
	p1_v25=open_contact(MDI3,p1_v1);
	p1_v256=relay_bool(&DO3,p1_v25);
	p1_v33=open_contact(MDI4,p1_v1);
	p1_v257=relay_bool(&DO4,p1_v33);
	p1_v41=open_contact(DI1,p1_v1);
	p1_v258=relay_bool(&DO5,p1_v41);
	p1_v49=open_contact(DI2,p1_v1);
	p1_v259=relay_bool(&DO6,p1_v49);
	p1_v57=open_contact(DI3,p1_v1);
	p1_v260=relay_bool(&MDO1,p1_v57);
	p1_v65=open_contact(DI4,p1_v1);
	p1_v261=relay_bool(&MDO2,p1_v65);
	p1_v73=open_contact(DI5,p1_v1);
	p1_v262=relay_bool(&MDO3,p1_v73);
	p1_v81=open_contact(DI6,p1_v1);
	p1_v263=relay_bool(&MDO4,p1_v81);
	p1_v89=variable_unsigned_short(MODB1,p1_v1);
	p1_v264=relay_unsigned_short(&CLREG24,p1_v89);
	p1_v97=variable_unsigned_short(MODB2,p1_v1);
	p1_v265=relay_unsigned_short(&CLREG25,p1_v97);
	p1_v105=variable_unsigned_short(MAI5,p1_v1);
	p1_v266=relay_unsigned_short(&CLREG17,p1_v105);
	p1_v113=variable_unsigned_short(MAI6,p1_v1);
	p1_v267=relay_unsigned_short(&CLREG18,p1_v113);
	p1_v121=variable_unsigned_short(MAI7,p1_v1);
	p1_v268=relay_unsigned_short(&CLREG19,p1_v121);
	p1_v129=variable_unsigned_short(MAI8,p1_v1);
	p1_v269=relay_unsigned_short(&CLREG20,p1_v129);
	p1_v137=open_contact(AIDI_LINK_ADDR2,p1_v1);
	p1_v270=relay_bool(&CLBIT17,p1_v137);
	p1_v145=open_contact(DO_LINK_ADDR3,p1_v1);
	p1_v271=relay_bool(&CLBIT18,p1_v145);
	p1_v153=open_contact(MDI1,p1_v1);
	p1_v272=relay_bool(&CLBIT19,p1_v153);
	p1_v161=open_contact(MDI2,p1_v1);
	p1_v273=relay_bool(&CLBIT20,p1_v161);
	p1_v169=open_contact(MDI3,p1_v1);
	p1_v274=relay_bool(&CLBIT21,p1_v169);
	p1_v177=open_contact(MDI4,p1_v1);
	p1_v275=relay_bool(&CLBIT22,p1_v177);
	p1_v178=open_contact(MDO1,p1_v1);
	p1_v184=open_contact(DO_LINK_ADDR3,p1_v178);
	p1_v276=relay_bool(&CLBIT23,p1_v184);
	p1_v185=open_contact(MDO2,p1_v1);
	p1_v191=open_contact(DO_LINK_ADDR3,p1_v185);
	p1_v277=relay_bool(&CLBIT24,p1_v191);
	p1_v192=open_contact(MDO3,p1_v1);
	p1_v198=open_contact(DO_LINK_ADDR3,p1_v192);
	p1_v278=relay_bool(&CLBIT25,p1_v198);
	p1_v199=open_contact(MDO4,p1_v1);
	p1_v205=open_contact(DO_LINK_ADDR3,p1_v199);
	p1_v279=relay_bool(&CLBIT26,p1_v205);
	p1_v213=open_contact(RS_LINK_ADDR4,p1_v1);
	p1_v280=relay_bool(&CLBIT27,p1_v213);
	p1_v221=variable_unsigned_short(MRS1,p1_v1);
	p1_v281=relay_unsigned_short(&CLREG21,p1_v221);
	p1_v229=variable_unsigned_short(MRS5,p1_v1);
	p1_v282=relay_unsigned_short(&CLREG22,p1_v229);
	p1_v237=variable_unsigned_short(MRS9,p1_v1);
	p1_v283=relay_unsigned_short(&CLREG23,p1_v237);
	p1_v284=variable_unsigned_short(MRS1_LINK,p1_v1);
	p1_v245=relay_unsigned_short(&IR1,p1_v284);
	p1_v285=open_contact(RS_LINK_ADDR4,p1_v1);
	p1_v253=relay_unsigned_short(&IR2,p1_v285);
}
