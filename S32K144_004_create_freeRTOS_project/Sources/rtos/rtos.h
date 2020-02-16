#ifndef RTOS_H
#define RTOS_H

#include "FreeRTOS.h"
#include "task.h"

#define PEX_RTOS_START rtos_start

void rtos_start(void);
void freertos_task_1000ms(void *pvParameters);

#endif

