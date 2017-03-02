/*      DESK1.C         */
/*      Routines specific to Desktop 1.x */
/*
*       Copyright 1999 Caldera Thin Clients, Inc. / Lineo Inc.
*                 2001 John Elliott
*                 2003-2016 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "string.h"
#include "intmath.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "biosdefs.h"
#include "dos.h"

#include "deskapp.h"
#include "deskfpd.h"
#include "gembind.h"
#include "deskwin.h"
#include "deskdir.h"
#include "deskbind.h"
#include "deskglob.h"
#include "aesbind.h"
#include "desksupp.h"
#include "deskfun.h"
#include "deskmain.h"
#include "desk1.h"
#include "deskins.h"
#include "kprint.h"

