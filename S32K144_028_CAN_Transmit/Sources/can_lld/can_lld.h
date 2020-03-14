#ifndef CAN_LLD_H
#define CAN_LLD_H

#include "canCom1.h"

void can_lld_init(void);
void can_lld_step(void);
void can_lld_tx(uint32_t mailbox, uint32_t messageId, uint8_t * data, uint32_t len);

#endif
