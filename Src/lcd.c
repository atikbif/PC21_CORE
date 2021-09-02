/*
 * lcd.c
 *
 *  Created on: 2 сент. 2021 г.
 *      Author: User
 */

#include "lcd.h"

volatile uint16_t lcd_rx_cnt = 0;
volatile uint16_t lcd_tx_cnt = 0;
volatile uint8_t lcd_buf[LCD_BUF_SIZE];

#define PC21_CMD_SET_DATA_TYPE				0
#define PC21_CMD_READ						1
#define PC21_CMD_WRITE						2
#define PC21_CMD_RESET						3

#define PC21_DATA_TYPE_CONF					0

uint8_t lcd_read_memory_mode = LCD_NOT_READING;
uint16_t lcd_mem_addr = 0;

static uint8_t lcd_mem_type = PC21_DATA_TYPE_CONF;
static uint8_t lcd_mem_conf[4] = {0x01,0x02,0x03,0x04};

static uint8_t get_checksum(uint8_t *ptr, uint8_t cnt) {
	uint8_t sum = 0;
	uint16_t i = 0;
	for(i=0;i<cnt;++i) {
		sum+=ptr[i];
	}
	return sum;
}

void check_lcd_rx_buf() {
	if(lcd_rx_cnt>=3 && lcd_buf[0]==LCD_HEADER_HIGH && lcd_buf[1]==LCD_HEADER_LOW) {
		uint8_t checksum = get_checksum((uint8_t*)lcd_buf,lcd_rx_cnt-1);
		if(lcd_buf[lcd_rx_cnt-1]==checksum) {
			switch(lcd_buf[2]) {
				case PC21_CMD_SET_DATA_TYPE:
					lcd_mem_type = lcd_buf[3];
					break;
				case PC21_CMD_READ:
					lcd_read_memory_mode = LCD_NEXT_PACKET_IS_FOR_READING;
					lcd_mem_addr = (uint16_t)lcd_buf[3] << 8;
					lcd_mem_addr |= lcd_buf[4];
					break;
				case PC21_CMD_WRITE:
					break;
				case PC21_CMD_RESET:
					break;
			}
		}
	}
}

uint8_t get_lcd_memory_byte() {
	uint8_t res = 0;
	if(lcd_mem_type==PC21_DATA_TYPE_CONF) {
		if(lcd_mem_addr>=sizeof(lcd_mem_conf)) lcd_mem_addr = 0;
		res = lcd_mem_conf[lcd_mem_addr++];
		if(lcd_mem_addr>=sizeof(lcd_mem_conf)) lcd_mem_addr = 0;
	}

	return res;
}
