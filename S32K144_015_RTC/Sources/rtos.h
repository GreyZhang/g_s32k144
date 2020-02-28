#ifndef RTOS_H
#define RTOS_H

#include "FreeRTOS.h"
#include "task.h"

#define PEX_RTOS_INIT board_init
#define PEX_RTOS_START rtos_start

void board_init(void);
void rtos_start(void);
void freertos_task_1000ms(void *pvParameters);
void freertos_task_trigger_by_queue(void *pvParameters);

#endif

