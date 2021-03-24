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
 * 9-06-12			 DS	    os_seconds made persistent
 * 9-21-12			 DS	    unlink UIP from the PIC32 and TCP/IP. simple conditional
 *							build switch
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

#ifndef	_PICO_H
	#define	_PICO_H
	#include "dtypes.h"
	#include "k_cfg.h"
	#include "portable.h"
	#include "pt.h"

	/*
	 * tcb related stuff
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
	#define TCB_NULL_ENV							 0xFF

	#define	ME				                  current_task
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
	#define	OS_DATA
	#define OS_VARS
	#define register

	typedef uint32_t timer_t;

	typedef uint8_t  s_link_t;
	typedef uint32_t stack_t;

	typedef	uint8_t os_error;

	typedef struct link
	{
	    struct link OS_DATA *next;
	    struct link OS_DATA *last;
	} k_list_t;

	typedef struct slink
	{
	    struct slink OS_DATA *next;
	} k_slist_t;

	typedef struct
	{
	    s_link_t	head;
	    s_link_t	tail;
	} k_queue_t;

	#define Q_NULL  (k_list_t *)0
	#define SL_NULL (k_slist_t *)0
	#define sl_NULL (s_link_t)-1;

	typedef struct pt tcb_pt_t;

	typedef struct
	{
	    k_list_t tcb_link;
	    timer_t  timer;
	    timer_t  gptimer;
	    uint8_t  flags;
	    uint8_t  task_env;
	    tcb_pt_t tcbpt;
	    int      ( *p_thread )( tcb_pt_t * );
	} tcb_entry_t;

	typedef struct
	{
	    k_slist_t  t_hook_link;
	    void     ( *p_timerhook )( void );
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
		    uint16_t r_port;
		    void  ( *p_app_handler )( void );
		} uip_appentry_t;

		#define ProtoTCP    1
		#define ProtoUDP    2
		#define ProtoNULL   0
		_SCOPE_	timer_t	uip_timer;
		_SCOPE_	timer_t	arp_timer;
		/*
		 * to get to ethernet header info in buffer
		 */
		#define BUF          	((struct uip_eth_hdr *)&uip_buf[0])
		_SCOPE_ uip_ipaddr_t 	ipaddr;
		_SCOPE_ void        	kuip_app_handler( void );
		_SCOPE_ uint8_t 		kuip_app_add_entry( uint8_t, uint16_t, void ( *)(void) );
		_SCOPE_ void 			kuip_app_remove_entry( uint16_t );
		_SCOPE_ void        	kuip_app_udphandler( void );
		_SCOPE_ uint8_t 		kuip_app_add_udpentry( uint8_t, uint16_t, void ( *)(void) );
		_SCOPE_ void 			kuip_app_remove_udpentry( uint16_t );

		#define UnusedPort	0
		#define ClientPort	1
		#define Server_port	2
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
	_SCOPE_ void         os_start_sched( void );
	_SCOPE_ tcb_entry_t *os_create_task( uint8_t, uint8_t, int ( *)(tcb_pt_t *));
	_SCOPE_ void 		 os_resume_task( tcb_entry_t * );
	_SCOPE_ void 	     os_suspend_task( k_list_t *, k_list_t * );
	_SCOPE_ void 		 os_kill_task( tcb_entry_t * );
	_SCOPE_ tcb_entry_t *os_get_tcb( void );
	_SCOPE_ void 	     os_release_tcb( tcb_entry_t * );
	_SCOPE_ tcb_entry_t *os_get_task_handle(int (*)(tcb_pt_t *));

	#define				 os_suspend( q )	os_suspend_task( q, (k_list_t *)ME )
	_SCOPE_ void		 os_add_timerhook( t_hook_entry_t *, void ( *)(void));
	_SCOPE_ void		 os_add_schedhook( t_hook_entry_t *, void ( *)(void));
	_SCOPE_ void		 os_hook_handler(t_hook_entry_t *);
	_SCOPE_ void		 os_release_timerhook( t_hook_entry_t *);
	_SCOPE_ void		 os_release_schedhook( t_hook_entry_t *);
	_SCOPE_ void		 os_add_hook( t_hook_entry_t *, t_hook_entry_t *, void ( *)(void));
	_SCOPE_ void		 os_release_hook( t_hook_entry_t *, t_hook_entry_t *);
	/*
	 *	Timing related API services
	 */
	_SCOPE_ void	os_delay( tcb_entry_t *, timer_t );
	#define			get_task_timer( t )		t->timer
	#define			set_task_timer( t, d )	t->timer = d
	#define start_task_timer(t) \
		t->flags |= TCB_TIMING; \
		t->flags &= ~TCB_TIMEOUT
	#define			get_timer_status( t )	(t->timer & TCB_TMRSTAT)
	#define			get_os_ticks( )			current_tick
	#define			task_timer_expired(t)	(0 != (t->flags & TCB_TIMEOUT))
	#define			setgptimer( t, d )	    t->gptimer = d
	#define			gp_timer_expired(t)	    (0 ==  t->gptimer)
	#define			gettask_env( t )		t->task_env
	/*
	 * low-level functions
	 */
	_SCOPE_ void	    os_init( void );
	_SCOPE_ void 	    service_os_timers( void );
	_SCOPE_ void        os_timerHook( void );
	_SCOPE_ void        os_delay_ms( uint16_t );
	_SCOPE_ void        os_delay_us( uint32_t );
	_SCOPE_ void        os_tick_delay( uint16_t );
	_SCOPE_ timer_t		os_get_elapsed_time( timer_t * );
	_SCOPE_ void	os_update_timer(timer_t *, timer_t);
	_SCOPE_ void 	    kq_qinsert( k_list_t *, k_list_t * );
	_SCOPE_ k_list_t   *kq_qdelete( k_list_t * );
	_SCOPE_ void        kq_ndelete( k_list_t * );
	_SCOPE_ void 	    kq_slinsert( k_slist_t *, k_slist_t * );
	_SCOPE_ k_slist_t  *kq_sldelete( k_slist_t * );
	_SCOPE_ void        kq_slndelete( k_slist_t *, k_slist_t * );
	_SCOPE_ uint16_t   calc_fletcher16(uint8_t const *buf, uint16_t len);

	/*
	 *	kernel data, ...
	 */
	_SCOPE_ tcb_entry_t	*current_task;
	_SCOPE_ timer_t	volatile current_tick;
	_SCOPE_	uint32_t	 os_seconds;
	/*
	 *	kernel event flags, ...
	 */
	#define FLAG_SYSTICK	0
	_SCOPE_	uint32_t os_event_flags;

	#undef _SCOPE_
#endif /* safety check for duplicate .h file */
/*
 *  END OF pico.h
 *
 *******************************************************************/
