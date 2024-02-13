
/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "encoder.h"
#include "print_display.h"
/* --------------------------------------------------------------------------*/
TIM_TypeDef * TIM_ENC = TIM1;

//--------------------------------------------------------------------------------------------------------//
void encoder_init(void) 
{
    
  LL_TIM_SetCounter(TIM_ENC, 32760); // ��������� �������� ��������:
	
	LL_TIM_CC_EnableChannel(TIM_ENC, LL_TIM_CHANNEL_CH1); //Enable the encoder interface channels 
	LL_TIM_CC_EnableChannel(TIM_ENC, LL_TIM_CHANNEL_CH2);

  LL_TIM_EnableCounter(TIM_ENC);     // ��������� �������
}

/* --------------------------------------------------------------------------*/
void read_encoder (encoder_data_t * HandleEncData) 
{
	int32_t currCounter=0;
	int32_t delta = 0;
	
	currCounter= LL_TIM_GetCounter(TIM_ENC); //������� ��������� ��������
	HandleEncData->currCounter = (32767 - ((currCounter-1) & 0xFFFF))/2; //������� �� 2, ���� ������� (������ = 2 ��������)
	
	if(HandleEncData->currCounter != HandleEncData->prevCounter) //���� ��������� �������� ����������
	{
		delta = (HandleEncData->currCounter - HandleEncData->prevCounter); //������� ����� ������� � ���������� ���������� ��������
    HandleEncData->prevCounter = HandleEncData->currCounter; //���������� �������� ��������� ��������    
		if((delta > -20) && (delta < 20)) // ������ �� �������� ��������� � ������������ �������� 
		{
			if (delta != 0) //���� ���������� ��������� ��������
			{ 				
				HandleEncData->need_step += delta;
				default_screen(HandleEncData);
			}
			else
			{	delta = 0;	}
		}
	}
}

