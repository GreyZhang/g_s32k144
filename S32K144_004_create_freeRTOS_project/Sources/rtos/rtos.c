#include "rtos.h"

uint32_t freertos_counter_1000ms = 0U;

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
    (void)pvParameters;

    const TickType_t delay_counter_1000ms = pdMS_TO_TICKS(1000UL);

    while(1)
    {
        freertos_counter_1000ms++;
        vTaskDelay(delay_counter_1000ms);
    }
}
