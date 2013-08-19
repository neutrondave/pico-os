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
 *   08-21-11   DS  	added OS_TickDelay for dead delays
 *   09-15-11   DS  	added single link list functions
 *						OS tick interrupt timer hook
 *   03-22-12   DS  	added pico scheduling loop hook
 *   09-19-12   DS  	single switch USES_UIP for the UIP TCP/IP stack
 *							as opposed to processor specific switching
 *						reworked ServiceOSTimers. changed the do{}while to
 *							a for loop for readability; also, we'll handle
 *							timer updates only when time's elapsed.
 *   09-28-12   DS  	doxygen documentation support
 *   05-30-13   DS  	fix OS_TickDelay(). immediate fall through meant no delay.
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

LOCAL 	K_LIST       k_ready_list;	/* !< list of tasks ready to run	                */
LOCAL	K_LIST		 k_wait_list;	/* !< list of tasks waiting to run	                */
LOCAL	Thook_Entry *k_thook_list;	/* !< list of functions to be executed every tick	*/
LOCAL	Thook_Entry *k_loop_list;	/* !< list of tasks executed every kernel pass      */
LOCAL 	TCB_Entry    TCB[N_TASKS];	/* !< pico taskc ontrol blocks						*/
LOCAL	TIMER_T		 LastTick;		/* !< last captured timer value 					*/
/** @} */
/*
 *********************************************************************
 *
 *   Prototypes
 */

EXTERN 		void	SetupTickInterrupt(void);

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
 *	Functionally, OS_StartSched executes forever, updating timers in
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
	OS_Init();
		.
		.
	OS_StartSched();
	return(-1);
}
 \endcode
 *
 */
void
OS_StartSched( void )
{
    FOREVER
    {
        ServiceOSTimers();
        OS_HookHandler(k_loop_list);

        if( &k_ready_list != k_ready_list.next )
        {
            CurTask = (TCB_Entry *)k_ready_list.next;
            CurTask->pThread(&(CurTask->TCBpt));
        }
    }
}

/**
 *
 *********************************************************************
 *
 * kernel data structures are initialized. OS_Init() must be called
 *	before executing the kernel main loop. Functionally, OS_Init()
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
	    OS_Init();
		    .
		    .
	    OS_StartSched();
	    return(-1);
    }
    \endcode
 *
 */
void
OS_Init( void )
{
    BYTE index = 0;
    /*
     * initialize the target's tick hardware
     */
    SetupTickInterrupt();
    k_ready_list.next = k_ready_list.last = &k_ready_list;
    k_wait_list.next  = k_wait_list.last  = &k_wait_list;
    k_thook_list	  = (Thook_Entry *)SL_NULL;
    k_loop_list	      = (Thook_Entry *)SL_NULL;
    LastTick          = getSysTick();
    do
    {
        OS_ReleaseTCB(&TCB[index]);
    }
    while ( ++index < N_TASKS );
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
 * \return 	 		pointer to the TCB; NULL if none available
 *
 * The following example illustrates the allocation and creation of a
 *	pico TASK block.
 *
 *	NOTE: The scheduler must have been initialized first.
 *
 \code
 #include pico.h

 #define myPrioLevel	8
 #define myTaskEnv		6

 int myTask(TCB_PT *);

 void
 anyfunc(void)
 {
	TCB_Entry t_handle;
	t_handle = OS_CreateTask(myPrioLevel, myTaskEnv, myTask);
	if ((TCB_PT *)NULL != t_handle){
		task created successfully...
	}
	else {
		handle the error if required here...
	}
}
 \endcode
 *
 */
TCB_Entry *
OS_CreateTask( BYTE prio, BYTE env, int (*pr_addr)(TCB_PT *))
{
    TCB_Entry *handle;
    handle = OS_GetTCB();
    if ((TCB_Entry *)Q_NULL != handle)
    {
        handle->Flags    = (prio & PRIOMASK);
        handle->TaskEnv  =  env;
        handle->pThread  =  pr_addr;
        PT_INIT(&handle->TCBpt);
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
 #define myTaskEnv		6

 int myTask(TCB_PT *);
 TCB_Entry *someTaskHandle;

 void
 anyfunc(void)
 {
	TCB_Entry t_handle;

	OS_ResumeTask(someTaskHandle);	// resume some task

	t_handle = OS_CreateTask(myPrioLevel, myTaskEnv, myTask); // create a new task
	if ((TCB_Entry *)NULL != t_handle){
		OS_ResumeTask(t_handle);							  // we were successful. resume it
	}
	else {
		otherwise handle the error as required here...
	}
}
 \endcode
 *
 */
void
OS_ResumeTask( TCB_Entry *tcbp )
{
    TCB_Entry *pReadyList;
    /*
     * remove the task from any queue it's waiting on
     * 	insert it onto the ready queue
     */
    KQ_ndelete( (K_LIST *)tcbp );
    pReadyList = (TCB_Entry *)k_ready_list.next;
    while( &k_ready_list != (K_LIST *)pReadyList )
    {
        if((pReadyList->Flags & PRIOMASK) > (tcbp->Flags & PRIOMASK))
        {
            break;
        }
        pReadyList = (TCB_Entry *)pReadyList->TcbLink.next;
    }
    KQ_qinsert((K_LIST *)pReadyList->TcbLink.last, (K_LIST *)tcbp);
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

 TCB_Entry *someTaskHandle;

 void
 anyfunc(void)
 {
	if (there's a reason to stop some task){
		OS_KillTask(someTaskHandle);	// stop this task from running
	}
}
 \endcode
 *
 */
void
OS_KillTask( TCB_Entry *tcbp )
{
    KQ_ndelete((K_LIST *)tcbp);
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

 TCB_Entry *someTaskHandle;
 K_LIST someQueue;

 void
 anyfunc(void)
 {
	OS_SuspendTask(&someQueue, (K_LIST *)someTaskHandle);

	pico.h also supplies a useful macro for the more likely occasions
		when the suspended task is the currently running task...

	OS_Suspend(someQueue);
}
 \endcode
 *
 */
void
OS_SuspendTask( K_LIST *queue, K_LIST *node )
{
    /*
     * remove the task from any queue it's on
     *	then insert it to the given one ...
     */
    KQ_ndelete(node);
    KQ_qinsert(queue, node);
}

/**
 *
 *********************************************************************
 *
 * Search through the task control block structure for the first
 *	non-used TCB. In pico, a number of TCBs are statically allocated,
 *	to create a task, we need to find a free control block.
 *
 * \param	none
 *
 * \return 	TCB_Entry *; NULL if none available
 */
TCB_Entry *
OS_GetTCB( void )
{
    BYTE index = 0;
    do
    {
        if (TCB_FREE == (TCB[index].Flags & TCB_FREE))
        {
            TCB[index].Flags &= ~TCB_FREE;
            return( &TCB[index] );
        }
    }
    while ( ++index < N_TASKS );
    return((TCB_Entry *)Q_NULL );
}

/**
 *
 *********************************************************************
 *
 * Return a TCB to the task control block structure. This is useful
 *	for dynamic tasks that might be created on need, and returned
 *	on completion (spawn).
 *
 * \param 	tcbp	pointer to the Task Control Block
 *
 * \return 	none
 */
void
OS_ReleaseTCB( TCB_Entry *tcbp )
{
    tcbp->TcbLink.next = (K_LIST *)tcbp;
    tcbp->TcbLink.last = (K_LIST *)tcbp;
    tcbp->Timer        =  0;
    tcbp->gpTimer      =  0;
    tcbp->Flags        = (TCB_FREE | PRIOMASK);
    tcbp->TaskEnv      =  0;
}

/**
 *
 *********************************************************************
 *
 * Add a function to the timer hook list. Functions are added without
 *	regard to priority (LIFO), and all functions on the list will be
 *	invoked on every timer tick.
 *
 * \param	node		is the caller's Thook_Entry structure
 * \param	function	is a pointer to caller's function
 *
 * \return	none
 */
void
OS_AddTimerHook( Thook_Entry *node, void (*tfun_addr)(void) )
{
    OS_AddHook((Thook_Entry *)&k_thook_list, node, tfun_addr);
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
 * \param 	node		is the caller's Thook_Entry structure
 * \param 	function	is a pointer to the caller's function
 *
 * \return	none
 */
void
OS_AddSchedHook( Thook_Entry *node, void (*tfun_addr)(void) )
{
    OS_AddHook((Thook_Entry *)&k_loop_list, node, tfun_addr);
}

/**
 *
 *********************************************************************
 *
 * This is a low level function to add a hook to any singly linked list.
 *
 * \param 	list		is the caller's Thook_Entry structure
 * \param	node 	    the node to insert
 * \param	function	is a pointer to the caller's function
 *
 * \return	none
 */
void
OS_AddHook( Thook_Entry *list, Thook_Entry *node, void (*tfun_addr)(void) )
{
    node->Thookfun  = tfun_addr;
    KQ_slinsert((K_SLIST *)list, (K_SLIST *)node);
}

/**
 *
 *********************************************************************
 *
 * Remove a function from the timer hook list. This function allows an
 *	application to dynamically hook and unhook to the system timer
 *	interrupt as necessary.
 *
 * \param	node		is the caller's Thook_Entry structure
 *
 * \return 	none
 */
void
OS_ReleaseTimerHook( Thook_Entry *node )
{
    OS_ReleaseHook((Thook_Entry *)&k_thook_list, node);
}

/**
 *
 *********************************************************************
 *
 * Remove a function from the system hook list. This function allows an
 *	application to dynamically hook and unhook to the kernel main loop
 *	as necessary.
 *
 * \param	node		is the caller's Thook_Entry structure
 *
 * \return	none
 */
void
OS_ReleaseSchedHook( Thook_Entry *node )
{
    OS_ReleaseHook((Thook_Entry *)&k_loop_list, node);
}

/**
 *
 *********************************************************************
 *
 * Low level function to release a hook node from a singly linked list.
 *
 * \param 	list		is the caller's Thook_Entry structure
 * \param	node 	    is the node to remove
 *
 * \return	none
 */
void
OS_ReleaseHook( Thook_Entry *list, Thook_Entry *node )
{
    KQ_slndelete((K_SLIST *)list, (K_SLIST *)node);
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
void
KQ_qinsert( K_LIST *queue, K_LIST *node )
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
K_LIST *
KQ_qdelete( K_LIST *queue )
{
    K_LIST *node;		/* the deleted node */
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
void
KQ_ndelete( K_LIST *node )
{
    K_LIST *next_node;
    K_LIST *last_node;
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
void
KQ_slinsert( K_SLIST *s_list, K_SLIST *node )
{
    node->next 	 = s_list->next;
    s_list->next = node;
}

/**
 *
 *********************************************************************
 *
 * Low level function to delete and return the next element of a singly
 *	linked list. KQ_sldelete() traverses to the end of the list, and
 *	deletes the last node. A pointer to that node is returned.
 *
 * \param 	s_list    	is a pointer to the list
 *
 * \return 	the deleted node; NULL if none
 */
K_SLIST *
KQ_sldelete( K_SLIST *s_list )
{
    K_SLIST *node;		/* the deleted node */
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
 *	KQ_slndelete() traverses the list, searching for the node. Once
 *	found, the node is removed from the list.
 *
 * \param	s_list    	is a pointer to the list
 * \param	node		is the node to delete
 *
 * \return 	none
 */
void
KQ_slndelete( K_SLIST *s_list, K_SLIST *node )
{
    K_SLIST *next_node;
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

void
OS_Delay( TCB_Entry *task, TIMER_T delay )
{
    /*
     * remove the task from any queue it's on
     *	set the timer value and leave ...
     */
    KQ_ndelete((K_LIST *)task);
    setTaskTimer( task, delay );
    startTaskTimer( task );
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
void
OS_TickDelay( WORD delay )
{
    TIMER_T		TempTick;
    TempTick = CurrentTick + delay;
    TempTick += 1;
    do
    {
        ClrWdt();
    }
    while(TempTick != CurrentTick);
}

/**
 *
 *********************************************************************
 *
 * Kernel timer interrupt processing. The system tick counter (CurrentTick)
 *	is incremented, followed by invocation of all functions on the system
 *	timer hook list.
 *
 * \param	none
 *
 * \return 	none
 */
void
OS_TimerHook( void )
{
    ++CurrentTick;
    OS_HookHandler(k_thook_list);
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
void
OS_HookHandler( Thook_Entry *hooks )
{
    for(; SL_NULL != (K_SLIST *)hooks ; hooks = (Thook_Entry *)hooks->ThookLink.next)
    {
        hooks->Thookfun();
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
void
ServiceOSTimers( void )
{
    BYTE 		TcbIndex;
    TIMER_T 	temp_t;
    TIMER_T 	ElapsedTime;
    temp_t 		=  getSysTick();
    ElapsedTime = (temp_t - LastTick);
    LastTick 	=  temp_t;
    if (0 != ElapsedTime)
    {
        #ifdef USES_UIP
           if ( UIP_Timer > ElapsedTime )
           {
               UIP_Timer -= ElapsedTime;
           }
           else
           {
               UIP_Timer =  0;
           }
           if ( ARP_Timer > ElapsedTime )
           {
               ARP_Timer -= ElapsedTime;
           }
           else
           {
               ARP_Timer =  0;
           }
        #endif
        for (TcbIndex = 0; TcbIndex < N_TASKS; TcbIndex++)
        {
            if ((0 != TCB[TcbIndex].gpTimer) && (NO_TIMEOUT != TCB[TcbIndex].gpTimer))
            {
                if ( TCB[TcbIndex].gpTimer > ElapsedTime )
                {
                    TCB[TcbIndex].gpTimer -= ElapsedTime;
                }
                else
                {
                   TCB[TcbIndex].gpTimer = TIME_EXPIRED;
                }
            }
            if ( TCB_TIMING == (TCB[TcbIndex].Flags & TCB_TIMINGMASK))
            {
                if ( TCB[TcbIndex].Timer > ElapsedTime )
                {
                    TCB[TcbIndex].Timer -= ElapsedTime;
                }
                else
                {
                    TCB[TcbIndex].Timer  =  TIME_EXPIRED;
                    TCB[TcbIndex].Flags &= ~TCB_TIMING;
                    TCB[TcbIndex].Flags |=  TCB_TIMEOUT;
                    KQ_ndelete((K_LIST *)&TCB[TcbIndex]);
                    OS_ResumeTask(&TCB[TcbIndex]);
                }
            }
        }
    }
}
/** @} */
/** @}
 * End pico.c
 *
 *********************************************************/
