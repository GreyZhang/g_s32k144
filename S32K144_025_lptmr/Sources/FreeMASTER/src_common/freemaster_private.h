/*******************************************************************************
 *
 * Copyright 2004-2016 NXP Semiconductor, Inc.
 *
 * This software is owned or controlled by NXP Semiconductor.
 * Use of this software is governed by the NXP FreeMASTER License
 * distributed with this Material.
 * See the LICENSE file distributed for more details.
 *
 ****************************************************************************
 *
 * @brief  FreeMASTER driver private declarations, used internally by the driver
 *
 *******************************************************************************/

#ifndef __FREEMASTER_PRIVATE_H
#define __FREEMASTER_PRIVATE_H

#ifndef __FREEMASTER_H
#error Please include the freemaster.h master header file before the freemaster_private.h
#endif

#ifdef FMSTR_USE_MQX_IO
#if FMSTR_USE_MQX_IO
/* include MQX headers */
#include <mqx.h>
#include <bsp.h>
#endif
#endif

/******************************************************************************
 * Platform-dependent types, macros and functions
 ******************************************************************************/

#undef FMSTR_PLATFORM

/* platform macro FMSTR_PLATFORM_xxxxxxx should be defined  */
/* as "non-zero" in the "freemaster.h" file */

#if defined(FMSTR_PLATFORM_56F8xx)
#if FMSTR_PLATFORM_56F8xx
#include "freemaster_56F8xx.h"
#define FMSTR_PLATFORM 56F8xx
#else
#undef FMSTR_PLATFORM_56F8xx
#endif
#endif

#if defined(FMSTR_PLATFORM_56F8xxx)
#if FMSTR_PLATFORM_56F8xxx
#include "freemaster_56F8xxx.h"
#define FMSTR_PLATFORM 56F8xxx
#else
#undef FMSTR_PLATFORM_56F8xxx
#endif
#endif

#if defined(FMSTR_PLATFORM_HC12)
#if FMSTR_PLATFORM_HC12
#include "freemaster_HC12.h"
#define FMSTR_PLATFORM HC12
#else
#undef FMSTR_PLATFORM_HC12
#endif
#endif

#if defined(FMSTR_PLATFORM_HC08)
#if FMSTR_PLATFORM_HC08
#include "freemaster_HC08.h"
#define FMSTR_PLATFORM HC08
#else
#undef FMSTR_PLATFORM_HC08
#endif
#endif

#if defined(FMSTR_PLATFORM_MPC55xx)
#if FMSTR_PLATFORM_MPC55xx
#include "freemaster_MPC55xx.h"
#define FMSTR_PLATFORM MPC55xx
#else
#undef FMSTR_PLATFORM_MPC55xx
#endif
#endif

#if defined(FMSTR_PLATFORM_MPC5xx)
#if FMSTR_PLATFORM_MPC5xx
#include "freemaster_MPC5xx.h"
#define FMSTR_PLATFORM MPC5xx
#else
#undef FMSTR_PLATFORM_MPC5xx
#endif
#endif

#if defined(FMSTR_PLATFORM_MCF51xx)
#if FMSTR_PLATFORM_MCF51xx
#include "freemaster_MCF51xx.h"
#define FMSTR_PLATFORM MCF51xx
#else
#undef FMSTR_PLATFORM_MCF51xx
#endif
#endif

#if defined(FMSTR_PLATFORM_MCF52xx)
#if FMSTR_PLATFORM_MCF52xx
#include "freemaster_MCF52xx.h"
#define FMSTR_PLATFORM MCF52xx
#else
#undef FMSTR_PLATFORM_MCF52xx
#endif
#endif

#if defined(FMSTR_PLATFORM_MPC56xx)
#if FMSTR_PLATFORM_MPC56xx
#include "freemaster_MPC56xx.h"
#define FMSTR_PLATFORM MPC56xx
#else
#undef FMSTR_PLATFORM_MPC56xx
#endif
#endif

#if defined(FMSTR_PLATFORM_MQX)
#if FMSTR_PLATFORM_MQX
#include "freemaster_MQX.h"
#define FMSTR_PLATFORM MQX
#else
#undef FMSTR_PLATFORM_MQX
#endif
#endif

#if defined(FMSTR_PLATFORM_KXX)
#if FMSTR_PLATFORM_KXX
#include "freemaster_Kxx.h"
#define FMSTR_PLATFORM KXX
#else
#undef FMSTR_PLATFORM_KXX
#endif
#endif

#if defined(FMSTR_PLATFORM_S32xx)
#if FMSTR_PLATFORM_S32xx
#include "freemaster_S32xx.h"
#define FMSTR_PLATFORM S32xx
#else
#undef FMSTR_PLATFORM_S32xx
#endif
#endif

#if defined(FMSTR_PLATFORM_KEAxx)
#if FMSTR_PLATFORM_KEAxx
#include "freemaster_KEAxx.h"
#define FMSTR_PLATFORM KEAxx
#else
#undef FMSTR_PLATFORM_KEAxx
#endif
#endif

#if defined(FMSTR_PLATFORM_MPC57xx)
#if FMSTR_PLATFORM_MPC57xx
#include "freemaster_MPC57xx.h"
#define FMSTR_PLATFORM MPC57xx
#else
#undef FMSTR_PLATFORM_MPC57xx
#endif
#endif

#ifndef FMSTR_PLATFORM
#error Unknown FreeMASTER driver platform
#endif

/******************************************************************************
 * Boolean values
 ******************************************************************************/

#ifndef FMSTR_TRUE
#define FMSTR_TRUE (1U)
#endif

#ifndef FMSTR_FALSE
#define FMSTR_FALSE (0U)
#endif


/******************************************************************************
 * inline functions
 ******************************************************************************/

/* we do not assume the inline is always supported by compiler
   rather each platform header defines its FMSTR_INLINE */
#ifndef FMSTR_INLINE
#define FMSTR_INLINE static
#endif

/* building macro-based inline code */
#define FMSTR_MACROCODE_BEGIN()     do{
#define FMSTR_MACROCODE_END()       }while(0)

/******************************************************************************
 * Global non-API functions (used internally in FreeMASTER driver)
 ******************************************************************************/

FMSTR_BOOL FMSTR_InitSerial(void);
void FMSTR_SendResponse(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nLength);
void FMSTR_ProcessSCI(void);
void FMSTR_ProcessJTAG(void);
void FMSTR_ProcessMBED(void);

FMSTR_BOOL FMSTR_Tx(FMSTR_U8 * pTxChar);
FMSTR_BOOL FMSTR_Rx(FMSTR_U8   nRxChar);

FMSTR_BOOL FMSTR_ProtocolDecoder(FMSTR_BPTR pMessageIO);
FMSTR_BOOL FMSTR_SendTestFrame(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetBoardInfo(FMSTR_BPTR pMessageIO);

FMSTR_BPTR FMSTR_ReadMem(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_ReadVar(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_WriteMem(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_WriteVar(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_WriteVarMask(FMSTR_BPTR pMessageIO, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_WriteMemMask(FMSTR_BPTR pMessageIO);

void FMSTR_InitAppCmds(void);
FMSTR_BPTR FMSTR_StoreAppCmd(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetAppCmdStatus(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetAppCmdRespData(FMSTR_BPTR pMessageIO);

void FMSTR_InitScope(void);
FMSTR_BPTR FMSTR_SetUpScope(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_ReadScope(FMSTR_BPTR pMessageIO);

void FMSTR_InitRec(void);
FMSTR_BPTR FMSTR_SetUpRec(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_StartRec(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_StopRec(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetRecStatus(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetRecBuff(FMSTR_BPTR pMessageIO);
FMSTR_BOOL FMSTR_IsInRecBuffer(FMSTR_ADDR nAddr, FMSTR_SIZE8 nSize);
FMSTR_SIZE_RECBUFF FMSTR_GetRecBuffSize(void);

void FMSTR_InitTsa(void);
FMSTR_BPTR FMSTR_GetTsaInfo(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_GetStringLen(FMSTR_BPTR pMessageIO);
FMSTR_BOOL FMSTR_CheckTsaSpace(FMSTR_ADDR nAddr, FMSTR_SIZE8 nSize, FMSTR_BOOL bWriteAccess);
FMSTR_U16  FMSTR_StrLen(FMSTR_ADDR nAddr);

void FMSTR_InitSfio(void);
FMSTR_BPTR FMSTR_SfioFrame(FMSTR_BPTR pMessageIO);
FMSTR_BPTR FMSTR_SfioGetResp(FMSTR_BPTR pMessageIO);

void FMSTR_InitPipes(void);
FMSTR_BPTR FMSTR_PipeFrame(FMSTR_BPTR pMessageIO);

FMSTR_BOOL FMSTR_InitCan(void);
void FMSTR_SetCanCmdID(FMSTR_U32 canID);
void FMSTR_SetCanRespID(FMSTR_U32 canID);
FMSTR_BOOL FMSTR_TxCan(void);
FMSTR_BOOL FMSTR_RxCan(void);
void FMSTR_ProcessCanRx(void);
void FMSTR_ProcessCanTx(void);

void FMSTR_InitPDBdm(void);

FMSTR_BOOL FMSTR_InitLinTL(void);

/****************************************************************************************
 * Potentially unused variable declaration
 *****************************************************************************************/
#if defined(_lint) || defined(__IAR_SYSTEMS_ICC__) || defined(__ARMCC_VERSION)
#define FMSTR_UNUSED(sym) /*lint -esym(715,sym) -esym(818,sym) -esym(529,sym) -e{960} */
#elif defined(__GNUC__)
#define FMSTR_UNUSED(x) (void)(x)
#else
#define FMSTR_UNUSED(sym) ((sym),0)
#endif


#if (FMSTR_LONG_INTR && (FMSTR_SHORT_INTR || FMSTR_POLL_DRIVEN)) || \
    (FMSTR_SHORT_INTR && (FMSTR_LONG_INTR || FMSTR_POLL_DRIVEN)) || \
    (FMSTR_POLL_DRIVEN && (FMSTR_LONG_INTR || FMSTR_SHORT_INTR)) || \
    !(FMSTR_POLL_DRIVEN || FMSTR_LONG_INTR || FMSTR_SHORT_INTR)
/* mismatch in interrupt modes, only one can be selected */
#error You have to enable exctly one of FMSTR_LONG_INTR or FMSTR_SHORT_INTR or FMSTR_POLL_DRIVEN
#endif

#if FMSTR_SHORT_INTR
#if FMSTR_COMM_RQUEUE_SIZE < 1
#error Error in FMSTR_COMM_RQUEUE_SIZE value.
#endif
#endif

/* count how many interfaces are enabled (only one should be enabled) */
#define FMSTR_COUNT_INTERFACES                                          \
    ( ((FMSTR_DISABLE)?1:0) + ((FMSTR_USE_SCI)?1:0) + ((FMSTR_USE_ESCI)?1:0) + ((FMSTR_USE_LPUART)?1:0) + \
      ((FMSTR_USE_PDBDM)?1:0) + ((FMSTR_USE_JTAG)?1:0) + ((FMSTR_USE_CAN)?1:0) + ((FMSTR_USE_LINTL)?1:0) + \
      ((FMSTR_USE_MQX_IO)?1:0) + ((FMSTR_USE_USB_CDC)?1:0) + ((FMSTR_USE_MBED)?1:0) )

/* only one communication link may be selected */
#if (!(FMSTR_DISABLE)) && FMSTR_COUNT_INTERFACES > 1
#error More than one communication interface selected for FreeMASTER driver.
#endif

/* All Interface options are set to 0 */
#if FMSTR_COUNT_INTERFACES == 0
#error No FreeMASTER communication interface is enabled. Please choose one intercace or set FMSTR_DISABLE option to 1.
#endif

/* CAN is MSCAN or FLEXCAN */
#if FMSTR_USE_CAN

/* can't be a few */
#if ((((FMSTR_USE_MSCAN)?1:0) + ((FMSTR_USE_FLEXCAN)?1:0) + ((FMSTR_USE_FLEXCAN32)?1:0) + ((FMSTR_USE_MCAN)?1:0)) > 1)
#error Can not configure FreeMASTER to use MSCAN, FlexCAN, MCAN together
#endif

/* should be at least one */
#if !(FMSTR_USE_MSCAN) && !(FMSTR_USE_FLEXCAN)  && !(FMSTR_USE_FLEXCAN32) && !(FMSTR_USE_MCAN)
#error Please select if MSCAN, MCAN or FlexCAN will be used
#endif

/* flexcan needs to know the transmit and receive MB number */
#if (FMSTR_USE_FLEXCAN) || (FMSTR_USE_FLEXCAN32)
#if (FMSTR_FLEXCAN_TXMB) == (FMSTR_FLEXCAN_RXMB)
#warning FCAN RX and FCAN TX are using same Message Buffer. FreeMASTER CAN driver doesnt support this configuration. Please change number of Message Buffer in FMSTR_FLEXCAN_TXMB or FMSTR_FLEXCAN_RXMB macros.
#endif
#endif

#else

/* Cannot define FMSTR_USE_CAN by user */
#if (FMSTR_USE_FLEXCAN) || (FMSTR_USE_MSCAN) || (FMSTR_USE_FLEXCAN32) || (FMSTR_USE_MCAN)
#error Please enable FMSTR_USE_CAN option when configuring FlexCan or msCan interface
#endif

#endif /* FMSTR_USE_CAN */

/* MQX IO communication interface warnings and errors */
#if FMSTR_USE_MQX_IO

#if !(defined(FMSTR_PLATFORM_MQX) && (FMSTR_PLATFORM_MQX))
#error MQX IO communication interface is supported only in FMSTR_PLATFORM_MQX, Please choose another communication interface.
#endif

/* define FreeMASTER IO Channel */
#ifndef FMSTR_MQX_IO_CHANNEL
#warning MQX IO Channel is not defined by FMSTR_MQX_IO_CHANNEL macro, FreeMASTER use "ttya:" IO Channel
#define FMSTR_MQX_IO_CHANNEL "ttya:"
#endif

/* configure the FreeMASTER communication channel to non blocking mode */
#ifndef FMSTR_MQX_IO_BLOCKING
#warning FMSTR_MQX_IO_BLOCKING macro is not defined, the MQX IO Channel will be open in non blocking mode
#define FMSTR_MQX_IO_BLOCKING 0
#endif

/* MQX open port parameters */
#if FMSTR_MQX_IO_BLOCKING
#define FMSTR_MQX_IO_PARAMETER 0
#else
#define FMSTR_MQX_IO_PARAMETER IO_SERIAL_NON_BLOCKING
#endif

/* MQX doesn't supports Long interrupt and Short interrupt */
#if FMSTR_LONG_INTR
#warning "MQX IO interface doesn't supports the Long Interrupt routines"
#elif FMSTR_SHORT_INTR
#warning "MQX IO interface doesn't supports the normal Short Interrupt routines, please open your MQX communication interface in Interrupt mode"
#endif
#endif

#if defined(FMSTR_PLATFORM_MQX)
#if FMSTR_PLATFORM_MQX

/* MQX communication interfaces require MQX header files to be included. */
#ifndef __MQX__
#error Please include the MQX header file before the freemaster.h
#endif

#if (FMSTR_USE_SCI) || (FMSTR_USE_ESCI) || (FMSTR_USE_LPUART)
#error "MQX target driver doesn't supports the SCI module as communication interface"
#elif FMSTR_USE_CAN
#error "MQX target driver doesn't supports the CAN module as communication interface"
#elif FMSTR_USE_JTAG
#error "MQX target driver doesn't supports the JTAG module as communication interface"
#elif FMSTR_USE_USB_CDC
#error "MQX target driver doesn't supports the USB CDC as communication interface"
#endif
#endif
#endif

#if (FMSTR_USE_SCI) || (FMSTR_USE_ESCI) || (FMSTR_USE_LPUART) || (FMSTR_USE_JTAG) || (FMSTR_USE_USB_CDC) || (FMSTR_USE_MQX_IO) || (FMSTR_USE_MBED)
#ifndef FMSTR_USE_SERIAL
#define FMSTR_USE_SERIAL 1
#else
#if FMSTR_USE_SERIAL == 0
#error "FMSTR_USE_SERIAL macro cannot be configured by user, FreeMASTER serial driver functionality is not garanted."
#endif
#endif
#else
#ifndef FMSTR_USE_SERIAL
#define FMSTR_USE_SERIAL 0
#endif
#endif

#if FMSTR_USE_USB_CDC
/* by default, the new USB stack from Kinetis SDK is used, set to 1 for to use legacy NXP 'Medical USB' stack */
#ifndef FMSTR_USB_LEGACY_STACK
#define FMSTR_USB_LEGACY_STACK 0
#endif

/* USB CDC driver doesn't support the POLL mode */
#if FMSTR_POLL_DRIVEN
#error "FreeMASTER USB CDC driver doesn't supports the POLL mode"
#endif

/* USB interface selection (KSDK USB stack only) */
#ifndef FMSTR_USB_CONTROLLER_ID
/* low-speed controller by default */
#ifndef FMSTR_USB_HIGH_SPEED
#define FMSTR_USB_HIGH_SPEED 0
#endif

/* pick one of USB_CONTROLLER_ identifiers as defined by KSDK USB driver */
#if FMSTR_USB_HIGH_SPEED
#define FMSTR_USB_CONTROLLER_ID USB_CONTROLLER_EHCI_0
#else
#define FMSTR_USB_CONTROLLER_ID USB_CONTROLLER_KHCI_0
#endif
#endif
#endif

#if FMSTR_USE_PDBDM
/* Packet Driven BDM communication does not support any interrupt mode */
#if (FMSTR_SHORT_INTR) || (FMSTR_LONG_INTR)
#warning "FreeMASTER Packet Driven BDM driver doesn't support any Interrupt mode. Call the FMSTR_Poll() function in the Timer ISR routine instead."
#endif
#endif

/* check scope settings */
#if FMSTR_USE_SCOPE
#if FMSTR_MAX_SCOPE_VARS > 32 || FMSTR_MAX_SCOPE_VARS < 2
#error Error in FMSTR_MAX_SCOPE_VARS value. Use a value in range 2..32
#endif
#endif

/* check recorder settings */
#if (FMSTR_USE_RECORDER) || (FMSTR_USE_FASTREC)
#if FMSTR_MAX_REC_VARS > 32 || FMSTR_MAX_REC_VARS < 2
#error Error in FMSTR_MAX_REC_VARS value. Use a value in range 2..32
#endif

#if !FMSTR_USE_READMEM
#error Recorder needs the FMSTR_USE_READMEM feature
#endif
#endif

/* fast recorder requires its own allocation of recorder buffer */
#if FMSTR_USE_FASTREC
#if FMSTR_REC_OWNBUFF
#error Fast recorder requires its own buffer allocation
#endif
#endif

#if FMSTR_USE_TSA
#if !FMSTR_USE_READMEM
#error TSA needs the FMSTR_USE_READMEM feature
#endif
#endif

/* check SFIO settings */
#if FMSTR_USE_SFIO

/* The SFIO configuration files (sfio.h and optionally also the sfio_cfg.h) exist
   in project to define SFIO parameters. */
#include "sfio.h"

#ifndef SFIO_MAX_INPUT_DATA_LENGTH
#error  SFIO_MAX_INPUT_DATA_LENGTH was not defined in sfio_cfg.h
#endif
#ifndef SFIO_MAX_OUTPUT_DATA_LENGTH
#error  SFIO_MAX_OUTPUT_DATA_LENGTH was not defined in sfio_cfg.h
#endif

#endif

#if FMSTR_USE_PIPES

#ifdef FMSTR_PIPES_EXPERIMENTAL
#warning The "pipes" feature is now in experimental code phase. Not yet tested on this platform.
#endif

/* must enable printf for vararg printf */
#if !(FMSTR_USE_PIPE_PRINTF) && (FMSTR_USE_PIPE_PRINTF_VARG)
#error You must enable pipe printf for vararg printf (see FMSTR_USE_PIPE_PRINTF)
#endif

/* at least one */
#if FMSTR_MAX_PIPES_COUNT < 1
#warning No sense to allocate zero-count pipes. Disabling pipes.
#undef  FMSTR_USE_PIPES
#define FMSTR_USE_PIPES 0
#endif

/* printf buffer should accept one integer printed */
#if FMSTR_PIPES_PRINTF_BUFF_SIZE < 8
#error Pipe printf buffer should be at least 8 (see FMSTR_PIPES_PRINTF_BUFF_SIZE)
#endif
#if FMSTR_PIPES_PRINTF_BUFF_SIZE > 255
#error Pipe printf buffer should not exceed 255 (see FMSTR_PIPES_PRINTF_BUFF_SIZE)
#endif

#endif

#if FMSTR_DEBUG_TX
/* When communication debugging mode is requested, this global variable is used to
   turn the debugging off once a valid connection is detected */
    extern FMSTR_BOOL pcm_bDebugTx;
#endif

/* check what kind of board info is sent */
#if FMSTR_USE_BRIEFINFO
#if FMSTR_USE_RECORDER
#warning The full information structure must be used when recorder is to be used
#undef  FMSTR_USE_BRIEFINFO
#define FMSTR_USE_BRIEFINFO 1
#endif
#endif

/* automatic buffer size by default */
#ifndef FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE 0
#endif

/* check minimal buffer size required for all enabled features */
#if FMSTR_COMM_BUFFER_SIZE

/* basic commands (getinfobrief, write/read memory etc.) */
#if FMSTR_USE_BRIEFINFO && (FMSTR_COMM_BUFFER_SIZE < 11)
#error FMSTR_COMM_BUFFER_SIZE set too small for basic FreeMASTER commands (11 bytes)
#endif

/* full info required */
#if !(FMSTR_USE_BRIEFINFO) && (FMSTR_COMM_BUFFER_SIZE < (35+1))
#error FMSTR_COMM_BUFFER_SIZE set too small for GETINFO command (size 35+1)
#endif

/* application commands */
#if FMSTR_USE_APPCMD && (FMSTR_COMM_BUFFER_SIZE < ((FMSTR_APPCMD_BUFF_SIZE)+1+2))
#error FMSTR_COMM_BUFFER_SIZE set too small for SENDAPPCMD command (size FMSTR_APPCMD_BUFF_SIZE+1+2)
#endif

/* configuring scope (EX) */
#if FMSTR_USE_SCOPE && (FMSTR_COMM_BUFFER_SIZE < (((FMSTR_MAX_SCOPE_VARS)*5)+1+2))
#error FMSTR_COMM_BUFFER_SIZE set too small for SETUPSCOPEEX command (size FMSTR_MAX_SCOPE_VARS*5+1+2)
#endif

/* configuring recorder (EX) */
#if FMSTR_USE_RECORDER && (FMSTR_COMM_BUFFER_SIZE < (((FMSTR_MAX_REC_VARS)*5)+18+2))
#error FMSTR_COMM_BUFFER_SIZE set too small for SETUPRECEX command (size FMSTR_MAX_REC_VARS*5+18+2)
#endif

    /* SFIO encapsulation */
#if FMSTR_USE_SFIO
#if (FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_INPUT_DATA_LENGTH)+1+2)) ||  \
    (FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1+2))
#error FMSTR_COMM_BUFFER_SIZE set too small for SFIO encapsulation (see SFIO_MAX_xxx_DATA_LENGTH)
#endif
#endif

#endif

#endif /* __FREEMASTER_PRIVATE_H */
