// ������ � SPI�. ������ � �������������� LIS3DH. ��������� � ���������� ������.

#include "stm32F4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "DELAY_TIM7.h"
#include "UART_STM32.h"
#include "string.h" // �������� ������� ������ �� ��������
typedef unsigned char byte;

#define BANDWITH 57600 //���������� ����������� ���� UART
#define CPU_FREQUENCY 24000000L // �������� �������

#define CS_ON (GPIOE->BSRRH |= GPIO_BSRR_BS_3); //���������� CS
#define CS_OFF (GPIOE->BSRRL |= GPIO_BSRR_BS_3); //������� CS
#define LIS3DH_Cmd_Read 0x80 // ������� ������
#define LIS3DH_Cmd_Write 0x00 // ������� ������

// �������� LIS3DH
#define LIS3DH_Reg_WHO_AM_I 0x0F // ������� ������������� LIS3DH
#define LIS3DH_Reg_Status 0x27 // ������� ������� LIS3DH
#define LIS3DH_CTRL_REG4 0x20 //������� ���������� LIS3DH �4
#define LIS3DH_CTRL_REG1 0x21 //������� ���������� LIS3DH �1
#define LIS3DH_CTRL_REG2 0x22 //������� ���������� LIS3DH �2
#define LIS3DH_CTRL_REG3 0x23 //������� ���������� LIS3DH �3
#define LIS3DH_CTRL_REG5 0x24 //������� ���������� LIS3DH �5
#define LIS3DH_CTRL_REG6 0x25 //������� ���������� LIS3DH �6
#define LIS3DH_Reg_OUT_X_L  0x28 // ������� ��������� X_L
#define LIS3DH_Reg_OUT_X_H  0x29 // ������� ��������� X_H
#define LIS3DH_Reg_OUT_Y_L  0x2A // ������� ��������� Y_L
#define LIS3DH_Reg_OUT_Y_H  0x2B // ������� ��������� Y_H
#define LIS3DH_Reg_OUT_Z_L  0x2C // ������� ��������� Z_L
#define LIS3DH_Reg_OUT_Z_H 0x2D // ������� ��������� Z_H

#define LIS3DH_Reg_Status_Reg_ZYXDA 0x08 //����� �������� �������� ���������  ���������

char Accel_X ; // ��������� �� X
char Accel_Y; // ��������� �� Y
char Accel_Z; // ��������� �� Z


__IO byte LIS3DH_SPI_Cmd; // �������, ������������ �� SPI
__IO byte LIS3DH_Status1 = 0; // ������ LIS3DH
__IO byte LIS3DH_Status2 = 0; // ������ LIS3DH
__IO byte LIS3DH_Status = 0; // ������ LIS3DH

//��������� ��������� ����� ��5-��7 �� ����.������� AF5
#define GPIO_AFRL_AF5_SPI1_MOSI 0x50000000 // ������� ���������� �������������� ������� - PA7/SPI1_MOSI
#define GPIO_AFRL_AF5_SPI1_MISO 0x05000000 // ������� ���������� �������������� ������� - PA6/SPI1_MISO
#define GPIO_AFRL_AF5_SPI1_SCK 0x00500000 // ������� ���������� �������������� ������� - PA5/SPI1_SCK

char msg[100]; // ������ ��� ������������ ��������� ��� ������ �� UART
//--------------------------------------------------------------------------------------------------------
int main(void)
{
// ��������� ������ CS
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN); // GPIOE
SET_BIT(GPIOE->MODER, GPIO_MODER_MODER3_0); // �� �����
//SET_BIT(GPIOE->MODER, GPIO_OSPEEDER_OSPEEDR3_0); //�������� �������
CS_OFF; //������ �� ����� CS (����� ����������), 1 - ����� ��������
//-------------------------------------------------------------------------------------------------------
// ����
/*RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0; // �� �����
GPIOD->MODER |= GPIO_MODER_MODER13_0; // �� �����
GPIOD->MODER |= GPIO_MODER_MODER14_0; // �� �����*/
//--------------------------------------------------------------------------------------------------------
// ��������� ����� � ��� ���������� SPI1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); // ��������� �������� ��������� GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER7_1); // ����������� ����� 7 �� �������������� �������
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER6_1); //����������� ����� 6 �� �������������� �������
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5_1); //����������� ����� 5 �� �������������� �������
//SET_BIT(GPIOA->MODER, GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1); //�������� �������
SET_BIT (GPIOA->AFR[0], GPIO_AFRL_AF5_SPI1_MOSI); //��������� ���� �7 �� ����. ������� SPI1_MOSI
SET_BIT (GPIOA->AFR[0], GPIO_AFRL_AF5_SPI1_MISO); //��������� ���� �6 �� ����. ������� SPI1_MISO
SET_BIT (GPIOA->AFR[0], GPIO_AFRL_AF5_SPI1_SCK); //��������� ���� �5 �� ����. ������� SPI1_SCK

// ��������� SPI1
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_SPI1EN); // ��������� �������� ��������� SPI1
SET_BIT (SPI1->CR1, SPI_CR1_MSTR); // �� � ������ ������
SET_BIT (SPI1->CR1, SPI_CR1_BR); //������� �������� ������� ����
SET_BIT (SPI1->CR1, SPI_CR1_CPOL); //���� ��� CPOL �������, ����� SCK ����� �������������� ��������� �������
SET_BIT (SPI1->CR1, SPI_CR1_CPHA); /*���� ���������� ��� CPHA , ������ ���� �� ������ SCK (��������� ����, ���� ��� CPOL �������, ���������� ����, ���� ��� CPOL ����������)
�������� ������� ������� MSBit. ������ ����������� ��� ����������� ������� ��������� ��������. ���� ��� CPHA �������, ����� ���� �� ������ SCK (��������� �����, ���� ��� CPOL ����������,
����������� �����, ���� ��� CPOL ������������) - ��� ������ MSBit ����������. ������ ����������� ��� ����������� ������� ��������� ��������*/
CLEAR_BIT (SPI1->CR1, SPI_CR1_DFF); //8 ��� ������
CLEAR_BIT (SPI1->CR1, SPI_CR1_LSBFIRST); // ������ ���� ������� ���
SET_BIT (SPI1->CR1, SPI_CR1_SSM); // ����������� ���������� ������� NSS, �������� �� ������ NSS ������������ ����� SPI_CR1_SSI
SET_BIT (SPI1->CR1, SPI_CR1_SSI); //�������� ����� ���� ������������� ��������������� �� ����� NSS, 1 - ���������� ������� ��������
CLEAR_BIT (SPI1->CR1, SPI_CR1_BIDIMODE); //����� 2-� ��������� ���������������� ���������������
CLEAR_BIT (SPI1->CR1, SPI_CR1_BIDIOE);
SET_BIT (SPI1->CR1, SPI_CR1_SPE); // ��������� ������ SPI
UART2_init (CPU_FREQUENCY, BANDWITH);
UART2_PutRAMString("Acceleration XYZ:\r\n");
//-----------------------------------------------------------------------------------------------------------------------------//
//������ �� �������� WHO_AM_I
CS_ON; //������� ���-������
LIS3DH_SPI_Cmd = LIS3DH_Cmd_Read + LIS3DH_Reg_WHO_AM_I; //�������� ������ + ����� �������� WHO_AM_I
SPI1->DR = LIS3DH_SPI_Cmd;  //���������� �������
while(!(SPI1->SR & SPI_SR_TXE)) {}; // ������� ��������� ����� TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ������� ��������� ����� RXNE
LIS3DH_Status1 = SPI1->DR; //�������� ���������� �������� WHO_AM_I
SPI1->DR = 0x00; //������� ������ ����
while(!(SPI1->SR & SPI_SR_TXE)){}; // ������� ��������� ����� TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ������� ��������� ����� RXNE
LIS3DH_Status2 = SPI1->DR; //�������� ���������� �������� WHO_AM_I
CS_OFF;  //��������� ���-������
sprintf (msg, ("LIS3DH_registr_Who_I_am = %x %x \r\n"), LIS3DH_Status1, LIS3DH_Status2); // ������� �������� �������� � ������ � ����������� ���
UART2_PutRAMString (msg); // ������� ������ �� UART
delay_ms(100);
//-----------------------------------------------------------------------------------------------//
// ��������� �������������
CS_ON; // ������ �� ����� CS (����� ����������), 0-SPI communication mode
// ��������� �������, ������������ �� SPI: ��� 7=1 - �������� ������; ��� 6 = 1 - ��������� ������ ��� ������������ ������, ���� 0-5: - ����� ��������
LIS3DH_SPI_Cmd = LIS3DH_Cmd_Write + LIS3DH_CTRL_REG4;
SPI1->DR = LIS3DH_SPI_Cmd; // �������� �������
while(!(SPI1->SR & SPI_SR_TXE)) {}; // ������� ��������� ����� TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ������� ��������� ����� RXNE
(void)SPI1->DR; // ���������� �������� �������� ������
SPI1->DR = 0b01110111; // �������� ������, �������� ����� �� ���� ���� ����
while(!(SPI1->SR & SPI_SR_TXE)) {}; // ������� ��������� ����� TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ������� ��������� ����� RXNE
(void)SPI1->DR; // ���������� �������� �������� ������
CS_OFF; // ������ �� ����� CS , 1 - ���������� ����� SPI
delay_ms(100);
//---------------------------------------------------------------------------------------------------//
// �������� ���� ���������
while(1)
	{
	//---------------------------------------------------------------------------------------------------------//
	CS_ON; // ������ �� ����� CS , 0 - �������� ����� SPI
	LIS3DH_SPI_Cmd = LIS3DH_Cmd_Read + LIS3DH_Reg_Status; //������� ������ + ����� �������� �������
	SPI1->DR = LIS3DH_SPI_Cmd; // �������� �������
	while(!(SPI1->SR & SPI_SR_TXE)) {}; // ������� ��������� ����� TXE
	while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ������� ��������� ����� RXNE
	(void)SPI1->DR;  // ���������� �������� �������� ������
	SPI1->DR = 0x00; //������� ������ ����
	while(!(SPI1->SR & SPI_SR_TXE)) {}; // ������� ��������� ����� TXE
	while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ������� ��������� ����� RXNE
	LIS3DH_Status = SPI1->DR; //��������� ������� LIS3DH
	CS_OFF // ������ �� ����� CS , 1 - ���������� ����� SPI
	sprintf (msg, ("LIS3DH_Status=%x \r\n"), LIS3DH_Status);
	UART2_PutRAMString (msg); // ������� ������ �� UART
	//----------------------------------------------------------------------------------------------------------------//
	if (!(LIS3DH_Status & LIS3DH_Reg_Status_Reg_ZYXDA))
		continue;

	CS_ON;
	LIS3DH_SPI_Cmd = LIS3DH_Cmd_Read + LIS3DH_Reg_OUT_X_L;
	SPI1->DR = LIS3DH_SPI_Cmd;
	while(!(SPI1->SR & SPI_SR_TXE)) {};
	while(!(SPI1->SR & SPI_SR_RXNE)) {};
	(void)SPI1->DR;
	SPI1->DR = 0x00;
	while(!(SPI1->SR & SPI_SR_TXE)) {};
	while(!(SPI1->SR & SPI_SR_RXNE)) {};
	Accel_X = SPI1->DR;
	CS_OFF;
	sprintf (msg, ("Accel_X = %x\r\n"), Accel_X); // ������� �������� �������� � ������ � ����������� ���
	UART2_PutRAMString (msg); // ������� ������ �� UART*/
	delay_ms(500); // ��������
	}
}
