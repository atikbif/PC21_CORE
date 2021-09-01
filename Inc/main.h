/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"

#include "stm32f4xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_RS2_TX_Pin GPIO_PIN_3
#define LED_RS2_TX_GPIO_Port GPIOE
#define LED_RS2_RX_Pin GPIO_PIN_4
#define LED_RS2_RX_GPIO_Port GPIOE
#define LED_RELE1_Pin GPIO_PIN_8
#define LED_RELE1_GPIO_Port GPIOI
#define LED_RELE5_Pin GPIO_PIN_13
#define LED_RELE5_GPIO_Port GPIOC
#define LED_RELE6_Pin GPIO_PIN_11
#define LED_RELE6_GPIO_Port GPIOI
#define LED_SYS_G_Pin GPIO_PIN_2
#define LED_SYS_G_GPIO_Port GPIOF
#define LED_SYS_R_Pin GPIO_PIN_3
#define LED_SYS_R_GPIO_Port GPIOF
#define SPI1_CS_Pin GPIO_PIN_15
#define SPI1_CS_GPIO_Port GPIOF
#define RELE5_Pin GPIO_PIN_9
#define RELE5_GPIO_Port GPIOE
#define RELE4_Pin GPIO_PIN_11
#define RELE4_GPIO_Port GPIOE
#define RELE3_Pin GPIO_PIN_13
#define RELE3_GPIO_Port GPIOE
#define RELE6_Pin GPIO_PIN_14
#define RELE6_GPIO_Port GPIOE
#define RS485_DIR1_Pin GPIO_PIN_10
#define RS485_DIR1_GPIO_Port GPIOD
#define RS485_DIR2_Pin GPIO_PIN_2
#define RS485_DIR2_GPIO_Port GPIOG
#define SPI4_CS_Pin GPIO_PIN_3
#define SPI4_CS_GPIO_Port GPIOG
#define SPI4_CS_EXTI_IRQn EXTI3_IRQn
#define RELE1_Pin GPIO_PIN_6
#define RELE1_GPIO_Port GPIOC
#define RELE2_Pin GPIO_PIN_7
#define RELE2_GPIO_Port GPIOC
#define LED_CAN1_RX_Pin GPIO_PIN_7
#define LED_CAN1_RX_GPIO_Port GPIOD
#define LED_RELE6_FAULT_Pin GPIO_PIN_10
#define LED_RELE6_FAULT_GPIO_Port GPIOG
#define LED_RELE5_FAULT_Pin GPIO_PIN_11
#define LED_RELE5_FAULT_GPIO_Port GPIOG
#define LED_RELE2_Pin GPIO_PIN_12
#define LED_RELE2_GPIO_Port GPIOG
#define LED_CAN2_TX_Pin GPIO_PIN_15
#define LED_CAN2_TX_GPIO_Port GPIOG
#define LED_RELE3_Pin GPIO_PIN_6
#define LED_RELE3_GPIO_Port GPIOB
#define LED_RELE4_FAULT_Pin GPIO_PIN_7
#define LED_RELE4_FAULT_GPIO_Port GPIOB
#define LED_RELE4_Pin GPIO_PIN_8
#define LED_RELE4_GPIO_Port GPIOB
#define LED_RELE1_FAULT_Pin GPIO_PIN_9
#define LED_RELE1_FAULT_GPIO_Port GPIOB
#define LED_RELE2_FAULT_Pin GPIO_PIN_0
#define LED_RELE2_FAULT_GPIO_Port GPIOE
#define LED_RELE3_FAULT_Pin GPIO_PIN_1
#define LED_RELE3_FAULT_GPIO_Port GPIOE
#define LED_CAN2_RX_Pin GPIO_PIN_4
#define LED_CAN2_RX_GPIO_Port GPIOI
#define LED_CAN1_TX_Pin GPIO_PIN_5
#define LED_CAN1_TX_GPIO_Port GPIOI
#define LED_RS1_TX_Pin GPIO_PIN_6
#define LED_RS1_TX_GPIO_Port GPIOI
#define LED_RS1_RX_Pin GPIO_PIN_7
#define LED_RS1_RX_GPIO_Port GPIOI
/* USER CODE BEGIN Private defines */

#define LWIP_NETIF_LINK_CALLBACK        1

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
