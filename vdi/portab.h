/*
 * portab.h - compatibility definitions
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _PORTAB_H
#define _PORTAB_H

/* Setup for MiNT's cross-gcc */
 
#define BYTE    char                            /* Signed byte             */
#define UBYTE   unsigned char                   /* Unsigned byte           */

#define WORD    short int                       /* Signed word (16 bits)   */
#define UWORD   unsigned short int              /* unsigned word           */

#define LONG    long                            /* signed long (32 bits)   */
#define ULONG   unsigned long                   /* Unsigned long           */

#define BOOLEAN short int                       /* 2 valued (true/false)   */

#define REG     register                        /* register variable       */
#define LOCAL   auto                            /* Local var on 68000      */
#define EXTERN  extern                          /* External variable       */
#define MLOCAL  static                          /* Local to module         */
#define GLOBAL  /**/                            /* Global variable         */
#define VOID    void                            /* Void function return    */

/****************************************************************************/
/*      Miscellaneous Definitions:                                          */
/****************************************************************************/
#define FAILURE (-1)                    /*      Function failure return val */
#define SUCCESS (0)                     /*      Function success return val */
#define YES     1                       /*      "TRUE"                      */
#define NO      0                       /*      "FALSE"                     */
#define FOREVER for(;;)                 /*      Infinite loop declaration   */
#define NULL    0                       /*      Null character value        */
#define NULLPTR (char *) 0              /*      Null pointer value          */
#define EOF     (-1)                    /*      EOF Value                   */
#define TRUE    (1)                     /*      Function TRUE  value        */
#define FALSE   (0)                     /*      Function FALSE value        */
#define STDIN    0                      /*      Standard Input              */
#define STDOUT   1                      /*      Standard Output             */
#define STDERR   2                      /*      Standard Error              */


/****************************************************************************/
/****************************************************************************/

#endif /* _PORTAB_H */
