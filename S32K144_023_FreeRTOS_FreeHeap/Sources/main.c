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
* File              main.c
* Owner             Jiri Kral
* Version           1.0
* Date              Sep-08-2017
* Classification    General Business Information
* Brief             FreeRTOS printf/scanf via UART
********************************************************************************
* Detailed Description:
* Application based on freertos S32DS example performs prinf/scanf functionality
* in FreeRTOS. Application run in two modes - standard runtime mode with two tasks
* maintained by scheduler and command mode implementing basic memory monitor.
* Application was tested with putty terminal (http://www.putty.org/)
*
* Putty serial terminal settings: 115200 8N1, no HW control.
*
* USAGE:
* In runtime mode is green LED blinking and on terminal are shown task
* counter values.
*
* To enter into COMMAND MODE - press button (next to LED diode). LED light
* switches to red and you can enter commands.
*
* Available commands:
* ===================
*
* F show free heap memory
* A set memory address
* V print value on address
* M set address to main()
* P previous mem page
* N next mem page
* ? help
* X Exit from command mode
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

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "pin_mux.h"
#include "clockMan1.h"
#include "FreeRTOS.h"
#include "lpuart1.h"
#include "dmaController1.h"
#include "osif.h"

volatile int exit_code = 0;

/* User includes (#include below this line is not maintained by Processor Expert) */
extern void rtos_start(void);
#define PEX_RTOS_START rtos_start


/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/
int main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/


  /* Write your code here */

  // All functionality is implemented in the rtos.c file.

  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the Freescale S32K series of microcontrollers.
**
** ###################################################################
*/
