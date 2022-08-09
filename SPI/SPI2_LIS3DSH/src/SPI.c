/*
 * SPI.c
 *
 *  Created on: 18 ����. 2020 �.
 *      Author: ALEXEY
 */
#include "stm32f4xx.h"
#include "SPI.h"
#include "delay_lib.h"

//-------------------------������� ������������� SPI2-------------------------------//
void SPI2_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //����������� ���� B

	GPIO_InitTypeDef 	GPIO_ini_SPI2;
	GPIO_ini_SPI2.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |  GPIO_Pin_15; //������� ��� 12-15 ����� B ��� ���������
	GPIO_ini_SPI2.GPIO_Mode = GPIO_Mode_AF; //���� � ��� 12-15 �� ����. �����
	GPIO_ini_SPI2.GPIO_Speed = GPIO_High_Speed; //�������� �����
	GPIO_ini_SPI2.GPIO_OType = GPIO_OType_PP; //����� � ��������� (push-pull)
	GPIO_ini_SPI2.GPIO_PuPd = GPIO_PuPd_NOPULL; //���� ��� ��������

	GPIO_Init (GPIOB, &GPIO_ini_SPI2);

	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource12), GPIO_AF_SPI2); //PB12 - SPI2_NSS
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource13), GPIO_AF_SPI2); //PB13 - SPI2_SCK
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource14), GPIO_AF_SPI2); //PB14 - SPI2_MISO
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource15), GPIO_AF_SPI2); //PB15 - SPI2_MOSI

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // ��������� �������� ��������� SPI2

	SPI_InitTypeDef		SPI2_Init_user;
	SPI2_Init_user.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //�� ���� ������ � ��������������� ������
	SPI2_Init_user.SPI_Mode = SPI_Mode_Master; //�� � ������ ������
	SPI2_Init_user.SPI_DataSize = SPI_DataSize_8b; //8 ��� ������
//	SPI2_Init_user.SPI_CPOL = SPI_CPOL_High; //���� ��� CPOL ����������, ����� SCK ����� ��������� �������� ������ ��� �������
	SPI2_Init_user.SPI_CPOL = SPI_CPOL_Low;
	SPI2_Init_user.SPI_CPHA = SPI_CPHA_1Edge;
//	SPI2_Init_user.SPI_CPHA = SPI_CPHA_2Edge; //���� ���������� ��� CPHA , ������ ���� �� ������ SCK (��������� ����, ���� ��� CPOL �������, ���������� ����, ���� ��� CPOL ����������) �������� ������� ������� MSBit.
	SPI2_Init_user.SPI_NSS = SPI_NSS_Soft;  // ����������� ���������� ������� NSS
	SPI2_Init_user.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; //������� �������� ������� ����
	SPI2_Init_user.SPI_FirstBit = SPI_FirstBit_MSB; // ������ ���� ������� ���
	SPI2_Init_user.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI2_Init_user);

	SPI_SSOutputCmd(SPI2, DISABLE);
	SPI_Cmd(SPI2, ENABLE); //������� SPI1
}

//-----------------------------�-� ������ ����� �� SPI2---------------------------//
uint8_t SPI2_read_byte (uint8_t adress)
{
	uint8_t SPI2_data= 0;
	SPI_SSOutputCmd(SPI2, ENABLE); //������� ����� CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //������� ���� ����� �������� �� ����
	*(uint8_t*)&SPI2->DR = (adress + LIS3DSH_Cmd_Read); //���������� ����� �������� � ��� ������
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //������� ���� ����� �������� �� ����
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //������� ���� ����� ����� ����
	(void) SPI2->DR; //��������� ������� �������� ������
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //������� ���� ����� �������� �� ����
	*(uint8_t*)&SPI2->DR = 0x00; //�������� ����
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //������� ���� ����� �������� �� ����
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //������� ���� ����� ����� ����
	SPI2_data = *(uint8_t*)&SPI2->DR; //�������� �������� ������
	SPI_SSOutputCmd(SPI2, DISABLE); //�������� ����� CS
	return SPI2_data;
}

//-----------------------------�-� ������ ����� �� SPI2---------------------------//
void SPI2_write_byte (uint8_t adress, uint8_t data)
{
	SPI_SSOutputCmd(SPI2, ENABLE); //������� ����� CS
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI2->DR = (adress + LIS3DSH_Cmd_Write); //���������� ����� �������� � ��� ������
	//SPI2->DR = (adress + LIS3DSH_Cmd_Write);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI2->DR;  //��������� ������� �������� ������
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	*(uint8_t*)&SPI2->DR = data; //���������� �������� ������
	//SPI2->DR = data;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {};
	(void) SPI2->DR; //��������� ������� �������� ������
	SPI_SSOutputCmd(SPI2, DISABLE); //�������� ����� CS
}

//-----------------------------�-� ������ ������ ������� �� SPI2---------------------------//
void SPI2_read_array(uint8_t adress, char *data, uint8_t data_len)  //data_len - ���������� ���� ����������� ��������
{
	for(uint8_t i = 0x0; i <= data_len; i++)
		{
		if(i == data_len)
			{
			SPI_SSOutputCmd(SPI2, DISABLE); //�������� ����� CS;
			break;
			}
		*data++ = SPI2_read_byte (adress+i);
		}
}

//-------------------------������� ������������� SPI1-------------------------------//
void SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //����������� ���� �

	SPI_I2S_DeInit (SPI1);

	GPIO_InitTypeDef 	GPIO_ini_SPI1;
	GPIO_ini_SPI1.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //������� ��� 5-7 ����� � ��� ���������
	GPIO_ini_SPI1.GPIO_Mode = GPIO_Mode_AF; //���� � ��� 5-7 �� ����. �����
	GPIO_ini_SPI1.GPIO_Speed = GPIO_Fast_Speed; //�������� �����
	GPIO_ini_SPI1.GPIO_OType = GPIO_OType_PP; //����� � ��������� (push-pull)
	GPIO_ini_SPI1.GPIO_PuPd = GPIO_PuPd_NOPULL; //���� ��� ��������

	GPIO_Init (GPIOA, &GPIO_ini_SPI1);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource4), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource5), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource6), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource7), GPIO_AF_SPI1);


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // ��������� �������� ��������� SPI1

	SPI_InitTypeDef		SPI_Init_user;
	SPI_Init_user.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //�� ���� ������ � ��������������� ������
	SPI_Init_user.SPI_Mode = SPI_Mode_Master; //�� � ������ ������
	SPI_Init_user.SPI_DataSize = SPI_DataSize_8b; //8 ��� ������
	SPI_Init_user.SPI_CPOL = SPI_CPOL_High; //���� ��� CPOL ����������, ����� SCK ����� ��������� �������� ������ ��� �������
//	SPI_Init_user.SPI_CPOL = SPI_CPOL_Low;
//	SPI_Init_user.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_Init_user.SPI_CPHA = SPI_CPHA_2Edge; //���� ���������� ��� CPHA , ������ ���� �� ������ SCK (��������� ����, ���� ��� CPOL �������, ���������� ����, ���� ��� CPOL ����������) �������� ������� ������� MSBit.
	SPI_Init_user.SPI_NSS = SPI_NSS_Soft;  // ����������� ���������� ������� NSS
	SPI_Init_user.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; //������� �������� ������� ����
	SPI_Init_user.SPI_FirstBit = SPI_FirstBit_MSB; // ������ ���� ������� ���
	SPI_Init_user.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_Init_user); //�������������� SPI � ������������ � ���������� ����������� � ��������� SPI_Init
	SPI_SSOutputCmd(SPI2, DISABLE); //�������� ����� CS

	SPI_Cmd(SPI1, ENABLE);

//	NVIC_EnableIRQ(SPI1_IRQn);


}
