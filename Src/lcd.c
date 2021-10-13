/*
 * lcd.c
 *
 *  Created on: 2 сент. 2021 г.
 *      Author: User
 */

#include "lcd.h"
#include "stm32f4xx_hal.h"

volatile uint16_t lcd_rx_cnt = 0;
volatile uint16_t lcd_tx_cnt = 0;
volatile uint8_t lcd_buf[LCD_BUF_SIZE];

#define PC21_CMD_READ						1
#define PC21_CMD_WRITE						2
#define PC21_CMD_RESET						3

enum lcd_data_type {MAIN_SCREEN_MEM, IP_MEM, LCD_MEM_TYPE_CNT};
static const uint8_t mem_length[LCD_MEM_TYPE_CNT] = {7, 4};

uint8_t lcd_read_memory_mode = LCD_NOT_READING;
uint16_t lcd_mem_addr = 0;

static uint8_t lcd_mem_type = MAIN_SCREEN_MEM;

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

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
				case PC21_CMD_READ:
					lcd_read_memory_mode = LCD_NEXT_PACKET_IS_FOR_READING;
					lcd_mem_type = lcd_buf[3];
					lcd_mem_addr = (uint16_t)lcd_buf[4] << 8;
					lcd_mem_addr |= lcd_buf[5];
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
	static uint8_t crc = 0;
	if(lcd_mem_type==MAIN_SCREEN_MEM) {
		if(lcd_mem_addr>=mem_length[MAIN_SCREEN_MEM]) lcd_mem_addr = 0;
		switch(lcd_mem_addr) {
			case 0:
				res = sTime.Hours;
				crc = res;
				break;
			case 1:
				res = sTime.Minutes;
				crc += res;
				break;
			case 2:
				res = sTime.Seconds;
				crc += res;
				break;
			case 3:
				res = sDate.Date;
				crc += res;
				break;
			case 4:
				res = sDate.Month;
				crc += res;
				break;
			case 5:
				res = sDate.Year;
				crc += res;
				break;
			case 6:
				res = crc;
				break;
		}
		lcd_mem_addr++;
		if(lcd_mem_addr>=mem_length[MAIN_SCREEN_MEM]) lcd_mem_addr = 0;
	}
	return res;
}

uint8_t get_lcd_header_byte(uint8_t num) {
	if(num==0) return LCD_HEADER_HIGH;
	else if(num==1) return LCD_HEADER_LOW;
	else if(num==2) return lcd_mem_type;
	else if(num==3) return lcd_mem_addr >> 8;
	else if(num==4) return lcd_mem_addr & 0xFF;
	return 0xFF;
}
