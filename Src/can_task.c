/*
 * can_task.c
 *
 *  Created on: 22 мар. 2020 г.
 *      Author: User
 */

#include "can_task.h"
#include "cmsis_os.h"
#include "main.h"
#include "os_conf.h"
#include "can_tx_stack.h"
#include <string.h>
#include "led.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

static CAN_RxHeaderTypeDef   RxHeader;
static uint8_t               RxData[8];

static CAN_RxHeaderTypeDef   RxHeader2;
static uint8_t               RxData2[8];

static CAN_TxHeaderTypeDef   TxHeader;
static uint32_t              TxMailbox=0;
static uint8_t               TxData[8];

static CAN_TxHeaderTypeDef   TxHeader2;
static uint32_t              TxMailbox2=0;
static uint8_t               TxData2[8];

extern uint8_t can1_tmr;

uint8_t heartbeat_cnt[MAX_NODE_CNT] = {HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX};
uint8_t net_heartbeat_cnt[MAX_NET_CNT] = {HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX,HEARTBEAT_MAX};

uint8_t update_all = 1;
uint16_t update_tmr = 0;
uint16_t update_di = 0x0000;
uint16_t update_ai = 0x0000;

extern uint16_t ai_type;
extern uint16_t used_ai;

extern unsigned short ain_raw[AI_CNT];
extern unsigned short ain[AI_CNT];
extern unsigned char ain_alarm[AI_CNT];

extern unsigned char din[DI_CNT];
extern uint8_t din_fault[DI_CNT];

extern const char* di_names[14];
extern const char* do_names[6];
extern const char* adc_names[14];

extern uint8_t tdu[14];

extern tx_stack can1_tx_stack;
extern tx_stack can2_tx_stack;

extern uint16_t app_id;
extern uint8_t can_addr;
uint8_t cluster_addr=0;

extern uint16_t cluster_regs[64];
extern uint16_t prev_cluster_regs[64];

extern uint8_t cluster_bits[224];
extern uint8_t prev_cluster_bits[224];

static uint8_t written_cluster_bits[28]={0};
static uint8_t written_cluster_regs[64]={0};

extern uint8_t net_bits[128];
extern uint8_t net_bits_tx[16];
extern uint8_t prev_net_bits_tx[16];
extern uint16_t net_regs[128];
extern uint16_t net_regs_tx[16];
extern uint16_t prev_net_regs_tx[16];

static uint8_t net_status = 0;
static uint8_t cluster_status = 0;


void init_can_addr_pins() {

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

uint8_t read_can_addr() {
	can_addr = 0;
	cluster_addr = 0;

	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_8)==GPIO_PIN_RESET) can_addr |= 0x01;
	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_7)==GPIO_PIN_RESET) can_addr |= 0x02;
	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_6)==GPIO_PIN_RESET) can_addr |= 0x04;

	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_5)==GPIO_PIN_RESET) {
		cluster_addr = can_addr;
		can_addr = 0;
	}else {
		if(can_addr==0) return 0;
	}
	return 1;
}

static void can_write_from_stack() {
	tx_stack_data packet;
	uint8_t i = 0;
	uint8_t try = 0;
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)!=0) {
		try++;
		if(try>=10) return;
		if(get_tx_can_packet(&can1_tx_stack,&packet)) {
			if(packet.length>8) continue;
			TxHeader.StdId = packet.id;
			TxHeader.ExtId = 0;
			TxHeader.RTR = CAN_RTR_DATA;
			TxHeader.IDE = CAN_ID_STD;
			TxHeader.TransmitGlobalTime = DISABLE;
			TxHeader.DLC = packet.length;
			for(i=0;i<packet.length;++i) TxData[i] = packet.data[i];
			HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
		}else break;
	}
}

static void can_write_from_stack2() {
	tx_stack_data packet;
	uint8_t i = 0;
	uint8_t try = 0;
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan2)!=0) {
		try++;
		if(try>=10) return;
		if(get_tx_can_packet(&can2_tx_stack,&packet)) {
			if(packet.length>8) continue;
			TxHeader2.StdId = packet.id;
			TxHeader2.ExtId = 0;
			TxHeader2.RTR = CAN_RTR_DATA;
			TxHeader2.IDE = CAN_ID_STD;
			TxHeader2.TransmitGlobalTime = DISABLE;
			TxHeader2.DLC = packet.length;
			for(i=0;i<packet.length;++i) TxData2[i] = packet.data[i];
			HAL_CAN_AddTxMessage(&hcan2, &TxHeader2, TxData2, &TxMailbox2);
		}else break;
	}
}

static void initCANFilter() {
	CAN_FilterTypeDef  sFilterConfig;
	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);

	sFilterConfig.FilterBank = 14;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	sFilterConfig.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig);
}

static void update_cluster_bits() {
	tx_stack_data packet;
	uint8_t offset = can_addr*28;
	uint8_t i=0;
	if(can_addr>7) return;
	for(i=0;i<28;i++) {
		if(cluster_bits[offset+i]!=prev_cluster_bits[offset+i])
		{
			uint8_t mask = 0;
			uint8_t state = 0;
			uint8_t j=0;
			for(j=0;j<8;j++) {
				if(i+j>=28) break;
				if(cluster_bits[offset+i+j]!=prev_cluster_bits[offset+i+j]) {
					mask|=1<<j;
					if(cluster_bits[offset+i+j]) state|=1<<j;
					prev_cluster_bits[offset+i+j] = cluster_bits[offset+i+j];
					written_cluster_bits[i+j]=1;
				}
			}
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x03; // packed deduced digitals
			packet.data[1] = offset+16+i; // bit num
			packet.data[2] = state;
			packet.data[3] = mask;
			add_tx_can_packet(&can1_tx_stack,&packet);
			i+=7; // +1 добавит i++ цикла
		}
	}
}

static void update_cluster_regs() {
	tx_stack_data packet;
	static uint8_t i =0;
	while(1) {
		if(cluster_regs[i]!=prev_cluster_regs[i]) {
			written_cluster_regs[i]=1;
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x06; // global integer values
			packet.data[1] = 17+i; // reg num
			packet.data[2] = cluster_regs[i] & 0xFF; // value low byte
			packet.data[3] = cluster_regs[i] >> 8; // fault high byte
			add_tx_can_packet(&can1_tx_stack,&packet);
			prev_cluster_regs[i]=cluster_regs[i];
			i++;if(i>=64) i=0;
			break;
		}
		i++;
		if(i>=64) {i=0;break;}
	}
}

static void update_net_regs() {
	tx_stack_data packet;
	static uint8_t i =0;
	if(can_addr>7 || cluster_addr>7) return;
	while(1) {
		if(net_regs_tx[i]!=prev_net_regs_tx[i]) {

			// external net
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x0f; // Network Integer Values
			packet.data[1] = (16*cluster_addr) + 1 + i; // starting point
			packet.data[2] = net_regs_tx[i] & 0xFF; // value low byte
			packet.data[3] = net_regs_tx[i] >> 8; // fault high byte
			add_tx_can_packet(&can2_tx_stack,&packet);
			prev_net_regs_tx[i]=net_regs_tx[i];

			// inside cluster
			add_tx_can_packet(&can1_tx_stack,&packet);

			net_regs[cluster_addr*16+i] = net_regs_tx[i];

			i++;if(i>=16) i=0;
			break;
		}
		i++;
		if(i>=16) {i=0;break;}
	}
}

static void update_net_bits() {
	tx_stack_data packet;
	uint8_t i=0;
	if(can_addr>7 || cluster_addr>7) return;
	for(i=0;i<16;i++) {
		if(net_bits_tx[i]!=prev_net_bits_tx[i])
		{
			uint8_t mask = 0;
			uint8_t state = 0;
			uint8_t j=0;
			for(j=0;j<8;j++) {
				if(i+j>=16) break;
				if(net_bits_tx[i+j]!=prev_net_bits_tx[i+j]) {
					net_bits[cluster_addr*16+i+j] = net_bits_tx[i+j];
					mask|=1<<j;
					if(net_bits_tx[i+j]) state|=1<<j;
					prev_net_bits_tx[i+j] = net_bits_tx[i+j];
				}
			}

			// external net
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x0E; // network packed deduced digitals
			packet.data[1] = cluster_addr*16+i+1; // bit num
			packet.data[2] = state;
			packet.data[3] = mask;
			add_tx_can_packet(&can2_tx_stack,&packet);

			// inside the cluster
			add_tx_can_packet(&can1_tx_stack,&packet);
			i+=7; // +1 добавит i++ цикла
		}
	}
}

static void update_di_data() {
	static uint16_t di_tmr=0;
	uint8_t di_mask = 0x00;
	uint8_t send_fault = 0x00;
	uint8_t send_state = 0x00;
	uint16_t di_fitted = 0x0000;
	uint8_t i = 0;

	tx_stack_data packet;

	di_fitted = ai_type & 0x3FFF;
	di_fitted &= ~used_ai;
	di_tmr++;
	if(di_tmr==20) {
		di_mask = update_di & di_fitted & 0xFF;
		if(di_mask)  {
			for(i=0;i<8;i++) {
				if(di_mask & (1<<i)) {
					if(din[i]) send_state |= 1<<i;
					if(din_fault[i]) send_fault |= 1<<i;
				}
			}

			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x01; // packed physical digits
			packet.data[1] = 0x01; // start bit
			packet.data[2] = send_state; // state
			packet.data[3] = send_fault; // fault
			packet.data[4] = di_mask; // mask
			add_tx_can_packet(&can1_tx_stack,&packet);
			update_di &= 0xFF00;
		}
	}else if(di_tmr==40) {
		di_mask = ((update_di & di_fitted)>>8) & 0xFF;
		if(di_mask)  {
			HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
			for(i=0;i<8;i++) {
				if(i+8>=DI_CNT) break;
				if(di_mask & (1<<i)) {
					if(din[i+8]) send_state |= 1<<i;
					if(din_fault[i+8]) send_fault |= 1<<i;
				}
			}

			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x01; // packed physical digits
			packet.data[1] = 0x09; // start bit
			packet.data[2] = send_state; // state
			packet.data[3] = send_fault; // fault
			packet.data[4] = di_mask; // mask
			add_tx_can_packet(&can1_tx_stack,&packet);
			update_di &= 0x00FF;
		}
		di_tmr=0;
	}
}

static void update_ai_data() {
	static uint16_t ai_tmr=0;
	static uint8_t ai_num = 0;
	static const uint8_t ai_max_num = 14;

	tx_stack_data packet;

	ai_tmr++;
	if(ai_tmr>=50) {
		ai_tmr=0;
		for(;;) {
			if((used_ai & (1<<ai_num)) && (update_ai & (1<<ai_num))) {

				packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
				packet.length = 8;
				packet.data[0] = 0x05; // Analogue data scaled with status
				packet.data[1] = ai_num + 1; // inputs number
				packet.data[2] = ain[ai_num]; // ain value
				packet.data[3] = tdu[ai_num]; // tdu type
				if(ain_alarm[ai_num]) {
					packet.data[4] = 0x02; // alarm bits
					packet.data[5] = 0x00;
				}else {
					packet.data[4] = 0x00; // alarm bits
					packet.data[5] = 0x00;
				}
				packet.data[6] = ain_raw[ai_num] & 0xFF; // raw value
				packet.data[7] = ain_raw[ai_num] >> 8;
				add_tx_can_packet(&can1_tx_stack,&packet);
				update_ai &= ~(1<<ai_num);
				break;
			}else {
				ai_num++;
				if(ai_num>=ai_max_num) {ai_num=0;break;}
			}
		}
	}
}

static void update_all_data() {
	uint16_t di_fitted = 0;
	tx_stack_data packet;
	update_tmr++;
	switch(update_tmr) {
		case 10:	// network status
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 3;
			packet.data[0] = 0x0D; // request not fragmented, eoid - 0x0D
			if(can_addr==0) {
				packet.data[1] = cluster_addr+1; // cluster number
				packet.data[2] = 0x00; // network_status
				for(uint8_t i=0;i<8;i++) {
					if(net_heartbeat_cnt[i]<HEARTBEAT_MAX) packet.data[2]|=1<<i;
				}
			}else {
				packet.data[1] = 0x00; // not networked
				packet.data[2] = 0x00;
			}
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 20:	// module data, internal faults
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x09; // internal faults
			packet.data[2] = 0x00; // faults value
			packet.data[3] = 0x00; // faults value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 30:	// module data, analogue o/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x08; // analogue o/p fitted
			packet.data[2] = 0x00; // value
			packet.data[3] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 40:	// module data, relay o/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x07; // relay o/p fitted
			packet.data[2] = 0x3F; // value
			packet.data[3] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 50:	// module data, switch i/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x06; // switch i/p fitted
			packet.data[2] = 0x00; // value
			packet.data[3] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 60:	// module data, digital i/p fitted
			di_fitted = ai_type & 0x3FFF;
			di_fitted &= ~used_ai;
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x05; // digital i/p fitted
			packet.data[2] = di_fitted & 0xFF; // value
			packet.data[3] = di_fitted >> 8; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 70:	// module data, analogue i/p fitted
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x04; // analogue i/p fitted
			packet.data[2] = 0xFF; // value
			packet.data[3] = 0x3F; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 80:	// module data, os version
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 8;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x02; // os version
			packet.data[2] = 0x30; // value
			packet.data[3] = 0x31; // value
			packet.data[4] = 0x2E; // value
			packet.data[5] = 0x34; // value
			packet.data[6] = 0x30; // value
			packet.data[7] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 90:	// module data, bootloader version
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 8;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x01; // bootloader version
			packet.data[2] = 0x30; // value
			packet.data[3] = 0x31; // value
			packet.data[4] = 0x2E; // value
			packet.data[5] = 0x30; // value
			packet.data[6] = 0x37; // value
			packet.data[7] = 0x00; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 100:	// module data, application cn
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 4;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x03; // application cn
			packet.data[2] = app_id & 0xFF; // value
			packet.data[3] = app_id >> 8; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 110:	// module data, module type
			packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 3;
			packet.data[0] = 0x1F; // request not fragmented, eoid - 0x1F
			packet.data[1] = 0x00; // module type
			packet.data[2] = 0x01; // value
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 120:	// display tx mask digital i/p
			di_fitted = ai_type & 0x3FFF;
			di_fitted &= ~used_ai;
			packet.id = 0x0400 | 0x06 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x0A; // tx mask, eoid - 0x0A
			packet.data[1] = 0x00; // display tx mask
			packet.data[2] = di_fitted & 0xFF; // value
			packet.data[3] = di_fitted >> 8; // value
			packet.data[4] = 0x01; // io type - digital i/p
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
		case 130:	// display tx mask analogue i/p
			packet.id = 0x0400 | 0x06 | (can_addr<<3) | (cluster_addr << 7);	// event
			packet.length = 5;
			packet.data[0] = 0x0A; // tx mask, eoid - 0x0A
			packet.data[1] = 0x00; // display tx mask
			packet.data[2] = used_ai & 0xFF; // value
			packet.data[3] = used_ai >> 8; // value
			packet.data[4] = 0x00; // io type - analogue i/p
			add_tx_can_packet(&can1_tx_stack,&packet);
			break;
	}
	if(update_tmr>=200) {
		update_tmr=0;update_all=0;
		update_di=0x3FFF;
		update_ai=0x3FFF;
		uint8_t i=0;
		for(i=0;i<64;i++) {
			if(cluster_regs[i]) prev_cluster_regs[i]=0; // provocate an update
		}
		if(can_addr<8) {
			uint8_t offset = can_addr*28;
			for(i=0;i<28;i++) {
				if(cluster_bits[offset+i]) prev_cluster_bits[offset+i] = 0; // provocate an update
			}
		}
	}
}

static void update_net_status() {
	if(can_addr==0) {
		uint8_t state = 0;
		for(uint8_t i=0;i<8;i++) {
			if(net_heartbeat_cnt[i]<HEARTBEAT_MAX) state|=1<<i;
		}
		if(state!=net_status) {
			tx_stack_data packet;
			packet.id = 0x0400 | 0x07 | (cluster_addr << 7);	// event
			packet.length = 3;
			packet.data[0] = 0x0D; // request not fragmented, eoid - 0x0D
			packet.data[1] = cluster_addr+1; // cluster number
			packet.data[2] = state; // network_status
			add_tx_can_packet(&can1_tx_stack,&packet);
			net_status = state;
		}
	}
}

void canTask(void const * argument) {
	uint16_t net_cnt = 0;
	uint8_t net_heartbeat_value = 0;
	uint16_t net_heartbeat_tmr = 0;
	uint16_t cnt = 0;
	uint16_t heartbeat_tmr = 0;
	uint8_t heartbeat_value = 1;
	uint16_t i = 0;
	uint16_t inp_tmr=0;
	uint16_t clust_tmr = 0;
	tx_stack_data packet;
	initCANFilter();
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	if(can_addr==0) {
		HAL_CAN_Start(&hcan2);
		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
	}

	for(;;) {
		if(can_addr==0) {
			net_cnt++;
			if(net_cnt>=2000) {
				net_cnt = 0;
				packet.id = 0x0400 | 0x01 | (cluster_addr << 7);
				packet.length = 2;
				packet.data[0] = 0x49;
				packet.data[1] = net_heartbeat_value++;
				add_tx_can_packet(&can2_tx_stack,&packet);
			}
		}
		cnt++;
		if(cnt>=500) {
			update_net_status();
			cnt = 0;
			packet.id = 0x0400 | 0x01 | (can_addr<<3) | (cluster_addr << 7);
			packet.length = 2;
			packet.data[0] = 0x49;
			packet.data[1] = heartbeat_value++;
			add_tx_can_packet(&can1_tx_stack,&packet);

			if(can_addr<MAX_NODE_CNT) heartbeat_cnt[can_addr]=0;
		}
		heartbeat_tmr++;
		if(heartbeat_tmr>=1000) {
			heartbeat_tmr = 0;
			for(i=0;i<sizeof(heartbeat_cnt);i++) {
				if(heartbeat_cnt[i]<HEARTBEAT_MAX) heartbeat_cnt[i]++;
			}
		}

		net_heartbeat_tmr++;
		if(net_heartbeat_tmr>=2000) {
			net_heartbeat_cnt[cluster_addr]=0;
			net_heartbeat_tmr = 0;
			for(i=0;i<sizeof(net_heartbeat_cnt);i++) {
				if(net_heartbeat_cnt[i]<HEARTBEAT_MAX) net_heartbeat_cnt[i]++;
			}
		}

		if(update_all) update_all_data();
		clust_tmr++;
		if(clust_tmr>=15) {
			clust_tmr=0;
			update_cluster_regs();
			update_cluster_bits();
			if(can_addr==0) {
				update_net_regs();
				update_net_bits();
			}
		}

		update_ai_data();
		update_di_data();
		inp_tmr++;
		if(inp_tmr==30000) {
			update_di=0x3FFF;
		}
		else if(inp_tmr>=60000) {
			inp_tmr = 0;
			update_ai=0x3FFF;
		}
		can_write_from_stack();
		if(can_addr==0) can_write_from_stack2();
		osDelay(1);
	}
}

void sendIOName(uint8_t ioNum, uint8_t type, uint8_t req_node, uint8_t req_num) {
	uint8_t name[20]={0};
	uint8_t lngth = 0;
	uint8_t i = 0;
	switch(type) {
		case 0:	// analogue input
			if(ioNum>=1 && ioNum<=14) {
				lngth = strlen(adc_names[ioNum-1]);
				if(lngth>20) lngth = 20;
				for(i=0;i<lngth;i++) name[i] = adc_names[ioNum-1][i];
			}
			break;
		case 1: // digital input
			if(ioNum>=1 && ioNum<=14) {
				lngth = strlen(di_names[ioNum-1]);
				if(lngth>20) lngth = 20;
				for(i=0;i<lngth;i++) name[i] = di_names[ioNum-1][i];
			}
			break;
		case 3: // relay output
			if(ioNum>=1 && ioNum<=6) {
				lngth = strlen(do_names[ioNum-1]);
				if(lngth>20) lngth = 20;
				for(i=0;i<lngth;i++) name[i] = do_names[ioNum-1][i];
			}
			break;
	}

	tx_stack_data packet;
	packet.id = (req_node<<3) | 0x05 | (can_addr<<3) | (cluster_addr << 7);
	packet.length = 8;
	packet.data[0] = (0x03<<5) | 0x0b; // ss-0x05 (fragmented response) eoid-0x0b(read name)
	packet.data[1] = req_num;
	packet.data[2] = 1; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i];
	add_tx_can_packet(&can1_tx_stack,&packet);

	packet.data[1] = req_num;
	packet.data[2] = 2; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i+5];
	add_tx_can_packet(&can1_tx_stack,&packet);

	packet.data[1] = req_num;
	packet.data[2] = 3; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i+10];
	add_tx_can_packet(&can1_tx_stack,&packet);

	packet.data[1] = req_num;
	packet.data[2] = 4; // part num
	for(i=0;i<5;i++) packet.data[3+i] = name[i+15];
	add_tx_can_packet(&can1_tx_stack,&packet);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	uint8_t srv = 0;
	uint8_t node = 0;
	uint8_t ss = 0;
	uint8_t eoid = 0;
	uint8_t dir = 0;
	uint8_t addr = 0x00;
	if(hcan==&hcan1) {
		if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0)) {
			if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
				can1_tmr = 0;
				//HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
				srv = RxHeader.StdId & 0x07;
				node = (RxHeader.StdId>>3) & 0x0F;
				eoid = RxData[0] & 0x1F;
				ss = (RxData[0] >> 5) & 0x07;
				dir = (RxData[0] >> 10) & 0x01;
				addr = RxData[1];
				if(dir==0x00 && node==can_addr && ss==0x01 && eoid==0x0b) { // read name request
					uint8_t num = RxData[2];
					uint8_t type = RxData[4];
					uint8_t req_node = RxData[5];
					sendIOName(num,type,req_node,addr);
				}
				if(srv==0x01) { // heartbeat
					if(node<8) {
						if(heartbeat_cnt[node]==HEARTBEAT_MAX) {update_all = 1;update_tmr = 0;}
						heartbeat_cnt[node]=0;
					}
				}
				if(eoid==0x06) {	// global integer values
					if(addr>=17 && addr<=80) {
						cluster_regs[addr-17] = RxData[2] | (((uint16_t)RxData[3])<<8);
						prev_cluster_regs[addr-17] = cluster_regs[addr-17];
					}
				}else if(eoid==0x03) {	// packed deduced digitals
					uint8_t i=0;
					for(i=0;i<8;i++) {
						if(RxData[3]&(1<<i)) { // check mask
							if( ((addr+i) >= 16) && ((addr+i) <= 239)) { // check address
								if(RxData[2]&(1<<i)) cluster_bits[addr+i-16]=1;
								else cluster_bits[addr+i-16]=0;
								prev_cluster_bits[addr+i-16] = cluster_bits[addr+i-16];
							}
						}
					}
				}else if(eoid==0x0c) {	// cluster status
					cluster_status = addr;
				}else if(eoid==0x0f) {	// network integer
					if(addr>=1 && addr<=128) {
						net_regs[addr-1] = RxData2[2] | (((uint16_t)RxData2[3])<<8);
					}
				}else if(eoid==0x0e) {	// networked packed deduced digitals
					for(uint8_t i=0;i<8;i++) {
						if(RxData2[3]&(1<<i)) { // check mask
							if(addr>=1 && ((addr+i) <= 128)) { // check address
								if(RxData2[2]&(1<<i)) net_bits[addr-1+i]=1;
								else net_bits[addr-1+i]=0;
							}
						}
					}
				}
			}
		}
	}else {
		if(HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO0)) {
			if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader2, RxData2) == HAL_OK) {
				tx_stack_data packet;
				srv = RxHeader2.StdId & 0x07;
				node = (RxHeader2.StdId>>3) & 0x0F;
				eoid = RxData2[0] & 0x1F;
				//ss = (RxData2[0] >> 5) & 0x07;
				//dir = (RxData2[0] >> 10) & 0x01;
				addr = RxData2[1];
				uint8_t net_num = (RxHeader2.StdId>>7) & 0x07;
				if(node==0) {
					if(srv==0x01) { // heartbeat
						if(net_num<8) {
							if(net_heartbeat_cnt[net_num]==HEARTBEAT_MAX) { // обнаружен новый узел
								// спровоцировать отсылку ненулевых данных
								for(uint8_t i=0;i<16;i++) {
									if(net_bits_tx[i]) prev_net_bits_tx[i]=0;
									if(net_regs_tx[i]) prev_net_regs_tx[i]=0;
								}
							}
							net_heartbeat_cnt[net_num]=0;
						}
					}
					if(eoid==0x0f) {	// network integer
						if(addr>=1 && addr<=128) {
							net_regs[addr-1] = RxData2[2] | (((uint16_t)RxData2[3])<<8);
							// send data inside the cluster
							packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
							packet.length = 4;
							packet.data[0] = 0x0F; // network integer values
							packet.data[1] = addr; // reg num
							packet.data[2] = RxData2[2]; // value low byte
							packet.data[3] = RxData2[3]; // value high byte
							add_tx_can_packet(&can1_tx_stack,&packet);
						}
					}else if(eoid==0x0e) {	// network packed deduced digitals
						uint8_t i=0;
						for(i=0;i<8;i++) {
							if(RxData2[3]&(1<<i)) { // check mask
								if(addr>=1 && ((addr+i) <= 128)) { // check address
									if(RxData2[2]&(1<<i)) net_bits[addr-1+i]=1;
									else net_bits[addr-1+i]=0;
								}
							}
						}
						packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
						packet.length = 4;
						packet.data[0] = 0x0E; // network packed deduced digitals
						packet.data[1] = addr; // bit num
						packet.data[2] = RxData2[2]; // value
						packet.data[3] = RxData2[3]; // mask
						add_tx_can_packet(&can1_tx_stack,&packet);
					}
				}
			}
		}
	}
}
