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
	#define	NO_TIMEOUT		                 ((timer_t)-1)
	#define	NOW				                  ((timer_t)0)
	#define	TIME_EXPIRED	                  ((timer_t)0)
	#define TICK_RATE_MS   ((timer_t) 1000 / TICK_RATE_HZ)
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

	typedef uint32_t timer_t;
	typedef uint16_t QSIZE_T;

	typedef uint8_t  SLINK_T;
	typedef uint32_t STACK_T;

	typedef	uint8_t OS_error;

	typedef struct link
	{
	    struct link OS_data *next;
	    struct link OS_data *last;
	} k_list_t;

	typedef struct slink
	{
	    struct slink OS_data *next;
	} k_slist_t;

	typedef struct
	{
	    SLINK_T	head;
	    SLINK_T	tail;
	} K_QUEUE;

	#define Q_NULL  (k_list_t *)0
	#define SL_NULL (k_slist_t *)0
	#define sl_NULL (SLINK_T)-1;

	typedef struct pt tcb_pt_t;

	typedef struct
	{
	    k_list_t TcbLink;
	    timer_t  Timer;
	    timer_t  gpTimer;
	    uint8_t  Flags;
	    uint8_t  TaskEnv;
	    tcb_pt_t TCBpt;
	    int      ( *pThread )( tcb_pt_t * );
	} tcb_entry_t;

	typedef struct
	{
	    k_slist_t  ThookLink;
	    void     ( *Thookfun )( void );
	} t_hook_entry_t;

	#ifdef USES_UIP
		#include "uip.h"
		#define UIP_TIMER_MS           500
		#define ARP_TIMER_MS         10000
		#define UIP_APP_TABLE_SZE        8
		#define FLAG_ETHRXPKT            1

		typedef struct
		{
		    uint8_t	type;
		    uint8_t	protocol;
		    uint16_t rPort;
		    void  ( *pAppHandler )( void );
		} uip_appentry_t;

		#define ProtoTCP    1
		#define ProtoUDP    2
		#define ProtoNULL   0
		_SCOPE_	timer_t	UIP_Timer;
		_SCOPE_	timer_t	ARP_Timer;
		/*
		 * to get to ethernet header info in buffer
		 */
		#define BUF          	((struct uip_eth_hdr *)&uip_buf[0])
		_SCOPE_ uip_ipaddr_t 	ipaddr;
		_SCOPE_ void        	kUIP_AppHandler( void );
		_SCOPE_ uint8_t 		kUIP_AppAddEntry( uint8_t, uint16_t, void ( *)(void) );
		_SCOPE_ void 			kUIP_AppDeleteEntry( uint16_t );
		_SCOPE_ void        	kUIP_AppUDPHandler( void );
		_SCOPE_ uint8_t 		kUIP_AppAddUDPEntry( uint8_t, uint16_t, void ( *)(void) );
		_SCOPE_ void 			kUIP_AppDeleteUDPEntry( uint16_t );

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
		#define _SCOPE_ 	/**/
	#else
		#define _SCOPE_ extern	/**/
	#endif
	/*
	 *	Task related API services
	 */
	_SCOPE_ void         OS_StartSched( void );
	_SCOPE_ tcb_entry_t *OS_CreateTask( uint8_t, uint8_t, int ( *)(tcb_pt_t *));
	_SCOPE_ void 		 OS_ResumeTask( tcb_entry_t * );
	_SCOPE_ void 	     OS_SuspendTask( k_list_t *, k_list_t * );
	_SCOPE_ void 		 OS_KillTask( tcb_entry_t * );
	_SCOPE_ tcb_entry_t *OS_GetTCB( void );
	_SCOPE_ void 	     OS_ReleaseTCB( tcb_entry_t * );
	#define				 OS_Suspend( q )	OS_SuspendTask( q, (k_list_t *)ME )
	_SCOPE_ void		 OS_AddTimerHook( t_hook_entry_t *, void ( *)(void));
	_SCOPE_ void		 OS_AddSchedHook( t_hook_entry_t *, void ( *)(void));
	_SCOPE_ void		 OS_HookHandler(t_hook_entry_t *);
	_SCOPE_ void		 OS_ReleaseTimerHook( t_hook_entry_t *);
	_SCOPE_ void		 OS_ReleaseSchedHook( t_hook_entry_t *);
	_SCOPE_ void		 OS_AddHook( t_hook_entry_t *, t_hook_entry_t *, void ( *)(void));
	_SCOPE_ void		 OS_ReleaseHook( t_hook_entry_t *, t_hook_entry_t *);
	/*
	 *	Timing related API services
	 */
	_SCOPE_ void	OS_Delay( tcb_entry_t *, timer_t );
	#define			getTaskTimer( t )		t->Timer
	#define			setTaskTimer( t, d )	t->Timer = d
	#define			startTaskTimer( t )		t->Flags |= TCB_TIMING; t->Flags &= ~TCB_TIMEOUT
	#define			getTimerStatus( t )		(t->Timer & TCB_TMRSTAT)
	#define			getSysTick( )			CurrentTick
	#define			TaskTimerExpired(t)	    (0 != (t->Flags & TCB_TIMEOUT))
	#define			setgpTimer( t, d )	    t->gpTimer = d
	#define			gpTimerExpired(t)	    (0 ==  t->gpTimer)
	#define			getTaskEnv( t )			t->TaskEnv
	/*
	 * low-level functions
	 */
	_SCOPE_ void	    OS_Init( void );
	_SCOPE_ void 	    ServiceOSTimers( void );
	_SCOPE_ void        OS_TimerHook( void );
	_SCOPE_ void        OS_DelayUs( uint32_t );
	_SCOPE_ void        OS_DelayMs( uint16_t );
	_SCOPE_ void        OS_TickDelay( uint16_t );
	_SCOPE_ void 	    KQ_qinsert( k_list_t *, k_list_t * );
	_SCOPE_ k_list_t   *KQ_qdelete( k_list_t * );
	_SCOPE_ void        KQ_ndelete( k_list_t * );
	_SCOPE_ void 	    KQ_slinsert( k_slist_t *, k_slist_t * );
	_SCOPE_ k_slist_t  *KQ_sldelete( k_slist_t * );
	_SCOPE_ void        KQ_slndelete( k_slist_t *, k_slist_t * );
	/*
	 *	kernel data, ...
	 */
	_SCOPE_ tcb_entry_t	*CurTask;
	_SCOPE_ timer_t		 CurrentTick;
	_SCOPE_	uint32_t	 OSTickSeconds;
	/*
	 *	kernel event flags, ...
	 */
	#define FLAG_SYSTICK	0
	_SCOPE_	uint32_t OSEventFlags;

	#undef _SCOPE_
#endif /* safety check for duplicate .h file */
/*
 *  END OF pico.h
 *
 *******************************************************************/
