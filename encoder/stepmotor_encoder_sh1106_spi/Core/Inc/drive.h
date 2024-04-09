
#ifndef __DRIVE_H__
#define __DRIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"

// Defines ---------------------------------------------------------------------//


// Prototypes ------------------------------------------------------------------//
void step_drive (void);
void semistep_drive (void);
void one_semistep (void);
void reverse_one_semistep (void);
void one_step (void);
void reset_all_pin (void);
void set_all_pin (void);
// Variables ------------------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif 

