/****************************************************************************
*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright:
*         C P / M   C   R U N   T I M E   L I B   H E A D E R   F I L E
*         -------------------------------------------------------------
*       Copyright 1982 by Digital Research Inc.  All rights reserved.
*
*       This is an include file for assisting the user to write portable
*       programs for C.
*
****************************************************************************/

/*
 *      Standard type definitions
 */
#define BYTE    signed char                     /* Signed byte             */
#define UBYTE   unsigned char                   /* Unsigned byte           */
#define BOOLEAN int                             /* 2 valued (true/false)   */
#define WORD    signed short                    /* Signed word (16 bits)   */
#define UWORD   unsigned short                  /* unsigned word           */

#define LONG    signed long                     /* signed long (32 bits)   */
#define ULONG   unsigned long                   /* Unsigned long           */

#define REG     register                        /* register variable       */
#define EXTERN  extern                          /* External variable       */
#define MLOCAL  static                          /* Local to module         */
#define GLOBAL  /**/                            /* Global variable         */
#define VOID    void                            /* Void function return    */


/****************************************************************************/
/*      Miscellaneous Definitions:                                          */
/****************************************************************************/
#define NULL    0                       /*      Null pointer value          */
#define NULLPTR ((void*)0)
#define EOF     (-1)                    /*      EOF Value                   */
#define TRUE    (1)                     /*      Function TRUE  value        */
#define FALSE   (0)                     /*      Function FALSE value        */
