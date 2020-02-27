/*******************************************************************************
*
* Copyright 2004-2013 NXP Semiconductor, Inc.
*
* This software is owned or controlled by NXP Semiconductor.
* Use of this software is governed by the NXP FreeMASTER License
* distributed with this Material.
* See the LICENSE file distributed for more details.
* 
****************************************************************************//*!
*
* @brief  FreeMASTER SFIO (Matlab/Simulink interface) encapsulation handler
*
* This file contains the SFIO communication handler and enables the SFIO tool
* to run over FreeMASTER. For more information, see NXP SFIO documentation.
* 
*******************************************************************************/

#include "freemaster.h"
#include "freemaster_private.h"
#include "freemaster_protocol.h"

#if (FMSTR_USE_SFIO) && (!(FMSTR_DISABLE))

/* the sfiolib needs to be added to the project */
#include "sfiolib.h"

/***********************************
*  local variables 
***********************************/

static FMSTR_U8 pcm_nSfioRespLen;

/* recorder runtime flags */
static volatile union 
{
    FMSTR_FLAGS all;
    
    struct 
    {
        unsigned bEvenRun : 1;     /* last command executed was the even one */
        unsigned bLastOK : 1;      /* last command executed properly */
    } flg;
    
} pcm_wSfioFlags;

/**************************************************************************//*!
*
* @brief  Initialization of SFIO communication layer
*
******************************************************************************/

void FMSTR_InitSfio(void)
{
    pcm_wSfioFlags.all = 0;
}

/**************************************************************************//*!
*
* @brief  Handling SFIOFRAME (even and odd) commands
*
* @param  pMessageIO - original command (in) and response buffer (out) 
*
* @note  This function handles the SFIO (Matlab/Simulink Interface) command
*        encapsulated into FreeMASTER protocol. It emulates the SFIO serial
*        char-by-char communication.
*
******************************************************************************/

FMSTR_BPTR FMSTR_SfioFrame(FMSTR_BPTR pMessageIO)
{
    FMSTR_BPTR pResponse = pMessageIO;
    FMSTR_U8 i, nFrameLen, nByte;
    SFIO_U16 nRet = 0;

    /* get command and remember if it was even/odd */
    pMessageIO = FMSTR_ValueFromBuffer8(&nByte, pMessageIO);
    pcm_wSfioFlags.flg.bEvenRun = (nByte & 1) ? 0 : 1;
    pcm_wSfioFlags.flg.bLastOK = 0;
    
    /* get data length */
    pMessageIO = FMSTR_ValueFromBuffer8(&nFrameLen, pMessageIO);

    /* feed the SFIO engine byte-by-byte */
    for(i=0; nRet == 0 && i<nFrameLen; i++)
    {
        pMessageIO = FMSTR_ValueFromBuffer8(&nByte, pMessageIO);
        nRet = SFIO_ProcessRecievedChar(nByte);
    }
       
    /* frame not handled or handled prematurely */
    if(!nRet || i < nFrameLen)
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_SFIOERR);
    
    /* how much data to return? */
    if(nRet > FMSTR_COMM_BUFFER_SIZE)
        return FMSTR_ConstToBuffer8(pResponse, FMSTR_STC_RSPBUFFOVF);
    
    /* remember this command had executed properly */
    pcm_nSfioRespLen = (FMSTR_U8) nRet;
    pcm_wSfioFlags.flg.bLastOK = 1;
    
    /* SFIO response to return */
    pResponse = FMSTR_ConstToBuffer8(pResponse, FMSTR_STS_OK | FMSTR_STSF_VARLEN);
    pResponse = FMSTR_ValueToBuffer8(pResponse, pcm_nSfioRespLen);
    return FMSTR_CopyToBuffer(pResponse, (FMSTR_ADDR) SFIO_GetOutputBuffer(), pcm_nSfioRespLen);
}

/**************************************************************************//*!
*
* @brief  Handling SFIOGETRESP (even and odd) commands
*
* @param  pMessageIO - original command (in) and response buffer (out) 
*
* @note  This function handles the retried request for the last SFIO response.
*        PC may request this retry when the last frame execution took too long
*        (e.g. due to breakpoint) but is still finished properly. The original
*        SFIOFRAME command returned timeout, so the PC will use SFIOGETRESP
*        to finish data.
*
*        The even/odd matching is here to have some dgree of robustness for 
*        a case when SFIOFRAME packet gets lost without ever reaching SFIO engine. 
*        Without any checking, the SFIOGETRESP would just blindly return the 
*        pre-last result and would definietelly cause Simulink problems. 
*        Having the check implemented, the PC can determine the even/odd mismatch
*        and may re-send the last SFIOFRAME command.
*
******************************************************************************/
    
FMSTR_BPTR FMSTR_SfioGetResp(FMSTR_BPTR pMessageIO)
{
    FMSTR_U8 nByte;
    
    /* get command and determine if it is even/odd */
    (void)FMSTR_ValueFromBuffer8(&nByte, pMessageIO);

    /* last command must have been finished propely */
    if(!pcm_wSfioFlags.flg.bLastOK)
         return FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STC_SFIOERR);

    /* only respond to "matching" request (even to even, odd to odd) */
    if(nByte & 1)
    {
        if(pcm_wSfioFlags.flg.bEvenRun)
            return FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STC_SFIOUNMATCH);
    }
    else
    {
        if(!pcm_wSfioFlags.flg.bEvenRun)
            return FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STC_SFIOUNMATCH);
    }    
        
    /* SFIO response to return */
    pMessageIO = FMSTR_ConstToBuffer8(pMessageIO, FMSTR_STS_OK | FMSTR_STSF_VARLEN);
    pMessageIO = FMSTR_ValueToBuffer8(pMessageIO, pcm_nSfioRespLen);
    return FMSTR_CopyToBuffer(pMessageIO, (FMSTR_ADDR) SFIO_GetOutputBuffer(), pcm_nSfioRespLen);
}

#else /* FMSTR_USE_SFIO && (!FMSTR_DISABLE) */

/*lint -efile(766, freemaster_protocol.h) include file is not used in this case */

#endif /* FMSTR_USE_SFIO && (!FMSTR_DISABLE) */
