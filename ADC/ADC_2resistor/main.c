//Программный запуск АЦП
//
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "LCD_STM32.h"
#include "UART_STM32.h"
#include  "string.h"

char buffer [16];
char msg[20]; // массив для формирования сообщений для вывода по UART
const float Uref=3.3;

int main()
{
// настройка порта АЦП
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // запускаем тактовый генератор GPIOC
GPIOC->MODER |=GPIO_MODER_MODER1 | GPIO_MODER_MODER2; // настраиваем пины PC1 и PC2 на аналоговый режим (ADC123_IN11 и ADC123_IN12)
// настройка АЦП
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // включаем тактовый генератор
//установлено при сбросе: регистр CR1-RES[1:0] = 0: разрешение 12 бит; регистр CR2: ALIGN = 0 - правое выравнивание;
// в качестве опорного напряжения используется AVDD и AVSS
ADC1->SQR1 &= ~ADC_SQR1_L; // длина последовательности регулярных каналов = 1;
/*ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3; // канал 11 для последовательности регулярных каналов = 1;
ADC1->SQR3 |= ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3; // канал 12 для последовательности регулярных каналов = 2;*/
ADC1->CR2 |= ADC_CR2_ADON; // включаем АЦП

UART3_init (); //инициализация UART
UART3_PutRAMString("ADC:\r\n");

LCD_STM_INIT (); //настрока портов МК, подключаемых к LCD
LCD_CHAR_init(16); // инициализация	16 символьной строки

while(1)
	{
	float voltage = 0;
	ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3; // канал 11 для последовательности регулярных каналов = 1;
	ADC1->CR2 |= ADC_CR2_SWSTART; // программный запуск АЦП
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // ждем пока преобразование завершится
	voltage = Uref * ADC1->DR / 4096;
	sprintf(msg,("ADC_channel_11=%f_V\r\n"), voltage); // выводим значение АЦП в строку
	UART3_PutRAMString(msg); // выводим строку по UART
	sprintf(buffer,("%ld; U=%.2fV"),ADC1->DR, voltage);
	LCD_CHAR_wr(0x01, 0); //очистить дисплей и установить курсор в начальную позицию
	delay_ms(1);
	LCD_CHAR_gotoxy(0,0); //перемещаем курсор в начало верхней строки
	LCD_CHAR_puts("ADC_channel_11:");
	LCD_CHAR_gotoxy(0,1); //перемещаем курсор в начало нижней строки
	LCD_CHAR_puts (buffer);
	memset(buffer, '\0', sizeof(buffer));
	memset(msg, '\0', sizeof(msg));
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	delay_ms (1000); // задержка

	ADC1->SQR3 |= ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_3; // канал 12 для последовательности регулярных каналов = 1;
	ADC1->CR2 |= ADC_CR2_SWSTART; // программный запуск АЦП
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // ждем пока преобразование завершится
	voltage = Uref * ADC1->DR / 4096;
	sprintf(msg,("ADC_channel_12=%f_V\r\n"), voltage); // выводим значение АЦП в строку
	UART3_PutRAMString(msg); // выводим строку по UART
	sprintf(buffer,("%ld; U=%.2fV"),ADC1->DR, voltage);
	LCD_CHAR_wr(0x01, 0); //очистить дисплей и установить курсор в начальную позицию
	delay_ms(1);
	LCD_CHAR_gotoxy(0,0); //перемещаем курсор в начало верхней строки
	LCD_CHAR_puts("ADC_channel_12:");
	LCD_CHAR_gotoxy(0,1); //перемещаем курсор в начало нижней строки
	LCD_CHAR_puts (buffer);
	memset(buffer, '\0', sizeof(buffer));
	memset(msg, '\0', sizeof(msg));
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	delay_ms (1000); // задержка
	}
}
