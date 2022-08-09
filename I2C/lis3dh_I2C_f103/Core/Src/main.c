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
#include "stdlib.h"
#include "string.h"
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

#define msg_SIZE 50  //размер массива длЯ формированиЯ сообщений длЯ отправки по UART
static char UART_msg_TX [msg_SIZE]; // массив для формирования сообщений для вывода по UART

#define DELAY 500 //задержка

char OUT_XYZ[6]; // массив данных с акселерометра
uint8_t status = 0;
uint8_t status_XYZ = 0;
uint8_t regAddress = 0; //буфер с адресом регистра
uint8_t data = 0; //буфер с данными

#define I2C_TIMEOUT 10 //величина таймаута в мс, по истечении которого функция выдаст ошибку и прекратит выполнение
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

void LIS3DH_init();
void I2C1_write_byte(uint8_t adress, uint8_t data);
uint8_t I2C1_read_byte(uint8_t adress);
void I2C1_read_array(uint8_t adress, char *data, uint8_t data_len);

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(SDO_GPIO_Port, SDO_Pin, DISABLE);
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, ENABLE);
  sprintf (UART_msg_TX,"lis3dsh_start\r\n");
  HAL_UART_Transmit (&huart1, &UART_msg_TX, strlen(UART_msg_TX), 0x1000);
  regAddress  = LIS3DSH_REG_WHOAMI;
  HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &regAddress, 1,  I2C_TIMEOUT);
  HAL_I2C_Master_Receive(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &status, 1,  I2C_TIMEOUT);

  sprintf (UART_msg_TX,"Who_I_am=%x\r\n", status);
  HAL_UART_Transmit (&huart1, &UART_msg_TX, strlen(UART_msg_TX), 0x1000);

  LIS3DH_init();

  HAL_Delay (DELAY);

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
	regAddress  = LIS3DSH_CTRL_REG4;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &regAddress, 1,  I2C_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &status, 1,  I2C_TIMEOUT);
	sprintf (UART_msg_TX,"Who_I_am=%x\r\n", status);
	HAL_UART_Transmit (&huart1, &UART_msg_TX, strlen(UART_msg_TX), 0x1000);
	HAL_GPIO_TogglePin (LED_GPIO_Port, LED_Pin);

	HAL_Delay (DELAY);

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 42;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SDO_GPIO_Port, SDO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SDO_Pin */
  GPIO_InitStruct.Pin = SDO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SDO_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//---------------------ф-я инициализации датчика LIS3DH----------------------------//
void LIS3DH_init()
	{
	// настройка акселерометра
	/*regAddress  = LIS3DSH_CTRL_REG4;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &regAddress, 1,  I2C_TIMEOUT);
	data = LIS3DSH_F_3HZ_NORMAL;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &data, 1,  I2C_TIMEOUT);*/
	HAL_I2C_Mem_Write(&hi2c1, LIS3DSH_I2C_ADDRESS << 1),LIS3DSH_CTRL_REG4, uint16_t MemAddSize, uint8_t *pData, 1, I2C_TIMEOUT);
	regAddress  = LIS3DSH_CTRL_REG3;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &regAddress, 1,  I2C_TIMEOUT);
	data = 0x88;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &data, 1,  I2C_TIMEOUT); //"0b10001000" сигнал готовности на INT1, сигнал готовности на INT1 - активный "0"
	regAddress  = LIS3DSH_CTRL_REG5;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &regAddress, 1,  I2C_TIMEOUT);
	data = 0x18;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &data, 1,  I2C_TIMEOUT); //"0b00011000" измеряемый диапазон +-8g
	regAddress  = LIS3DSH_CTRL_REG6;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &regAddress, 1,  I2C_TIMEOUT);
	data = 0x10;
	HAL_I2C_Master_Transmit(&hi2c1, (LIS3DSH_I2C_ADDRESS << 1), &data, 1,  I2C_TIMEOUT); //включим инкремент адресов регистров
	}

// -----------------------------------I2C1 - запись байта данных-----------------------------------------//
/*void I2C1_write_byte(uint8_t adress, uint8_t data)
	{
	SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие "Start"
	while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции
	WRITE_REG (I2C1->DR, ((LIS3DSH_I2C_ADDRESS<<1) + I2C_WR)); // записываем адрес устройства и операцию записи
	while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
	(void)I2C1->SR2;
	(void)I2C1->SR1; // сбросим флаг ADDR
	WRITE_REG (I2C1->DR, adress); // записываем адрес ячейки
	while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
	WRITE_REG (I2C1->DR, data); // записываем данные
	while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
	SET_BIT (I2C1->CR1, I2C_CR1_STOP); // Условие "Stop"
	}

//------------------------------I2C1 - чтение одиного байта данных------------------------------------------//

uint8_t I2C1_read_byte(uint8_t adress) // адрес данных
	{
	uint8_t data_I2C;
	SET_BIT (I2C1->CR1, I2C_CR1_ACK); //передавать бит подтверждения ACK
	SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие "Start"
	while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции
	WRITE_REG (I2C1->DR, ((LIS3DSH_I2C_ADDRESS<<1) + I2C_WR)); // записываем адрес устройства и операцию записи
	while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
	(void)I2C1->SR2;
	(void)I2C1->SR1; // сбросим флаг ADDR
	WRITE_REG (I2C1->DR, adress); // записываем адрес ячейки
	while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
	SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие повторный "Start"
	while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции
	WRITE_REG (I2C1->DR, ((LIS3DSH_I2C_ADDRESS<<1) + I2C_RD)); // записываем адрес устройства и операцию чтения
	while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
	(void)I2C1->SR1;
	(void)I2C1->SR2; // сбросим флаг ADDR
	while(!READ_BIT (I2C1->SR1, I2C_SR1_RXNE)) {} // ожидание приема данных
	CLEAR_BIT (I2C1->CR1, I2C_CR1_ACK);  //отправим сигнал NACK
	SET_BIT (I2C1->CR1, I2C_CR1_STOP); // Условие "Stop"
	return data_I2C = I2C1->DR;
	}

//------------------------------------------I2C1 - чтение нескольких байт данных-------------------------------------------------//

void I2C1_read_array(uint8_t adress, char *data, uint8_t data_len)  //data_len - количество байт необходимых получить
	{
	SET_BIT (I2C1->CR1, I2C_CR1_ACK); // передавать бит подтверждения ACK
	SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие "Start"
	while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции "Start"
	WRITE_REG (I2C1->DR, ((LIS3DSH_I2C_ADDRESS<<1) + I2C_WR)); // записываем адрес устройства и операцию записи
	while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
	(void)I2C1->SR2;
	(void)I2C1->SR1; // сбросим флаг ADDR
	WRITE_REG (I2C1->DR, adress); // записываем адрес ячейки
	while(!READ_BIT(I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
	SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие повторный "Start"
	while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции "Start"
	WRITE_REG (I2C1->DR, ((LIS3DSH_I2C_ADDRESS<<1) + I2C_RD)); // записываем адрес устройства и операцию чтения
	while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
	(void)I2C1->SR2;
	(void)I2C1->SR1; // сбросим флаг ADDR
	for(uint8_t i = 0; i < data_len; i++)
		{
		if(i == (data_len-1))
			{
			CLEAR_BIT (I2C1->CR1, I2C_CR1_ACK); //отправим сигнал NACK
			}
		while(!READ_BIT (I2C1->SR1, I2C_SR1_RXNE)) {}; // ожидание приема данных
		*data++ = I2C1->DR; // записываем данные в массив
		}
	SET_BIT (I2C1->CR1, I2C_CR1_STOP); // Условие "Stop"
	}
*/
/* USER CODE END 4 */

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
