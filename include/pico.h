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
 * 9-30-10			 DS	    Modify for the PIC32MX and Microchip libs
 * 9-15-11			 DS	    Modify for singly linked lists
 * 9-06-12			 DS	    OSTickSeconds made persistent
 * 9-21-12			 DS	    unlink UIP from the PIC32 and TCP/IP. simple conditional
 *							build switch
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

#ifndef	_PICO_H
	#define	_PICO_H
	#include "dtypes.h"
	#include "k_cfg.h"
	#include "portable.h"
	#include "pt.h"

	/*
	 * TCB related stuff
	 */
	#define FOREVER         				while( 1 == 1)
	#define PRIOMASK						         0x0F
	#define OS_LO_PRIO						     PRIOMASK
	#define OS_MED_PRIO						            8
	#define OS_HI_PRIO						            0
	#define TCB_FREE						         0x80
	#define TCB_TIMING						         0x40
	#define TCB_TIMINGMASK					         0xC0
	#define TCB_TIMEOUT						         0x20
	#define TCB_TMRSTAT						         0x60

	#define	ME				                      CurTask
	#define	NO_TIMEOUT		                 ((TIMER_T)-1)
	#define	NOW				                  ((TIMER_T)0)
	#define	TIME_EXPIRED	                  ((TIMER_T)0)
	#define TICK_RATE_MS   ((TIMER_T) 1000 / TICK_RATE_HZ)
	#define SYSTICKHZ                        TICK_RATE_HZ
	#define SYSTICKMIN                   (60*TICK_RATE_HZ)
	#define SYSTICKHR                  (3600*TICK_RATE_HZ)
	#define SYSTICKMS       		    (SYSTICKHZ / 1000)
	#define SYSTICKUS        		 (SYSTICKHZ / 1000000)
	#define SYSTICKNS        	  (SYSTICKHZ / 1000000000)


	/*
	* data types
	 */
	#define	OS_data
	#define OS_vars
	#define register

	typedef unsigned long  TIMER_T;
	typedef unsigned short QSIZE_T;

	typedef unsigned char  SLINK_T;
	typedef unsigned long  STACK_T;

	typedef	BYTE 		   OS_error;

	typedef struct link
	{
	    struct link OS_data *next;
	    struct link OS_data *last;
	} K_LIST;

	typedef struct slink
	{
	    struct slink OS_data *next;
	} K_SLIST;

	typedef struct
	{
	    SLINK_T	head;
	    SLINK_T	tail;
	} K_QUEUE;

	#define Q_NULL        (K_LIST *)0
	#define SL_NULL       (K_SLIST *)0
	#define sl_NULL       (SLINK_T)-1;

	typedef struct pt TCB_PT;

	typedef struct
	{
	    K_LIST   TcbLink;
	    TIMER_T  Timer;
	    TIMER_T  gpTimer;
	    BYTE     Flags;
	    BYTE     TaskID;
	    TCB_PT   TCBpt;
	    int      ( *pThread )( TCB_PT * );
	} TCB_Entry;

	typedef struct
	{
	    K_SLIST  ThookLink;
	    void     ( *Thookfun )( void );
	} Thook_Entry;

	#ifdef USES_UIP
		#include "uip.h"
		#define UIP_TIMER_MS           500
		#define ARP_TIMER_MS         10000
		#define UIP_APP_TABLE_SZE        8
		#define FLAG_ETHRXPKT            1

		typedef struct
		{
		    BYTE	type;
		    BYTE	protocol;
		    WORD    rPort;
		    void  ( *pAppHandler )( void );
		} UIP_AppEntry;

		#define ProtoTCP    1
		#define ProtoUDP    2
		#define ProtoNULL   0
		_SCOPE_	TIMER_T		 UIP_Timer;
		_SCOPE_	TIMER_T		 ARP_Timer;
		/*
		 * to get to ethernet header info in buffer
		 */
		#define BUF          ((struct uip_eth_hdr *)&uip_buf[0])
		_SCOPE_ uip_ipaddr_t ipaddr;
		_SCOPE_ void        kUIP_AppHandler( void );
		_SCOPE_ BYTE 		kUIP_AppAddEntry( BYTE, WORD, void ( *)(void) );
		_SCOPE_ void 		kUIP_AppDeleteEntry( WORD );
		_SCOPE_ void        kUIP_AppUDPHandler( void );
		_SCOPE_ BYTE 		kUIP_AppAddUDPEntry( BYTE, WORD, void ( *)(void) );
		_SCOPE_ void 		kUIP_AppDeleteUDPEntry( WORD );

		#define UnusedPort	0
		#define ClientPort	1
		#define ServerPort	2
	#endif

	/*
	 ********************************************************************
	 *
	 *   routines exposed by this module
	 */
	#ifdef PICO_C
		#define _SCOPE_ PUBLIC	/**/
	#else
		#define _SCOPE_ EXTERN	/**/
	#endif

	/*
	*	Task related API services
	*/

	_SCOPE_ void        OS_StartSched( void );
	_SCOPE_ TCB_Entry  *OS_CreateTask( BYTE, BYTE, int ( *)(TCB_PT *));
	_SCOPE_ void 		OS_ResumeTask( TCB_Entry * );
	_SCOPE_ void 	    OS_SuspendTask( K_LIST *, K_LIST * );
	_SCOPE_ void 		OS_KillTask( TCB_Entry * );
	_SCOPE_ TCB_Entry  *OS_FindTask( BYTE );
	_SCOPE_ TCB_Entry  *OS_GetTCB( void );
	_SCOPE_ void 	    OS_ReleaseTCB( TCB_Entry * );
	#define		OS_Suspend( q )	OS_SuspendTask( q, (K_LIST *)ME )
	_SCOPE_ void		OS_AddTimerHook( Thook_Entry *, void ( *)(void));
	_SCOPE_ void		OS_AddSchedHook( Thook_Entry *, void ( *)(void));
	_SCOPE_ void		OS_HookHandler(Thook_Entry *);
	_SCOPE_ void		OS_ReleaseTimerHook( Thook_Entry *);
	_SCOPE_ void		OS_ReleaseSchedHook( Thook_Entry *);
	_SCOPE_ void		OS_AddHook( Thook_Entry *, Thook_Entry *, void ( *)(void));
	_SCOPE_ void		OS_ReleaseHook( Thook_Entry *, Thook_Entry *);

	/*
	 *	Timing related API services
	 */

	_SCOPE_ void 		OS_Delay( TCB_Entry *, TIMER_T );
	#define		getTaskTimer( t )		t->Timer
	#define		setTaskTimer( t, d )	t->Timer = d
	#define		startTaskTimer( t )		t->Flags |= TCB_TIMING; t->Flags &= ~TCB_TIMEOUT
	#define		getTimerStatus( t )		(t->Timer & TCB_TMRSTAT)
	#define		getSysTick( )			CurrentTick
	#define		TaskTimerExpired(t)	    (0 != (t->Flags & TCB_TIMEOUT))
	#define		setgpTimer( t, d )	    t->gpTimer = d
	#define		gpTimerExpired(t)	    (0 ==  t->gpTimer)
	/*
	 * low-level functions
	 */

	_SCOPE_ void	    OS_Init( void );
	_SCOPE_ void 	    ServiceOSTimers( void );
	_SCOPE_ void        OS_TimerHook( void );
	_SCOPE_ void        OS_DelayUs( DWORD );
	_SCOPE_ void        OS_DelayMs( WORD );
	_SCOPE_ void        OS_TickDelay( WORD );
	_SCOPE_ TCB_Entry  *OS_GetTCB( void );
	_SCOPE_ void 	    OS_ReleaseTCB( TCB_Entry * );
	_SCOPE_ void 	    KQ_qinsert( K_LIST *, K_LIST * );
	_SCOPE_ K_LIST     *KQ_qdelete( K_LIST * );
	_SCOPE_ void        KQ_ndelete( K_LIST * );
	_SCOPE_ void 	    KQ_slinsert( K_SLIST *, K_SLIST * );
	_SCOPE_ K_SLIST    *KQ_sldelete( K_SLIST * );
	_SCOPE_ void        KQ_slndelete( K_SLIST *, K_SLIST * );

	/*
	 *	kernel data, ...
	 */

	_SCOPE_ TCB_Entry   *CurTask;
	_SCOPE_ TIMER_T		 CurrentTick;
	_SCOPE_	DWORD		 OSTickSeconds __attribute__ ((persistent));

	EXTERN 				 TCB_Entry   *CurTask;

	/*
	 *	kernel event flags, ...
	 */

	#define FLAG_SYSTICK	0
	_SCOPE_	DWORD OSEventFlags;

	#undef _SCOPE_

#endif /* safety check for duplicate .h file */
/*
 *  END OF pico.h
 *
 *******************************************************************/
