/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h"
#include "queue.h"
#include "semphr.h"
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
UART_HandleTypeDef huart3;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for myTask1ms */
osThreadId_t myTask1msHandle;
const osThreadAttr_t myTask1ms_attributes = {
  .name = "myTask1ms",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for myTask10ms */
osThreadId_t myTask10msHandle;
const osThreadAttr_t myTask10ms_attributes = {
  .name = "myTask10ms",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for myTask100ms */
osThreadId_t myTask100msHandle;
const osThreadAttr_t myTask100ms_attributes = {
  .name = "myTask100ms",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for ButtonCheck */
osThreadId_t ButtonCheckHandle;
const osThreadAttr_t ButtonCheck_attributes = {
  .name = "ButtonCheck",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for ButtonLedBlink */
osThreadId_t ButtonLedBlinkHandle;
const osThreadAttr_t ButtonLedBlink_attributes = {
  .name = "ButtonLedBlink",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for Task_UART */
osThreadId_t Task_UARTHandle;
const osThreadAttr_t Task_UART_attributes = {
  .name = "Task_UART",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};
/* Definitions for BinSem01_UART_Green */
osSemaphoreId_t BinSem01_UART_GreenHandle;
const osSemaphoreAttr_t BinSem01_UART_Green_attributes = {
  .name = "BinSem01_UART_Green"
};
/* USER CODE BEGIN PV */
//uint8_t message_USB_tx [20]; //массив для передачи по USB
//uint8_t message_USB_rx [20]; //массив для приёма по USB
char message_UART_tx [20];
//uint8_t message_UART_rx [20];

uint32_t task1msCnt = 0;
uint32_t task10msCnt = 0;
uint32_t task100msCnt = 0;

//osThreadId Task1msHandle,Task10msHandle,Task100msHandle; // специальная глобальная переменная для очереди
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
void StartDefaultTask(void *argument);
void Task1msHandler(void *argument);
void Task10msHandler(void *argument);
void Task100msHandler(void *argument);
void ButtonCheckHandler(void *argument);
void ButtonLedBlinkHandler(void *argument);
void Task_UART_Handle(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BinSem01_UART_Green */
  BinSem01_UART_GreenHandle = osSemaphoreNew(1, 1, &BinSem01_UART_Green_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (1, sizeof(uint16_t), &myQueue01_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask1ms */
  myTask1msHandle = osThreadNew(Task1msHandler, NULL, &myTask1ms_attributes);

  /* creation of myTask10ms */
  myTask10msHandle = osThreadNew(Task10msHandler, NULL, &myTask10ms_attributes);

  /* creation of myTask100ms */
  myTask100msHandle = osThreadNew(Task100msHandler, NULL, &myTask100ms_attributes);

  /* creation of ButtonCheck */
  ButtonCheckHandle = osThreadNew(ButtonCheckHandler, NULL, &ButtonCheck_attributes);

  /* creation of ButtonLedBlink */
  ButtonLedBlinkHandle = osThreadNew(ButtonLedBlinkHandler, NULL, &ButtonLedBlink_attributes);

  /* creation of Task_UART */
  Task_UARTHandle = osThreadNew(Task_UART_Handle, NULL, &Task_UART_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 57600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_Task1msHandler */
/**
* @brief Function implementing the myTask1ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task1msHandler */
void Task1msHandler(void *argument)
{
  /* USER CODE BEGIN Task1msHandler */
    TickType_t xLastWakeTime; //xLastWakeTime хранит значение времени, соответствующее моменту, когда задача была разблокирована в предыдущий раз.
    const TickType_t xFrequency = 1 / portTICK_PERIOD_MS; // аргумент задаёт нужный период
    xLastWakeTime = xTaskGetTickCount(); //xLastWakeTime нуждается в инициализации текущим значением счетчика тиков. Переменная записывается явно только в этот момент, затем обновляется автоматически внутри функции vTaskDelayUntil()
  /* Infinite loop */
  for(;;)
  {
	  task1msCnt++;
	  if (task1msCnt == 100)
	  	  {
		  task1msCnt = 0;
		  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	  	  }
	  vTaskDelayUntil(&xLastWakeTime, xFrequency); //функция vTaskDelayUntil() вызывает диспетчер, который переводит задачу в WAIT на период T = xLastWakeTime + xFrequency от момента прошлого пробуждения задачи*/
  }
  /* USER CODE END Task1msHandler */
}

/* USER CODE BEGIN Header_Task10msHandler */
/**
* @brief Function implementing the myTask10ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task10msHandler */
void Task10msHandler(void *argument)
{
  /* USER CODE BEGIN Task10msHandler */
	TickType_t xLastWakeTime; //аргумент хранит значение времени, соответствующее моменту, когда задача была разблокирована в предыдущий раз
	const TickType_t xFrequency = 10 / portTICK_PERIOD_MS; // аргумент задаёт нужный период (в данном случае 10 мс)
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	task10msCnt++;
	if (task10msCnt == 100) //(досчитаем до 1 с)
		{
		task10msCnt = 0;
	 	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	 	}
	vTaskDelayUntil(&xLastWakeTime, xFrequency); //функция vTaskDelayUntil() заблокирует задачу до момента времени T = xLastWakeTime + xFrequency*/
  }
  /* USER CODE END Task10msHandler */
}

/* USER CODE BEGIN Header_Task100msHandler */
/**
* @brief Function implementing the myTask100ms thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task100msHandler */
void Task100msHandler(void *argument)
{
  /* USER CODE BEGIN Task100msHandler */
	TickType_t xLastWakeTime; //аргумент хранит значение времени, соответствующее моменту, когда задача была разблокирована в предыдущий раз
	const TickType_t xFrequency = 50 / portTICK_PERIOD_MS; // аргумент задаёт нужный период (в данном случае 50 мс)
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		task100msCnt++;
	if (task100msCnt == 100) //(досчитаем до 5 с)
		{
		task100msCnt = 0;
	 	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	 	xSemaphoreGive (BinSem01_UART_GreenHandle); //выдать семафор
	 	}
	vTaskDelayUntil(&xLastWakeTime, xFrequency); //функция vTaskDelayUntil() заблокирует задачу до момента времени T = xLastWakeTime + xFrequency*/
  }

  /* USER CODE END Task100msHandler */
}

/* USER CODE BEGIN Header_ButtonCheckHandler */
/**
* @brief Function implementing the ButtonCheck thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ButtonCheckHandler */
void ButtonCheckHandler(void *argument)
{
  /* USER CODE BEGIN ButtonCheckHandler */
  /* Infinite loop */
	uint8_t button = 0;
  for(;;)
  {
	  	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==GPIO_PIN_SET) // button is pressed?
			{
			button = 1;
			}
		if (button)
			{
			xQueueSend (myQueue01Handle, &button, 0); //xQueueSendToFront() используется для отправки данных в начало очереди.
			button = 0;
			vTaskDelay(200 / portTICK_RATE_MS); //вызовом vTaskDelay(), задача помещается в состояние Blocked до истечения периода задержки. Период задержки указывается в 'тиках', можно использовать portTICK_RATE_MS для преобразования в мс
			}
    osDelay(1);
  }
  /* USER CODE END ButtonCheckHandler */
}

/* USER CODE BEGIN Header_ButtonLedBlinkHandler */
/**
* @brief Function implementing the ButtonLedBlink thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ButtonLedBlinkHandler */
void ButtonLedBlinkHandler(void *argument)
{
  /* USER CODE BEGIN ButtonLedBlinkHandler */
  /* Infinite loop */
	uint8_t blink_rate=0; //индикатор включения кнопки
  for(;;)
  {
	xQueueReceive (myQueue01Handle, &blink_rate, 0); //xQueueReceive() используется для приема (чтения) элемента из очереди, при этом элемент удаляется из очереди
	if (blink_rate)
		{
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
		blink_rate = 0;
		}
    osDelay(1);
  }
  /* USER CODE END ButtonLedBlinkHandler */
}

/* USER CODE BEGIN Header_Task_UART_Handle */
/**
* @brief Function implementing the Task_UART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task_UART_Handle */
void Task_UART_Handle(void *argument)
{
  /* USER CODE BEGIN Task_UART_Handle */
  /* Infinite loop */
  for(;;)
  {
	if( BinSem01_UART_GreenHandle != NULL )
	{
		if( xSemaphoreTake( BinSem01_UART_GreenHandle, (TickType_t) (200 / portTICK_RATE_MS) ) == pdTRUE ) //подождать 200 тиков, пока семафор станет свободным
		{
			sprintf (message_UART_tx, "red_led!\r\n");
			HAL_UART_Transmit (&huart3, (unsigned char*)message_UART_tx, strlen(message_UART_tx), 0xFFFF);
			memset(message_UART_tx, '\0', sizeof(message_UART_tx)); //очищаем буфер передачи
		}
	}
  }
  /* USER CODE END Task_UART_Handle */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
