/*
 * aesdefs.h - Public definitions for AES system calls
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _AESDEFS_H
#define _AESDEFS_H

/*
 * values for shel_write() 'doex' arg
 */
#define SHW_NOEXEC      0       /* just return to desktop */
#define SHW_EXEC        1       /* run another program after this */
#define SHW_SHUTDOWN    4       /* shutdown system */
#define SHW_RESCHNG     5       /* change resolution */

#endif /* _AESDEFS_H */
