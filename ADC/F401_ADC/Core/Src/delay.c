#include "delay.h"

void delay_ms(uint16_t delay)
{

	SET_BIT (RCC->APB2ENR, RCC_APB2ENR_TIM10EN ); //тактирование таймера 6
	CLEAR_REG(TIM10->CNT);
	WRITE_REG (TIM10->PSC, (uint16_t)(HAL_RCC_GetPCLK2Freq()/10000) - 1); //GetPCLK2Freq - частота шины таймера 10 		
	//WRITE_REG (TIM10->PSC, (uint16_t)(7999)); //GetPCLK1Freq - частота шины таймера 6 
	WRITE_REG (TIM10->ARR, 10*delay-1); //время задержки
	SET_BIT (TIM10->EGR, TIM_EGR_UG); //запись новых настроек в таймер 
	CLEAR_BIT (TIM10->SR, TIM_SR_UIF);// сброс флага обновления
	
	SET_BIT (TIM10->CR1, TIM_CR1_CEN); //включение таймера
	while (!READ_BIT(TIM10->SR, TIM_SR_UIF)) {} //ожидание установки флага обновления таймера 6
	CLEAR_BIT (TIM10->CR1, TIM_CR1_CEN);  //отключение таймера
	CLEAR_BIT (TIM10->SR, TIM_SR_UIF);
}

void delay_us(uint16_t delay)
{

	SET_BIT (RCC->APB2ENR, RCC_APB2ENR_TIM10EN ); //тактирование таймера 6
	CLEAR_REG(TIM10->CNT);
	WRITE_REG (TIM10->PSC, (uint16_t)(HAL_RCC_GetPCLK2Freq()/1000000) - 1);  	 
	WRITE_REG (TIM10->ARR, delay); //время задержки
	SET_BIT (TIM10->EGR, TIM_EGR_UG); //запись новых настроек в таймер 
	CLEAR_BIT (TIM10->SR, TIM_SR_UIF);// сброс флага обновления
	
	SET_BIT (TIM10->CR1, TIM_CR1_CEN); //включение таймера
	while (!READ_BIT(TIM10->SR, TIM_SR_UIF)) {} //ожидание установки флага обновления таймера 6
	CLEAR_BIT (TIM10->CR1, TIM_CR1_CEN);  //отключение таймера
	CLEAR_BIT (TIM10->SR, TIM_SR_UIF);
}
