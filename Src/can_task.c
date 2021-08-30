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
#include "leds.h"
#include "update_plc_data.h"
#include "heartbeat.h"
#include "can_protocol.h"
#include "ai_module.h"
#include "do_module.h"
#include "rs_module.h"
#include "ai_calculate.h"
#include "modules.h"
#include "eeprom.h"

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

uint8_t update_all = 1;
extern uint16_t update_tmr;

extern tx_stack can1_tx_stack;
extern tx_stack can2_tx_stack;

extern uint8_t can_addr;
uint8_t cluster_addr=0;

extern uint16_t cluster_regs[64];
extern uint16_t prev_cluster_regs[64];
extern uint8_t cluster_bits[224];
extern uint8_t prev_cluster_bits[224];
extern uint8_t net_bits[128];
extern uint8_t net_bits_tx[16];
extern uint8_t prev_net_bits_tx[16];
extern uint16_t net_regs[128];
extern uint16_t net_regs_tx[16];
extern uint16_t prev_net_regs_tx[16];
static uint8_t cluster_status = 0;
extern uint16_t ai_mod_cnt;
extern struct ai_mod* ai_modules_ptr;
extern uint16_t do_mod_cnt;
extern struct do_mod* do_modules_ptr;
extern uint16_t rs_mod_cnt;
extern struct rs_mod* rs_modules_ptr;
volatile uint8_t can1_tx_tmr = 0;

extern struct led_state can1_led_rx;
extern struct led_state can1_led_tx;
extern struct led_state can2_led_rx;
extern struct led_state can2_led_tx;

extern uint8_t ob_time_upd;
extern uint8_t answer_time[4];

extern uint16_t VirtAddVarTab[NB_OF_VAR];
extern uint8_t ip_addr[4];

void init_can_addr_pins() {

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

uint8_t read_can_addr() {
	can_addr = 0;
	cluster_addr = 0;

	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9)==GPIO_PIN_RESET) can_addr |= 0x01;
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8)==GPIO_PIN_RESET) can_addr |= 0x02;
	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_8)==GPIO_PIN_RESET) can_addr |= 0x04;

	if(HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_7)==GPIO_PIN_RESET) {
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
	if(can1_tx_tmr<CAN_TX_TMR_GAP) return;
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)!=0) {
		try++;
		if(try>=10) return;
		if(get_tx_can_packet(&can1_tx_stack,&packet)) {
			can1_tx_tmr = 0;
			if(packet.length>8) continue;
			TxHeader.StdId = packet.id;
			TxHeader.ExtId = 0;
			TxHeader.RTR = CAN_RTR_DATA;
			TxHeader.IDE = CAN_ID_STD;
			TxHeader.TransmitGlobalTime = DISABLE;
			TxHeader.DLC = packet.length;
			for(i=0;i<packet.length;++i) TxData[i] = packet.data[i];
			HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
			can1_led_tx.on_cmd = 1;
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
			can2_led_tx.on_cmd = 1;
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

void canTask(void const * argument) {
	initCANFilter();
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	if(can_addr==0) {
		HAL_CAN_Start(&hcan2);
		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
	}

	init_modules();
	osDelay(500);
	// запрос текущих данных аналоговых модулей
	for(uint8_t i=0;i<ai_mod_cnt;i++) {
		sendReqDataFromMod(&can1_tx_stack, ai_modules_ptr[i].addr);
		can_write_from_stack();
		osDelay(10);
	}

	for(;;) {
		send_heartbeat();
		increment_others_heartbeats_counters();
		increment_modules_heartbeats_counters();
		if(update_all) update_all_data();
		if(ob_time_upd) {
			ob_time_upd = 0;
			send_time(answer_time);
		}
		send_changed_data();
		for(uint8_t i=0;i<rs_mod_cnt;++i) {
			rs_module_write_config(&rs_modules_ptr[i],&can1_tx_stack);
		}
		can_write_from_stack();
		if(can_addr==0) can_write_from_stack2();
		osDelay(1);
	}
}

static void handle_can1_extended_request(struct can_packet *rx_packet) {
	struct can_packet_ext_id *can_id = (struct can_packet_ext_id *)(&rx_packet->id);
	uint8_t mod_addr = rx_packet->data[0];
	uint8_t cmd = rx_packet->data[1];
	if(mod_addr&0x80) mod_addr&=0x0F;
	else mod_addr&=0x7F;
	if(can_id->req==RequireAnswer) sendResponse(&can1_tx_stack, rx_packet->data[CAN_EXT_HEADER_LENGTH], cmd);
	if(can_id->mod_type==Ext_AI_Mod) {
		for(uint8_t i=0;i<ai_mod_cnt;++i) {
			if(ai_modules_ptr[i].addr == mod_addr) {
				switch(cmd) {
					case ExtHeartbeat:
						if(ai_modules_ptr[i].link_state==0) {
							// update new detected module configuration
							sendByteWrite(&can1_tx_stack, mod_addr, 0, get_input_types(&ai_modules_ptr[i]), Ext_AI_Mod);
						}
						ai_modules_ptr[i].heartbeat_cnt = 0;
						ai_modules_ptr[i].link_state = 1;
						break;
					case ExtAIState:{
						uint8_t ai_num = rx_packet->data[CAN_EXT_HEADER_LENGTH];
						if(ai_num>=1 && ai_num<=MOD_AI_INP_CNT) {
							uint16_t val = rx_packet->data[CAN_EXT_HEADER_LENGTH+1] |
									(((uint16_t)rx_packet->data[CAN_EXT_HEADER_LENGTH+2])<<8);
							handle_adc(val,ai_modules_ptr[i].sens_type[ai_num-1],&ai_modules_ptr[i],ai_num-1);
						}}
						break;
				}
				break;
			}
		}
	}else if(can_id->mod_type==Ext_DO_Mod) {
		for(uint8_t i=0;i<do_mod_cnt;++i) {
			if(do_modules_ptr[i].addr == mod_addr) {
				uint8_t do_value = 0;
				switch(cmd) {
					case ExtHeartbeat:
						if(do_modules_ptr[i].link_state==0) do_modules_ptr[i].update_data = 1;
						do_modules_ptr[i].heartbeat_cnt = 0;
						do_modules_ptr[i].link_state = 1;
						// контроль текущего состояния выходов модуля
						if(do_modules_ptr[i].do_state[0]) do_value|=0x01;
						if(do_modules_ptr[i].do_state[1]) do_value|=0x02;
						if(do_modules_ptr[i].do_state[2]) do_value|=0x04;
						if(do_modules_ptr[i].do_state[3]) do_value|=0x08;
						if(do_value != (rx_packet->data[CAN_EXT_HEADER_LENGTH+1]&0x0F)) {
							sendOutState(&can1_tx_stack,do_modules_ptr[i].addr,do_value);
						}
						break;
					case ExtDOState:
						for(uint8_t j=0;j<MOD_DO_OUT_CNT;++j) {
							if(rx_packet->data[CAN_EXT_HEADER_LENGTH+2] & (1<<j)) do_modules_ptr[i].do_err[j] = 1;
							else do_modules_ptr[i].do_err[j] = 0;
						}
						break;
				}
				break;
			}
		}
	}else if(can_id->mod_type==Ext_RS_Mod) {
		for(uint8_t i=0;i<rs_mod_cnt;++i) {
			if(rs_modules_ptr[i].addr == mod_addr) {
				switch(cmd) {
					case ExtHeartbeat:
						// отправка конфигурации в случае появления модуля в сети
						if(rs_modules_ptr[i].link_state==0) rs_modules_ptr[i].config.write_state = RS_START_WR_CONFIG;
						if(rx_packet->data[CAN_EXT_HEADER_LENGTH+1]==0) {
							if(rs_modules_ptr[i].config.write_state == RS_WAIT_CMD)	rs_modules_ptr[i].config.write_state = RS_START_WR_CONFIG;
						}
						rs_modules_ptr[i].heartbeat_cnt = 0;
						rs_modules_ptr[i].link_state = 1;
						break;
					case ExtResponse:
						if(rx_packet->data[CAN_EXT_HEADER_LENGTH+1]==ExtWriteConf) {
							rs_modules_ptr[i].config.rcv_id_value = rx_packet->data[CAN_EXT_HEADER_LENGTH];
							rs_modules_ptr[i].config.rcv_id_flag = 1;
						}
						break;
					case ExtRegsStateWithValidation:
						if(rx_packet->length>=CAN_EXT_HEADER_LENGTH+2) {
							uint16_t reg_addr = (uint16_t)rx_packet->data[CAN_EXT_HEADER_LENGTH]<<8;
							reg_addr |= rx_packet->data[CAN_EXT_HEADER_LENGTH+1];
							uint8_t reg_cnt = (rx_packet->length-CAN_EXT_HEADER_LENGTH-2)/2;
							if(reg_cnt<=2) {
								for(uint8_t j=0;j<reg_cnt;j++) {
									if(reg_addr+j<RS_DEVICE_CNT/2) {
										// состояние устройств
										rs_modules_ptr[i].dev_state[(reg_addr+j)*2] = rx_packet->data[CAN_EXT_HEADER_LENGTH+2+j*2];
										rs_modules_ptr[i].dev_state[(reg_addr+j)*2+1] = rx_packet->data[CAN_EXT_HEADER_LENGTH+2+j*2+1];
									}
									else {
										// состояние регистров
										uint16_t value = (uint16_t)rx_packet->data[CAN_EXT_HEADER_LENGTH+2+j*2]<<8;
										value |= rx_packet->data[CAN_EXT_HEADER_LENGTH+2+j*2+1];
										if(reg_addr-RS_DEVICE_CNT/2+j<RS_REG_CNT) {
											rs_modules_ptr[i].data[reg_addr-RS_DEVICE_CNT/2+j] = value;
										}
									}
								}

								// send feedback
								tx_stack_data tx_packet;
								tx_packet.id = 0;
								struct can_packet_ext_id *tx_can_id = (struct can_packet_ext_id *)&tx_packet.id;
								tx_can_id->mod_type = Ext_PC21;
								tx_can_id->req = NoAnswer;
								tx_can_id->dir = ToNodeReq;
								tx_can_id->srv = SRV_Channel;
								tx_packet.data[0] = mod_addr;
								tx_packet.data[1] = ExtRegsValidation;
								uint8_t byte_cnt = rx_packet->length-CAN_EXT_HEADER_LENGTH;
								if(byte_cnt>6) byte_cnt = 6;
								for(uint8_t j=0;j<byte_cnt;j++) {
									tx_packet.data[2+j] = rx_packet->data[CAN_EXT_HEADER_LENGTH+j];
								}
								tx_packet.length = CAN_EXT_HEADER_LENGTH + byte_cnt;
								add_tx_can_packet(&can1_tx_stack,&tx_packet);
							}
						}
						break;
				}
			}
		}
	}
}

static void handle_can1_stand_request(struct can_packet *rx_packet) {
	static uint8_t tmp_ip[4] = {0,0,0,0};
	struct can_packet_id *can_id = (struct can_packet_id *)(&rx_packet->id);
	enum SS_type ss = get_ss(rx_packet);
	uint8_t eoid = get_eoid(rx_packet);
	uint8_t intern_addr = get_intern_addr(rx_packet);
	rx_packet->length-=CAN_HEADER_LENGTH;

	if(can_id->node_addr==can_addr && ss==SS_Req_fragm && eoid==0x0B) {
		uint8_t point_num = rx_packet->data[CAN_HEADER_LENGTH];
		uint8_t io_type = rx_packet->data[CAN_HEADER_LENGTH+2];
		uint8_t req_node = rx_packet->data[CAN_HEADER_LENGTH+3];
		sendIOName(point_num,io_type,req_node,intern_addr);
	}

	if(can_id->srv==SRV_Heartbeat) {
		if(can_id->node_addr<8) {
			if(is_internal_node_offline(can_id->node_addr)) { // обнаружен новый узел
				// скинуть все данные текущего узла в сеть
				update_all = 1;
				update_tmr = 0;
			}
			clear_internal_heartbeat(can_id->node_addr);
		}
	}
	switch(eoid) {
		case 0x06:	// global integer values
			if(intern_addr>=17 && intern_addr<=80) {
				cluster_regs[intern_addr-17] = rx_packet->data[CAN_HEADER_LENGTH] |
						(((uint16_t)rx_packet->data[CAN_HEADER_LENGTH+1])<<8);
				prev_cluster_regs[intern_addr-17] = cluster_regs[intern_addr-17];
			}
			break;
		case 0x03:	// packed deduced digitals
			for(uint8_t i=0;i<8;i++) {
				if(rx_packet->data[CAN_HEADER_LENGTH+1]&(1<<i)) { // check mask
					if( ((intern_addr+i) >= 16) && ((intern_addr+i) <= 239)) { // check address
						if(rx_packet->data[CAN_HEADER_LENGTH]&(1<<i)) cluster_bits[intern_addr+i-16]=1;
						else cluster_bits[intern_addr+i-16]=0;
						prev_cluster_bits[intern_addr+i-16] = cluster_bits[intern_addr+i-16];
					}
				}
			}
			break;
		case 0x0C:	// cluster status
			cluster_status = intern_addr;
			break;
		case 0x0F:	// network integer
			if(intern_addr>=1 && intern_addr<=128) {
				net_regs[intern_addr-1] = rx_packet->data[CAN_HEADER_LENGTH] |
						(((uint16_t)rx_packet->data[CAN_HEADER_LENGTH+1])<<8);
			}
			break;
		case 0x0E:	// networked packed deduced digitals
			for(uint8_t i=0;i<8;i++) {
				if(rx_packet->data[CAN_HEADER_LENGTH+1]&(1<<i)) { // check mask
					if(intern_addr>=1 && ((intern_addr+i) <= 128)) { // check address
						if(rx_packet->data[CAN_HEADER_LENGTH]&(1<<i)) net_bits[intern_addr-1+i]=1;
						else net_bits[intern_addr-1+i]=0;
					}
				}
			}
			break;
		case 0x1F:
			if(intern_addr==16) {
				if(rx_packet->data[CAN_HEADER_LENGTH+4]==0) { // fixed ip
					tmp_ip[0] = rx_packet->data[CAN_HEADER_LENGTH+3];
					tmp_ip[1] = rx_packet->data[CAN_HEADER_LENGTH+2];
					tmp_ip[2] = rx_packet->data[CAN_HEADER_LENGTH+1];
					tmp_ip[3] = rx_packet->data[CAN_HEADER_LENGTH+0];
					if(tmp_ip[0]!=ip_addr[0] || tmp_ip[1]!=ip_addr[1] || tmp_ip[2]!=ip_addr[2] || tmp_ip[3]!=ip_addr[3]) {
						ip_addr[0] = tmp_ip[0];
						ip_addr[1] = tmp_ip[1];
						ip_addr[2] = tmp_ip[2];
						ip_addr[3] = tmp_ip[3];
						EE_WriteVariable(VirtAddVarTab[3],((uint16_t)ip_addr[0]<<8) | ip_addr[1]);
						EE_WriteVariable(VirtAddVarTab[4],((uint16_t)ip_addr[2]<<8) | ip_addr[3]);
						HAL_Delay(50);
						NVIC_SystemReset();
					}
				}
			}
			break;
	}
}

static void handle_can1() {
	static struct can_packet rx_packet;
	if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0)) {
		if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
			can1_tmr = 0;
			can1_led_rx.on_cmd = 1;
			rx_packet.id = RxHeader.StdId & 0xFFFF;
			rx_packet.length = RxHeader.DLC & 0xFF;
			memcpy(rx_packet.data,RxData,CAN_DATA_SIZE);
			memset(RxData,0,CAN_DATA_SIZE);
			can1_tmr = 0;
			if(is_packet_extended(RxHeader.StdId)) {
				handle_can1_extended_request(&rx_packet);
			}else {
				handle_can1_stand_request(&rx_packet);
			}
		}
	}
}

static void handle_can2() {
	static struct can_packet rx_packet2;
	if(HAL_CAN_GetRxFifoFillLevel(&hcan2, CAN_RX_FIFO0)) {
		if(HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader2, RxData2) == HAL_OK) {
			can2_led_rx.on_cmd = 1;
			tx_stack_data packet;
			rx_packet2.id = RxHeader2.StdId & 0xFFFF;
			rx_packet2.length = RxHeader2.DLC & 0xFF;
			memcpy(rx_packet2.data,RxData2,CAN_DATA_SIZE);
			memset(RxData2,0,CAN_DATA_SIZE);
			if(is_packet_extended(RxHeader2.StdId)) {

			}else {
				struct can_packet_id *can_id = (struct can_packet_id *)(&RxHeader2.StdId);
				__attribute__((unused)) enum SS_type ss = get_ss(&rx_packet2);
				uint8_t eoid = get_eoid(&rx_packet2);
				uint8_t intern_addr = get_intern_addr(&rx_packet2);
				rx_packet2.length-=CAN_HEADER_LENGTH;
				if(can_id->node_addr==0) {
					if(can_id->srv==SRV_Heartbeat) {
						if(can_id->clust_addr<8) {
							if(is_external_node_offline(can_id->clust_addr)) { // обнаружен новый узел
								// спровоцировать отсылку ненулевых данных
								for(uint8_t i=0;i<16;i++) {
									if(net_bits_tx[i]) prev_net_bits_tx[i]=0;
									if(net_regs_tx[i]) prev_net_regs_tx[i]=0;
								}
							}
							clear_external_heartbeat(can_id->clust_addr);
						}
					}
				}
				switch(eoid) {
					case 0x0F:	// network integer
						if(intern_addr>=1 && intern_addr<=128) {
							net_regs[intern_addr-1] = rx_packet2.data[CAN_HEADER_LENGTH] |
									(((uint16_t)rx_packet2.data[CAN_HEADER_LENGTH+1])<<8);
							// send data inside the cluster
							packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
							packet.length = 4;
							packet.data[0] = 0x0F; // network integer values
							packet.data[1] = intern_addr; // reg num
							packet.data[2] = rx_packet2.data[CAN_HEADER_LENGTH]; // value low byte
							packet.data[3] = rx_packet2.data[CAN_HEADER_LENGTH+1]; // value high byte
							add_tx_can_packet(&can1_tx_stack,&packet);
						}
						break;
					case 0x0E:
						for(uint8_t i=0;i<8;i++) {
							if(rx_packet2.data[CAN_HEADER_LENGTH+1]&(1<<i)) { // check mask
								if(intern_addr>=1 && ((intern_addr+i) <= 128)) { // check address
									if(rx_packet2.data[CAN_HEADER_LENGTH]&(1<<i)) net_bits[intern_addr-1+i]=1;
									else net_bits[intern_addr-1+i]=0;
								}
							}
						}
						packet.id = 0x0400 | 0x07 | (can_addr<<3) | (cluster_addr << 7);	// event
						packet.length = 4;
						packet.data[0] = 0x0E; // network packed deduced digitals
						packet.data[1] = intern_addr; // bit num
						packet.data[2] = rx_packet2.data[CAN_HEADER_LENGTH]; // value
						packet.data[3] = rx_packet2.data[CAN_HEADER_LENGTH+1]; // mask
						add_tx_can_packet(&can1_tx_stack,&packet);
						break;
				}
			}
		}
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan==&hcan1) handle_can1();
	else handle_can2();
}
