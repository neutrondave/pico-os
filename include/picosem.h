/********************************************************************
 * 	DESC			picosem.h
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
 *	09-27-2012		DS		modified to use protothreads
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

#ifndef	_PICOSEM_H
	#define	_PICOSEM_H

	#include "pico.h"

	typedef struct
	{
	    k_list_t  sem_link;
	    uint8_t   sem_count;
	} os_sem_t;

	#ifdef PICO_C
		#define _SCOPE_ /**/
	#else
		#define _SCOPE_ extern
	#endif

	/*
	 *********************************************************
	 *
	 * void os_sem_wait(pt,*sem,timeout)
	 *	wait on a semaphore
	 */
	#define os_sem_wait(pt, sem, timeout)                    \
	    do												\
	    {												\
	        os_delay(ME, timeout);						\
	        LC_SET((pt)->lc);							\
	        if(!sem.sem_count && !(task_timer_expired(ME)))\
	        {											\
	            os_suspend( (k_list_t *)&sem );			\
	            return PT_WAITING;						\
	        }											\
		} while (0);                                         \
	    if (sem.sem_count)								\
	    {												\
	        sem.sem_count--;								\
	    }												\
	/*
	 *	Semaphore related API services
	 */
	_SCOPE_ void os_sem_init( os_sem_t * );
	_SCOPE_ void os_sem_signal( os_sem_t * );
	_SCOPE_ uint8_t os_sem_peek( os_sem_t * );

	#undef _SCOPE_

#endif /* safety check for duplicate .h file */
/*
 *  END OF picosem.h
 *
 *******************************************************************/
