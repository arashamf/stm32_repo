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
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DefineFont.h"
#include "delay.h"
#include "st7735.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PERIOD 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t flag = 0; //флаг окончания преобразования АЦП
volatile uint16_t adc[2] = {0}; //  два канала АЦП поэтому массив из двух элементо
uint16_t value_adc = 0;

char	led_buffer [30];
uint8_t dimension = 10;

unsigned int duty_ch1 = 0; // длительность импульсов Ш�?М канала 1 таймера1
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
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Transmit (&huart1, (uint8_t *)"start\r\n", strlen ("start\r\n"), 0xFFFF);
	
	lcdInit();
	ClearLcdMemory();
	LCD_SetFont(Arial_15x17,black);
	LCD_ShowString(5,10,"RC522 Config reader");
	LCD_Refresh();
	
	LED(1);	
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_Delay (100);
//	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc, 2); // старт АЦП с DMA
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
/*		
		duty_ch1 += PERIOD/50; // прибавляем длительность импульса  PWM по 1
    TIM1->CCR1 = duty_ch1;
    if(duty_ch1 >= PERIOD)
    {
       duty_ch1 = 0;
    }
		HAL_Delay (500);*/
	 
		HAL_ADC_Start (&hadc1); // старт АЦП
		HAL_ADC_PollForConversion (&hadc1, 100); 
		adc[0] = HAL_ADC_GetValue (&hadc1);
		if (adc[0] >= 4000)
		{
				adc[0] = 4000;
		}
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		TIM1->CCR1 = adc [0] ; //впишем в нашу настройку длительности импульса Ш�?М разницу между полученными значениями
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //запустим Ш�?М
	//	adc[0] = 0; adc[1] = 0; //обнулим принятые значения АЦП
			
		sprintf (led_buffer, "ADC %u ", adc[0]);
		ClearLcdMemory();
		LCD_ShowString(5, dimension, led_buffer);
		LCD_Refresh();
		
		if (((GPIOC->ODR) & (GPIO_ODR_OD13)) == GPIO_ODR_OD13)
		{LED(1);}
		else 
		{LED(0);}
		
		HAL_Delay(500);
	//	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc, 2); // запускаем преобразование сигнала АЦП1
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
  RCC_OscInitStruct.PLL.PLLN = 160;
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
    	flag = 1;
    }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	if(hadc->Instance == ADC1)
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)"ADC1_error\n\r", strlen("ADC1_error\n\r"), 1000);
	}
}
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
