/* USER CODE BEGIN Header */

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
#define msg_SIZE 60

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static uint16_t delay = 0;
uint8_t P1_ADDRESS[5] = {0xE7, 0xD3, 0xB6, 0xB7, 0x01}; //адрес передатчика канала 1. Адреса должны быть уникальные, для каналов Pipe2-5 заносим только младший байт, а старшие берутся из адреса канала pipe1
uint8_t P2_ADDRESS[5] = {0xE7, 0xD3, 0xB6, 0xB7, 0x02}; //адрес передатчика канала 2
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void DelayMicro (__IO uint16_t );
void NRF24_ini();
uint8_t NRF24_ReadReg(uint8_t );
void NRF24_WriteReg(uint8_t, uint8_t );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static char UART_msg_TX[msg_SIZE]; // массив для формирования сообщений для вывода по UART
static unsigned char buffer [20]; //буффер приёма данных по SPI
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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  sprintf (UART_msg_TX, "start\r\n");
  HAL_UART_Transmit (&huart2, (unsigned char*) UART_msg_TX, strlen(UART_msg_TX), 0xFFFF);
  NRF24_ini ();

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_Delay(1000);
	  sprintf (UART_msg_TX,"CONFIG: 0x%02X\r\n", NRF24_ReadReg(CONFIG));
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);

	  sprintf (UART_msg_TX,"EN_AA: 0x%02X\r\n", NRF24_ReadReg(EN_AA));
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);

	  sprintf (UART_msg_TX,"EN_RXADDR: 0x%02X\r\n", NRF24_ReadReg (EN_RXADDR));
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);

	  sprintf (UART_msg_TX,"STATUS: 0x%02X\r\n", NRF24_ReadReg(STATUS));
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);

	  sprintf (UART_msg_TX,"RF_SETUP: 0x%02X\r\n", NRF24_ReadReg(RF_SETUP));
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);

	  NRF24_Read_Buf (TX_ADDR, buffer, TX_ADR_WIDTH);
	  sprintf (UART_msg_TX,"TX_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);

	  NRF24_Read_Buf (RX_ADDR_P1, buffer, TX_ADR_WIDTH);
	  sprintf (UART_msg_TX,"RX1_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);

	  NRF24_Read_Buf (RX_ADDR_P2, buffer, TX_ADR_WIDTH);
	  sprintf (UART_msg_TX,"RX2_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	  HAL_UART_Transmit(&huart2, (unsigned char*)UART_msg_TX, strlen(UART_msg_TX), 0x1000);
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 31;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(L01_CE_GPIO_Port, L01_CE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : L01_CE_Pin */
  GPIO_InitStruct.Pin = L01_CE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(L01_CE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : L01_IRQ_Pin */
  GPIO_InitStruct.Pin = L01_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(L01_IRQ_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//------------------------------------------------------------------------------------------------------------------------------------//
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim3)
	{
		if (delay)
			delay--; //уменьшаем задержку раз в 1 мкс
	}
}
//------------------------------------------------------------------------------------------------------------------------------------//
void DelayMicro (__IO uint16_t delay_us) // IO=volatile,  указание компилятору, что эту переменную не трогать в процессе оптимизации кода
{
	HAL_TIM_Base_Start_IT(&htim3);
	delay = delay_us/10; //задержка раз в 1 мкс
	while (delay) {};
	HAL_TIM_Base_Stop_IT(&htim3);
}

//------------------------------------------------------------------------------------------------------------------------------------//
uint8_t NRF24_ReadReg(uint8_t address)
{
  uint8_t data = 0; //считанные данные
  uint8_t cmd = 0; //передадим любой байт
  address |= R_REGISTER; //включим бит чтения в адрес
  CS_ON;
  HAL_SPI_TransmitReceive(&hspi1, &address, &data, 1, 1000); //отправим адрес в шину
  if (address != STATUS)//если адрес равен адрес регистра статус то и возвращаем его состояние
  {
    cmd=0xFF;
    HAL_SPI_TransmitReceive(&hspi1, &cmd, &data, 1, 1000);
  }
  CS_OFF;
  return data;
}

//------------------------------------------------------------------------------------------------------------------------------------//
void NRF24_WriteReg(uint8_t address, uint8_t data)
{
  address |= W_REGISTER;//включим бит записи в адрес
  CS_ON;
  HAL_SPI_Transmit (&hspi1, &address, 1, 1000);//отправим адрес в шину
  HAL_SPI_Transmit (&hspi1, &data, 1, 1000);//отправим данные в шину
  CS_OFF;
}

//------------------------------------------------------------------------------------------------------------------------------------//
void NRF24_Write_Buf(uint8_t address, uint8_t *pBuf, uint8_t bytes)

{

	address |= W_REGISTER;//включим бит записи в адрес
	CS_ON;
	HAL_SPI_Transmit(&hspi1, &address, 1, 1000);//отправим адрес в шину
	DelayMicro(1);
	HAL_SPI_Transmit(&hspi1, pBuf, bytes, 1000);//отправим данные в буфер
	CS_OFF;
}

//------------------------------------------------------------------------------------------------------------------------------------//
void NRF24_Read_Buf (uint8_t address, uint8_t *pBuf, uint8_t bytes)

{
  CS_ON;
  HAL_SPI_Transmit(&hspi1, &address, 1, 1000);//отправим адрес в шину
  HAL_SPI_Receive(&hspi1, pBuf, bytes, 1000);//получим данные в буфер
  CS_OFF;
}

//------------------------------------------------------------------------------------------------------------------------------------//
void NRF24_command (uint8_t cmd)
{
  CS_ON;
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);//отправим команду
  DelayMicro (10);
  CS_OFF;
}

//------------------------------------------------------------------------------------------------------------------------------------//
void NRF24_activate ()

{
	uint8_t activate = 0x50; //команда активирует команды R_RX_PL_WID, W_ACK_PAYLOAD и W_TX_PAYLOAD_NOACK
	uint8_t data = 0x73;
	CS_ON;
	HAL_SPI_Transmit(&hspi1, &activate, 1, 1000);//отправим команду activate
	DelayMicro (10);
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);//отправим 0x73 (стр. 46)
	CS_OFF;
}

//------------------------------------------------ф-я для включения режима приёма--------------------------------------------------//

void NRF24L01_RX_Mode(void)

{
  uint8_t regval=0;
  regval = NRF24_ReadReg (CONFIG); //считаем текущее значение регистра CONFIG
  regval |= (1<<PWR_UP)|(1<<PRIM_RX); //разбудим модуль и переведём его в режим приёмника, включив биты PWR_UP и PRIM_RX
  NRF24_WriteReg(CONFIG,regval);
  CE_SET;
  DelayMicro(150); //Задержка минимум 130 мкс
  NRF24_command (FLUSH_RX); //очистки буферов FIFO приёма
  NRF24_command (FLUSH_TX); //очистки буферов FIFO передачи
}

//---------------------------------------------------ф-я инициализация NRF----------------------------------------------------//
void NRF24_ini()

{
	CE_RESET; //отключим модуль (standby mode). все записи в регистры настроек производятся в этом режиме
	DelayMicro (5000);
	NRF24_WriteReg (STATUS, 0x0A); // 0b00001010 включим передатчик, включеним использование контрольной суммы (CRC = 1 байт)
	DelayMicro (5000);
	NRF24_WriteReg (EN_AA, 0x04); // включам автоподтверждение для 2 канала обмена
	NRF24_WriteReg (EN_RXADDR, 0x04); // включим испольхование 2 канала обмена
	NRF24_WriteReg (SETUP_AW, 0x03); // устанавливаем величину адреса - 5 байта
	NRF24_WriteReg (SETUP_RETR, 0x2E); //устанавливаем задержку = 750 uс и 14 повторов при неудачной отправке

	NRF24_activate (); //отправим команду ACTIVATE, за которой отправим значение 0x73. Активирует команды R_RX_PL_WID, W_ACK_PAYLOAD и W_TX_PAYLOAD_NOACK

	NRF24_WriteReg(FEATURE, 0x0); //0 - установка по умолчанию
	NRF24_WriteReg(DYNPD, 0); //0 - установка по умолчанию
	NRF24_WriteReg(STATUS, 0x70); //сбросим все флаги прерываний записью '1'
	NRF24_WriteReg(RF_CH, 76); // частота 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06); //мощность передачи: 0dBm, скорость передачи:1Mbps
	NRF24_Write_Buf (TX_ADDR, P1_ADDRESS, TX_ADR_WIDTH); //установим адрес передатчика
	NRF24_Write_Buf (RX_ADDR_P1, P1_ADDRESS, TX_ADR_WIDTH); //установим адрес получения данных (длиной TX_ADR_WIDTH байт)  для 1 канала
	NRF24_Write_Buf (RX_ADDR_P2, P2_ADDRESS, TX_ADR_WIDTH); //установим адрес получения данных (длиной TX_ADR_WIDTH байт)  для 2 канала
	NRF24_WriteReg(RX_PW_P2, TX_PLOAD_WIDTH); //установим количество байт в пакете для 2 канала

	NRF24L01_RX_Mode(); //уходим в режим приёмника
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
