/* 
 * File:   common.h
 * 
 * Common definitions used in this project.
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
 * Created on 11th June 2020, 14:35
 */

#ifndef COMMON_H
#define	COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include <pic.h>
#include <pic16f628a.h>

//#pragma config FOSC = INTOSCCLK
#pragma config FOSC = HS
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)


#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000
#endif
    
#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

