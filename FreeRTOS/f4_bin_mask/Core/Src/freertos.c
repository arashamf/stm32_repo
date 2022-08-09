/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "stdio.h"
#include "usart.h"
#include "task.h"
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
extern char UART_msg_TX [TX_BUFFER_SIZE];

osThreadId Task_button_Handle;

osTimerId osProgTimerLed;

osMessageQId LED_toogle_Queue; //обычная очередь
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void key_button (void const * argument);
static void ProgTimerLedCallback(void const *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

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

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  osTimerDef(ProgTimerLed, ProgTimerLedCallback);
	osProgTimerLed = osTimerCreate(osTimer(ProgTimerLed), osTimerOnce, NULL);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  osMessageQDef (LED_toogle_Queuename, 14, uint8_t); //
	LED_toogle_Queue = osMessageCreate (osMessageQ (LED_toogle_Queuename), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  osThreadDef(Task_button, key_button, osPriorityNormal, 0, 128);
	Task_button_Handle = osThreadCreate(osThread(Task_button), NULL);
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
  /* USER CODE BEGIN StartDefaultTask */
	uint8_t byte; //буфферный символ
	osEvent event;
  /* Infinite loop */
  for(;;)
  {
		event = osMessageGet(LED_toogle_Queue, 50);
		if (event.status == osEventMessage)
		{	
			byte = event.value.v; //v - message as 32-bit value
			if (byte == 0x1) 
			{
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET); //активация 4 выхода 
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET); //активация 2 выхода 
				UART3_SendString("button_1_on\r\n");
			}
			if (byte == 0x9) 
			{
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET); //активация 4 выхода 
				HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET); //активация 3 выхода 
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET); //активация 2 выхода 
				UART3_SendString("button_1_4_on\r\n");
			}
			if (byte == 0x2) 
			{
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET); //активация 4 выхода 
				UART3_SendString("button_2_on\r\n");
			}
			if (byte == 0x4) 
			{
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET); //активация 4 выхода 
				UART3_SendString("button_3_on\r\n");
			}
			if (byte == 0xC) 
			{
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET); //активация 4 выхода 
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET); //активация 3 выхода 
				UART3_SendString("button_3_4_on\r\n");
			}
			if (byte == 0x8) 
			{
				HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET); //активация 4 выхода 
				UART3_SendString("button_4_on\r\n");
			}
			osTimerStart(osProgTimerLed, 2000); //запустим программный таймер с выключением всех светодиодов
		}
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void key_button (void const * argument)
{

	uint8_t key_status = 0x0;
	
	
  for(;;)
  {
		
		if (HAL_GPIO_ReadPin(button_1_GPIO_Port,  button_1_Pin) == GPIO_PIN_SET) //если нажата 1 кнопка
		{
			osDelay(100); //задержка 100 мс
			if (HAL_GPIO_ReadPin(button_1_GPIO_Port,  button_1_Pin) == GPIO_PIN_SET) //если срабатывание кнопки из-за нажатия
			{
				key_status |= (0x1 << 0);
				if (HAL_GPIO_ReadPin(button_4_GPIO_Port,  button_4_Pin) == GPIO_PIN_SET) //если одновременно с первой, нажата и четвёртая кнопка
				{
					key_status |= (0x1 << 3);
				}
				osMessagePut(LED_toogle_Queue, key_status, 10);
				continue;
			}
		}
		if (HAL_GPIO_ReadPin(button_2_GPIO_Port,  button_2_Pin) == GPIO_PIN_SET) //если нажата 2 кнопка
		{
			osDelay(100); //задержка 100 мс
			if (HAL_GPIO_ReadPin(button_2_GPIO_Port,  button_2_Pin) == GPIO_PIN_SET) //если срабатывание кнопки из-за нажатия
			{
				key_status |= (0x1 << 1);
				osMessagePut(LED_toogle_Queue, key_status, 10);
				continue;
			}
		}
		if (HAL_GPIO_ReadPin(button_3_GPIO_Port,  button_3_Pin) == GPIO_PIN_SET) //если нажата 3 кнопка
		{
			osDelay(100); //задержка 100 мс
			if (HAL_GPIO_ReadPin(button_3_GPIO_Port,  button_3_Pin) == GPIO_PIN_SET) //если срабатывание кнопки из-за нажатия
			{
				key_status |= (0x1 << 2);
				if (HAL_GPIO_ReadPin(button_4_GPIO_Port,  button_4_Pin) == GPIO_PIN_SET) //если одновременно с третьей, нажата и четвёртая кнопка
				{
					key_status |= (0x1 << 3);
				}
				osMessagePut(LED_toogle_Queue, key_status, 10);
				continue;
			}
		}
		if (HAL_GPIO_ReadPin(button_4_GPIO_Port,  button_4_Pin) == GPIO_PIN_SET) //если нажата 4 кнопка
		{
			osDelay(100); //задержка 100 мс
			if (HAL_GPIO_ReadPin(button_4_GPIO_Port,  button_4_Pin) == GPIO_PIN_SET) //если срабатывание кнопки из-за нажатия
			{
				key_status |= (0x1 << 3);
			}
			osMessagePut(LED_toogle_Queue, key_status, 10);
			continue;
		}
		key_status = 0x0;
		osDelay(10);
  }
}

//------------------------------------------------------------------------------------------------------------//
static void ProgTimerLedCallback(void const *argument)
{
	(void) argument;
	HAL_GPIO_WritePin (LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET); //выключение всех светодиодов
	HAL_GPIO_WritePin (LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
