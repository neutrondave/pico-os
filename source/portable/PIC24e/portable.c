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
 *  Copyright (c) 2009 - 2021 Dave Sandler
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
#include	"pico.h"
#include	"portable.h"
//#include    "HardwareProfile.h"
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

void 	os_delay_us( uint32_t );
void 	os_delay_ms( uint16_t );
void   _T1Interrupt( void );
void   _OscillatorFail(void);
void   _AddressError(void);
void   _StackError(void);
void   _MathError(void);
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

static uint16_t     one_sec_prescale;
extern tcb_entry_t  *current_task;
extern k_list_t     k_ready_list, k_wait_list;
extern tcb_entry_t  tcb[N_TASKS];
extern timer_t      last_tick;

#define SYS_FREQ    CPU_CLOCK_HZ
#define T1_PRESCALE 8
#define T1_RELOAD   (SYS_FREQ/T1_PRESCALE/TICK_RATE_HZ)

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
    T1CONbits.TON		= 1;			/* start the timer		*/
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
    T1CONbits.TON		= 0;			/* stop the timer		*/
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
    /*
     * turn off timer 1, clear it, set it,
     *	and turn it on...
     */
    TMR1  				= 0;			/* clear the timer register 	*/
    PR1   				= T1_RELOAD;    /* set the prescaler		*/
    T1CON				= 0;			/* reset the timer control reg	*/
    T1CONbits.TCKPS0	= 1;			/* div by 8			*/
    T1CONbits.TCKPS1	= 0;			/* "				*/
    IPC0bits.T1IP		= 4;			/* priority level		*/
    IFS0bits.T1IF		= 0;			/* clear the interrupt flag	*/
    IEC0bits.T1IE		= 1;			/* enable the timer interrupt	*/
    SRbits.IPL	 		= 3;			/* cpu priority levels 4-7	*/
    os_tick_start();
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
 *  INPUT:		Delay interval
 *
 *  OUTPUT:		none
 *
 *******************************************************************/

void os_delay_us( uint32_t MicroSecondCounter )
{
    uint32_t i;
    for (i = 0; i < MicroSecondCounter; i++)
    {
        __asm__ volatile ("repeat #25");
        __asm__ volatile ("nop");
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	os_delay_ms
 *
 *  DESCRIPTION:	os_delay in units of 1mS
 *
 *  INPUT:		Delay interval
 *
 *  OUTPUT:		none
 *
 *******************************************************************/
void os_delay_ms( uint16_t ms )
{
    while (ms--)
    {
        os_delay_us(1000);
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _T1Interrupt
 *
 *  DESCRIPTION:    OS timer interrupt. For the PIC24E we chose to use
 *                  timer 1. should that change, modify the setup and
 *                  the interrupt vector
 *
 *  INPUT:	none
 *
 *  OUTPUT:	none
 *
 *******************************************************************/

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    /*
     * clear the interrupt flag
     *	and handle the event
     */
    IFS0bits.T1IF	= 0;
    os_timerHook();
    if (0 == --one_sec_prescale)
    {
        one_sec_prescale = SYSTICKHZ;
        os_seconds++;
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	_OscillatorFail
 *
 *  DESCRIPTION:	PIC24E oscillator fail exception interrupt handler
 *
 *  INPUT:		none
 *
 *  OUTPUT:		does not return
 *
 *******************************************************************/

void __attribute__((interrupt, no_auto_psv)) _OscillatorFail(void)
{
    while (1);
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	_AddressError
 *
 *  DESCRIPTION:	PIC24E address fail exception interrupt handler
 *
 *  INPUT:		none
 *
 *  OUTPUT:		does not return
 *
 *******************************************************************/

void __attribute__((interrupt, no_auto_psv)) _AddressError(void)
{
    while (1);
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	_StackError
 *
 *  DESCRIPTION:	PIC24E stack fail exception interrupt handler
 *
 *  INPUT:		none
 *
 *  OUTPUT:		does not return
 *
 *******************************************************************/

void __attribute__((interrupt, no_auto_psv)) _StackError(void)
{
    while (1);
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	_MathError
 *
 *  DESCRIPTION:	PIC24E math fail exception interrupt handler
 *
 *  INPUT:		none
 *
 *  OUTPUT:		does not return
 *
 *******************************************************************/

void __attribute__((interrupt, no_auto_psv)) _MathError(void)
{
    while (1);
}
/*
 *  END OF portable.c
 *
 *******************************************************************/
