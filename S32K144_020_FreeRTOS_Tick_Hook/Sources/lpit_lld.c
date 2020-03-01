#include "lpit_lld.h"

#define INC_DIREC 0
#define DEC_DIREC 1

float pit_lld_counter;
uint8_t pit_lld_cnt_direction;

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
    /* PINS_DRV_TogglePins(PTD, 1 << 0); */
    if(pit_lld_cnt_direction == INC_DIREC)
    {
        pit_lld_counter += 0.1F;
        if(pit_lld_counter > 1.0F)
        {
            pit_lld_cnt_direction = DEC_DIREC;
        }
    }
    else
    {
        pit_lld_counter -= 0.1F;
        if(pit_lld_counter < 0.0F)
        {
            pit_lld_cnt_direction = INC_DIREC;
        }
    }
}
