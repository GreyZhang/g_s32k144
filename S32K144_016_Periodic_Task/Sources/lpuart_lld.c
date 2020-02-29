#include "lpuart_lld.h"


void lpuart_lld_init(void)
{
    /* Initialize LPUART instance */
    LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
    INT_SYS_SetPriority(LPUART1_RxTx_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
}

void lpuart_lld_step(void)
{
    uint8_t rxBuff[5];
    status_t rx_status = STATUS_UNSUPPORTED;

    rx_status = LPUART_DRV_ReceiveData(INST_LPUART1,rxBuff,5);

    if(STATUS_SUCCESS == rx_status)
    {
        printf("received data: %s\n", rxBuff);
        rx_status = STATUS_UNSUPPORTED;
    }
}
