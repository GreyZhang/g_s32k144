#include "rtos.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "string.h"

uint32_t freertos_counter_1000ms = 0U;

void board_init(void)
{
    /* Initialize and configure clocks
     *  -   Setup system clocks, dividers
     *  -   see clock manager component for more details
     */
    CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
    /* Initialize LPUART instance */
    LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
    INT_SYS_SetPriority(LPUART1_RxTx_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
}

void rtos_start(void)
{
    UBaseType_t priority = 0U;
    /* Start the two tasks as described in the comments at the top of this
       file. */

    xTaskCreate(freertos_task_1000ms, "1000ms", configMINIMAL_STACK_SIZE, NULL, ++priority, NULL );

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
       will never be reached.  If the following line does execute, then there was
       insufficient FreeRTOS heap memory available for the idle and/or timer tasks
       to be created.  See the memory management section on the FreeRTOS web site
       for more details. */
    for( ;; )
    {
        /* no code here */
    }
}

void freertos_task_1000ms(void *pvParameters)
{
    const TickType_t delay_counter_1000ms = pdMS_TO_TICKS(1000UL);
    char test_str[] = "hello world\n";
    uint8_t tx_buf[20];
    (void)pvParameters;

    memcpy(tx_buf, test_str, sizeof(test_str));

    while(1)
    {
        freertos_counter_1000ms++;
        LPUART_DRV_SendDataBlocking(INST_LPUART1, tx_buf, sizeof(test_str), 500);
        vTaskDelay(delay_counter_1000ms);
    }
}
