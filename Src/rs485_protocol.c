/*
 * rs485_protocol.c
 *
 *  Created on: 14 нояб. 2019 г.
 *      Author: User
 */

#include "rs485_protocol.h"
#include "rs485.h"
#include <string.h>
#include "main.h"
#include "crc.h"
#include "eeprom.h"
#include "os_conf.h"
#include "modbus.h"

static uint8_t tmp_rx1_buf[UART_BUF_SISE];
static uint8_t tmp_rx2_buf[UART_BUF_SISE];
static uint8_t tx1_buf[UART_BUF_SISE];
static uint8_t tx2_buf[UART_BUF_SISE];
extern uint16_t VirtAddVarTab[NB_OF_VAR];

uint8_t net_address = 0x01;

extern unsigned short ain[AI_CNT];
extern unsigned short ain_raw[AI_CNT];
extern unsigned short ireg[IREG_CNT];
extern uint16_t ai_type;

extern uint16_t di_state_reg;
extern uint16_t di_sh_circ_reg;
extern uint16_t di_break_reg;
extern uint16_t di_fault_reg;
extern uint8_t din[DI_CNT];
extern uint8_t din_break[DI_CNT];
extern uint8_t din_short_circuit[DI_CNT];
extern uint8_t din_fault[DI_CNT];

extern uint16_t do_reg;
extern unsigned char dout[DO_CNT];

extern unsigned char ibit[IBIT_CNT];

extern uint8_t ip_addr[4];
extern uint8_t ip_mask[4];
extern uint8_t ip_gate[4];
extern uint16_t rs485_conf1;
extern uint16_t rs485_conf2;

extern uint16_t app_id;
extern uint8_t can_addr;
extern unsigned short plc_cycle;

static void modbus_error(unsigned char func, unsigned char code, uint8_t * tx_ptr, void (*send)(uint8_t*,uint16_t)) {
	unsigned short crc=0;
	tx_ptr[0] = net_address;
	tx_ptr[1] = func | 0x80;
	tx_ptr[2] = code;
	crc = GetCRC16((unsigned char*)tx_ptr,3);
	tx_ptr[3]=crc>>8;
	tx_ptr[4]=crc&0xFF;
	send(tx_ptr,5);
}

void rx_callback(uint8_t* rx_ptr,uint16_t rx_cnt, uint8_t * tx_ptr, void (*send)(uint8_t*,uint16_t)) {
	uint16_t crc=0;
	uint16_t cnt=0;
	uint16_t value = 0;
	uint16_t mem_addr = 0;
	uint16_t tmp=0;
	uint16_t byte_count = 0;
	if((rx_cnt>=4) && ((rx_ptr[0]==net_address) || (rx_ptr[0]==0x00)) && (GetCRC16(rx_ptr,rx_cnt)==0)) {
		switch(rx_ptr[1]) {
			case 0xA0:
				tx_ptr[0] = rx_ptr[0];
				tx_ptr[1] = 0xA0;
				// request id
				tx_ptr[2] = rx_ptr[2];
				tx_ptr[3] = rx_ptr[3];
				// device id
				tx_ptr[4] = 0x00;
				tx_ptr[5] = 0x00;
				tx_ptr[6] = 0x00;
				tx_ptr[7] = 0x04;

				crc = GetCRC16((unsigned char*)tx_ptr,8);
				tx_ptr[8]=crc>>8;
				tx_ptr[9]=crc&0xFF;
				send(tx_ptr,10);
				break;
			case 0xEB:	// вернуть число секторов для стирания
				tx_ptr[0] = rx_ptr[0];
				tx_ptr[1] = 0xEB;
				// request id
				tx_ptr[2] = rx_ptr[2];
				tx_ptr[3] = rx_ptr[3];
				cnt = rx_ptr[4];
				cnt = cnt<<8; cnt|=rx_ptr[5]; // размер программы в килобайтах
				if(cnt%128==0) {
					cnt=cnt/128;
					tx_ptr[4] = cnt>>8;
					tx_ptr[5] = cnt & 0xFF;
					crc = GetCRC16((unsigned char*)tx_ptr,6);
					tx_ptr[6]=crc>>8;
					tx_ptr[7]=crc&0xFF;
					send(tx_ptr,8);
				}
				break;
			case 0xEC:// restart in boot mode
				tx_ptr[0] = rx_ptr[0];
				tx_ptr[1] = 0xEC;
				// request id
				tx_ptr[2] = rx_ptr[2];
				tx_ptr[3] = rx_ptr[3];
				crc = GetCRC16((unsigned char*)tx_ptr,4);
				tx_ptr[4]=crc>>8;
				tx_ptr[5]=crc&0xFF;
				send(tx_ptr,6);
				EE_WriteVariable(VirtAddVarTab[1],0);
				HAL_Delay(50);
				NVIC_SystemReset();
				break;
			case READ_COILS:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(cnt>COIL_COUNT || cnt==0) {modbus_error(READ_COILS,0x03,tx_ptr,send);break;}
				if(mem_addr+cnt>COIL_COUNT) {modbus_error(READ_COILS,0x02,tx_ptr,send);break;}
				byte_count = cnt>>3;
				if(cnt!=(byte_count<<3)) byte_count++;
				for(tmp=0;tmp<byte_count;tmp++) tx_ptr[3+tmp] = 0;
				for(tmp=0;tmp<cnt;tmp++) {
					if(mem_addr+tmp<DO_CNT) {if(dout[mem_addr+tmp]) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
					else if(mem_addr+tmp<DO_CNT + IBIT_CNT) {if(ibit[mem_addr+tmp-DO_CNT]) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
					else if(mem_addr+tmp<DO_CNT + IBIT_CNT + DI_CNT) {if(din[mem_addr+tmp-DO_CNT-IBIT_CNT]) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
					else if(mem_addr+tmp<DO_CNT + IBIT_CNT + DI_CNT + AI_CNT) {if(ai_type & ((uint16_t)1<<(mem_addr+tmp-DO_CNT-IBIT_CNT-DI_CNT)) ) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
				}
				tx_ptr[0] = rx_ptr[0];
				tx_ptr[1] = READ_COILS;
				tx_ptr[2] = byte_count;
				crc = GetCRC16((unsigned char*)tx_ptr,3+byte_count);
				tx_ptr[3+byte_count]=crc>>8;
				tx_ptr[4+byte_count]=crc&0xFF;
				send(tx_ptr,5+byte_count);
				break;
			case READ_DINPUTS:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(cnt>DINPUTS_COUNT || cnt==0) {modbus_error(READ_DINPUTS,0x03,tx_ptr,send);break;}
				if(mem_addr+cnt>DINPUTS_COUNT) {modbus_error(READ_DINPUTS,0x02,tx_ptr,send);break;}
				byte_count = cnt>>3;
				if(cnt!=(byte_count<<3)) byte_count++;
				for(tmp=0;tmp<byte_count;tmp++) tx_ptr[3+tmp] = 0;
				for(tmp=0;tmp<cnt;tmp++) {
					if(mem_addr+tmp<DI_CNT) {if(din[mem_addr+tmp]) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
					else if(mem_addr+tmp<DI_CNT + DI_CNT) {if(din_short_circuit[mem_addr+tmp-DI_CNT]) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
					else if(mem_addr+tmp<DI_CNT + DI_CNT + DI_CNT) {if(din_break[mem_addr+tmp-DI_CNT-DI_CNT]) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
					else if(mem_addr+tmp<DI_CNT + DI_CNT + DI_CNT + DI_CNT) {if(din_fault[mem_addr+tmp-DI_CNT-DI_CNT-DI_CNT]) tx_ptr[3+(tmp>>3)] |= 1<<(tmp%8);}
				}
				tx_ptr[0] = rx_ptr[0];
				tx_ptr[1] = READ_DINPUTS;
				tx_ptr[2] = byte_count;
				crc = GetCRC16((unsigned char*)tx_ptr,3+byte_count);
				tx_ptr[3+byte_count]=crc>>8;
				tx_ptr[4+byte_count]=crc&0xFF;
				send(tx_ptr,5+byte_count);
				break;
			case READ_INP_REGS:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(cnt>INPR_COUNT || cnt==0) {modbus_error(READ_INP_REGS,0x03,tx_ptr,send);break;}
				if(mem_addr+cnt>INPR_COUNT) {modbus_error(READ_INP_REGS,0x02,tx_ptr,send);break;}

				for(tmp=0;tmp<cnt;tmp++) {
					if(mem_addr+tmp<IREG_CNT) {
						tx_ptr[3+tmp*2] = ireg[mem_addr+tmp]>>8;
						tx_ptr[4+tmp*2] = ireg[mem_addr+tmp]&0xFF;
					}else if(mem_addr+tmp<IREG_CNT + AI_CNT) {
						tx_ptr[3+tmp*2] = ain_raw[mem_addr+tmp-IREG_CNT]>>8;
						tx_ptr[4+tmp*2] = ain_raw[mem_addr+tmp-IREG_CNT]&0xFF;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT) {
						tx_ptr[3+tmp*2] = ai_type>>8;
						tx_ptr[4+tmp*2] = ai_type&0xFF;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT + 1) {
						tx_ptr[3+tmp*2] = di_state_reg>>8;
						tx_ptr[4+tmp*2] = di_state_reg&0xFF;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT + 2) {
						tx_ptr[3+tmp*2] = di_sh_circ_reg>>8;
						tx_ptr[4+tmp*2] = di_sh_circ_reg&0xFF;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT + 3) {
						tx_ptr[3+tmp*2] = di_break_reg>>8;
						tx_ptr[4+tmp*2] = di_break_reg&0xFF;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT + 4) {
						tx_ptr[3+tmp*2] = di_fault_reg>>8;
						tx_ptr[4+tmp*2] = di_fault_reg&0xFF;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT + 5) {
						tx_ptr[3+tmp*2] = app_id>>8;
						tx_ptr[4+tmp*2] = app_id&0xFF;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT + 6) {
						tx_ptr[3+tmp*2] = 0;
						tx_ptr[4+tmp*2] = can_addr;
					}else if(mem_addr+tmp==IREG_CNT + AI_CNT + 7) {
						tx_ptr[3+tmp*2] = plc_cycle>>8;
						tx_ptr[4+tmp*2] = plc_cycle&0xFF;
					}
				}
				tx_ptr[0]=rx_ptr[0];
				tx_ptr[1]=READ_INP_REGS;
				tx_ptr[2]=cnt*2;
				crc=GetCRC16(tx_ptr,3+cnt*2);
				tx_ptr[3+cnt*2]=crc>>8;
				tx_ptr[4+cnt*2]=crc&0xFF;
				send(tx_ptr,5+cnt*2);
				break;
			case READ_HOLD_REGS:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(cnt>HOLDR_COUNT || cnt==0) {modbus_error(READ_HOLD_REGS,0x03,tx_ptr,send);break;}
				if(mem_addr+cnt>HOLDR_COUNT) {modbus_error(READ_HOLD_REGS,0x02,tx_ptr,send);break;}

				for(tmp=0;tmp<cnt;tmp++) {
					switch(mem_addr+tmp) {
					case 0:
						tx_ptr[3+tmp*2] = ai_type>>8;
						tx_ptr[4+tmp*2] = ai_type&0xFF;
						break;
					case 1:
						tx_ptr[3+tmp*2] = 0;
						tx_ptr[4+tmp*2] = net_address;
						break;
					case 2:
						tx_ptr[3+tmp*2] = ip_addr[0];
						tx_ptr[4+tmp*2] = ip_addr[1];
						break;
					case 3:
						tx_ptr[3+tmp*2] = ip_addr[2];
						tx_ptr[4+tmp*2] = ip_addr[3];
						break;
					case 4:
						tx_ptr[3+tmp*2] = ip_mask[0];
						tx_ptr[4+tmp*2] = ip_mask[1];
						break;
					case 5:
						tx_ptr[3+tmp*2] = ip_mask[2];
						tx_ptr[4+tmp*2] = ip_mask[3];
						break;
					case 6:
						tx_ptr[3+tmp*2] = ip_gate[0];
						tx_ptr[4+tmp*2] = ip_gate[1];
						break;
					case 7:
						tx_ptr[3+tmp*2] = ip_gate[2];
						tx_ptr[4+tmp*2] = ip_gate[3];
						break;
					case 8:
						tx_ptr[3+tmp*2] = rs485_conf1>>8;
						tx_ptr[4+tmp*2] = rs485_conf1&0xFF;
						break;
					case 9:
						tx_ptr[3+tmp*2] = rs485_conf2>>8;
						tx_ptr[4+tmp*2] = rs485_conf2&0xFF;
						break;
					default:
						tx_ptr[3+tmp*2] = 0;
						tx_ptr[4+tmp*2] = 0;
						break;
					}
				}
				tx_ptr[0]=rx_ptr[0];
				tx_ptr[1]=READ_HOLD_REGS;
				tx_ptr[2]=cnt*2;
				crc=GetCRC16(tx_ptr,3+cnt*2);
				tx_ptr[3+cnt*2]=crc>>8;
				tx_ptr[4+cnt*2]=crc&0xFF;
				send(tx_ptr,5+cnt*2);
				break;
			case WR_SINGLE_REG:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(mem_addr >= HOLDR_COUNT) {modbus_error(WR_SINGLE_REG,0x02,tx_ptr,send);break;}
				switch(mem_addr) {
					case 0:
						ai_type = cnt;
						EE_WriteVariable(VirtAddVarTab[9],ai_type);
						break;
					case 1:
						net_address = cnt;
						EE_WriteVariable(VirtAddVarTab[2],cnt);
						break;
					case 2:
						ip_addr[0] = cnt>>8;
						ip_addr[1] = cnt & 0xFF;
						EE_WriteVariable(VirtAddVarTab[3],cnt);
						break;
					case 3:
						ip_addr[2] = cnt>>8;
						ip_addr[3] = cnt & 0xFF;
						EE_WriteVariable(VirtAddVarTab[4],cnt);
						break;
					case 4:
						ip_mask[0] = cnt>>8;
						ip_mask[1] = cnt & 0xFF;
						EE_WriteVariable(VirtAddVarTab[5],cnt);
						break;
					case 5:
						ip_mask[2] = cnt>>8;
						ip_mask[3] = cnt & 0xFF;
						EE_WriteVariable(VirtAddVarTab[6],cnt);
						break;
					case 6:
						ip_gate[0] = cnt>>8;
						ip_gate[1] = cnt & 0xFF;
						EE_WriteVariable(VirtAddVarTab[7],cnt);
						break;
					case 7:
						ip_gate[2] = cnt>>8;
						ip_gate[3] = cnt & 0xFF;
						EE_WriteVariable(VirtAddVarTab[8],cnt);
						break;
					case 8:
						rs485_conf1 = cnt;
						EE_WriteVariable(VirtAddVarTab[10],rs485_conf1);
						break;
					case 9:
						rs485_conf2 = cnt;
						EE_WriteVariable(VirtAddVarTab[11],rs485_conf2);
						break;
				}

				tx_ptr[0]=rx_ptr[0];
				tx_ptr[1]=WR_SINGLE_REG;
				tx_ptr[2]=mem_addr>>8;
				tx_ptr[3]=mem_addr&0xFF;
				tx_ptr[4]=cnt>>8;
				tx_ptr[5]=cnt&0xFF;
				crc=GetCRC16(tx_ptr,6);
				tx_ptr[6]=crc>>8;
				tx_ptr[7]=crc&0xFF;
				send(tx_ptr,8);
				break;
			case WR_MULTI_REGS:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(cnt>=128 || cnt==0) {modbus_error(WR_MULTI_REGS,0x03,tx_ptr,send);break;}
				if(mem_addr+cnt>HOLDR_COUNT) {modbus_error(WR_MULTI_REGS,0x02,tx_ptr,send);break;}
				for(tmp=0;tmp<cnt;tmp++) {
					value = ((unsigned short)rx_ptr[7+tmp*2]<<8) | rx_ptr[8+tmp*2];
					switch(mem_addr+tmp) {
						case 0:
							ai_type = value;
							EE_WriteVariable(VirtAddVarTab[9],ai_type);
							break;
						case 1:
							net_address = rx_ptr[8+tmp*2] ;
							EE_WriteVariable(VirtAddVarTab[2],value);
							break;
						case 2:
							ip_addr[0] = rx_ptr[7+tmp*2];
							ip_addr[1] = rx_ptr[8+tmp*2];
							EE_WriteVariable(VirtAddVarTab[3],value);
							break;
						case 3:
							ip_addr[2] = rx_ptr[7+tmp*2];
							ip_addr[3] = rx_ptr[8+tmp*2];
							EE_WriteVariable(VirtAddVarTab[4],value);
							break;
						case 4:
							ip_mask[0] = rx_ptr[7+tmp*2];
							ip_mask[1] = rx_ptr[8+tmp*2];
							EE_WriteVariable(VirtAddVarTab[5],value);
							break;
						case 5:
							ip_mask[2] = rx_ptr[7+tmp*2];
							ip_mask[3] = rx_ptr[8+tmp*2];
							EE_WriteVariable(VirtAddVarTab[6],value);
							break;
						case 6:
							ip_gate[0] = rx_ptr[7+tmp*2];
							ip_gate[1] = rx_ptr[8+tmp*2];
							EE_WriteVariable(VirtAddVarTab[7],value);
							break;
						case 7:
							ip_gate[2] = rx_ptr[7+tmp*2];
							ip_gate[3] = rx_ptr[8+tmp*2];
							EE_WriteVariable(VirtAddVarTab[8],value);
							break;
						case 8:
							rs485_conf1 = value;
							EE_WriteVariable(VirtAddVarTab[10],rs485_conf1);
							break;
						case 9:
							rs485_conf2 = value;
							EE_WriteVariable(VirtAddVarTab[11],rs485_conf2);
							break;
					}
				}
				tx_ptr[0]=rx_ptr[0];
				tx_ptr[1]=WR_MULTI_REGS;
				tx_ptr[2]=mem_addr>>8;
				tx_ptr[3]=mem_addr&0xFF;
				tx_ptr[4]=cnt>>8;
				tx_ptr[5]=cnt&0xFF;
				crc=GetCRC16(tx_ptr,6);
				tx_ptr[6]=crc>>8;
				tx_ptr[7]=crc&0xFF;
				send(tx_ptr,8);
				break;
			case WR_SINGLE_COIL:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(cnt&&(cnt!=0xFF00)) {modbus_error(WR_SINGLE_COIL,0x03,tx_ptr,send);break;}
				if(mem_addr>=COIL_COUNT) {modbus_error(WR_SINGLE_COIL,0x02,tx_ptr,send);break;}
				if(mem_addr<DO_CNT) {if(cnt) dout[mem_addr]=1;else dout[mem_addr]=0;}
				else if(mem_addr<DO_CNT + IBIT_CNT) {if(cnt) ibit[mem_addr-DO_CNT]=1; else ibit[mem_addr-DO_CNT]=0;}
				else if(mem_addr<DO_CNT + IBIT_CNT + DI_CNT) {if(cnt) din[mem_addr-DO_CNT-IBIT_CNT]=1; else din[mem_addr-DO_CNT-IBIT_CNT]=0;}
				else if(mem_addr<DO_CNT + IBIT_CNT + DI_CNT + AI_CNT) {
					if(cnt) ai_type |= ((uint16_t)1<<(mem_addr-DO_CNT-IBIT_CNT-DI_CNT));
					else ai_type &= ~((uint16_t)1<<(mem_addr-DO_CNT-IBIT_CNT-DI_CNT));
					EE_WriteVariable(VirtAddVarTab[9],ai_type);
				}
				tx_ptr[0]=rx_ptr[0];
				tx_ptr[1]=WR_SINGLE_COIL;
				tx_ptr[2]=mem_addr>>8;
				tx_ptr[3]=mem_addr&0xFF;
				tx_ptr[4]=cnt>>8;
				tx_ptr[5]=cnt&0xFF;
				crc=GetCRC16(tx_ptr,6);
				tx_ptr[6]=crc>>8;
				tx_ptr[7]=crc&0xFF;
				send(tx_ptr,8);
				break;
			case WR_MULTI_COIL:
				mem_addr = ((unsigned short)rx_ptr[2]<<8) | rx_ptr[3];
				cnt = ((unsigned short)rx_ptr[4]<<8) | rx_ptr[5];
				if(cnt>COIL_COUNT || cnt==0) {modbus_error(WR_MULTI_COIL,0x03,tx_ptr,send);break;}
				if(mem_addr+cnt>COIL_COUNT) {modbus_error(WR_MULTI_COIL,0x02,tx_ptr,send);break;}
				for(tmp=0;tmp<cnt;tmp++) {
					if(mem_addr+tmp<DO_CNT) {
						if((rx_ptr[7+(tmp>>3)])&(1<<(tmp%8))) dout[mem_addr+tmp]=1;else dout[mem_addr+tmp]=0;
					}else if(mem_addr+tmp<DO_CNT + IBIT_CNT) {
						if((rx_ptr[7+(tmp>>3)])&(1<<(tmp%8))) ibit[mem_addr+tmp-DO_CNT]=1; else ibit[mem_addr+tmp-DO_CNT]=0;
					}else if(mem_addr+tmp<DO_CNT + IBIT_CNT + DI_CNT) {
						if((rx_ptr[7+(tmp>>3)])&(1<<(tmp%8))) din[mem_addr+tmp-DO_CNT-IBIT_CNT]=1; else din[mem_addr+tmp-DO_CNT-IBIT_CNT]=0;
					}else if(mem_addr+tmp<DO_CNT + IBIT_CNT + DI_CNT + AI_CNT) {
						if((rx_ptr[7+(tmp>>3)])&(1<<(tmp%8))) ai_type |= ((uint16_t)1<<(mem_addr+tmp-DO_CNT-IBIT_CNT-DI_CNT));
						else ai_type &= ~((uint16_t)1<<(mem_addr+tmp-DO_CNT-IBIT_CNT-DI_CNT));
						EE_WriteVariable(VirtAddVarTab[9],ai_type);
					}
				}
				tx_ptr[0]=rx_ptr[0];
				tx_ptr[1]=WR_MULTI_COIL;
				tx_ptr[2]=mem_addr>>8;
				tx_ptr[3]=mem_addr&0xFF;
				tx_ptr[4]=cnt>>8;
				tx_ptr[5]=cnt&0xFF;
				crc=GetCRC16(tx_ptr,6);
				tx_ptr[6]=crc>>8;
				tx_ptr[7]=crc&0xFF;
				send(tx_ptr,8);
				break;
			default:
				tx_ptr[0] = rx_ptr[0];
				tx_ptr[1] = 0xA0;
				// request id
				tx_ptr[2] = rx_ptr[2];
				tx_ptr[3] = rx_ptr[3];
				// device id
				tx_ptr[4] = 0x04;
				tx_ptr[5] = 0x00;
				tx_ptr[6] = 0x00;
				tx_ptr[7] = 0x01;

				crc = GetCRC16((unsigned char*)tx_ptr,8);
				tx_ptr[8]=crc>>8;
				tx_ptr[9]=crc&0xFF;
				send(tx_ptr,10);
		}
	}
}

void rx1_callback(uint8_t* rx_ptr,uint16_t rx_cnt) {
	if(rx_cnt<=UART_BUF_SISE) {
		memcpy(tmp_rx1_buf,rx_ptr,rx_cnt);
		rx_callback(tmp_rx1_buf, rx_cnt, tx1_buf, send_data_to_uart1);
	}
}

void rx2_callback(uint8_t* rx_ptr,uint16_t rx_cnt) {
	if(rx_cnt<=UART_BUF_SISE) {
		memcpy(tmp_rx2_buf,rx_ptr,rx_cnt);
		rx_callback(tmp_rx2_buf, rx_cnt, tx2_buf, send_data_to_uart2);
	}
}
