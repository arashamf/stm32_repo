#ifndef MCU_ini
#define MCU_ini
void LEDS_ini ();
void UART2_ini ();
void DMA1_ini ();
void PLL_ini (uint8_t , uint8_t , uint32_t , uint32_t );
#define BAUDRATE 57600

// определения частот шин микроконтроллера
#define CPU_CLOCK 168000000L // частота процессора
#define APB1_CLOCK 42000000L // частота шины APB1
#endif
