#include <stm32f4xx.h>
#define GPIO_AFRH_PIN12_AF2 0x00020000
#define GPIO_AFRH_PIN13_AF2 0x00200000
#define PAUSE 32000
#define PERIOD 100 //период импульсов ШИМ

unsigned int duty_ch1 = PERIOD/100; // длительность импульсов ШИМ канала 1 пина12
unsigned int duty_ch2 = PERIOD/50; //длительность импульсов ШИМ канала 2 пина13

void TIM3_IRQHandler(void)
{
	TIM3->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
	duty_ch1 += PERIOD/100; // прибавляем длительность по 1 % от периода
	duty_ch2 += PERIOD/50; // прибавляем длительность по 2 % от периода
	if(duty_ch1 > PERIOD)
		duty_ch1 = PERIOD/100; // ограничиваем диапазон изменения - от 0 до 100%
	if(duty_ch2 > PERIOD)
		duty_ch2 = PERIOD/50; // ограничиваем диапазон изменения - от 0 до 100%
	TIM4->CCR1 = duty_ch1; // обновляем длительность ШИМ1
	TIM4->CCR2 = duty_ch2; // обновляем длительность ШИМ2
}

int main(void)
{

/*инициализация пина12 порта D*/
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт D
GPIOD->MODER |= GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1; //сконфигурируем пин 12 на альтернативный режим ввода/вывода
GPIOD->OTYPER &= ~GPIO_OTYPER_ODR_12 & ~GPIO_OTYPER_ODR_13;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12 &  ~GPIO_OSPEEDER_OSPEEDR13;  //скорость порта самая низкая

/*инициализация таймера 4*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN;; //тактируем таймер4 и таймер 3
// Для получения времени 0,1 мсек (10 кГц часота ШИМ) выберем следующие параметры: (1/16000000Гц * 16 * 100 = 0,0001 сек
TIM4->PSC = 16-1; // предделитель TIM4
TIM4->ARR = PERIOD; //значение перезагрузки TIM4, в данном случае это значение равно периоду импульса ШИМ
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; /*TIM4_CCMR1 - регистр1 настройки режима
захвата/сравнения TIM4; биты 4-6 канала1 и биты 14-12 канала2 конфигурируют выходной режим сравнения, в данном случает в эти биты записано 110,
что задаёт каналам 1 и 2 TIM4 режим ШИМ1; если счетный регистр работает на сложение: при CNT<CCR1 сигнал OC1ref=1, иначе OC1ref=0.*/
TIM4->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; /*TIM4_CCER регистр включения режима захвата/сравнения таймера4; бит 0 и 5 запускает выходной
											режим сравнения канала 1 и 2 TIM4 на пины 12 и 13 порта D*/
TIM4->CCR1 = duty_ch1; // TIM4_CCR1 регистр1 захвата/сравнения TIM4, здесь зададим длительность ШИМ
TIM4->CCR2 = duty_ch2;
TIM4->CR1 |= TIM_CR1_CEN; //включим таймер4

/*инициализация таймера 3*/
TIM3->PSC = 3200-1;
TIM3->ARR = 100;
TIM3->DIER |= TIM_DIER_UIE; //разрешим прерывание при обновлении (переполнении) таймера 3
TIM3->CR1 |= TIM_CR1_CEN; //включение таймер3
NVIC_EnableIRQ(TIM3_IRQn); //разрешаем прерывание в контроллере прерываний. стр.218 PM0214
__enable_irq(); // разрешим прерывания глобально

GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2 | GPIO_AFRH_PIN13_AF2; /*конфигурируем пин12 на CH1 TIM4 (alt.mode), конфигурируем пин13 на CH2 TIM4,
для этого записываем "1" в 17 и 21 бит регистра GPIO_AFRH. Про выбор альтернативного режима стр 62-70 datasheet STM32F407xx*/

    while(1)
    {
 /*  	for (unsigned int cnt_delay = 0; cnt_delay < PAUSE; cnt_delay++) {};

    	duty_ch1 += Period/100; // прибавляем длительность по 1 % от периода
    	duty_ch2 += Period/50; // прибавляем длительность по 2 % от периода
    	if(duty_ch1 > Period)
    		duty_ch1 = 0; // ограничиваем диапазон изменения - от 0 до 100%
    	if(duty_ch2 > Period)
    	    duty_ch2 = 0; // ограничиваем диапазон изменения - от 0 до 100%
    	TIM4->CCR1 = duty_ch1; // обновляем длительность ШИМ1
    	TIM4->CCR2 = duty_ch2; // обновляем длительность ШИМ2*/
    }
}



