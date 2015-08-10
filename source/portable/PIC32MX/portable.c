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
 *   09-06-12   DS  	don't clear OSTickSeconds on reset
 *   09-19-12   DS  	Modified for the dsPic
 *   05-07-13   DS  	Add dsPIC, PIC24 support
 *   05-21-13   DS  	break out into platform specific directories
 *
 *  Copyright (c) 2009 - 2013 Dave Sandler
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
#include 	<plib.h>
#include    "HardwareProfile.h"

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

void 	OSTickInt( void );
void 	OS_DelayUs( uint32_t );
void 	OS_DelayMs( uint16_t );
void 	OSTickInt( void );
void   _general_exception_handler(unsigned int, unsigned int);
void 	SetupTickInterrupt( void );

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

static uint16_t		OneSecPrescaler;
extern tcb_entry_t  *CurTask;
extern k_list_t     k_ready_list, k_wait_list;
extern tcb_entry_t  TCB[N_TASKS];
extern timer_t		CurrentTick;
extern timer_t		LastTick;

/*
 * Let compile time pre-processor calculate the Timer 1 tick period
 *
 */
#define SYS_FREQ 				CPU_CLOCK_HZ
#define T2_PRESCALE				       64
#define PB_DIV					        1
#define T2_PRIO				            4
#define T2_CONFIG				(T2_ON | T2_IDLE_CON | T2_GATE_OFF | T2_PS_1_64 | T2_32BIT_MODE_OFF | T2_SOURCE_INT)
#define T2_RELOAD				(SYS_FREQ/(PB_DIV*T2_PRESCALE*TICK_RATE_HZ) - 1)

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

void
SetupTickInterrupt( void )
{
    /*
     * Configure SysTick to
     *	interrupt at the requested rate.
     *			and start it.
     */
    /*
     * Turn ON the system clock
     *
     */
    OpenTimer2(T2_CONFIG, T2_RELOAD);
    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_4);
    /*
     * Turn on the interrupts
     *
     */
    INTEnableSystemMultiVectoredInt();
    OneSecPrescaler = SYSTICKHZ;
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	OS_DelayUs
 *
 *  DESCRIPTION:	OS_Delay in units of 1uS
 *
 *  INPUT:			Delay interval
 *
 *  OUTPUT:			none
 *
 *******************************************************************/

void
OS_DelayUs( uint32_t MicroSecondCounter )
{
    volatile uint32_t cyclesRequiredForEntireDelay;
    if(GetInstructionClock() <= 500000)				 //for all FCY speeds under 500KHz (FOSC <= 1MHz)
    {
        //10 cycles burned through this path (includes return to caller).
        //For FOSC == 1MHZ, it takes 5us.
        //For FOSC == 4MHZ, it takes 0.5us
        //For FOSC == 8MHZ, it takes 0.25us.
        //For FOSC == 10MHZ, it takes 0.2us.
    }
    else
    {
        //7 cycles burned to this point.
        //We want to pre-calculate number of cycles required to delay 10us * tenMicroSecondCounter using a 1 cycle granule.
        cyclesRequiredForEntireDelay = (INT32)(GetInstructionClock()/1000000)*MicroSecondCounter;
        //We subtract all the cycles used up until we reach the while loop below, where each loop cycle count is subtracted.
        //Also we subtract the 5 cycle function return.
        cyclesRequiredForEntireDelay -= 24; 		//(19 + 5)
        if(cyclesRequiredForEntireDelay <= 0)
        {
            // If we have exceeded the cycle count already, bail!
        }
        else
        {
            while(cyclesRequiredForEntireDelay>0) 	//19 cycles used to this point.
            {
                cyclesRequiredForEntireDelay -= 8; 	//Subtract cycles burned while doing each delay stage, 8 in this case.
            }
        }
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	OS_DelayMs
 *
 *  DESCRIPTION:	OS_Delay in units of 1mS
 *
 *  INPUT:			Delay interval
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void
OS_DelayMs( uint16_t ms )
{
    while (ms--)
    {
        OS_DelayUs(1000);
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   OSTickInt
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

void
__ISR(_TIMER_2_VECTOR, IPL(TICK_IPL)) OSTickInt( void )
{
    mT2ClearIntFlag();
    OS_TimerHook();
    if (0 == --OneSecPrescaler)
    {
        OneSecPrescaler = SYSTICKHZ;
        OSTickSeconds++;
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:	_general_exception_handler
 *
 *  DESCRIPTION:	MIPS general exception interrupt handler
 *
 *  INPUT:			cause, status
 *
 *  OUTPUT:			does not return
 *
 *******************************************************************/

void
_general_exception_handler( unsigned int cause, unsigned int status )
{
    uint32_t excep_code;
    uint32_t excep_addr;
    excep_code = (cause & 0x0000007C) >> 2;
    excep_addr = __builtin_mfc0(_CP0_EPC, _CP0_EPC_SELECT);
    if ((cause & 0x80000000) != 0)
    {
        excep_addr += 4;
    }
    while (1);
}
/*
 *  END OF portable.c
 *
 *******************************************************************/
