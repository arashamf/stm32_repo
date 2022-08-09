/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define msg_SIZE 30

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */
static char UART_msg_TX [msg_SIZE]; // массив для формирования сообщений для вывода по UART
static char UART_buf_RX [msg_SIZE]; // массив для полученных сообщений по UART
const char message1 [] = "yes\r\n";
const char message2 [] = "error_command\r\n";
uint8_t count = 0; //номер элемента массива
char symbol = 0;
union data {             //объявим структуру
	uint16_t name;
	uint8_t day;
	uint8_t month;
	uint16_t year;
	} kordinata;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*-------------------------функция записывающая данные во флэш микроконтроллера--------------------------------*/
void write_flash ()
{
	HAL_FLASH_Unlock(); // разблокировать флеш
	uint32_t address = FLASH_START_ADDR; // адрес страницы для записи
	kordinata.name = 0xCAFE;
	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, kordinata.name) != HAL_OK) //запись и обработчик ошибки
	{
	    uint32_t code_error = HAL_FLASH_GetError();
	    sprintf(UART_msg_TX, "error = %lu\r\n", code_error);
	    HAL_UART_Transmit_DMA (&huart1, (uint8_t*)UART_msg_TX, strlen(UART_msg_TX));
	    return;
	}
	address = address + 2; // смещаем адрес на 2 байта

	kordinata.day = 0x13;
	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, kordinata.day) != HAL_OK) //обработчик ошибки
	{
	 	uint32_t code_error = HAL_FLASH_GetError();
	 	sprintf(UART_msg_TX, "error = %lu\r\n", code_error);
	 	HAL_UART_Transmit_DMA (&huart1, (uint8_t*)UART_msg_TX, strlen(UART_msg_TX));
	 	return;
	}
	address = address + 2; // смещаем адрес на 2 байта

	kordinata.month = 0x3;
	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, kordinata.month) != HAL_OK) //обработчик ошибки
	{
		 uint32_t code_error = HAL_FLASH_GetError();
		 sprintf(UART_msg_TX, "error = %lu\r\n", code_error);
		 HAL_UART_Transmit_DMA (&huart1, (uint8_t*)UART_msg_TX, strlen(UART_msg_TX));
		 return;
	}
	address = address + 2; // смещаем адрес на 2 байта

	kordinata.year = 0x2020;
	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, kordinata.year) != HAL_OK) //обработчик ошибки
	{
		 uint32_t code_error = HAL_FLASH_GetError();
		 sprintf(UART_msg_TX, "error = %lu\r\n", code_error);
		 HAL_UART_Transmit_DMA (&huart1, (uint8_t*)UART_msg_TX, strlen(UART_msg_TX));
		 return;
	}
	address = address + 2; // смещаем адрес на 2 байта

	HAL_UART_Transmit_DMA (&huart1, (uint8_t*)"Write in flash OK\r\n", strlen("Write in flash OK\r\n"));
	HAL_FLASH_Lock(); // заблокировать флеш
}

//--------------------------------------------колбэк прерывания от UART 1 при передаче половины сообщения------------------------------------//
/*DMA вызывает два прерывания, первое после отправки половины буфера, а второе при завершении. Чтобы отключить половинку,
нужно в файле stm32f1xx_hal_uart.c найти функцию HAL_UART_Transmit_DMA(...) и закомментировать строку huart->hdmatx->XferHalfCpltCallback = UART_DMATxHalfCplt*/
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
		{

		}
}

//--------------------------------------колбэк прерывания от UART 1 при передаче полного сообщения-------------------------------------------//
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
		{

        }
}


//--------------------------------------колбэк прерывания от UART 1 при приёме полного сообщения-------------------------------------------//
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
		{
		memset(UART_msg_TX, '\0', sizeof(UART_msg_TX)); //ф-я memset копирует младший байт символа ch в первые count символов массива buf. Возвращает указатель на массив.
		for (count = 0; count < 7; count++)
		{
			if (count == 5) //добавим в массив символ \r
				{
				symbol = UART_msg_TX [count] = '\r';
				HAL_UART_Transmit (&huart1, (unsigned char*)&symbol, 1, 0xFFF);
				continue;
				}
			if (count == 6) //добавим в массив символ \n
				{
				symbol = UART_msg_TX [count] = '\n';
				HAL_UART_Transmit (&huart1, (unsigned char*)&symbol, 1, 0xFFF);
				break;
				}
			symbol = UART_msg_TX [count] = UART_buf_RX [count]; //эхо
			HAL_UART_Transmit (&huart1, (unsigned char*)&symbol, 1, 0xFFF);
		}
		if(!strncmp(UART_buf_RX, "write", 5)) //ф-я strncmp сравнивает первые n символов строк. Возвращает "0", если строки одинаковы, "< 0" - если строка1 меньше сроки2, "> 0" - если строка1 больше строки 2
			{
//			HAL_UART_Transmit_DMA (&huart1, (unsigned char*)message1, strlen(message1));
			write_flash (); //запишем данные во флэш
			}
		else
			{
			HAL_UART_Transmit_DMA (&huart1, (unsigned char*)message2, strlen(message2));
			}
		HAL_UART_Receive_IT(&huart1, (uint8_t*)UART_buf_RX, 5);
        }
}
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  sprintf (UART_msg_TX,"flash_edit_start\r\n");
  HAL_UART_Transmit_DMA (&huart1, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX));
  HAL_GPIO_WritePin (LED_GPIO_Port, LED_Pin, ENABLE);
  HAL_UART_Receive_IT(&huart1, (uint8_t*)UART_buf_RX, 5); //ожидаем получения 5 символов по УАРТ
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	HAL_GPIO_TogglePin (LED_GPIO_Port, LED_Pin);
	HAL_Delay (1000);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
