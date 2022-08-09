
#ifndef MCU_INI_H_
#define MCU_INI_H_
void LEDS_ini ();
void UART2_ini ();
uint8_t  PLL_ini (uint8_t, uint8_t, uint32_t, uint32_t);
#define BAUDRATE 57600  //пропускная способность UART
#define APB_CLOCK 42000000L // частота шины APB1
#endif
