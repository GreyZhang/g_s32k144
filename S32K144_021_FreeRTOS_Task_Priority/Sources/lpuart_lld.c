#include "lpuart_lld.h"

uint8_t lpuart_lld_rx_data[5];
uint8_t lpuart_lld_rx_flag = 0U;
uint32_t lpuart_lld_rx_bytes_num = 0U;

void lpuart_lld_init(void)
{
    /* Initialize LPUART instance */
    LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
    INT_SYS_SetPriority(LPUART1_RxTx_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
}

void lpuart_lld_polling_task(void)
{
/*     uint8_t rxBuff[5]; */
/*     static status_t rx_status = STATUS_UNSUPPORTED; */

/*     if(rx_status == STATUS_UNSUPPORTED) */
/*     { */
/*         rx_status = LPUART_DRV_ReceiveDataPolling(INST_LPUART1,rxBuff,5); */
/*     } */

/*     if(STATUS_SUCCESS == rx_status) */
/*     { */
/*         memcpy(lpuart_lld_rx_data, rxBuff, 5); */
/*         printf("UART received data: %s\n", lpuart_lld_rx_data); */
/*         rx_status = LPUART_DRV_ReceiveDataPolling(INST_LPUART1,rxBuff,5); */
/*         lpuart_lld_rx_flag = 1U; */
/*         lpuart_lld_rx_bytes_num += 5U; */
/*     } */
}

void lpuart_lld_step(void)
{
}

void freertos_task_uart_rx(void *pvParameters)
{
    const TickType_t delay_tick_1ms = pdMS_TO_TICKS(1UL);
    TickType_t last_wake_time = xTaskGetTickCount();
    status_t rx_status;
    uint8_t rxBuff[5];

    (void) pvParameters;

    rx_status = LPUART_DRV_ReceiveDataPolling(INST_LPUART1,rxBuff,5);

    for(;;)
    {
        if(STATUS_SUCCESS == rx_status)
        {
            memcpy(lpuart_lld_rx_data, rxBuff, 5);
            printf("UART received data: %s\n", lpuart_lld_rx_data);
            rx_status = LPUART_DRV_ReceiveDataPolling(INST_LPUART1,rxBuff,5);
            lpuart_lld_rx_bytes_num += 5U;
        }
        vTaskDelayUntil(&last_wake_time, delay_tick_1ms);
    }
}

