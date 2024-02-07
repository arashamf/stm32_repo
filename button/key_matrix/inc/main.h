/*
 * main.h
 *
 *  Created on: 7 мар. 2020 г.
 *      Author: arashamf
 */

#ifndef MAIN_H_
#define MAIN_H_
#include "stm32f4xx.h"

#define KSCAN_port GPIOE->ODR // порт для вывода данных
#define KSCAN0_bit GPIO_ODR_ODR_0 // бит порта для вывода данных KSCAN0 - PE0
#define KSCAN1_bit GPIO_ODR_ODR_1 // бит порта для вывода данных KSCAN1 - PE1
#define KSCAN2_bit GPIO_ODR_ODR_2 // бит порта для вывода данных KSCAN2 - PE2
#define KSCAN3_bit GPIO_ODR_ODR_3 // бит порта для вывода данных KSCAN3 - PE3

#define KDATA_port GPIOE->IDR // порт для ввода данных
#define KDATA0_bit GPIO_IDR_IDR_4 // бит порта для ввода данных KDATA0 - PE4
#define KDATA1_bit GPIO_IDR_IDR_5 // бит порта для ввода данных KDATA1 - PE5
#define KDATA2_bit GPIO_IDR_IDR_6 // бит порта для ввода данных KDATA2 - PE6

// код кнопки, состоит из кода сканирования и считанных данных: // 0 - не нажата
#define KEY_s0_d0 0xE6 // 1110 0110
#define KEY_s0_d1 0xEA // 1110 1010
#define KEY_s0_d2 0xEC // 1110 1100
#define KEY_s1_d0 0xD6 // 1101 0110
#define KEY_s1_d1 0xDA // 1101 1010
#define KEY_s1_d2 0xDC // 1101 1100
#define KEY_s2_d0 0xB6 // 1011 0110
#define KEY_s2_d1 0xBA // 1011 1010
#define KEY_s2_d2 0xBC // 1011 1100
#define KEY_s3_d0 0x76 // 0111 0110
#define KEY_s3_d1 0x7A // 0111 1010
#define KEY_s3_d2 0x7C // 0111 1100


#endif /* MAIN_H_ */
