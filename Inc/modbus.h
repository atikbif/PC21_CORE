/*
 * modbus.h
 *
 *  Created on: 31 янв. 2020 г.
 *      Author: User
 */

#ifndef MODBUS_H_
#define MODBUS_H_

#include "os_conf.h"

#define HOLDR_COUNT 16
#define INPR_COUNT	IREG_CNT + AI_CNT + 8
#define DINPUTS_COUNT	(DI_CNT + DI_CNT + DI_CNT + DI_CNT) // дискр входы , кор. замыкание , обрыв , ошибки входов
#define COIL_COUNT	(DO_CNT + IBIT_CNT + DI_CNT + AI_CNT)

#define READ_COILS		1
#define READ_DINPUTS	2
#define READ_HOLD_REGS	3
#define READ_INP_REGS	4
#define WR_SINGLE_COIL	5
#define WR_SINGLE_REG	6
#define WR_MULTI_REGS	0x10
#define WR_MULTI_COIL	0x0F


#endif /* MODBUS_H_ */
