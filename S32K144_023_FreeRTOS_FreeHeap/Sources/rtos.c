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
* File              rtos.c
* Owner             Jiri Kral
* Version           1.0
* Date              Sep-08-2017
* Classification    General Business Information
* Brief             FreeRTOS printf/scanf via UART
********************************************************************************

 *
 * The idle hook function:
 * The idle hook function demonstrates how to query the amount of FreeRTOS heap
 * space that is remaining (see vApplicationIdleHook() defined in this file).
 *
 * The rtos_star() Function:
 * rtos_star() creates one software timer, one queue, and two tasks.  It then starts
 * the scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 200 milliseconds, before sending the value 100 to the queue that
 * was created within main().  Once the value is sent, the task loops back
 * around to block for another 200 milliseconds.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop that causes it to
 * repeatedly attempt to read data from the queue that was created within
 * main().  When data is received, the task checks the value of the data, and
 * if the value equals the expected 100, toggles the green LED.  The 'block
 * time' parameter passed to the queue receive function specifies that the task
 * should be held in the Blocked state indefinitely to wait for data to be
 * available on the queue.  The queue receive task will only leave the Blocked
 * state when the queue send task writes to the queue.  As the queue send task
 * writes to the queue every 200 milliseconds, the queue receive task leaves the
 * Blocked state every 200 milliseconds, and therefore toggles the blue LED
 * every 200 milliseconds.
 *
 * The LED Software Timer and the Button Interrupt:
 * The user button BTN1 is configured to generate an interrupt each time it is
 * pressed. The interrupt service routine switches the red LED on and switch
 * application into command mode. Exit from command mode (x or q entered in
 * putty console) turn off red LED and app is switched into runtime mode.
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* SDK includes. */
#include "interrupt_manager.h"
#include "clock_manager.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "lpuart1.h"
#include "dmaController1.h"
#include "osif.h"

#include "BoardDefines.h"
#include <string.h>
#include <stdio.h>
#include "console.h"
#include "MemMon.h"


/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue, specified in milliseconds, and
converted to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 400 / portTICK_PERIOD_MS )

/* The LED will remain on until the button has not been pushed for a full
5000ms. */
#define mainBUTTON_LED_TIMER_PERIOD_MS		( 5000UL / portTICK_PERIOD_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/* The LED toggle by the queue receive task (blue). */
#define mainTASK_CONTROLLED_LED				( 1UL << 0UL )

/* The LED turned on by the button interrupt, and turned off by the LED timer
(green). */
#define mainTIMER_CONTROLLED_LED			( 1UL << 1UL )

/* The vector used by the GPIO port C.  Button SW7 is configured to generate
an interrupt on this port. */
#define mainGPIO_C_VECTOR					( 61 )

/* A block time of zero simply means "don't block". */
#define mainDONT_BLOCK						( 0UL )
#define MAX_MESSAGE_SIZE					( 256 )

/*-----------------------------------------------------------*/

/*
 * Setup the NVIC, LED outputs, and button inputs.
 */
static void prvSetupHardware( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/*
 * The LED timer callback function.  This does nothing but switch off the
 * LED defined by the mainTIMER_CONTROLLED_LED constant.
 */
static void prvButtonLEDTimerCallback( TimerHandle_t xTimer );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* The LED software timer.  This uses prvButtonLEDTimerCallback() as its callback
function. */

/* free heap memory get from idle task */
volatile size_t xFreeHeapSpace;

static TimerHandle_t xButtonLEDTimer = NULL;
/* Task 1 counter */
volatile unsigned long task1_counter;
/* Task 2 counter */
volatile unsigned long task2_counter;
/* idle task counter */
volatile unsigned long idle_counter;

/* Command mode message buffer */
static char msg[MAX_MESSAGE_SIZE];


/*-----------------------------------------------------------*/

void rtos_start( void )
{
	/* Configure the NVIC, LED outputs and button inputs. */
	prvSetupHardware();

	/* Create the queue. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( unsigned long ) );

	if( xQueue != NULL )
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */

		xTaskCreate( prvQueueReceiveTask, "RX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
		xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );


		/* Create the software timer that is responsible for turning off the LED
		if the button is not pushed within 5000ms, as described at the top of
		this file. */
		xButtonLEDTimer = xTimerCreate( "ButtonLEDTimer", 			/* A text name, purely to help debugging. */
									mainBUTTON_LED_TIMER_PERIOD_MS,	/* The timer period, in this case 5000ms (5s). */
									pdFALSE,						/* This is a one shot timer, so xAutoReload is set to pdFALSE. */
									( void * ) 0,					/* The ID is not used, so can be set to anything. */
									prvButtonLEDTimerCallback		/* The callback function that switches the LED off. */
								);

		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvButtonLEDTimerCallback( TimerHandle_t xTimer )
{
	/* Casting xTimer to void because it is unused */
	(void)xTimer;
	printf("Led timer callback\n");
	/* The timer has expired - so no button pushes have occurred in the last
	five seconds - turn the LED off. */
	//PINS_DRV_SetPins(LED_GPIO, (1 << LED1));
}
/*-----------------------------------------------------------*/

/* The ISR executed when the user button is pushed. */
void vPort_C_ISRHandler( void )
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    int CommandMode=1;

	/* The button was pushed, so ensure the LED is on before resetting the
	LED timer.  The LED timer will turn the LED off if the button is not
	pushed within 5000ms. */
    PINS_DRV_ClearPins(LED_GPIO, (1 << LED1));
    PINS_DRV_SetPins(LED_GPIO, (1 << LED2));
	/* This interrupt safe FreeRTOS function can be called from this interrupt
	because the interrupt priority is below the
	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY setting in FreeRTOSConfig.h. */
	xTimerResetFromISR( xButtonLEDTimer, &xHigherPriorityTaskWoken );



	printf("\nCommand mode\n============\n");
	printf("%s\nReady\n",helpMsg);


	while (CommandMode)
	{
		scanf("%s",msg);
		if (strlen(msg)>1)
			printf("Unknown command %s\n\n\rReady\n\r",msg);
		else
			CommandMode=ProcessMessage(msg[0]);
	}
	PINS_DRV_ClearPins(LED_GPIO, (1 << LED2));
	PINS_DRV_SetPins(LED_GPIO, (1 << LED1));
	/* If calling xTimerResetFromISR() caused a task (in this case the timer
	service/daemon task) to unblock, and the unblocked task has a priority
	higher than or equal to the task that was interrupted, then
	xHigherPriorityTaskWoken will now be set to pdTRUE, and calling
	portEND_SWITCHING_ISR() will ensure the unblocked task runs next. */

	/* Clear the interrupt before leaving. */
	PINS_DRV_ClearPortIntFlagCmd(BTN_PORT);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;

const unsigned long ulValueToSend = 100UL;
	/* Casting pvParameters to void because it is unused */
	(void)pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();
	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, the constant used converts ticks
		to ms.  While in the Blocked state this task will not consume any CPU
		time. */
		printf("Task 1 counter: 0x%.8X\n",(int)task1_counter++);
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle an LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, mainDONT_BLOCK );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
unsigned long ulReceivedValue;

	/* Casting pvParameters to void because it is unused */
	(void)pvParameters;

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */


		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */


		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		if( ulReceivedValue == 100 )
		{
			if (task1_counter % 2 == 0)
			{
				task2_counter++;
				printf("Task 2 counter: 0x%.8X\n",(int)task2_counter);
			}
		    PINS_DRV_TogglePins(LED_GPIO, (1 << LED2));
		}
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Initialize and configure clocks
     *  -   Setup system clocks, dividers
     *  -   see clock manager component for more details
     */

	/* Write your code here */
	 CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT, g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	 CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

	 PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
	 LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);

//	 LPUART_DRV_SendData(INST_LPUART1, (uint8_t *)welcomeMsg, strlen(welcomeMsg));
//	 while(LPUART_DRV_GetTransmitStatus(INST_LPUART1, &bytesRemaining) != STATUS_SUCCESS);

	 printf ("Welcome to FreeRTOS\n");

    boardSetup();

	/* Change LED1, LED2 to outputs. */
	PINS_DRV_SetPinsDirection(LED_GPIO,  (1 << LED1) | (1 << LED2));

	/* Change BTN1 to input */
	PINS_DRV_SetPinsDirection(BTN_GPIO, ~(1 << BTN_PIN));

	/* Start with LEDs off. */
	PINS_DRV_SetPins(LED_GPIO, (1 << LED1) | (1 << LED2));

//	EDMA_DRV_Init(&dmaController1_State, &dmaController1_InitConfig0, edmaChnStateArray, edmaChnConfigArray, EDMA_CONFIGURED_CHANNELS_COUNT);

	/* Install Button interrupt handler */
    INT_SYS_InstallHandler(BTN_PORT_IRQn, vPort_C_ISRHandler, (isr_t *)NULL);
    /* Enable Button interrupt handler */
    INT_SYS_EnableIRQ(BTN_PORT_IRQn);

    /* The interrupt calls an interrupt safe API function - so its priority must
    be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
    INT_SYS_SetPriority( BTN_PORT_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );

}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeHeapSpace = xPortGetFreeHeapSize();

	/* idle_counter may be used for monitoring CPU usage
	(ratio between Idle and task counters) */
	idle_counter++;

	if( xFreeHeapSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}

}
/*-----------------------------------------------------------*/

/* The Blinky build configuration does not include run time stats gathering,
however, the Full and Blinky build configurations share a FreeRTOSConfig.h
file.  Therefore, dummy run time stats functions need to be defined to keep the
linker happy. */
void vMainConfigureTimerForRunTimeStats( void ) {}
unsigned long ulMainGetRunTimeCounterValue( void ) { return 0UL; }

/* A tick hook is used by the "Full" build configuration.  The Full and blinky
build configurations share a FreeRTOSConfig.h header file, so this simple build
configuration also has to define a tick hook - even though it does not actually
use it for anything. */
void vApplicationTickHook( void ) {}

/*-----------------------------------------------------------*/
