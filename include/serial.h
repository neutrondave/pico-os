/********************************************************************
 * 	DESC
 *
 *  MODULE NAME:	serial.h
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
 * 5-21-13			 DS	    Creation
 *
 *  Copyright (c) 2021 Dave Sandler
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

#ifndef	_SERIAL_H
	#define	_SERIAL_H
	/*
	 **********************************************************************
	 *  Include Section
	 */
	#include "pico.h"
	#include "board.h"
	/*
	 **********************************************************************
	 *  Defines Section
	 */
	typedef struct
	{
		uint8_t      port;
		uint32_t     bitrate;
		uint8_t      parity;
		uint8_t      stop;
		uint8_t      data_ctrl;
		uint8_t      tx_prio;
		uint8_t      rx_prio;
		uint8_t      wake;
		os_queue_t  *ser_tx_q;
		os_queue_t  *ser_rx_q;
	} ser_port_info_t;

	#define	SER_PORT1		0u
	#define	SER_PORT2		1u
	#define	SER_PORT3		2u
	#define	SER_PORT4		3u

	#define	DC_TXRX_ONLY	0u
	#define	DC_TXRX_RTS		1u
	#define	DC_TXRX_RTSCTS	2u
	#define	DC_MASK		    0x0F
	#define	DC_RS485		4

	#define	DATA8_NP		0u
	#define	DATA7_NP		1u
	#define	DATA8_OP		2u
	#define	DATA8_EP		3u
	#define	DATA9_NP		4u

	#define ONE_STOP		0u
	#define TWO_STOP		1u

	#define SERPORT_WAKE	1u

	#define	SER_SET_PASS	0u
	#define	SER_BAD_PORT	1u

	/*
	 **********************************************************************
	 *  Prototypes
	 */
	#ifdef PICOSER_C
		#define _SCOPE_ 	/**/
	#else
		#define _SCOPE_ extern	/**/
	#endif

	_SCOPE_ uint8_t serial_init(ser_port_info_t *port);
	_SCOPE_ void    serial_tx_start(uint8_t port);

	#ifdef ENABLE_UART1_DRIVER
	    _SCOPE_  void   _U1RXInterrupt( void );
	    _SCOPE_  void   _U1TXInterrupt( void );
	#endif
	#ifdef ENABLE_UART2_DRIVER
	    _SCOPE_  void   _U2RXInterrupt( void );
	    _SCOPE_  void   _U2TXInterrupt( void );
	#endif
	#ifdef ENABLE_UART3_DRIVER
	    _SCOPE_  void   _U3RXInterrupt( void );
	    _SCOPE_  void   _U3TXInterrupt( void );
	#endif
	#ifdef ENABLE_UART4_DRIVER
	    _SCOPE_  void   _U4RXInterrupt( void );
	    _SCOPE_  void   _U4TXInterrupt( void );
	#endif
#endif
/*
 * end of serial.h
 *
 **********************************************************************/
