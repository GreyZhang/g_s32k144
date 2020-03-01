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
* File              MemMon.c
* Owner             Jiri Kral
* Version           1.0
* Date              Sep-08-2017
* Classification    General Business Information
* Brief             FreeRTOS printf/scanf via UART
*
********************************************************************************
* Detailed Description:
* In this file are implemented basic memory monitor functions.
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

#include "MemMon.h"



static char membuff[256];
static char in_msg[256];
static unsigned int addr;
int main(void);
extern volatile size_t xFreeHeapSpace;
extern volatile unsigned long idle_counter;
extern volatile unsigned long task1_counter;

/*!
  \brief Outputs formated memory content.
  \details Function print 256 bytes of memory. Starting addres is stored in the global addr variable.
*/
void PrintMem(void)
{
	int i;
	memcpy(membuff,(unsigned int*)addr,256);
	for (i=0;i<256;i++)
	{
		if (i%16==0)
		{
			printf("\n%.8X: ",addr+i);
		}
		else
			if ((i%8==0) && (i>0)) printf("  ");
		printf("%.2X ",membuff[i]);
	}
	printf("\n\nReady\n");
}

/*!
  \brief Outputs data in couple formats.
  \details Function print out data from entered memory address in couple different formats.
*/
void PrintVal(void)
{
	int data;
	memcpy(&data,(unsigned int*)addr,4);
	printf("\nChar:  %c\nByte:  %d (0x%.2X)\n\rInt:   %d\n\rHEX:   0x%.8X\n\rFloat: %f\n\r",(char)data,(unsigned char)data,(unsigned char)data,data,data,(float)data);
	printf("\nReady\n");
}

/*!
  \brief Process message
  \details Function perform command entered by message.
*/
int ProcessMessage(char message)
{
	int retval = MEMMON_COMMAND_MODE;
	float cpu_usage;
	switch (message)
	{
	case 'A':
	case 'a':
		addr=0;
		printf("Enter start memory address(HEX):\n");
		scanf("%X",&addr);
		PrintMem();
		break;

	case 'F':
	case 'f':
		cpu_usage=(float)(2*task1_counter)/(float)idle_counter;
		printf("Free heap memory: %d Bytes.\nIdle counter: %X\n",xFreeHeapSpace,(unsigned int)idle_counter);
		printf("CPU usage: %.6f\n",cpu_usage);
		printf("\r\nReady\n\r");
		break;


	case 'M':
	case 'm':
		addr=(unsigned int)&main;
		PrintMem();
		break;

	case 'N':
	case 'n':
		addr+=256;
		PrintMem();
		break;

	case 'P':
	case 'p':
		addr-=256;
		PrintMem();
		break;

	case 'V':
	case 'v':
		addr=0;
		printf("Enter value address(HEX):\n");
		scanf("%X",&addr);
		printf("Data on address: 0x%X\n",addr);
		PrintVal();
		break;


	case '?':
		printf("%s\n",helpMsg);
		printf("\r\nReady\n\r");
		break;

	case 'X':
	case 'x':
	case 'Q':
	case 'q':
		retval=MEMMON_EXIT_COMMAND_MODE;
		break;

	default:
		printf("Unknown command %c.\n\r",message);
		printf("%s\r\nReady\n\r",helpMsg);
		break;
	}
	return retval;
}


void MemMon(void)
{
	while(1)
    {
	    scanf("%s",in_msg);
	    if (strlen(in_msg)>1)
	    	printf("Unknown command %s\n\n\rReady\n\r",in_msg);
	    else
	    	ProcessMessage(in_msg[0]);
	    }
}
