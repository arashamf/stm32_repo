//ЦАП. ПИЛА

#define stm32f4xx
#include "stm32f4xx.h" // описание периферии


int main()
{
uint16_t cnt; // счетчик для вывода сигнала ЦАП

// Настройка порта ввода-вывода для DAC1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);// запускаем тактовый генератор GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4); //пин PA4 в аналоговый режим для DAC_OUT1, чтобы избежать паразитного потребления
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5); //пин PA5 в аналоговый режим выход для DAC_OUT2, чтобы избежать паразитного потребления

// Настройка DAC
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_DACEN); // запускаем тактовый генератор DAC
SET_BIT (DAC->CR, (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0)); // 111 - программный запуск ЦАП1
SET_BIT (DAC->CR, (DAC_CR_TSEL2_2 | DAC_CR_TSEL2_1 | DAC_CR_TSEL2_0)); //111 - программный запуск ЦАП2
//SET_BIT (DAC->CR, DAC_CR_WAVE1_1);
SET_BIT (DAC->CR,  DAC_CR_EN1); // включение 1 канала ЦАП
SET_BIT (DAC->CR,  DAC_CR_EN2); // включение 2 канала ЦАП

// основной цикл программы
while(1)
	{
	for(cnt = 0; cnt < 4096; cnt++)
		{
		WRITE_REG(DAC->DHR12R1, cnt); // запишем новое значение в регистр с правым выравниванием DAC->DHR12R1 (0 бит в 0, 11 в 11)
		SET_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG1); // разрешим программный запуск ЦАП1
		while(READ_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG1)) // ожидаем окончания преобразования ЦАП1
			{}; //бит очищается SWTRIG1 аппаратным обеспечением (один такт), как только значение регистра DAC_DHR1ххх было загружено в регистр DAC_DOR1
		}
	}
}
