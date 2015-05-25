#ifndef __READVB_H__
#define __READVB_H__

#include <stdint.h>

#define ADC1_DR_Address ((u32)0x4001244C)

void ADC1_Init(void);
uint16_t readVB(void);

#endif
