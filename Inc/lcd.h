/*
 * lcd.h
 *
 *  Created on: 2 сент. 2021 г.
 *      Author: User
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>

#define LCD_BUF_SIZE	256
#define LCD_HEADER_HIGH								0xA1
#define LCD_HEADER_LOW								0x40

#define LCD_NOT_READING								0x00
#define LCD_NEXT_PACKET_IS_FOR_READING				0x01
#define LCD_CUR_PACKET_IS_FOR_READING				0x02

void check_lcd_rx_buf();
uint8_t get_lcd_memory_byte();

#endif /* LCD_H_ */
