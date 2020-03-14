#ifndef POWER_LLD_H
#define POWER_LLD_H

#include "pwrMan1.h"
#include "printf.h"

void power_lld_init(void);
void power_lld_print_mode(power_manager_modes_t mode);

#endif
