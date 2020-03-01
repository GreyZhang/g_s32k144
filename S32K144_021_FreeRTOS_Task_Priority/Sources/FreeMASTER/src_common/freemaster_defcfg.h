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
 * @brief  FreeMASTER driver default configuration
 *
 *******************************************************************************/

#ifndef __FREEMASTER_DEFCFG_H
#define __FREEMASTER_DEFCFG_H

/* Read user configuration */
#include "freemaster_cfg.h"

/* Define global version macro */
#define FMSTR_VERSION 0x00020000 /* 2.0.0 */

/******************************************************************************
 * Configuration check
 ******************************************************************************/

/* polling mode as default when nothing selected */
#if !defined(FMSTR_POLL_DRIVEN) && !defined(FMSTR_LONG_INTR) && !defined(FMSTR_SHORT_INTR)
#define FMSTR_LONG_INTR   0
#define FMSTR_SHORT_INTR  0
#define FMSTR_POLL_DRIVEN 1
#endif

/* otherwise, "undefined" means false for all three options */
#ifndef FMSTR_POLL_DRIVEN
#define FMSTR_POLL_DRIVEN 0
#endif
#ifndef FMSTR_LONG_INTR
#define FMSTR_LONG_INTR 0
#endif
#ifndef FMSTR_SHORT_INTR
#define FMSTR_SHORT_INTR 0
#endif


#if FMSTR_SHORT_INTR
/* default short-interrupt FIFO size */
#ifndef FMSTR_COMM_RQUEUE_SIZE
#define FMSTR_COMM_RQUEUE_SIZE 32
#endif

#if !FMSTR_COMM_RQUEUE_SIZE
#undef  FMSTR_COMM_RQUEUE_SIZE
#define FMSTR_COMM_RQUEUE_SIZE 32
#endif
#endif

/* default selection of communication interface
   one of these is typically enabled in freemaster_cfg.h */
#ifndef FMSTR_USE_ESCI
#define FMSTR_USE_ESCI 0   /* ESCI/UART communication module */
#endif
#ifndef FMSTR_USE_LPUART
#define FMSTR_USE_LPUART 0 /* LPUART communication module */
#endif
#ifndef FMSTR_USE_JTAG
#define FMSTR_USE_JTAG 0  /* DSC JTAG/RTDX interface */
#endif
#ifndef FMSTR_USE_MSCAN
#define FMSTR_USE_MSCAN 0  /* CAN communication (msCAN) */
#endif
#ifndef FMSTR_USE_FLEXCAN
#define FMSTR_USE_FLEXCAN 0  /* CAN communication (FlexCAN) */
#endif
#ifndef FMSTR_USE_MCAN
#define FMSTR_USE_MCAN 0 /* CAN communication (mCAN)*/
#endif
#ifndef FMSTR_USE_FLEXCAN32
#define FMSTR_USE_FLEXCAN32 0  /* CAN communication (FlexCAN 32bit) */
#endif
#ifndef FMSTR_USE_CAN
#if ((FMSTR_USE_MSCAN) || (FMSTR_USE_FLEXCAN)  || (FMSTR_USE_FLEXCAN32) || (FMSTR_USE_MCAN))
#define FMSTR_USE_CAN 1
#else
#define FMSTR_USE_CAN 0
#endif
#endif
#ifndef FMSTR_USE_LINTL
#define FMSTR_USE_LINTL 0  /* communication over LIN Transport Layer API */
#endif
#ifndef FMSTR_USE_MQX_IO
#define FMSTR_USE_MQX_IO 0  /* communication over MQX serial driver */
#endif
#ifndef FMSTR_USE_USB_CDC
#define FMSTR_USE_USB_CDC 0 /* communication over NXP USB CDC class driver */
#endif
#ifndef FMSTR_USE_PDBDM
#define FMSTR_USE_PDBDM 0  /* communication over Packet-Driven BDM protocol */
#endif
#ifndef FMSTR_USE_MBED
#define FMSTR_USE_MBED 0  /* communication over mbed serial_t class */
#endif
#ifndef FMSTR_DISABLE
#define FMSTR_DISABLE 0   /* FreeMASTER driver de-activated */
#endif

/* SCI used by default if BASE address is provided */
#ifndef FMSTR_USE_SCI
#if defined(FMSTR_SCI_BASE) && !FMSTR_USE_ESCI && !FMSTR_USE_LPUART && !FMSTR_USE_JTAG && !FMSTR_USE_CAN && !FMSTR_USE_MQX_IO && !FMSTR_USE_USB_CDC && !FMSTR_DISABLE && !FMSTR_USE_PDBDM && !FMSTR_USE_MBED
#define FMSTR_USE_SCI 1
#else
#define FMSTR_USE_SCI 0
#endif
#endif

/* SCI does not have a transmission double buffer (kind of queue) */
/* (if not specified differently in platform-dependent header file) */
#if (FMSTR_USE_SCI) || (FMSTR_USE_ESCI)|| (FMSTR_USE_LPUART)
#ifndef FMSTR_SCI_HAS_TXQUEUE
#define FMSTR_SCI_HAS_TXQUEUE 0
#endif

/* SCI base address must be assigned in runtime */
#ifndef FMSTR_SCI_BASE
#undef FMSTR_SCI_BASE_DYNAMIC
#define FMSTR_SCI_BASE_DYNAMIC 1
#endif
#endif

/* SCI may be also used with dynamically assigned address */
#ifndef FMSTR_SCI_BASE_DYNAMIC
#define FMSTR_SCI_BASE_DYNAMIC 0
#endif

/* Enables or disables clearing SCI status register by write one. Requred by KLxx UART */
#ifndef FMSTR_SCI_STATUS_W1C
#define FMSTR_SCI_STATUS_W1C 1
#endif

/* CAN is MSCAN or FLEXCAN */
#if FMSTR_USE_CAN

/* CAN-related constants */
#ifdef FMSTR_CAN_EXTID
#if FMSTR_CAN_EXTID != 0x80000000U
#error FMSTR_CAN_EXTID must be defined as 0x80000000
#undef FMSTR_CAN_EXTID
#endif
#endif

#ifndef FMSTR_CAN_EXTID
#define FMSTR_CAN_EXTID 0x80000000U
#endif

/* flexcan needs to know the transmit and receive MB number */
#if ((FMSTR_USE_FLEXCAN) || (FMSTR_USE_FLEXCAN32))
/* Flexcan TX message buffer must be defined */
#ifndef FMSTR_FLEXCAN_TXMB
/* #error FlexCAN transmit buffer needs to be specified (use FMSTR_FLEXCAN_TXMB) */
#warning "FlexCAN Message Buffer 0 is used for transmitting messages"
#define FMSTR_FLEXCAN_TXMB 0
#endif
/* Flexcan RX message buffer must be defined */
#ifndef FMSTR_FLEXCAN_RXMB
/* #error FlexCAN receive buffer needs to be specified (use FMSTR_FLEXCAN_RXMB) */
#warning "FlexCAN Message Buffer 1 is used for receiving messages"
#define FMSTR_FLEXCAN_RXMB 1
#endif
#endif

/* MCAN  needs to know offset of the mcan shared memory, offsets of the buffers into shared memory,
 * transmit/receive MB numbers */
#if FMSTR_USE_MCAN
#ifndef FMSTR_MCAN_TXMB
#error "MCAN transmit buffer number must be defined"
#endif
#ifndef FMSTR_MCAN_RXMB
#error "MCAN receive buffer number must be defined"
#endif
#ifndef FMSTR_MCAN_TXMB_OFFSET
#error "MCAN transmit buffers offset into shared memory must be defined"
#endif
#ifndef FMSTR_MCAN_RXMB_OFFSET
#error "MCAN receive buffers offset into shared memory must be defined"
#endif
#ifndef FMSTR_MCAN_SHAREDMEMORY_OFFSET
#error "MCAN shared memory address must be defined"
#endif
#endif

/* incoming (command) CAN message ID */
#ifndef FMSTR_CAN_CMDID
#define FMSTR_CAN_CMDID 0x7aa
#endif

/* command ID can be changed in runtime (before FMSTR_Init) */
#ifndef FMSTR_CAN_CMDID_DYNAMIC
#define FMSTR_CAN_CMDID_DYNAMIC 0  /* disabled by default */
#endif

/* response CAN message ID, may be the same as command ID */
#ifndef FMSTR_CAN_RESPID
#define FMSTR_CAN_RESPID 0x7aa
#endif

/* response ID can be changed in runtime (before FMSTR_Init) */
#ifndef FMSTR_CAN_RESPID_DYNAMIC
#define FMSTR_CAN_RESPID_DYNAMIC 0  /* disabled by default */
#endif

/* CAN base address must be assigned in runtime */
#ifndef FMSTR_CAN_BASE
#undef FMSTR_CAN_BASE_DYNAMIC
#define FMSTR_CAN_BASE_DYNAMIC 1
#endif

#endif

/* CAN may be also used with dynamically assigned address */
#ifndef FMSTR_CAN_BASE_DYNAMIC
#define FMSTR_CAN_BASE_DYNAMIC 0
#endif

/* LIN Transport Layer (uses standard 0x3c/0x3d request/response frames) */
#if FMSTR_USE_LINTL

/* TL SID identifier, standard Read-Data-by-ID service used by default */
#ifndef FMSTR_LINTL_SID
#define FMSTR_LINTL_SID 0x22U
#endif

/* TL RSID identifier, the SID+0x40 is used by default as defined by LIN standard */
#ifndef FMSTR_LINTL_RSID
#define FMSTR_LINTL_RSID ((FMSTR_LINTL_SID)+0x40U)
#endif

/* comma-separated list of additional command identifiers which follow the SID in the TL request */
#ifndef FMSTR_LINTL_CMDIDS
#define FMSTR_LINTL_CMDIDS 0x2bU
#endif

/* comma-separated list of additional response identifiers which follow the RSID in the TL response */
#ifndef FMSTR_LINTL_RSPIDS
#define FMSTR_LINTL_RSPIDS 0x2bU
#endif

#endif /* FMSTR_USE_LINTL */

/* USB CDC transport layer */
#if FMSTR_USE_USB_CDC
#ifndef FMSTR_USB_CDC_ID
#define  FMSTR_USB_CDC_ID      (0)   /* ID to identify USB CONTROLLER used by FreeMASTER */
#endif
#endif

/* Enables or disables byte access to communication buffer */
#ifndef FMSTR_BYTE_BUFFER_ACCESS
#define FMSTR_BYTE_BUFFER_ACCESS 1
#endif

/* Disable call of User callback function
   #ifndef FMSTR_ISR_CALLBACK
   #define FMSTR_ISR_CALLBACK 0
   #endif
*/

/* read memory commands are ENABLED by default */
#ifndef FMSTR_USE_READMEM
#define FMSTR_USE_READMEM 1
#endif
#ifndef FMSTR_USE_WRITEMEM
#define FMSTR_USE_WRITEMEM 1
#endif
#ifndef FMSTR_USE_WRITEMEMMASK
#define FMSTR_USE_WRITEMEMMASK 1
#endif

/* read variable commands are DISABLED by default */
#ifndef FMSTR_USE_READVAR
#define FMSTR_USE_READVAR 0
#endif
#ifndef FMSTR_USE_WRITEVAR
#define FMSTR_USE_WRITEVAR 0
#endif
#ifndef FMSTR_USE_WRITEVARMASK
#define FMSTR_USE_WRITEVARMASK 0
#endif

/* default scope settings */
#ifndef FMSTR_USE_SCOPE
#define FMSTR_USE_SCOPE 0
#endif

#ifndef FMSTR_MAX_SCOPE_VARS
#define FMSTR_MAX_SCOPE_VARS 8
#endif
/* default recorder settings */
#ifndef FMSTR_USE_RECORDER
#define FMSTR_USE_RECORDER 0
#endif

#ifndef FMSTR_MAX_REC_VARS
#define FMSTR_MAX_REC_VARS 8
#endif

#ifndef FMSTR_REC_FARBUFF
#define FMSTR_REC_FARBUFF 0
#endif

#ifndef FMSTR_REC_OWNBUFF
#define FMSTR_REC_OWNBUFF 0
#endif

#ifndef FMSTR_USE_FASTREC
#define FMSTR_USE_FASTREC 0
#endif

/* Enable code size optimalization */
#ifndef FMSTR_LIGHT_VERSION
#define FMSTR_LIGHT_VERSION 0
#endif

/* Always report single error code from recorder routines  */
#ifndef FMSTR_REC_COMMON_ERR_CODES
#define FMSTR_REC_COMMON_ERR_CODES FMSTR_LIGHT_VERSION
#endif

/* Remove code for single wire communication */
#ifndef FMSTR_SCI_TWOWIRE_ONLY
#define FMSTR_SCI_TWOWIRE_ONLY FMSTR_LIGHT_VERSION
#endif

/* Number of recorder post-trigger samples is by default controlled by PC */
#ifndef FMSTR_REC_STATIC_POSTTRIG
#define FMSTR_REC_STATIC_POSTTRIG 0
#endif

/* Recorder divisor is by default controlled by PC */
#ifndef FMSTR_REC_STATIC_DIVISOR
#define FMSTR_REC_STATIC_DIVISOR 0
#endif

/* Enable Floating point support in Recorder triggering */
#ifndef FMSTR_REC_FLOAT_TRIG
#define FMSTR_REC_FLOAT_TRIG 0
#endif

/* Enable larger mode of recorder */
#ifndef FMSTR_REC_LARGE_MODE
#define  FMSTR_REC_LARGE_MODE 0
#endif

#if FMSTR_REC_LARGE_MODE
#define FMSTR_CFG_REC_LARGE_MODE FMSTR_CFGFLAG_REC_LARGE_MODE
#else
#define FMSTR_CFG_REC_LARGE_MODE 0
#endif

/* Debug-TX mode is a special mode used to test or debug the data transmitter. Our driver
   will be sending test frames periodically until a first valid command is received from the
   PC Host. You can hook a logic analyzer to transmission pins to determine port and baudrate.
   Or you can connect the FreeMASTER tool and run the connection wizard to listen for the
   dummy frames. */
#ifndef FMSTR_DEBUG_TX
#define FMSTR_DEBUG_TX 0
#endif

/* check recorder settings */
#if FMSTR_USE_RECORDER || FMSTR_USE_FASTREC

/* 0 means recorder time base is "unknown" */
#ifndef FMSTR_REC_TIMEBASE
#define FMSTR_REC_TIMEBASE 0
#endif

/* default recorder buffer size is 256 */
#ifndef FMSTR_REC_BUFF_SIZE
#define FMSTR_REC_BUFF_SIZE 256
#endif

#endif
/* default app.cmds settings */
#ifndef FMSTR_USE_APPCMD
#define FMSTR_USE_APPCMD 0
#endif

#ifndef FMSTR_APPCMD_BUFF_SIZE
#define FMSTR_APPCMD_BUFF_SIZE 16
#endif

#ifndef FMSTR_MAX_APPCMD_CALLS
#define FMSTR_MAX_APPCMD_CALLS 0
#endif

/* TSA configuration check */
#ifndef FMSTR_USE_TSA
#define FMSTR_USE_TSA 0
#endif

#if (FMSTR_USE_TSA) && (FMSTR_DISABLE)
#warning Forcing FMSTR_USE_TSA to 0 due to FMSTR_DISABLE
#undef FMSTR_USE_TSA
#define FMSTR_USE_TSA 0
#endif

#ifndef FMSTR_USE_TSA_SAFETY
#define FMSTR_USE_TSA_SAFETY 0
#endif

/* TSA table allocation modifier */
#ifndef FMSTR_USE_TSA_INROM
#define FMSTR_USE_TSA_INROM 0
#endif

#if FMSTR_USE_TSA_INROM
#define FMSTR_TSA_CDECL const
#else
#define FMSTR_TSA_CDECL
#endif

#ifndef FMSTR_USE_TSA_DYNAMIC
#define FMSTR_USE_TSA_DYNAMIC 0
#endif

/* SFIO not used by default */
#ifndef FMSTR_USE_SFIO
#define FMSTR_USE_SFIO 0
#endif

/* use transport "pipe" functionality */
#ifndef FMSTR_USE_PIPES
#define FMSTR_USE_PIPES 0
#endif

/* "pipe" putstring formatting (enabled by default) */
#ifndef FMSTR_USE_PIPE_PRINTF
#define FMSTR_USE_PIPE_PRINTF 1
#endif

/* "pipe" variable-argument printf (enabled by default) */
#ifndef FMSTR_USE_PIPE_PRINTF_VARG
#define FMSTR_USE_PIPE_PRINTF_VARG FMSTR_USE_PIPE_PRINTF
#endif

#if FMSTR_USE_PIPES
/* one pipe by default */
#ifndef FMSTR_MAX_PIPES_COUNT
#define FMSTR_MAX_PIPES_COUNT 1
#endif

/* pipe printf buffer */
#ifndef FMSTR_PIPES_PRINTF_BUFF_SIZE
#define FMSTR_PIPES_PRINTF_BUFF_SIZE 48
#endif
#endif

/* what kind of board information structure will be sent? */
#ifndef FMSTR_USE_BRIEFINFO
#if FMSTR_USE_RECORDER
/* recorder requires full info */
#define FMSTR_USE_BRIEFINFO 0
#else
/* otherwise no brief info is enough */
#define FMSTR_USE_BRIEFINFO 1
#endif
#endif

/* automatic buffer size by default */
#ifndef FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE 0
#endif

/* automatic: determine required buffer size based on features enabled */
#if !FMSTR_COMM_BUFFER_SIZE
/* smallest for basic commands (getinfobrief, write/read memory etc.) */
#undef  FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE 11

/* full info required */
#if !(FMSTR_USE_BRIEFINFO) && (FMSTR_COMM_BUFFER_SIZE < (35+1))
#undef  FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE (35+1)
#endif

/* using application commands (must accommodate maximal app.cmd data length) */
#if FMSTR_USE_APPCMD && (FMSTR_COMM_BUFFER_SIZE < ((FMSTR_APPCMD_BUFF_SIZE)+1+2))
#undef  FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE ((FMSTR_APPCMD_BUFF_SIZE)+1+2)
#endif

/* configuring scope (EX) */
#if FMSTR_USE_SCOPE && (FMSTR_COMM_BUFFER_SIZE < (((FMSTR_MAX_SCOPE_VARS)*5)+1+2))
#undef  FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE (((FMSTR_MAX_SCOPE_VARS)*5)+1+2)
#endif

/* configuring recorder (EX) */
#if FMSTR_USE_RECORDER && (FMSTR_COMM_BUFFER_SIZE < (((FMSTR_MAX_REC_VARS)*5)+18+2))
#undef  FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE (((FMSTR_MAX_REC_VARS)*5)+18+2)
#endif

/* SFIO encapsulation (in buffer) */
#if FMSTR_USE_SFIO
#if FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_INPUT_DATA_LENGTH)+1+2)
#undef  FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE ((SFIO_MAX_INPUT_DATA_LENGTH)+1+2)
#endif
#endif

/* SFIO encapsulation (out buffer) */
#if FMSTR_USE_SFIO
#if FMSTR_COMM_BUFFER_SIZE < ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1+2)
#undef  FMSTR_COMM_BUFFER_SIZE
#define FMSTR_COMM_BUFFER_SIZE ((SFIO_MAX_OUTPUT_DATA_LENGTH)+1+2)
#endif
#endif

#endif

/* Demo application configuration. Enough RAM and ROM resources by default,
 * but not really large ROM by default (storing images and other active content) */
#ifndef FMSTR_DEMO_ENOUGH_RAM
#define FMSTR_DEMO_ENOUGH_RAM 1
#endif
#ifndef FMSTR_DEMO_ENOUGH_ROM
#define FMSTR_DEMO_ENOUGH_ROM 1
#endif
#ifndef FMSTR_DEMO_LARGE_ROM
#define FMSTR_DEMO_LARGE_ROM  0
#endif
/* in LARGE_ROM is defined, then also enable ENOUGH_ROM option */
#if FMSTR_DEMO_LARGE_ROM && !(FMSTR_DEMO_ENOUGH_ROM)
#warning Probably wrong demo application settings
#undef  FMSTR_DEMO_ENOUGH_ROM
#define FMSTR_DEMO_ENOUGH_ROM 1
#endif
/* Platform configuration for demo application */
#ifndef FMSTR_DEMO_SUPPORT_I64
#define FMSTR_DEMO_SUPPORT_I64 1  /* support for long long type */
#endif
#ifndef FMSTR_DEMO_SUPPORT_FLT
#define FMSTR_DEMO_SUPPORT_FLT 1  /* support for float type */
#endif
#ifndef FMSTR_DEMO_SUPPORT_DBL
#define FMSTR_DEMO_SUPPORT_DBL 1  /* support for double type */
#endif

#endif /* __FREEMASTER_DEF_CFG_H */
