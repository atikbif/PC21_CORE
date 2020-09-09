/*
 * led.h
 *
 *  Created on: 17 апр. 2020 г.
 *      Author: User
 */

#ifndef LED_H_
#define LED_H_

void init_leds();
void set_sys_green_led(int value);
void set_sys_red_led(int value);
void set_usr1_green_led(int value);
void set_usr1_red_led(int value);
void set_usr2_green_led(int value);
void set_usr2_red_led(int value);
void set_can1_green_led(int value);
void set_can1_red_led(int value);
void set_can2_green_led(int value);
void set_can2_red_led(int value);
void set_rele_green_led(int num, int value);
void set_rele_red_led(int num, int value);

#endif /* LED_H_ */
