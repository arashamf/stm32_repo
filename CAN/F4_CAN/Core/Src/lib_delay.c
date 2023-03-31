#include "lib_delay.h"


void GetClocksFreq (LL_RCC_ClocksTypeDef *SysClock)
{
	
	LL_RCC_GetSystemClocksFreq(SysClock);
}

void delay_ms(uint16_t delay)
{

	SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM13EN ); //тактирование таймера 3
	CLEAR_REG(TIM13->CNT);
	WRITE_REG (TIM13->PSC, (uint16_t)((PCLK1_Clock/1000) - 1)); //GetPCLK1Freq - частота шины таймера 10 						
	WRITE_REG (TIM13->ARR, (2*delay)-1); //время задержки
	SET_BIT (TIM13->EGR, TIM_EGR_UG); //запись новых настроек в таймер 
	CLEAR_BIT (TIM13->SR, TIM_SR_UIF);// сброс флага обновления
	
	SET_BIT (TIM13->CR1, TIM_CR1_CEN); //включение таймера
	while (!READ_BIT(TIM13->SR, TIM_SR_UIF)) {} //ожидание установки флага обновления таймера 
	CLEAR_BIT (TIM13->CR1, TIM_CR1_CEN);  //отключение таймера
	CLEAR_BIT (TIM13->SR, TIM_SR_UIF);
}

void delay_us(uint16_t delay)
{

	SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM4EN ); //тактирование таймера 13
	CLEAR_REG(TIM13->CNT);
	WRITE_REG (TIM13->PSC, (uint16_t)((PCLK1_Clock/1000000) - 1));  	 
	WRITE_REG (TIM13->ARR, 2*delay-1); //время задержки
	SET_BIT (TIM13->EGR, TIM_EGR_UG); //запись новых настроек в таймер 
	CLEAR_BIT (TIM13->SR, TIM_SR_UIF);// сброс флага обновления
	
	SET_BIT (TIM13->CR1, TIM_CR1_CEN); //включение таймера
	while (!READ_BIT(TIM13->SR, TIM_SR_UIF)) {} //ожидание установки флага обновления таймера 
	CLEAR_BIT (TIM13->CR1, TIM_CR1_CEN);  //отключение таймера
	CLEAR_BIT (TIM13->SR, TIM_SR_UIF);
}


