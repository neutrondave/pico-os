/********************************************************************
 *
 *  DESC
 *
 *  MODULE NAME:        pico.c
 *
 *  AUTHOR:        		Dave Sandler
 *
 *  DESCRIPTION:        This module contains the pico micro-kernel
 *
 *  EDIT HISTORY:
 *  BASELINE
 *  VERSION     INIT    DESCRIPTION OF CHANGE
 *  --------    ----    ----------------------
 *   04-23-07   DS  	Module creation.
 *   09-30-10   DS  	Modified for the PIC32MX.
 *   10-10-10   DS  	added proto thread support
 *   08-21-11   DS  	added os_tick_delay for dead delays
 *   09-15-11   DS  	added single link list functions
 *						OS tick interrupt timer hook
 *   03-22-12   DS  	added pico scheduling loop hook
 *   09-19-12   DS  	single switch USES_UIP for the UIP TCP/IP stack
 *							as opposed to processor specific switching
 *						reworked service_os_timers. changed the do{}while to
 *							a for loop for readability; also, we'll handle
 *							timer updates only when time's elapsed.
 *   09-28-12   DS  	doxygen documentation support
 *   05-30-13   DS  	fix os_tick_delay(). immediate fall through meant no delay.
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
 ********************************************************************/

/**
 *   \addtogroup pico
 *	  @{
 */

/**
 * \file
 * 	pico kernel implementation.
 * \author
 * 	Dave Sandler <dsandler3@gmail.com>
 *
 */

#define                 PICO_C

/*
 *********************************************************************
 *
 *   System Includes
 */

#include	"pico.h"
#include	"picosem.h"
#include	"picomsg.h"

#ifdef USES_UIP
#include 	"uip.h"
#include 	"uip_arp.h"
#include 	"dhcpc.h"
#include 	"ustdlib.h"
#endif

/*
 *********************************************************************
 *
 *   Common Includes
 */

/*
 *********************************************************************
 *
 *   Board Specific Includes
 */

/*
 *********************************************************************
 *
 *   Constants
 */

/*
 *********************************************************************
 *
 *   Program Globals
 */

/**
 *********************************************************************
 *
 * \name Module Globals
 * @{
 */

static 	k_list_t       	k_ready_list;	/* !< list of tasks ready to run	                */
static	k_list_t		k_wait_list;	/* !< list of tasks waiting to run	                */
static	t_hook_entry_t *k_thook_list;	/* !< list of functions to be executed every tick	*/
static	t_hook_entry_t *k_loop_list;	/* !< list of tasks executed every kernel pass      */
static 	tcb_entry_t    	tcb[N_TASKS];	/* !< pico taskc ontrol blocks						*/
static	timer_t		 	last_tick;		/* !< last captured timer value 					*/
/** @} */
/*
 *********************************************************************
 *
 *   Prototypes
 */

extern void	os_tick_init(void);

/*
 *********************************************************************
 *
 *   Constants
 */
/**
 *********************************************************************
 *
 * \name OS Initialization and Invocation
 * @{
 */

/**
 *
 *********************************************************************
 *
 * This is the kernel's main loop.
 *	Functionally, os_start_sched executes forever, updating timers in
 *	task control blocks, executing any kernel 'hooked' functions, as
 *	well as the highest priority task waiting on the ready list.
 *
 * The kernel is non-preemptive. Any 'hooked' function or task will
 *	execute until returning control to the kernel.
 *
 * \param 	none
 *
 * \return 	should never return
 *
 * The following example shows how to start the pico scheduler.
 *
 \code
 #include pico.h
 #include hardwareprofile.h

 int
 main(void)
 {
	os_init();
		.
		.
	os_start_sched();
	return(-1);
}
 \endcode
 *
 */
void os_start_sched(void)
{
    FOREVER
    {
        service_os_timers();
        os_hook_handler(k_loop_list);

        if( &k_ready_list != k_ready_list.next )
        {
            current_task = (tcb_entry_t *)k_ready_list.next;
            current_task->p_thread(&(current_task->tcbpt));
        }
    }
}

/**
 *
 *********************************************************************
 *
 * kernel data structures are initialized. os_init() must be called
 *	before executing the kernel main loop. Functionally, os_init()
 *	sets up the system tick timer, flushes all kernel queues, and
 *	clears and releases all of the kernel's task control blocks.
 *
 * \param 	none
 *
 * \return 	none
 *
 * The following example shows how to initialize the pico scheduler.
 *
    \code
    #include pico.h
    #include hardwareprofile.h

    int
    main(void)
    {
	    os_init();
		    .
		    .
	    os_start_sched();
	    return(-1);
    }
    \endcode
 *
 */
void os_init(void)
{
    uint8_t index = 0;
    /*
     * initialize the target's tick hardware
     */
    os_tick_init();
#if (PICO_USE_WDT)
	os_wdt_init();
#endif
	os_sleep_init();
    k_ready_list.next = k_ready_list.last = &k_ready_list;
    k_wait_list.next  = k_wait_list.last  = &k_wait_list;
    k_thook_list	  = (t_hook_entry_t *)SL_NULL;
    k_loop_list	      = (t_hook_entry_t *)SL_NULL;
    last_tick         = get_os_ticks();
    do
    {
        os_release_tcb(&tcb[index]);
	} while (++index < N_TASKS);
}
/* @} */
/**
 *********************************************************************
 *
 * \name OS Task Services
 * @{
 */
/**
 *
 *********************************************************************
 *
 * Allocate an empty task control block, and initialize it for use.
 *
 * \param prio		task priority
 * \param env 		task 'environment' variable
 * \param pr_addr	task function pointer
 *
 * \return 	 		pointer to the tcb; NULL if none available
 *
 * The following example illustrates the allocation and creation of a
 *	pico TASK block.
 *
 *	NOTE: The scheduler must have been initialized first.
 *
 \code
 #include pico.h

 #define myPrioLevel	8
 #define mytask_env		6

 int myTask(tcb_pt_t *);

 void
 anyfunc(void)
 {
	tcb_entry_t t_handle;
	t_handle = os_create_task(myPrioLevel, mytask_env, myTask);
	if ((tcb_pt_t *)NULL != t_handle){
		task created successfully...
	}
	else {
		handle the error if required here...
	}
}
 \endcode
 *
 */
tcb_entry_t *os_create_task(uint8_t prio, uint8_t env, int (*pr_addr)(tcb_pt_t *))
{
    tcb_entry_t *handle;
    handle = os_get_tcb();
    if ((tcb_entry_t *)Q_NULL != handle)
    {
        handle->flags     = (prio & PRIOMASK);
        handle->task_env  =  env;
        handle->p_thread  =  pr_addr;
        PT_INIT(&handle->tcbpt);
    }
    return( handle );
}

/**
 *
 *********************************************************************
 *
 * Insert a the task onto the ready list by priority.
 *	For a given priority level, the task is inserted FIFO.
 *
 * \param	tcbp	pointer to the Task Control Block
 *
 * \return	none
 *
 * The following example illustrates how to resume a task that
 *	may or may not have been suspended, or to launch the task
 *	for the first time.
 *
 *	NOTE: The scheduler must have been initialized first.
 *
 \code
 #include pico.h

 #define myPrioLevel	8
 #define mytask_env		6

 int myTask(tcb_pt_t *);
 tcb_entry_t *someTaskHandle;

 void
 anyfunc(void)
 {
	tcb_entry_t t_handle;

	os_resume_task(someTaskHandle);	// resume some task

	t_handle = os_create_task(myPrioLevel, mytask_env, myTask); // create a new task
	if ((tcb_entry_t *)NULL != t_handle){
		os_resume_task(t_handle);							  // we were successful. resume it
	}
	else {
		otherwise handle the error as required here...
	}
}
 \endcode
 *
 */
void os_resume_task(tcb_entry_t *tcbp)
{
    tcb_entry_t *pReadyList;
    /*
     * remove the task from any queue it's waiting on
     * 	insert it onto the ready queue
     */
    kq_ndelete( (k_list_t *)tcbp );
    pReadyList = (tcb_entry_t *)k_ready_list.next;
    while( &k_ready_list != (k_list_t *)pReadyList )
    {
        if((pReadyList->flags & PRIOMASK) > (tcbp->flags & PRIOMASK))
        {
            break;
        }
        pReadyList = (tcb_entry_t *)pReadyList->tcb_link.next;
    }
    kq_qinsert((k_list_t *)pReadyList->tcb_link.last, (k_list_t *)tcbp);
}

/**
 *
 *********************************************************************
 *
 * Remove a task from any queue it's waiting on.
 *
 * \param  	tcbp	 pointer to the Task Control Block
 *
 * \return	none
 *
 * The following example stop a task by removing it from any queue
 *	it's waiting on.
 *
 *	NOTE: The scheduler must have been initialized.
 *
 \code
 #include pico.h

 tcb_entry_t *someTaskHandle;

 void
 anyfunc(void)
 {
	if (there's a reason to stop some task){
		os_kill_task(someTaskHandle);	// stop this task from running
	}
}
 \endcode
 *
 */
void os_kill_task(tcb_entry_t *tcbp)
{
    kq_ndelete((k_list_t *)tcbp);
}

/**
 *
 *********************************************************************
 *
 * Insert a task on the given queue.
 *
 * \param 	queue	pointer to the queue
 * \param	node   the node to insert
 *
 * \return 	none
 *
 * The following example illustrates how suspend a task to a
 *	specified queue.
 *
 *	NOTE: The scheduler must have been initialized first.
 *
 \code
 #include pico.h

 tcb_entry_t *someTaskHandle;
 k_list_t someQueue;

 void
 anyfunc(void)
 {
	os_suspend_task(&someQueue, (k_list_t *)someTaskHandle);

	pico.h also supplies a useful macro for the more likely occasions
		when the suspended task is the currently running task...

	os_suspend(someQueue);
}
 \endcode
 *
 */
void os_suspend_task(k_list_t *queue, k_list_t *node)
{
    /*
     * remove the task from any queue it's on
     *	then insert it to the given one ...
     */
    kq_ndelete(node);
    kq_qinsert(queue, node);
}

/**
 *
 *********************************************************************
 *
 * Search through the task control block structure for the first
 *	non-used tcb. In pico, a number of tcbs are statically allocated,
 *	to create a task, we need to find a free control block.
 *
 * \param	none
 *
 * \return 	tcb_entry_t *; NULL if none available
 */
tcb_entry_t *os_get_tcb(void)
{
    uint8_t index = 0;
    do
    {
        if (TCB_FREE == (tcb[index].flags & TCB_FREE))
        {
            tcb[index].flags &= ~TCB_FREE;
            return( &tcb[index] );
        }
	} while (++index < N_TASKS);
    return((tcb_entry_t *)Q_NULL );
}

/**
 *
 *********************************************************************
 *
 * Return a tcb to the task control block structure. This is useful
 *	for dynamic tasks that might be created on need, and returned
 *	on completion (spawn).
 *
 * \param 	tcbp	pointer to the Task Control Block
 *
 * \return 	none
 */
void os_release_tcb(tcb_entry_t *tcbp)
{
	os_kill_task(tcbp);
    tcbp->tcb_link.next = (k_list_t *)tcbp;
    tcbp->tcb_link.last = (k_list_t *)tcbp;
    tcbp->timer        =  0;
    tcbp->gptimer      =  0;
    tcbp->flags        = (TCB_FREE | PRIOMASK);
    tcbp->task_env     =  0;
}

/**
 *
 *********************************************************************
 *
 * Get a 'handle' for the selected task
 *
 * \param pr_addr	task function pointer
 *
 * \return 	 		pointer to the tcb; NULL if none available
 *
 * The following example illustrates getting a handle for a particular task
 *
 *	NOTE: The scheduler must have been initialized first.
 *
 \code
 #include pico.h

 int someTask(tcb_pt_t *);

 void
 anyfunc(void)
 {
	tcb_entry_t t_handle = os_get_task_handle(someTask);
	if ((tcb_pt_t *)NULL != t_handle){
		we have a good handle
	}
	else {
		handle the error if required here...
	}
}
 \endcode
 *
 */
tcb_entry_t *os_get_task_handle(int (*pr_addr)(tcb_pt_t *))
{
	uint8_t index = 0;
	do
	{
		if (TCB_FREE != (tcb[index].flags & TCB_FREE))
		{
			if (tcb[index].p_thread == pr_addr)
			{
				return (&tcb[index]);
			}
		}
	} while (++index < N_TASKS);
	return ((tcb_entry_t *)Q_NULL);
}

/**
 *
 *********************************************************************
 *
 * Return a fletcher 16 checksum for the given data block. This is a useful
 *	function generally, and can provide results approximating a 16 bit CRC.
 *
 * \param 	buf		pointer to the data block
 * \param 	len		length of the data block
 *
 * \return 	16 bit Fletcher's checksum
 */
uint16_t calc_fletcher16(uint8_t const *buf, uint16_t len)
{
	uint16_t sum1 = 0;
	uint16_t sum2 = 0;
	uint16_t index;

	for (index = 0; index < len; index++)
	{
		sum1 = (sum1 + buf[index]) % 255;
		sum2 = (sum1 + sum2) % 255;
	}
	return ((sum2 << 8) | sum1);
}

/**
 *
 *********************************************************************
 *
 * Add a function to the timer hook list. Functions are added without
 *	regard to priority (LIFO), and all functions on the list will be
 *	invoked on every timer tick.
 *
 * \param	node		is the caller's t_hook_entry_t structure
 * \param	function	is a pointer to caller's function
 *
 * \return	none
 */
void os_add_timerhook(t_hook_entry_t *node, void (*tfun_addr)(void))
{
    os_add_hook((t_hook_entry_t *)&k_thook_list, node, tfun_addr);
}

/**
 *
 *********************************************************************
 *
 * Add a function to the scheduler hook list. Functions are added without
 *	regard to priority (LIFO), and all functions on the list will be
 *	invoked at each pass through the main scheduling loop. Kernel hook
 *	functions do not have the ability to preempt tasks, but they can run
 *	more often than the timed granularity of a single tick. At the same
 *	time, while a convention task could run more often by leaving it on
 *	the ready queue, it would preempt everyone behind it.
 *
 * \param 	node		is the caller's t_hook_entry_t structure
 * \param 	function	is a pointer to the caller's function
 *
 * \return	none
 */
void os_add_schedhook(t_hook_entry_t *node, void (*tfun_addr)(void))
{
    os_add_hook((t_hook_entry_t *)&k_loop_list, node, tfun_addr);
}

/**
 *
 *********************************************************************
 *
 * This is a low level function to add a hook to any singly linked list.
 *
 * \param 	list		is the caller's t_hook_entry_t structure
 * \param	node 	    the node to insert
 * \param	function	is a pointer to the caller's function
 *
 * \return	none
 */
void os_add_hook(t_hook_entry_t *list, t_hook_entry_t *node, void (*tfun_addr)(void))
{
    node->p_timerhook  = tfun_addr;
    kq_slinsert((k_slist_t *)list, (k_slist_t *)node);
}

/**
 *
 *********************************************************************
 *
 * Remove a function from the timer hook list. This function allows an
 *	application to dynamically hook and unhook to the system timer
 *	interrupt as necessary.
 *
 * \param	node		is the caller's t_hook_entry_t structure
 *
 * \return 	none
 */
void os_release_timerhook(t_hook_entry_t *node)
{
    os_release_hook((t_hook_entry_t *)&k_thook_list, node);
}

/**
 *
 *********************************************************************
 *
 * Remove a function from the system hook list. This function allows an
 *	application to dynamically hook and unhook to the kernel main loop
 *	as necessary.
 *
 * \param	node		is the caller's t_hook_entry_t structure
 *
 * \return	none
 */
void os_release_schedhook(t_hook_entry_t *node)
{
    os_release_hook((t_hook_entry_t *)&k_loop_list, node);
}

/**
 *
 *********************************************************************
 *
 * Low level function to release a hook node from a singly linked list.
 *
 * \param 	list		is the caller's t_hook_entry_t structure
 * \param	node 	    is the node to remove
 *
 * \return	none
 */
void os_release_hook(t_hook_entry_t *list, t_hook_entry_t *node)
{
    kq_slndelete((k_slist_t *)list, (k_slist_t *)node);
}

/**
 *
 *********************************************************************
 *
 * Low level function to insert node as the next element of a queue
 *	(doubly linked list), or after a given node
 *
 * \param	queue		is a pointer to the queue
 * \param	node		is the node to insert
 *
 * \return 	none
 */
void kq_qinsert(k_list_t *queue, k_list_t *node)
{
    node->next 		   = queue->next;
    node->last 		   = queue;
    queue->next 	   = node;
    node->next->last   = node;
}

/**
 *
 *********************************************************************
 *
 * Low level function to delete and return the next element of a queue
 *	(doubly linked list). The neighbors to the deleted node are relinked,
 *	and a pointer to the deleted node is returned.
 *
 * \param	queue    	is a pointer to the queue
 * \param	node		is the node to insert
 *
 * \return 	deleted node; NULL if none
 */
k_list_t *kq_qdelete(k_list_t *queue)
{
    k_list_t *node;		/* the deleted node */
    if (queue->next == queue )
    {
        return( Q_NULL );
    }
    node 				= queue->next;
    queue->next 		= node->next;
    queue->next->last 	= queue;
    return( node );
}

/**
 *
 *********************************************************************
 *
 * Low level function to delete a node from any queue (doubly linked list)
 *	it's waiting on.
 *
 * \param	node		is the node to delete
 *
 * \return 	none
 */
void kq_ndelete(k_list_t *node)
{
    k_list_t *next_node;
    k_list_t *last_node;
    last_node 		= node->last;
    next_node 		= node->next;
    last_node->next = next_node;
    next_node->last = last_node;
    node->next 		= node->last = node;
}

/**
 *
 *********************************************************************
 *
 * Low level function to insert node as the next element of a singly
 *	linked list. The node is inserted at the end (or next item) of
 *	the list.
 *
 * \param	s_list    	is a pointer to the list
 * \param	node		is the node to insert
 *
 * \return 	none
 */
void kq_slinsert(k_slist_t *s_list, k_slist_t *node)
{
    node->next 	 = s_list->next;
    s_list->next = node;
}

/**
 *
 *********************************************************************
 *
 * Low level function to delete and return the next element of a singly
 *	linked list. kq_sldelete() traverses to the end of the list, and
 *	deletes the last node. A pointer to that node is returned.
 *
 * \param 	s_list    	is a pointer to the list
 *
 * \return 	the deleted node; NULL if none
 */
k_slist_t *kq_sldelete(k_slist_t *s_list)
{
    k_slist_t *node;		/* the deleted node */
    node = s_list->next;
    if (SL_NULL != s_list->next)
    {
        s_list->next = node->next;
    }
    return( node );
}

/**
 *
 *********************************************************************
 *
 * Low level function to delete a node from a singly linked list.
 *	kq_slndelete() traverses the list, searching for the node. Once
 *	found, the node is removed from the list.
 *
 * \param	s_list    	is a pointer to the list
 * \param	node		is the node to delete
 *
 * \return 	none
 */
void kq_slndelete(k_slist_t *s_list, k_slist_t *node)
{
    k_slist_t *next_node;
    for((next_node = s_list->next); (SL_NULL != next_node); (next_node = s_list->next))
    {
        if (next_node->next == node)
        {
            next_node->next = node->next;
            node->next      = SL_NULL;
            break;
        }
    }
}
/* @} */
/**
 *********************************************************************
 *
 * \name OS Timing Services
 * @{
 */
/**
 *
 *********************************************************************
 *
 * A task is scheduled for a time delay. The task is first removed from
 *	any queue it is waiting on, it's timer is set to the given delay value,
 *	and the timing flags are set to start timing.
 *
 * \param 	task		pointer to the Task Control Block
 * \param	delay		delay value
 *
 * \return 	none
 */

void os_delay(tcb_entry_t *task, timer_t delay)
{
    /*
     * remove the task from any queue it's on
     *	set the timer value and leave ...
     */
    kq_ndelete((k_list_t *)task);
    set_task_timer( task, delay );
    start_task_timer( task );
}

/**
 *
 *********************************************************************
 *
 * A dead delay is performed for a number of system ticks. As a note,
 *	we add one to the delay value in order to be sure we don't come
 *	up short. The net effect is that on average, we'll be 1/2 tick
 *	too long.
 *
 * \param	delay		delay value (unsigned short)
 *
 * \return 	none
 */
void os_tick_delay(uint16_t delay)
{
    timer_t		temp_tick;
    temp_tick = current_tick + delay;
    temp_tick += 1;
    do
    {
		//clr_wdt();
	} while (temp_tick != current_tick);
}

/**
 *
 *********************************************************************
 *
 * Kernel timer interrupt processing. The system tick counter (current_tick)
 *	is incremented, followed by invocation of all functions on the system
 *	timer hook list.
 *
 * \param	none
 *
 * \return 	none
 */
void os_timerHook(void)
{
    ++current_tick;
    os_hook_handler(k_thook_list);
}

/**
 *
 *********************************************************************
 *
 * Low level execute all functions on a given hook list. The list is
 *	traversed from beginning to end, executing the function provided
 *	for each entry along the way.
 *
 * \param	hooks		pointer to the singly linked hook list
 *
 * \return 	none
 */
void os_hook_handler(t_hook_entry_t *hooks)
{
    for(; SL_NULL != (k_slist_t *)hooks ; hooks = (t_hook_entry_t *)hooks->t_hook_link.next)
    {
        hooks->p_timerhook();
    }
}

/**
 *
 *********************************************************************
 *
 * calculate the time elapsed since the last call
 *
 * \param	pointer to the last tick count captured
 *
 * \return 	number of ticks elapsed
 */
timer_t os_get_elapsed_time(timer_t *last)
{
    timer_t retval = (get_os_ticks() - *last);
    *last = get_os_ticks();
	return (retval);
}

/**
 *
 *********************************************************************
 *
 * update a timer given the elapsed time
 *
 * \param	pointer to the timer, elapsed time
 *
 * \return 	none
 */
void os_update_timer(timer_t *timer, timer_t elapsed_time)
{
	if (elapsed_time >= *timer)
	{
		*timer = 0;
	}
	else
	{
		*timer -= elapsed_time;
	}
}

/**
 *
 *********************************************************************
 *
 * At each pass through the main loop we determine the time elapsed since
 *	our last entry into this function. The elapsed time is subtracted from
 *	each 'running' task timer (bounded by 0 at underflow). If a task timer
 *	reaches 0 (becomes ready), that task is inserted by priority onto the
 *	ready list.
 *
 *	Note: two timers used by the UIP or LWIP TCP/IP stacks were updated
 *		in this function call and are conditionally compiled. At the time
 *		this code was written, timer hooks were not available. Should those
 *		stacks be used in an application, a more appropriate method of timer
 *		support would be to remove the linkage / dependency on the kernel,
 *		and use a timer hook or any other method to handle.
 *
 * \param	none
 *
 * \return 	none
 */
void service_os_timers(void)
{
    uint8_t tcb_index;
	timer_t elapsed_time = os_get_elapsed_time(&last_tick);

    if (0 != elapsed_time)
    {
		os_wdt_reset();
        #ifdef USES_UIP
           if ( uip_timer > elapsed_time )
           {
               uip_timer -= elapsed_time;
           }
           else
           {
               uip_timer =  0;
           }
           if ( arp_timer > elapsed_time )
           {
               arp_timer -= elapsed_time;
           }
           else
           {
               arp_timer =  0;
           }
        #endif
        for (tcb_index = 0; tcb_index < N_TASKS; tcb_index++)
        {
            if ((0 != tcb[tcb_index].gptimer) && (NO_TIMEOUT != tcb[tcb_index].gptimer))
            {
                if ( tcb[tcb_index].gptimer > elapsed_time )
                {
                    tcb[tcb_index].gptimer -= elapsed_time;
                }
                else
                {
                   tcb[tcb_index].gptimer = TIME_EXPIRED;
                }
            }
            if ( TCB_TIMING == (tcb[tcb_index].flags & TCB_TIMINGMASK))
            {
                if ( tcb[tcb_index].timer > elapsed_time )
                {
                    tcb[tcb_index].timer -= elapsed_time;
                }
                else
                {
                    tcb[tcb_index].timer  =  TIME_EXPIRED;
                    tcb[tcb_index].flags &= ~TCB_TIMING;
                    tcb[tcb_index].flags |=  TCB_TIMEOUT;
                    kq_ndelete((k_list_t *)&tcb[tcb_index]);
                    os_resume_task(&tcb[tcb_index]);
                }
            }
        }
    }
}
/** @}
 * End pico.c
 *
 *********************************************************/
