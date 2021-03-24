/********************************************************************
 *
 *  DESC
 *
 *  MODULE NAME:        picocque.c
 *
 *  AUTHOR:        		Dave Sandler
 *
 *  DESCRIPTION:        This module contains the pico micro-kernel
 *						queue functions.
 *
 *  EDIT HISTORY:
 *  BASELINE
 *  VERSION     INIT    DESCRIPTION OF CHANGE
 *  --------    ----    ----------------------
 *   04-23-07   DS  	Module creation.
 *   09-24-12   DS  	clean up. was never used.
 *   05-21-13   DS  	greatly simplified...
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
 ********************************************************************
 *
 **! \addtogroup pico_api
 *! @{
 *
 ********************************************************************/

#define PICOCQ_C

/*
 ********************************************************************
 *
 *   System Includes
 */

#include "pico.h"
#include "picocque.h"

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
// static void increment_index(os_cqueue_t);
// static void decrement_index(os_cqueue_t);
/*
 ********************************************************************
 *
 *   Constants
 */

/**
 * \brief Helper function to check we need to wrap around the index
 *			when adding an item to the cq.
 *
 * \param cq the cq
 *
 * \return void
 */
static void cq_increment_index(os_cqueue_t *cq)
{
	// see if we need to wrap around
	if (cq->isfull)
		cq->tail = (cq->tail + 1) % cq->qsize;

	// adjust indexes
	cq->head   = (cq->head + 1) % cq->qsize;
	cq->isfull = (cq->head == cq->tail);
}

/**
 * \brief Helper function to decrement current index when removing an item
 *
 * \param cq
 *
 * \return void
 */
static void cq_deccrement_index(os_cqueue_t *cq)
{
	cq->isfull = FALSE;						 // no longer full
	cq->tail   = (cq->tail + 1) % cq->qsize; // wrap around if needed.
}

/*
 *********************************************************
 *
 *! os_que_init( os_cqueue_t *, size, BUFFER)
 *!
 *! \param 		none.
 *!
 *!	This function will initialize a queue
 *!
 *! \return 	none.
 */
void os_cque_init(os_cqueue_t *cq, cq_size_t qsize, uint8_t *buffer)
{
	cq->qsize  = qsize;
	cq->head   = 0;
	cq->tail   = 0;
	cq->buff   = buffer;
	cq->isfull = FALSE;
}

/*
 *********************************************************
 *
 *! os_que_add( os_queue_t *, q_type_t * )
 *!
 *! \param 		none.
 *!
 *!	This function will add an item to a queue
 *!
 *! \return 	status.
 */
uint8_t os_cque_add(os_cqueue_t *cq, cq_type_t *item)
{
	if (!os_cque_full(cq))
	{
		cq->buff[cq->head] = *item;
		cq_increment_index(cq);
		return (Q_SUCCESS);
	}
	else
	{
		return (Q_FULL);
	}
}

/*
 *********************************************************
 *
 *! os_que_remove( os_queue_t *, q_type_t *)
 *!
 *! \param 		none.
 *!
 *!	This function will remove an item from a queue
 *!
 *! \return 	status.
 */
uint8_t os_cque_remove(os_cqueue_t *cq, cq_type_t *item)
{
	if (!os_cque_empty(cq))
	{
		*item = cq->buff[cq->tail];
		cq_deccrement_index(cq);
		return (Q_SUCCESS);
	}
	else
	{
		return (Q_EMPTY);
	}
}

/*
 *********************************************************
 *
 *! os_que_peek( os_queue_t *, q_type_t *)
 *!
 *! \param 		none.
 *!
 *!	extract an item, but leave it on the queue
 *!
 *! \return 	status.
 */
uint8_t os_cque_peek(os_cqueue_t *cq, cq_type_t *item)
{
	if (!os_cque_empty(cq))
	{
		*item = cq->buff[cq->tail];
		return (Q_SUCCESS);
	}
	else
	{
		return (Q_EMPTY);
	}
}

/*
 *********************************************************
 *
 *! os_que_flush( os_queue_t *)
 *!
 *! \param 		none.
 *!
 *!	This function will flush a queue
 *!
 *! \return 	none.
 */
void os_cque_flush(os_cqueue_t *cq)
{
	cq->tail   = 0;
	cq->head   = 0;
	cq->isfull = 0;
}
/*
 * End picoque.c
 * Close the Doxygen group.
 *! @}
 *
 *********************************************************/
