#include "can_lld.h"
#include "string.h"
#include "lpspiCom1.h"
#include "sbc_uja116x1.h"
#include "printf.h"

status_t can_lld_debug_tx_ret_val;
flexcan_data_info_t can_lld_rx_data_info;
flexcan_msgbuff_t can_lld_rx_test_msg;
flexcan_user_config_t can_lld_config_data_0;
flexcan_user_config_t can_lld_config_data_1;
static uint8_t can_tx_data[8];
flexcan_id_table_t can_lld_fifo_filter_table[8];


void can_lld_init(void)
{
    uint8_t i = 0U;

    for(i = 0U; i < 8U; i++)
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
}

void can_lld_step(void)
{
    status_t rx_ret_value = STATUS_UNSUPPORTED;
    uint8_t i = 0U;

    can_lld_tx(10, 0x77, can_tx_data, 8);
    *(uint32_t *)can_tx_data += 1U;

    rx_ret_value = FLEXCAN_DRV_RxFifo(INST_CANCOM1, &can_lld_rx_test_msg);

    if(rx_ret_value == STATUS_SUCCESS)
    {
        printf("can message is received.\n");
        printf("    1) CAN ID is: %d\n", can_lld_rx_test_msg.msgId);
        printf("    2) CS is: %d\n", can_lld_rx_test_msg.cs);
        printf("    3) date length is: %d\n", can_lld_rx_test_msg.dataLen);
        printf("    4) data is:");
        for(i = 0U; i < can_lld_rx_test_msg.dataLen; i++)
        {
            printf("%2x ", can_lld_rx_test_msg.data[i]);
        }
        printf("\n");
    }
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
