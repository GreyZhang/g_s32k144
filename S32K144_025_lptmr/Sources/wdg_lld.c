#include "wdg_lld.h"


void wdg_lld_init(void)
{
    WDOG_DRV_Init(INST_WATCHDOG1, &watchdog1_Config0);
}

void wdg_lld_feed_dog(void)
{
    WDOG_DRV_Trigger(INST_WATCHDOG1);
}
