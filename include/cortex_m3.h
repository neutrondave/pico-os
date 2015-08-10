/********************************************************************
 * 	DESC
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
 *
 *
 *******************************************************************/

#ifndef	CORTEX_M3_H
#define	CORTEX_M3_H
#ifdef _C
#define _SCOPE_ /**/
#else
#define _SCOPE_ extern
#endif

/*
 ********************************************************************
 *
 *   type / data definitions
 */

/*
 ********************************************************************
 *
 *   routines exposed by this module
 */

#undef _SCOPE_
/*
 ********************************************************************
 *
 *   defines to include in every file
 */

/*
 *	Constants required to manipulate the NVIC.
 */

#define NVIC_SYSTICK_CTRL		( ( volatile Uint32_t *) 0xe000e010 )
#define NVIC_SYSTICK_LOAD		( ( volatile Uint32_t *) 0xe000e014 )
#define NVIC_INT_CTRL			( ( volatile Uint32_t *) 0xe000ed04 )
#define NVIC_SYSPRI2			( ( volatile Uint32_t *) 0xe000ed20 )
#define NVIC_SYSTICK_CLK		                      0x00000004
#define NVIC_SYSTICK_INT		                      0x00000002
#define NVIC_SYSTICK_ENABLE		                      0x00000001
#define NVIC_PENDSVSET			                      0x10000000

#endif /* safety check for duplicate .h file */

/*
 *  END OF cortex_m3
 *
 *******************************************************************/
