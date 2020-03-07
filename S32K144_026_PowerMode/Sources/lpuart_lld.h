#ifndef LPUART_LLD_H
#define LPUART_LLD_H

#include "lpuart1.h"
#include "FreeRTOS.h"
#include "printf.h"
#include "string.h"
#include "task.h"

extern uint32_t lpuart_lld_rx_bytes_num;
extern uint8_t lpuart_lld_data_received_flg;
extern uint8_t lpuart_lld_rx_data[5];

void lpuart_lld_init(void);
void lpuart_lld_step(void);

#endif
