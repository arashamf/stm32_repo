/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7735.h"
#include "delay.h"
#include "rc522.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_ADRESS 64
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t RC_size;
uint8_t status = MI_ERR; 
uint8_t	buffer_MFRC522 [MFRC522_MAX_LEN];
char	led_buffer [30];
char	tmp_buffer [15];
uint8_t sn[4];
uint8_t dimension = 10;
uint8_t dimension2 = 0;

char buffer_UART[50];

uint8_t sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_SPI3_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Transmit (&huart1, (uint8_t *)"start\r\n", strlen ("start\r\n"), 0xFFFF);
	
	RC522_enable();
	
	lcdInit();
	ClearLcdMemory();
	LCD_SetFont(Arial_15x17,black);
	LCD_ShowString(5,10,"RC522 Config reader");
	LCD_Refresh();
	
	LED(1);	
	HAL_Delay (500);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {			
		MFRC522_Init();
		dimension = 10;
	  HAL_Delay(20);		
		ClearLcdMemory();
		
	  status = MFRC522_Request(PICC_REQIDL, buffer_MFRC522);	// Look for the card, return type				
	  if (status == MI_OK) 
		{
			sprintf(led_buffer, "Card SAK: 0x%x, 0x%x", buffer_MFRC522[1], buffer_MFRC522[0]);
		  LCD_ShowString(5,dimension,led_buffer); 
			dimension += 15;
			status = MFRC522_Anticoll(sn); // процедура антиколлизии, получение 4-байтного адреса
			if (status == MI_OK) 
			{
				sprintf(led_buffer, "Number: 0x%x%x%x%x", sn[0],sn[1],sn[2],sn[3]);
				LCD_ShowString(5,dimension,led_buffer);
			}
			
			dimension += 15;			
			RC_size = MFRC522_SelectTag(sn); // команда выбора карты, возврат размера карты
			if (RC_size != 0) 
			{
				sprintf(led_buffer, "Size: %d kBits", RC_size);
				LCD_ShowString(5,dimension,led_buffer);				
				dimension += 15;
				dimension2 = dimension;
				for (uint8_t block = 1; block < MAX_ADRESS; block++)
				{
					dimension = dimension2;
					status = MFRC522_Auth(PICC_AUTHENT1A, block, sectorKeyA[2], sn);	// ввод ключа А, доступ к блокам
					if (status == MI_OK) 
					{			
						status = MFRC522_Read (block, buffer_MFRC522); // Read data
						if (status == MI_OK) 
						{							
							sprintf (led_buffer, "sector %u block %u", block/4, block%4);							
							LCD_ShowString(5, dimension, led_buffer);
							
							dimension += 15;
							sprintf(led_buffer, "%02X %02X %02X %02X", buffer_MFRC522[0], buffer_MFRC522[1], buffer_MFRC522[2], buffer_MFRC522[3]);							
							LCD_ShowString(5, dimension, led_buffer);
						
							dimension += 15;			
							sprintf(led_buffer, "%02X %02X %02X %02X %02X %02X", buffer_MFRC522[4], buffer_MFRC522[5], buffer_MFRC522[6],
							buffer_MFRC522[7], buffer_MFRC522[8], buffer_MFRC522[9]);
							LCD_ShowString(5, dimension, led_buffer);
						
							dimension += 15;				
							sprintf(led_buffer, "%02X %02X %02X %02X %02X %02X", buffer_MFRC522[10], buffer_MFRC522[11], buffer_MFRC522[12], buffer_MFRC522[13], 
							buffer_MFRC522[14], buffer_MFRC522[15]);
							LCD_ShowString(5, dimension, led_buffer);				
							/*sprintf (buffer_UART, "sector 11: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X \r\n", 
							buffer_MFRC522[0], buffer_MFRC522[1], buffer_MFRC522[2], buffer_MFRC522[3], buffer_MFRC522[4], buffer_MFRC522[5], buffer_MFRC522[6],
							buffer_MFRC522[7], buffer_MFRC522[8], buffer_MFRC522[9], buffer_MFRC522[10], buffer_MFRC522[11], buffer_MFRC522[12], buffer_MFRC522[13],
							buffer_MFRC522[14], buffer_MFRC522[15]);
							HAL_UART_Transmit (&huart1, (uint8_t *)buffer_UART, strlen (buffer_UART), 0xFFFF);*/
						}
						else
						{	
							ClearLcdMemory();
							break;
						}				
					}
					else
					{	
						sprintf (led_buffer, "sector %u block %u", block/4, block%4);							
						LCD_ShowString(5, dimension, led_buffer);
							
						dimension += 15;
						sprintf(led_buffer, "acceses denied");
						LCD_ShowString(5, dimension, led_buffer); //MI_NOTAGERR
					}
					LCD_Refresh();
					HAL_Delay (1000);
					ClearLcdMemory();
				}
			}
		}
		else 
		{
			sprintf(led_buffer, "card not found");
			LCD_ShowString(5,dimension,led_buffer); 				
		}
		LCD_Refresh();
		MFRC522_Halt();	
		MFRC522_AntennaOff();
		if (((GPIOC->ODR) & (GPIO_ODR_OD13)) == GPIO_ODR_OD13)
		{LED(1);}
		else 
		{LED(0);}
//		delay_ms (500);
		HAL_Delay (1000);

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
