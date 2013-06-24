/*
 * vdi_mouse.c
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2013 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "asm.h"
#include "xbiosbind.h"
#include "vdi_defs.h"
#include "tosvars.h"
#include "lineavars.h"
#include "kprint.h"



/* mouse related vectors (linea variables in bios/lineavars.S) */

extern void     (*user_but)(void);      // user button vector
extern void     (*user_cur)(void);      // user cursor vector
extern void     (*user_mot)(void);      // user motion vector

/* call the vectors from C */
extern void call_user_but(WORD status);
extern void call_user_wheel(WORD wheel_number, WORD wheel_amount);


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
static void cur_display(WORD x, WORD y);
static void cur_replace(void);
static void vb_draw(void);             /* user button vector */

extern void mouse_int(void);    /* mouse interrupt routine */
extern void wheel_int(void);    /* wheel interrupt routine */
extern void mov_cur(void);      /* user button vector */


/* global storage area for mouse form definition */
/* as long, as we use parts in assembler, we need these */
extern WORD m_pos_hx;       // (cdb+0) Mouse hot spot - x coord
extern WORD m_pos_hy;       // (cdb+2) Mouse hot spot - y coord
extern WORD m_planes;       // (cdb+4) unused - Plane count for mouse pointer
extern WORD m_cdb_bg;       // (cdb+6) Mouse background color as pel value
extern WORD m_cdb_fg;       // (cdb+8) Mouse foreground color as pel value
extern UWORD mask_form;     // (cdb+10) Storage for mouse mask and cursor

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
        0xE000, //%1110000000000000
        0xF000, //%1111000000000000
        0xF800, //%1111100000000000
        0xFC00, //%1111110000000000
        0xFE00, //%1111111000000000
        0xFF00, //%1111111100000000
        0xFF80, //%1111111110000000
        0xFFC0, //%1111111111000000
        0xFE00, //%1111111000000000
        0xFE00, //%1111111000000000
        0xEF00, //%1110111100000000
        0x0F00, //%0000111100000000
        0x0780, //%0000011110000000
        0x0780, //%0000011110000000
        0x03C0, //%0000001111000000
        0x0000  //%0000000000000000
    },
    /* foreground definition */
    {
        0x4000, //%0100000000000000
        0x6000, //%0110000000000000
        0x7000, //%0111000000000000
        0x7800, //%0111100000000000
        0x7C00, //%0111110000000000
        0x7E00, //%0111111000000000
        0x7F00, //%0111111100000000
        0x7F80, //%0111111110000000
        0x7C00, //%0111110000000000
        0x6C00, //%0110110000000000
        0x4600, //%0100011000000000
        0x0600, //%0000011000000000
        0x0300, //%0000001100000000
        0x0300, //%0000001100000000
        0x0180, //%0000000110000000
        0x0000  //%0000000000000000
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
    mouse_flag += 1;            // disable mouse redrawing
    HIDE_CNT -= 1;              // decrement hide operations counter
    if (HIDE_CNT == 0) {
        cur_display(GCURX, GCURY);          // display the cursor
        draw_flag = 0;          // disable vbl drawing routine
    }
    else if (HIDE_CNT < 0) {
        HIDE_CNT = 0;           // hide counter should not become negative
    }
    mouse_flag -= 1;            // re-enable mouse drawing
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
    HIDE_CNT += 1;              // increment it
    if (HIDE_CNT == 1) {        // if cursor was not hidden...
        cur_replace();          // remove the cursor from screen
        draw_flag = 0;          // disable vbl drawing routine
    }

    mouse_flag -= 1;            /* re-enable mouse drawing */
}



/* LOCATOR_INPUT: */
void v_locator(Vwk * vwk)
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
        CONTRL[4] = 1;
        CONTRL[2] = 0;

        i = gloc_key();
        switch (i) {
        case 0:
            CONTRL[2] = 0;
            break;

        case 1:
            CONTRL[2] = 0;
            CONTRL[4] = 1;
            *(INTOUT) = TERM_CH & 0x00ff;
            break;

        case 2:
            PTSOUT[0] = point->x;
            PTSOUT[1] = point->y;
            break;

        case 3:
            CONTRL[4] = 1;
            PTSOUT[0] = point->x;
            PTSOUT[1] = point->y;
            break;

        case 4:
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
 * _v_show_c - show cursor
 */

void _v_show_c(Vwk * vwk)
{
    if (!*INTIN && HIDE_CNT)
        HIDE_CNT = 1;           /* reset cursor to on */

    dis_cur();
}



/*
 * _v_hide_c - hide cursor
 */

void _v_hide_c(Vwk * vwk)
{
    hide_cur();
}



/*
 * _vq_mouse - Query mouse position and button status
 */

void _vq_mouse(Vwk * vwk)
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



/* VALUATOR_INPUT: */
void v_valuator(Vwk * vwk)
{
}



/*
 * _vex_butv
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

void _vex_butv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*)&CONTRL[9];
    *pointer = (LONG)user_but;
    user_but = (void (*)(void)) *--pointer;
}



/*
 * _vex_motv
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

void _vex_motv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_mot;
    user_mot = (void (*)(void)) *--pointer;
}



/*
 * _vex_curv
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

void _vex_curv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_cur;
    user_cur = (void (*)(void)) *--pointer;
}



/*
 * _vex_wheelv
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

void _vex_wheelv(Vwk * vwk)
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_wheel;
    user_wheel = (void (*)(void)) *--pointer;
}



static void set_mouse_form (Vwk * vwk, Mcdb * mcdb)
{
    int i;
    UWORD * gmdt;                /* global mouse definition table */
    UWORD * mask;
    UWORD * data;

    mouse_flag += 1;            /* disable updates while redefining cursor */

    /* save x-offset of mouse hot spot */
    m_pos_hx = mcdb->xhot & 0x000f;

    /* save y-offset of mouse hot spot */
    m_pos_hy = mcdb->yhot & 0x000f;

    /* is background color index too high? */
    if (mcdb->bg_col >= DEV_TAB[13]) {
        mcdb->bg_col = 1;               /* yes - default to 1 */
    }
    m_cdb_bg = MAP_COL[mcdb->bg_col];

    /* is forground color index too high? */
    if (mcdb->fg_col >= DEV_TAB[13]) {
        mcdb->fg_col = 1;               /* yes - default to 1 */
    }
    m_cdb_fg = MAP_COL[mcdb->fg_col];

    /*
     * Move the new mouse defintion into the global mouse cursor definition
     * table.  The values for the mouse mask and data are supplied as two
     * separate 16-word entities.  They must be stored as a single array
     * starting with the first word of the mask followed by the first word
     * of the data and so on.
     */

    /* copy the data to the global mouse definition table */
    gmdt = &mask_form;
    mask = mcdb->mask;
    data = mcdb->data;
    for (i = 15; i >= 0; i--) {
        *gmdt++ = *mask++;              /* get next word of mask */
        *gmdt++ = *data++;              /* get next word of data */
    }

    mouse_flag -= 1;                    /* re-enable mouse drawing */
}



/*
 * xfm_crfm - Transforms user defined cursor to device specific format.
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
void xfm_crfm (Vwk * vwk)
{
    set_mouse_form(vwk, (Mcdb *)INTIN);
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
    set_mouse_form(vwk, (Mcdb *)&arrow_cdb);    /* transform mouse */

    MOUSE_BT = 0;               // clear the mouse button state
    cur_ms_stat = 0;            // clear the mouse status
    mouse_flag = 0;             // clear the mouse flag
    draw_flag = 0;              // clear the hide operations counter
    newx = 0;                   // set cursor x-coordinate to 0
    newy = 0;                   // set cursor y-coordinate to 0

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
 * * and redraws it at a new location.
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
    WORD old_sr = set_sr(0x2700);       // disable interrupts
    if (draw_flag) {
        draw_flag = FALSE;
        set_sr(old_sr);
        if (!mouse_flag) {
            cur_replace();              // remove the old cursor from the screen
            cur_display(newx, newy);    // display the cursor
        }
    } else
        set_sr(old_sr);

}



#define F_SAVRDY        1       // save buffer status flag: 0:empty  1:full
#define F_SAVWID        2       // saved line width        0:word   1:longword



/*
 * cur_display - blits a "cursor" to the destination
 *
 * combining a background color form, foreground color form,
 * and destination.  There are two forms.  Each form is
 * blt'ed in transparent mode.  The actual logic operation
 * is based upon the current color bit for each form.
 *
 * Procedure:
 *
 *   plane loop
 *       i. advance the destination pointer to next plane
 *      ii. set up logic routine address based on current
 *          foreground color bit
 *     iii. initialize BG form and FG form pointers
 *
 *   outer loop
 *       i. advance destination pointer to next row
 *
 *   inner loop
 *       i. fetch destination and save it.
 *      ii. init and allign BG form and FG form.
 *     iii. combine BG form, FG form, and destination.
 *      iv. store value back to destination.
 *
 *      fetching and saving a destination long word
 *
 *  in:
 *      a0.l    points to start of BG/FG form
 *      a1.l    points to start of destination
 *      a2.l    points to start of save area
 *      a3.l    thread to alignment fragment
 *      a4.l    thread to logic fragment
 *      a5.l    thread to storage segment
 *
 *      d2.w
 *      d3.w    offset to next word
 *      d4.w    form wrap offset
 *      d5.w    row counter
 *      d6.w    shift count
 */

static void cur_display (WORD x, WORD y)
{
    int row_count, plane, inc, op, dst_inc;
    UWORD * addr, * mask_start;
    UWORD shft, cdb_fg, cdb_bg;
    UWORD * save_w;
    ULONG * save_l;
    MCS *mcs = mcs_ptr;

    x -= m_pos_hx;              /* d0 <- left side of destination block */
    y -= m_pos_hy;              /* d1 <- hi y : destination block */

    mcs->stat = 0x00;           /* reset status of save buffer */
    op = 0;
    /* clip x axis */
    if ( x < 0 ) {
        /* clip left */
        x += 16;                /* get address of right word */
        op = 1;                 /* index left clip routine addresses */
    }
    else {
        /* check for need to clip on right side */
        /* compare to width of screen(maximum x value) */
        if ( x >= (DEV_TAB[0] - 15) ) {
            op = 2;             /* index to right clip routine addresses */
        }
        else {
            mcs->stat |= 0x02;  /* indicate longword save */
        }
    }

    /* clip y axis */
    mask_start = &mask_form;            /* a3 -> MASK/FORM for cursor */
    if ( y < 0 ) {
        /* clip up */
        row_count = y + 16;             /* calculate row count */
        mask_start -= y << 1;           /* a0 -> first visible row of MASK/FORM */
        y = 0;                  /* ymin=0 */
    }
    else {
        /* check for need to clip on the down side */
        /* compare to height of screen(maximum y value) */
        if ( y > (DEV_TAB[1] - 15) ) {
            row_count = DEV_TAB[1] - y + 1;
        }
        else {
            row_count = 16;   /* long */    /* d5 <- row count */
        }
    }

    /*
     *  Compute the bit offset into the desired word, save it, and remove
     *  these bits from the x-coordinate.
     */
    addr = get_start_addr(x, y);
    shft = x&0xf;               /* initial bit position in WORD */

    /*
     * Initialize
     */

    inc = v_planes;             /* # distance to next word in same plane */
    dst_inc = v_lin_wr >> 1;    /* calculate number of words in a scan line */

    /* these are stored for later bringing back the cursors background */
    mcs->len = row_count;       /* number of cursor rows */
    mcs->addr = addr;           /* save area: origin of material */
    mcs->stat |= 1;             /* flag the buffer as being loaded */

    save_w = (UWORD *)mcs->area;/* for word stores */
    save_l = mcs->area;         /* for long stores */

    cdb_bg = m_cdb_bg;          /* get mouse background color bits */
    cdb_fg = m_cdb_fg;          /* get mouse foreground color bits */

    /* plane controller, draw cursor in each graphic plane */
    for (plane = v_planes - 1; plane >= 0; plane--) {
        int row;
        UWORD * src, * dst;

        /* setup the things we need for each plane again */
        src = mask_start;               /* calculated mask data begin */
        dst = addr++;                   /* current destination address */

        /* loop through rows */
        for (row = row_count - 1; row >= 0; row--) {
            ULONG bits = 0;             /* our graphics data */
            ULONG fg = 0;               /* the foreground color */
            ULONG bg = 0;               /* the background color */

            /*
             * proces the needed fetch operation
             */

            switch(op) {
            case 0:
                /* long word */
                bits = ((ULONG)*dst) << 16;       /* bring to left pos. */
                bits |= *(dst + inc);
                *save_l++ = bits;
                break;

            case 1:
                /* right word only */
                *save_w++ = *dst;         /* dst already at right word */
                bits = *dst;
                break;

            case 2:
                /* left word only  */
                *save_w++ = *dst;
                bits = ((ULONG)*dst) << 16;       /* bring to left pos. */
                break;

            }

            /*
             * proces the needed alignment
             */

            /* get and align background form */
            bg = (ULONG)*src++ << 16;
            bg = bg >> shft;

            /* get and align foreground form */
            fg = (ULONG)*src++ << 16;
            fg = fg >> shft ;

            /*
             * logical operation for cursor interaction with screen
             */

            /* select operation for mouse mask background color */
            if (cdb_bg & 0x0001)
                bits |= bg;
            else
                bits &= ~bg;

            /* select operation for mouse mask foreground color */
            if (cdb_fg & 0x0001)
                bits |= fg;
            else
                bits &= ~fg;

            /*
             * proces the needed store operation
             */

            switch(op) {
            case 0:
                /* long word */
                *(dst + inc) = (UWORD)bits;
            case 2:
                /* left word only  */
                bits = bits >> 16;

            case 1:
                /* right word only */
                *dst = (UWORD)bits;
            }

            dst += dst_inc;             /* a1 -> next row of screen */
        } /* loop through rows */

        cdb_bg >>= 1;           /* advance to next bg color bit */
        cdb_fg >>= 1;           /* advance to next fg color bit */

    } /* loop through planes */
}



/*
 * cur_replace - replace cursor with data in save area.
 *
 * input:
 *     mcs_ptr         ptr to mouse cursor save area
 *     _v_planes       number of planes in destination
 *     _v_line_wr      line wrap (byte width of form)
 */

static void cur_replace (void)
{
    int inc, dst_inc, plane;
    UWORD * addr;
    MCS *mcs = mcs_ptr;

    if (!(mcs->stat & 1) )      /* does save area contain valid data ? */
        return;

    addr = mcs->addr;
    inc = v_planes;
    dst_inc = v_lin_wr >> 1;    /* calculate LONGs in a scan line */

    /* word or longword ? */
    if (mcs->stat & 2) {
        /* longword ? */
        ULONG * src = mcs->area;

        /* plane controller, draw cursor in each graphic plane */
        for (plane = v_planes - 1; plane >= 0; plane--) {
            int row;
            UWORD * dst = addr++;       /* current destination address */

            /* loop through rows */
            for (row = mcs->len - 1; row >= 0; row--) {
                ULONG bits = *src++;       /* get the save bits */
                *(dst + inc) = (UWORD)bits;
                *dst = (UWORD)(bits >> 16);
                dst += dst_inc;         /* a1 -> next row of screen */
            }
        }
    }
    else {
        /* word */
        UWORD * src = (UWORD *)mcs->area;

        /* plane controller, draw cursor in each graphic plane */
        for (plane = v_planes - 1; plane >= 0; plane--) {
            int row;
            UWORD * dst = addr++;       /* current destination address */

            /* loop through rows */
            for (row = mcs->len - 1; row >= 0; row--) {
                *dst = *src++;
                dst += dst_inc;         /* a1 -> next row of screen */
            }
        }
    }
}
