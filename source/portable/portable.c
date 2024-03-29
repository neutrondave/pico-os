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
 *   08-12-15   DS  	support for the Arm Cortex-M
 *
 *  Copyright (c) 2009 - 2015 Dave Sandler
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
#include	<asf.h>
#include	"pico.h"
#include	"portable.h"
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
void SysTick_Handler(void);
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

#define SYSTICK_RELOAD		(CPU_CLOCK_HZ/SYSTICKHZ)
#define NVIC_SYSTICK_CTRL   ((volatile unsigned long *) 0xe000e010)
#define NVIC_SYSTICK_LOAD   ((volatile unsigned long *) 0xe000e014)
#define NVIC_SYSTICK_VAL	((volatile unsigned long *) 0xe000e018)
#define NVIC_SYSTICK_CLK    0x00000004
#define NVIC_SYSTICK_INT    0x00000002
#define NVIC_SYSTICK_ENABLE 0x00000001

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

void os_tick_init(void)
{
	/*
     * Configure SysTick to
     *	interrupt at the requested rate.
     *			and start it.
     */
	*(NVIC_SYSTICK_LOAD) = (system_cpu_clock_get_hz() / SYSTICKHZ) - 1UL;
	*(NVIC_SYSTICK_CTRL) = NVIC_SYSTICK_CLK | NVIC_SYSTICK_INT | NVIC_SYSTICK_ENABLE;
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
	struct wdt_conf config;
	wdt_get_config_defaults(&config);
	wdt_set_config(&config);
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
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
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

static void os_tick_start(void)
{
	*(NVIC_SYSTICK_CTRL) = NVIC_SYSTICK_CLK | NVIC_SYSTICK_INT | NVIC_SYSTICK_ENABLE;
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
	*(NVIC_SYSTICK_CTRL) = 0;
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
	struct wdt_conf config;
	bool			wd_enable;

	os_tick_stop();
	wdt_get_config_defaults(&config);
	wd_enable	  = config.enable;
	config.enable = false;
	wdt_set_config(&config);

	system_sleep();
	wdt_get_config_defaults(&config);
	config.enable = wd_enable;
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

void os_delay_us(uint32_t us)
{
	cpu_delay_us(us);
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
void os_delay_ms(uint16_t ms)
{
	os_tick_delay(ms);
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   SysTickHandler
 *
 *  DESCRIPTION:    OS timer interrupt. For the Arm Cortex M we chose 
 *					to use the system tick interrupt.
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/

void SysTick_Handler(void)
{
    system_interrupt_enter_critical_section();
	/*
     * clear the interrupt flag
     *	and handle the event
     */
    os_timerHook();
    if (0 == --one_sec_prescale)
    {
        one_sec_prescale = SYSTICKHZ;
        os_seconds++;
    }
	system_interrupt_leave_critical_section();
}
/*
 *  END OF portable.c
 *
 *******************************************************************/
