/*      FUNCDEF.H
*
*       Copyright 1999, Caldera Thin Clients, Inc.
*       Copyright (C) 2016-2019 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -----------------------------------------------------------
*       GEM Application Environment Services            Version 2.3
*       Serial No.  XXXX-0000-654321            All rights Reserved
*       Copyright (C) 1986                    Digital Research Inc.
*       -----------------------------------------------------------
*/
#ifndef _FUNCDEF_H
#define _FUNCDEF_H

#define OPEN_WORKSTATION        1
#define CLOSE_WORKSTATION       2
#define CLEAR_WORKSTATION       3

#define ESCAPE_FUNCTION         5
#define POLYLINE                6

#define TEXT                    8

#define SET_CHAR_HEIGHT         12

#define SET_LINE_TYPE           15
#define SET_LINE_WIDTH          16
#define SET_LINE_COLOR          17

#define SET_TEXT_COLOR          22
#define SET_FILL_INTERIOR       23
#define SET_FILL_STYLE          24
#define SET_FILL_COLOR          25

#define LOCATOR_INPUT           28

#define STRING_INPUT            31
#define SET_WRITING_MODE        32

#define SET_INPUT_MODE          33

#define INQ_TEXT_ATTRIBUTES     38

#define EXTENDED_INQUIRE        102

#define COPY_RASTER_OPAQUE      109
#define TRANSFORM_FORM          110
#define SET_CUR_FORM            111

#define SET_UD_LINE_STYLE       113
#define FILL_RECTANGLE          114

#define TIM_VECX                118

#define COPY_RASTER_TRANS       121
#define SHOW_CUR                122
#define HIDE_CUR                123
#define MOUSE_STATE             124
#define BUT_VECX                125
#define MOT_VECX                126
#define CUR_VECX                127
#define KEY_STATE               128
#define TEXT_CLIP               129

#define WHEEL_VECX              134

#endif  /* _FUNCDEF_H */
