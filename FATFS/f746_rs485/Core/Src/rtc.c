#include "rtc.h"
extern FIL logfile;     //файловый объект
extern char wtext[TXT_BUFFER_SIZE];  //буффер записи на SD карту

extern uint8_t I2C_RTC_buffer [8];
extern char time [6]; //массив со значениями времени для отправки на RTC

//*******************************************************************************************************************************************//
void GetTime (I2C_HandleTypeDef hi, uint8_t adress, uint8_t sizebuf)
{
	uint8_t reg_adr = 0x0; //адрес регистра RTC
	//передача адреса ds3231
	while(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)adress, &reg_adr, 1, (uint32_t)0xFFFF)!= HAL_OK)
  {
		if (HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{
			sprintf (UART3_msg_TX , "write_error\r\n");
			UART3_SendString (UART3_msg_TX);
		}
	}
	
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
		
		//запись показателей времени	
	while (HAL_I2C_Master_Receive (&hi2c1, (uint16_t) adress, (uint8_t*)I2C_RTC_buffer, (uint16_t) sizebuf, (uint32_t)0xFFFF)!=HAL_OK)
	{
		if(HAL_I2C_GetError(&hi)!=HAL_I2C_ERROR_AF)
		{
			sprintf (UART3_msg_TX , "read_error\r\n");
			UART3_SendString (UART3_msg_TX);
		}
	}
	//перевод числа из двоично-десятичного представления  в обычное
	for (uint8_t count = 0; count < sizebuf; count++)
	{
		I2C_RTC_buffer[count] = RTC_ConvertToDec(I2C_RTC_buffer[count]);
	}
}

//*******************************************************************************************************************************************//
void SetTime (I2C_HandleTypeDef hi, uint8_t adress, char * time)
{
	//формирование сообщения для RTC
	uint8_t size = 6;
	I2C_RTC_buffer [0] = 0x0; //в первый элемент массива - адрес регистра RTC
	uint8_t ptr_buffer = size/2; //указатель на 4 элемент массива
	I2C_RTC_buffer [ptr_buffer] = 0; //обнулим 4 элемент массива
	for (uint8_t count = 0; count < size; count++) //на 0 и 1 итерации запишется значение часов
	{
		if (!(count%2)) //если 0, 2, 4 итерация
			{
			I2C_RTC_buffer [ptr_buffer] += 10 * ((*time++) - 0x30); //десятый разряд часов, минут, секунд
			}
		else
			{
			I2C_RTC_buffer [ptr_buffer] += ((*time++) - 0x30); //единичный разряд часов, минут, секунд
			I2C_RTC_buffer [ptr_buffer] = RTC_ConvertToBinDec(I2C_RTC_buffer [ptr_buffer]); //перевод в двоично-десятичное представление
			I2C_RTC_buffer [--ptr_buffer] = 0; //обнулим следующий элемент массива
			}		
	}
	
	//отправка данных
	while(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)adress, (uint8_t*)I2C_RTC_buffer, (size/2+1), (uint32_t)0xFFFF)!= HAL_OK)
  {
		if (HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{

			sprintf (UART3_msg_TX , "write_error\r\n");
			UART3_SendString (UART3_msg_TX);
		}
	}
	memset(UART3_msg_RX, '\0', sizeof(UART3_msg_RX)); //очистка приёмного буффера
}

//*************************************ф-я перевода числа из двоично-десятичного представления  в обычное*************************************//
uint8_t RTC_ConvertToDec(uint8_t digit)
{
	uint8_t ch = ((digit>>4)*10 + (0x0F & digit));
	return ch;
}

//*********************************ф-я перевода числа из обычного в двоично-десятичного представления*****************************************//
uint8_t RTC_ConvertToBinDec(uint8_t digit)
{
	uint8_t ch = ((digit/10) << 4) + (digit%10);
	return ch;
}


//*******************************************************************************************************************************************//
void read_reg_RTC (I2C_HandleTypeDef hi, uint8_t adress)
{
	uint8_t reg_adr = 0x2; //адрес регистра RTC 
	uint8_t sizebuf = 0x3; //количество байт для чтения
	//передача адреса ds3231
	while(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)adress, &reg_adr, 1, (uint32_t)0xFFFF)!= HAL_OK) //запись адреса RTC
  {
		if (HAL_I2C_GetError(&hi) != HAL_I2C_ERROR_AF)
		{
			sprintf (UART3_msg_TX , "write_error\r\n");
			UART3_SendString (UART3_msg_TX);
		}
	}
	while(HAL_I2C_GetState(&hi2c1)!=HAL_I2C_STATE_READY){}
		
	while (HAL_I2C_Master_Receive (&hi2c1, (uint16_t) adress, (uint8_t*)I2C_RTC_buffer, (uint16_t) sizebuf, (uint32_t)0xFFFF)!=HAL_OK)
	{
		if(HAL_I2C_GetError(&hi)!=HAL_I2C_ERROR_AF)
		{
			sprintf (UART3_msg_TX , "read_error\r\n");
			UART3_SendString (UART3_msg_TX);
		}
	}
	//перевод числа из двоично-десятичного представления  в обычное
	for (uint8_t count = 0; count < sizebuf; count++)
	{
		I2C_RTC_buffer[count] = RTC_ConvertToDec(I2C_RTC_buffer[count]);
		sprintf (UART3_msg_TX , "%x ", I2C_RTC_buffer[count]);
		UART3_SendString (UART3_msg_TX);
	}
	HAL_UART_Transmit (&huart3, (uint8_t*)"\r\n", strlen("\r\n"), 0xFFFF);
}

//*******************************************************************************************************************************************//
void edit_RTC_data (I2C_HandleTypeDef hi, uint8_t adress)
{	
	char *ptr;	
	uint8_t errflag = 1; //флаг ошибки данных
	ptr = strtok(UART3_msg_RX , ": "); //Ф-ия возвращает указатель на первую найденную лексему в строке. Если не найдено, то возвращается пустой указатель
	ptr = strtok(NULL, " \r\n"); // для последующего вызова можно передать NULL, тогда функция продолжит поиск в оригинальной строке
	strncpy (time, ptr, 6);  //копирование 6 символов цифр в массив для отправки
	for (uint8_t count = 0; count < 6; count++) //проверка чисел на корректность значений
	{
		if (count == 0)
		{
			if (!((time [count] > 47) && (time [count] < 51))) //если символ меньше 0 и больше 2
			{
				errflag = 0;
				break;
			}
		}
		else
		{
			if (count == 1)
			{
				if (time [0] == 50) //если в старшем разряде часов 2
				{
					if (!((time [count] > 47) && (time [count] < 52))) //если символ меньше 0 и больше 3
					{
						errflag = 0;
						break;
					}
				}
				else
				{
					if (!((time [count] > 47) && (time [count] < 58))) //если символ меньше 0 и больше 3
					{
						errflag = 0;
						break;
					}
				}
			}
			else
			{
				if ((count == 2) || (count == 4))
				{
					if (!((time [count] > 47) && (time [count] < 54))) //если символ меньше 0 и больше 5
					{
					errflag = 0;
					break;
					}	
				}	
				else
				{
					if (!((time [count] > 47) && (time [count] < 58)))  //если символ меньше 0 и больше 9
					{
						errflag = 0;
						break;
					}
				}
			}
		}
	}			
	if (errflag == 1)
	{
		sprintf (UART3_msg_TX , "data_ok\r\n");	
		UART3_SendString (UART3_msg_TX);
		SetTime (hi, adress, time); //отправка данных времени на мк RTC
		sprintf (wtext, "%u:%u:%u set time\r\n", RTC_ConvertToDec (I2C_RTC_buffer [3]),RTC_ConvertToDec (I2C_RTC_buffer [2]), RTC_ConvertToDec (I2C_RTC_buffer [1]));
		write_reg (&logfile , wtext);
	}
	else
	{
		sprintf (UART3_msg_TX , "%s- data_error\r\n", time);	
		UART3_SendString (UART3_msg_TX);
	}
}
//*******************************************************************************************************************************************//
