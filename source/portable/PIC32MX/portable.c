/********************************************************************
 *
 *  DESC
 *
 *  MODULE NAME:        portable.c
 *
 *  AUTHOR:        		Dave Sandler
 *
 *  DESCRIPTION:        This module contains the pico micro-kernel
 *						portable functions.
 *
 *  EDIT HISTORY:
 *  BASELINE
 *  VERSION     INIT    DESCRIPTION OF CHANGE
 *  --------    ----    ----------------------
 *   06-20-09   DS  	Module creation.
 *   09-30-10   DS  	Modified for the PIC32MX.
 *   09-06-12   DS  	don't clear os_seconds on reset
 *   09-19-12   DS  	Modified for the dsPic
 *   05-07-13   DS  	Add dsPIC, PIC24 support
 *   05-21-13   DS  	break out into platform specific directories
 *
 *  Copyright (c) 2009 - 2016 Dave Sandler
 *
 *  This file is part of pico.
 *
 *  pico is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License 
 *  as published by the Free Software Foundation, either version 3 
 *  of the License, or (at your option) any later version.
 *
 *  pico is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with pico.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contiki-OS protothreads license:
 *  Copyright (c) 2004-2005, Swedish Institute of Computer Science.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Institute nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *******************************************************************/

/*
 ********************************************************************
 *
 *   System Includes
 */

#define 	 PORTABLE_C
#include    <xc.h>
#include    <sys/attribs.h>

/*
 ********************************************************************
 *
 *   Common Includes
 */

/*
 ********************************************************************
 *
 *   Board Specific Includes
 */

/*
 ********************************************************************
 *
 *   Constants
 */

/*
 ********************************************************************
 *
 *   Program Globals
 */

/*
 ********************************************************************
 *
 *   Module Globals
 */

/*
 ********************************************************************
 *
 *   Prototypes
 */

void 	os_tick_interrupt( void );
void 	os_delay_us( uint32_t );
void 	os_delay_ms( uint16_t );
void 	os_tick_interrupt( void );
void 	os_tick_init( void );

/*
 ********************************************************************
 *
 *   External Procedures
 */

/*
 ********************************************************************
 *
 *   Module Data
 */

static uint16_t		one_sec_prescale;
extern tcb_entry_t  *current_task;
extern k_list_t     k_ready_list, k_wait_list;
extern tcb_entry_t  tcb[N_TASKS];
extern timer_t		last_tick;

/*
 * Let compile time pre-processor calculate the timer 1 tick period
 *
 */
#define SYS_FREQ 	CPU_CLOCK_HZ
#define T2_PRESCALE	64
#define PB_DIV		1
#define T2_PRIO		4
#define T2_CONFIG	(T2_ON | T2_IDLE_CON | T2_GATE_OFF | T2_PS_1_64 | T2_32BIT_MODE_OFF | T2_SOURCE_INT)
#define T2_RELOAD	(SYS_FREQ/(PB_DIV*T2_PRESCALE*TICK_RATE_HZ) - 1)

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:
 *
 *  DESCRIPTION:
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

static void os_tick_start(void)
{
    T2CONbits.ON = 1;			/* start the timer		*/
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:
 *
 *  DESCRIPTION:
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

static void os_tick_stop(void)
{
    T2CONbits.ON = 0;			/* stop the timer		*/
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:
 *
 *  DESCRIPTION:
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

void os_tick_init( void )
{
    /*
     * Configure SysTick to
     *	interrupt at the requested rate.
     *			and start it.
     */
    T2CONbits.ON = 0;       // timer2 is disabled       
    one_sec_prescale = SYSTICKHZ;
    
    T2CONbits.TCS = 1;      // external clock source
    T2CONbits.TCKPS = 0;    // 1:1 prescaler  
    
    T2CKR = 0b1101;         //external clock pin PA14
    TMR2  = 0;              //clear timer2    
    PR2   = T2_RELOAD;      //PR2 = 999, 1Hz Timer     
    IPC2bits.T2IP = 1;      //priority 1
    IPC2bits.T2IS = 0;      //sub-priority 0
    IFS0bits.T2IF = 0;      //clear flag
    IEC0bits.T2IE = 1;      //enable interrupt    
    T2CONbits.ON  = 1;      //timer is enabled
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:
 *
 *  DESCRIPTION:
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

void os_wdt_init(void)
{
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:
 *
 *  DESCRIPTION:
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

void os_wdt_reset(void)
{
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:
 *
 *  DESCRIPTION:
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

void os_sleep_init(void)
{
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:
 *
 *  DESCRIPTION:
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

void os_sleep(void)
{
    os_tick_stop();
    /*
     * here we do whatever we need to sleep...
     */
    os_tick_start();
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	os_delay_us
 *
 *  DESCRIPTION:	os_delay in units of 1uS
 *
 *  INPUT:			Delay interval
 *
 *  OUTPUT:			none
 *
 *******************************************************************/

void os_delay_us( uint32_t us )
{
    uint32_t i;
	os_wdt_reset();
    DI();
    for (i = 0; i < us; i++)
    {
    }
    EI();
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	os_delay_ms
 *
 *  DESCRIPTION:	os_delay in units of 1mS
 *
 *  INPUT:			Delay interval
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void
os_delay_ms( uint16_t ms )
{
    while (ms--)
    {
        os_delay_us(1000);
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   os_tick_interrupt
 *
 *  DESCRIPTION:    OS timer interrupt. The PIC32MX and the Luminary
 *					Cortex use the respective core timers. For the PIC32MX
 *                  we copied the services provided in the Microchip supplied
 *                  system_services.c
 *
 *  INPUT:
 *
 *  OUTPUT:
 *
 *******************************************************************/

void __ISR_AT_VECTOR(_TIMER_2_VECTOR, IPL1AUTO) os_tick_interrupt(void)
{
    IFS0bits.T2IF = 0; //clear flag
    os_timerHook();
    if (0 == --one_sec_prescale)
    {
        one_sec_prescale = SYSTICKHZ;
        os_seconds++;
    }
}
/*
 *  END OF portable.c
 *
 *******************************************************************/
