#include "lpit_lld.h"

void lpit_lld_init(void)
{
    LPIT_DRV_Init(INST_LPIT1, &lpit1_InitConfig);
    LPIT_DRV_InitChannel(INST_LPIT1, 0, &lpit1_ChnConfig0);
    /* Install LPIT_ISR as LPIT interrupt handler */
    INT_SYS_InstallHandler(LPIT0_Ch0_IRQn, &lpit_ch0_isr, (isr_t *)0);

    /* Start LPIT0 channel 0 counter */
    LPIT_DRV_StartTimerChannels(INST_LPIT1, (1 << 0));
}

void lpit_ch0_isr(void)
{
    LPIT_DRV_ClearInterruptFlagTimerChannels(INST_LPIT1, (1 << 0));
    PINS_DRV_TogglePins(PTD, 1 << 0);
}
