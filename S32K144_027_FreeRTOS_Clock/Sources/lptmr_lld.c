#include "lptmr_lld.h"

void lptmr_lld_init(void)
{
    LPTMR_DRV_Init(INST_LPTMR1, &lpTmr1_config0, 0);
    LPTMR_DRV_StartCounter(INST_LPTMR1);
}

