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
#include "fatfs.h"
#include "i2c.h"
#include "sdmmc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtc.h"
#include "delay.h"
#include "st7735.h"
#include "DefineFont.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char UART3_msg_TX [UART_BUFFER_SIZE]; //буффер для передачи сообщений по UART3
char UART3_msg_RX [30]; //буффер для приёма сообщений по UART3
char UART1_msg_RX [6]; //буффер для приёма сообщений
char UART1_msg_TX [4]; //буффер для передачи сообщений
unsigned int counter_msg_UART3 = 0; //счётчик элемента массива
unsigned int counter_msg_UART1; //счётчик элемента массива
uint8_t recieve_flag = 0; //флаг принятия сообщения по УАРТ

uint8_t I2C_RTC_buffer [8];
char time [6]; //массив со значениями времени для отправки на RTC

char logSDPath; // User logical drive path 
FIL logfile;     //файловый объект 
FATFS log_fs ;    // рабочая область (file system object) для логических диска
FRESULT result; //код возврата функций FatFs
uint32_t byteswritten, bytesread; //счетчик чтения/записи файла
char wtext[TXT_BUFFER_SIZE];  //буффер записи на SD карту
uint8_t rtext[TXT_BUFFER_SIZE]; //буффер считанных данных с SD карты

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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SDMMC1_SD_Init();
  MX_USART3_UART_Init();
  MX_FATFS_Init();
//  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	lcdInit();
	ClearLcdMemory();
	LCD_SetFont(Arial_22x23,black);
	LCD_ShowString(10,15,"WEB interface");
	LCD_Refresh();
	
	USART3->CR1|= USART_CR1_RXNEIE; //enable recieve uart3 interrupt
	NVIC_EnableIRQ(USART3_IRQn);
	USART1->CR1|= USART_CR1_RXNEIE; //enable recieve uart1 interrupt
	NVIC_EnableIRQ(USART1_IRQn);
	ENABLE_12V;
	counter_msg_UART1 = 0;
	RS485_RX; //режим на приём
	
	HAL_UART_Transmit(&huart3, (unsigned char*)"start\r\n", strlen("start\r\n"), 0xFFFF);

//	ENABLE_LED_SD;
	ENABLE_SD_CARD;
	mount_card (&log_fs);
	HAL_Delay (10);
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		LED_RED(1);
		if (recieve_flag)
		{
			parse_command ();
			recieve_flag = 0; //обнуление флага принятых данных
		}
		GetTime (hi2c1, DEV_ADDR, 7); //получение с, м, ч, дн, д, м, г
		sprintf (UART3_msg_TX , "time: %u:%u:%u\r\n", I2C_RTC_buffer[2], I2C_RTC_buffer[1], I2C_RTC_buffer[0]);
	//	ClearLcdMemory();
//		LCD_SetFont(Arial_22x23,black);
//		LCD_ShowString(15, 15, UART3_msg_TX);
		sprintf (UART3_msg_TX , "date: %u:%u:%u\r\n", I2C_RTC_buffer[4], I2C_RTC_buffer[5], I2C_RTC_buffer[6]);
//		LCD_ShowString(15, 35, UART3_msg_TX);
//		LCD_Refresh();
		HAL_Delay (500);
		
		LED_RED(0);
		if (recieve_flag)
		{
			parse_command ();
			recieve_flag = 0; //обнуление флага принятых данных
		}
		HAL_Delay (500);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART3
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
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
