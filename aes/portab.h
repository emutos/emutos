
/****************************************************************************
*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright     
*                            
*         C P / M   C   R U N   T I M E   L I B   H E A D E R   F I L E
*         -------------------------------------------------------------
*       Copyright 1982 by Digital Research Inc.  All rights reserved.
*
*       This is an include file for assisting the user to write portable
*       programs for C.
*
****************************************************************************/

#define mc68k 1

/*
 *      Standard type definitions
 */
#define BYTE    char                            /* Signed byte             */
#define UBYTE   unsigned char                   /* Unsigned byte           */
#define BOOLEAN int                             /* 2 valued (true/false)   */
#define WORD    short                           /* Signed word (16 bits)   */
#define UWORD   unsigned short                  /* unsigned word           */

#define LONG    long                            /* signed long (32 bits)   */
#define ULONG   long                            /* Unsigned long           */

#define REG     register                        /* register variable       */
#define LOCAL   auto                            /* Local var on 68000      */
#define EXTERN  extern                          /* External variable       */
#define MLOCAL  static                          /* Local to module         */
#define GLOBAL  /**/                            /* Global variable         */

#define FAR     /**/ 


#define VOID    void                            /* type void                */

/****************************************************************************/
/*      Miscellaneous Definitions:                                          */
/****************************************************************************/
#define NULL    0                       /*      Null pointer value          */
#define NULLPTR (void *) 0
#define TRUE    (1)                     /*      Function TRUE  value        */
#define FALSE   (0)                     /*      Function FALSE value        */
