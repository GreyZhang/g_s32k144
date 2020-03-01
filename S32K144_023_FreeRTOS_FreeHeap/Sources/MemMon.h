/*******************************************************************************
* NXP Semiconductors
* (c) Copyright 2017 NXP Semiconductors
* ALL RIGHTS RESERVED.
********************************************************************************
Services performed by NXP in this matter are performed AS IS and without any
warranty. CUSTOMER retains the final decision relative to the total design
and functionality of the end product. NXP neither guarantees nor will be held
liable by CUSTOMER for the success of this project.
NXP DISCLAIMS ALL WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING,
BUT NOT LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR
A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE ORE ADVISE SUPPLIED
TO THE PROJECT BY NXP, AND OR NAY PRODUCT RESULTING FROM NXP SERVICES.
IN NO EVENT SHALL NXP BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THIS AGREEMENT.
CUSTOMER agrees to hold NXP harmless against any and all claims demands
or actions by anyone on account of any damage, or injury, whether commercial,
contractual, or tortuous, rising directly or indirectly as a result
of the advise or assistance supplied CUSTOMER in connection with product,
services or goods supplied under this Agreement.
********************************************************************************
* Project			FreeRTOS_printf
* File              MemMon.h
* Owner             Jiri Kral
* Version           1.0
* Date              Sep-08-2017
* Classification    General Business Information
* Brief             FreeRTOS printf/scanf via UART
*
********************************************************************************
* Detailed Description:
* Basic memory monitor header file.
*

*
* ------------------------------------------------------------------------------
* Test HW:         S32K144EVB,
* MCU:             S32K144
*
* Debugger:        OpenSDA
*
* Target:          internal_FLASH (debug mode, debug_ram and release mode)
*
*
*/

#ifndef MEM_MON_H
#define MEM_MON_H


#include "ewl_misra_types.h"
#include "lpuart1.h"
#include "file_struc.h"
#include <stdio.h>
#include <string.h>


#define helpMsg "\n\rCommands:\r\n \
F show free heap memory\r\n \
A set memory address\r\n \
V print value on address\r\n \
M set address to main()\n\r \
P previous mem page\n\r \
N next mem page\n\r \
? help\n\r \
X Exit from command mode\n\r"


#define MEMMON_EXIT_COMMAND_MODE	0
#define MEMMON_COMMAND_MODE			1

void PrintMem(void);
void PrintVal(void);
int ProcessMessage(char message);
void MemMon(void);

#endif

