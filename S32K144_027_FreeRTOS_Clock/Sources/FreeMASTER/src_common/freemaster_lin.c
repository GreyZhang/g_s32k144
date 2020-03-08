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
* @brief  FreeMASTER LIN communication routines
*
*******************************************************************************/

#include "freemaster.h"
#include "freemaster_private.h"
#include "freemaster_protocol.h"

#if (FMSTR_USE_LINTL) && (!(FMSTR_DISABLE))

/* keep SID and RSID in variable, so it can be changed dynamically if needed */
static FMSTR_BCHR pcm_linSID = FMSTR_LINTL_SID; 
static FMSTR_BCHR pcm_linRSID = FMSTR_LINTL_RSID; 

/* note that this array declaration works for empty macro, single value, 
   and comma-separated list too. Just skip the [0] element when processing. */ 
static FMSTR_BCHR pcm_linCmdIDs[] = { 0, FMSTR_LINTL_CMDIDS }; 
static FMSTR_BCHR pcm_linRspIDs[] = { 0, FMSTR_LINTL_RSPIDS }; 

/* number of IDs including the leading dummy [0] element. */ 
static FMSTR_SIZE pcm_linCmdIDCount = sizeof(pcm_linCmdIDs)/sizeof(pcm_linCmdIDs[0]); 
static FMSTR_SIZE pcm_linRspIDCount = sizeof(pcm_linRspIDs)/sizeof(pcm_linRspIDs[0]); 

/* FreeMASTER communication buffer (in/out) plus SID/RSID, the worst-case (8)
   CMD/RSP ID count, and STS and LEN bytes */
static FMSTR_BCHR pcm_pCommBuffer[FMSTR_COMM_BUFFER_SIZE+1+8+3];    

/***********************************
*  local function prototypes
***********************************/

static FMSTR_BOOL FMSTR_Process(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize, FMSTR_BOOL bCheckOnly);

/**************************************************************************//*!
*
* @brief    LIN-TL communication initialization
*
******************************************************************************/

FMSTR_BOOL FMSTR_InitLinTL(void)
{
    static const FMSTR_BCHR initCmdIDs[] = { 0, FMSTR_LINTL_CMDIDS }; 
    static const FMSTR_BCHR initRspIDs[] = { 0, FMSTR_LINTL_RSPIDS };
     
    FMSTR_SIZE i;
    
    /* re-initialize SID/RSID */
    pcm_linSID = FMSTR_LINTL_SID; 
    pcm_linRSID = FMSTR_LINTL_RSID; 

    /* re-initialize list of CMD IDs (maximum 8 IDs are supported) */
    pcm_linCmdIDCount = sizeof(pcm_linCmdIDs)/sizeof(pcm_linCmdIDs[0]);
    if(pcm_linCmdIDCount > 8)
        pcm_linCmdIDCount = 8;
    for(i=1; i<pcm_linCmdIDCount; i++)
        pcm_linCmdIDs[i] = initCmdIDs[i];
        
    /* re-initialize list of RSP IDs */
    pcm_linRspIDCount = sizeof(pcm_linRspIDs)/sizeof(pcm_linRspIDs[0]);
    if(pcm_linRspIDCount > 8)
        pcm_linRspIDCount = 8;
    for(i=1; i<pcm_linRspIDCount; i++)
        pcm_linRspIDs[i] = initRspIDs[i];

    /* that is all, we assume the LIN and LIN-TL layers are already initialized */
    return FMSTR_TRUE;
}

/*******************************************************************************
*
* @brief    LIN TL Frame processing, combines functionality of both API calls below 
*
*******************************************************************************/

static FMSTR_BOOL FMSTR_Process(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize, FMSTR_BOOL bCheckOnly)
{
    FMSTR_BPTR pSrc = (FMSTR_BPTR) nFrameAddr;
    FMSTR_U8 nByte;
    FMSTR_SIZE i, len;
    FMSTR_BCHR csum = 0;
    FMSTR_BPTR pDest;
    FMSTR_BPTR pCmd;
    
    /* SID should be the first */
    pSrc = FMSTR_ValueFromBuffer8(&nByte, pSrc);
    if(!nFrameSize-- || nByte != pcm_linSID)
        return FMSTR_FALSE;
    
    /* compare the CMD IDs */
    for(i=1; i<pcm_linCmdIDCount; i++)
    {
        pSrc = FMSTR_ValueFromBuffer8(&nByte, pSrc);
        if(!nFrameSize-- || nByte != pcm_linCmdIDs[i])
            return FMSTR_FALSE;
    }

    /* IDs are matching, it should be the FreeMASTER frame */
    if(bCheckOnly)
        return FMSTR_TRUE;
    
    /* start preparing the answer frame */
    pDest = (FMSTR_BPTR) pcm_pCommBuffer;

    /* put RSID */    
    pDest = FMSTR_ValueToBuffer8(pDest, pcm_linRSID);    

    /* put the RSP IDs */
    for(i=1; i<pcm_linRspIDCount; i++)
        pDest = FMSTR_ValueToBuffer8(pDest, pcm_linRspIDs[i]);

    /* remember where does the FreeMASTER command start */
    pCmd = pDest;
    
    /* the FreeMASTER frame command, first for checksum calculation */
    if(!nFrameSize--)
        return FMSTR_FALSE;
    pSrc = FMSTR_ValueFromBuffer8(&nByte, pSrc);
    pDest = FMSTR_ValueToBuffer8(pDest, nByte);
    csum = nByte;
    
    if(nByte >= FMSTR_FASTCMD)
    {
        /* length encoded in the fast command code */
        len = (FMSTR_SIZE8) ((((FMSTR_SIZE8)nByte) & FMSTR_FASTCMD_DATALEN_MASK) >> FMSTR_FASTCMD_DATALEN_SHIFT);
    }
    else
    {
        /* length byte follows the cmd */
        if(!nFrameSize--)
            return FMSTR_FALSE;
        pSrc = FMSTR_ValueFromBuffer8(&nByte, pSrc);
        pDest = FMSTR_ValueToBuffer8(pDest, nByte);
        len = nByte;
        csum += nByte;
    }

    /* need at least len bytes and one checksum byte */
    if(nFrameSize < (len+1))
        return FMSTR_FALSE;

    /* copy the frame out from user buffer while calculating checksum */
    while(len-- > 0)
    {
        pSrc = FMSTR_ValueFromBuffer8(&nByte, pSrc);
        pDest = FMSTR_ValueToBuffer8(pDest, nByte);
        csum += nByte;
    }

    /* check sum must be 0 */
    pSrc = FMSTR_ValueFromBuffer8(&nByte, pSrc);
    if((csum+nByte) & 0xff != 0)
        return FMSTR_FALSE;

    /* process the frame (this calls the SendResponse for valid frames) */
    if(!FMSTR_ProtocolDecoder(pCmd))
        return FMSTR_FALSE;

    return FMSTR_TRUE;    
}

/* callback from FMSTR_ProtocolDecoder to transmit the response */
 
void FMSTR_SendResponse(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nLength)
{
    /* this is prototype of standard LIN API function to trasmit LIN TL frame */
    void ld_send_message(unsigned short length, const unsigned char* data);
    
    /* we can safely assume the pMessageIO is the same value as pCmd passed
       above to the ProtocolDecoder function. This means the outgoing LIN TL
       frame is now ready to be sent */
    FMSTR_BCHR c, csum = 0;
    FMSTR_SIZE len;
    
    /* calculate the FreeMASTER checksum */
    while(nLength--)
    {
        pMessageIO = FMSTR_ValueFromBuffer8(&c, pMessageIO);
        csum += c;
    }

    /* put checksum to buffer */
    pMessageIO = FMSTR_ValueToBuffer8(pMessageIO, (0x100-csum)&0xff);
    
    /* length is the distance from the final point to the very beginning (RSID) */
    len = (pMessageIO - pcm_pCommBuffer);
    
    /* this is a workaround for issue in FSL LIN driver which sends the 
       last data byte corrupted as 0xff. Simply send one byte more, it 
       will be gracefully ignored by the Host side. */ 
    len++;
    
    /* send it now */
    ld_send_message(len, pcm_pCommBuffer);
}

/*******************************************************************************
*
* @brief    API: Determine if given buffer contains a valid FreeMASTER command.
*           
* User is responsible to call ld_receive_message() first to receive the LIN TL 
* message and pass it to this API function. 
* 
* @return This function returns TRUE if the frame SID and the IDs which follow
*         the SID match the values configured for FreeMASTER processing. 
*         When TRUE is returned, the user should call FMSTR_ProcessLinTLFrame
*         to process the FreeMASTER command encapsulated in the frame. 
*
*******************************************************************************/

FMSTR_BOOL FMSTR_IsLinTLFrame(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize)
{
    return FMSTR_Process(nFrameAddr, nFrameSize, FMSTR_TRUE);
}

/*******************************************************************************
*
* @brief    API: Process the FreeMASTER command encapsulated in LIN TL frame.
*           
* User is responsible to call ld_receive_message() first to receive the LIN TL 
* message and pass it to this API function for processing. 
* 
* @return This function returns TRUE if valid FreeMASTER command was processed and 
*         if a response was transmitted using the ld_send_message() LIN API call.
*
*******************************************************************************/

FMSTR_BOOL FMSTR_ProcessLinTLFrame(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize)
{
    return FMSTR_Process(nFrameAddr, nFrameSize, FMSTR_FALSE);
}

/*******************************************************************************
*
* @brief    API: Main "Polling" call from the application main loop
*
*******************************************************************************/

void FMSTR_Poll(void)
{
    /* Tothing to do for LIN because all communication takes place outside
       of the FreeMASTER driver. */   
}

#else

FMSTR_BOOL FMSTR_IsLinTLFrame(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize)
{
    FMSTR_UNUSED(nFrameAddr);
    FMSTR_UNUSED(nFrameSize);
    return FMSTR_FALSE;
}

FMSTR_BOOL FMSTR_ProcessLinTLFrame(FMSTR_ADDR nFrameAddr, FMSTR_SIZE nFrameSize)
{
    FMSTR_UNUSED(nFrameAddr);
    FMSTR_UNUSED(nFrameSize);
    return FMSTR_FALSE;
}

#endif /* (FMSTR_USE_LINTL) && (!(FMSTR_DISABLE)) */
