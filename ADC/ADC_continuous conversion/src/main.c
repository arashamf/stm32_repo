
 // Работа с АЦП.  Непрерывное преобразование.
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "string.h" // библиотека C работы со строками
#include "DELAY_TIM7.h" // функции задержки
#include "UART_STM32.h" // объявления функций для работы с USART3

typedef unsigned char byte;

// определения частот шин МК
#define BANDWITH 57600 //пропускная способность шины
#define APB1_FREQUENCY 16000000 // частота шины APB1

char msg[100]; // массив для формирования сообщений для вывода по UART
char buffer [50]; //массив для сохранения данных полученных по UART

__I float Vref = 3.3; // опорное напряжение для АЦП; __I = volatile const
__IO float ADC_float = 0; // результат АЦП в виде напряжения; __IO = volatile
__IO byte ADC_count = 0; //счётчик количества преобразований

#define GPIO_AFRH_PIN13_AF2 0x200000 //альт. режим AF2 пина 13
#define COMMAND_ADC "start" //команда запуска АЦП
__IO byte receive_flag = 0; //флаг получения данных по UART
__IO byte ykaz_buffer = 0; //указатель на буффер UART

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
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A)) //проверка окончания полученных данных
	{
	receive_flag = 1; // 1- флаг принятия данных
	UART3_PutRAMString("get!\r");
	}
}

int main()
{

// настройка порта С для АЦП
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOC
SET_BIT(GPIOC->MODER, GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0); // настраиваем вывод PC1 на аналоговый режим

//настройка UART
UART3_init_IRQ(APB1_FREQUENCY, BANDWITH); // инициализация UART3
UART3_PutRAMString("ADC continuous conversion:\r\n");

// настройка АЦП
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // включаем тактовый генератор
/*разряды SQx[4:0] задают номер канала, где х – это номер преобразования.*/
CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L); //зададим количество каналов регулярной группы: 1
SET_BIT(ADC1->SQR3, ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3); // канал 11 (PC1)
SET_BIT(ADC1->CR2, ADC_CR2_ADON); // включаем АЦП

__enable_irq(); // разрешим прерывания глобально

while(1)
	{
		if (receive_flag)
			{
		if (!strncmp (buffer,  COMMAND_ADC, 5)) //проверка корректности команды
			{
			SET_BIT(ADC1->CR2, ADC_CR2_CONT); // включим непрерывное преобразование
			ADC1->CR2 |= ADC_CR2_SWSTART; // программный запуск АЦП
			UART3_PutRAMString ("ADC conversion enabled\n\r");
			for (ADC_count = 0; ADC_count <=10; ADC_count++)
				{
				ADC_float = Vref*ADC1->DR/4096; // опорное напряжение/максимальное число дискретов * результат преобразования
				sprintf(msg,("ADC_count = %d, ADC_result 12 = %f V\r\n") ,ADC_count, ADC_float); // выводим значение АЦП в строку
				UART3_PutRAMString (msg); // выводим строку по UART
				TIM4->CCR2 = ADC1->DR/4.096; // новая длительность ШИМ
				delay_ms(500); // задержка
				}
			CLEAR_BIT(ADC1->CR2, ADC_CR2_CONT); // выключим непрерывное преобразование
			UART3_PutRAMString("ADC conversion disabled\r\n");
			}
		else
			{
			UART3_PutRAMString("ERROR\r\n"); //некорректная команда
			UART3_PutRAMString("write 'start'\r\n");
			}
		memset(buffer, '\0', sizeof(buffer)); // очистка массива
		receive_flag = 0;  //обнуляем флаг
		ykaz_buffer = 0; //обнуляем указатель
		}
	}
}
