/*
 * leds.c
 *
 *  Created on: 8 но€б. 2020 г.
 *      Author: User
 */

#include "leds.h"

struct led_state sys_led_green;
struct led_state sys_led_red;
struct led_state can1_led_rx;
struct led_state can1_led_tx;
struct led_state can2_led_rx;
struct led_state can2_led_tx;
struct led_state rs1_led_rx;
struct led_state rs1_led_tx;
struct led_state rs2_led_rx;
struct led_state rs2_led_tx;

struct led_state rele_led_on[RELE_LED_QUANTITY];
struct led_state rele_led_fault[RELE_LED_QUANTITY];

#define LED_CNT		(10 + RELE_LED_QUANTITY*2)

struct led_state * const leds[LED_CNT] = {	&sys_led_green,&sys_led_red,
											&can1_led_rx,&can1_led_tx,
											&can2_led_rx,&can2_led_rx,
											&rs1_led_rx,&rs1_led_tx,
											&rs2_led_rx,&rs2_led_tx,
											&rele_led_on[0],&rele_led_on[1],
											&rele_led_on[2],&rele_led_on[3],
											&rele_led_on[4],&rele_led_on[5],
											&rele_led_fault[0],&rele_led_fault[1],
											&rele_led_fault[2],&rele_led_fault[3],
											&rele_led_fault[4],&rele_led_fault[5]};

static const GPIO_TypeDef* led_gpio_ports[LED_CNT] = {
	LED_SYS_G_GPIO_Port, LED_SYS_R_GPIO_Port,
	LED_CAN1_RX_GPIO_Port, LED_CAN1_TX_GPIO_Port,
	LED_CAN2_RX_GPIO_Port, LED_CAN2_TX_GPIO_Port,
	LED_RS1_RX_GPIO_Port, LED_RS1_TX_GPIO_Port,
	LED_RS2_RX_GPIO_Port, LED_RS2_TX_GPIO_Port,
	LED_RELE1_GPIO_Port, LED_RELE2_GPIO_Port,
	LED_RELE3_GPIO_Port, LED_RELE4_GPIO_Port,
	LED_RELE5_GPIO_Port, LED_RELE6_GPIO_Port,
	LED_RELE1_FAULT_GPIO_Port, LED_RELE2_FAULT_GPIO_Port,
	LED_RELE3_FAULT_GPIO_Port, LED_RELE4_FAULT_GPIO_Port,
	LED_RELE5_FAULT_GPIO_Port, LED_RELE6_FAULT_GPIO_Port
};

static const uint16_t led_gpio_pins[LED_CNT] = {
	LED_SYS_G_Pin, LED_SYS_R_Pin,
	LED_CAN1_RX_Pin, LED_CAN1_TX_Pin,
	LED_CAN2_RX_Pin, LED_CAN2_TX_Pin,
	LED_RS1_RX_Pin, LED_RS1_TX_Pin,
	LED_RS2_RX_Pin, LED_RS2_TX_Pin,
	LED_RELE1_Pin, LED_RELE2_Pin,
	LED_RELE3_Pin, LED_RELE4_Pin,
	LED_RELE5_Pin, LED_RELE6_Pin,
	LED_RELE1_FAULT_Pin, LED_RELE2_FAULT_Pin,
	LED_RELE3_FAULT_Pin, LED_RELE3_FAULT_Pin,
	LED_RELE5_FAULT_Pin, LED_RELE6_FAULT_Pin
};

void set_on_off_time(struct led_state *led, uint16_t on_time, uint16_t off_time) {
	led->on_time = on_time;
	led->off_time = off_time;
}

static void leds_hardware() {
	for(uint8_t i=0;i<LED_CNT;++i) {
		if(leds[i]->inverse==0) {
			if(leds[i]->out_state) HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_SET);
			else HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_RESET);
		}else {
			if(leds[i]->out_state) HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_RESET);
			else HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_SET);
		}
	}
}

void init_leds() {
	sys_led_green.mode =  LED_FLASH_BY_CMD;
	sys_led_green.on_cmd = 0;
	sys_led_green.out_state = 0;
	sys_led_green.off_cmd = 0;
	sys_led_green.tmr = 0;
	sys_led_green.inverse = 1;
	set_on_off_time(&sys_led_green,50,50);

	sys_led_red.mode =  LED_FLASH_BY_CMD;
	sys_led_red.out_state = 0;
	sys_led_red.on_cmd = 0;
	sys_led_red.off_cmd = 1;
	sys_led_red.tmr = 0;
	sys_led_red.inverse = 1;
	set_on_off_time(&sys_led_red,50,50);

	can1_led_rx.mode =  LED_FLASH_BY_CMD;
	can1_led_rx.out_state = 0;
	can1_led_rx.on_cmd = 0;
	can1_led_rx.off_cmd = 0;
	can1_led_rx.tmr = 0;
	can1_led_rx.inverse = 0;
	set_on_off_time(&can1_led_rx, 30,30);

	can1_led_tx = can1_led_rx;
	can2_led_tx = can1_led_rx;
	can2_led_rx = can1_led_rx;
	rs1_led_rx = can1_led_rx;
	rs1_led_tx = can1_led_rx;
	rs2_led_rx = can1_led_rx;
	rs2_led_tx = can1_led_rx;

	for(uint8_t i=0;i<RELE_LED_QUANTITY;i++) {
		rele_led_on[i].mode = LED_ON_OFF;
		rele_led_on[i].off_cmd = 0;
		rele_led_on[i].out_state = 0;
		rele_led_on[i].tmr = 0;
		rele_led_on[i].inverse = 0;

		rele_led_fault[i].mode = LED_ON_OFF;
		rele_led_fault[i].off_cmd = 0;
		rele_led_fault[i].out_state = 0;
		rele_led_fault[i].tmr = 0;
		rele_led_fault[i].inverse = 0;
	}
}

void led_cycle(uint8_t add_value) {
	for(uint8_t i=0;i<LED_CNT;i++) {
		struct led_state *led = leds[i];
		if(led->off_cmd) {
			led->out_state = 0;
			led->tmr = 0;
		}else {
			switch(led->mode) {
				case LED_FLASH_BY_CMD:
					if(led->on_cmd) {
						led->tmr+=add_value;
						if(led->tmr<led->on_time) led->out_state = 1;
						else led->out_state = 0;
						if(led->tmr>led->on_time + led->off_time) {
							led->tmr = 0;
							led->on_cmd = 0;
						}
					}else {
						led->out_state = 0;
						led->tmr = 0;
					}
					break;
				case LED_TOGGLE:
					led->tmr+=add_value;
					if(led->tmr<led->on_time) led->out_state = 1;
					else led->out_state = 0;
					if(led->tmr>=led->on_time + led->off_time) led->tmr = 0;
					break;
				case LED_ON_OFF:
					break;
			}
		}
	}
	leds_hardware();
}

