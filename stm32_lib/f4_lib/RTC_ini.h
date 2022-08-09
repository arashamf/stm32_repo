/*
 * RTC_ini.h
 *
 *  Created on: 2 ���. 2020 �.
 *      Author: ALEXEY
 */
#ifndef RTC_INI_H_
#define RTC_INI_H_
#include "delay_lib.h"
void RTC_ini ();
void RTC_clock_ini ();
void RTC_clock_ini_SPL ();
uint8_t UART_RTC_set_time_SPL (char *);
void RTC_set_date_SPL ();
void ALARMA_ini_SPL (uint8_t );
void ALARMA_ini_IRQ (uint8_t );
void ALARMB_ini_IRQ (uint8_t );
void ALARM_ini_IRQ_SPL (uint8_t );
void WAKEUP_ini_SPL ();
RTC_TimeTypeDef RTC_set_time; //инициализируем шаблон структуры установки времени RTC
RTC_DateTypeDef RTC_set_date; //инициализируем шаблон структуры установки даты RTC
extern RCC_ClocksTypeDef RCC_Clocks;  //объявим структуру c тактовыми частотами

#define EXTI_PR_PR22 ((uint32_t)0x400000) // объявление флага внешнего прерывания линии EXTI22 для таймера Wakeup

#endif /* RTC_INI_H_ */
