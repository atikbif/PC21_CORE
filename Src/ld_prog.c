#include "ld_prog.h"
#include "elements.h"
#include "iodef.h"
#include "os_conf.h"
#include "stdint.h"
#include "system_vars.h"

unsigned short plc_cycle = 100;

unsigned short ain[AI_CNT];
unsigned short ain_raw[AI_CNT];
unsigned char ain_under[AI_CNT];
unsigned char ain_over[AI_CNT];
unsigned char ain_alarm[AI_CNT];
unsigned char din[DI_CNT];
unsigned char dinr[DI_CNT];
unsigned char dinf[DI_CNT];
unsigned char dout[DO_CNT];
extern unsigned char din_break[4];;
extern unsigned char din_short_circuit[4];;
extern unsigned char din_fault[4];;
unsigned char ibit[IBIT_CNT];
unsigned short tmrms[TMRMS_CNT];
unsigned short tmrs[TMRS_CNT];
unsigned short ireg[IREG_CNT];
unsigned short frsh[FRSH_CNT];
double frd[FRD_CNT];
unsigned long frl[FRL_CNT];

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


uint16_t used_ai = 0x0000;


extern unsigned short work_time;



unsigned short p1_v1=1;
unsigned long p1_v2=0;
unsigned long p1_v4=0;
unsigned long p1_v9=0;
unsigned char p1_v18=0;
unsigned char p1_v24=0;
unsigned char p1_v29=0;
unsigned char p1_v30=0;
unsigned short p1_v38=0;
unsigned long p1_v39=0;
unsigned long p1_v41=0;
unsigned long p1_v46=0;
unsigned char p1_v55=0;
unsigned char p1_v61=0;
unsigned char p1_v66=0;
unsigned char p1_v67=0;
unsigned short p1_v75=0;
unsigned char p1_v83=0;
double p1_v87=0;
unsigned long p1_v89=0;
double p1_v93=0;
unsigned long p1_v95=0;
unsigned char p1_v101=0;
unsigned char p1_v102=0;
unsigned char p1_v103=0;
unsigned char p1_v109=0;
unsigned char p1_v116=0;
unsigned char p1_v122=0;
unsigned short p1_v123=0;
unsigned long p1_v124=0;
unsigned long p1_v125=0;
unsigned long p1_v126=0;
unsigned short p1_v127=0;
unsigned long p1_v128=0;
unsigned short p1_v129=0;
unsigned long p1_v130=0;
unsigned short p1_v131=0;
unsigned char p1_v132=0;
unsigned long p1_v133=0;
unsigned short p1_v134=0;
unsigned char p1_v135=0;
unsigned char p1_v136=0;
unsigned char p1_v137=0;
unsigned char p1_v138=0;
unsigned char p1_v139=0;
unsigned short p1_v140=0;
unsigned short p1_v141=0;
unsigned char p1_v142=0;
unsigned char p1_v143=0;
unsigned char p1_v144=0;
unsigned short p1_v145=0;
unsigned short p1_v146=0;
unsigned long p1_v147=0;
unsigned long p1_v148=0;
unsigned long p1_v149=0;
unsigned long p1_v150=0;
unsigned short p1_v151=0;
unsigned long p1_v152=0;
unsigned short p1_v153=0;
unsigned char p1_v154=0;
unsigned long p1_v155=0;
unsigned short p1_v156=0;
unsigned char p1_v157=0;
unsigned char p1_v158=0;
unsigned char p1_v159=0;
unsigned long p1_v160=0;
unsigned long p1_v161=0;
unsigned short p1_v162=0;
unsigned short p1_v163=0;
unsigned long p1_v164=0;
unsigned long p1_v165=0;
unsigned short p1_v166=0;
unsigned short p1_v167=0;
unsigned short p1_v168=0;
unsigned short p1_v169=0;
unsigned short p1_v170=0;
unsigned short p1_v171=0;
unsigned short p1_v172=0;
unsigned short p1_v173=0;

void inc_timers(){
}

const char* di_names[14] = {"","","","","","","","","","","","","","",};
const char* do_names[6] = {"","","","","","",};
const char* adc_names[14] = {"","","","","","","","","","","","","","",};
uint8_t tdu[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

uint16_t app_id = 0;
uint8_t can_addr = 0x00;

void calculate_adc(){
	unsigned char i=0;
	float v=0;
	for(i=0;i<14;i++) {
		v=ain_raw[i];
		switch(i) {
			case 0:
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
			case 1:
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
			case 2:
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


extern unsigned char scada_bits[16];
extern unsigned short scada_regs[16];

#include "modbus_master.h"

uint16_t mmb[64]={0};
uint8_t err_mod[256]={0};


const uint16_t canal1_req_count=0;
const uint16_t canal1_modbus_delay = 100;


const mvar_reqs canal1_mvar_reqs[] = {
};

const uint16_t canal2_req_count=1;
const uint16_t canal2_modbus_delay = 90;

const mvar canal2_req1_vars[] = {
	{0,-1,&MODB1},
	{4,-1,&MODB3}
};

const mvar_reqs canal2_mvar_reqs[] = {
	{"\x01\x04\x00\x00\x00\x03\xb0\x0b", 8, 11, canal2_req1_vars, 2, 0}
};

void ld_process(void) {
	p1_v123=variable_unsigned_short(AI1,p1_v1);
	p1_v124=const_unsigned_long(250,p1_v1);
	p1_v2=const_unsigned_long(128,p1_v1);
	p1_v4=const_unsigned_long(255,p1_v1);
	p1_v125=multiple_unsigned_long(p1_v123,p1_v124);
	p1_v126=plus_unsigned_long(p1_v125,p1_v2);
	p1_v9=divide_unsigned_long(p1_v126,p1_v4);
	p1_v127=relay_unsigned_short(&IR1,p1_v9);
	p1_v129=variable_unsigned_short(IR1,p1_v1);
	p1_v128=const_unsigned_long(100,p1_v1);
	p1_v131=variable_unsigned_short(IR1,p1_v1);
	p1_v130=const_unsigned_long(90,p1_v1);
	p1_v18=greater_or_equal_unsigned_long(p1_v129,p1_v128);
	p1_v132=greater_or_equal_unsigned_long(p1_v131,p1_v130);
	p1_v18=p1_v18 | open_contact(IB1,p1_v132);
	p1_v136=relay_bool(&IB1,p1_v18);
	p1_v134=variable_unsigned_short(IR1,p1_v1);
	p1_v133=const_unsigned_long(90,p1_v1);
	p1_v135=greater_or_equal_unsigned_long(p1_v134,p1_v133);
	p1_v24=clos_contact(IB1,p1_v135);
	p1_v137=relay_bool(&IB2,p1_v24);
	p1_v138=clos_contact(IB1,p1_v1);
	p1_v30=clos_contact(IB2,p1_v138);
	p1_v29=not_operation_bool(p1_v30);
	p1_v139=relay_bool(&DO1,p1_v29);
	p1_v38=variable_unsigned_short(AI1,p1_v1);
	p1_v140=relay_unsigned_short(&IR2,p1_v38);
	p1_v83=clos_contact(IB1,p1_v1);
	p1_v159=relay_bool(&DO5,p1_v83);
	p1_v146=variable_unsigned_short(AI2,p1_v1);
	p1_v147=const_unsigned_long(500,p1_v1);
	p1_v39=const_unsigned_long(128,p1_v1);
	p1_v41=const_unsigned_long(255,p1_v1);
	p1_v148=multiple_unsigned_long(p1_v146,p1_v147);
	p1_v149=plus_unsigned_long(p1_v148,p1_v39);
	p1_v46=divide_unsigned_long(p1_v149,p1_v41);
	p1_v141=relay_unsigned_short(&IR3,p1_v46);
	p1_v151=variable_unsigned_short(IR3,p1_v1);
	p1_v150=const_unsigned_long(170,p1_v1);
	p1_v153=variable_unsigned_short(IR3,p1_v1);
	p1_v152=const_unsigned_long(153,p1_v1);
	p1_v55=greater_or_equal_unsigned_long(p1_v151,p1_v150);
	p1_v154=greater_or_equal_unsigned_long(p1_v153,p1_v152);
	p1_v55=p1_v55 | open_contact(IB3,p1_v154);
	p1_v142=relay_bool(&IB3,p1_v55);
	p1_v156=variable_unsigned_short(IR3,p1_v1);
	p1_v155=const_unsigned_long(153,p1_v1);
	p1_v157=greater_or_equal_unsigned_long(p1_v156,p1_v155);
	p1_v61=clos_contact(IB3,p1_v157);
	p1_v143=relay_bool(&IB4,p1_v61);
	p1_v158=clos_contact(IB3,p1_v1);
	p1_v67=clos_contact(IB4,p1_v158);
	p1_v66=not_operation_bool(p1_v67);
	p1_v144=relay_bool(&DO2,p1_v66);
	p1_v75=variable_unsigned_short(AI2,p1_v1);
	p1_v145=relay_unsigned_short(&IR4,p1_v75);
	p1_v160=const_unsigned_long(0,p1_v1);
	p1_v161=const_unsigned_long(250,p1_v1);
	p1_v162=variable_unsigned_short(AI1_RAW,p1_v1);
	p1_v163=relay_unsigned_short(&IR5,p1_v89);
	p1_v164=const_unsigned_long(0,p1_v1);
	p1_v165=const_unsigned_long(500,p1_v1);
	p1_v166=variable_unsigned_short(AI2_RAW,p1_v1);
	p1_v167=relay_unsigned_short(&IR6,p1_v95);
	p1_v168=variable_unsigned_short(SC_REG1,p1_v1);
	p1_v169=const_unsigned_short(1,p1_v1);
	p1_v101=relay_bool(&DO3,p1_v109);
	p1_v170=variable_unsigned_short(SC_REG1,p1_v1);
	p1_v171=const_unsigned_short(2,p1_v1);
	p1_v103=relay_bool(&DO4,p1_v116);
	p1_v172=variable_unsigned_short(SC_REG1,p1_v1);
	p1_v173=const_unsigned_short(4,p1_v1);
	p1_v102=relay_bool(&DO6,p1_v122);
}
