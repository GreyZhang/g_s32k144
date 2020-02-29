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
* @brief  FreeMASTER BDM communication routines
*
*******************************************************************************/

#include "freemaster.h"
#include "freemaster_private.h"
#include "freemaster_protocol.h"

#if FMSTR_USE_PDBDM
typedef struct
{
    FMSTR_U16 buffBorder1;                 // Border code of Packet driven BDM communication buffer
    FMSTR_U8 buffSize;                     // Size of Packet driven BDM communication buffer
    FMSTR_U8 bdmState;                     // State of the Packet driven BDM communication
    FMSTR_BCHR commBuffer[FMSTR_COMM_BUFFER_SIZE+1]; //FreeMASTER Packet driven BDM communication buffer (in/out) plus the STS
    FMSTR_U16 buffBorder2;                 // Border code of Packet driven BDM communication buffer
}FMSTR_PDBDM_COMBUFF;

static FMSTR_PDBDM_COMBUFF pcm_bdm_pCommBuffer; //Packet driven communication buffer

/**************************************************************************//*!
*
* @brief    Packet Driven BDM communication initialization
*
******************************************************************************/

void FMSTR_InitPDBdm(void)
{   
    /* Initialize buffer border and size in communication buffer */
    pcm_bdm_pCommBuffer.buffBorder1 = FMSTR_PDBDM_BUFFBORDER_CODE1;
    pcm_bdm_pCommBuffer.buffSize = FMSTR_COMM_BUFFER_SIZE + 1;
    pcm_bdm_pCommBuffer.buffBorder2 = FMSTR_PDBDM_BUFFBORDER_CODE2;

    /* initialize state variables */
    pcm_bdm_pCommBuffer.bdmState = FMSTR_PDBDM_IDLE;
}

/**************************************************************************//*!
*
* @brief    Finalize transmit buffer before transmitting 
*
* @param    nLength - response length (1 for status + data length)
*
*
* This Function takes the data already prepared in the transmit buffer 
* (including the status byte). Change state of BDM driver.
*
******************************************************************************/

void FMSTR_SendResponse(FMSTR_BPTR pResponse, FMSTR_SIZE8 nLength)
{
    FMSTR_UNUSED(pResponse);
    FMSTR_UNUSED(nLength);
    /* Frame is ready to send */
    pcm_bdm_pCommBuffer.bdmState = FMSTR_PDBDM_FRAME_TO_SEND;
}

/*******************************************************************************
*
* @brief    API: Main "Polling" call from the application main loop
*
*******************************************************************************/

void FMSTR_Poll(void)
{ 
    if (pcm_bdm_pCommBuffer.bdmState == FMSTR_PDBDM_RECEIVED_FRAME)
    {
        /* Packet is received */ 
        pcm_bdm_pCommBuffer.bdmState = FMSTR_PDBDM_DECODING_FRAME;
        /* Decode packet and create response */
        FMSTR_ProtocolDecoder(pcm_bdm_pCommBuffer.commBuffer);
    }
}
#else /* FMSTR_USE_PDBDM */

/*lint -efile(766, freemaster_protocol.h) include file is not used in this case */

#endif /* FMSTR_USE_PDBDM */

