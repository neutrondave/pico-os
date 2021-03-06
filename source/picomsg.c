/********************************************************************
 *
 *  DESC
 *
 *  MODULE NAME:        picomsg.c
 *
 *  AUTHOR:        		Dave Sandler
 *
 *  DESCRIPTION:        This module contains the pico micro-kernel
 *						messaging services
 *
 *  EDIT HISTORY:
 *  BASELINE
 *  VERSION     INIT    DESCRIPTION OF CHANGE
 *  --------    ----    ----------------------
 *   06-20-09   DS  	Module creation.
 *   09-28-12   DS  	OS_MsgReceive move to picomsg.h in order to
 *						use proto-threads
 *
 *  Copyright (c) 2009 - 2016 Dave Sandler
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

#define                 PICOMSG_C

#include				"pico.h"
#include				"picosem.h"
#include				"picomsg.h"
/*
 *********************************************************
 *
 * void os_mbox_init(  os_mail_t *MBox  )
 *	initialize a Mailbox
 */
void
os_mbox_init( os_mail_t *MBox )
{
    os_sem_init( &MBox->mbox_sem );
    MBox->msg_queue.next = MBox->msg_queue.last = (k_list_t *)MBox;
}

/*
 *********************************************************
 *
 * void os_msg_init(  os_msg_t *Msg  )
 *	initialize pointers in a message record
 */
void
os_msg_init( os_msg_t *Msg )
{
    Msg->msg_list.next = Msg->msg_list.last = (k_list_t *)Msg;
}

/*
 *********************************************************
 *
 * void os_msg_send(  os_msg_t *Msg, os_mail_t *MBox, os_mail_t *reply  )
 *	send a message
 */
void
os_msg_send( os_msg_t *Msg, os_mail_t *MBox )
{
    kq_qinsert( (k_list_t *)MBox, (k_list_t *)Msg );
    os_sem_signal( &MBox->mbox_sem );
}

/*
 *  END OF picomsg.c
 *
 ********************************************************/
