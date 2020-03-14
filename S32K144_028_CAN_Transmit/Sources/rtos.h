#ifndef RTOS_H
#define RTOS_H

#include "FreeRTOS.h"
#include "task.h"

#define PEX_RTOS_INIT board_init
#define PEX_RTOS_START rtos_start

#define HSRUN (0u) /* High speed run      */
#define RUN   (1u) /* Run                 */
#define VLPR  (2u) /* Very low power run  */
#define STOP1 (3u) /* Stop option 1       */
#define STOP2 (4u) /* Stop option 2       */
#define VLPS  (5u) /* Very low power stop */

void board_init(void);
void rtos_start(void);
void freertos_task_1ms(void *pvParameters);
void freertos_task_1000ms(void *pvParameters);
void freertos_task_trigger_by_queue(void *pvParameters);
void freertos_task_uart_rx(void *pvParameters);
void freertos_task_power_mode_test(void *pvParameters);
void freertos_task_100ms(void *pvParameters);

#endif

