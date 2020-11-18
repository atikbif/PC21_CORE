/*
 * modbus_master.c
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: User
 */

#include "modbus_master.h"
#include "rs485_protocol.h"
#include "rs485.h"
#include "crc.h"
#include "main.h"
#include "leds.h"

static uint8_t tx2_buf[256];
static uint8_t tx1_buf[256];

extern const uint16_t canal1_req_count;
extern const uint16_t canal1_modbus_delay;
extern const mvar_reqs canal1_mvar_reqs[];

extern const uint16_t canal2_req_count;
extern const uint16_t canal2_modbus_delay;
extern const mvar canal2_req1_vars[];
extern const mvar_reqs canal2_mvar_reqs[];

extern uint8_t err_mod_can1[256];
extern uint8_t err_mod_can2[256];

extern uint8_t rx2_buf[UART_BUF_SISE];
extern uint16_t rx2_cnt;

extern uint8_t rx1_buf[UART_BUF_SISE];
extern uint16_t rx1_cnt;

extern struct led_state rs1_led_rx;
extern struct led_state rs2_led_rx;

void modbus_master_process_canal1() {
	static uint8_t mb_cnt = 0;
	static uint16_t mb_tmp = 0;
	static uint16_t mb_tmp2 = 0;
	static uint16_t mb_delay = 0;
	static uint16_t mb_value = 0;
	static const mvar* vars;
	if(canal1_req_count) {
		if(mb_delay==0) {
			for(mb_tmp=0;mb_tmp<canal1_mvar_reqs[mb_cnt].req_length;mb_tmp++) {
				tx1_buf[mb_tmp] = canal1_mvar_reqs[mb_cnt].request[mb_tmp];
			}
			if(canal1_mvar_reqs[mb_cnt].wr_flag) {
				vars = canal1_mvar_reqs[mb_cnt].vars_ptr;
				for(mb_tmp2=0;mb_tmp2<canal1_mvar_reqs[mb_cnt].var_cnt;mb_tmp2++) {
					mb_value = *(vars->ptr);
					if(vars->bit_offset == -1) {
						tx1_buf[vars->start_byte_num+7] = mb_value >> 8;
						tx1_buf[vars->start_byte_num+8] = mb_value & 0xFF;
					}else {
						if(mb_value) tx1_buf[vars->start_byte_num+7] |= 1 << vars->bit_offset;
					}
					vars++;
				}
				mb_tmp2 = GetCRC16(tx1_buf,canal1_mvar_reqs[mb_cnt].req_length-2);
				tx1_buf[canal1_mvar_reqs[mb_cnt].req_length-2] = mb_tmp2 >> 8;
				tx1_buf[canal1_mvar_reqs[mb_cnt].req_length-1] = mb_tmp2 & 0xFF;
			}
			send_data_to_uart1(tx1_buf,mb_tmp);
			rx1_cnt=0;
		}
		mb_delay++;
		if(mb_delay>=canal1_modbus_delay) {
			mb_delay = 0;
			// анализ ответа
			if(rx1_cnt>=canal1_mvar_reqs[mb_cnt].answer_length){
				if(GetCRC16(rx1_buf,rx1_cnt)==0) {
					rs1_led_rx.on_cmd = 1;
					if(rx1_buf[0]==tx1_buf[0]) {
						if(canal1_mvar_reqs[mb_cnt].wr_flag==0) {
							vars = canal1_mvar_reqs[mb_cnt].vars_ptr;
							for(mb_tmp=0;mb_tmp<canal1_mvar_reqs[mb_cnt].var_cnt;mb_tmp++) {
								if(vars->bit_offset == -1) {
									mb_value = (((unsigned short)rx1_buf[vars->start_byte_num+3])<<8) | (unsigned char)rx1_buf[vars->start_byte_num + 4];
									*(vars->ptr) = mb_value;
									err_mod_can1[tx1_buf[0]] = 0;
								}else {
									if(rx1_buf[vars->start_byte_num+3] & (1<<vars->bit_offset)) mb_value=1;else mb_value = 0;
									*(vars->ptr) = mb_value;
									err_mod_can1[tx1_buf[0]] = 0;
								}
								vars++;
							}
						}else err_mod_can1[tx1_buf[0]] = 0;
					}else err_mod_can1[tx1_buf[0]]=1;
				}else  err_mod_can1[tx1_buf[0]]=1;
			}else  err_mod_can1[tx1_buf[0]]=1;
			if(err_mod_can1[tx1_buf[0]]) {
				// обнуление переменной
				if(canal1_mvar_reqs[mb_cnt].wr_flag==0) {
					vars = canal1_mvar_reqs[mb_cnt].vars_ptr;
					for(mb_tmp=0;mb_tmp<canal1_mvar_reqs[mb_cnt].var_cnt;mb_tmp++) {
						*(vars->ptr) = 0;
						vars++;
					}
				}
			}
			mb_cnt++;
			if(mb_cnt>=canal1_req_count) {mb_cnt = 0;}
		}
	}
}

void modbus_master_process_canal2() {
	static uint8_t mb_cnt = 0;
	static uint16_t mb_tmp = 0;
	static uint16_t mb_tmp2 = 0;
	static uint16_t mb_delay = 0;
	static uint16_t mb_value = 0;
	static const mvar* vars;
	if(canal2_req_count) {
		if(mb_delay==0) {
			for(mb_tmp=0;mb_tmp<canal2_mvar_reqs[mb_cnt].req_length;mb_tmp++) {
				tx2_buf[mb_tmp] = canal2_mvar_reqs[mb_cnt].request[mb_tmp];
			}
			if(canal2_mvar_reqs[mb_cnt].wr_flag) {
				vars = canal2_mvar_reqs[mb_cnt].vars_ptr;
				for(mb_tmp2=0;mb_tmp2<canal2_mvar_reqs[mb_cnt].var_cnt;mb_tmp2++) {
					mb_value = *(vars->ptr);
					if(vars->bit_offset == -1) {
						tx2_buf[vars->start_byte_num+7] = mb_value >> 8;
						tx2_buf[vars->start_byte_num+8] = mb_value & 0xFF;
					}else {
						if(mb_value) tx2_buf[vars->start_byte_num+7] |= 1 << vars->bit_offset;
					}
					vars++;
				}
				mb_tmp2 = GetCRC16(tx2_buf,canal2_mvar_reqs[mb_cnt].req_length-2);
				tx2_buf[canal2_mvar_reqs[mb_cnt].req_length-2] = mb_tmp2 >> 8;
				tx2_buf[canal2_mvar_reqs[mb_cnt].req_length-1] = mb_tmp2 & 0xFF;
			}
			send_data_to_uart2(tx2_buf,mb_tmp);
			rx2_cnt=0;
		}
		mb_delay++;
		if(mb_delay>=canal2_modbus_delay) {
			mb_delay = 0;
			// анализ ответа
			if(rx2_cnt>=canal2_mvar_reqs[mb_cnt].answer_length){
				if(GetCRC16(rx2_buf,rx2_cnt)==0) {
					rs2_led_rx.on_cmd = 1;
					if(rx2_buf[0]==tx2_buf[0]) {
						if(canal2_mvar_reqs[mb_cnt].wr_flag==0) {
							vars = canal2_mvar_reqs[mb_cnt].vars_ptr;
							for(mb_tmp=0;mb_tmp<canal2_mvar_reqs[mb_cnt].var_cnt;mb_tmp++) {
								if(vars->bit_offset == -1) {
									mb_value = (((unsigned short)rx2_buf[vars->start_byte_num+3])<<8) | (unsigned char)rx2_buf[vars->start_byte_num + 4];
									*(vars->ptr) = mb_value;
									err_mod_can2[tx2_buf[0]] = 0;
								}else {
									if(rx2_buf[vars->start_byte_num+3] & (1<<vars->bit_offset)) mb_value=1;else mb_value = 0;
									*(vars->ptr) = mb_value;
									err_mod_can2[tx2_buf[0]] = 0;
								}
								vars++;
							}
						}else err_mod_can2[tx2_buf[0]] = 0;
					}else err_mod_can2[tx2_buf[0]]=1;
				}else  err_mod_can2[tx2_buf[0]]=1;
			}else  err_mod_can2[tx2_buf[0]]=1;
			if(err_mod_can2[tx2_buf[0]]) {
				// обнуление переменной
				if(canal2_mvar_reqs[mb_cnt].wr_flag==0) {
					vars = canal2_mvar_reqs[mb_cnt].vars_ptr;
					for(mb_tmp=0;mb_tmp<canal2_mvar_reqs[mb_cnt].var_cnt;mb_tmp++) {
						*(vars->ptr) = 0;
						vars++;
					}
				}
			}
			mb_cnt++;
			if(mb_cnt>=canal2_req_count) {mb_cnt = 0;}
		}
	}
}
