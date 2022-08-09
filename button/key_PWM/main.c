#define stm32f4xx
#include "stm32f4xx.h" // описание периферии

// переменные для обработки дребезга контактов кнопки
#define KEY_Bounce_Time 400 // время дребезга + автоповтора в мс
static volatile unsigned int KEY_Bounce_Time_Cnt; // счетчик времени дребезга
static volatile unsigned char KEY_Press_Flag = 0; // флаг нажатия на кнопку: 0 - не нажата; 1 - нажата

#define PERIOD 100 //период импульсов ШИМ
#define GPIO_AFRH_PIN12_AF2 0x00020000
unsigned int duty_ch1 = 0; // длительность импульсов ШИМ
static volatile unsigned char flag = 0; //флаг нарастания или убывания скважности ШИМ

// обработка прерывания EXTI0 от кнопки
void EXTI0_IRQHandler(void)
{

KEY_Bounce_Time_Cnt = KEY_Bounce_Time; // счетчик времени дребезга - запустим
KEY_Press_Flag = 1; // флаг нажатия на кнопку - установим
duty_ch1 = PERIOD/100;
TIM4->CCR1 = duty_ch1;  //сбросим скважность ШИМ = 100
EXTI->PR |= EXTI_PR_PR0; // сброс флага прерывания
EXTI->IMR &= ~EXTI_IMR_MR0; // запретим прерывание от EXTI0
}

//ф-я обработки прерывания от таймера2, задержка после нажатия кнопки
void TIM2_IRQHandler(void)
{
TIM2->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
if(KEY_Bounce_Time_Cnt)
	KEY_Bounce_Time_Cnt--; // уменьшаем счетчик
else //если можно
	{
//	EXTI->PR |= EXTI_PR_PR0; // сброс флага прерывания
	EXTI->IMR |= EXTI_IMR_MR0; // разрешим прерывание от EXTI0
	}
}

//ф-я обработки прерывания от таймера3, изменение скважности ШИМ
void TIM3_IRQHandler(void)
	{
	TIM3->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
	if (!flag)
		duty_ch1 += PERIOD/100; // прибавляем длительность по 1 % от периода
	else
		duty_ch1 -= PERIOD/100;
	if((duty_ch1 == (PERIOD - 1)) || (duty_ch1 == 1))
		flag ^= 1; //инвертируем значение флага
	TIM4->CCR1 = duty_ch1; // обновляем длительность ШИМ1
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
GPIOD->MODER |= GPIO_MODER_MODER12_1; // настраиваем вывод на альт. выход
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая
//GPIOD->OTYPER - // при сбросе стоит Push-Pull
GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2; //конфигурируем пин12 на CH1 TIM4 (alt.mode)
GPIOD->BSRRH |= GPIO_BSRR_BS_12;

// настройка таймера 2
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // запускаем тактовый генератор TIM2
TIM2->PSC = 16-1;
TIM2->ARR = 1000;
TIM2->DIER |= TIM_DIER_UIE; // разрешим прерывание при перезагрузке таймера2
TIM2->CR1 |= TIM_CR1_CEN; // разрешим счет таймера2
NVIC_EnableIRQ(TIM2_IRQn); // настройка NVIC для TIM2

// настройка таймера 3
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // запускаем тактовый генератор TIM3
TIM3->PSC = 8000-1; //предделитель TIM3
TIM3->ARR = PERIOD; //значение перезагрузки TIM3, прерывание 16000000/8000/100=100=50мс
TIM3->DIER |= TIM_DIER_UIE; // разрешим прерывание при перезагрузке TIM3
//TIM3->CR1 |= TIM_CR1_CEN; // разрешим счет таймера 3
NVIC_EnableIRQ(TIM3_IRQn); // настройка NVIC для TIM3

/*инициализация таймера 4*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //запускаем тактовый генератор TIM4
TIM4->CCER |= TIM_CCER_CC1E; //TIM4_CCER регистр включения режима захвата/сравнения таймера4; бит 0 запускает выходной режим сравнения канала 1 TIM4 на пин 12
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /*TIM4_CCMR1 - регистр1 настройки режима захвата/сравнения TIM4;  110задаёт каналу 1 TIM4 режим ШИМ1 - прямой ШИМ*/
//TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0; //инверсный ШИМ
// Для получения времени 0,2 мсек (5 кГц часота ШИМ) выберем следующие параметры: (1/16000000Гц * 16 * 100 = 0,0001с)
TIM4->CR1 |= TIM_CR1_CMS_1 | TIM_CR1_CMS_0; //выравнивания ШИМ-сигнала по центру, событие совпадение появляется два раза, во время счета вверх и вниз, частота ШИМ уменьшена в 2 раза
//TIM4->CR1 |= TIM_CR1_CMS_1;
TIM4->PSC = 32-1; // предделитель TIM4
TIM4->ARR = PERIOD; //значение перезагрузки TIM4, период импульса ШИМ
//TIM4->CCR1 = duty_ch1; // здесь скважность ШИМ
//TIM4->CCR1 = PERIOD/2;  //здесь скважность ШИМ
//TIM4->CR1 |= TIM_CR1_CEN; //включим таймер4

__enable_irq(); // разрешим прерывания глобально

while(1)
	{
	// проверяем флаг нажатия кнопки
	if(KEY_Press_Flag)
		{
		TIM4->CR1 ^= TIM_CR1_CEN;
		TIM3->CR1 ^= TIM_CR1_CEN;
		KEY_Press_Flag = 0; // флаг нажатия на кнопку - сбросим
		}
	}
}
