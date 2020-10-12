#include "ld_prog.h"
#include "elements.h"
#include "iodef.h"
#include "os_conf.h"
#include "stdint.h"
volatile unsigned short plc_cycle = 100;

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


double res=0;
double real_value=0;

double p1_v8=0;
double p1_v16=0;
unsigned long p1_v20=0;
unsigned short p1_v26=0;
unsigned short p1_v31=0;
unsigned short p1_v32=1;
double p1_v33=0;
unsigned short p1_v34=0;
unsigned short p1_v35=0;
double p1_v36=0;
double p1_v37=0;
double p1_v38=0;
double p1_v39=0;
double p1_v40=0;
double p1_v41=0;
double p1_v42=0;
double p1_v43=0;
double p1_v44=0;
unsigned short p1_v45=0;
double p1_v46=0;
unsigned short p1_v47=0;
unsigned short p1_v48=0;
double p1_v49=0;
struct pid_data p1_pid_st1={0,0,0};

void init_vars() {
}

void inc_timers(){
}

uint16_t app_id = 0;
uint8_t can_addr = 0x00;
const char* di_names[14] = {"","","","","","","","","","","","","","",};

const char* do_names[6] = {"","","","","","",};

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


extern unsigned char scada_bits[16];
extern unsigned short scada_regs[16];

#include "modbus_master.h"

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
	p1_v33=variable_double(real_value,p1_v32);
	p1_v34=const_unsigned_short(200,p1_v32);
	p1_v35=const_unsigned_short(50,p1_v32);
	p1_v36=const_double(0.01,p1_v32);
	p1_v37=const_double(1,p1_v32);
	p1_v38=const_double(2,p1_v32);
	p1_v39=const_double(0,p1_v32);
	p1_v40=const_double(100,p1_v32);
	p1_v41=calculate_pid(&p1_pid_st1,p1_v33,p1_v34,p1_v35,p1_v36,p1_v37,p1_v38,p1_v39,p1_v40);
	p1_v8=relay_double(&res,p1_v41);
	p1_v42=variable_double(res,p1_v32);
	p1_v43=const_double(2,p1_v32);
	p1_v44=multiple_double(p1_v42,p1_v43);
	p1_v45=const_unsigned_short(25,p1_v32);
	p1_v46=plus_double(p1_v44,p1_v45);
	p1_v16=relay_double(&real_value,p1_v46);
	p1_v49=variable_double(real_value,p1_v32);
	//p1_v20=fl_to_long(p1_v49);
	//p1_v47=low_word(p1_v20);
	//p1_v48=high_word(p1_v20);
	p1_v26=relay_unsigned_short(&IR3,p1_v47);
	p1_v31=relay_unsigned_short(&IR4,p1_v48);
}
