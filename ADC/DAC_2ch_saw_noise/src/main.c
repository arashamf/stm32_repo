//ЦАП. ПИЛА

#define stm32f4xx
#include "stm32f4xx.h" // описание периферии


int main()
{
uint16_t cnt = 0; // счетчик для вывода сигнала ЦАП

// Настройка порта ввода-вывода для DAC1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);// запускаем тактовый генератор GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4); //пин PA4 в аналоговый режим для DAC_OUT1, чтобы избежать паразитного потребления
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5); //пин PA5 в аналоговый режим выход для DAC_OUT2, чтобы избежать паразитного потребления

// Настройка DAC
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_DACEN); // запускаем тактовый генератор DAC
SET_BIT (DAC->CR, DAC_CR_TEN1); //триггер ЦАП1 включён, данные из регистра DAC->DHRх, передаются через 3 такта APB1 в регистр DAC_DOR1 (при прграммном запуске через один такт)
SET_BIT (DAC->CR, DAC_CR_TEN2); //триггер ЦАП1 включён, данные из регистра DAC->DHRх, передаются через 3 такта APB1 в регистр DAC_DOR2 (при прграммном запуске через один такт)
SET_BIT (DAC->CR, (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0)); // 111 - выберем вместо триггера программный запуск ЦАП1
SET_BIT (DAC->CR, (DAC_CR_TSEL2_2 | DAC_CR_TSEL2_1 | DAC_CR_TSEL2_0)); //111 - выберем вместо триггера программный запуск ЦАП2
SET_BIT (DAC->CR, DAC_CR_WAVE1_1); //генерация пилы в ЦАП1
SET_BIT (DAC->CR, DAC_CR_WAVE2_1); //генерация пилы в ЦАП2
SET_BIT (DAC->CR, (DAC_CR_MAMP1_3 | DAC_CR_MAMP1_1)); //максимальная амплитуда пилы на ЦАП1
SET_BIT (DAC->CR, (DAC_CR_MAMP2_3 | DAC_CR_MAMP2_0)); //максимальная амплитуда пилы на ЦАП2
SET_BIT (DAC->CR,  DAC_CR_EN1); // включение ЦАП1
SET_BIT (DAC->CR,  DAC_CR_EN2); // включение ЦАП2

// основной цикл программы
while(1)
	{
	SET_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2)); // разрешим программный запуск ЦАП1 и ЦАП2
	while (READ_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2))) {}; // ожидаем окончания преобразования ЦАП1 и ЦАП2
	//while (READ_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG2)) {}; // ожидаем окончания преобразования
	}
}
