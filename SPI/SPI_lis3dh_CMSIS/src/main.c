// Работа с SPI®. Работа с акселерометром LIS3DH. Настройка и считывание данных.

#include "stm32F4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "DELAY_TIM7.h"
#include "UART_STM32.h"
#include "string.h" // описание функций работы со строками
typedef unsigned char byte;

#define BANDWITH 57600 //пропускная способность шины UART
#define CPU_FREQUENCY 24000000L // тактовая частота

#define CS_ON (GPIOE->BSRRH |= GPIO_BSRR_BS_3); //активируем CS
#define CS_OFF (GPIOE->BSRRL |= GPIO_BSRR_BS_3); //сбросим CS
#define LIS3DH_Cmd_Read 0x80 // команда чтения
#define LIS3DH_Cmd_Write 0x00 // команда записи

// регистры LIS3DH
#define LIS3DH_Reg_WHO_AM_I 0x0F // регистр идентификации LIS3DH
#define LIS3DH_Reg_Status 0x27 // регистр статуса LIS3DH
#define LIS3DH_CTRL_REG4 0x20 //регистр управления LIS3DH №4
#define LIS3DH_CTRL_REG1 0x21 //регистр управления LIS3DH №1
#define LIS3DH_CTRL_REG2 0x22 //регистр управления LIS3DH №2
#define LIS3DH_CTRL_REG3 0x23 //регистр управления LIS3DH №3
#define LIS3DH_CTRL_REG5 0x24 //регистр управления LIS3DH №5
#define LIS3DH_CTRL_REG6 0x25 //регистр управления LIS3DH №6
#define LIS3DH_Reg_OUT_X_L  0x28 // регистр ускорения X_L
#define LIS3DH_Reg_OUT_X_H  0x29 // регистр ускорения X_H
#define LIS3DH_Reg_OUT_Y_L  0x2A // регистр ускорения Y_L
#define LIS3DH_Reg_OUT_Y_H  0x2B // регистр ускорения Y_H
#define LIS3DH_Reg_OUT_Z_L  0x2C // регистр ускорения Z_L
#define LIS3DH_Reg_OUT_Z_H 0x2D // регистр ускорения Z_H

#define LIS3DH_Reg_Status_Reg_ZYXDA 0x08 //адрес регистра статусов измерения  ускорений

char Accel_X ; // ускорение по X
char Accel_Y; // ускорение по Y
char Accel_Z; // ускорение по Z


__IO byte LIS3DH_SPI_Cmd; // команда, отправляемая по SPI
__IO byte LIS3DH_Status1 = 0; // статус LIS3DH
__IO byte LIS3DH_Status2 = 0; // статус LIS3DH
__IO byte LIS3DH_Status = 0; // статус LIS3DH

//константы настройки пинов РА5-РА7 на альт.функцию AF5
#define GPIO_AFRL_AF5_SPI1_MOSI 0x50000000 // зададим конкретную альтернативную функцию - PA7/SPI1_MOSI
#define GPIO_AFRL_AF5_SPI1_MISO 0x05000000 // зададим конкретную альтернативную функцию - PA6/SPI1_MISO
#define GPIO_AFRL_AF5_SPI1_SCK 0x00500000 // зададим конкретную альтернативную функцию - PA5/SPI1_SCK

char msg[100]; // массив для формирования сообщений для вывода по UART
//--------------------------------------------------------------------------------------------------------
int main(void)
{
// настройка вывода CS
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN); // GPIOE
SET_BIT(GPIOE->MODER, GPIO_MODER_MODER3_0); // на выход
//SET_BIT(GPIOE->MODER, GPIO_OSPEEDER_OSPEEDR3_0); //скорость быстрая
CS_OFF; //сигнал на вывод CS (выбор микросхемы), 1 - режим ожидания
//-------------------------------------------------------------------------------------------------------
// тест
/*RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0; // на выход
GPIOD->MODER |= GPIO_MODER_MODER13_0; // на выход
GPIOD->MODER |= GPIO_MODER_MODER14_0; // на выход*/
//--------------------------------------------------------------------------------------------------------
// настройка порта А для интерфейса SPI1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); // запускаем тактовый генератор GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER7_1); // настраиваем вывод 7 на альтернативную функцию
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER6_1); //настраиваем вывод 6 на альтернативную функцию
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5_1); //настраиваем вывод 5 на альтернативную функцию
//SET_BIT(GPIOA->MODER, GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1); //скорость быстрая
SET_BIT (GPIOA->AFR[0], GPIO_AFRL_AF5_SPI1_MOSI); //настройка пина А7 на альт. функцию SPI1_MOSI
SET_BIT (GPIOA->AFR[0], GPIO_AFRL_AF5_SPI1_MISO); //настройка пина А6 на альт. функцию SPI1_MISO
SET_BIT (GPIOA->AFR[0], GPIO_AFRL_AF5_SPI1_SCK); //настройка пина А5 на альт. функцию SPI1_SCK

// настройка SPI1
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_SPI1EN); // запускаем тактовый генератор SPI1
SET_BIT (SPI1->CR1, SPI_CR1_MSTR); // мк в режиме мастер
SET_BIT (SPI1->CR1, SPI_CR1_BR); //зададим делитель частоты шины
SET_BIT (SPI1->CR1, SPI_CR1_CPOL); //если бит CPOL сброшен, вывод SCK имеет низкоуровневое состояние простоя
SET_BIT (SPI1->CR1, SPI_CR1_CPHA); /*Если установлен бит CPHA , второй край на выводе SCK (спадающий фрон, если бит CPOL сброшен, восходящий край, если бит CPOL установлен)
является стробом захвата MSBit. Данные фиксируются при наступлении второго тактового перехода. Если бит CPHA сброшен, первй край на выводе SCK (спадающий фронт, если бит CPOL установлен,
нарастающий фронт, если бит CPOL сбрасывается) - это захват MSBit стробоскоп. Данные фиксируются при наступлении первого тактового перехода*/
CLEAR_BIT (SPI1->CR1, SPI_CR1_DFF); //8 бит данные
CLEAR_BIT (SPI1->CR1, SPI_CR1_LSBFIRST); // первый идет старший бит
SET_BIT (SPI1->CR1, SPI_CR1_SSM); // программное управление выводом NSS, значение на выводе NSS определяется битом SPI_CR1_SSI
SET_BIT (SPI1->CR1, SPI_CR1_SSI); //значение этого бита принудительно устанавливается на вывод NSS, 1 - внутренняя выборка ведомого
CLEAR_BIT (SPI1->CR1, SPI_CR1_BIDIMODE); //режим 2-х проводный однонаправленный полнодуплексный
CLEAR_BIT (SPI1->CR1, SPI_CR1_BIDIOE);
SET_BIT (SPI1->CR1, SPI_CR1_SPE); // разрешаем работу SPI
UART2_init (CPU_FREQUENCY, BANDWITH);
UART2_PutRAMString("Acceleration XYZ:\r\n");
//-----------------------------------------------------------------------------------------------------------------------------//
//чтение из регистра WHO_AM_I
CS_ON; //включам чип-селект
LIS3DH_SPI_Cmd = LIS3DH_Cmd_Read + LIS3DH_Reg_WHO_AM_I; //операция чтения + адрес регистра WHO_AM_I
SPI1->DR = LIS3DH_SPI_Cmd;  //отправляем команду
while(!(SPI1->SR & SPI_SR_TXE)) {}; // ожидаем установки флага TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ожидаем установки флага RXNE
LIS3DH_Status1 = SPI1->DR; //получаем содержимое регистра WHO_AM_I
SPI1->DR = 0x00; //передаём пустой байт
while(!(SPI1->SR & SPI_SR_TXE)){}; // ожидаем установки флага TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ожидаем установки флага RXNE
LIS3DH_Status2 = SPI1->DR; //получаем содержимое регистра WHO_AM_I
CS_OFF;  //отключаем чип-селект
sprintf (msg, ("LIS3DH_registr_Who_I_am = %x %x \r\n"), LIS3DH_Status1, LIS3DH_Status2); // выводим значение счетчика в строку и увеличиваем его
UART2_PutRAMString (msg); // выводим строку по UART
delay_ms(100);
//-----------------------------------------------------------------------------------------------//
// настройка акселерометра
CS_ON; // сигнал на вывод CS (выбор микросхемы), 0-SPI communication mode
// структура команды, отправляемая по SPI: бит 7=1 - операция чтения; бит 6 = 1 - инкремент адреса при многобайтном чтении, биты 0-5: - адрес регистра
LIS3DH_SPI_Cmd = LIS3DH_Cmd_Write + LIS3DH_CTRL_REG4;
SPI1->DR = LIS3DH_SPI_Cmd; // передаем команду
while(!(SPI1->SR & SPI_SR_TXE)) {}; // ожидаем установки флага TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ожидаем установки флага RXNE
(void)SPI1->DR; // считывание принятых ненужных данных
SPI1->DR = 0b01110111; // передаем данные, активный режим по всем трем осям
while(!(SPI1->SR & SPI_SR_TXE)) {}; // ожидаем установки флага TXE
while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ожидаем установки флага RXNE
(void)SPI1->DR; // считывание принятых ненужных данных
CS_OFF; // сигнал на вывод CS , 1 - неактивный режим SPI
delay_ms(100);
//---------------------------------------------------------------------------------------------------//
// основной цикл программы
while(1)
	{
	//---------------------------------------------------------------------------------------------------------//
	CS_ON; // сигнал на вывод CS , 0 - активный режим SPI
	LIS3DH_SPI_Cmd = LIS3DH_Cmd_Read + LIS3DH_Reg_Status; //команда чтения + адрес регистра статуса
	SPI1->DR = LIS3DH_SPI_Cmd; // передаем команду
	while(!(SPI1->SR & SPI_SR_TXE)) {}; // ожидаем установки флага TXE
	while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ожидаем установки флага RXNE
	(void)SPI1->DR;  // считывание принятых ненужных данных
	SPI1->DR = 0x00; //передаём пустой байт
	while(!(SPI1->SR & SPI_SR_TXE)) {}; // ожидаем установки флага TXE
	while(!(SPI1->SR & SPI_SR_RXNE)) {}; // ожидаем установки флага RXNE
	LIS3DH_Status = SPI1->DR; //получение статуса LIS3DH
	CS_OFF // сигнал на вывод CS , 1 - неактивный режим SPI
	sprintf (msg, ("LIS3DH_Status=%x \r\n"), LIS3DH_Status);
	UART2_PutRAMString (msg); // выводим строку по UART
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
	sprintf (msg, ("Accel_X = %x\r\n"), Accel_X); // выводим значение счетчика в строку и увеличиваем его
	UART2_PutRAMString (msg); // выводим строку по UART*/
	delay_ms(500); // задержка
	}
}
