
// Includes ------------------------------------------------------------------//
#include "drive.h"
#include "gpio.h"
#include "tim.h"

// Functions -----------------------------------------------------------------//


// Variables -----------------------------------------------------------------//
const uint8_t number_check_pins = 4;

const uint8_t motorPhasesSemiStep[8][4] = 
{
  // Motor Pin  1  2  3  4
  // Color      Bl Pi Ye Or
  {             1, 1, 0, 0},  // [phase][pin]
  {             0, 1, 0, 0},
  {             0, 1, 1, 0},
  {             0, 0, 1, 0},
  {             0, 0, 1, 1},
  {             0, 0, 0, 1},
  {             1, 0, 0, 1},
  {             1, 0, 0, 0}
};

const uint8_t motorPhasesStep[4][4] = 
{
  // Motor Pin  1  2  3  4
  // Color      Bl Pi Ye Or
  {             1, 0, 0, 1},  // [phase][pin]
  {             1, 1, 0, 0},
  {             0, 1, 1, 0},
  {             0, 0, 1, 1}
};

//------------------------------------------------------------------------------//
void semistep_drive (void)
{
	IN1(ON); IN2(ON); IN3(OFF); IN4(OFF);
	delay_us (750);
	IN1(OFF); IN2(ON); IN3(OFF); IN4(OFF);
	delay_us (750);
	IN1(OFF); IN2(ON); IN3(ON); IN4(OFF);
	delay_us (750);
	IN1(OFF); IN2(OFF); IN3(ON); IN4(OFF);
	delay_us (750);
	IN1(OFF); IN2(OFF); IN3(ON); IN4(ON);
	delay_us (750);
	IN1(OFF); IN2(OFF); IN3(OFF); IN4(ON);
	delay_us (750);
	IN1(ON); IN2(OFF); IN3(OFF); IN4(ON);
	delay_us (750);
	IN1(ON); IN2(OFF); IN3(OFF); IN4(OFF);
	delay_us (750);
	
	IN1(OFF); IN2(OFF); IN3(OFF); IN4(OFF);	
	
}

//------------------------------------------------------------------------------//
void step_drive (void)
{
	IN1(ON); IN2(ON); IN3(OFF); IN4(OFF);
	delay_us (2500);
	IN1(OFF); IN2(ON); IN3(ON); IN4(OFF);
	delay_us (2500);
	IN1(OFF); IN2(OFF); IN3(ON); IN4(ON);
	delay_us (2500);
	IN1(ON); IN2(OFF); IN3(OFF); IN4(ON);
	delay_us (2500);
	
	IN1(OFF); IN2(OFF); IN3(OFF); IN4(OFF);	
}

//------------------------------------------------------------------------------//
void one_semistep (void)
{
	size_t numberPhases = 0;
	numberPhases = sizeof(motorPhasesSemiStep) / sizeof(motorPhasesSemiStep[0]);
	//reset_all_pin ();
	for (uint8_t count1 = 0 ; count1 < numberPhases; count1++ )
	{
		for(uint8_t count2 = 0 ; count2 < number_check_pins; count2++ )
		{	
			(*(motorPhasesSemiStep[count1]+count2))? (LL_GPIO_SetOutputPin(outPin[count2].PORTx, outPin[count2].PORT_Pin))
			: (LL_GPIO_ResetOutputPin(outPin[count2].PORTx, outPin[count2].PORT_Pin));  
		}
		delay_us (1800);
	}
	reset_all_pin ();
}

//------------------------------------------------------------------------------//
void reverse_one_semistep (void)
{
	size_t numberPhases = 0;
	numberPhases = sizeof(motorPhasesSemiStep) / sizeof(motorPhasesSemiStep[0]);
//	reset_all_pin ();
	for (uint8_t count1 = 0 ; count1 < numberPhases; count1++ )
	{
		for(uint8_t count2 = 0 ; count2 < number_check_pins; count2++ )
		{	
			(*(motorPhasesSemiStep[count1]+count2))? (LL_GPIO_ResetOutputPin(outPin[count2].PORTx, outPin[count2].PORT_Pin))
			: (LL_GPIO_SetOutputPin(outPin[count2].PORTx, outPin[count2].PORT_Pin));  
		}
		delay_us (1000);
	}
	reset_all_pin ();
}

//------------------------------------------------------------------------------//
void one_step (void)
{
	size_t numberPhases = 0;
	numberPhases = sizeof(motorPhasesStep) / sizeof(motorPhasesStep[0]);
	
	for (uint8_t count1 = 0 ; count1 < numberPhases; count1++ )
	{
		for(uint8_t count2 = 0 ; count2 < number_check_pins; count2++ )
		{	
			(*(motorPhasesStep[count1]+count2))? (LL_GPIO_SetOutputPin(outPin[count2].PORTx, outPin[count2].PORT_Pin))
			: (LL_GPIO_ResetOutputPin(outPin[count2].PORTx, outPin[count2].PORT_Pin));  
		}
		delay_us (3000);
	}
	reset_all_pin ();
}

//------------------------------------------------------------------------------//
void reset_all_pin (void)
{
		for(uint8_t count = 0 ; count < number_check_pins; count++ )
		{	
			(LL_GPIO_ResetOutputPin(outPin[count].PORTx, outPin[count].PORT_Pin));  
		}
}

//------------------------------------------------------------------------------//
void set_all_pin (void)
{
		for(uint8_t count = 0 ; count < number_check_pins; count++ )
		{	
			(LL_GPIO_SetOutputPin(outPin[count].PORTx, outPin[count].PORT_Pin));  
		}
}
