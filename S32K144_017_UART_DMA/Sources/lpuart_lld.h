#ifndef LPUART_LLD_H
#define LPUART_LLD_H

#include "lpuart1.h"
#include "FreeRTOS.h"
#include "printf.h"

void lpuart_lld_init(void);
void lpuart_lld_step(void);

#endif
