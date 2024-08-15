
#ifndef __ENCODER_H__
#define __ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif
//Includes -----------------------------------------------------------------------------//
#include "main.h"

//Private typedef-----------------------------------------------------------------------//
typedef struct 
{
	int32_t 	prevCounter; 			//сохранённое показание энкодера
	int32_t 	currCounter; 			//текущее показание энкодера
	int32_t 	need_step;
} encoder_data_t;

//Prototypes-----------------------------------------------------------------------------//
void encoder_init(void) ;
void read_encoder (encoder_data_t * ) ;

#ifdef __cplusplus
}
#endif
#endif 

