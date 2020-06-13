/**
 * main.h
 *
 * Main module definition file
 *
 * Author: Luis Teixeira (creationfactory.co)

 * Licence and copyright notice:
 *
 * Copyright 2020 Luis Teixeira
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, softwar
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Created on 11th June 2020, 14:38
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <pic16f628.h>
#include "pic.h"

#define BAUD_RATE 115200

// (watchdog) timer related settings:
    
/*
 * The offset can be calculated as follows:
 * 
 * TMR1_OFFSET = 65535 - (_XTAL_FREQ / (Prescaler * 40))
 *
 */
#define TMR1_OFFSET             0x0BDB      // Offset for calibrating the timer.
#define WT_TIMEOUT              4000        // timeout period (in tenths of seconds) for the watchdog to trigger
#define POWER_OFF_DURATION      8000        // ms to keep power off for resetting the target device
#define POWER_OFF_DELAY         3000        // delay between the poweroff command and actually powering off the device
#define WDT_TOKEN_LENGTH        14

#define SHUTDOWN_CMD            "poweroff -f\r\n"
#define SHUTDOWN_CMD_LEN        14

unsigned int timerCount;

// this is should be a pseudo-unique string, so that 
// there is no chance of other output from the serial
// port coincide with this value:

unsigned char wdtToken[WDT_TOKEN_LENGTH + 1] = "kiosk_wdtToken";

void init(void);
void enableTimer(void);
void serialWriteChar(char value);
void serialWrite(char* str, char length);

/**
 * This function checks if the bytes we are receiving are part of the string
 * we are expecting:
 * 
 **/
void tokenMatchLoop(void);

void interrupt isr(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

