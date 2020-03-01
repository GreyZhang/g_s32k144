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
* File              console.h
* Owner             Jiri Kral
* Version           1.0
* Date              Sep-08-2017
* Classification    General Business Information
* Brief             FreeRTOS printf/scanf via UART
*
********************************************************************************
* Detailed Description:
* console call back function header.
*
*
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


#ifndef CONSOLE_H
#define CONSOLE_H

#include "ewl_misra_types.h"
#include "file_struc.h"



int_t __read_console(__file_handle handle, uchar_t * buffer, size_t * count);
int_t __write_console(__file_handle handle, uchar_t * buffer, size_t * count);
int_t __close_console(__file_handle handle);

#endif

