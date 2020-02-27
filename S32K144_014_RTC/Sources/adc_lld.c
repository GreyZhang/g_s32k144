#include "adc_lld.h"

/* Variables in which we store data from ADC */
uint16_t adcRawValue;
uint16_t adcMax;
float adcValue;

void adc_lld_init(void)
{
    /* Get ADC max value from the resolution */
    if (adConv1_ConvConfig0.resolution == ADC_RESOLUTION_8BIT)
        adcMax = (uint16_t) (1 << 8);
    else if (adConv1_ConvConfig0.resolution == ADC_RESOLUTION_10BIT)
        adcMax = (uint16_t) (1 << 10);
    else
        adcMax = (uint16_t) (1 << 12);

    ADC_DRV_ConfigConverter(INST_ADCONV1, &adConv1_ConvConfig0);
    ADC_DRV_AutoCalibration(INST_ADCONV1);
    INT_SYS_SetPriority(ADC0_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
}

void adc_lld_step(void)
{
    /* Configure ADC channel and software trigger a conversion */
    ADC_DRV_ConfigChan(INST_ADCONV1, 0U, &adConv1_ChnConfig0);
    /* Wait for the conversion to be done */
    ADC_DRV_WaitConvDone(INST_ADCONV1);
    /* Store the channel result into a local variable */
    ADC_DRV_GetChanResult(INST_ADCONV1, 0U, &adcRawValue);

    /* Process the result to get the value in volts */
    adcValue = ((float) adcRawValue / adcMax) * (ADC_VREFH - ADC_VREFL);

    printf("adcMax: %d\n", adcMax);
    printf("adc channel: %d\n", adConv1_ChnConfig0.channel);
    printf("adc value is: %f\n", adcValue);
}

