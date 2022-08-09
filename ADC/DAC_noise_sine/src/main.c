//ЦАП. синусоида и шум

#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "math.h"
typedef unsigned char byte;


int main()
{
//byte cnt = 0; // счетчик для вывода сигнала ЦАП

//----------------массив для формирования синусоиды------------------//
uint16_t sinus[72]={2225,2402,2577,2747,2912,3070,3221,3363,3494,3615,
					3724,3820,3902,3971,4024,4063,4085,4095,4085,4063,
					4024,3971,3902,3820,3724,3615,3495,3363,3221,3071,
					2912,2747,2577,2403,2226,2047,1869,1692,1517,1347,
					1182,1024, 873, 731, 600, 479, 370, 274, 192, 124,
					70,	 31,	10,	0,	 10,  31,  70,  123, 192, 274,
					370, 479, 599, 731, 873,1023,1182,1347,1517, 1691,
					1868,2047
					};

// Настройка порта ввода-вывода для DAC1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);// запускаем тактовый генератор GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4); //пин PA4 в аналоговый режим для DAC_OUT1, чтобы избежать паразитного потребления
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5); //пин PA5 в аналоговый режим выход для DAC_OUT2, чтобы избежать паразитного потребления

// Настройка DAC
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_DACEN); // запускаем тактовый генератор DAC
SET_BIT (DAC->CR, DAC_CR_TEN1); //триггер ЦАП1 включён, данные из регистра DAC->DHRх, передаются через 3 такта APB1 в регистр DAC_DOR1 (при программном запуске через один такт)
SET_BIT (DAC->CR, DAC_CR_TEN2); //триггер ЦАП2 включён
SET_BIT (DAC->CR, (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0)); // 111 - выберем вместо триггера программный запуск ЦАП1
//SET_BIT (DAC->CR, (DAC_CR_TSEL1_2) ); // триггер запуска ЦАП1 - "Timer 2 TRGO event"
//SET_BIT (DAC->CR, DAC_CR_WAVE1_1); //генерация пилы в ЦАП1
//SET_BIT (DAC->CR, DAC_CR_WAVE1_0); //генерация псевдослучайного шума в ЦАП1
SET_BIT (DAC->CR, (DAC_CR_TSEL2_2 | DAC_CR_TSEL2_1 | DAC_CR_TSEL2_0)); // 111 - выберем вместо триггера программный запуск ЦАП2
SET_BIT (DAC->CR, DAC_CR_WAVE2_0); //генерация псевдослучайного шума в ЦАП2
SET_BIT (DAC->CR, (DAC_CR_MAMP2_3 | DAC_CR_MAMP2_1)); //максимальная амплитуда на ЦАП2
SET_BIT (DAC->CR,  DAC_CR_EN1); // включение ЦАП1
SET_BIT (DAC->CR,  DAC_CR_EN2); // включение ЦАП2
// настройка таймера TIM2
/*SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // включаем тактовый генератор TIM2
WRITE_REG(TIM2->PSC, 800-1); // предделитель
WRITE_REG(TIM2->ARR, 200-1); // значение перезагрузки
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // разрешим вывод сигнала со схемы сравнения на вывод МК
SET_BIT (TIM2->CR2, TIM_CR2_MMS_1); // в качестве TRGO будет использоваться событие "Update"
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // разрешим работу таймера*/

// основной цикл программы
while(1)
	{
	for (byte i = 0; i < 72; i++)
		{
		WRITE_REG (DAC->DHR12R1, sinus [i]);
		WRITE_REG (DAC->DHR12R2, sinus [i]);
		SET_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2)); // разрешим программный запуск ЦАП1 и ЦАП2
		while (READ_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2))) {}; // ожидаем окончания преобразования ЦАП1 и ЦАП2
		}
	}
}

