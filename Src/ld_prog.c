#include "ld_prog.h"
#include "elements.h"
#include "iodef.h"
#include "os_conf.h"
#include "stdint.h"
#include "heartbeat.h"
#include "moddef.h"
#include "modules.h"

volatile unsigned short plc_cycle = 100;

uint16_t cluster_regs[CLUST_REG_NUM]={0};
uint16_t prev_cluster_regs[CLUST_REG_NUM]={0};
uint8_t cluster_bits[CLUSTER_BITS_NUM]={0};
uint8_t prev_cluster_bits[CLUSTER_BITS_NUM]={0};
uint8_t net_bits[NET_BIT_NUM]={0};
uint8_t net_bits_tx[NET_TX_BIT_NUM]={0};
uint8_t prev_net_bits_tx[NET_TX_BIT_NUM]={0};
uint16_t net_regs[NET_REG_NUM]={0};
uint16_t net_regs_tx[NET_TX_REG_NUM]={0};
uint16_t prev_net_regs_tx[NET_TX_REG_NUM]={0};


// SS1..SS8 (nodes online offline)
extern uint16_t node_link[MAX_NODE_CNT];

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


extern unsigned short work_time;


unsigned short p1_v1=1;
unsigned char p1_v10=0;
unsigned char p1_v12=0;
unsigned char p1_v19=0;
unsigned char p1_v26=0;
unsigned char p1_v35=0;
unsigned char p1_v37=0;
unsigned short p1_v45=0;
unsigned short p1_v53=0;
unsigned short p1_v61=0;
unsigned short p1_v69=0;
unsigned short p1_v77=0;
unsigned short p1_v85=0;
unsigned short p1_v93=0;
unsigned short p1_v101=0;
unsigned char p1_v102=0;
unsigned short p1_v103=0;
unsigned short p1_v104=0;
unsigned char p1_v105=0;
unsigned short p1_v106=0;
unsigned short p1_v107=0;
unsigned char p1_v108=0;
unsigned char p1_v109=0;
unsigned short p1_v110=0;
unsigned short p1_v111=0;
unsigned short p1_v112=0;
unsigned short p1_v113=0;
unsigned short p1_v114=0;
unsigned short p1_v115=0;
unsigned short p1_v116=0;
unsigned short p1_v117=0;
struct pid_data p1_pid_st1={0,0,0};

void init_vars() {
}

void inc_timers(){
}

uint16_t app_id = 0;
uint8_t can_addr = 0x00;
const char* di_names[AI_CNT] = {"","","","","","","","","","","","","","",};
const char* do_names[DO_CNT] = {"","","","","","",};
const char* adc_names[AI_CNT] = {"","","","","","","","","","","","","","",};

uint16_t used_ai = 0x0;
uint8_t tdu[AI_CNT] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

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


extern unsigned char scada_bits[16];
extern unsigned short scada_regs[16];

#include "modbus_master.h"

unsigned char bit_state(long inp,short n){if(n<0) n=0;if(n>31) n=31;if(inp & (1<<n)) return 1;return 0;}

uint16_t mmb[64]={0};
uint8_t err_mod[256]={0};

const uint16_t canal1_req_count=0;
const uint16_t canal1_modbus_delay = 100;

const mvar_reqs canal1_mvar_reqs[] = {
};

const uint16_t canal2_req_count=0;
const uint16_t canal2_modbus_delay = 100;

const mvar_reqs canal2_mvar_reqs[] = {
};

void ld_process(void) {
	p1_v12=open_contact(SC_BIT1,p1_v1);
	p1_v10=open_contact(DO5,p1_v1);
	p1_v12=p1_v12 | clos_contact(SC_BIT2,p1_v10);
	p1_v102=relay_bool(&DO5,p1_v12);
	p1_v103=variable_unsigned_short(SC_REG1,p1_v1);
	p1_v104=const_unsigned_short(0,p1_v1);
	p1_v19=bit_state(p1_v103,p1_v104);
	p1_v105=relay_bool(&IB1,p1_v19);
	p1_v106=variable_unsigned_short(SC_REG1,p1_v1);
	p1_v107=const_unsigned_short(1,p1_v1);
	p1_v26=bit_state(p1_v106,p1_v107);
	p1_v108=relay_bool(&IB2,p1_v26);
	p1_v37=open_contact(IB1,p1_v1);
	p1_v35=open_contact(DO6,p1_v1);
	p1_v37=p1_v37 | clos_contact(IB2,p1_v35);
	p1_v109=relay_bool(&DO6,p1_v37);
	p1_v45=variable_unsigned_short(MODB1,p1_v1);
	p1_v110=relay_unsigned_short(&CLREG17,p1_v45);
	p1_v69=variable_unsigned_short(MODB2,p1_v1);
	p1_v111=relay_unsigned_short(&CLREG18,p1_v69);
	p1_v53=variable_unsigned_short(MODB3,p1_v1);
	p1_v112=relay_unsigned_short(&CLREG19,p1_v53);
	p1_v61=variable_unsigned_short(MODB4,p1_v1);
	p1_v113=relay_unsigned_short(&CLREG20,p1_v61);
	p1_v77=variable_unsigned_short(MODB5,p1_v1);
	p1_v114=relay_unsigned_short(&CLREG21,p1_v77);
	p1_v85=variable_unsigned_short(MODB6,p1_v1);
	p1_v115=relay_unsigned_short(&CLREG22,p1_v85);
	p1_v93=variable_unsigned_short(MODB7,p1_v1);
	p1_v116=relay_unsigned_short(&CLREG23,p1_v93);
	p1_v101=variable_unsigned_short(MODB8,p1_v1);
	p1_v117=relay_unsigned_short(&CLREG24,p1_v101);
}
