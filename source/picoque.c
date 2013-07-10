/********************************************************************
 *
 *  DESC
 *
 *  MODULE NAME:        picoque.c
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
 ********************************************************************
 *
 **! \addtogroup pico_api
 *! @{
 *
 ********************************************************************/

#define 	PICOQ_C

/*
 ********************************************************************
 *
 *   System Includes
 */

#include	"pico.h"
#include	"picoque.h"

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

/*
 ********************************************************************
 *
 *   Constants
 */

/*
 *********************************************************
 *
 *! OS_QueInit( OS_Queue *, size, BUFFER)
 *!
 *! \param 		none.
 *!
 *!	This function will initialize a queue
 *!
 *! \return 	none.
 */
PUBLIC void
OS_QueInit(OS_Queue *q, Q_SIZE_T qsize, BYTE *buffer)
{
    q->qsize        = qsize;
    q->inptr        = 0;
    q->outptr       = 0;
    q->buff         = buffer;
}

/*
 *********************************************************
 *
 *! OS_QueAdd( OS_Queue *, Q_TYPE_T * )
 *!
 *! \param 		none.
 *!
 *!	This function will add an item to a queue
 *!
 *! \return 	status.
 */
PUBLIC BYTE
OS_QueAdd(OS_Queue *q, Q_TYPE_T *item)
{
    if(!OS_QueFull(q))
    {
        q->buff[q->inptr++] = *item;
        if (q->inptr == q->qsize)
        {
            q->inptr = 0;
        }
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
 *! OS_QuePutArray( OS_Queue *, Q_TYPE_T *, Q_SIZE_T )
 *!
 *! \param 		none.
 *!
 *!	This function will add an array to a queue
 *!
 *! \return 	number of items inserted
 */
PUBLIC Q_SIZE_T
OS_QuePutArray(OS_Queue *q, Q_TYPE_T *item, Q_SIZE_T len)
{
    Q_SIZE_T putCount;
    
    putCount = 0;
    do
    {
        if (Q_SUCCESS == OS_QueAdd(q, item))
        {
            item++;
            putCount++;
        }
        else
        {
            break;
        }
    }
    while (--len);
    return (putCount);
}

/*
 *********************************************************
 *
 *! OS_QuePutString( OS_Queue *, Q_TYPE_T * )
 *!
 *! \param 		none.
 *!
 *!	This function will add an string to a queue
 *!
 *! \return 	number of items inserted
 */
PUBLIC Q_SIZE_T
OS_QuePutString(OS_Queue *q, Q_TYPE_T *item)
{
    Q_SIZE_T putCount;
    
    putCount = 0;
    while (*item)
    {
        if (Q_SUCCESS == OS_QueAdd(q, item++))
        {
            putCount++;
        }
        else
        {
            break;
        }
    }
    return (putCount);
}

/*
 *********************************************************
 *
 *! OS_QueRemove( OS_Queue *, Q_TYPE_T *)
 *!
 *! \param 		none.
 *!
 *!	This function will remove an item from a queue
 *!
 *! \return 	status.
 */
PUBLIC BYTE
OS_QueRemove(OS_Queue *q, Q_TYPE_T *item)
{
    if(!OS_QueEmpty(q))
    {
        *item = q->buff[q->outptr++];
        if(q->outptr == q->qsize)
        {
            q->outptr = 0;
        }
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
 *! OS_QuePeek( OS_Queue *, Q_TYPE_T *)
 *!
 *! \param 		none.
 *!
 *!	extract an item, but leave it on the queue
 *!
 *! \return 	status.
 */
PUBLIC BYTE
OS_QuePeek(OS_Queue *q, Q_TYPE_T *item)
{
    if(!OS_QueEmpty(q))
    {
        *item = q->buff[q->outptr];
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
 *! OS_QueFlush( OS_Queue *)
 *!
 *! \param 		none.
 *!
 *!	This function will flush a queue
 *!
 *! \return 	none.
 */
PUBLIC void
OS_QueFlush(OS_Queue *q)
{
    q->inptr  = 0;
    q->outptr = 0;
}

/*
 * End picoque.c
 * Close the Doxygen group.
 *! @}
 *
 *********************************************************/
