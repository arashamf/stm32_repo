/*
 * nrf24l01.c
 *
 *  Created on: 9 янв. 2021 г.
 *      Author: arashamf
 */
#include "nrf24l01.h"

uint8_t P1_ADDRESS[5] = {0xE7, 0xD3, 0xB6, 0xB7, 0x01}; //адрес передатчика канала 1. Адреса должны быть уникальные, для каналов Pipe2-5 заносим только младший байт, а старшие берутся из адреса канала pipe1
uint8_t P2_ADDRESS[5] = {0xE7, 0xD3, 0xB6, 0xB7, 0x02}; //адрес передатчика канала 2



/*uint8_t NRF24_ReadReg(uint8_t address)
{
  uint8_t data = 0; //считанные данные
  uint8_t cmd = 0; //передадим любой байт
  address |= READ_REG; //включим бит чтения в адрес
  data = SPI1_read_byte (address, READ_REG); //отправим адрес в шину
  if (address != STATUS)//если адрес равен адрес регистра статус то и возвращаем его состояние
  {
    cmd=0xFF;
    data = SPI1_read_byte (cmd, READ_REG);
  }
  return data;
}*/

//------------------------------------------------------------------------------------------------------------------------------------//
/*void NRF24_WriteReg(uint8_t address, uint8_t data)
{
  address |= WRITE_REG;//включим бит записи в адрес
  SPI1_write_byte (address, WRITE_REG, data); //отправим данные в шину
}*/

//------------------------------------------------------------------------------------------------------------------------------------//
/*void NRF24_Write_Buf(uint8_t address, uint8_t *pBuf, uint8_t bytes)

{

	address |= W_REGISTER;//включим бит записи в адрес
	CS_ON;
	HAL_SPI_Transmit(&hspi1, &address, 1, 1000);//отправим адрес в шину
	DelayMicro(1);
	HAL_SPI_Transmit(&hspi1, pBuf, bytes, 1000);//отправим данные в буфер
	CS_OFF;
}*/

//------------------------------------------------------------------------------------------------------------------------------------//
/*void NRF24_Read_Buf (uint8_t address, uint8_t *pBuf, uint8_t bytes)

{
  CS_ON;
  HAL_SPI_Transmit(&hspi1, &address, 1, 1000);//отправим адрес в шину
  HAL_SPI_Receive(&hspi1, pBuf, bytes, 1000);//получим данные в буфер
  CS_OFF;
}*/

//------------------------------------------------------------------------------------------------------------------------------------//
void NRF24_command (uint8_t cmd)
{
  CS_ON; //включим вывод CS
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
  *(uint8_t*)&SPI1->DR = cmd; ////отправим команду
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {};
  (void) SPI1->DR;  //считываем регистр ненужных данных
  delay_us (10);
  CS_OFF; //отключим вывод CS
}

//------------------------------------------------------------------------------------------------------------------------------------//
void NRF24_activate ()

{
	uint8_t activate = 0x50; //команда активирует команды R_RX_PL_WID, W_ACK_PAYLOAD и W_TX_PAYLOAD_NOACK
	uint8_t data = 0x73;
	CS_ON; //включим вывод CS
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI1->DR = activate; //отправим команду activate
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI1->DR;  //считываем регистр ненужных данных
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	delay_us (10);
	*(uint8_t*)&SPI1->DR = data; //отправим 0x73 (стр. 46)
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI1->DR; //считываем регистр ненужных данных
	CS_OFF; //отключим вывод CS
}

//------------------------------------------------ф-я для включения режима приёма--------------------------------------------------//

void NRF24L01_RX_Mode()

{
  uint8_t regval=0;
  regval = SPI1_read_byte (CONFIG, READ_REG); //считаем текущее значение регистра CONFIG
  regval |= (1<<PWR_UP)|(1<<PRIM_RX); //разбудим модуль и переведём его в режим приёмника, включив биты PWR_UP и PRIM_RX
  SPI1_write_byte (CONFIG, regval, WRITE_REG);
  delay_us (150); //Задержка минимум 130 мкс
  NRF24_command (FLUSH_RX); //очистки буферов FIFO приёма
  NRF24_command (FLUSH_TX); //очистки буферов FIFO передачи
}

//---------------------------------------------------ф-я инициализация NRF----------------------------------------------------//
void NRF24_ini()

{
	CE_RESET; //отключим модуль (standby mode). все записи в регистры настроек производятся в этом режиме
	delay_us (5000);
	SPI1_write_byte (STATUS, 0x0A, WRITE_REG); // 0b00001010 включим передатчик, включеним использование контрольной суммы (CRC = 1 байт)
	delay_us (5000);
	SPI1_write_byte (EN_AA, 0x04, WRITE_REG); // включам автоподтверждение для 2 канала обмена
	SPI1_write_byte (EN_RXADDR, 0x04, WRITE_REG); // включим испольхование 2 канала обмена
	SPI1_write_byte (SETUP_AW, 0x03, WRITE_REG); // устанавливаем величину адреса - 5 байта
	SPI1_write_byte (SETUP_RETR, 0x2E, WRITE_REG); //устанавливаем задержку = 750 uс и 14 повторов при неудачной отправке

	NRF24_activate (); //отправим команду ACTIVATE, за которой отправим значение 0x73. Активирует команды R_RX_PL_WID, W_ACK_PAYLOAD и W_TX_PAYLOAD_NOACK

	SPI1_write_byte (FEATURE, 0x0, WRITE_REG); //0 - установка по умолчанию
	SPI1_write_byte (DYNPD, 0, WRITE_REG); //0 - установка по умолчанию
	SPI1_write_byte (STATUS, 0x70, WRITE_REG); //сбросим все флаги прерываний записью '1'
	SPI1_write_byte (RF_CH, 76, WRITE_REG); // частота 2476 MHz
	SPI1_write_byte (RF_SETUP, 0x06, WRITE_REG); //мощность передачи: 0dBm, скорость передачи:1Mbps
	SPI1_write_array (TX_ADDR, P1_ADDRESS, TX_ADR_WIDTH, WRITE_REG); //установим адрес передатчика
	SPI1_write_array (RX_ADDR_P1, P1_ADDRESS, TX_ADR_WIDTH, WRITE_REG); //установим адрес получения данных (длиной TX_ADR_WIDTH байт)  для 1 канала
	SPI1_write_array (RX_ADDR_P2, P2_ADDRESS, TX_ADR_WIDTH, WRITE_REG); //установим адрес получения данных (длиной TX_ADR_WIDTH байт)  для 2 канала
	SPI1_write_byte (RX_PW_P2, TX_PLOAD_WIDTH, WRITE_REG); //установим количество байт в пакете для 2 канала

	NRF24L01_RX_Mode(); //уходим в режим приёмника
}


