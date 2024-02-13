
// Includes ------------------------------------------------------------------//
#include "button.h"
#include "typedef.h"
#include "tim.h"

// Exported types -------------------------------------------------------------//
static struct KEY_MACHINE_t Key_Machine;

// Prototypes ---------------------------------------------------------------------------------------//
static uint8_t scan_buttons_GPIO (void);

//Private defines -----------------------------------------------------------------------------------//
#define KEY_BOUNCE_TIME 			50 				// ����� �������� � ��
#define KEY_AUTOREPEAT_TIME 	100 			// ����� ����������� � ��
#define COUNT_REPEAT_BUTTON 	5

// Private variables -------------------------------------------------------------------------------//
uint8_t count_autorepeat = 0; //������� ��������� ������

//--------------------------------------------------------------------------------------------------//
uint16_t scan_keys (void)
{
	static __IO uint8_t key_state = KEY_STATE_OFF; // ��������� ��������� ������ - �� ������
	static __IO uint16_t key_code;
	//static __IO uint16_t key_repeat_time; // ������� ������� �������
	
	if(key_state == KEY_STATE_OFF) //���� ������ ���� �������� - �������� �������
	{
		if(LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin) == 1)	//���� ������ ���� ������ - ��������� ���� ������� ������
		{
			key_state =  KEY_STATE_ON; //������� � ����� ������� ������
			key_code = KEY_RIGHT;
		}
		else
		{
			if (LL_GPIO_IsInputPinSet(CENTER_BUTTON_GPIO_Port, CENTER_BUTTON_Pin) == 1)
			{
				key_state =  KEY_STATE_ON;
				key_code = KEY_CENTER_SHORT;
			}
			else
			{
				if (LL_GPIO_IsInputPinSet(LEFT_BUTTON_GPIO_Port, LEFT_BUTTON_Pin) == 1)
				{
					key_state =  KEY_STATE_ON;
					key_code = KEY_LEFT;
				}
				else
				{
					if (LL_GPIO_IsInputPinSet(ENCODER_BUTTON_GPIO_Port, ENCODER_BUTTON_Pin) == 1)
					{
						key_state = KEY_STATE_ON;
						key_code = KEY_ENC_SHORT;
					}
					else
					{
						if (LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_Port, MODE_BUTTON_Pin) == 1)
						{
							key_state =  	KEY_STATE_ON;
							key_code 	= 	KEY_MODE_SHORT;
						}
					}
				}
			}
		}
	}
	
	if (key_state ==  KEY_STATE_ON)  //����� ������� ������
	{
		repeat_time (KEY_BOUNCE_TIME); //������ ������� �������� ��������� ��������
		key_state = KEY_STATE_BOUNCE; // ������� � ����� ��������� ��������
	}
	
	if(key_state == KEY_STATE_BOUNCE) //����� ��������� ��������
	{
		if (end_bounce == SET) //���� ���� ��������� �������� ����������
		{
			end_bounce = RESET;  //����� �����
			if(scan_buttons_GPIO() == 0)	 // ���� ������ �������� (������� ����� 50 �� ��� �������)
			{
				key_state = KEY_STATE_OFF; //������� � ��������� ��������� �������� ������� ������
				return NO_KEY; //������� 0 
			}	
			else //���� ������ ���������� ������������
			{	
				repeat_time (KEY_AUTOREPEAT_TIME); //��������� ������� �������� ���������� ������
				key_state = KEY_STATE_AUTOREPEAT;   //������� � ����� ����������� 
				count_autorepeat = 0;
			}
		}
	}
	
	if (key_state == KEY_STATE_AUTOREPEAT) //���� ������� ����� �����������
	{
		if (end_bounce == SET) //���� ���� ��������� �������� ���������� (��������������� � ���������� �������)
		{
			end_bounce = RESET; //����� �����
			if(scan_buttons_GPIO() == 0)	 // ���� ������ ���� �������� (�������� ������� ������ < 150 ��)
			{
				key_state = KEY_STATE_OFF; //������� � ��������� ��������� �������� ������� ������
				return key_code; //�������� ������ ������
			}
			else //���� ������ ���������� ������������
			{			
				if (count_autorepeat < COUNT_REPEAT_BUTTON)
				{	count_autorepeat++;	}
				else //���� ������ ������������ ����� 650 ��
				{	
					switch (key_code)
					{
						case KEY_CENTER_SHORT:
							key_code = KEY_CENTER_LONG;	
							break;
						
						case KEY_ENC_SHORT:
							key_code = KEY_ENC_LONG;	
							break;
						
						case KEY_MODE_SHORT:
							key_code = KEY_MODE_LONG;	
							break;	
						
						default:
							break;	
					}
				}
				repeat_time (KEY_AUTOREPEAT_TIME); //��������� ������� �������� ���������� ������
			} 	
		}					
	}
	return NO_KEY;
}

//-------------------------------------------------------------------------------------------------//
static uint8_t scan_buttons_GPIO (void)
{
	return ((LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin)) 		|| 
					(LL_GPIO_IsInputPinSet(CENTER_BUTTON_GPIO_Port, CENTER_BUTTON_Pin)) 	|| 
					(LL_GPIO_IsInputPinSet(LEFT_BUTTON_GPIO_Port, LEFT_BUTTON_Pin)) 			||
					(LL_GPIO_IsInputPinSet(ENCODER_BUTTON_GPIO_Port, ENCODER_BUTTON_Pin)) ||
					(LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_Port, MODE_BUTTON_Pin))) 			; 																																		
}

