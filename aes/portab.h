
/****************************************************************************
*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
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
#define UCHARA 0                                /* if char is unsigned     */
#define METAWARE  0                             /* Metaware High C compiler*/

/*
 *      Standard type definitions
 */
#define BYTE    char                            /* Signed byte             */
#define BOOLEAN int                                     /* 2 valued (true/false)   */
#define WORD    short                           /* Signed word (16 bits)   */
#define UWORD   unsigned short          /* unsigned word           */

#define LONG    long                            /* signed long (32 bits)   */
#define ULONG   long                            /* Unsigned long           */

#define REG     register                                /* register variable       */
#define LOCAL   auto                            /* Local var on 68000      */
#define EXTERN  extern                          /* External variable       */
#define MLOCAL  static                          /* Local to module         */
#define GLOBAL  /**/                            /* Global variable         */

#if METAWARE                                            /* MW compiler recognizes  */
#define FAR     _far                                    /* long pointer         */
#define NEAR    _near                           /* short pointer        */
#endif

#if !METAWARE
#define FAR  /*!!!*/ 
#endif

#if 1
#define VOID    void                            /* type void                */
#else
/* If your C compiler does not support "void", use the following: */
#define VOID    /**/                            /* Void function return    */
#endif

#if UCHARA
#define UBYTE   char                            /* Unsigned byte           */
#else
#define UBYTE   unsigned char           /* Unsigned byte           */
#endif

/****************************************************************************/
/*      Miscellaneous Definitions:                                          */
/****************************************************************************/
#define FAILURE (-1)                    /*      Function failure return val */
#define SUCCESS (0)                     /*      Function success return val */
#define YES     1                       /*      "TRUE"                      */
#define NO      0                       /*      "FALSE"                     */
#define FOREVER for(;;)                 /*      Infinite loop declaration   */
#define NULL    0                       /*      Null pointer value          */
#define NULLPTR (char *) 0              /*                                  */
#define EOF     (-1)                    /*      EOF Value                   */
#define TRUE    (1)                     /*      Function TRUE  value        */
#define FALSE   (0)                     /*      Function FALSE value        */
