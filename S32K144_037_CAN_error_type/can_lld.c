#include "can_lld.h"
#include "string.h"
#include "lpspiCom1.h"
#include "sbc_uja116x1.h"
#include "printf.h"

status_t can_lld_debug_tx_ret_val;
flexcan_data_info_t can_lld_rx_data_info;
flexcan_msgbuff_t can_lld_rx_test_msg;
flexcan_user_config_t can_lld_config_data_1;
flexcan_user_config_t can_lld_config_data_0;
static uint8_t can_tx_data[8];
flexcan_id_table_t can_lld_fifo_filter_table[8];
uint32_t can_lld_event_num;
uint32_t can_lld_rx_complete_num;
uint32_t can_lld_rx_fifo_compete_num;
uint32_t can_lld_rx_fifo_warning_num;
uint32_t can_lld_rx_fifo_overflow_num;
uint32_t can_lld_tx_complete_num;
uint32_t can_lld_wake_up_timeout_num;
uint32_t can_lld_wake_up_match_num;
uint32_t can_lld_self_wake_up_num;
uint32_t can_lld_dma_complete_num;
uint32_t can_lld_dma_error_num;
uint32_t can_lld_error_num;
uint32_t can_lld_default1_num;
uint32_t can_lld_default2_num;
uint32_t can_lld_error_value;

flexcan_msgbuff_t can_lld_rx_fifo_msg;

void can_lld_init(void)
{
    uint8_t i = 0U;

    for (i = 0U; i < 8U; i++)
    {
        can_lld_fifo_filter_table[i].isRemoteFrame = false;
        can_lld_fifo_filter_table[i].isExtendedFrame = false;
        can_lld_fifo_filter_table[i].id = i + 1;
    }

    FLEXCAN_DRV_GetDefaultConfig(&can_lld_config_data_0);
    LPSPI_DRV_MasterInit(LPSPICOM1, &lpspiCom1State, &lpspiCom1_MasterConfig0);
    INT_SYS_SetPriority(LPSPI1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    SBC_Init(&sbc_uja116x1_InitConfig0, LPSPICOM1);
    FLEXCAN_DRV_Init(INST_CANCOM1, &canCom1_State, &canCom1_InitConfig0);
    INT_SYS_SetPriority(CAN0_ORed_0_15_MB_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    /* Configure RX message buffer with index RX_MSG_ID and RX_MAILBOX */
    can_lld_rx_data_info.msg_id_type = FLEXCAN_MSG_ID_STD;
    can_lld_rx_data_info.fd_enable = 0;
    can_lld_rx_data_info.is_remote = 0;
    /* FLEXCAN_DRV_ConfigRxMb(INST_CANCOM1, 0, &can_lld_rx_data_info, RX_MSG_ID); */
    FLEXCAN_DRV_ConfigRxFifo(INST_CANCOM1, FLEXCAN_RX_FIFO_ID_FORMAT_A, can_lld_fifo_filter_table);
    FLEXCAN_DRV_SetRxFifoGlobalMask(INST_CANCOM1, FLEXCAN_RX_FIFO_ID_FORMAT_A, 0);
    FLEXCAN_DRV_GetDefaultConfig(&can_lld_config_data_1);
    FLEXCAN_DRV_InstallEventCallback(INST_CANCOM1, can_lld_cbk_func, NULL);
}

void can_lld_fifo_rx_func(void)
{
    status_t rx_fifo_status = FLEXCAN_DRV_RxFifo(INST_CANCOM1, &can_lld_rx_fifo_msg);

    while (STATUS_SUCCESS == rx_fifo_status)
    {
        rx_fifo_status = FLEXCAN_DRV_RxFifo(INST_CANCOM1, &can_lld_rx_fifo_msg);

#if CAN_LLD_PRINTF_TEST_ENABLE
        if(can_lld_rx_fifo_msg.msgId == 0x10)
        {
            printf("%s\n", can_lld_rx_fifo_msg.data);
        }
#endif
    }
}

void can_lld_step(void)
{
    can_lld_tx(10, 0x77, can_tx_data, 8);
    *(uint32_t *)can_tx_data += 1U;

#if CAN_LLD_EVENT_COUNTER_DISPLAY_ENABLE
    printf("CAN event number: %d\n", can_lld_event_num);
    printf("can_lld_rx_complete_num: %d\n", can_lld_rx_complete_num);
    printf("can_lld_rx_fifo_compete_num: %d\n", can_lld_rx_fifo_compete_num);
    printf("can_lld_rx_fifo_warning_num: %d\n", can_lld_rx_fifo_warning_num);
    printf("can_lld_rx_fifo_overflow_num: %d\n", can_lld_rx_fifo_overflow_num);
    printf("can_lld_tx_complete_num: %d\n", can_lld_tx_complete_num);
    printf("can_lld_wake_up_timeout_num: %d\n", can_lld_wake_up_timeout_num);
    printf("can_lld_wake_up_match_num: %d\n", can_lld_wake_up_match_num);
    printf("can_lld_self_wake_up_num: %d\n", can_lld_self_wake_up_num);
    printf("can_lld_dma_complete_num: %d\n", can_lld_dma_complete_num);
    printf("can_lld_dma_error_num: %d\n", can_lld_dma_error_num);
    printf("can_lld_error_num: %d\n", can_lld_error_num);
    printf("can_lld_default1_num: %d\n", can_lld_default1_num);
    printf("can_lld_default2_num: %d\n", can_lld_default2_num);
#endif

#if CAN_LLD_ERROR_PRINT_ENABLE
    can_lld_error_value = FLEXCAN_DRV_GetErrorStatus(INST_CANCOM1);
    printf("can error information: %b\n", can_lld_error_value);

    if(can_lld_error_value & CAN_ESR1_ERRINT_MASK)
    {
        printf("ERR flag is %d\n", (can_lld_error_value & CAN_ESR1_ERRINT_MASK) >> CAN_ESR1_ERRINT_SHIFT);
    }

    if(can_lld_error_value & CAN_ESR1_BOFFINT_MASK)
    {
        printf("busoff flag is %d\n", (can_lld_error_value & CAN_ESR1_BOFFINT_MASK) >> CAN_ESR1_BOFFINT_SHIFT);
    }

/* #define FLEXCAN_ALL_INT                                  (0x3B0006U) */
    if((can_lld_error_value & 0x3B0006U) != 0)
    {
        printf("try to clear error flags.\n");
        FLEXCAN_ClearErrIntStatusFlag(CAN0);
    }
#endif
}

/* @brief: Send data via CAN to the specified mailbox with the specified message id
 * @param mailbox   : Destination mailbox number
 * @param messageId : Message ID
 * @param data      : Pointer to the TX data
 * @param len       : Length of the TX data
 * @return          : None
 */
void can_lld_tx(uint32_t mailbox, uint32_t messageId, uint8_t *data, uint32_t len)
{
    /* Set information about the data to be sent
     *  - 1 byte in length
     *  - Standard message ID
     *  - Bit rate switch enabled to use a different bitrate for the data segment
     *  - Flexible data rate enabled
     *  - Use zeros for FD padding
     */
    static flexcan_data_info_t dataInfo;

    dataInfo.data_length = len;
    dataInfo.fd_enable = 0;
    dataInfo.msg_id_type = FLEXCAN_MSG_ID_STD;
    dataInfo.is_remote = 0;

    /* Configure TX message buffer with index TX_MSG_ID and TX_MAILBOX*/
    FLEXCAN_DRV_ConfigTxMb(INST_CANCOM1, mailbox, &dataInfo, messageId);
    FLEXCAN_DRV_AbortTransfer(INST_CANCOM1, mailbox);

    /* Execute send non-blocking */
    can_lld_debug_tx_ret_val = FLEXCAN_DRV_Send(INST_CANCOM1, mailbox, &dataInfo, messageId, data);
}


void can_lld_cbk_func(uint8_t instance, flexcan_event_type_t eventType,
                      uint32_t buffIdx, flexcan_state_t *flexcanState)
{
    can_lld_event_num++;

    switch (instance)
    {
    case INST_CANCOM1:
        switch (eventType)
        {
        case FLEXCAN_EVENT_RX_COMPLETE:
            can_lld_rx_complete_num++;
            break;
        case FLEXCAN_EVENT_RXFIFO_COMPLETE:
            can_lld_rx_fifo_compete_num++;
            break;
        case FLEXCAN_EVENT_RXFIFO_WARNING:
            can_lld_rx_fifo_warning_num++;
            break;
        case FLEXCAN_EVENT_RXFIFO_OVERFLOW:
            can_lld_rx_fifo_overflow_num++;
            break;
        case FLEXCAN_EVENT_TX_COMPLETE:
            can_lld_tx_complete_num++;
            break;
        case FLEXCAN_EVENT_WAKEUP_TIMEOUT:
            can_lld_wake_up_timeout_num++;
            break;
        case FLEXCAN_EVENT_WAKEUP_MATCH:
            can_lld_wake_up_match_num++;
            break;
        case FLEXCAN_EVENT_SELF_WAKEUP:
            can_lld_self_wake_up_num++;
            break;
        case FLEXCAN_EVENT_DMA_COMPLETE:
            can_lld_dma_complete_num++;
            break;
        case FLEXCAN_EVENT_DMA_ERROR:
            can_lld_dma_error_num++;
            break;
        case FLEXCAN_EVENT_ERROR:
            can_lld_error_num++;
            break;
        default:
            can_lld_default2_num++;
            break;
        }
        break;
    default:
        can_lld_default1_num++;
        break;
    }
}
