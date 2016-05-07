/*
 * scancode.h - invariant IKBD scancode definitions
 *
 * This file exists to centralise the definition of invariant IKBD
 * scancodes: those scancodes which do not change, regardless of
 * language.  These were previously defined in many places.
 *
 * Copyright (c) 2016 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _SCANCODE_H
#define _SCANCODE_H

#define ESCAPE              0x011b
#define BACKSPACE           0x0e08
#define TAB                 0x0f09
#define RETURN              0x1c0d
#define DELETE              0x537f
#define ENTER               0x720d

/*
 * arrow keys
 */
#define ARROW_UP            0x4800
#define ARROW_DOWN          0x5000
#define ARROW_LEFT          0x4b00
#define ARROW_RIGHT         0x4d00

#define SHIFT_ARROW_UP      0x4838
#define SHIFT_ARROW_DOWN    0x5032
#define SHIFT_ARROW_LEFT    0x4b34
#define SHIFT_ARROW_RIGHT   0x4d36

#endif /* _SCANCODE_H */
