/*АЦП.Регулярные каналы.Запуск по триггеру от внешнего прерывания*/
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций

// определения частот шин МК
#define BANDWITH 57600 //пропускная способность шины
#define APB1_FREQUENCY 16000000 // частота шины APB1

#include "DELAY_TIM7.h" // функции задержки
#include "UART_STM32.h" // объявления функций для работы с USART3
char msg[130]; // массив для формирования сообщений для вывода по UART
typedef unsigned char byte;

__I float Vref = 3.3; // опорное напряжение для АЦП; __I = volatile const
__IO float ADC_float = 0; // результат АЦП в виде напряжения; __IO = volatile

#define GPIO_AFRH_PIN13_AF2 0x200000 // коэффициент для альт. функции AF2 для PD13
//#define GPIO_AFRL_PIN6_AF2 0x200000//

#define PERIOD 10000  //значение периода ШИМ

// переменные для обработки дребезга контактов кнопки
#define KEY_BOUNCE_TIME 500// время дребезга + автоповтора в мс
__IO byte flag_ADC = 0;
static volatile unsigned int KEY_Bounce_Time_Cnt;
static volatile byte KEY_Press_Flag = 0; // флаг нажатия на кнопку: 0 - не нажата; 1 - нажата

// обработка прерывания EXTI0
void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR11)
	{
		UART3_PutRAMString("hello\r\n"); // выводим строку по UART
		KEY_Bounce_Time_Cnt = KEY_BOUNCE_TIME; // счетчик времени дребезга - запустим
		KEY_Press_Flag = 1; // флаг нажатия на кнопку - установим
		EXTI->PR |= EXTI_PR_PR11; // сброс флага прерывания
		EXTI->IMR &= ~EXTI_IMR_MR11; // запретим прерывание от EXTI0
		flag_ADC = 0;
	}
}

// обработка прерываний от TIM3
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
if(KEY_Bounce_Time_Cnt)
	{
	KEY_Bounce_Time_Cnt--; // уменьшаем счетчик
	}
else //если можно
	{
//	EXTI->PR |= EXTI_PR_PR11; // сброс флага прерывания
	if (!READ_BIT(EXTI->IMR, EXTI_IMR_MR11))
		{
		flag_ADC = 1;
		EXTI->IMR |= EXTI_IMR_MR11; // разрешим прерывание от EXTI0
		}
	}
}

// обработка прерываний от ADC
void ADC_IRQHandler(void)
{

//if(ADC1->SR & ADC_SR_EOC) // если сработало прерывание от ADC1 EOC
if (flag_ADC)
	{
	ADC_float = Vref*ADC1->DR/4096; // опорное напряжение/максимальное число дискретов * результат преобразования
	sprintf(msg,("ADC_result = %f V\r\n"), ADC_float); // выводим значение АЦП в строку
	UART3_PutRAMString(msg); // выводим строку по UART
//	WRITE_REG (TIM4->CCR2, PERIOD/2); //длительность ШИМ
	ADC1->SR &= ~ADC_SR_EOC; // сбросим флаг перывания
	}
}

int main()
{
//настройка вывода 13 для световой сигнализации
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // запускаем тактовый генератор GPIOD
SET_BIT(GPIOD->MODER, GPIO_MODER_MODER13_1); //пин 13 на альтернативный выход
GPIOD->AFR[1] |= GPIO_AFRH_PIN13_AF2; //выбор альт. режима для пина 13 (AF2, TIM4_CH2)

// настройка порта для кнопки
//SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // запускаем тактовый генератор GPIOD
//CLEAR_BIT(GPIOD->MODER, GPIO_MODER_MODER11); // настраиваем вывод 11 на вход, при сбросе порт настроен на вход
//SET_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPDR11_0); // внутренния подтяжка вверх
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOD
CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER11); // настраиваем вывод 11 на вход, при сбросе порт настроен на вход
SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR11_0); // внутренния подтяжка вверх

// настроим вход на "внешнее прерывание"
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); // включаем тактирование
//SET_BIT(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI11_PD); // подключим EXTI11 к PD11
SET_BIT(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI11_PC); // подключим EXTI11 к PC11
//SET_BIT(EXTI->RTSR, EXTI_RTSR_TR11); // срабатыввание по нарастающему фронту (rise)
SET_BIT(EXTI->FTSR, EXTI_FTSR_TR11); //срабатывание по спадающему фронту
SET_BIT(EXTI->IMR, EXTI_IMR_MR11); // разрешим прерывание от EXTI11

// настройка порта С для АЦП
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOC
SET_BIT(GPIOC->MODER, GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0); // настраиваем вывод PC1 на аналоговый режим

// настройка АЦП
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // включаем тактовый генератор
//разряды SQx[4:0] задают номер канала, где х – это номер преобразования
CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L); //зададим количество каналов регулярной группы: 1
SET_BIT(ADC1->SQR3, ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3); // канал 11 (PC1)
//SET_BIT(ADC1->CR2, ADC_CR2_EXTEN_0); // триггер срабатывает по нарастающему фронту
SET_BIT(ADC1->CR2, ADC_CR2_EXTEN_1); // триггер срабатывает по спадающему фронту
//SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2); // в качестве триггера выбран сигнал "Timer 2 TRGO event"
//SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0); // в качестве триггера выбран сигнал " Timer 3 CC1 event"
//SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_3); // в качестве триггера выбран сигнал "Timer 3 TRGO event"
SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0); // в качестве триггера выбран сигнал от внеш. прерывания
SET_BIT(ADC1->CR1, ADC_CR1_EOCIE); // разрешим прерывания ADC по окончанию преобразования
SET_BIT(ADC1->CR2, ADC_CR2_ADON); // включаем АЦП

//настройка TIM4 для ШИМ
/*SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN); //включим тактовый генератор таймера4
WRITE_REG (TIM4->PSC, 16-1); // предделитель TIM4
WRITE_REG (TIM4->ARR, PERIOD); // значение перезагрузки TIM4
SET_BIT(TIM4->CCMR1, TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2); //запуск режима 1 ШИМ канала 2
SET_BIT(TIM4->CCER, TIM_CCER_CC2E); //активируем канал 2 TIM4
WRITE_REG (TIM4->CCR2, PERIOD/2); //длительность импульса ШИМ
SET_BIT(TIM4->CR1, TIM_CR1_CEN); //включим таймер4*/

// настройка таймера TIM4
SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN); // включаем тактовый генератор TIM3
WRITE_REG (TIM4->PSC, 16-1); //предделитель TIM3
WRITE_REG (TIM4->ARR, 1000);  //значение перезагрузки TIM3
SET_BIT(TIM4->DIER, TIM_DIER_UIE); // разрешим прерывание при перезагрузке TIM3
SET_BIT(TIM4->CR1, TIM_CR1_CEN); // разрешим счет TIM3

//настройка UART
UART3_init(APB1_FREQUENCY, BANDWITH); // инициализация UART3
UART3_PutRAMString("ADC_External_Trigger_EXTI:\r\n");

NVIC_EnableIRQ(EXTI15_10_IRQn); // настройка NVIC для EXTI11
NVIC_EnableIRQ(TIM4_IRQn); // настройка NVIC для TIM4
NVIC_EnableIRQ(ADC_IRQn); // разрешим прерывания ADC в контроллере NVIC

__enable_irq(); // разрешим прерывания глобально

while(1)
	{
	}
}
