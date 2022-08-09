#define stm32f4xx
#include "stm32f4xx.h" // описание периферии

// переменные для обработки дребезга контактов кнопки
#define KEY_Bounce_Time 400 // время дребезга + автоповтора в мс
static volatile unsigned int KEY_Bounce_Time_Cnt; // счетчик времени дребезга
static volatile unsigned char KEY_Press_Flag = 0; // флаг нажатия на кнопку: 0 - не нажата; 1 - нажата

static volatile unsigned char flag = 0; //номер светодиода, который необходимо зажечь

// обработка прерываний от TIM4
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
if(KEY_Bounce_Time_Cnt)
	KEY_Bounce_Time_Cnt--; // уменьшаем счетчик
else //если можно
	{
	EXTI->PR |= EXTI_PR_PR0; // сброс флага прерывания
	EXTI->IMR |= EXTI_IMR_MR0; // разрешим прерывание от EXTI0
	}
}

// обработка прерывания EXTI0
void EXTI0_IRQHandler(void)
{
KEY_Bounce_Time_Cnt = KEY_Bounce_Time; // счетчик времени дребезга - запустим
KEY_Press_Flag = 1; // флаг нажатия на кнопку - установим
EXTI->PR |= EXTI_PR_PR0; // сброс флага прерывания
EXTI->IMR &= ~EXTI_IMR_MR0; // запретим прерывание от EXTI0
}

int main()
{
// порт для кнопки
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // запускаем тактовый генератор GPIOA
GPIOA->MODER &= ~GPIO_MODER_MODER0; // настраиваем вывод 0 на вход, при сбросе порт настроен на вход
GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0; // внутренния подтяжка вверх

// настроим вход на "внешнее прерывание"
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // включаем тактирование
//SYSCFG_EXTICR1 = 0; // при сбросе EXTI0 подключен к PA0
EXTI->IMR |= EXTI_IMR_MR0; // разрешим прерывание от EXTI0
EXTI->RTSR |= EXTI_RTSR_TR0; // срабатыввание по нарастающему фронту (rise)
NVIC_EnableIRQ(EXTI0_IRQn); // настройка NVIC для EXTI
// выход на светодиод
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; // настраиваем выводы на выход
//GPIOD->OTYPER - // при сбросе стоит Push-Pull

// настройка таймера TIM4
// на 1 мс: 16000000/16/1000 = 1000Гц (1мс)
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // запускаем тактовый генератор TIM4
TIM4->PSC = 16-1; //предделитель TIM4
TIM4->ARR = 1000; //значение перезагрузки TIM4
TIM4->DIER |= TIM_DIER_UIE; // разрешим прерывание при перезагрузке таймера
TIM4->CR1 |= TIM_CR1_CEN; // разрешим счет таймера
NVIC_EnableIRQ(TIM4_IRQn); // настройка NVIC для TIM4
__enable_irq(); // разрешим прерывания глобально

while(1)
	{
	// проверяем флаг нажатия кнопки
	if(KEY_Press_Flag)
		{
		switch (flag)
			{
			case 0: GPIOD->BSRRH |= GPIO_BSRR_BS_15;  //выключаем вывод 15
					GPIOD->BSRRL |= GPIO_BSRR_BS_12; //включаем вывод 12
					flag++;
					break;
			case 1: GPIOD->BSRRH |= GPIO_BSRR_BS_12;  //выключаем вывод 12
					GPIOD->BSRRL |= GPIO_BSRR_BS_13; //включаем вывод 13
					flag++;
					break;
			case 2: GPIOD->BSRRH |= GPIO_BSRR_BS_13;  //выключаем вывод 13
					GPIOD->BSRRL |= GPIO_BSRR_BS_14; //включаем вывод 14
					flag++;
					break;
			case 3: GPIOD->BSRRH |= GPIO_BSRR_BS_14;  //выключаем вывод 14
					GPIOD->BSRRL |= GPIO_BSRR_BS_15; //включаем вывод 15
					flag = 0;  //обнуляем флаг, при следующем нажатии кнопки засветится 1 светодиод
					break;
			default: break;
			}
		KEY_Press_Flag = 0; // флаг нажатия на кнопку - сбросим
		}
	}
}
