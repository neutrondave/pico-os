/********************************************************************
 *
 *  DESC
 *
 *  MODULE NAME:        serial.c
 *
 *  AUTHOR:        		Dave Sandler
 *
 *  DESCRIPTION:        This module contains the portable serial
 *						functions.
 *
 *  EDIT HISTORY:
 *  BASELINE
 *  VERSION     INIT    DESCRIPTION OF CHANGE
 *  --------    ----    ----------------------
 *   05-21-13   DS  	Module creation.
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

/*
 ********************************************************************
 *
 *   System Includes
 */
#define   PICOSER_C
#include "HardwareProfile.h"
#include "pico.h"
#include "picoque.h"
#include "portable.h"
#include "serial.h"
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
#ifdef ENABLE_UART1_DRIVER
    static os_queue_t *Rx1Que;
    static os_queue_t *Tx1Que;
#endif
#ifdef ENABLE_UART2_DRIVER
    static os_queue_t *Rx2Que;
    static os_queue_t *Tx2Que;
#endif
#ifdef ENABLE_UART3_DRIVER
    static os_queue_t *Rx3Que;
    static os_queue_t *Tx3Que;
#endif
#ifdef ENABLE_UART4_DRIVER
    static os_queue_t *Rx4Que;
    static os_queue_t *Tx4Que;
#endif

/*
 ********************************************************************
 *
 *   Prototypes
 */
 uint8_t UARTInit(SerPortInfo *portInfo);

#ifdef ENABLE_UART1_DRIVER
      void   _U1RXInterrupt( void );
      void   _U1TXInterrupt( void );
#endif
#ifdef ENABLE_UART2_DRIVER
      void   _U2RXInterrupt( void );
      void   _U2TXInterrupt( void );
#endif
#ifdef ENABLE_UART3_DRIVER
      void   _U3RXInterrupt( void );
      void   _U3TXInterrupt( void );
#endif
#ifdef ENABLE_UART4_DRIVER
      void   _U4RXInterrupt( void );
      void   _U4TXInterrupt( void );
#endif

/*
 ********************************************************************
 *
 *   External Procedures
 */

/*
 ********************************************************************
 *
 *   Module Data
 */
/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   SerialInit
 *
 *  DESCRIPTION:    initialize a UART
 *
 *  INPUT:			port, baud rate, parity / stop, TxPRIO, RxPRIO
 *
 *  OUTPUT:			result
 *
 *******************************************************************/
uint8_t 
SerialInit(SerPortInfo *portInfo)
{
    uint8_t retval;
    uint16_t BaudDivide;
    
    retval = SER_SET_PASS;
    /*
     * calculate the baud rate divider
     *  for BRGH = 0
     */
    BaudDivide = (CPU_CLOCK_HZ / (16 * portInfo->bitrate)) - 1;
    
    switch (portInfo->port)
    {
        case SER_PORT1:
            #ifdef ENABLE_UART1_DRIVER
                IPC2bits.U1RXIP    = portInfo->RxPrio;
                IPC3bits.U1TXIP    = portInfo->TxPrio;
                U1BRG              = BaudDivide;
                U1MODE             = 0;
                U1MODEbits.UEN     = (portInfo->DataCtrl & DC_MASK);
                U1MODEbits.PDSEL   = portInfo->parity;
                U1MODEbits.STSEL   = portInfo->stop;
                U1MODEbits.WAKE    = portInfo->wake;
                U1MODEbits.UARTEN  = 1;
                U1STA              = 0;
                U1STAbits.UTXEN    = 1;
	            IEC0bits.U1RXIE    = 1;
	            IEC0bits.U1TXIE    = 0;
                Tx1Que             = portInfo->serTxQ; 
                Rx1Que             = portInfo->serRxQ; 
            #else
                retval = SER_BAD_PORT;
            #endif
            break;
        case SER_PORT2:
            #ifdef ENABLE_UART2_DRIVER
                IPC7bits.U2RXIP    = portInfo->RxPrio;
                IPC7bits.U2TXIP    = portInfo->TxPrio;
                U2BRG              = BaudDivide;
                U2MODE             = 0;
                U2MODEbits.UEN     = (portInfo->DataCtrl & DC_MASK);
                U2MODEbits.PDSEL   = portInfo->parity;
                U2MODEbits.STSEL   = portInfo->stop;
                U2MODEbits.WAKE    = portInfo->wake;
                U2MODEbits.UARTEN  = 1;
                U2STA              = 0;
                U2STAbits.UTXEN    = 1;
	            IEC1bits.U2RXIE    = 1;
	            IEC1bits.U2TXIE    = 0;
                Tx2Que             = portInfo->serTxQ; 
                Rx2Que             = portInfo->serRxQ; 
            #else
                retval = SER_BAD_PORT;
            #endif
            break;
        case SER_PORT3:
            #ifdef ENABLE_UART3_DRIVER
                IPC20bits.U3RXIP   = portInfo->RxPrio;
                IPC20bits.U3TXIP   = portInfo->TxPrio;
                U3BRG              = BaudDivide;
                U3MODE             = 0;
                U3MODEbits.UEN     = (portInfo->DataCtrl & DC_MASK);
                U3MODEbits.PDSEL   = portInfo->parity;
                U3MODEbits.STSEL   = portInfo->stop;
                U3MODEbits.WAKE    = portInfo->wake;
                U3MODEbits.UARTEN  = 1;
                U3STA              = 0;
                U3STAbits.UTXEN    = 1;
	            IEC5bits.U3RXIE    = 1;
	            IEC5bits.U3TXIE    = 0;
                Tx3Que             = portInfo->serTxQ; 
                Rx3Que             = portInfo->serRxQ; 
            #else
                retval = SER_BAD_PORT;
            #endif
            break;
        case SER_PORT4:
            #ifdef ENABLE_UART4_DRIVER
                IPC22bits.U4RXIP   = portInfo->RxPrio;
                IPC22bits.U4TXIP   = portInfo->TxPrio;
                U4BRG              = BaudDivide;
                U4MODE             = 0;
                U4MODEbits.UEN     = (portInfo->DataCtrl & DC_MASK);
                U4MODEbits.PDSEL   = portInfo->parity;
                U4MODEbits.STSEL   = portInfo->stop;
                U4MODEbits.WAKE    = portInfo->wake;
                U4MODEbits.UARTEN  = 1;
                U4STA              = 0;
                U4STAbits.UTXEN    = 1;
	            IEC5bits.U4RXIE    = 1;
	            IEC5bits.U4TXIE    = 0;
                Tx4Que             = portInfo->serTxQ; 
                Rx4Que             = portInfo->serRxQ; 
            #else
                retval = SER_BAD_PORT;
            #endif
            break;
        default:
            retval = SER_BAD_PORT;
            break;
    }
    return (retval);
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   SerialTxStart
 *
 *  DESCRIPTION:    start transmitting
 *
 *  INPUT:			port
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void 
SerialTxStart(uint8_t port)
{
    switch (port)
    {
        case SER_PORT1:
            #ifdef ENABLE_UART1_DRIVER
                if(U1STAbits.TRMT)
                {
	                IFS0bits.U1TXIF = 1;
                }   
	            IEC0bits.U1TXIE     = 1;
            #endif
            break;
        case SER_PORT2:
            #ifdef ENABLE_UART2_DRIVER
                if(U2STAbits.TRMT)
                {
	                IFS1bits.U2TXIF = 1;
                }   
	            IEC1bits.U2TXIE     = 1;
            #endif
            break;
        case SER_PORT3:
            #ifdef ENABLE_UART3_DRIVER
                if(U3STAbits.TRMT)
                {
    	            IFS5bits.U3TXIF = 1;
                }   
	            IEC5bits.U3TXIE     = 1;
            #endif
            break;
        case SER_PORT4:
            #ifdef ENABLE_UART4_DRIVER
                if(U4STAbits.TRMT)
                {
    	            IFS5bits.U4TXIF = 1;
                }   
	            IEC5bits.U4TXIE     = 1;
            #endif
            break;
        default:
            break;
    }
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U1RxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 1
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void
__attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
    uint16_t temp;
    /*
     * clear interrupt flag
     */
    IFS0bits.U1RXIF = 0;
    #ifdef ENABLE_UART1_DRIVER
	    while (U1STAbits.URXDA)
	    {
		    temp = U1RXREG;
		    if((U1STA & 0x0E) == 0)
	        {						
	            /*
	             * no parity, framing or overrun 
	             *  error set on this character
	             *  read the character
	             */
                os_que_put(Rx1Que, temp);
		    }
	        else
	        {	
	            /*
	             * error in this character
	             *  read the character
	             */
			    if(U1STAbits.FERR)
	            {	
	                /*
	                 * overrun error set on this character
	                 *  clear overflow bit and empty receiver
	                 */
				    U1STAbits.OERR = 0;
			    }
		    }
	    }
    #else
        IEC0bits.U1RXIE = 0;
    #endif    
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U1TxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 1
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void 
__attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
    q_type_t temp;

    IFS0bits.U1TXIF = 0;
    #ifdef ENABLE_UART1_DRIVER
        if (!os_que_empty(Tx1Que))
        {
            os_que_get(Tx1Que, temp);        
            U1TXREG = temp;
        }
        else
        {
		    IEC0bits.U1TXIE = 0;
	    }
    #else
	    IEC0bits.U1TXIE = 0;
    #endif    
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U2RxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 2
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void
__attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
    uint16_t temp;
    /*
     * clear interrupt flag
     */
    IFS1bits.U2RXIF = 0;
    #ifdef ENABLE_UART2_DRIVER
	    while (U2STAbits.URXDA)
	    {
		    temp = U2RXREG;
		    if((U2STA & 0x0E) == 0)
	        {						
	            /*
	             * no parity, framing or overrun 
	             *  error set on this character
	             *  read the character
	             */
                os_que_put(Rx2Que, temp);
		    }
	        else
	        {	
	            /*
	             * error in this character
	             *  read the character
	             */
			    if(U2STAbits.FERR)
	            {	
	                /*
	                 * overrun error set on this character
	                 *  clear overflow bit and empty receiver
	                 */
				    U2STAbits.OERR = 0;
			    }
		    }
	    }
    #else
        IEC1bits.U2RXIE = 0;
    #endif    
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U2TxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 2
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void 
__attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
    q_type_t temp;

    IFS1bits.U2TXIF = 0;
    #ifdef ENABLE_UART2_DRIVER
        if (!os_que_empty(Tx2Que))
        {
            os_que_get(Tx2Que, temp);        
            U2TXREG = temp;
        }
        else
        {
		    IEC1bits.U2TXIE = 0;
	    }
    #else
	    IEC1bits.U2TXIE = 0;
    #endif    
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U3RxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 3
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void
__attribute__((interrupt, no_auto_psv)) _U3RXInterrupt(void)
{
    uint16_t temp;
    /*
     * clear interrupt flag
     */
    IFS5bits.U3RXIF = 0;
    #ifdef ENABLE_UART3_DRIVER
	    while (U3STAbits.URXDA)
	    {
		    temp = U3RXREG;
		    if((U3STA & 0x0E) == 0)
	        {						
	            /*
	             * no parity, framing or overrun 
	             *  error set on this character
	             *  read the character
	             */
                os_que_put(Rx3Que, temp);
		    }
	        else
	        {	
	            /*
	             * error in this character
	             *  read the character
	             */
			    if(U3STAbits.FERR)
	            {	
	                /*
	                 * overrun error set on this character
	                 *  clear overflow bit and empty receiver
	                 */
				    U3STAbits.OERR = 0;
			    }
		    }
	    }
    #else
        IEC5bits.U3RXIE = 0;
    #endif    
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U3TxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 3
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void 
__attribute__((interrupt, no_auto_psv)) _U3TXInterrupt(void)
{
    q_type_t temp;

    IFS5bits.U3TXIF = 0;
    #ifdef ENABLE_UART3_DRIVER
        if (!os_que_empty(Tx3Que))
        {
            os_que_get(Tx3Que, temp);        
            U3TXREG = temp;
        }
        else
        {
		    IEC5bits.U3TXIE = 0;
	    }
    #else
	    IEC5bits.U3TXIE = 0;
    #endif    
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U4RxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 4
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void
__attribute__((interrupt, no_auto_psv)) _U4RXInterrupt(void)
{
    uint16_t temp;
    /*
     * clear interrupt flag
     */
    IFS5bits.U4RXIF = 0;
    #ifdef ENABLE_UART4_DRIVER
	    while (U4STAbits.URXDA)
	    {
		    temp = U4RXREG;
		    if((U4STA & 0x0E) == 0)
	        {						
	            /*
	             * no parity, framing or overrun 
	             *  error set on this character
	             *  read the character
	             */
                os_que_put(Rx4Que, temp);
		    }
	        else
	        {	
	            /*
	             * error in this character
	             *  read the character
	             */
			    if(U4STAbits.FERR)
	            {	
	                /*
	                 * overrun error set on this character
	                 *  clear overflow bit and empty receiver
	                 */
				    U4STAbits.OERR = 0;
			    }
		    }
	    }
    #else
        IEC5bits.U4RXIE = 0;
    #endif    
}

/********************************************************************
 *  DESC
 *
 *  ROUTINE NAME:   _U4TxInterrupt
 *
 *  DESCRIPTION:    serial interrupt for UART 4
 *
 *  INPUT:			none
 *
 *  OUTPUT:			none
 *
 *******************************************************************/
void 
__attribute__((interrupt, no_auto_psv)) _U4TXInterrupt(void)
{
    q_type_t temp;

    IFS5bits.U4TXIF = 0;
    #ifdef ENABLE_UART4_DRIVER
        if (!os_que_empty(Tx4Que))
        {
            os_que_get(Tx4Que, temp);        
            U4TXREG = temp;
        }
        else
        {
		    IEC5bits.U4TXIE = 0;
	    }
    #else
	    IEC5bits.U4TXIE = 0;
    #endif    
}
/*
 *  END OF serial.c
 *
 *******************************************************************/
