/*
 * File:   main.c
 *
 * Main module of the generic-watchdog-timer firmware.
 *
 * This project introduces a standalone watchdog timer for use with
 * any device featuring one availabe serial port (RS-232), and having the 
 * capability of periodically sending a specific string of data during its 
 * normal operation.
 * 
 * Combined with a relay, this device is then capable of cycling the power
 * of the target device in order to (hard) restart it in case of failure.
 * 
 * This source code is accompanied with an application schematic diagram to
 * help in the construction of a complete unit using the PIC16F628A microcontroller
 * and this firmware.
 *
 * The diagram below shows the pin mappings of the PIC16F628A microntroller 
 * used  for this particular application:
 *
 *                     +---------------+
 *   N/A         -   --|(RA2)|   |(RA1)|--   - N/A
 *                     |     \---/     |
 *   N/A         -   --|(RA3)     (RA0)|--   - N/A
 *                     |               |
 *   N/A         -   --|(RA4)     (RA7)|--   - N/A
 *                     |               |
 *   N/A         -   --|(RA5)     (RA6)|--   - N/A
 *                     |               |
 *   GND         -   --|(VSS)     (VDD)|--   - +5V
 *                     |               |
 *   N/A         -   --|(RB0)     (RB7)|--   - N/A
 *                     |               |
 *   RX          >   --|(RB1)     (RB6)|--   - N/A
 *                     |               |
 *   TX          <   --|(RB2)     (RB5)|--   > RELAY
 *                     |               |
 *   N/A         -   --|(RB3)     (RB4)|--   - N/A
 *                     +---------------+
 *
 * Author: Luis Teixeira (creationfactory.co)
 * 
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
 * Created on 11th June 2020, 14:41
 */

#include "main.h"
#include "common.h"

void init(void) {
    // We need to set this if we are using the internal oscillator
    // OSCF = 1 -> 4 MHz; OSCF = 0 -> 48 KHz
    PCONbits.OSCF = 1;

    // Disable the comparator:
    CMCON = 0x07;
    
    // Initialize the GPIO pins:
    PORTA = 0;
    PORTB = 0;
    
    // Serial port TX and RX pins set as output and input respectively.
    // RB5 set as output for controlling the relay (or other switching device).
    TRISB1 = 1;
    TRISB2 = 0;
    TRISB5 = 0;
  
    SPBRG = SP_BAUD_TIMER;
    BRGH = 1;  // for high baud_rate
    SYNC = 0;  // asynchronous mode
    SPEN = 1;  // enable the serial port pins
    TXEN = 1;  // enable transmission
    CREN = 1;  // enable continuous reception
    TX9 = 0;   // 8-bit transmission selected
    RX9 = 0;   // 8-bit reception selected

    // timer initialization:
    TMR1CS = 0; // select internal Fosc/4 clock source
    T1CONbits.T1CKPS = 0b11; // enable 1:8 prescale
    
    // turn on the output for the relay:
    PORTBbits.RB5 = 1;
}

void enableTimer(void) {
    TMR1ON = 0;
    TMR1IE = 0;
    TMR1IF = 0;

    TMR1 = TMR1_OFFSET;
    
    timerCount = 0;

    // Enable timer1:
    
    TMR1ON = 1;
    
    TMR1IE = 1; // re-enable timer1 interrupts
    PEIE = 1;
    GIE = 1;
}

void serialWriteChar(char value) {
    while(!TXIF);
    
    TXREG = value;
}

void serialWrite(char* str, char length) {
    int i;
    
    for(i = 0; i < length; i++) {
        serialWriteChar(str[i]);
    }
}

void __interrupt() isr(void) {   
    TMR1IE = 0; // disable timer1 interrupts
    PEIE = 0;
    GIE = 0;

    if(TMR1IF) { // Timer tick expired
        TMR1 = TMR1_OFFSET;

        if(timerCount < WT_TIMEOUT) {
            timerCount++;
        }
        else {
            // Send the suthdown command to the device
            // and wait some time before cycling its power.
            
            serialWrite((char*) SHUTDOWN_CMD, SHUTDOWN_CMD_LEN);

            __delay_ms(POWER_OFF_DELAY);
            
            // cycle the power of the device:
            PORTBbits.RB5 = 0;
            __delay_ms(POWER_OFF_DURATION);
            PORTBbits.RB5 = 1;
            timerCount = 0;
        }
    }

    TMR1IF = 0; // reset timer interrupt flag

    TMR1IE = 1; // re-enable timer1 interrupts
    PEIE = 1;
    GIE = 1;
}

void tokenMatchLoop(void) {
    char matches;
    unsigned char wdtTokenCurrChar;
    
    // check for errors:
    if(OERR) {
        CREN = 0;
        CREN = 1;
    }

    matches = 1;
    wdtTokenCurrChar = 0;
    
    while(matches) {
        // let's wait for data:

        while(!RCIF);
        
        // echo the received character into the output (use for debug only):
        // serialWriteChar(RCREG);
        
        // read a byte from the serial port and compare with the 
        // current char in the expected token:
        if(RCREG == wdtToken[wdtTokenCurrChar]) {
            if(wdtTokenCurrChar < WDT_TOKEN_LENGTH - 1) {
                wdtTokenCurrChar++;
            }
            else {
                // We got to the end of the state machine
                // and all characters matched the string.
                // We may reset the watchdog:

                wdtTokenCurrChar = 0;
                TMR1 = TMR1_OFFSET;
                timerCount = 0;
                matches = 0;
            }
        }
        else {
            // the current character did not match so we return
            // to the beginning:
            matches = 0;
            wdtTokenCurrChar = 0;
        }
    }
}

void main(void) {
    __delay_ms(1000);
    
    // initialize the peripherals:
    init();

    // enable the watchdog timer:
    enableTimer();
    
    // program loop:
    for(;;) {
        tokenMatchLoop();
    }
}
