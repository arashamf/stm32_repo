//Работа с UART. Прием данных. Проверка аппаратных флагов ошибок.
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций C
#include "stdlib.h"
#include "string.h" // библиотека C работы со строками
#include "ctype.h" // библиотека C работы с символами
#include "DELAY_TIM7.h" // функции задержки
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070

char msg[130]; // массив для формирования сообщений для вывода по UART
char buffer[100]; // массив для хранения принятых данных
unsigned char ykaz_buffer = 0; // указатель на следующий символ в массиве
unsigned char receive_flag;  //флаг принятия данных
char device_name[10]; // имя устройства
char device_num_str[10]; // номер устройства (строка)
unsigned int device_num; // номер устройства (число)
char device_funk[10]; // функция устройства
unsigned char error_flag; // флаг ошибки: 1,2,4 - ошибка (каждый бит показывает свою ошибку), 0 - нет ошибки
char * ptr; // указатель

// Передача байта по UART3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему (предыдущие данные переданы)
USART3->DR = c; // передаем байт
}

// Отправка строки символов, размещенной в RAM по UART3
void UART3_PutRAMString(char *str)
{
	char c;
	while((c = *(str++))) // отправляем символы строки последовательно, пока не достигнут символ конца строки "0x00"
		UART3_PutByte(c);
}

//работа с UART на ввод, обработка прерываний от USART3, прием байта
void USART3_IRQHandler(void)
{
unsigned long USART_SR_COPY; // переменная для хранения копии регистра SR (при считывании SR флаги сбрасываются)

USART_SR_COPY = USART3->SR; // переменная для хранения копии регистра SR
/* проверка аппаратных флагов ошибок*/
if(USART_SR_COPY & USART_SR_ORE) /*Overrun error. Флаг устанавливается, когда новые данные не успеваются считыватся. Генерирует прерывание и сбрасывается, когда считывается регистр статуса,
	а потом считывается регистр данных. Исчезают входящие данные только в сдвиговом регистре, т.е. пока не прочитаются данные из регистра данных, этим данным ничего не грозит.*/
	{
	sprintf(msg, "Error ORE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_NE) // Noise error flag. Шум в линии, вызывает прерывание. Сбрасывается также как и ORE.
	{
	sprintf(msg, "Error NE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_FE) // Frame Error. Данные побились при передачи. Например, частота плавает у передатчика и в линию идут некорректные данные. Дает прерывание, сбрасывается также как и ORE.
	{
	sprintf(msg, "Error FE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_PE) //Parity Error-ошибка контроля четности. Очищается аналогично, но тут надо перед сборосом подождать пока установится бит RXNE. Дает прерывание.
	{
	sprintf(msg, "Error PE\r\n"); UART3_PutRAMString(msg);
	}
/*окончание проверки аппаратных флагов ошибок*/
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
buffer[ykaz_buffer++] = USART3->DR; // записываем данные в буфер и увеличиваем указатель
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A))
	{
	receive_flag = 1; // 1- флаг принятия данных
	UART3_PutRAMString("get!\r");
	}
//delay_ms(2000);
}

int main()
{
// настройка портов PD12, 13, 14, 15 для светодиодов
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; // на обычный выход
// настройка портов PD8 и PD9 под UART
GPIOD->MODER |= GPIO_MODER_MODER8_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8;
GPIOD->MODER |= GPIO_MODER_MODER9_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // зададим альтернативную функцию - USART3_RX
// настройка UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE; // включить передатчик
USART3->CR1 |= USART_CR1_RE; // включить приемник
USART3->BRR = (17 << 4) + 6; //
USART3->CR1 |= USART_CR1_RXNEIE; // разрешаем прерывания при приеме
USART3->CR1 |= USART_CR1_UE; // включить USART
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
NVIC_EnableIRQ(USART3_IRQn); // разрешаем прерывание в контроллере прерываний

__enable_irq(); // разрешим прерывания глобально

UART3_PutRAMString("HELLO!:\r\n");
UART3_PutRAMString("UART_RX_INT:\r\n");

while(1)
	{
	if (receive_flag)
		{
		// синтаксический анализ (парсинг)
		// очистка массивов
		memset(device_name, '\0', sizeof(device_name));
		/*ф-я memset копирует младший байт символа ch в первые count символов массива buf. Возвращает указатель на массив.
		В данном случае используется, чтобы записать в массив нули*/
		memset(device_num_str, '\0', sizeof(device_num_str));
		memset(device_funk, '\0', sizeof(device_funk));

		//заполняем массивы. Записываем в массив название
		ptr = strtok(buffer , "_"); // получим указатель на 1 лексему
		/*ф-я strtok возвращает указатель на следующую лексему в строке str1. Символы, образующие строку str2, является разделителями,
		определяющему лексему. Если лексемы не обнаружены возвращается нуль. Если такой символ найден. то он заменяется \0.
		Если ф-я strtok вернула не нуль, можно продолжить разбиение строки str на части, для чего повторно вызывается ф-я strtok,
		но вместо указателя на разбиваемую строку в качестве первого аргумента указывается \0. Ф-я strtok продолжит разбиение с запомненного адреса.*/
		strcpy(device_name, ptr); // копируем лексему в массив
		/*ф-я strcpy копирует содержимое строки str2 в строку str1. Указатель str1 должен ссылаться на на строку str2,
		завершаемую нулевым символом.*/
		for(unsigned char i=0; i < strlen(device_name); i++) //ф-я strlen возвращает длину строки без учёта символа /0
			device_name[i] = tolower(device_name[i]); //переводим лексему в нижний регистр
		sprintf(msg, "device_name - %s\r\n", device_name); UART3_PutRAMString(msg); //тестовая печать

		//Записываем в массив номер устройства
		ptr = strtok('\0' , "_"); // получим указатель на 2 лексему
		strcpy(device_num_str, ptr); // копируем лексему в массив
		for(unsigned char i=0; i < strlen(device_num_str); i++)
			device_num_str[i] = tolower(device_num_str[i]);// переводим лексему в нижний регистр
		device_num = atoi(device_num_str); // преобразуем номер устройства из символов в числа
		sprintf(msg, "device_num - %d\r\n", device_num); UART3_PutRAMString(msg); //тестовая печать

		//Записываем в массив функцию устройства
		ptr = strtok('\0' , "_"); // получим указатель на 3 лексему
		strcpy(device_funk, ptr); // копируем лексему в массив
		for(unsigned char i=0; i < strlen(device_funk); i++)
			device_funk[i] = tolower(device_funk[i]); //переводим лексему в нижний регистр
		sprintf(msg, "device_funk - %s\r\n", device_funk); UART3_PutRAMString(msg); //тестовая печать
		error_flag = 0; // флаг ошибки: >0 - ошибка, 0 - нет

		// выполнение принятой команды
		if(strncmp(device_name, "led", 3) == 0) // проверим 1 лексему
			/*ф-я strncmp сравнивает первые n символов строк. Возвращает "0", если строки одинаковы, "меньше 0" -
			 * если строка1 меньше сроки2, "больше 0" - если строка1 больше строки 2*/
			{
			if(strncmp(device_funk, "on", 2) == 0) // проверим 3 лексему
				{
				switch(device_num) // проверим 2 лексему
					{
					case 1: GPIOD->BSRRL |= GPIO_BSRR_BS_12;
							break; //включаем 1
					case 2: GPIOD->BSRRL |= GPIO_BSRR_BS_13;
							break; //включаем 2
					case 3: GPIOD->BSRRL |= GPIO_BSRR_BS_14;
							break; //включаем 3
					case 4: GPIOD->BSRRL |= GPIO_BSRR_BS_15;
							break; //включаем 4
					default:error_flag += 2; // флаг ошибки: >0 - ошибка, 0 - нет
							break;
					}
				}
			else
				if(strncmp(device_funk, "off", 3) == 0) // проверим 3 лексему
					{
					switch(device_num) // проверим 2 лексему
						{
						case 1: GPIOD->BSRRH |= GPIO_BSRR_BS_12;
								break; //выключаем 1
						case 2: GPIOD->BSRRH |= GPIO_BSRR_BS_13;
								break; //выключаем 2
						case 3: GPIOD->BSRRH |= GPIO_BSRR_BS_14;
								break; //выключаем 3
						case 4: GPIOD->BSRRH |= GPIO_BSRR_BS_15;
								break; //выключаем 4
						default:error_flag += 2; // флаг ошибки: >0 - ошибка, 0 - нет
								break;
						}
					}
				else
					{
					error_flag += 4; // флаг ошибки: >0 - ошибка, 0 - нет
					}
				}
		else
			{
			error_flag += 1; // флаг ошибки: >0 - ошибка, 0 - нет
			}
	if(error_flag)
		{
		sprintf(msg, "Error %d\r\n", error_flag); UART3_PutRAMString(msg); // печать ошибки
		}
	receive_flag = 0; //флаг приема данных (команды): 1 - принято, 0 - нет
	for(unsigned char i=0; i < ykaz_buffer; i++)
		*(buffer + i) = '\0'; //очищаем буфер
	ykaz_buffer = 0; //устанавливаем указатель на начало буфера
		}
	}
}
