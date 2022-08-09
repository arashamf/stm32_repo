//Работа с UART. Управление бипером
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций C
#include "stdlib.h"
#include "string.h" // библиотека C работы со строками
#include "ctype.h" // библиотека C работы с символами
#include "DELAY_TIM7.h" // функции задержки
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070
#define FREQ_TIM3 100000 //частота таймера 3, Гц

char msg[130]; // массив для формирования сообщений для вывода по UART
char buffer[100]; // массив для хранения принятых данных
unsigned char ykaz_buffer = 0; // указатель на следующий символ в массиве
unsigned char receive_flag;  //флаг принятия данных
char beeper_freq[10];
short freq;
char beeper_length[10];
short length = 0;
short leght_beeper;
unsigned char error_flag; // флаг ошибки: 1,2,4 - ошибка (каждый бит показывает свою ошибку), 0 -
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
}

int main()
{
// настройка портов PD12, 13, 14, 15 для светодиодов
//GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; // на обычный выход*/
// настройка портов PD8 и PD9 под UART
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D
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

//настройка порта PE11 для управления бипером
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // включаем тактовый генератор порта E
GPIOE->MODER |= GPIO_MODER_MODER11_0; // настроим на выход порт PE11

// настройка таймера TIM3 генерации звука бипера
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // подали тактирование на TIM3
TIM3->PSC = 160 - 1; // значение предделителя (на вход TIM3 16000/160=100кГц)
TIM3->ARR = 0; // прерывание TIM3 раз в 0,01 мс
 TIM3->DIER |= TIM_DIER_UIE; // разрешим прерывание при обновлении таймера TIM3
//TIM3->CR1 |= TIM_CR1_CEN; // разрешим работу таймера TIM3
NVIC_EnableIRQ(TIM3_IRQn); // разрешаем прерывание в контроллере NVIC

// настройка таймера TIM4 длительности звучания бипера
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // подадим тактирование на TIM4
// срабатывание прерывания TIM4 через 1 мсек (1 кГц)
//тактовая частота=16000000; 1000=16000000/(предделитель)*(значение сравнения)=>(предделитель)*(значение сравнения)=16000
TIM4->PSC = 160 - 1; // значение предделителя (на вход TIM4 16000/160=100кГц)
TIM4->ARR = 100; // прерывание TIM4 раз в 1 мс
TIM4->DIER |= TIM_DIER_UIE; // разрешим прерывание при обновлении таймера TIM4
//TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера TIM4
NVIC_EnableIRQ(TIM4_IRQn); // разрешаем прерывание в контроллере NVIC

__enable_irq(); // разрешим прерывания глобально

UART3_PutRAMString("HELLO!:\r\n");
UART3_PutRAMString("Vvedite chastoty v Hz i dlitelnost zvyka v sekundax:\r\n");

while(1)
	{
	if (receive_flag)
		{
		// синтаксический анализ (парсинг)
		memset(beeper_freq, '\0', sizeof(beeper_freq)); // очистка массивов
		/*ф-я memset копирует младший байт символа ch в первые count символов массива buf. Возвращает указатель на массив.
		В данном случае используется, чтобы записать в массив нули*/
		memset(beeper_length, '\0', sizeof(beeper_length));

		//заполняем массивы. Записываем в массив частоту
		ptr = strtok(buffer, " _,."); // получим указатель на 1 лексему
		/*ф-я strtok возвращает указатель на следующую лексему в строке str1. Символы, образующие строку str2, является разделителями,
		определяющему лексему. Если лексемы не обнаружены возвращается нуль. Если такой символ найден. то он заменяется \0.
		Если ф-я strtok вернула не нуль, можно продолжить разбиение строки str на части, для чего повторно вызывается ф-я strtok,
		но вместо указателя на разбиваемую строку в качестве первого аргумента указывается \0. Ф-я strtok продолжит разбиение с запомненного адреса.*/
		strcpy(beeper_freq, ptr); // копируем лексему в массив
		/*ф-я strcpy копирует содержимое строки str2 в строку str1. Указатель str1 должен ссылаться на на строку str2,
		завершаемую нулевым символом.*/
		for(unsigned char i=0; i < strlen(beeper_freq); i++) //ф-я strlen возвращает длину строки без учёта символа /0
			beeper_freq[i] = tolower(beeper_freq[i]); //переводим лексему в нижний регистр
		freq = atoi(beeper_freq); // преобразуем номер устройства из символов в числа
		sprintf(msg, "beeper frequency  - %d Hz\r\n", freq); UART3_PutRAMString(msg); //тестовая печать

		//Записываем в массив длительность
		ptr = strtok('\0' , " _,."); // получим указатель на 2 лексему
		strcpy(beeper_length, ptr); // копируем лексему в массив
		for(unsigned char i=0; i < strlen(beeper_length); i++)
			beeper_length[i] = tolower(beeper_length[i]);// переводим лексему в нижний регистр
		length = atoi(beeper_length); // преобразуем номер устройства из символов в числа
		leght_beeper = length*1000;
		sprintf(msg, "time beeper - %d sek\r\n", length); UART3_PutRAMString(msg); //тестовая печать

		TIM3->ARR = (float) FREQ_TIM3 / (freq*2);
		TIM3->CR1 |= TIM_CR1_CEN; // разрешим работу таймера TIM3
		TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера TIM3

		receive_flag = 0; //флаг приема данных (команды): 1 - принято, 0 - нет
		/*for(unsigned char i=0; i < ykaz_buffer; i++)
			*(buffer + i) = '\0'; //очищаем буфер*/
		memset(buffer, '\0', sizeof(buffer));
		ykaz_buffer = 0; //устанавливаем указатель на начало буфера
		}
	}
}

void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
if(leght_beeper) // проверим счетчик времени
	{
	leght_beeper--; // уменьшаем счетчик времени
	}
}

// функция обработки прерывания TIM3
void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания

if(leght_beeper) // проверим счетчик времени
	{
	GPIOE->ODR ^= GPIO_ODR_ODR_11; // переключаем порт для бипера
	}
else
	{
	GPIOE->ODR &= ~GPIO_ODR_ODR_11; // выключим порт для бипера
	}
}
