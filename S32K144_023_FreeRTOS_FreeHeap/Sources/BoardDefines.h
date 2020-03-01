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

#ifndef _BOARD_DEFINES_H_
#define _BOARD_DEFINES_H_

#include <stdint.h>
#include <stdbool.h>

/* This file defines the specific ports and pins for Evaluation Board and
 * Validation Board. To use with Validation Board uncomment the following
 * line
 */

#define EVB

#ifdef EVB
    #define LED1            15U
    #define LED2            16U
    #define LED_GPIO        PTD
    #define LED_PORT        PORTD
    #define LED_PORT_PCC    PCC_PORTD_CLOCK
    #define BTN_GPIO        PTC
    #define BTN_PIN         13U
    #define BTN_PORT        PORTC
    #define BTN_PORT_PCC    PCC_PORTC_CLOCK
    #define BTN_PORT_IRQn   PORTC_IRQn
#else
    #define LED1            0U
    #define LED2            1U
    #define LED_GPIO        PTC
    #define LED_PORT        PORTC
    #define LED_PORT_PCC    PCC_PORTC_CLOCK
    #define BTN_GPIO        PTC
    #define BTN_PIN         13U
    #define BTN_PORT        PORTC
    #define BTN_PORT_PCC    PCC_PORTC_CLOCK
    #define BTN_PORT_IRQn   PORTC_IRQn
#endif

#include "pins_driver.h"

void boardSetup(void)
{
    /* Configure ports */
    PINS_DRV_SetMuxModeSel(LED_PORT, LED1,      PORT_MUX_AS_GPIO);
    PINS_DRV_SetMuxModeSel(LED_PORT, LED2,      PORT_MUX_AS_GPIO);
    PINS_DRV_SetMuxModeSel(BTN_PORT, BTN_PIN,   PORT_MUX_AS_GPIO);
#ifdef EVB
    PINS_DRV_SetPinIntSel(BTN_PORT, BTN_PIN, PORT_INT_RISING_EDGE);
#else
    PINS_DRV_SetPinIntSel(BTN_PORT, BTN_PIN, PORT_INT_FALLING_EDGE);
#endif
}
#endif
