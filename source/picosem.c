/********************************************************************
 *
 *  DESC
 *
 *  MODULE NAME:        picosem.c
 *
 *  AUTHOR:        		Dave Sandler
 *
 *  DESCRIPTION:        This module contains the pico micro-kernel
 *						semaphore services
 *
 *  EDIT HISTORY:
 *  BASELINE
 *  VERSION     INIT    DESCRIPTION OF CHANGE
 *  --------    ----    ----------------------
 *   06-20-09   DS  	Module creation.
 *   09-27-12   DS  	OS_SemWait rewritten to use protothreads
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

/**
 *   \addtogroup pico_sem
 *	  @{
 */

/**
 * \file
 * 	pico kernel semaphore implementation.
 * \author
 * 	Dave Sandler <dsandler3@gmail.com>
 *
 */

#define                 PICOSEM_C

#include				"pico.h"
#include				"picosem.h"

/**
 *********************************************************************
 *
 * \name OS Semaphore Services
 * @{
 */

/**
 *
 *********************************************************************
 *
 * Initialize a semaphore and make it ready for use. This function
 *	must be called before using the semaphore.
 *
 * \param	sem			pointer to the semaphore
 *
 * \return	none
 *
 */
void
OS_SemInit( os_sem_t *sem )
{
    sem->SemCount = 0;
    sem->SemLink.next = sem->SemLink.last = (k_list_t *)sem;
}

/**
 *
 *********************************************************************
 *
 * Signal a semaphore. Increment the semaphore counter. If any
 *	tasks are waiting on the semaphore, resume them.
 *
 * \param	sem			pointer to the semaphore
 *
 * \return	none
 */
void
OS_SemSignal( os_sem_t *sem )
{
    if( sem->SemLink.next != ( k_list_t *)sem )
        {
            while( sem->SemLink.next != ( k_list_t *)sem )
                {
                    /*
                     * resume anyone wating on this semaphore
                     */
                    OS_ResumeTask( (tcb_entry_t *)sem->SemLink.next );
                }
        }
    /*
     * bump the counter
     */
    sem->SemCount++;
}

/**
 *
 *********************************************************************
 *
 * OS_SemWait(pt, sem, timeout) - is provided in picosem.h. As the kernel
 *	uses protothreads to implement blocking, the sem wait macro was
 *	included in the header file to be made available for use. This macro
 *	must be used at the top most level of a task. Task execution will
 *	continue if the semaphore was set, or on a timeout. The user must
 *	check for success on resumption.
 *
 * \param	pt			task protothread control structure
 * \param	sem			pointer to the semaphore
 * \param	timeout		time to wait for the semaphore
 *
 * \return  none
 */
/**
*
*********************************************************************
*
* Peek at the semaphore count. This is a useful tool to see if the
*	semaphore has been set without waiting for it.
*
* \param	sem			pointer to the semaphore
*
* \return	semaphore count value
*/
uint8_t
OS_SemPeek( os_sem_t *sem )
{
    return(sem->SemCount);
}
/** @} */
/*
 *  END OF picosem.c
 *
 ********************************************************/
