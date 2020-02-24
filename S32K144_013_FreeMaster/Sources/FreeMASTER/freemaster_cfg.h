/*
 * Copyright (c) 2015 - 2016 , Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
*
*   @file    freemaster_cfg.h
*
*   @brief   FreeMASTER Serial Communication Driver configuration file.
*
******************************************************************************/

#ifndef __FREEMASTER_CFG_H
#define __FREEMASTER_CFG_H

/******************************************************************************
* Select interrupt or poll-driven serial communication
******************************************************************************/

#define FMSTR_LONG_INTR        0    /* Complete message processing in interrupt */
#define FMSTR_SHORT_INTR       1    /* SCI FIFO-queuing done in interrupt */
#define FMSTR_POLL_DRIVEN      0    /* No interrupt needed, polling only */

/*****************************************************************************
* Select communication interface (LPUART/FlexCAN base address)
******************************************************************************/

#define FMSTR_SCI_BASE         0x4006B000UL /* LPUART1 base on S32K14x */
#define FMSTR_CAN_BASE         0x40024000UL /* FlexCAN0 base on S32K14x */

#define FMSTR_DISABLE          0    /* To disable all FreeMASTER functionalities */
#define FMSTR_USE_LPUART       1    /* To select LPUART communication interface */
#define FMSTR_USE_FLEXCAN      0    /* To select FlexCAN communication interface */
#define FMSTR_USE_PDBDM        0    /* To select Packet Driven BDM communication interface (optional) */

/* Select RX and TX FlexCAN Message buffers */
#define FMSTR_FLEXCAN_TXMB     0
#define FMSTR_FLEXCAN_RXMB     1

/******************************************************************************
* Input/output communication buffer size
******************************************************************************/

#define FMSTR_COMM_BUFFER_SIZE 0    /* Set to 0 for "automatic" */

/******************************************************************************
* Receive FIFO queue size (use with FMSTR_SHORT_INTR only)
******************************************************************************/

#define FMSTR_COMM_RQUEUE_SIZE 32   /* Set to 0 for "default" */

/*****************************************************************************
* When RAM or ROM resources are limited, exclude some of the FreeMASTER
* features by default in the demo application
******************************************************************************/
#if 1
#define FMSTR_DEMO_ENOUGH_RAM  1
#define FMSTR_DEMO_ENOUGH_ROM  1
#define FMSTR_DEMO_LARGE_ROM   1
#else
#define FMSTR_DEMO_ENOUGH_RAM  0
#define FMSTR_DEMO_ENOUGH_ROM  0
#define FMSTR_DEMO_LARGE_ROM   0
#endif

/* Configure example application to match physical capabilities of the CPU */
#define FMSTR_DEMO_SUPPORT_I64 1
#define FMSTR_DEMO_SUPPORT_FLT 1
#define FMSTR_DEMO_SUPPORT_DBL 1

/*****************************************************************************
* Support for Application Commands
******************************************************************************/

#define FMSTR_USE_APPCMD       FMSTR_DEMO_ENOUGH_ROM    /* Enable/disable App.Commands support */
#define FMSTR_APPCMD_BUFF_SIZE 32   /* App.Command data buffer size */
#define FMSTR_MAX_APPCMD_CALLS 4    /* How many app.cmd callbacks? (0=disable) */

/*****************************************************************************
* Oscilloscope support
******************************************************************************/

#define FMSTR_USE_SCOPE        FMSTR_DEMO_ENOUGH_RAM     /* Enable/disable scope support */
#define FMSTR_MAX_SCOPE_VARS   8     /* Max. number of scope variables (2..8) */

/*****************************************************************************
* Recorder support
******************************************************************************/

#define FMSTR_USE_RECORDER     FMSTR_DEMO_ENOUGH_RAM     /* Enable/disable recorder support */
#define FMSTR_MAX_REC_VARS     8     /* Max. number of recorder variables (2..8) */
#define FMSTR_REC_OWNBUFF      0     /* Use user-allocated rec. buffer (1=yes) */

/* Built-in recorder buffer (use when FMSTR_REC_OWNBUFF is 0) */
#define FMSTR_REC_BUFF_SIZE    512   /* Built-in buffer size */

/* Recorder time base, specifies how often the recorder is called in the user app. */
#define FMSTR_REC_TIMEBASE     FMSTR_REC_BASE_MILLISEC(0) /* 0 = "unknown" */

#define FMSTR_REC_FLOAT_TRIG   0    /* Enable/disable floating point triggering */

/*****************************************************************************
* Target-side address translation (TSA)
******************************************************************************/

#define FMSTR_USE_TSA          0    /* Enable TSA functionality */
#define FMSTR_USE_TSA_INROM    1    /* TSA tables to ROM or RAM */
#define FMSTR_USE_TSA_SAFETY  FMSTR_DEMO_ENOUGH_RAM     /* Enable access to TSA variables only */
#define FMSTR_USE_TSA_DYNAMIC FMSTR_DEMO_ENOUGH_RAM     /* Enable dynamic TSA table */

/*****************************************************************************
* Pipes as data streaming over FreeMASTER protocol
******************************************************************************/

#define FMSTR_USE_PIPES        1   /* Enable/Disable pipes */
#define FMSTR_MAX_PIPES_COUNT  3   /* 3 pipes for demo purposes */

/*****************************************************************************
* Enable/Disable read/write memory commands
******************************************************************************/

#define FMSTR_USE_READMEM      1    /* Enable read memory commands */
#define FMSTR_USE_WRITEMEM     1    /* Enable write memory commands */
#define FMSTR_USE_WRITEMEMMASK 1    /* Enable write memory bits commands */

/*****************************************************************************
* Enable/Disable read/write variable commands (a bit faster than Read Mem)
******************************************************************************/

#define FMSTR_USE_READVAR      1    /* Enable read variable fast commands */
#define FMSTR_USE_WRITEVAR     1    /* Enable write variable fast commands */
#define FMSTR_USE_WRITEVARMASK 1    /* Enable write variable bits fast commands */

#endif /* __FREEMASTER_CFG_H */
