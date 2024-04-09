#include "systick.h"
#include "main.h"
//#include "HW_Profile.h"

//����������---------------------------------------------------------------------
static uint32_t TicksCounter = 0;
static SysTick_CALLBACK SysTick_CallbackFunction = NULL;

//������������� SysTick � �������� 1 ��------------------------------------------ 
void SysTick_Init(SysTick_CALLBACK CallbackFunction)
{
	/*SysTick->CTRL = 0;
	SysTick->LOAD = (uint32_t)(CPU_CLOCK_VALUE / TICKS_PER_SECOND) - 1;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk;*/
	SysTick_CallbackFunction = CallbackFunction; //����� ���������� ��������� ��������� ��  xTimer_Task(uint32_t portTick)
}

//------------------------------------------------------------------------------- 
void CallbackFunction_Init (SysTick_CALLBACK CallbackFunction)
{
	SysTick_CallbackFunction = CallbackFunction;
}

//------------------������� ���������� ��� ���������� SysTick------------------//� 
void SysTick_Callback (uint32_t Ticks)
{
	TicksCounter = Ticks;
	if ( SysTick_CallbackFunction != NULL ) 
	{	SysTick_CallbackFunction(TicksCounter);	}
}

// ���������� �������� SysTick----------------------------------------------------
uint32_t Get_SysTick(void)
{
	return TicksCounter;
}
 
// �������� � ��------------------------------------------------------------------
void Delay_MS(uint32_t delay)
{	
  uint32_t ticks;

	//ticks = Get_SysTick();
	ticks = HAL_GetTick();
	
	//while( ( Get_SysTick() - ticks ) < delay)
	while((HAL_GetTick() - ticks ) < delay)
	{	__NOP();	}
}
