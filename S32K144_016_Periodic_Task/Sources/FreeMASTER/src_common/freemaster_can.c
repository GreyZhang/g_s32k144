/*******************************************************************************
*
* Copyright 2004-2016 NXP Semiconductor, Inc.
*
* This software is owned or controlled by NXP Semiconductor.
* Use of this software is governed by the NXP FreeMASTER License
* distributed with this Material.
* See the LICENSE file distributed for more details.
* 
****************************************************************************//*!
*
* @brief  FreeMASTER CAN communication routines
*
*******************************************************************************/

#include "freemaster.h"
#include "freemaster_private.h"
#include "freemaster_protocol.h"

#if (FMSTR_USE_CAN) && (!(FMSTR_DISABLE))

/***********************************
*  CAN low-level access macros 
***********************************/

#if FMSTR_USE_MSCAN

    #define FMSTR_CAN_ETXI() FMSTR_MSCAN_ETXI() 
    #define FMSTR_CAN_DTXI() FMSTR_MSCAN_DTXI() 
    #define FMSTR_CAN_ERXI() FMSTR_MSCAN_ERXI() 
    #define FMSTR_CAN_DRXI() FMSTR_MSCAN_DRXI() 
    #define FMSTR_CAN_TEST_RXFLG() FMSTR_MSCAN_TEST_RXFLG() 
    #define FMSTR_CAN_CLEAR_RXFLG() FMSTR_MSCAN_CLEAR_RXFLG() 
    #define FMSTR_CAN_TEST_TXFLG() FMSTR_MSCAN_TEST_TXFLG() 
    #define FMSTR_CAN_GET_MBSTATUS() FMSTR_MSCAN_GET_MBSTATUS()
    #define FMSTR_CAN_MAKEIDR0(id) FMSTR_MSCAN_MAKEIDR0(id) 
    #define FMSTR_CAN_MAKEIDR1(id) FMSTR_MSCAN_MAKEIDR1(id) 
    #define FMSTR_CAN_MAKEIDR2(id) FMSTR_MSCAN_MAKEIDR2(id) 
    #define FMSTR_CAN_MAKEIDR3(id) FMSTR_MSCAN_MAKEIDR3(id) 

    #define FMSTR_CAN_TCTX FMSTR_MSCAN_TCTX 
    #define FMSTR_CAN_TCFG(pctx) FMSTR_MSCAN_TCFG(pctx) 
    #define FMSTR_CAN_TID(pctx, idr0, idr1, idr2, idr3) FMSTR_MSCAN_TID(pctx, idr0, idr1, idr2, idr3) 
    #define FMSTR_CAN_TLEN(pctx, len) FMSTR_MSCAN_TLEN(pctx, len) 
    #define FMSTR_CAN_TPRI(pctx, txPri) FMSTR_MSCAN_TPRI(pctx, txPri) 
    #define FMSTR_CAN_PUTBYTE(pctx, dataByte) FMSTR_MSCAN_PUTBYTE(pctx, dataByte) 
    #define FMSTR_CAN_TX(pctx) FMSTR_MSCAN_TX(pctx) 

    #define FMSTR_CAN_RCTX FMSTR_MSCAN_RCTX 
    #define FMSTR_CAN_RINIT(idr0, idr1, idr2, idr3) FMSTR_MSCAN_RINIT(idr0, idr1, idr2, idr3) 
    #define FMSTR_CAN_TINIT(idr0, idr1, idr2, idr3) FMSTR_MSCAN_TINIT(idr0, idr1, idr2, idr3)
    #define FMSTR_CAN_RCFG() FMSTR_MSCAN_RCFG() 
    #define FMSTR_CAN_RX(pctx) FMSTR_MSCAN_RX(pctx) 
    #define FMSTR_CAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3) FMSTR_MSCAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3) 
    #define FMSTR_CAN_RLEN(pctx) FMSTR_MSCAN_RLEN(pctx) 
    #define FMSTR_CAN_GETBYTE(pctx) FMSTR_MSCAN_GETBYTE(pctx) 
    #define FMSTR_CAN_RFINISH(pctx) FMSTR_MSCAN_RFINISH(pctx) 

#elif (FMSTR_USE_FLEXCAN) || (FMSTR_USE_FLEXCAN32)

    #define FMSTR_CAN_ETXI() FMSTR_FCAN_ETXI() 
    #define FMSTR_CAN_DTXI() FMSTR_FCAN_DTXI() 
    #define FMSTR_CAN_ERXI() FMSTR_FCAN_ERXI() 
    #define FMSTR_CAN_DRXI() FMSTR_FCAN_DRXI() 
    #define FMSTR_CAN_TEST_RXFLG() FMSTR_FCAN_TEST_RXFLG() 
    #define FMSTR_CAN_CLEAR_RXFLG() FMSTR_FCAN_CLEAR_RXFLG() 
    #define FMSTR_CAN_TEST_TXFLG() FMSTR_FCAN_TEST_TXFLG() 
    #define FMSTR_CAN_GET_MBSTATUS() FMSTR_FCAN_GET_MBSTATUS()
    #define FMSTR_CAN_MAKEIDR0(id) FMSTR_FCAN_MAKEIDR0(id) 
    #define FMSTR_CAN_MAKEIDR1(id) FMSTR_FCAN_MAKEIDR1(id) 
    #define FMSTR_CAN_MAKEIDR2(id) FMSTR_FCAN_MAKEIDR2(id) 
    #define FMSTR_CAN_MAKEIDR3(id) FMSTR_FCAN_MAKEIDR3(id) 

    #define FMSTR_CAN_TCTX FMSTR_FCAN_TCTX 
    #define FMSTR_CAN_TCFG(pctx) FMSTR_FCAN_TCFG(pctx) 
    #define FMSTR_CAN_TID(pctx, idr0, idr1, idr2, idr3) FMSTR_FCAN_TID(pctx, idr0, idr1, idr2, idr3) 
    #define FMSTR_CAN_TLEN(pctx, len) FMSTR_FCAN_TLEN(pctx, len) 
    #define FMSTR_CAN_TPRI(pctx, txPri) FMSTR_FCAN_TPRI(pctx, txPri) 
    #define FMSTR_CAN_PUTBYTE(pctx, dataByte) FMSTR_FCAN_PUTBYTE(pctx, dataByte) 
    #define FMSTR_CAN_TX(pctx) FMSTR_FCAN_TX(pctx) 

    #define FMSTR_CAN_RCTX FMSTR_FCAN_RCTX 
    #define FMSTR_CAN_RINIT(idr0, idr1, idr2, idr3) FMSTR_FCAN_RINIT(idr0, idr1, idr2, idr3) 
    #define FMSTR_CAN_TINIT(idr0, idr1, idr2, idr3) FMSTR_FCAN_TINIT(idr0, idr1, idr2, idr3)
    #define FMSTR_CAN_RCFG() FMSTR_FCAN_RCFG() 
    #define FMSTR_CAN_RX(pctx) FMSTR_FCAN_RX(pctx) 
    #define FMSTR_CAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3) FMSTR_FCAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3) 
    #define FMSTR_CAN_RLEN(pctx) FMSTR_FCAN_RLEN(pctx) 
    #define FMSTR_CAN_GETBYTE(pctx) FMSTR_FCAN_GETBYTE(pctx) 
    #define FMSTR_CAN_RFINISH(pctx) FMSTR_FCAN_RFINISH(pctx)

#elif FMSTR_USE_MCAN

    #define FMSTR_CAN_ETXI() FMSTR_MCAN_ETXI()
    #define FMSTR_CAN_DTXI() FMSTR_MCAN_DTXI()
    #define FMSTR_CAN_ERXI() FMSTR_MCAN_ERXI()
    #define FMSTR_CAN_DRXI() FMSTR_MCAN_DRXI()
    #define FMSTR_CAN_TEST_RXFLG() FMSTR_MCAN_TEST_RXFLG()
    #define FMSTR_CAN_CLEAR_RXFLG() FMSTR_MCAN_CLEAR_RXFLG()
    #define FMSTR_CAN_TEST_TXFLG() FMSTR_MCAN_TEST_TXFLG()
    #define FMSTR_CAN_MAKEIDR0(id) FMSTR_MCAN_MAKEIDR0(id)
    #define FMSTR_CAN_MAKEIDR1(id) FMSTR_MCAN_MAKEIDR1(id)
    #define FMSTR_CAN_MAKEIDR2(id) FMSTR_MCAN_MAKEIDR2(id)
    #define FMSTR_CAN_MAKEIDR3(id) FMSTR_MCAN_MAKEIDR3(id)

    #define FMSTR_CAN_TCTX FMSTR_MCAN_TCTX
    #define FMSTR_CAN_TCFG(pctx) FMSTR_MCAN_TCFG(pctx)
    #define FMSTR_CAN_TID(pctx, idr0, idr1, idr2, idr3) FMSTR_MCAN_TID(pctx, idr0, idr1, idr2, idr3)
    #define FMSTR_CAN_TLEN(pctx, len) FMSTR_MCAN_TLEN(pctx, len)
    #define FMSTR_CAN_TPRI(pctx, txPri) FMSTR_MCAN_TPRI(pctx, txPri)
    #define FMSTR_CAN_PUTBYTE(pctx, dataByte) FMSTR_MCAN_PUTBYTE(pctx, dataByte)
    #define FMSTR_CAN_TX(pctx) FMSTR_MCAN_TX(pctx)

    #define FMSTR_CAN_RCTX FMSTR_MCAN_RCTX
    #define FMSTR_CAN_RINIT(idr0, idr1, idr2, idr3) FMSTR_MCAN_RINIT(idr0, idr1, idr2, idr3)
    #define FMSTR_CAN_TINIT(idr0, idr1, idr2, idr3) FMSTR_MCAN_TINIT(idr0, idr1, idr2, idr3)
    #define FMSTR_CAN_RCFG() FMSTR_MCAN_RCFG()
    #define FMSTR_CAN_RX(pctx) FMSTR_MCAN_RX(pctx)
    #define FMSTR_CAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3) FMSTR_MCAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3)
    #define FMSTR_CAN_RLEN(pctx) FMSTR_MCAN_RLEN(pctx)
    #define FMSTR_CAN_GETBYTE(pctx) FMSTR_MCAN_GETBYTE(pctx)
    #define FMSTR_CAN_RFINISH(pctx) FMSTR_MCAN_RFINISH(pctx)

#else
#error CAN interface undefined
#endif

/***********************************
*  local variables 
***********************************/

/* Runtime base address used when constant address is not specified */
#if FMSTR_CAN_BASE_DYNAMIC
static FMSTR_ADDR pcm_pCanBaseAddr;
#define FMSTR_CAN_BASE pcm_pCanBaseAddr
#endif

/* FreeMASTER communication buffer (in/out) plus the STS and LEN bytes */
static FMSTR_BCHR pcm_pCommBuffer[FMSTR_COMM_BUFFER_SIZE+3];    

/* FreeMASTER runtime flags */
/*lint -e{960} using union */
static volatile union 
{
    FMSTR_FLAGS all;
    
    struct 
    {
        unsigned bTxActive : 1;        /* response is just being transmitted */
        unsigned bTxFirst : 1;         /* first frame to be send out */
        unsigned bRxActive : 1;        /* just in the middle of receiving (fragmented) frame */
        unsigned bRxFirst : 1;         /* expecting the first frame (FST) */
        unsigned bRxTgl1 : 1;          /* expecting TGL=1 in next frame */
        unsigned bRxFrameReady : 1;    /* frame received (waiting to be handled in poll) */
        unsigned bRxSpecial : 1;       /* special command received (not passed to ProtocolDecode) */
        
    } flg;
    
} pcm_wFlags;

/* receive and transmit buffers and counters */
static FMSTR_SIZE8 pcm_nTxTodo;        /* transmission to-do counter (0 when tx is idle) */
static FMSTR_BPTR  pcm_pTxBuff;        /* pointer to next byte to transmit */
static FMSTR_SIZE8 pcm_nRxCtr;         /* how many bytes received (total across all fragments) */
static FMSTR_BPTR  pcm_pRxBuff;        /* pointer to next free place in RX buffer */
static FMSTR_BCHR  pcm_nRxErr;         /* error raised during receive process */
static FMSTR_BCHR  pcm_nRxCheckSum;    /* checksum of data being received */
static FMSTR_U8    pcm_uTxCtlByte;
static FMSTR_U8    pcm_uTxFrmCtr;      /* TX CAN frame counter (within one FreeMASTER frame) */

#if FMSTR_CAN_CMDID_DYNAMIC
    static FMSTR_U8 pcm_cmdIdr[4] = 
    { 
        FMSTR_CAN_MAKEIDR0(FMSTR_CAN_CMDID),
        FMSTR_CAN_MAKEIDR1(FMSTR_CAN_CMDID),
        FMSTR_CAN_MAKEIDR2(FMSTR_CAN_CMDID),
        FMSTR_CAN_MAKEIDR3(FMSTR_CAN_CMDID)
    };
    
    #define FMSTR_CAN_CMDID_IDR0 pcm_cmdIdr[0]
    #define FMSTR_CAN_CMDID_IDR1 pcm_cmdIdr[1]
    #define FMSTR_CAN_CMDID_IDR2 pcm_cmdIdr[2]
    #define FMSTR_CAN_CMDID_IDR3 pcm_cmdIdr[3]
#else
    #define FMSTR_CAN_CMDID_IDR0 FMSTR_CAN_MAKEIDR0(FMSTR_CAN_CMDID)
    #define FMSTR_CAN_CMDID_IDR1 FMSTR_CAN_MAKEIDR1(FMSTR_CAN_CMDID)
    #define FMSTR_CAN_CMDID_IDR2 FMSTR_CAN_MAKEIDR2(FMSTR_CAN_CMDID)
    #define FMSTR_CAN_CMDID_IDR3 FMSTR_CAN_MAKEIDR3(FMSTR_CAN_CMDID)
#endif

#if FMSTR_CAN_RESPID_DYNAMIC
    static FMSTR_U8 pcm_respIdr[4] = 
    { 
        FMSTR_CAN_MAKEIDR0(FMSTR_CAN_CMDID),
        FMSTR_CAN_MAKEIDR1(FMSTR_CAN_CMDID),
        FMSTR_CAN_MAKEIDR2(FMSTR_CAN_CMDID),
        FMSTR_CAN_MAKEIDR3(FMSTR_CAN_CMDID)
    };
    
    #define FMSTR_CAN_RESPID_IDR0 pcm_respIdr[0]
    #define FMSTR_CAN_RESPID_IDR1 pcm_respIdr[1]
    #define FMSTR_CAN_RESPID_IDR2 pcm_respIdr[2]
    #define FMSTR_CAN_RESPID_IDR3 pcm_respIdr[3]
#else
    #define FMSTR_CAN_RESPID_IDR0 FMSTR_CAN_MAKEIDR0(FMSTR_CAN_RESPID)
    #define FMSTR_CAN_RESPID_IDR1 FMSTR_CAN_MAKEIDR1(FMSTR_CAN_RESPID)
    #define FMSTR_CAN_RESPID_IDR2 FMSTR_CAN_MAKEIDR2(FMSTR_CAN_RESPID)
    #define FMSTR_CAN_RESPID_IDR3 FMSTR_CAN_MAKEIDR3(FMSTR_CAN_RESPID)
#endif

#if FMSTR_DEBUG_TX
/* The poll counter is used to roughly measure duration of test frame transmission.
 * The test frame will be sent once per N.times this measured period. */
static FMSTR_S32 pcm_nDebugTxPollCount; 
/* the N factor for multiplying the transmission time to get the wait time */
#define FMSTR_DEBUG_TX_POLLCNT_XFACTOR 256
#define FMSTR_DEBUG_TX_POLLCNT_MIN     (-1*0x100000L)
#endif

/***********************************
*  local function prototypes
***********************************/

static void FMSTR_Listen(void);
static void FMSTR_RxDone(void);
static void FMSTR_SendError(FMSTR_BCHR nErrCode);

/**************************************************************************//*!
*
* @brief    CAN communication initialization
*
******************************************************************************/

FMSTR_BOOL FMSTR_InitCan(void)
{   
    /* initialize all state variables */
    pcm_wFlags.all = 0U;
    pcm_nTxTodo = 0U;

#if FMSTR_CAN_BASE_DYNAMIC
    if(!pcm_pCanBaseAddr)
        return FMSTR_FALSE;
#endif
    
    /* configure CAN receiving (used by FlexCAN to setup MB) */
    FMSTR_CAN_RINIT(FMSTR_CAN_CMDID_IDR0, FMSTR_CAN_CMDID_IDR1, \
                    FMSTR_CAN_CMDID_IDR2, FMSTR_CAN_CMDID_IDR3);
    
    /* configure CAN transmitting (used by FlexCAN to setup MB) */
    FMSTR_CAN_TINIT(FMSTR_CAN_CMDID_IDR0, FMSTR_CAN_CMDID_IDR1, \
                    FMSTR_CAN_CMDID_IDR2, FMSTR_CAN_CMDID_IDR3);

#if FMSTR_DEBUG_TX
    /* this zero will initiate the test frame transmission 
     * as soon as possible during Listen */
    pcm_nDebugTxPollCount = 0;
#endif
    
    /* start listening for commands */
    FMSTR_Listen();
    
    return FMSTR_TRUE;
}


/**************************************************************************//*!
*
* @brief    Assigning FreeMASTER communication module base address
*
******************************************************************************/

#if FMSTR_CAN_BASE_DYNAMIC
void FMSTR_SetCanBaseAddress(FMSTR_ADDR nCanAddr)
{
    pcm_pCanBaseAddr = nCanAddr;
}
#endif

/**************************************************************************//*!
*
* @brief    Assigning CAN frame ID - command frame
*
******************************************************************************/

void FMSTR_SetCanCmdID(FMSTR_U32 canID)
{   
#if FMSTR_CAN_CMDID_DYNAMIC
    pcm_cmdIdr[0] = FMSTR_CAN_MAKEIDR0(canID);
    pcm_cmdIdr[1] = FMSTR_CAN_MAKEIDR1(canID);
    pcm_cmdIdr[2] = FMSTR_CAN_MAKEIDR2(canID);
    pcm_cmdIdr[3] = FMSTR_CAN_MAKEIDR3(canID);
#else
    FMSTR_UNUSED(canID);    
#endif
}

/**************************************************************************//*!
*
* @brief    Assigning CAN frame ID - response frame
*
******************************************************************************/

void FMSTR_SetCanRespID(FMSTR_U32 canID)
{   
#if FMSTR_CAN_RESPID_DYNAMIC
    pcm_respIdr[0] = FMSTR_CAN_MAKEIDR0(canID);
    pcm_respIdr[1] = FMSTR_CAN_MAKEIDR1(canID);
    pcm_respIdr[2] = FMSTR_CAN_MAKEIDR2(canID);
    pcm_respIdr[3] = FMSTR_CAN_MAKEIDR3(canID);
#else
    FMSTR_UNUSED(canID);    
#endif
}

/**************************************************************************//*!
*
* @brief    Start listening on a CAN bus
*
******************************************************************************/

static void FMSTR_Listen(void)
{
    /* disable transmitter state machine */
    pcm_wFlags.flg.bTxActive = 0U;
    
    /* wait for first frame */
    pcm_wFlags.flg.bRxFrameReady = 0U;
    pcm_wFlags.flg.bRxFirst = 1U;
    pcm_wFlags.flg.bRxActive = 1U;

    /* enable CAN receiving */
    FMSTR_CAN_RCFG();

#if FMSTR_DEBUG_TX
    /* we have just finished the transmission of the test frame, now wait the 32x times the sendtime 
       to receive any command from PC (count<0 is measurement, count>0 is waiting, count=0 is send trigger) */
    if(pcm_nDebugTxPollCount < 0)
        pcm_nDebugTxPollCount *= -32;
#endif
    
#if FMSTR_LONG_INTR || FMSTR_SHORT_INTR
    FMSTR_CAN_ERXI();
#endif
}

/**************************************************************************//*!
*
* @brief    Send response of given error code (no data) 
*
* @param    nErrCode - error code to be sent
*
******************************************************************************/

static void FMSTR_SendError(FMSTR_BCHR nErrCode)
{
    /* fill & send single-byte response */
    *pcm_pCommBuffer = nErrCode;
    FMSTR_SendResponse(pcm_pCommBuffer, 1U);
}

/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting 
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer 
* (including the status byte). It computes the check sum and kicks on tx.
*
******************************************************************************/

void FMSTR_SendResponse(FMSTR_BPTR pResponse, FMSTR_SIZE8 nLength)
{
    FMSTR_U16 chSum = 0U;
    FMSTR_U8 i, c;

    /* remember the buffer to be sent */
    pcm_pTxBuff = pResponse;
    
    /* status byte and data are already there, compute checksum only     */
    for (i=0U; i<nLength; i++)
    {
        c = 0U;
        pResponse = FMSTR_ValueFromBuffer8(&c, pResponse);
        /* add character to checksum */
        chSum += c;
        /* prevent saturation to happen on DSP platforms */
        chSum &= 0xffU;
    }
    
    /* store checksum after the message */
    pResponse = FMSTR_ValueToBuffer8(pResponse, (FMSTR_U8) (((FMSTR_U16)~(chSum)) + 1U));

    /* send the message and the checksum */
    pcm_nTxTodo = (FMSTR_SIZE8) (nLength + 1U); 

    /* now transmitting the response */
    pcm_wFlags.flg.bTxActive = 1U;
    pcm_wFlags.flg.bTxFirst = 1U;

    /* enable TX interrupt */
#if FMSTR_LONG_INTR || FMSTR_SHORT_INTR
    FMSTR_CAN_ETXI();
#endif
#if (FMSTR_SHORT_INTR) && ((FMSTR_USE_FLEXCAN) || (FMSTR_USE_FLEXCAN32) || (FMSTR_USE_MCAN))
    FMSTR_ProcessCanTx();
#endif
}

/**************************************************************************//*!
*
* @brief Send one CAN frame if needed
*
* @return TRUE if frame was actually filled to a buffer and submitted for send
*
* This function is called internally by our interrupt handler or poll routine.
* User may also decide to use FMSTR_CAN_PASSIVE_SW so it is then his responsibility
* to call us when a frame can be send.
*
******************************************************************************/

FMSTR_BOOL FMSTR_TxCan(void)
{
    FMSTR_U8 ch;
    FMSTR_CAN_TCTX tctx;
    FMSTR_SIZE8 len = pcm_nTxTodo;
    
    if(!pcm_wFlags.flg.bTxActive || !pcm_nTxTodo)
        return FMSTR_FALSE;

    if(len > 7)
        len = 7;
    
    /* first byte is control */
    if(pcm_wFlags.flg.bTxFirst)
    {
        /* the first frame and the length*/
        pcm_uTxCtlByte = (FMSTR_U8) (FMSTR_CANCTL_FST | len);
        pcm_uTxFrmCtr = 0U;
        pcm_wFlags.flg.bTxFirst = 0U;
    }
    else
    {
        /* the next frame */
        pcm_uTxCtlByte &= ~(FMSTR_CANCTL_FST | FMSTR_CANCTL_LEN_MASK);
        pcm_uTxCtlByte ^= FMSTR_CANCTL_TGL;
        pcm_uTxCtlByte |= len;
        pcm_uTxFrmCtr++;
    }

    /* prepare transmit buffer */
    FMSTR_CAN_TCFG(&tctx);

    /* set transmit priority and ID */
    FMSTR_CAN_TPRI(&tctx, pcm_uTxFrmCtr);
    FMSTR_CAN_TID(&tctx, FMSTR_CAN_RESPID_IDR0,
        FMSTR_CAN_RESPID_IDR1, FMSTR_CAN_RESPID_IDR2, FMSTR_CAN_RESPID_IDR3);

    /* is it the last frame? */
    pcm_nTxTodo -= len;
    if(!pcm_nTxTodo)
        pcm_uTxCtlByte |= FMSTR_CANCTL_LST; 

    /* set frame len */
    FMSTR_CAN_TLEN(&tctx, (FMSTR_U8) (len+1));

    /* put control byte */
    FMSTR_CAN_PUTBYTE(&tctx, pcm_uTxCtlByte);

    /* put data part */
    while(len--)
    {
        pcm_pTxBuff = FMSTR_ValueFromBuffer8(&ch, pcm_pTxBuff);
        FMSTR_CAN_PUTBYTE(&tctx, ch);
    }

    /* submit frame for transmission */
    FMSTR_CAN_TX(&tctx);

    /* if the full frame is safe in tx buffer(s), release the received command */
    if(!pcm_nTxTodo)
    {
        /* no more transmitting */        
        pcm_wFlags.flg.bTxActive = 0U;

        /* start listening immediately (also frees the last received frame) */
        FMSTR_Listen();
    }

    /* returning TRUE, frame was sent */
    return FMSTR_TRUE;
}


/**************************************************************************//*!
*
* @brief  Handle received CAN frame
*
* @return TRUE if this frame was handled by us (ID matched) 
* 
******************************************************************************/

FMSTR_BOOL FMSTR_RxCan(void)
{
    FMSTR_CAN_RCTX rctx;
    FMSTR_SIZE8 len;
    FMSTR_U8 ctl, ch;

    if(!pcm_wFlags.flg.bRxActive)
        return FMSTR_FALSE;

    /* last FreeMASTER frame not yet handled */
    if(pcm_wFlags.flg.bRxFrameReady)
        return FMSTR_FALSE;

    /* get the frame */
    FMSTR_CAN_RX(&rctx);

    /* test message ID is it matches the CMDID */
    if(!FMSTR_CAN_TEST_RIDR(&rctx, FMSTR_CAN_CMDID_IDR0, FMSTR_CAN_CMDID_IDR1, 
        FMSTR_CAN_CMDID_IDR2, FMSTR_CAN_CMDID_IDR3))
    {
        FMSTR_CAN_RFINISH(&rctx);
        return FMSTR_FALSE;
    }

    /* get the first (control) byte */
    ctl = FMSTR_CAN_GETBYTE(&rctx);

    /* should be master-to-slave (otherwise perhaps a self-received frame) */
    if(!(ctl & FMSTR_CANCTL_M2S))
        goto frame_contd;

    /* first frame resets the state machine */
    if((ctl & FMSTR_CANCTL_FST) || pcm_wFlags.flg.bRxFirst)
    {
        if(!(ctl & FMSTR_CANCTL_FST) || /* must be the first frame! */
            (ctl & FMSTR_CANCTL_TGL))   /* TGL must be zero! */
        {
            /* frame is ours, but we ignore it */
            goto frame_contd;
        }

        /* now receive the rest of bigger FreeMASTER frame */
        pcm_wFlags.flg.bRxFirst = 0U;
        pcm_wFlags.flg.bRxTgl1 = 1U;

        /* special command? */
        if(ctl & FMSTR_CANCTL_SPC)
            pcm_wFlags.flg.bRxSpecial = 1U;
        else
            pcm_wFlags.flg.bRxSpecial = 0U;

        /* start receiving the frame */
        pcm_pRxBuff = pcm_pCommBuffer;
        pcm_nRxCheckSum = 0;
        pcm_nRxCtr = 0;
        pcm_nRxErr = 0;
    }
    else
    {
        /* toggle bit should match */
        if((pcm_wFlags.flg.bRxTgl1 && !(ctl & FMSTR_CANCTL_TGL)) ||
           (!pcm_wFlags.flg.bRxTgl1 && (ctl & FMSTR_CANCTL_TGL)))
        {
            /* invalid sequence detected */
            pcm_nRxErr = FMSTR_STC_CANTGLERR;
            goto frame_done;
        }

        /* expect next frame toggled again */
        pcm_wFlags.flg.bRxTgl1 ^= 1U;
    }

    /* frame is valid, get the data */
    len = (FMSTR_SIZE8) (ctl & FMSTR_CANCTL_LEN_MASK);

    /* sanity check of the len field */
    if(len >= FMSTR_CAN_RLEN(&rctx))
    {
        /* invalid frame length, re-start receiving */
        pcm_nRxErr = FMSTR_STC_CANMSGERR;
        goto frame_done;
    }

    /* will data fit? (+1 is for checksum byte to fit) */
    if((pcm_nRxCtr+len) > (FMSTR_COMM_BUFFER_SIZE+1))
    {
        /* this frame must be ignored, start receiving again */
        pcm_nRxErr = FMSTR_STC_CANMSGERR;
        goto frame_done;
    }

    /* okay, receive all data bytes */
    pcm_nRxCtr += len;
    while(len--)
    {
        ch = FMSTR_CAN_GETBYTE(&rctx);
        pcm_pRxBuff = FMSTR_ValueToBuffer8(pcm_pRxBuff, ch);
        pcm_nRxCheckSum += ch;
    }

    /* was it the last frame? */    
    if(!(ctl & FMSTR_CANCTL_LST))
        goto frame_contd;

    /* done with the FreeMASTER frame, it will be handled later in RxDone */
frame_done:
    /* disable receiver so it does not corrupt the frame until it is handled */
    pcm_wFlags.flg.bRxActive = 0;
    /* frame received. Checksum and frame will be checked later in RxDone */
    pcm_wFlags.flg.bRxFrameReady = 1U;

    /* frame will continue by next chunk next time */
frame_contd:
    FMSTR_CAN_RFINISH(&rctx);
    return FMSTR_TRUE;
}

/**************************************************************************//*!
*
* @brief  Handle received FreeMASTER frame
*
******************************************************************************/

static void FMSTR_RxDone(void)
{
    /* frame really here? */
    if(!pcm_wFlags.flg.bRxFrameReady)
        return ;

    /* acknowledge the flag, we're going to process the frame now */
    pcm_wFlags.flg.bRxFrameReady = 0U;

    /* any fragmentation error detected during the receive process? */
    if(!pcm_nRxErr)
    {
        /* check for our standard frame errors */

        /* checksum should be zero */
        if(pcm_nRxCheckSum)
        {
            pcm_nRxErr = FMSTR_STC_CMDCSERR;
        }
        /* checksum okay, check frame length */
        else
        {
            FMSTR_BPTR pFrame = pcm_pCommBuffer;
            FMSTR_BCHR len;

            pFrame = FMSTR_ValueFromBuffer8(&len, pFrame);

            /* fast command? */
            if((len & FMSTR_FASTCMD) == FMSTR_FASTCMD)
            {
                /* get length */
                len = (FMSTR_BCHR)((len & FMSTR_FASTCMD_DATALEN_MASK) >> FMSTR_FASTCMD_DATALEN_SHIFT);

                /* add command-byte and checksum (are included in the nRxCtr) */
                len += 2;
            }
            /* std command */
            else
            {
                /* get length */
                pFrame = FMSTR_ValueFromBuffer8(&len, pFrame);

                /* add command-byte, length and checksum (are included in the nRxCtr) */
                len += 3;
            }
            
            /* now the len received should match the data bytes received */
            if(pcm_nRxCtr != len)
                pcm_nRxErr = FMSTR_STC_CMDCSERR;
        }
    }

    /* any error? */
    if(pcm_nRxErr)
    {
        FMSTR_SendError(pcm_nRxErr);
    }
    /* no error */
    else 
    {
        /* special CAN command? for our layer */
        if(pcm_wFlags.flg.bRxSpecial)
        {
            FMSTR_BPTR pFrame = pcm_pCommBuffer;
            FMSTR_BCHR c;

            /* get the command */            
            (void)FMSTR_ValueFromBuffer8(&c, pFrame);

            switch(c)
            {
            case FMSTR_CANSPC_PING:
                FMSTR_SendError(FMSTR_STS_OK);
                break;

            default:
                /* unknown command */
                FMSTR_SendError(FMSTR_STC_INVCMD);
            }
        }
        /* standard FreeMASTER command to be passed above */
        else
        {
            /* decode and handle frame by SCI classic driver */
            if(!FMSTR_ProtocolDecoder(pcm_pCommBuffer))
            {
                /* if no response was generated, start listening again, otherwise, 
                   the receive will be initiated after transmission  is complete in 
                   FMSTR_TxCan (this prevents our TX buffer to be corrupted by RX) */
                FMSTR_Listen();  
            }
        }
    }
}

/**************************************************************************//*!
*
* @brief    Handle CAN communication
*
* @note This function can be called either from CAN ISRs or from the polling routine
*
******************************************************************************/

void FMSTR_ProcessCanRx(void)
{
    if(FMSTR_CAN_TEST_RXFLG())
    {
        /* process the CAN frame */ 
        FMSTR_RxCan();

        /* CAN frame handled, release the flag */
        FMSTR_CAN_CLEAR_RXFLG();

#if FMSTR_LONG_INTR
        /* handle completed frame now? (may be we're in the interrupt) */
        if(pcm_wFlags.flg.bRxFrameReady)
            FMSTR_RxDone();
#endif
    }
#if FMSTR_DEBUG_TX
    /* time to send another test frame? */
    else if(pcm_bDebugTx && pcm_nDebugTxPollCount == 0)
    {
        /* yes, start sending it now */
        if(FMSTR_SendTestFrame(pcm_pCommBuffer))
        {
            /* measure how long it takes to transmit it */
            pcm_nDebugTxPollCount = -1;
        }
    }
#endif
}

/**************************************************************************//*!
*
* @brief    Handle CAN communication
*
* @note This function can be called either from CAN ISRs or from the polling routine
*
******************************************************************************/

void FMSTR_ProcessCanTx(void)
{
#if (FMSTR_USE_MSCAN || FMSTR_USE_MCAN)
    /* any TX buffer available? */
    if(FMSTR_CAN_TEST_TXFLG())
    {
#if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
        /* send one CAN frame (fills buffer and clears its TXFLG */
        if(!FMSTR_TxCan())
        {
            /* no more frames, disable TX Interrupt */
            FMSTR_CAN_DTXI();
        }
#else
        /* send if you have anything to be sent */
        FMSTR_TxCan();
#endif
    }
#elif (FMSTR_USE_FLEXCAN) || (FMSTR_USE_FLEXCAN32)

    /* is TX buffer ready for next packet? */
    if((FMSTR_CAN_GET_MBSTATUS()) == FMSTR_FCANMB_CTXREADY)
    {
#if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
        /* send one CAN frame (fills buffer and clears its TXFLG */
        if(!FMSTR_TxCan())
        {
            /* no more frames, disable TX Interrupt */
            FMSTR_CAN_DTXI();
        }
#else
        /* send if you have anything to be sent */
        FMSTR_TxCan();
#endif
    }
#endif
}

/*******************************************************************************
*
* @brief    API: Main "Polling" call from the application main loop
*
*******************************************************************************/

void FMSTR_Poll(void)
{ 
    /* handle the physical CAN module */    
#if FMSTR_POLL_DRIVEN
    FMSTR_ProcessCanRx();
    FMSTR_ProcessCanTx();
#endif

#if FMSTR_POLL_DRIVEN || FMSTR_SHORT_INTR

    /* except in the LONG_INTR mode, the frame gets handled here */
    if(pcm_wFlags.flg.bRxFrameReady)
        FMSTR_RxDone();
#endif
    
#if FMSTR_DEBUG_TX
    /* down-counting the polls for heuristic time measurement */
    if(pcm_nDebugTxPollCount != 0 && pcm_nDebugTxPollCount > FMSTR_DEBUG_TX_POLLCNT_MIN)
        pcm_nDebugTxPollCount--;
#endif
}

#endif /* (FMSTR_USE_CAN) && (!(FMSTR_DISABLE)) */
