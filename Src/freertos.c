/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ld_prog.h"
#include "os_conf.h"
#include "rs485.h"
#include "iwdg.h"
#include "crc.h"
#include "din.h"
#include "dout.h"
#include "ain.h"
#include "udp_server.h"
#include "modbus_master.h"
#include "can_task.h"
#include "system_vars.h"
#include "scada.h"
#include "leds.h"
#include "modules.h"
#include "do_module.h"
#include "eeprom.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern unsigned short sys_tmr;
unsigned short work_time = 1;
extern volatile unsigned short plc_cycle;
extern unsigned short ain[AI_CNT];
extern unsigned short ain_raw[AI_CNT];
extern unsigned char ain_alarm[AI_CNT];

unsigned short prev_ain[AI_CNT] = {0};
unsigned char prev_ain_alarm[AI_CNT] = {0};
extern uint16_t update_ai;
extern uint16_t used_ai;

extern uint8_t start_up;

uint16_t ai_type = 0xFFFF;

uint8_t adc_spi_tx[32];
uint8_t adc_spi_rx[32];
uint32_t adc_sum[14];

unsigned char scada_bits[16];
unsigned short scada_regs[16];

extern unsigned short ireg[IREG_CNT];
extern SPI_HandleTypeDef hspi1;
osThreadId canTaskHandle;

extern struct led_state sys_led_green;

uint8_t loader_flag = 0;
extern uint16_t  VirtAddVarTab[NB_OF_VAR];

extern uint8_t adc_link;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId progTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

extern void tcp_server_init(void);
extern void calculate_adc();
extern void update_ethip_intern_regs();
extern void update_ethip_intern_bits();
extern void update_ethip_scada_bits();
extern void update_ethip_scada_regs();

__weak void init_vars() {};

static void update_data_to_scada() {
	static uint8_t scada_tmr = 0;
	scada_tmr++;
	switch(scada_tmr) {
		case 10:net_bits_to_scada_first();break;
		case 20:net_bits_to_scada_second();break;
		case 30:net_regs_to_scada_first();break;
		case 40:net_regs_to_scada_second();break;
		case 50:
		  //update_ethip_intern_regs();
		  cluster_regs_to_scada();
		  break;
		case 60:
		  //update_ethip_intern_bits();
		  cluster_bits_to_scada(0);
		  break;
		case 62:cluster_bits_to_scada(1);break;
		case 64:cluster_bits_to_scada(2);break;
		case 66:cluster_bits_to_scada(3);break;

		case 70:update_ethip_scada_bits();break;
		case 80:update_ethip_scada_regs();break;
		case 90:node_and_cluster_state_to_scada();break;
	}
	if(scada_tmr>=100) scada_tmr = 0;
}
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void ProgTask(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

	init_can_addr_pins();
	uint8_t can_check = read_can_addr();
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1024);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of progTask */
  osThreadDef(progTask, ProgTask, osPriorityBelowNormal, 0, 1024);
  progTaskHandle = osThreadCreate(osThread(progTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  osThreadDef(can, canTask, osPriorityNormal, 0, 512);
  canTaskHandle = osThreadCreate(osThread(can), NULL);

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  init_leds();
  led_cycle(0);
  tcp_server_init();
  udp_server_init();
  static uint16_t led_tmr = 0;
  static uint16_t adc_spi_tmr = 0;
  static uint16_t crc = 0;
  static uint16_t i=0;
  static uint16_t value=0;
  static uint16_t filter_cnt = 0;
  static uint8_t ms_tmr = 0;
  static uint16_t sys_led_tmr = 0;

  init_din();
  init_leds();
  //osDelay(1000);
  start_up = 1;
  for(;;)
  {
	  led_tmr++;
	  sys_led_tmr++;
	  if(led_tmr>=10) {
		  led_tmr=0;
		  if(sys_led_tmr>=500) {
			  sys_led_tmr = 0;
			  sys_led_green.on_cmd = 1;
		  }
		  led_cycle(10);
	  }

	  update_data_to_scada();
	  ms_tmr++;
	  if(ms_tmr>=100) {
		  ms_tmr=0;
		  update_system_vars();
	  }

	  adc_spi_tmr++;
	  if(adc_spi_tmr==1) {
		  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_RESET);
	  }else if(adc_spi_tmr==4) {
		  adc_spi_tx[0]=0x31;
		  adc_spi_tx[1]=0x31;
		  adc_spi_tx[2]=ai_type>>8;
		  adc_spi_tx[3]=ai_type&0xFF;
		  crc = GetCRC16(adc_spi_tx,30);
		  adc_spi_tx[30] = crc >> 8;
		  adc_spi_tx[31] = crc & 0xFF;
		  HAL_SPI_TransmitReceive_DMA(&hspi1, adc_spi_tx, adc_spi_rx, 32);
	  }else if(adc_spi_tmr==7) {
		  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,GPIO_PIN_SET);
		  crc = GetCRC16(adc_spi_rx,32);
		  if(crc==0) {
			  adc_link = 1;
			  for(i=0;i<AI_CNT;i++) {
				  value = (uint16_t)adc_spi_rx[2+i*2]<<8;
				  adc_spi_rx[2+i*2] = 0;
				  value|=adc_spi_rx[3+i*2];
				  adc_spi_rx[3+i*2] = 0;
				  adc_sum[i]+=value;
			  }
			  filter_cnt++;
			  if(filter_cnt==10) {
				  for(i=0;i<AI_CNT;i++) {
					  if(ai_type & ((uint16_t)1<<i)) {value = adc_sum[i]*50/62;}	// mA
					  else { value = adc_sum[i]/10; }	// mV
					  adc_sum[i] = 0;
					  ain_raw[i]=value;
				  }
				  filter_cnt = 0;
				  calculate_adc();
				  for(i=0;i<AI_CNT;i++) {
					  if(used_ai & (1<<i)) {
						  if((prev_ain[i] != ain[i]) || (prev_ain_alarm[i] != ain_alarm[i])) {
							  update_ai |= 1<<i;
							  prev_ain[i] = ain[i];
							  prev_ain_alarm[i] = ain_alarm[i];
						  }
					  }
				  }
				  update_ethip_ain();
			  }
		  }else adc_link = 0;
	  }
	  if(adc_spi_tmr>=10) {adc_spi_tmr=0;}

	  update_din();
	  uart1_scan();
	  uart2_scan();
	  modbus_master_process_canal1();
	  modbus_master_process_canal2();
	  do_mod_cycle();

	  if(loader_flag) {
		  loader_flag++;
		  if(loader_flag>=50) {
			  EE_WriteVariable(VirtAddVarTab[1],0);
			  //EE_ReadVariable(VirtAddVarTab[1],  &v);
			  NVIC_SystemReset();
		  }
	  }

	  osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_ProgTask */
/**
* @brief Function implementing the progTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ProgTask */
void ProgTask(void const * argument)
{
  /* USER CODE BEGIN ProgTask */
  /* Infinite loop */

  static unsigned short work_tmr = 0;
  while(start_up==0) {osDelay(1);}
  osDelay(2000);
  init_vars();
  MX_IWDG_Init();
  for(;;)
  {
	  work_tmr = sys_tmr;
	  ld_process();
	  if(sys_tmr>=work_tmr) work_time = sys_tmr-work_tmr+1;
	  else work_time = 0xFFFF-work_tmr+sys_tmr;
	  inc_timers();
	  update_dout();
	  if(plc_cycle) osDelay(plc_cycle);
	  LL_IWDG_ReloadCounter(IWDG);
  }
  /* USER CODE END ProgTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
