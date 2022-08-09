#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "LCD_STM32.h" 
#include "stdio.h"

// определения выводов, к которым подключается LCD: D4-D7, R/S, R/W, E
#define LCD_CHAR_D4_PORT GPIOE->ODR
#define LCD_CHAR_D4_PIN GPIO_ODR_ODR_7 //вывод PE7
#define LCD_CHAR_D5_PORT GPIOE->ODR
#define LCD_CHAR_D5_PIN GPIO_ODR_ODR_10  //вывод PE10
#define LCD_CHAR_D6_PORT GPIOE->ODR
#define LCD_CHAR_D6_PIN GPIO_ODR_ODR_14 //вывод PE14
#define LCD_CHAR_D7_PORT GPIOE->ODR
#define LCD_CHAR_D7_PIN GPIO_ODR_ODR_15 //вывод PE15
#define LCD_CHAR_RS_PORT GPIOA->ODR
#define LCD_CHAR_RS_PIN GPIO_ODR_ODR_2 //вывод PA2
#define LCD_CHAR_RW_PORT GPIOA->ODR
#define LCD_CHAR_RW_PIN GPIO_ODR_ODR_3 //вывод PA3
#define LCD_CHAR_E_PORT GPIOA->ODR
#define LCD_CHAR_E_PIN GPIO_ODR_ODR_1 //вывод PA1

// макросы записи бит в порты
#define SetBit(PORT, BIT) PORT|=BIT // установка бита
#define ClrBit(PORT, BIT) PORT&=~BIT  // сброс бита

//функция настроки портов МК, подключаемых к LCD
void LCD_STM_INIT () //ф-я инициализации портов IO МК для подключения LCD
{
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // запускаем тактовый генератор GPIOA
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // запускаем тактовый генератор GPIOE

// настройка портов на выход 
GPIOE->MODER |= GPIO_MODER_MODER7_0; // D4
GPIOE->MODER |= GPIO_MODER_MODER10_0; // D5
GPIOE->MODER |= GPIO_MODER_MODER14_0; // D6
GPIOE->MODER |= GPIO_MODER_MODER15_0; // D7
GPIOA->MODER |= GPIO_MODER_MODER2_0; // RS
GPIOA->MODER |= GPIO_MODER_MODER3_0; // RW
GPIOA->MODER |= GPIO_MODER_MODER1_0; // E	
}

// функция записи команды/данных
void LCD_CHAR_wr(char LCD_CHAR_data, unsigned char LCD_CHAR_RS) //LCD_CHAR_RS==1 - передаём данные, 0 - команду
{
// управляющие сигналы
if(LCD_CHAR_RS)
	SetBit(LCD_CHAR_RS_PORT, LCD_CHAR_RS_PIN); // R/S = 1 (данные)
else
	ClrBit(LCD_CHAR_RS_PORT, LCD_CHAR_RS_PIN); // R/S = 0 (команда)
ClrBit(LCD_CHAR_RW_PORT, LCD_CHAR_RW_PIN); // R/W = 0 (запись)
// передача старшей тетрады данных
if(LCD_CHAR_data & 0x10)
	SetBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
else
	ClrBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
if(LCD_CHAR_data & 0x20)
	SetBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
else
	ClrBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
if(LCD_CHAR_data & 0x40)
	SetBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
else
	ClrBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
if(LCD_CHAR_data & 0x80)
	SetBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
else
	ClrBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
// управляющие сигналы
SetBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 1
delay_us(2);
ClrBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 0
// передача младшей тетрады данных
if(LCD_CHAR_data & 0x01)
	SetBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
else
	ClrBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
if(LCD_CHAR_data & 0x02)
	SetBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
else
	ClrBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
if(LCD_CHAR_data & 0x04)
	SetBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
else
	ClrBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
if(LCD_CHAR_data & 0x08)
	SetBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
else
	ClrBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
// управляющие сигналы
SetBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 1
delay_us(2); //задержка
ClrBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 0
}


// функция шага инициализационной последовательности
void LCD_CHAR_init_step(char LCD_CHAR_data) 
{
// управляющие сигналы
ClrBit(LCD_CHAR_RS_PORT, LCD_CHAR_RS_PIN); // R/S = 0 (команда)
ClrBit(LCD_CHAR_RW_PORT, LCD_CHAR_RW_PIN); // R/W = 0 (запись)
// передача старшей тетрады данных
if(LCD_CHAR_data & 0x10)
	SetBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
else
	ClrBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
if(LCD_CHAR_data & 0x20)
	SetBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
else
	ClrBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
if(LCD_CHAR_data & 0x40)
	SetBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
else
	ClrBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
if(LCD_CHAR_data & 0x80)
	SetBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
else
	ClrBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
// управляющие сигналы
SetBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 1
delay_us(2); 
ClrBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 0
}

// функция инициализации LCD
void LCD_CHAR_init(unsigned char columns) //columns - число символов в строке
{
// интерфейс - 4-битный, строк - 2, шрифт - 5х7 точек
// инициализирующая последовательность
delay_ms(30); 
LCD_CHAR_init_step(0x30);  //установка 8 разрядной шины дисплея
delay_us(40); 
LCD_CHAR_init_step(0x20);
delay_us(40); 
// настройка индикатора
 LCD_CHAR_wr(0x28, 0); // 4-битный интерфейс
delay_us(40);
// параметры отображения
LCD_CHAR_wr(0x0C, 0);  //включить дисплей, выключить курсор
delay_us(40); 
LCD_CHAR_wr(0x06, 0); //дисплей не сдвигать
delay_us(40); 
LCD_CHAR_wr(0x01, 0); //очистить дисплей и установить курсор в начальную позицию
delay_ms(2);
}

// функция установки позиции индикатора для вывода
void LCD_CHAR_gotoxy(unsigned char column, unsigned char row) //column - столбец, row - строка
{
if (row == 0)
	LCD_CHAR_wr(0x80 + 0x00 + column, 0);
else
	if (row == 1)
		LCD_CHAR_wr(0x80 + 0x40 + column, 0);
delay_us(40); 
}

// функция вывода строки str из RAM на индикатор с заданной позиции
void LCD_CHAR_puts(char *str)
{
char c;
// отправляем символы строки последовательно, пока не достигнут символ конца строки "0x00"
while((c = *(str++)))
	{
	LCD_CHAR_wr(c, 1); // запись данных
	delay_us(40); 
	}
}

// функция задержки в мкс
void delay_us(unsigned int us)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // включаем тактовый генератор
TIM7->CNT = 0; // сбросим счетчик таймера
TIM7->PSC = 16-1; //делитель, 16000000/16 = 1000000 = 1 мкс
TIM7->ARR = us;
TIM7->EGR |= TIM_EGR_UG; /* Бит устанавливается программно, сбрасывается автоматически аппаратно; Перезапускает счётчик таймера,
счётчик прескалера и обновляет регистры с предзагрузкой (обновляет теневые регистры значениями из соответствующих регистров).
Если бит UDIS=1, счётчики сбрасываются, но теневые регистры не обновляются.*/
TIM7->SR &= ~TIM_SR_UIF; // сбросим флаг обновления
TIM7->CR1 |= TIM_CR1_CEN; // включаем таймер на счет
while(!(TIM7->SR & TIM_SR_UIF)) {} //  если установлен флаг обновления таймера в "1", то цикл заканчивается
TIM7->CR1 &= ~TIM_CR1_CEN; // выключаем таймер
TIM7->SR &= ~TIM_SR_UIF; // сбросим флаг события обновления
}

// функция задержки в мс
void delay_ms(unsigned int ms)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // включаем тактовый генератор
TIM7->CNT = 0; // сбросим счетчик таймера
TIM7->PSC = 16000-1; // делитель, 16000000/16000 = 1000 = 1 мс
TIM7->ARR = ms;
TIM7->EGR |= TIM_EGR_UG; // переинициализация таймера
TIM7->SR &= ~TIM_SR_UIF; // сбросим флаг обновления
TIM7->CR1 |= TIM_CR1_CEN; // включаем таймер на счет
while(!(TIM7->SR & TIM_SR_UIF)) {} //можно так: while((TIM7->SR & TIM_SR_UIF)==0){};
TIM7->CR1 &= ~TIM_CR1_CEN; // выключаем таймер
TIM7->SR &= ~TIM_SR_UIF; // сбросим флаг события обновления
}
