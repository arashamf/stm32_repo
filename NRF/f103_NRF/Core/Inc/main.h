/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */


/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define L01_CE_Pin GPIO_PIN_0
#define L01_CE_GPIO_Port GPIOB
#define L01_IRQ_Pin GPIO_PIN_1
#define L01_IRQ_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define LED_GPIO_PORT GPIOC
#define LED_PIN GPIO_PIN_13

#define CS_ON HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define CS_OFF HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
#define CE_RESET HAL_GPIO_WritePin(L01_CE_GPIO_Port, L01_CE_Pin, GPIO_PIN_RESET)
#define CE_SET HAL_GPIO_WritePin(L01_CE_GPIO_Port, L01_CE_Pin, GPIO_PIN_SET)

#define W_REGISTER 0x20 //запись в регистр
#define R_REGISTER 0x0 //чтение регистра
#define RD_RX_PLOAD 0x61 //команда чтения буфера FIFO. После команды по MISO начинают передаваться данные с младшего байта. После окончания чтения буфер FIFO освобождается
#define WR_TX_PLOAD 0xA0 //команда записи данных в буфер FIFO. После команды по MOSI данные передаются в модуль и записываются в буфер FIFO. Загружать данные с младшего байта
#define FLUSH_TX 0xE1 //команда очистки буфера FIFO, предназначенного для передачи
#define FLUSH_RX 0xE2 //команда очистки буфера FIFO, предназначенного для приёма

#define CONFIG 0x00 //'Config' register address
#define EN_AA 0x01 //регистр, который использует технологию Enhanced ShockBurst и включает автоподтверждение для определённого канала обмена
#define EN_RXADDR 0x02 //регистр включает использование каналов
#define SETUP_AW 0x03 //регистр является общим для всех каналов обмена и устанавливает величину адресов приёмника и передатчика в байтах (от 3 до 5)
#define SETUP_RETR 0x04 //регистр, который устанавливает параметры для повторных передач пакета при их неудачной отправке
#define RF_CH 0x05 //регистр устанавливает частоту работы передатчика
#define RF_SETUP 0x06 //регистр используется для настройки скорости и мощности передачи
#define STATUS 0x07 //регистр используется для того, чтобы узнать то или иное состояние, но порой нужно будет его использовать и для записи
#define OBSERVE_TX 0x08 //'Transmit observe' register
#define RX_ADDR_P0 0x0A //регистр адреса канала 0 обмена информации для приёмных устройств
#define RX_ADDR_P1 0x0B //регистр адреса канала 1 обмена информации для приёмных устройств
#define RX_ADDR_P2 0x0C //регистр адреса канала 2 обмена информации для приёмных устройств
#define RX_ADDR_P3 0x0D //регистр адреса канала 3 обмена информации для приёмных устройств
#define RX_ADDR_P4 0x0E //регистр адреса канала 4 обмена информации для приёмных устройств
#define RX_ADDR_P5 0x0F //регистр адреса канала 5 обмена информации для приёмных устройств
#define TX_ADDR 0x10 //регистр с адресом для передатчи
#define RX_PW_P0 0x11 //регистр c количествоv байт в пакете для канала 0
#define RX_PW_P1 0x12 //регистр c количествоv байт в пакете для канала 1
#define RX_PW_P2 0x13 //регистр c количествоv байт в пакете для канала 2
#define RX_PW_P3 0x14 //регистр c количествоv байт в пакете для канала 3
#define RX_PW_P4 0x15 //регистр c количествоv байт в пакете для канала 4
#define RX_PW_P5 0x16 //регистр c количествоv байт в пакете для канала 5
#define FIFO_STATUS 0x17 //регистр состояния FIFO
#define DYNPD 0x1C //регистр управляет использованием режима динамического количества байт в пакете для каждого канала обмена
#define FEATURE 0x1D //регистр используется в режиме динамического количества байтов в пакете и содержит некоторые настройки данного режима

#define PRIM_RX 0x00 //управление RX/TX (1: PRX, 0: PTX)
#define PWR_UP 0x01 //бит PWR_UP – бит управления включением передатчика. 1: POWER UP, 0:POWER DOWN
#define MAX_RT 0x10 //бит прерывания, происходящее при исчерпывании максимального количество повторных отправок пакета передатчиком
#define TX_DS 0x20 //бит прерывания, при успешной отправке пакета приёмнику в передающем рессивере. Если включено автоподтвержение, то это прерывание происходит после получения подтверждения от приёмника
#define RX_DR 0x40 //бит прерывания, возникающее при получении пакета в приёмнике в тот момент, когда пакет появится в приёмном буфере

#define TX_ADR_WIDTH 5 //количество байт в адресе передатчика
#define TX_PLOAD_WIDTH 2 //количество байт в пакете для каждого канала обмена информацией
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
