/********************************************************************
 * 	DESC			dtypes.h
 *
 *  MODULE NAME:
 *
 *  AUTHOR:
 *
 *  DESCRIPTION:    This header file contains prototypes and variables
 *                  	that require a scope outside of the home .C module.
 *
 *
 *  EDIT HISTORY:
 *  DATE    VERSION  INIT   DESCRIPTION OF CHANGE
 *  ------  -------  ----   ----------------------
 *  03-10-12   		DS  	Define sunrise / sunset
 *  06-15-12   		DS  	network and local calls
 *  09-19-12   		DS  	check if NULL was already defined;
 *							typically in stddef
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

#ifndef	_DTYPES_H
	#define	_DTYPES_H

	#include "compiler.h"
	/*
	 * data scope
	 */
	#define	PUBLIC
	#define	PRIVATE   static
	#define	LOCAL     static
	#define	EXTERN    extern

	/*
	 * check if we're using pico
	 *	data types with Microchip's
	 *	application libraries...
	 */
	#ifndef __GENERIC_TYPE_DEFS_H_
		/*
		 * data types
		 */
		typedef signed   int  	 INT;
		typedef unsigned int	UINT;

		/*
		 * These types are assumed as 8-bit integer
		 */
		typedef signed   char 	CHAR;
		typedef signed 	 char  	S_BYTE;
		typedef signed   char 	INT8S;
		typedef unsigned char	UCHAR;
		typedef unsigned char	BYTE;
		typedef unsigned char	INT8U;

		/*
		 * These types are assumed as 16-bit integer
		 */
		typedef signed   short	SHORT;
		typedef signed   short 	S_WORD;
		typedef signed   short 	INT16S;
		typedef unsigned short	WORD;
		typedef unsigned short	INT16U;

		/*
		 * These types are assumed as 32-bit integer
		 */
		typedef signed   long	LONG;
		typedef signed 	 long	S_LONG;
		typedef signed 	 long	INT32S;
		typedef unsigned long	DWORD;
		typedef unsigned long	INT32U;

		/*
		 * These types are assumed as 64-bit integer
		 */
		typedef signed   long long LONG64;
		typedef signed 	 long long S_QWORD;
		typedef signed 	 long long INT64S;
		typedef unsigned long long QWORD;
		typedef unsigned long long INT64U;

		typedef union
		{
		    DWORD	val;
		    BYTE	v[4];
		} Q_BYTE;

		/*
		 * Boolean type
		 */
		typedef BYTE BOOL;
		#ifndef NULL
			#define	NULL (void *)0
		#endif

		#define	TRUE			1
		#define	FALSE			0
	#endif
	typedef unsigned short	   USHORT;
	typedef unsigned long	   ULONG;
	typedef unsigned long long ULONG64;

	typedef	struct
	{
	    BYTE	Major;
	    BYTE	Minor;
	    WORD	Build;
	} VERSION;

	typedef	struct
	{
	    BYTE	Hour;
	    BYTE	Minute;
	} CLOCK_TIME;

	#define LOW(x)  (BYTE) (x & 0xFF)
	#define HIGH(x) (BYTE)((x >> 8) & 0xFF)

	#ifdef PIC32MX
		#define NVM_ALLOCATE(_SCOPE_, name, align, bytes) \
		    _SCOPE_ ROM BYTE __attribute__ ((aligned(align))) name[(bytes)] = \
		            {[0 ...(bytes)-1] = 0xFF}
	#endif

	#define PASS       		TRUE
	#define FAIL       		FALSE

	#ifndef MAX
		#define MAX(a,b)	(a >= b) ? a : b
	#endif
	#ifndef MIN
		#define MIN(a,b)	(a <= b) ? a : b
	#endif

#endif /* safety check for duplicate .h file */

/*
 *  END OF dtypes.h
 *
 *******************************************************************/
