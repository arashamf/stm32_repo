//программа управление яркостью светодиода с помощью двух кнопок c изменением параметров в широких пределах
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии

// переменные для обработки дребезга контактов кнопки
#define KEY_BOUNCE_TIME 200 // время дребезга в мс
#define KEY_REPEAT_TIME 500 // время автоповтора в мс
static volatile unsigned int key_bounce_time_cnt; // счетчик времени дребезга
static volatile unsigned int key_repeat_time_cnt; // счетчик времени автоповтора
static volatile unsigned char key_press_flag_short = 0; // флаг короткого нажатия на кнопку: 0 - не нажата; 1 - нажата
static volatile unsigned char key_press_flag_middle = 0; // флаг среднего нажатия на кнопку
static volatile unsigned char key_press_flag_long = 0; // флаг долгого нажатия на кнопку

#define GPIO_AFRH_PIN12_AF2 0x00020000  //битовая маска настройки пина 12 на альтернативную функцию
#define PERIOD 1000 //период импульсов ШИМ
short duty_ch1 = PERIOD/2; // длительность импульсов ШИМ
static volatile unsigned char regul_flag = 0; //флаг уменьшения или увеличения яркости, 1 - увеличить яркость, 0 - уменьшить

short regul_led_brightness (unsigned char, short, short); //прототип функции

// обработка прерывания EXTI0 от кнопки
void EXTI0_IRQHandler(void)
{
key_bounce_time_cnt = KEY_BOUNCE_TIME; // счетчик времени дребезга - запустим
key_press_flag_short = 1; // флаг нажатия на кнопку - установим
regul_flag = 1; //флаг увеличения яркости
EXTI->PR |= EXTI_PR_PR0; // сброс флага прерывания
EXTI->IMR &= ~EXTI_IMR_MR0 & ~EXTI_IMR_MR1; // запретим прерывание от EXTI0 и EXTI1
}

// обработка прерывания EXTI1 от кнопки
void EXTI1_IRQHandler(void)
{
key_bounce_time_cnt = KEY_BOUNCE_TIME; // счетчик времени дребезга - запустим
key_press_flag_short = 1; // флаг нажатия на кнопку - установим
regul_flag = 0; //флаг уменьшения яркости
EXTI->PR |= EXTI_PR_PR1; // сброс флага прерывания
EXTI->IMR &= ~EXTI_IMR_MR0 & ~EXTI_IMR_MR1; // запретим прерывание от EXTI0 и EXTI1
}

//ф-я обработки прерывания от таймера3, задержка после нажатия кнопки
void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
if(key_bounce_time_cnt)
	key_bounce_time_cnt--; // уменьшаем счетчик
else //если можно
	{
	if (key_press_flag_short)
		{
		GPIOD->BSRRH |= GPIO_BSRR_BS_14; //выключаем 14 пин
		GPIOD->BSRRH |= GPIO_BSRR_BS_13; //выключаем 13 пин
		duty_ch1 = regul_led_brightness (regul_flag, 100, duty_ch1);
		if ((!(GPIOA->IDR & GPIO_IDR_IDR_0)) || (!(GPIOA->IDR & GPIO_IDR_IDR_1)))  //если одна из кнопок попрежнеиу нажата
			{
			key_press_flag_middle = 1; //установим флаг среднего нажатия
			key_repeat_time_cnt = KEY_REPEAT_TIME; //установим время задержки для среднего нажатия
			GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //включаем 13 пин
//			GPIOD->BSRRH |= GPIO_BSRR_BS_14; //выключаем 14 пин
			}
		else
			{
			EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // разрешим прерывание от EXTI0 и EXTI1
			}
		key_press_flag_short = 0;  //обнулим флаг короткого нажатия
		TIM4->CCR1 = duty_ch1;
		}
	if (key_press_flag_middle)
		{
		if (key_repeat_time_cnt)
			{
			key_repeat_time_cnt--;
			}
		else
			{
			duty_ch1 = regul_led_brightness (regul_flag, 50, duty_ch1);
			key_press_flag_middle = 0;
			TIM4->CCR1 = duty_ch1;
			if ((!(GPIOA->IDR & GPIO_IDR_IDR_0)) || (!(GPIOA->IDR & GPIO_IDR_IDR_1)))  //если одна из кнопок попрежнеиу нажата
				{
				key_press_flag_long = 1; //установим флаг длинного нажатия
				key_repeat_time_cnt = KEY_REPEAT_TIME; //установим время задержки для среднего нажатия
				GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //включаем 14 пин
				GPIOD->BSRRH |= GPIO_BSRR_BS_13; //выключаем 13 пин
				}
			else
				{
				EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // разрешим прерывание от EXTI0 и EXTI1
				}
			}
		}
	if (key_press_flag_long)
		{
		if (key_repeat_time_cnt)
			{
			key_repeat_time_cnt--;
			}
		else
			{
			duty_ch1 = regul_led_brightness (regul_flag, 25, duty_ch1);
			key_press_flag_long = 0;
			TIM4->CCR1 = duty_ch1;
			if ((!(GPIOA->IDR & GPIO_IDR_IDR_0)) || (!(GPIOA->IDR & GPIO_IDR_IDR_1)))  //если одна из кнопок попрежнеиу нажата
				{
				key_press_flag_long = 1; //установим флаг длинного нажатия
				key_repeat_time_cnt = KEY_REPEAT_TIME; //установим время задержки для среднего нажатия
				}
			else
				{
				EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // разрешим прерывание от EXTI0 и EXTI1
				}
			}
		}
	}
}


int main()
{
// порт для кнопок управления яркостью светодиода
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // запускаем тактовый генератор GPIOA
GPIOA->MODER &= ~GPIO_MODER_MODER0 & ~GPIO_MODER_MODER1; // настраиваем вывод 0 и 1 на вход
GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0; // внутренния подтяжка вверх

// настроим вход на "внешнее прерывание"
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // включаем тактирование
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0 & ~SYSCFG_EXTICR1_EXTI1; // назначим прерывания EXTI0 и EXTI1 для порта PA0 и PA1
EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // разрешим прерывание от EXTI0 и EXTI1
//EXTI->RTSR |= EXTI_RTSR_TR0 | EXTI_RTSR_TR1; // срабатыввание по нарастающему фронту (rise) от EXTI0 и EXTI1
EXTI->FTSR |= EXTI_FTSR_TR0 | EXTI_FTSR_TR1; // срабатыввание прерывания по спадающему фронту (fall) от EXTI0 и EXTI1
NVIC_EnableIRQ(EXTI0_IRQn); // настройка NVIC для EXTI
NVIC_EnableIRQ(EXTI1_IRQn); // настройка NVIC для EXTI

// выход на светодиод
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0;
GPIOD->MODER |= GPIO_MODER_MODER12_1; // настраиваем вывод на альт. выход
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая
//GPIOD->OTYPER - // при сбросе стоит Push-Pull
GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2; //конфигурируем пин12 на CH1 TIM4 (alt.mode)

// настройка таймера 3
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // запускаем тактовый генератор TIM3
TIM3->PSC = 16-1; //предделитель TIM3
TIM3->ARR = 1000; //значение перезагрузки TIM3, прерывание 16000000/16/1000=1000=1мс
TIM3->DIER |= TIM_DIER_UIE; // разрешим прерывание при перезагрузке TIM3
TIM3->CR1 |= TIM_CR1_CEN; // разрешим счет таймера 3
NVIC_EnableIRQ(TIM3_IRQn); // настройка NVIC для TIM3

/*инициализация таймера 4 для ШИМ*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //запускаем тактовый генератор TIM4
TIM4->CCER |= TIM_CCER_CC1E; //TIM4_CCER регистр включения режима захвата/сравнения таймера4; бит 0 запускает выходной режим сравнения канала 1 TIM4 на пин 12
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /*TIM4_CCMR1 - регистр1 настройки режима захвата/сравнения TIM4;  110задаёт каналу 1 TIM4 режим ШИМ1 - прямой ШИМ*/
//TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0; //инверсный ШИМ
// Для получения времени 1 мс (1 кГц часота ШИМ) выберем следующие параметры: (1/16000000Гц * 16 * 1000 = 0,001с)
TIM4->CR1 |= TIM_CR1_CMS_1 | TIM_CR1_CMS_0; //выравнивания ШИМ-сигнала по центру, событие совпадение появляется два раза, во время счета вверх и вниз, частота ШИМ уменьшена в 2 раза
TIM4->PSC = 16-1; // предделитель TIM4
TIM4->ARR = PERIOD; //значение перезагрузки TIM4, период импульса ШИМ
TIM4->CCR1 = duty_ch1;  //здесь скважность ШИМ
TIM4->CR1 |= TIM_CR1_CEN; //включим таймер4

__enable_irq(); // разрешим прерывания глобально

while(1)
	{

	}
}

/*функция регулировки яркости светодиода*/
short regul_led_brightness (unsigned char button_flag, short pulse_divider, short pulse_duration)
{
if (button_flag) //если прерывание пришло от первой кнопки
	{
	if (pulse_duration >= PERIOD) //если длительность импульса превышает период ШИМ
		pulse_duration = 0; //гасим светодиод
	else
		pulse_duration += PERIOD/pulse_divider; //если нет, то увеличиваем яркость на шаг
	}
else //если прерывание пришло от второй кнопки
	{
	if (pulse_duration <= 0) //если длительность импульса меньше или равно 0
		pulse_duration = PERIOD; //то, светодиод светится в полную силу
	else
		pulse_duration -= PERIOD/pulse_divider; //если нет, то уменьшаем яркость на шаг
	}
return pulse_duration;
}
