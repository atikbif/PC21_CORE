/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "rs485.h"
#include "lcd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

extern uint8_t rx1_buf[UART_BUF_SISE];
extern uint16_t rx1_cnt;
extern uint16_t rx1_tmr;
extern uint8_t dir1_tmr;

extern uint8_t rx2_buf[UART_BUF_SISE];
extern uint16_t rx2_cnt;
extern uint16_t rx2_tmr;
extern uint8_t dir2_tmr;

extern uint8_t baud_dir1;
extern uint8_t baud_dir2;


extern volatile uint16_t lcd_rx_cnt;
extern volatile uint16_t lcd_tx_cnt;
extern volatile uint8_t lcd_buf[LCD_BUF_SIZE];
extern uint8_t lcd_read_memory_mode;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ETH_HandleTypeDef heth;
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI3_IRQn 1 */
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  __NOP();
  if(HAL_GPIO_ReadPin(SPI4_CS_GPIO_Port, SPI4_CS_Pin)==GPIO_PIN_RESET) {
	  // beginning of packet
	  if(lcd_read_memory_mode==LCD_NEXT_PACKET_IS_FOR_READING) {
		  lcd_read_memory_mode = LCD_CUR_PACKET_IS_FOR_READING;
	  }
	  lcd_rx_cnt = 0;
  }else {
	  // end of packet
	  SPI4->DR = LCD_HEADER_HIGH;
	  lcd_tx_cnt = 1;
	  check_lcd_rx_buf();
	  if(lcd_read_memory_mode==LCD_CUR_PACKET_IS_FOR_READING) {
		  lcd_read_memory_mode = LCD_NOT_READING;
	  }
  }

  /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

	if(LL_DMA_IsActiveFlag_TC6(DMA1))
	{
		LL_DMA_ClearFlag_TC6(DMA1);
		/* Call function Transmission complete Callback */
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);
		LL_USART_EnableIT_TC(USART2);
		//dir2_tmr = baud_dir2;
	}
	if(LL_DMA_IsActiveFlag_TE6(DMA1))
	{
	 /* Call Error function */
		LL_DMA_ClearFlag_TE6(DMA1);
		LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);
		LL_USART_EnableIT_TC(USART2);
	}

  /* USER CODE END DMA1_Stream6_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */

  /* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
  * @brief This function handles CAN1 RX0 interrupts.
  */
void CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX0_IRQn 0 */

  /* USER CODE END CAN1_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan1);
  /* USER CODE BEGIN CAN1_RX0_IRQn 1 */

  /* USER CODE END CAN1_RX0_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

	if(LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1))
	{
		rx1_buf[rx1_cnt++] = LL_USART_ReceiveData8(USART1);
		if(rx1_cnt>=UART_BUF_SISE) rx1_cnt = 0;
		rx1_tmr = 0;
	}
	if(LL_USART_IsActiveFlag_TC(USART1) && LL_USART_IsEnabledIT_TC(USART1)) {
		LL_USART_ClearFlag_TC(USART1);
		HAL_GPIO_WritePin(RS485_DIR1_GPIO_Port,RS485_DIR1_Pin,GPIO_PIN_RESET);
		LL_USART_EnableIT_RXNE(USART1);
	}

  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

	if(LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2))
	{
		rx2_buf[rx2_cnt++] = LL_USART_ReceiveData8(USART2);
		if(rx2_cnt>=UART_BUF_SISE) rx2_cnt = 0;
		rx2_tmr = 0;
	}
	if(LL_USART_IsActiveFlag_TC(USART2) && LL_USART_IsEnabledIT_TC(USART2)) {
		LL_USART_ClearFlag_TC(USART2);
		HAL_GPIO_WritePin(RS485_DIR2_GPIO_Port,RS485_DIR2_Pin,GPIO_PIN_RESET);
		LL_USART_EnableIT_RXNE(USART2);
	}

  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles Ethernet global interrupt.
  */
void ETH_IRQHandler(void)
{
  /* USER CODE BEGIN ETH_IRQn 0 */

  /* USER CODE END ETH_IRQn 0 */
  HAL_ETH_IRQHandler(&heth);
  /* USER CODE BEGIN ETH_IRQn 1 */

  /* USER CODE END ETH_IRQn 1 */
}

/**
  * @brief This function handles CAN2 RX0 interrupts.
  */
void CAN2_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_RX0_IRQn 0 */

  /* USER CODE END CAN2_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan2);
  /* USER CODE BEGIN CAN2_RX0_IRQn 1 */

  /* USER CODE END CAN2_RX0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

	if(LL_DMA_IsActiveFlag_TC7(DMA2))
	{
		LL_DMA_ClearFlag_TC7(DMA2);
		/* Call function Transmission complete Callback */
		LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_7);
		LL_USART_EnableIT_TC(USART1);
		//dir1_tmr = baud_dir1;
	}
	if(LL_DMA_IsActiveFlag_TE7(DMA2))
	{
	 /* Call Error function */
		LL_DMA_ClearFlag_TE7(DMA2);
		LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_7);
		LL_USART_EnableIT_TC(USART1);
	}

  /* USER CODE END DMA2_Stream7_IRQn 0 */

  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/**
  * @brief This function handles SPI4 global interrupt.
  */
void SPI4_IRQHandler(void)
{
  /* USER CODE BEGIN SPI4_IRQn 0 */
	uint8_t tmp = 0;
	if(LL_SPI_IsActiveFlag_RXNE(SPI4))
	{
		tmp = SPI4->DR;
		if(HAL_GPIO_ReadPin(SPI4_CS_GPIO_Port, SPI4_CS_Pin)==GPIO_PIN_RESET) {
			lcd_buf[lcd_rx_cnt++] = tmp;
			if(lcd_rx_cnt>=LCD_BUF_SIZE) lcd_rx_cnt = 0;
		}
	}
	else if(LL_SPI_IsActiveFlag_TXE(SPI4))
	{
		if(lcd_tx_cnt<LCD_HEADER_LENGTH) SPI4->DR = get_lcd_header_byte(lcd_tx_cnt);
		else {
			if(lcd_read_memory_mode==LCD_CUR_PACKET_IS_FOR_READING) {
				SPI4->DR = get_lcd_memory_byte();
			}else SPI4->DR=0xFF;
		}
		lcd_tx_cnt++;
	}
	else if(LL_SPI_IsActiveFlag_OVR(SPI4))
	{
		__NOP();
	}



  /* USER CODE END SPI4_IRQn 0 */
  /* USER CODE BEGIN SPI4_IRQn 1 */

  /* USER CODE END SPI4_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
