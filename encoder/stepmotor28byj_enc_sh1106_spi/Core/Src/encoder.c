
/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "encoder.h"
#include "print_display.h"
/* --------------------------------------------------------------------------*/
TIM_TypeDef * TIM_ENC = TIM1;

//--------------------------------------------------------------------------------------------------------//
void encoder_init(void) 
{
    
  LL_TIM_SetCounter(TIM_ENC, 32760); // начальное значение счетчика:
	
	LL_TIM_CC_EnableChannel(TIM_ENC, LL_TIM_CHANNEL_CH1); //Enable the encoder interface channels 
	LL_TIM_CC_EnableChannel(TIM_ENC, LL_TIM_CHANNEL_CH2);

  LL_TIM_EnableCounter(TIM_ENC);     // включение таймера
}

/* --------------------------------------------------------------------------*/
void read_encoder (encoder_data_t * HandleEncData) 
{
	int32_t currCounter=0;
	int32_t delta = 0;
	
	currCounter= LL_TIM_GetCounter(TIM_ENC); //текущее показание энкодера
	HandleEncData->currCounter = (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	
	if(HandleEncData->currCounter != HandleEncData->prevCounter) //если показания энкодера изменились
	{
		delta = (HandleEncData->currCounter - HandleEncData->prevCounter); //разница между текущим и предыдущим показанием энкодера
    HandleEncData->prevCounter = HandleEncData->currCounter; //сохранение текущего показания энкодера    
		if((delta > -20) && (delta < 20)) // защита от дребезга контактов и переполнения счетчика 
		{
			if (delta != 0) //если изменилось положение энкодера
			{ 				
				HandleEncData->need_step += delta;
				default_screen(HandleEncData);
			}
			else
			{	delta = 0;	}
		}
	}
}

