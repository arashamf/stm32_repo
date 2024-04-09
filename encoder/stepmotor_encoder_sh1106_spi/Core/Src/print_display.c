
/* Includes ------------------------------------------------------------------*/
#include "print_display.h"
#include "OLED.h"

/* --------------------------------------------------------------------------*/
char Display_Buffer[20];

/* --------------------------------------------------------------------------*/
void default_screen (encoder_data_t * HandleEncData)
{
	snprintf (Display_Buffer, sizeof(Display_Buffer), "step=%d", HandleEncData->need_step);
	OLED_Clear(0);
	OLED_DrawStr(Display_Buffer, 2, 45);
	OLED_UpdateScreen();
}

