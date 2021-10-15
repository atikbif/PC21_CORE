/*
 * lcd.c
 *
 *  Created on: 2 сент. 2021 г.
 *      Author: User
 */

#include "lcd.h"
#include "stm32f4xx_hal.h"
#include "os_conf.h"
#include "system_vars.h"
#include <string.h>

volatile uint16_t lcd_rx_cnt = 0;
volatile uint16_t lcd_tx_cnt = 0;
volatile uint8_t lcd_buf[LCD_BUF_SIZE];

#define PC21_CMD_READ						1
#define PC21_CMD_WRITE						2
#define PC21_CMD_RESET						3

#define REGS_ONE_RANGE		64

enum lcd_data_type {MAIN_SCREEN_MEM, IP_MEM, REG1_MEM, REG2_MEM,REG3_MEM,
					REG4_MEM, APP_NAME_MEM, APP_BUILD_DATE_MEM, APP_VERSION_MEM,
					APP_CN_MEM, BITS_MEM, MODB1_MEM, MODB2_MEM, SYS_REG_MEM,
					REL_MEM,
					LCD_MEM_TYPE_CNT};
static const uint8_t mem_length[LCD_MEM_TYPE_CNT] = {8, 5, 129, 129, 129, 129, 21, 21, 11, 3, 33, 129, 129, 49, 67};

uint8_t lcd_read_memory_mode = LCD_NOT_READING;
uint16_t lcd_mem_addr = 0;

static uint8_t lcd_mem_type = MAIN_SCREEN_MEM;

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

extern uint8_t ip_addr[4];
extern unsigned short ireg[IREG_CNT];
extern unsigned char ibit[IBIT_CNT];
extern uint16_t mmb[128];

extern const char* app_name;
extern const char* app_build_date;
extern const char* app_version;
extern const char* do_names[6];

extern unsigned char dout[DO_CNT];

extern uint16_t app_id;

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

static uint8_t get_main_screen_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;
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
			res = getSSVar(13); // telemetry state
			crc += res;
			break;
		case 7:
			res = crc;
			break;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[MAIN_SCREEN_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_sys_reg_screen_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;
	if(lcd_mem_addr>=mem_length[SYS_REG_MEM]) lcd_mem_addr = 0;
	if(lcd_mem_addr != mem_length[SYS_REG_MEM]-1) {
		if(lcd_mem_addr%2==0) res = getSSVar(lcd_mem_addr/2)>>8;
		else res = getSSVar(lcd_mem_addr/2) & 0xFF;
		if(lcd_mem_addr==0) crc = 0;
		crc+= res;
	}else res = crc;
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[SYS_REG_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_relay_screen_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;
	if(lcd_mem_addr>=mem_length[REL_MEM]) lcd_mem_addr = 0;
	if(lcd_mem_addr != mem_length[REL_MEM]-1) {
		uint16_t rel_num = lcd_mem_addr/11;
		uint16_t offset = lcd_mem_addr%11;
		if(rel_num<6) {
			if(offset<10) {
				uint16_t name_length = strlen(do_names[rel_num]);
				if(offset<name_length) res = do_names[rel_num][offset];
				else res = ' ';
			}else {
				if(dout[rel_num]) res|=0x01;
			}
		}
		if(lcd_mem_addr==0) crc = 0;
		crc+=res;
	}else res = crc;
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[REL_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_reg_mem_byte(unsigned char range_num) {
	uint8_t res = 0;
	static uint8_t crc = 0;
	uint16_t offset = REGS_ONE_RANGE*range_num;
	if(lcd_mem_addr>=mem_length[REG1_MEM+range_num]) lcd_mem_addr = 0;
	if(lcd_mem_addr == 0) crc = 0;
	if(lcd_mem_addr != mem_length[REG1_MEM+range_num]-1) {
		if(lcd_mem_addr%2==0) {
			res = ireg[offset+lcd_mem_addr/2]>>8;
		}else {
			res = ireg[offset+lcd_mem_addr/2] & 0xFF;
		}
		crc += res;
	}else {
		res = crc;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[REG1_MEM+range_num]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_modb_mem_byte(unsigned char range_num) {
	uint8_t res = 0;
	static uint8_t crc = 0;
	uint16_t offset = REGS_ONE_RANGE*range_num;
	if(lcd_mem_addr>=mem_length[MODB1_MEM+range_num]) lcd_mem_addr = 0;
	if(lcd_mem_addr == 0) crc = 0;
	if(lcd_mem_addr != mem_length[MODB1_MEM+range_num]-1) {
		if(lcd_mem_addr%2==0) {
			res = mmb[offset+lcd_mem_addr/2]>>8;
		}else {
			res = mmb[offset+lcd_mem_addr/2] & 0xFF;
		}
		crc += res;
	}else {
		res = crc;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[MODB1_MEM+range_num]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_ip_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;
	if(lcd_mem_addr>=mem_length[IP_MEM]) lcd_mem_addr = 0;
	switch(lcd_mem_addr) {
		case 0:
			res = ip_addr[0];
			crc = res;
			break;
		case 1:
			res = ip_addr[1];
			crc += res;
			break;
		case 2:
			res = ip_addr[2];
			crc += res;
			break;
		case 3:
			res = ip_addr[3];
			crc += res;
			break;
		case 4:
			res = crc;
			break;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[IP_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_app_name_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;

	uint8_t length = 0;
	for(uint8_t i=0;i<20;i++) {
		if(app_name[i]) length++;
		else break;
	}

	if(lcd_mem_addr>=mem_length[APP_NAME_MEM]) lcd_mem_addr = 0;
	if(lcd_mem_addr==0) crc = 0;

	if(lcd_mem_addr != mem_length[APP_NAME_MEM]-1) {
		if(lcd_mem_addr<length) res = app_name[lcd_mem_addr];
		else res = ' ';
		crc += res;
	}else {
		res = crc;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[APP_NAME_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_app_build_time_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;

	uint8_t length = 0;
	for(uint8_t i=0;i<20;i++) {
		if(app_build_date[i]) length++;
		else break;
	}

	if(lcd_mem_addr>=mem_length[APP_BUILD_DATE_MEM]) lcd_mem_addr = 0;
	if(lcd_mem_addr==0) crc = 0;

	if(lcd_mem_addr != mem_length[APP_BUILD_DATE_MEM]-1) {
		if(lcd_mem_addr<length) res = app_build_date[lcd_mem_addr];
		else res = ' ';
		crc += res;
	}else {
		res = crc;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[APP_BUILD_DATE_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_app_ver_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;

	uint8_t length = 0;
	for(uint8_t i=0;i<10;i++) {
		if(app_version[i]) length++;
		else break;
	}

	if(lcd_mem_addr>=mem_length[APP_VERSION_MEM]) lcd_mem_addr = 0;
	if(lcd_mem_addr==0) crc = 0;

	if(lcd_mem_addr != mem_length[APP_VERSION_MEM]-1) {
		if(lcd_mem_addr<length) res = app_version[lcd_mem_addr];
		else res = ' ';
		crc += res;
	}else {
		res = crc;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[APP_VERSION_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_app_cn_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;
	if(lcd_mem_addr>=mem_length[APP_CN_MEM]) lcd_mem_addr = 0;
	switch(lcd_mem_addr) {
		case 0:
			res = app_id >> 8;
			crc = res;
			break;
		case 1:
			res = app_id & 0xFF;
			crc += res;
			break;
		case 2:
			res = crc;
			break;
	}
	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[APP_CN_MEM]) lcd_mem_addr = 0;
	return res;
}

static uint8_t get_bits_mem_byte() {
	uint8_t res = 0;
	static uint8_t crc = 0;
	if(lcd_mem_addr>=mem_length[BITS_MEM]) lcd_mem_addr = 0;
	if(lcd_mem_addr != mem_length[BITS_MEM]-1) {
		uint16_t start_num = lcd_mem_addr*8;
		for(uint16_t i=0;i<8;i++) {
			if(start_num+i < sizeof(ibit)) {
				if(ibit[start_num+i]) res |= 1<<i;
			}
		}
		if(lcd_mem_addr==0) crc = res;
		else crc+=res;
	}else res = crc;

	lcd_mem_addr++;
	if(lcd_mem_addr>=mem_length[BITS_MEM]) lcd_mem_addr = 0;
	return res;
}

uint8_t get_lcd_memory_byte() {
	uint8_t res = 0;

	if(lcd_mem_type==MAIN_SCREEN_MEM) {
		res = get_main_screen_mem_byte();
	}else if(lcd_mem_type==IP_MEM) {
		res = get_ip_mem_byte();
	}else if(lcd_mem_type==REG1_MEM) {
		res = get_reg_mem_byte(0);
	}else if(lcd_mem_type==REG2_MEM) {
		res = get_reg_mem_byte(1);
	}else if(lcd_mem_type==REG3_MEM) {
		res = get_reg_mem_byte(2);
	}else if(lcd_mem_type==REG4_MEM) {
		res = get_reg_mem_byte(3);
	}else if(lcd_mem_type==APP_NAME_MEM) {
		res = get_app_name_mem_byte();
	}else if(lcd_mem_type==APP_BUILD_DATE_MEM) {
		res = get_app_build_time_mem_byte();
	}else if(lcd_mem_type==APP_CN_MEM) {
		res = get_app_cn_mem_byte();
	}else if(lcd_mem_type==APP_VERSION_MEM) {
		res = get_app_ver_mem_byte();
	}else if(lcd_mem_type==BITS_MEM) {
		res = get_bits_mem_byte();
	}else if(lcd_mem_type==MODB1_MEM) {
		res = get_modb_mem_byte(0);
	}else if(lcd_mem_type==MODB2_MEM) {
		res = get_modb_mem_byte(1);
	}else if(lcd_mem_type==SYS_REG_MEM) {
		res = get_sys_reg_screen_mem_byte();
	}else if(lcd_mem_type==REL_MEM) {
		res = get_relay_screen_mem_byte();
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
