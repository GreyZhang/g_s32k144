#ifndef CAN_LLD_H
#define CAN_LLD_H

#include "canCom1.h"
#include "flexcan_hw_access.h"

#define RX_MSG_ID 0x100U
#define CAN_LLD_PRINTF_TEST_ENABLE 0
#define CAN_LLD_EVENT_COUNTER_DISPLAY_ENABLE 0
#define CAN_LLD_ERROR_PRINT_ENABLE 1

void can_lld_init(void);
void can_lld_step(void);
void can_lld_tx(uint32_t mailbox, uint32_t messageId, uint8_t * data, uint32_t len);
void can_lld_cbk_func(uint8_t instance, flexcan_event_type_t eventType,
                                   uint32_t buffIdx, flexcan_state_t *flexcanState);
void can_lld_fifo_rx_func(void);

#endif
