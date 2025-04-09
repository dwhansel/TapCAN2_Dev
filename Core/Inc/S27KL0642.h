#ifndef S27KL0642_H
#define S27KL0642_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

extern void ExtRAM_Init();
extern void RAM_MAXTRAN_Cfg();
extern void EnblRAM_MemMapped();

#ifdef __cplusplus
}
#endif

#endif