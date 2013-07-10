/********************************************************************
 * 	DESC
 *
 *  MODULE NAME:	pico.h
 *
 *  AUTHOR:        	Dave Sandler
 *
 *  DESCRIPTION:    This header file contains prototypes and variables
 *                  	that require a scope outside of the home .C module.
 *
 *
 *  EDIT HISTORY:
 *  DATE    VERSION  INIT   DESCRIPTION OF CHANGE
 *  ------  -------  ----   ----------------------
 * 4-26-07			 DS	    Creation
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

#ifndef	_PICOQ_H
	#define	_PICOQ_H
	#include "pico.h"
	#include "picosem.h"

	/*
	* return codes
	 */
	#define Q_SUCCESS	0
	#define	Q_FULL		1
	#define	Q_EMPTY		2

	typedef unsigned char Q_SIZE_T;
	typedef unsigned char Q_TYPE_T;

	/*
	* data types
	 */
	typedef struct
	{
	    Q_SIZE_T  qsize;
	    Q_SIZE_T  inptr;
	    Q_SIZE_T  outptr;
	    Q_TYPE_T *buff;
	} OS_Queue;

	/*
	 ********************************************************************
	 *
	 *   routines exposed by this module
	 */
	#ifdef PICOQ_C
		#define _SCOPE_ PUBLIC	/**/
	#else
		#define _SCOPE_ EXTERN	/**/
	#endif

	/*
	*	Task related API services
	*/

	_SCOPE_ void     OS_QueInit(OS_Queue *, Q_SIZE_T, Q_TYPE_T *);
	_SCOPE_ BYTE     OS_QueAdd(OS_Queue *, Q_TYPE_T *);
	_SCOPE_ Q_SIZE_T OS_QuePutArray(OS_Queue *, Q_TYPE_T *, Q_SIZE_T);
	_SCOPE_ BYTE     OS_QuePutString(OS_Queue *, Q_TYPE_T *);
	_SCOPE_ BYTE     OS_QueRemove(OS_Queue *, Q_TYPE_T *);
	_SCOPE_ BYTE     OS_QuePeek(OS_Queue *, Q_TYPE_T *);
	_SCOPE_ void     OS_QueFlush(OS_Queue *);
	#define		     OS_QuePut(q, i) 	OS_QueAdd(q, (Q_TYPE_T *)&i)
	#define		     OS_QueGet(q, i) 	OS_QueRemove(q, (Q_TYPE_T *)&i)
	#define		     OS_QueEmpty(q)		(q->inptr == q->outptr)
	#define		     OS_QueFull(q)		(((q->inptr + 1) % q->qsize) == q->outptr)
	#define  		 OS_QueAvailable(q) (Q_SIZE_T)(q.outptr - q.inptr - 1)
	#undef _SCOPE_
#endif
/*
 ********************************************************/
