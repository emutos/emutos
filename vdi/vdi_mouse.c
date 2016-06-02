/*
 * vdi_mouse.c
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2016 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "asm.h"
#include "xbiosbind.h"
#include "vdi_defs.h"
#include "../bios/tosvars.h"
#include "../bios/lineavars.h"
#include "kprint.h"



/* mouse related vectors (linea variables in bios/lineavars.S) */

extern void     (*user_but)(void);      /* user button vector */
extern void     (*user_cur)(void);      /* user cursor vector */
extern void     (*user_mot)(void);      /* user motion vector */

/* call the vectors from C */
extern void call_user_but(WORD status);
extern void call_user_wheel(WORD wheel_number, WORD wheel_amount);

/* Mouse / sprite structure */
typedef struct Mcdb_ Mcdb;
struct Mcdb_ {
        WORD    xhot;
        WORD    yhot;
        WORD    planes;
        WORD    bg_col;
        WORD    fg_col;
        UWORD   mask[16];
        UWORD   data[16];
};

/* prototypes */
static void cur_display(Mcdb *sprite, MCS *savebuf, WORD x, WORD y);
static void cur_replace(MCS *savebuf);
static void vb_draw(void);             /* user button vector */

extern void mouse_int(void);    /* mouse interrupt routine */
extern void wheel_int(void);    /* wheel interrupt routine */
extern void mov_cur(void);      /* user button vector */

/* global line-A storage area for mouse form definition */
extern Mcdb mouse_cdb;

extern WORD HIDE_CNT;
extern WORD MOUSE_BT;
extern WORD GCURX, GCURY;


/* FIXME: should go to linea variables */
void     (*user_wheel)(void);   /* user provided mouse wheel vector */
PFVOID old_statvec;             /* original IKBD status packet routine */




/* Default Mouse Cursor Definition */
static const Mcdb arrow_cdb = {
    1, 0, 1, 0, 1,
    /* background definition */
    {
        0xE000, /* %1110000000000000 */
        0xF000, /* %1111000000000000 */
        0xF800, /* %1111100000000000 */
        0xFC00, /* %1111110000000000 */
        0xFE00, /* %1111111000000000 */
        0xFF00, /* %1111111100000000 */
        0xFF80, /* %1111111110000000 */
        0xFFC0, /* %1111111111000000 */
        0xFE00, /* %1111111000000000 */
        0xFE00, /* %1111111000000000 */
        0xEF00, /* %1110111100000000 */
        0x0F00, /* %0000111100000000 */
        0x0780, /* %0000011110000000 */
        0x0780, /* %0000011110000000 */
        0x03C0, /* %0000001111000000 */
        0x0000  /* %0000000000000000 */
    },
    /* foreground definition */
    {
        0x4000, /* %0100000000000000 */
        0x6000, /* %0110000000000000 */
        0x7000, /* %0111000000000000 */
        0x7800, /* %0111100000000000 */
        0x7C00, /* %0111110000000000 */
        0x7E00, /* %0111111000000000 */
        0x7F00, /* %0111111100000000 */
        0x7F80, /* %0111111110000000 */
        0x7C00, /* %0111110000000000 */
        0x6C00, /* %0110110000000000 */
        0x4600, /* %0100011000000000 */
        0x0600, /* %0000011000000000 */
        0x0300, /* %0000001100000000 */
        0x0300, /* %0000001100000000 */
        0x0180, /* %0000000110000000 */
        0x0000  /* %0000000000000000 */
    }
};

/*
 * do_nothing - doesn't do much  :-)
 */

static void do_nothing(void)
{
}



/*
 * dis_cur - Displays the mouse cursor if the number of hide
 *           operations has gone back to 0.
 *
 *  Decrement the counter for the number of hide operations performed.
 *  If this is not the last one then do nothing because the cursor
 *  should remain hidden.
 *
 *   Outputs:
 *      hide_cnt = hide_cnt - 1
 *      draw_flag = 0
 */

static void dis_cur(void)
{
    mouse_flag += 1;            /* disable mouse redrawing */
    HIDE_CNT -= 1;              /* decrement hide operations counter */
    if (HIDE_CNT == 0) {
        cur_display(&mouse_cdb, mcs_ptr, GCURX, GCURY);  /* display the cursor */
        draw_flag = 0;          /* disable vbl drawing routine */
    }
    else if (HIDE_CNT < 0) {
        HIDE_CNT = 0;           /* hide counter should not become negative */
    }
    mouse_flag -= 1;            /* re-enable mouse drawing */
}



/*
 * hide_cur
 *
 * This routine hides the mouse cursor if it has not already
 * been hidden.
 *
 * Inputs:         None
 *
 * Outputs:
 *    hide_cnt = hide_cnt + 1
 *    draw_flag = 0
 */

static void hide_cur(void)
{
    mouse_flag += 1;            /* disable mouse redrawing */

    /*
     * Increment the counter for the number of hide operations performed.
     * If this is the first one then remove the cursor from the screen.
     * If not then do nothing, because the cursor wasn't on the screen.
     */
    HIDE_CNT += 1;              /* increment it */
    if (HIDE_CNT == 1) {        /* if cursor was not hidden... */
        cur_replace(mcs_ptr);   /* remove the cursor from screen */
        draw_flag = 0;          /* disable vbl drawing routine */
    }

    mouse_flag -= 1;            /* re-enable mouse drawing */
}



/* LOCATOR_INPUT: implements vrq_locator()/vsm_locator() */
void vdi_v_locator(Vwk * vwk)
{
    WORD i;
    Point * point = (Point*)PTSIN;

    *INTIN = 1;

    /* Set the initial locator position. */

    GCURX = point->x;
    GCURY = point->y;
    if (loc_mode == 0) {
        dis_cur();
        /* loop till some event */
        while ((i = gloc_key()) != 1) {
            if (i == 4) {       /* keyboard cursor? */
                hide_cur();     /* turn cursor off */
                GCURX = point->x;
                GCURY = point->y;
                dis_cur();      /* turn cursor on */
            }
        }
        *(INTOUT) = TERM_CH & 0x00ff;

        CONTRL[4] = 1;
        CONTRL[2] = 1;

        PTSOUT[0] = point->x;
        PTSOUT[1] = point->y;
        hide_cur();
    } else {
        i = gloc_key();
        switch (i) {
        case 0:
            break;

        case 1:
            CONTRL[4] = 1;
            *(INTOUT) = TERM_CH & 0x00ff;
            break;

        case 2:
            CONTRL[2] = 1;
            PTSOUT[0] = point->x;
            PTSOUT[1] = point->y;
            break;

        case 3:
            CONTRL[2] = 1;
            PTSOUT[0] = point->x;
            PTSOUT[1] = point->y;
            break;

        case 4:
            CONTRL[2] = 1;
            if (HIDE_CNT == 0) {
                hide_cur();
                PTSOUT[0] = GCURX = point->x;
                PTSOUT[1] = GCURY = point->y;
                dis_cur();
            } else {
                PTSOUT[0] = GCURX = point->x;
                PTSOUT[1] = GCURY = point->y;
            }
            break;
        }
    }
}



/*
 * vdi_v_show_c - show cursor
 */
void vdi_v_show_c(Vwk * vwk)
{
    if (!*INTIN && HIDE_CNT)
        HIDE_CNT = 1;           /* reset cursor to on */

    dis_cur();
}



/*
 * vdi_v_hide_c - hide cursor
 */
void vdi_v_hide_c(Vwk * vwk)
{
    hide_cur();
}



/*
 * vdi_vq_mouse - Query mouse position and button status
 */
void vdi_vq_mouse(Vwk * vwk)
{
    WORD *pointer;

    INTOUT[0] = MOUSE_BT;

    pointer = CONTRL;
    *(pointer + 4) = 1;
    *(pointer + 2) = 1;

    pointer = PTSOUT;
    *pointer++ = GCURX;
    *pointer = GCURY;
}



/* VALUATOR_INPUT: implements vrq_valuator()/vsm_valuator() */
void vdi_v_valuator(Vwk * vwk)
{
}



/*
 * vdi_vex_butv
 *
 * This routine replaces the mouse button change vector with
 * the address of a user-supplied routine.  The previous value
 * is returned so that it also may be called when there is a
 * change in the mouse button status.
 *
 * Inputs:
 *    contrl[7], contrl[8] - pointer to user routine
 *
 * Outputs:
 *    contrl[9], contrl[10] - pointer to old routine
 */
void vdi_vex_butv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*)&CONTRL[9];
    *pointer = (LONG)user_but;
    user_but = (void (*)(void)) *--pointer;
}



/*
 * vdi_vex_motv
 *
 * This routine replaces the mouse coordinate change vector with the address
 * of a user-supplied routine.  The previous value is returned so that it
 * also may be called when there is a change in the mouse coordinates.
 *
 *  Inputs:
 *     contrl[7], contrl[8] - pointer to user routine
 *
 *  Outputs:
 *     contrl[9], contrl[10] - pointer to old routine
 */
void vdi_vex_motv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_mot;
    user_mot = (void (*)(void)) *--pointer;
}



/*
 * vdi_vex_curv
 *
 * This routine replaces the mouse draw vector with the
 * address of a user-supplied routine.  The previous value
 * is returned so that it also may be called when the mouse
 * is to be drawn.
 *
 * Inputs:
 *    contrl[7], contrl[8] - pointer to user routine
 *
 * Outputs:
 *    contrl[9], contrl[10] - pointer to old routine
 *
 */
void vdi_vex_curv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_cur;
    user_cur = (void (*)(void)) *--pointer;
}



#if CONF_WITH_VDI_EXTENSIONS
/*
 * vdi_vex_wheelv: a Milan VDI extension
 *
 * This routine replaces the mouse wheel vector with the
 * address of a user-supplied routine.  The previous value
 * is returned so that it also may be called when the mouse
 * wheel is used.
 *
 * Inputs:
 *    contrl[7], contrl[8] - pointer to user routine
 *
 * Outputs:
 *    contrl[9], contrl[10] - pointer to old routine
 *
 */
void vdi_vex_wheelv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_wheel;
    user_wheel = (void (*)(void)) *--pointer;
}
#endif



/* copies src mouse form to dst, constrains hotspot
 * position & colors and maps colors
 */
static void set_mouse_form (const Mcdb *src, Mcdb * dst)
{
    int i;
    WORD col;
    UWORD * gmdt;                /* global mouse definition table */
    const UWORD * mask;
    const UWORD * data;

    mouse_flag += 1;            /* disable updates while redefining cursor */

    /* save x-offset of mouse hot spot */
    dst->xhot = src->xhot & 0x000f;

    /* save y-offset of mouse hot spot */
    dst->yhot = src->yhot & 0x000f;

    /* is background color index too high? */
    col = src->bg_col;
    if (col >= DEV_TAB[13]) {
        col = 1;               /* yes - default to 1 */
    }
    dst->bg_col = MAP_COL[col];

    /* is foreground color index too high? */
    col = src->fg_col;
    if (col >= DEV_TAB[13]) {
        col = 1;               /* yes - default to 1 */
    }
    dst->fg_col = MAP_COL[col];

    /*
     * Move the new mouse definition into the global mouse cursor definition
     * table.  The values for the mouse mask and data are supplied as two
     * separate 16-word entities.  They must be stored as a single array
     * starting with the first word of the mask followed by the first word
     * of the data and so on.
     */

    /* copy the data to the global mouse definition table */
    gmdt = dst->mask;
    mask = src->mask;
    data = src->data;
    for (i = 15; i >= 0; i--) {
        *gmdt++ = *mask++;              /* get next word of mask */
        *gmdt++ = *data++;              /* get next word of data */
    }

    mouse_flag -= 1;                    /* re-enable mouse drawing */
}



/*
 * vdi_vsc_form - Transforms user defined cursor to device specific format.
 *
 * Get the new values for the x and y-coordinates of the mouse hot
 * spot and the new color indices for the mouse mask and data.
 *
 * Inputs:
 *     intin[0] - x coordinate of hot spot
 *     intin[1] - y coordinate of hot spot
 *     intin[2] - reserved for future use. must be 1
 *     intin[3] - Mask color index
 *     intin[4] - Data color index
 *     intin[5-20]  - 16 words of cursor mask
 *     intin[21-36] - 16 words of cursor data
 *
 * Outputs:        None
 */
void vdi_vsc_form(Vwk * vwk)
{
    set_mouse_form((const Mcdb *)INTIN, &mouse_cdb);
}


/*
 * vdi_mousex_handler - Handle additional mouse buttons
 */

static void vdi_mousex_handler (WORD scancode)
{
    WORD old_buttons = MOUSE_BT;

    if (scancode == 0x37)      /* Mouse button 3 press */
        MOUSE_BT |= 0x04;
    else if (scancode == 0xb7) /* Mouse button 3 release */
        MOUSE_BT &= ~0x04;
    else if (scancode == 0x5e) /* Mouse button 4 press */
        MOUSE_BT |= 0x08;
    else if (scancode == 0xde) /* Mouse button 4 release */
        MOUSE_BT &= ~0x08;
    else if (scancode == 0x5f) /* Mouse button 5 press */
        MOUSE_BT |= 0x10;
    else if (scancode == 0xdf) /* Mouse button 5 release */
        MOUSE_BT &= ~0x10;

    if (MOUSE_BT != old_buttons)
        call_user_but(MOUSE_BT);

    if (scancode == 0x59)      /* Wheel up */
        call_user_wheel(0, -1);
    else if (scancode == 0x5a) /* Wheel down */
        call_user_wheel(0, 1);
    else if (scancode == 0x5c) /* Wheel left */
        call_user_wheel(1, -1);
    else if (scancode == 0x5d) /* Wheel right */
        call_user_wheel(1, 1);
}



/*
 * vdimouse_init - Initializes the mouse (VDI part)
 *
 * entry:          none
 * exit:           none
 */

void vdimouse_init(Vwk * vwk)
{
    struct kbdvecs *kbd_vectors;
    static const struct {
        BYTE topmode;
        BYTE buttons;
        BYTE xparam;
        BYTE yparam;
    } mouse_params = {0, 0, 1, 1};

    /* Input must be initialized here and not in init_wk */
    loc_mode = 0;               /* default is request mode  */
    val_mode = 0;               /* default is request mode  */
    chc_mode = 0;               /* default is request mode  */
    str_mode = 0;               /* default is request mode  */

    /* mouse settings */
    HIDE_CNT = 1;               /* mouse is initially hidden */
    GCURX = DEV_TAB[0] / 2;     /* initialize the mouse to center */
    GCURY = DEV_TAB[1] / 2;

    user_but = do_nothing;
    user_mot = do_nothing;
    user_cur = mov_cur;         /* initialize user_cur vector */
    user_wheel = do_nothing;

    /* Move in the default mouse form (presently the arrow) */
    set_mouse_form(&arrow_cdb, &mouse_cdb);

    MOUSE_BT = 0;               /* clear the mouse button state */
    cur_ms_stat = 0;            /* clear the mouse status */
    mouse_flag = 0;             /* clear the mouse flag */
    draw_flag = 0;              /* clear the hide operations counter */
    newx = 0;                   /* set cursor x-coordinate to 0 */
    newy = 0;                   /* set cursor y-coordinate to 0 */

    /* vblqueue points to start of vbl_list[] */
    *vblqueue = (LONG)vb_draw;   /* set GEM VBL-routine to vbl_list[0] */

    /* Initialize mouse via XBIOS in relative mode */
    Initmous(1, (LONG)&mouse_params, (LONG)mouse_int);

    kbd_vectors = (struct kbdvecs *)Kbdvbase();
    old_statvec = kbd_vectors->statvec;
    kbd_vectors->statvec = wheel_int;
    mousexvec = vdi_mousex_handler;
}



/*
 * vdimouse_exit - deinitialize/disable mouse
 */

void vdimouse_exit(Vwk * vwk)
{
    LONG * pointer;             /* help for storing LONGs in INTIN */
    struct kbdvecs *kbd_vectors;

    user_but = do_nothing;
    user_mot = do_nothing;
    user_cur = do_nothing;
    user_wheel = do_nothing;

    pointer = vblqueue;         /* vblqueue points to start of vbl_list[] */
    *pointer = (LONG)vb_draw;   /* set GEM VBL-routine to vbl_list[0] */

    /* disable mouse via XBIOS */
    Initmous(0, 0, 0);

    kbd_vectors = (struct kbdvecs *)Kbdvbase();
    kbd_vectors->statvec = old_statvec;
}



/*
 * vb_draw - moves mouse cursor, GEM VBL routine
 *
 * It removes the mouse cursor from its current location, if necessary,
 * and redraws it at a new location.
 *
 *      Inputs:
 *         draw_flag - signals need to redraw cursor
 *         newx - new cursor x-coordinate
 *         newy - new cursor y-coordinate
 *         mouse_flag - cursor hide/show flag
 *
 *      Outputs:
 *         draw_flag is cleared
 *
 *      Registers Modified:     d0, d1
 *
 */

/* If we do not need to draw the cursor now then just exit. */

static void vb_draw(void)
{
    WORD old_sr = set_sr(0x2700);       /* disable interrupts */
    if (draw_flag) {
        draw_flag = FALSE;
        set_sr(old_sr);
        if (!mouse_flag) {
            cur_replace(mcs_ptr);       /* remove the old cursor from the screen */
            cur_display(&mouse_cdb, mcs_ptr, newx, newy);  /* display the cursor */
        }
    } else
        set_sr(old_sr);

}



/*
 * cur_display_clip()
 *
 * handles cursor display for cursors that are subject to L/R clipping
 */
static void cur_display_clip(WORD op,Mcdb *sprite,MCS *mcs,UWORD *mask_start,UWORD shft)
{
    WORD dst_inc, plane;
    UWORD cdb_fg, cdb_bg;
    UWORD *addr, *save;

    dst_inc = v_lin_wr >> 1;    /* calculate number of words in a scan line */

    addr = mcs->addr;           /* starting screen address */
    save = (UWORD *)mcs->area;  /* we save words, not longwords */

    cdb_bg = sprite->bg_col;    /* get mouse background color bits */
    cdb_fg = sprite->fg_col;    /* get mouse foreground color bits */

    /* plane controller, draw cursor in each graphic plane */
    for (plane = v_planes - 1; plane >= 0; plane--) {
        WORD row;
        UWORD *src, *dst;
        UWORD cdb_mask = 0x01;  /* for checking cdb_bg/cdb_fg */

        /* setup the things we need for each plane again */
        src = mask_start;               /* calculated mask data begin */
        dst = addr++;                   /* current destination address */

        /* loop through rows */
        for (row = mcs->len - 1; row >= 0; row--) {
            ULONG bits;                 /* our graphics data */
            ULONG fg;                   /* the foreground color */
            ULONG bg;                   /* the background color */

            /*
             * first, save the existing data
             */
            *save++ = *dst;
            if (op == 1) {          /* right word only */
                bits = *dst;            /* dst already at right word */
            } else {                /* left word only  */
                bits = ((ULONG)*dst) << 16; /* move to left posn */
            }

            /*
             * align the forms with the cursor position on the screen
             */

            /* get and align background & foreground forms */
            bg = (ULONG)*src++ << shft;
            fg = (ULONG)*src++ << shft;

            /*
             * logical operation for cursor interaction with screen
             */

            /* select operation for mouse mask background color */
            if (cdb_bg & cdb_mask)
                bits |= bg;
            else
                bits &= ~bg;

            /* select operation for mouse mask foreground color */
            if (cdb_fg & cdb_mask)
                bits |= fg;
            else
                bits &= ~fg;

            /*
             * update the screen with the new data
             */
            if (op == 1) {          /* right word only */
                *dst = (UWORD)bits;
            } else {                /* left word only */
                *dst = (UWORD)(bits >> 16);
            }

            dst += dst_inc;             /* a1 -> next row of screen */
        } /* loop through rows */

        cdb_mask <<= 1;
    } /* loop through planes */
}

/*
 * cur_display() - blits a "cursor" to the destination
 *
 * before the destination is overwritten, the current contents are
 * saved to the user-provided save area (MCS).  then the cursor is
 * written, combining a background colour form, a foreground colour
 * form, and the current contents of the destination.
 *
 * some points to note:
 * the cursor is always 16x16 pixels.  in the general case, it will
 * overlap two adjacent screen words in each plane; thus the save area
 * requires 4 bytes per plane for each row of the cursor, or 64 bytes
 * in total per plane (plus some bookkeeping overhead).  if the cursor
 * is subject to left or right clipping, however, then it must lie
 * within one screen word (per plane), so we only save 32 bytes/plane.
 */

static void cur_display (Mcdb *sprite, MCS *mcs, WORD x, WORD y)
{
    int row_count, plane, inc, op, dst_inc;
    UWORD * addr, * mask_start;
    UWORD shft, cdb_fg, cdb_bg;
    ULONG *save;

    x -= sprite->xhot;          /* x = left side of destination block */
    y -= sprite->yhot;          /* y = top of destination block */

    mcs->stat = 0x00;           /* reset status of save buffer */

    /*
     * clip x axis
     */
    if (x < 0) {            /* clip left */
        x += 16;                /* get address of right word */
        op = 1;                 /* remember we're clipping left */
    }
    else if (x >= (DEV_TAB[0]-15)) {    /* clip right */
        op = 2;                 /* remember we're clipping right */
    }
    else {                  /* no clipping */
        op = 0;                 /* longword save */
        mcs->stat |= MCS_LONGS; /* mark savearea as longword save */
    }

    /*
     * clip y axis
     */
    mask_start = sprite->mask;  /* MASK/FORM for cursor */
    if (y < 0) {            /* clip top */
        row_count = y + 16;
        mask_start -= y << 1;   /* point to first visible row of MASK/FORM */
        y = 0;                  /* and reset starting row */
    }
    else if (y > (DEV_TAB[1]-15)) { /* clip bottom */
        row_count = DEV_TAB[1] - y + 1;
    }
    else {
        row_count = 16;
    }

    /*
     *  Compute the bit offset into the desired word, save it, and remove
     *  these bits from the x-coordinate.
     */
    addr = get_start_addr(x, y);
    shft = 16 - (x&0x0f);       /* amount to shift forms by */

    /*
     *  Store values required by cur_replace()
     */
    mcs->len = row_count;       /* number of cursor rows */
    mcs->addr = addr;           /* save area: origin of material */
    mcs->stat |= MCS_VALID;     /* flag the buffer as being loaded */

    /*
     *  To allow performance optimisations in this function, we handle
     *  L/R clipping in a separate function
     */
    if (op) {
        cur_display_clip(op,sprite,mcs,mask_start,shft);
        return;
    }

    /*
     * The rest of this function handles the no-L/R clipping case
     */
    inc = v_planes;             /* # distance to next word in same plane */
    dst_inc = v_lin_wr >> 1;    /* calculate number of words in a scan line */

    save = mcs->area;           /* for long stores */

    cdb_bg = sprite->bg_col;    /* get mouse background color bits */
    cdb_fg = sprite->fg_col;    /* get mouse foreground color bits */

    /* plane controller, draw cursor in each graphic plane */
    for (plane = v_planes - 1; plane >= 0; plane--) {
        int row;
        UWORD * src, * dst;
        UWORD cdb_mask = 0x01;  /* for checking cdb_bg/cdb_fg */

        /* setup the things we need for each plane again */
        src = mask_start;               /* calculated mask data begin */
        dst = addr++;                   /* current destination address */

        /* loop through rows */
        for (row = row_count - 1; row >= 0; row--) {
            ULONG bits;                 /* our graphics data */
            ULONG fg;                   /* the foreground color */
            ULONG bg;                   /* the background color */

            /*
             * first, save the existing data
             */
            bits = ((ULONG)*dst) << 16; /* bring to left pos. */
            bits |= *(dst + inc);
            *save++ = bits;

            /*
             * align the forms with the cursor position on the screen
             */

            /* get and align background & foreground forms */
            bg = (ULONG)*src++ << shft;
            fg = (ULONG)*src++ << shft;

            /*
             * logical operation for cursor interaction with screen
             * note that this only implements the "VDI" mode
             */

            /* select operation for mouse mask background color */
            if (cdb_bg & cdb_mask)
                bits |= bg;
            else
                bits &= ~bg;

            /* select operation for mouse mask foreground color */
            if (cdb_fg & cdb_mask)
                bits |= fg;
            else
                bits &= ~fg;

            /*
             * update the screen with the new data
             */
            *dst = (UWORD)(bits >> 16);
            *(dst + inc) = (UWORD)bits;
            dst += dst_inc;             /* next row of screen */
        } /* loop through rows */

        cdb_mask <<= 1;
    } /* loop through planes */
}


/*
 * cur_replace - replace cursor with data in save area
 *
 * note: the near-duplication of loops for the word and longword cases
 * is done deliberately for performance reasons
 *
 * input:
 *      mcs         ptr to mouse cursor save area
 *      v_planes    number of planes in destination
 *      v_lin_wr    line wrap (byte width of form)
 */
static void cur_replace (MCS *mcs)
{
    WORD plane, row;
    UWORD *addr, *src, *dst;
    const WORD inc = v_planes;      /* # words to next word in same plane */
    const WORD dst_inc = v_lin_wr >> 1; /* # words in a scan line */

    if (!(mcs->stat & MCS_VALID))   /* does save area contain valid data ? */
        return;
    mcs->stat &= ~MCS_VALID;        /* yes but (like TOS) don't allow reuse */

    addr = mcs->addr;
    src = (UWORD *)mcs->area;

    /*
     * handle longword data
     */
    if (mcs->stat & MCS_LONGS) {
        /* plane controller, draw cursor in each graphic plane */
        for (plane = v_planes - 1; plane >= 0; plane--) {
            dst = addr++;           /* current destination address */
            /* loop through rows */
            for (row = mcs->len - 1; row >= 0; row--) {
                *dst = *src++;
                *(dst + inc) = *src++;
                dst += dst_inc;     /* next row of screen */
            }
        }
        return;
    }

    /*
     * handle word data
     */

    /* plane controller, draw cursor in each graphic plane */
    for (plane = v_planes - 1; plane >= 0; plane--) {
        dst = addr++;               /* current destination address */
        /* loop through rows */
        for (row = mcs->len - 1; row >= 0; row--) {
            *dst = *src++;
            dst += dst_inc;         /* next row of screen */
        }
    }
}


/* line-A support */

/* set by linea.S */
WORD sprite_x, sprite_y;
Mcdb *sprite_def;
MCS *sprite_sav;

void draw_sprite(void)
{
    cur_display(sprite_def, sprite_sav, sprite_x, sprite_y);
}

void undraw_sprite(void)
{
    cur_replace(sprite_sav);
}
