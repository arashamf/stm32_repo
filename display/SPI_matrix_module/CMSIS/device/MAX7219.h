/*
 * MAX7219.h
 *
 *  Created on: 18 мар. 2020 г.
 *      Author: ALEXEY
 */

#ifndef MAX7219_H_
#define MAX7219_H_
void send_data_displey (uint8_t, uint8_t, uint8_t );
void send_nop ();
void init_MAX7219 ();
void test_displays (uint16_t );
void shutdown_display (uint8_t );
void putchar_display (uint8_t number_display, uint8_t reg_string, uint8_t *numb);

#define reg_No_Op 0x00 //регистр No-Op
#define reg_Decode 0x09 //регистр настройки декодирования
#define reg_Intensity 0x0A //регистр настройки яркости дисплея
#define reg_Scan_Limit 0x0B //регистр настройки количества отображаемых чисел
#define reg_Shutdown 0x0C //регистр отключения дисплея
#define reg_Test 0x0F //регистр запуска тестирования дисплея
#define reg_string 0x01 //номер 1 регистра строк матрицы (нижняя строка)

#define CS_SET_HIGH (SET_BIT (GPIOA->ODR, GPIO_ODR_ODR_4)) //установим высокий уровень на линии CS
#define CS_SET_LOW (CLEAR_BIT (GPIOA->ODR, GPIO_ODR_ODR_4)) //установим низкий уровень на линии CS

enum displays {display1=1, display2, display3, display4}; //создадим перечисляемый тип с именами дисплеев (displays-дескриптор)
enum displays number_display; //number_display - переменная типа displays

#endif /* MAX7219_H_ */
