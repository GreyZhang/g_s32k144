#include "power_lld.h"

void power_lld_init(void)
{
    POWER_SYS_Init(&powerConfigsArr, POWER_MANAGER_CONFIG_CNT,
                   &powerStaticCallbacksConfigsArr, POWER_MANAGER_CALLBACK_CNT);
}

void power_lld_print_mode(power_manager_modes_t mode)
{
    switch (mode)
    {
#if FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    case POWER_MANAGER_HSRUN:
        printf("POWER_MANAGER_HSRUN\n");
        break;
#endif
    case POWER_MANAGER_RUN:
        printf("POWER_MANAGER_RUN\n");
        break;
    case POWER_MANAGER_VLPR:
        printf("POWER_MANAGER_VLPR\n");
        break;
#if FEATURE_SMC_HAS_WAIT_VLPW
    case POWER_MANAGER_WAIT:
        printf("POWER_MANAGER_WAIT\n");
        break;
    case POWER_MANAGER_VLPW:
        printf("POWER_MANAGER_VLPW\n");
        break;
#endif
#if FEATURE_SMC_HAS_PSTOPO
    case POWER_MANAGER_PSTOP1:
        printf("POWER_MANAGER_PSTOP1\n");
        break;
    case POWER_MANAGER_PSTOP2:
        printf("POWER_MANAGER_PSTOP2\n");
        break;
#endif
#if FEATURE_SMC_HAS_STOPO
    case POWER_MANAGER_STOP1:
        printf("POWER_MANAGER_STOP1\n");
        break;
    case POWER_MANAGER_STOP2:
        printf("POWER_MANAGER_STOP2\n");
        break;
#endif
    case POWER_MANAGER_VLPS:
        printf("POWER_MANAGER_VLPS\n");
        break;
    case POWER_MANAGER_MAX:
        printf("POWER_MANAGER_MAX\n");
        break;
    default:
        printf("not support!\n");
    }
}