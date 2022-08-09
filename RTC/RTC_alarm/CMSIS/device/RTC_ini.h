/*
 * RTC_ini.h
 *
 *  Created on: 2 ���. 2020 �.
 *      Author: ALEXEY
 */
#ifndef RTC_INI_H_
#define RTC_INI_H_
void RTC_clock_ini ();
void RTC_clock_ini_SPL ();
char UART_RTC_set_time (char *);
void ALARM_ini (uint8_t );
void ALARM_ini_SPL (uint8_t );
RTC_TimeTypeDef RTC_set_time; //инициализируем шаблон структуры установки времени

#endif /* RTC_INI_H_ */
