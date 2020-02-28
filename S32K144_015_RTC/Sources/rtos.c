#include "rtos.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "string.h"
#include "lpit_lld.h"
#include "printf.h"
#include "freemaster.h"
#include "math.h"
#include "adConv1.h"
#include "pdb1.h"
#include "adc_lld.h"
#include "rtc_lld.h"

#define LED_TEST_MODE 0
#define FREERTOS_QUEUE_TEST_MODE 0

uint32_t freertos_counter_1000ms = 0U;
double value_sin_x;
double value_sin_y;

#if FREERTOS_QUEUE_TEST_MODE
QueueHandle_t freertos_queue_test = NULL;
#endif

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
    PINS_DRV_SetPins(PTD, (1 << 0) | (1 << 15) |(1 << 16));
    /* EDMA_DRV_Init(&dmaController1_State, &dmaController1_InitConfig0, \ */
    /*               edmaChnStateArray, edmaChnConfigArray, EDMA_CONFIGURED_CHANNELS_COUNT); */
    /* Initialize LPUART instance */
    LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
#if FMSTR_DISABLE
#else
    INT_SYS_InstallHandler(LPUART1_RxTx_IRQn, FMSTR_Isr, NULL);
    FMSTR_Init();
#endif
    INT_SYS_SetPriority(LPUART1_RxTx_IRQn,configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    adc_lld_init();
    rtc_lld_init();
    lpit_lld_init();
}

void rtos_start(void)
{
    UBaseType_t priority = 0U;
    /* Start the two tasks as described in the comments at the top of this
       file. */
#if  FREERTOS_QUEUE_TEST_MODE
    freertos_queue_test = xQueueCreate(10, sizeof( unsigned long ) );
#endif

    xTaskCreate(freertos_task_1000ms, "1000ms", 2 * configMINIMAL_STACK_SIZE, NULL, ++priority, NULL );
#if FREERTOS_QUEUE_TEST_MODE
    xTaskCreate(freertos_task_trigger_by_queue, "queue", configMINIMAL_STACK_SIZE, NULL, ++priority, NULL );
#endif

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
    uint32_t print_indicating_counter = 0U;
#if FREERTOS_QUEUE_TEST_MODE
    uint32_t counter_sent_by_queue = 0U;
    uint8_t i = 0U;
#endif

    (void)pvParameters;

    memcpy(tx_buf, test_str, sizeof(test_str));

    while(1)
    {
        freertos_counter_1000ms++;
        /* LPUART_DRV_SendDataBlocking(INST_LPUART1, tx_buf, sizeof(test_str), 100); */

#if LED_TEST_MODE
        /* test code for LED blink */
        PINS_DRV_TogglePins(PTD, 1 << 0);
        PINS_DRV_TogglePins(PTD, 1 << 15);
        PINS_DRV_TogglePins(PTD, 1 << 16);
#endif
#if FREERTOS_QUEUE_TEST_MODE
        for(i = 0U; i < 9U; i++)
        {
            xQueueSend(freertos_queue_test, &counter_sent_by_queue, 0 );
            counter_sent_by_queue++;
        }
#endif


        switch(print_indicating_counter)
        {
        case 1U:
            printf("1. test for ADC:\n");
            adc_lld_step();
            break;
        case 2U:
            printf("2. test for RTC:\n");
            rtc_lld_step();
            break;
        default:
            printf("-----new test loop started-----\n");
            print_indicating_counter = 0U;
            break;
        }

        print_indicating_counter++;
        vTaskDelay(delay_counter_1000ms);
    }
}

#if FREERTOS_QUEUE_TEST_MODE
void freertos_task_trigger_by_queue(void *pvParameters)
{
    uint32_t received_data;
    uint8_t data[] = "deadbeaf\n";

    (void)pvParameters;

    while(1)
    {
        xQueueReceive( freertos_queue_test, &received_data, portMAX_DELAY );

        LPUART_DRV_SendDataBlocking(INST_LPUART1, &data[received_data % 9], 1, 100);
    }
}
#endif

void vApplicationIdleHook( void )
{
#if FMSTR_DISABLE
#else
    static FMSTR_APPCMD_CODE cmd;
    static FMSTR_APPCMD_PDATA cmdDataP;
    static FMSTR_SIZE cmdSize;

    value_sin_x += 0.0001;
    value_sin_y = sin(value_sin_x);

    /* Process FreeMASTER application commands */
    cmd = FMSTR_GetAppCmd();
    if (cmd != FMSTR_APPCMDRESULT_NOCMD)
    {
        cmdDataP = FMSTR_GetAppCmdData(&cmdSize);
        switch (cmd)
        {
        case 0:
            /* Acknowledge the command */
            FMSTR_AppCmdAck(0);
            break;
        case 1:
            /* Acknowledge the command */
            FMSTR_AppCmdAck(0);
            break;
        case 2:
            /* Acknowledge the command */
            FMSTR_AppCmdAck(0);
            break;
        case 3:
            /* Acknowledge the command */
            FMSTR_AppCmdAck(0);
            break;
        default:
            /* Acknowledge the command with failure */
            FMSTR_AppCmdAck(1);
            break;
        }
    }

    /* Handle the protocol decoding and execution */
    FMSTR_Poll();

    (void)cmdDataP;
#endif
}
