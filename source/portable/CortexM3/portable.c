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

#include 	"hw_ints.h"
#include 	"hw_memmap.h"
#include 	"hw_sysctl.h"
#include 	"hw_types.h"
#include	"interrupt.h"
#include 	"flash.h"
#include 	"adc.h"
#include 	"debug.h"
#include 	"gpio.h"
#include 	"pwm.h"
#include 	"sysctl.h"
#include 	"uart.h"
#include 	"systick.h"

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

PUBLIC	void 	SetupTickInterrupt( void );

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

LOCAL		WORD		OneSecPrescaler;
EXTERN  	TCB_Entry  *CurTask;
EXTERN  	K_LIST      k_ready_list, k_wait_list;
EXTERN  	TCB_Entry   TCB[N_TASKS];
EXTERN  	TIMER_T		CurrentTick;
EXTERN		TIMER_T		LastTick;
/*
 ********************************************************************
 *
 *	Interrupt priority definitions for the Luminary (TI) Cortex.  The top
 *  3 bits of these values are significant with lower values
 *  indicating higher priority interrupts.
 */

#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

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
    SysTickPeriodSet(SysCtlClockGet() / TICK_RATE_HZ);
    SysTickEnable();
    SysTickIntEnable();
}
#endif
/*
 *  END OF portable.c
 *
 *******************************************************************/
