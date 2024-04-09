/*
 * MAX7219.c
 *
 *  Created on: 18 мар. 2020 г.
 *      Author: ALEXEY
 */
#include "stm32f4xx.h"
#include "MAX7219.h"
#include "delay_lib.h"

enum displays number_display; //number_display - переменная типа displays

//--------------ф-я отправки пустого сообщения в регистр no-op--------------------//
void send_nop ()
{
	*(uint16_t*)&SPI2->DR = (uint16_t)((reg_No_Op << 8)+ 0x00); //отправим 0 в регистр no-op
	while(!(SPI2->SR & SPI_SR_TXE)) {};  //проверяем установку флага SPI_SR_TXE в 1 (буффер TXE пуст)
}

//--------------------ф-я отправки данных на дисплей ----------------------//
void send_data_displey (uint8_t number_display, uint8_t addres, uint8_t data)
{
	for (uint8_t count = 4; count > 0; count--)
		{
		if (number_display == count) //если номер дисплея равен номеру итерации, то отправим полезные данные
			{
			*(uint16_t*)&SPI2->DR = (uint16_t)((addres << 8)+ data); //адрес регистра + данные для записи
			while(!(SPI2->SR & SPI_SR_TXE)) {};  //проверяем установку флага SPI_SR_TXE в 1 (буффер TXE пуст)
			}
		else
			send_nop ();  //если не равен, то оправим пустое сообщение в регистр no-op для сдвига сообщений
		}
	delay_us (2); //небольшая задержка

	CS_SET_HIGH; // фиксируем переданые данные
	CS_SET_LOW; //сбрасываем CS
}

//-------------------------инициализация дисплея MAX7219----------------------//
void init_MAX7219 ()
{
	//включим дисплеи в нормальный режим
	send_data_displey (display1, reg_Shutdown, 0x1);
	send_data_displey (display2, reg_Shutdown, 0x1);
	send_data_displey (display3, reg_Shutdown, 0x1);
	send_data_displey (display4, reg_Shutdown, 0x1);

	//выйдем из режима тестирования
	send_data_displey (display1, reg_Test, 0x0);
	send_data_displey (display2, reg_Test, 0x0);
	send_data_displey (display3, reg_Test, 0x0);
	send_data_displey (display4, reg_Test, 0x0);

	//отключим режим декодирования для строк от 0 до 7
	send_data_displey (display1, reg_Decode, 0x00);
	send_data_displey (display2, reg_Decode, 0x00);
	send_data_displey (display3, reg_Decode, 0x00);
	send_data_displey (display4, reg_Decode, 0x00);

	//настроим яркость свечения - 5/32
	send_data_displey (display1, reg_Intensity, 0x1);
	send_data_displey (display2, reg_Intensity, 0x1);
	send_data_displey (display3, reg_Intensity, 0x1);
	send_data_displey (display4, reg_Intensity, 0x1);

	//количество отображаемых строк 8
	send_data_displey (display1, reg_Scan_Limit, 0x07);
	send_data_displey (display2, reg_Scan_Limit, 0x07);
	send_data_displey (display3, reg_Scan_Limit, 0x07);
	send_data_displey (display4, reg_Scan_Limit, 0x07);
}

//--------------------------ф-я тестирования дисплеев--------------------------//
void test_displays (uint16_t delay)
{
	send_data_displey (display1, reg_Test, 0x1); //запустим режим тестирования дисплея 1
	delay_ms (delay); //длительность задержки в мс
	send_data_displey (display1, reg_Test, 0x0); //остановим режим тестирования дисплея 1
	send_data_displey (display2, reg_Test, 0x1);//запустим режим тестирования дисплея 2
	delay_ms (delay);
	send_data_displey (display2, reg_Test, 0x0);//остановим режим тестирования дисплея 2
	send_data_displey (display3, reg_Test, 0x1);//запустим режим тестирования дисплея 3
	delay_ms (delay);
	send_data_displey (display3, reg_Test, 0x0);//остановим режим тестирования дисплея 3
	send_data_displey (display4, reg_Test, 0x1);//запустим режим тестирования дисплея 4
	delay_ms (delay);
	send_data_displey (display4, reg_Test, 0x0);//остановим режим тестирования дисплея 4
}

//----------------------------ф-я отключения дисплеев----------------------------------//
void shutdown_display (uint8_t number_display)
{
	send_data_displey (number_display, reg_Shutdown, 0x0);
}

//--------------------------ф-я отправки символов на дисплей--------------------------//
void putchar_display (uint8_t number_display, uint8_t first_registr_string, uint8_t (*numb) [8], uint8_t number_digit)
{
	for (uint8_t count = 0; count < 8; count++)
	{
		send_data_displey (number_display, (first_registr_string + count), *(*(numb + number_digit)+count)); //получим и передадим значение из двухмерного массива
	}

}
