//Программный запуск АЦП
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "LCD_STM32.h"
#include "UART_STM32.h"
#include  "string.h"
#define PERIOD 4096 //период импульсов ШИМ
#define GPIO_AFRH_PIN12_AF2 0x00020000

char buffer [16];
char msg[20]; // массив для формирования сообщений для вывода по UART
const float Uref=3.3;
unsigned int duty = 4096;

int main()
{
// настройка порта АЦП
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // запускаем тактовый генератор GPIOC
GPIOC->MODER |=GPIO_MODER_MODER1; // настраиваем пины PC1 на аналоговый режим (ADC123_IN11)
// настройка АЦП
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // включаем тактовый генератор
//установлено при сбросе: регистр CR1-RES[1:0] = 0: разрешение 12 бит; регистр CR2: ALIGN = 0 - правое выравнивание;
// в качестве опорного напряжения используется AVDD и AVSS
ADC1->SQR1 &= ~ADC_SQR1_L; // длина последовательности регулярных каналов = 1;
ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3; // канал 11 для последовательности регулярных каналов = 1;
ADC1->CR2 |= ADC_CR2_ADON; // включаем АЦП

UART3_init (); //инициализация UART
UART3_PutRAMString("ADC:\r\n");

LCD_STM_INIT (); //настрока портов МК, подключаемых к LCD
LCD_CHAR_init(16); // инициализация	16 символьной строки

/*инициализация таймера 4*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //тактируем таймер4
// Для получения времени 0,1 мс (10 кГц часота ШИМ) выберем следующие параметры: (1/16000000Гц * 16 * 100 = 0,1 мс)
TIM4->PSC = 16-1; // предделитель TIM4
TIM4->ARR = PERIOD; //значение перезагрузки TIM4 равно периоду импульса ШИМ
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
/*TIM4_CCMR1 - регистр1 настройки режима захвата/сравнения TIM4; биты 4-6 канала1 и биты 14-12 канала2 конфигурируют выходной режим сравнения, в данном случает в эти биты записано 110,
что задаёт каналам 1 и 2 TIM4 режим ШИМ1; если счетный регистр работает на сложение: при CNT<CCR1 сигнал OC1ref=1, иначе OC1ref=0*/
TIM4->CCER |= TIM_CCER_CC1E;
//TIM4_CCER регистр включения режима захвата/сравнения таймера4; бит 0 и 5 запускает выходной режим сравнения канала 1 и 2 TIM4 на пины 12 и 13 порта D
TIM4->CCR1 = duty; //TIM4_CCR1 регистр1 захвата/сравнения TIM4, здесь зададим длительность ШИМ
TIM4->CR1 |= TIM_CR1_CEN; //включим таймер4

/*инициализация пина12 порта D*/
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт D
GPIOD->MODER |= GPIO_MODER_MODER12_1; //сконфигурируем пин 12 на альтернативный режим ввода/вывода
GPIOD->OTYPER &= ~GPIO_OTYPER_ODR_12;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12;  //скорость порта самая низкая
GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2;

while(1)
	{
	float voltage = 0;
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
	duty = ADC1->DR;
	TIM4->CCR1 = duty;
	memset(buffer, '\0', sizeof(buffer));
	memset(msg, '\0', sizeof(msg));
	delay_ms (1000); // задержка
	}
}
