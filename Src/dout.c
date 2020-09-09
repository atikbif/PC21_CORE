/*
 * dout.c
 *
 *  Created on: 8 џэт. 2020 у.
 *      Author: User
 */

#include "dout.h"
#include "os_conf.h"
#include "main.h"
#include "led.h"

extern unsigned char dout[DO_CNT];
uint16_t do_reg = 0;
extern uint8_t answer_93[14];

static void update_ethip_dout() {
	uint8_t tmp=0;
	uint8_t bit_offset = 0;
	uint8_t byte_num = 0;
	uint8_t bit_num = 0;
	uint8_t copy_value = 0;
	for(tmp=0;tmp<DO_CNT;tmp++) {
		bit_offset = tmp*2;
		byte_num = bit_offset/8;
		bit_num = bit_offset % 8;
		copy_value = answer_93[byte_num];
		if(dout[tmp]==0)  {copy_value |= 1<<bit_num;copy_value &= ~((uint8_t)1<<(bit_num+1));}
		else {copy_value &= ~((uint8_t)1<<bit_num);copy_value &= ~((uint8_t)1<<(bit_num+1));}
		//copy_value &= ~((uint8_t)1<<bit_num);copy_value &= ~((uint8_t)1<<(bit_num+1));
		answer_93[byte_num] = copy_value;
	}
}

void update_dout() {
	uint16_t tmp_do_reg=0;
	if(dout[0]) {
		HAL_GPIO_WritePin(RELE1_GPIO_Port,RELE1_Pin,GPIO_PIN_SET);
		set_rele_green_led(1,1);
		tmp_do_reg |= 0x0001;
	}else {
		HAL_GPIO_WritePin(RELE1_GPIO_Port,RELE1_Pin,GPIO_PIN_RESET);
		set_rele_green_led(1,0);
	}
	if(dout[1]) {
		HAL_GPIO_WritePin(RELE2_GPIO_Port,RELE2_Pin,GPIO_PIN_SET);
		set_rele_green_led(2,1);
		tmp_do_reg |= 0x0002;
	}else {
		HAL_GPIO_WritePin(RELE2_GPIO_Port,RELE2_Pin,GPIO_PIN_RESET);
		set_rele_green_led(2,0);
	}
	if(dout[2]) {
		HAL_GPIO_WritePin(RELE3_GPIO_Port,RELE3_Pin,GPIO_PIN_SET);
		set_rele_green_led(3,1);
		tmp_do_reg |= 0x0004;
	}else {
		HAL_GPIO_WritePin(RELE3_GPIO_Port,RELE3_Pin,GPIO_PIN_RESET);
		set_rele_green_led(3,0);
	}
	if(dout[3]) {
		HAL_GPIO_WritePin(RELE4_GPIO_Port,RELE4_Pin,GPIO_PIN_SET);
		set_rele_green_led(4,1);
		tmp_do_reg |= 0x0008;
	}else {
		HAL_GPIO_WritePin(RELE4_GPIO_Port,RELE4_Pin,GPIO_PIN_RESET);
		set_rele_green_led(4,0);
	}
	if(dout[4]) {
		HAL_GPIO_WritePin(RELE5_GPIO_Port,RELE5_Pin,GPIO_PIN_SET);
		set_rele_green_led(5,1);
		tmp_do_reg |= 0x0010;
	}else {
		HAL_GPIO_WritePin(RELE5_GPIO_Port,RELE5_Pin,GPIO_PIN_RESET);
		set_rele_green_led(5,0);
	}
	if(dout[5]) {
		HAL_GPIO_WritePin(RELE6_GPIO_Port,RELE6_Pin,GPIO_PIN_SET);
		set_rele_green_led(6,1);
		tmp_do_reg |= 0x0020;
	}else {
		HAL_GPIO_WritePin(RELE6_GPIO_Port,RELE6_Pin,GPIO_PIN_RESET);
		set_rele_green_led(6,0);
	}
	do_reg = tmp_do_reg;
	update_ethip_dout();
}
