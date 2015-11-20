/********************************************************************
 * 	DESC			portable.h
 *
 *  MODULE NAME:
 *
 *  AUTHOR:
 *
 *  DESCRIPTION:    This header file contains prototypes and variables
 *                  	that require a scope outside of the home .C module.
 *
 *
 *  EDIT HISTORY:
 *  DATE    VERSION  INIT   DESCRIPTION OF CHANGE
 *  ------  -------  ----   ----------------------
 * 4-26-07			 DS	    Creation
 * 9-30-10			 DS	    Modify for the PIC32MX and Microchip libs
 * 9-19-12			 DS	    expand core selection switches
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

#ifndef	PORTABLE_H
	#define	PORTABLE_H
	#ifdef PORTABLE_C
		#define _SCOPE_ /**/
	#else
		#define _SCOPE_ extern
	#endif

	#if defined(PIC32MX)
		#include 	<GenericTypeDefs.h>
		#include    "HardwareProfile.h"
	#elif defined(PIC24E)
		//#include 	<GenericTypeDefs.h>
		//#include    "HardwareProfile.h"
	#elif defined(DSPIC30)
		//#include 	<GenericTypeDefs.h>
		//#include    "HardwareProfile.h"
	#elif defined(CORTEXM3)
		//#include	"hw_types.h"
		//#include	"cortex_m3.h"
		//#include	"interrupt.h"
	#elif defined(CORTEXM0)
	#else
		#error Unknown processor or compiler.
	#endif

	/*
	 ********************************************************************
	 *
	 *   type / data definitions
	 */

	/*
	 ********************************************************************
	 *
	 *   routines exposed by this module
	 */

	#undef _SCOPE_

	/*
	 *********************************************************
	 *
	 * 	Critical section management.
	 */

	#ifdef	PIC32MX
		#define DI()	INTDisableInterrupts()
		#define EI()	INTEnableInterrupts()
	#endif

	#ifdef	CORTEXM0
		/*
		 * Set basepri to MAX_SYSCALL_INTERRUPT_PRIORITY without effecting other
		 * registers.  r0 is clobbered.
		 */
		#define SET_INTERRUPT_MASK()	\
			__asm volatile                  \
			(								\
				" mov r0, %0         \n"	\
				" msr basepri, r0    \n"	\
				::"i"(configMAX_SYSCALL_INTERRUPT_PRIORITY):"r0"  \
			)
		/*
		 * Set basepri back to 0 without effective other registers.
		 * r0 is clobbered.
		 */
		#define CLEAR_INTERRUPT_MASK()		\
			__asm volatile                  \
			(								\
				" mov r0, #0          \n"	\
				" msr basepri, r0     \n"	\
				:::"r0"						\
			)

		#define SET_INTERRUPT_MASK_FROM_ISR()     0;SET_INTERRUPT_MASK()
		#define CLEAR_INTERRUPT_MASK_FROM_ISR(x)  CLEAR_INTERRUPT_MASK();(void)x
		#define DI()	SET_INTERRUPT_MASK()
		#define EI()	CLEAR_INTERRUPT_MASK()
		#define ClrWdt()
	#endif
	#define ENTER_CRITICAL()		DI()
	#define EXIT_CRITICAL()			EI()
	#define portNOP()
	#ifdef PORTABLE_C
		void SetupTickInterrupt(void);
	#else
		extern void SetupTickInterrupt(void);
	#endif

	#ifdef	CORTEXM3
		/*
		 * Set basepri to MAX_SYSCALL_INTERRUPT_PRIORITY without effecting other
		 * registers.  r0 is clobbered.
		 */
		#define SET_INTERRUPT_MASK()	\
			__asm volatile                  \
			(								\
				" mov r0, %0         \n"	\
				" msr basepri, r0    \n"	\
				::"i"(configMAX_SYSCALL_INTERRUPT_PRIORITY):"r0"  \
			)
		/*
		 * Set basepri back to 0 without effective other registers.
		 * r0 is clobbered.
		 */
		#define CLEAR_INTERRUPT_MASK()		\
			__asm volatile                  \
			(								\
				" mov r0, #0          \n"	\
				" msr basepri, r0     \n"	\
				:::"r0"						\
			)

		#define SET_INTERRUPT_MASK_FROM_ISR()     0;SET_INTERRUPT_MASK()
		#define CLEAR_INTERRUPT_MASK_FROM_ISR(x)  CLEAR_INTERRUPT_MASK();(void)x
		#define DI()	SET_INTERRUPT_MASK()
		#define EI()	CLEAR_INTERRUPT_MASK()
		#define ClrWdt()
	#endif
	#define ENTER_CRITICAL()		DI()
	#define EXIT_CRITICAL()			EI()
	#define portNOP()
	#ifdef PORTABLE_C
		void SetupTickInterrupt(void);
	#else
		extern void SetupTickInterrupt(void);
	#endif
#endif /* safety check for duplicate .h file */
/*
 *  END OF portable.h
 *
 *******************************************************************/
