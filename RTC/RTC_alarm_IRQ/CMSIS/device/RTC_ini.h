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
uint8_t UART_RTC_set_time_SPL (char *);
void RTC_set_date_SPL ();
void ALARM_ini_IRQ (uint8_t );
void ALARM_ini_SPL (uint8_t );
RTC_TimeTypeDef RTC_set_time; //инициализируем шаблон структуры установки времени RTC
RTC_DateTypeDef RTC_set_date; //инициализируем шаблон структуры установки даты RTC

#endif /* RTC_INI_H_ */
