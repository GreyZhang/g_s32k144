#ifndef ADC_LLD_H
#define ADC_LLD_H

#include "adConv1.h"
#include "printf.h"

#define ADC_INSTANCE    0UL
#define ADC_CHN         12U
#define ADC_VREFH       5.0f
#define ADC_VREFL       0.0f

void adc_lld_init(void);
void adc_lld_step(void);

#endif
