/*
 * vdimouse.S - machine independent portion of the mouse driver
 *
 * Copyright (c) 1999 Caldera, Inc. and Authors:
 *                    SCC
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "vdiconf.h"

#define F_SAVRDY        0               // save buffer status flag: 0:empty  1:full
#define F_SAVWID        1               // saved line width        0:word   1:longword


        .text

/* Locally Defined Constants and Variables */

        .globl  _DIS_CUR        // display cursor routine
        .globl  _HIDE_CUR       // hide cursor routine
        .globl  _XFM_CRFM       // set mouse form
        .globl  vb_draw         // vertical blank mouse draw routine
        .globl  arrow_cdb       // default mouse definition

/* Externally Defined Labels */

        .xdef   _newx           // New mouse x position
        .xdef   _newy           // New mouse y position
        .xdef   _draw_flag      // Non-zero means draw mouse form on vblank
        .xdef   _GCURX          // graphics cursor x-coordinate
        .xdef   _GCURY          // graphics cursor y-coordinate
        .xdef   _INTIN          // address of INTIN array
        .xdef   _HIDE_CNT       // graphics cursor hide count
        .xdef   _CONTRL         // address of CONTRL array
        .xdef   _v_planes       // number of video planes
        .xdef   _v_lin_wr       // number of bytes in a scan line
        .xdef   _v_bas_ad       // base address of video planes
        .xdef   _DEV_TAB        // address of device table
        .xdef   _MAP_COL        // color map table

        .xdef   mouse_cdb       // storage area for mouse form definition
        .xdef   m_pos_hx        // (cdb+0) Mouse hot spot - x coord
        .xdef   m_pos_hy        // (cdb+2) Mouse hot spot - y coord
        .xdef   m_planes        // (cdb+4) unused - Plane count for mouse pointer
        .xdef   m_cdb_bg        // (cdb+6) Mouse background color as pel value
        .xdef   m_cdb_fg        // (cdb+8) Mouse foreground color as pel value
        .xdef   mask_form       // (cdb+10) Storage for mouse mask and cursor

        .xdef   _mouse_flag     // Non-zero if mouse ints disabled
        .xdef   cur_ms_stat     // current mouse status

        .xdef   save_addr
        .xdef   save_len
        .xdef   save_area
        .xdef   save_stat

        .xdef   concat          // calculate offset


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

vb_draw:
        move.w  sr,-(sp)        // save current value of status register
        ori.w   #0x0700,sr      // mask off all interrupts
        bclr.b  #0,_draw_flag   // draw the cursor?
        beq     vblank_noop     // nope - done

/*
 *      We want to draw the cursor.  Get the new coordinates for the cursor
 *      and check the value of the hide/show flag.  If its value is non-zero
 *      then we are already attempting to hide or display the cursor so there
 *      is nothing more to be done here.
 */
        move.w  _newx,d0        // get new cursor x-coordinate
        move.w  _newy,d1        // get new cursor y-coordinate
        move.w  (sp)+,SR        // restore the status register
        tst.b   _mouse_flag     // are we in a hide/show operation?
        bne     vblank_exit     // yes - done

/*
 *      Everything is now clear to redraw the mouse cursor.  First, remove
 *      the old cursor, if one exists, from the screen.  Finally, redraw
 *      the cursor in its new location.
 */
        bsr     cur_replace     // remove the old cursor from the screen
        bsr     cur_display     // redraw the cursor
vblank_exit:
        rts

vblank_noop:
        move.w  (sp)+, sr               // restore interrupt context
        rts



/*
 *    cur_display
 *
 *      This routine blts a "cursor" to the destination by com-
 *      bining a background color form, foreground color form,
 *      and destination.  There are two forms.  Each form is
 *      blt'ed in transparent mode.  The actual logic operation
 *      is based upon the current color bit for each form.
 *
 *      Procedure:
 *
 *       plane loop
 *           i. advance to destination pointer to next plane
 *          ii. set up logic routine address based on current
 *              foreground color bit
 *         iii. initialize BG form and FG form pointers
 *
 *        outer loop
 *            i. advance destination pointer to next row
 *
 *         inner loop
 *             i. fetch destination and save it.
 *            ii. init and allign BG form and FG form.
 *           iii. combine BG form, FG form, and destination.
 *            iv. store value back to destination.
 *
 *      Date:   27-sep-84
 *
 *      Latest Update:  07-dec-84
 *
 *      Inputs:
 *         d0.w - cursor x
 *         d1.w - cursor y
 *
 *      Outputs:        None
 *
 *      Registers Modified:     All
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

#define RTN_REC         2*4


cur_display:
        movem.l d2-d7/a2-a6,-(sp)       // save registers used by 'C'
        clr.w   d7                      // init entry & store routine index
        lea     _DEV_TAB,a2             // get address of the DEVICE TABLE
        lea     save_stat,a4            // get address of status flags for
                                        // the cursor buffer
clip_x:
        bclr.b  #F_SAVWID,(a4)          // default to word wide save format

        sub.w   m_pos_hx,d0             // d0 <- left side of destination block
        bcs     clip_left

        move.w  (a2),d3                 // get width of screen(maximum x value)
        subi.w  #15,d3                  // check for need to clip on right side

        cmp     d3,d0
        bhi     clip_right
        bset.b  #F_SAVWID,(a4)          // indicate longword save
        bra     clip_y

clip_left:
        addi.w  #16,d0                  // get address of right word
        moveq.l #RTN_REC*1,d7          // index left clip routine addresses
        bra     clip_y
clip_right:
        moveq.l #RTN_REC*2,d7          // index to right clip routine addresses

clip_y:
        lea     mask_form,a0            // a3 -> MASK/FORM for cursor
        sub.w   m_pos_hy,d1             // d1 <- hi y : destination block
        bcs     clip_up                 // if y<0 clip top of form

        move.w  2(a2),d3                // get height of screen(maximum y value)
        subi.w  #15,d3

        cmp.w   d3,d1
        bhi     clip_down
        moveq.l #16,d5                  // d5 <- row count
        bra     get_addr

clip_up:
        move.w  d1,d5                   // calculate row count
        addi.w  #16,d5
        asl.w   #2,d1
        suba.w  d1,a0                   // a0 -> first visible row of MASK/FORM
        clr.w   d1                      // ymin=0
        bra     get_addr

clip_down:
        move.w  2(a2),d5                // get height of screen(maximum y value)
        sub.w   d1,d5                   // d5 <- row count
        addq.w  #1,d5

        
get_addr:
/*
 *  Compute the bit offset into the desired word, save it, and remove
 *  these bits from the x-coordinate.
 */

        move.w  _v_lin_wr,d4    // get number of bytes in a scan line
        move.l  _v_bas_ad,a1    // get start addr. of 1st graphic plane

	jsr concat              // calculate offset
        adda.l  d1,a1           // get destination address

/*
 *   optimize shift count and direction: choose shift routine
 */
        move.w  d0,d6                   // get the shift count for bit alignment
        cmpi.w  #8,d6                   // > than 8 shifts needed for alignment?
        bcc.b   shift_lf                // >8 => select left shift routine
        lea     align_rt,a3             // <= 8 => select right shift routine
        bra.b   init
shift_lf:
        lea     align_lf,a3             // get address of left alignment routine
        moveq.l #16,d6                  // compensate for left shift
        sub.w   d0,d6                   // shift count for left alignment
/*
 *              a. save area address
 *              b. save area length (per plane)
 *              c. use table to load fetch/store pointers
 */
init:
        move.l  rtn_tab+00(pc,d7.w),a5  // get starting address of store routine
        move.l  rtn_tab+04(pc,d7.w),a6  // get starting address of fetch routine
        move.w  _v_planes,d7            // get number of graphic planes
#if rbp
        moveq.l #0,d3
        move.w  d7,d3
        add.w   d3,d3                   // # bytes between words in same plane
#else
        move.l  _v_pl_dspl,d3           // get # bytes between graphic planes
#endif

        move.l  a1,save_addr            // save area: origin of material
        move.w  d5,save_len             //           number of rows
        
        lea     save_area,a2            // a2 -> save area buffer
        bset.b  #F_SAVRDY,(a4)          // flag the buffer as being loaded

#define curs_parm               12              // bytes needed on stack for temp. vars
#define cdb_fg                  0               // mouse foreground colors
#define cdb_bg                  2               // mouse background colors
#define crow_cnt                4               // number of cursor rows to be displayed
#define cpl_size                6               // number of bytes in cursor plane
#define d_addr                  8               // destination starting address

        adda.w  #-curs_parm,sp          // get temporary stack space
        move.w  m_cdb_fg,cdb_fg(sp)     // get copy of mouse foreground colors
        move.w  m_cdb_bg,cdb_bg(sp)     // get copy of mouse background colors
        move.w  d5,d1                   // cursor row count
        lsl.w   #2,d1                   // get # of cursor bytes per plane
        move.w  d5,crow_cnt(sp)         // get copy # of displayable cur. rows
        move.w  d1,cpl_size(sp)         // copy of cursor bytes per plane
        move.l  a1,d_addr(sp)           // get copy of destination address
        subq.w  #1,d7                   // adjust plane count for loop
        bra.b   p_loop

rtn_tab:
        dc.l    store_lw,enter_lw,store_rt,enter_rt,store_lf,enter_lf
.page

/* plane controller */
new_plane:
        move.w  crow_cnt(sp),d5         // get # of displayable cursor rows
        suba.w  cpl_size(sp),a0         // starting address of cursor source

#if rbp
        addq.l  #2,d_addr(sp)           // update destination pointer
                                        // FIXME: Use d3 here, too???
#else
        add.l   d3,d_addr(sp)           // get address of next graphics plane
#endif
        movea.l d_addr(sp),a1           // current destination address

 *  Logic routine selection is based on the foreground and background colors
 *  for each plane.
 *
p_loop:
        clr.w   d0                      // use for selecting logic op entry
        lsr.w   cdb_fg(sp)              // get foreground color for this plane
        addx.w  d0,d0                   // transfer foreground color
        lsr.w   cdb_bg(sp)              // get background color for this plane
        addx.w  d0,d0                   // transfer background color
        asl.w   #2,d0                   // convert byte index to long word index
        movea.l fgbg_op(pc,d0.w),a4     // get logic op entry address

        jmp     (a6)                    // enter fetch-logic op-store loop

p_next_plane:

        dbra    d7,new_plane            // draw cursor in next graphic plane
        adda.w  #curs_parm,sp           // remove temp. variables from stack
        movem.l (sp)+,d2-d7/a2-a6       // restore 'C' registers
        rts
/*
 *  Entry addresses for the logic operation routines.
 */
fgbg_op:
#if rev_vid
        dc.l    fg0bg0                  // foreground = 0, background = 0
        dc.l    fg0bg1                  // foreground = 0, background = 1
        dc.l    fg1bg0                  // foreground = 1, background = 0
        dc.l    fg1bg1                  // foreground = 1, background = 1
#else
        dc.l    fg1bg1                  // foreground = 0, background = 0
        dc.l    fg1bg0                  // foreground = 0, background = 1
        dc.l    fg0bg1                  // foreground = 1, background = 0
        dc.l    fg0bg0                  // foreground = 1, background = 1
#endif



/*
 *    fetching and saving a destination long word
 *
 *  in:
 *      a0.l    points to start of BG/FG form
 *      a1.l    points to start of destination
 *      a2.l    points to start of save area
 *      a3.l    points to alignment fragment
 *
 *      d3.w    offset to next word
 *      d4.w    form wrap offset
 */


/* fetch/store/advance row:  long word */
fetch_lw:
#if rbp
        move.w  (a1),d2
        swap    d2
        move.w  (a1,d3.w),d2            // d2(31:00) <- destination
#else
        move.l  (a1),d2
#endif
        move.l  d2,(a2)+                // save original destination

        jmp     (a3)                    // a3 -> alignment routine

store_lw:
#if rbp
        move.w  d2,(a1,d3.w)
        swap    d2
        move.w  d2,(a1)                 // send long word to screen
#else
        move.l  d2,(a1)
#endif
        add.w   d4,a1                   // a1 -> next row of destination

enter_lw:

        dbra    d5,fetch_lw
        bra.w   p_next_plane

/* fetch/store/advance row:  right word only */

fetch_rt:

        move.w  (a1),d2                 // fetch destination
        move.w  d2,(a2)+                // save it for later

        jmp     (a3)                    // a3 -> alignment routine

store_rt:

        move.w  d2,(a1)                 // send new destination to screen
        add.w   d4,a1                   // a1 -> next row of destination

enter_rt:

        dbra    d5,fetch_rt
        bra.w   p_next_plane
/*
 *      fetch/store/advance row:  left word only
 */
fetch_lf:

        move.w  (a1),d2                 // fetch destination
        move.w  d2,(a2)+                // save it for later
        swap    d2                      // d2(31:16) <- destination word

        jmp     (a3)                    // a3 -> alignment routine

store_lf:

        swap    d2
        move.w  d2,(a1)                 // send new destination to screen
        add.w   d4,a1                   // a1 -> next row of destination

enter_lf:

        dbra    d5,fetch_lf
        bra.w   p_next_plane



/*
 *    fetch and align BG and FG form
 *
 *  in:
 *           d6.w       shift count
 *
 *           a0.l       points to BG/FG form word of intrest
 *           a4.l       points to logical operation fragment
 *
 * out:
 *           d0.l       alligned background form padded with 0's
 *           d1.l       alligned foreground form padded with 0's
 *
 *           a0.l       points to next BG/FG form word
 *
 *      align BG form and FG form: rotate left  (short left shift)
 */
align_lf:
        moveq.l #00,d0                  // pad background form with zeros
        move.w  (a0)+,d0                // a0 -> foreground word
        rol.l   d6,d0                   // d0(31:00) <- aligned background word

        moveq.l #00,d1                  // pad foreground form with zeros
        move.w  (a0)+,d1                // a0 -> next background word
        rol.l   d6,d1                   // d1(31:00) <- aligned foreground word

        jmp     (a4)                    // a4 -> logic fragment
/*
 *      align BG form and FG form: rotate right  (long left shift)
 */
align_rt:
        moveq.l #0,d0                   // pad background form with zeros
        move.w  (a0)+,d0                // a0 -> foreground word
        swap    d0
        ror.l   d6,d0                   // d0(31:00) <- aligned background word

        moveq.l #00,d1                  // pad foreground form with zeros
        move.w  (a0)+,d1                // a0 -> next background word
        swap    d1
        ror.l   d6,d1                   // d1(31:00) <- aligned foreground word

        jmp     (a4)                    // a4 -> logic fragment



/*
 *    logical operation for cursor interaction with destination
 *
 *  in:
 *      d0.l    alligned background form padded with 0's
 *      d1.l    alligned foreground form padded with 0's
 *      d2.l    DESTination
 *
 *      a5.l    points to storage code fragment
 */
fg0bg0:
        not.l   d0
        and.l   d0,d2
        not.l   d1
        and.l   d1,d2
        jmp     (a5)

fg0bg1:
        or.l    d0,d2
        not.l   d1
        and.l   d1,d2
        jmp     (a5)

fg1bg0:
        not.l   d0
        and.l   d0,d2
        or.l    d1,d2
        jmp     (a5)

fg1bg1:
        or.l    d0,d2
        or.l    d1,d2
        jmp     (a5)



/*
 * cur_replace - replace cursor with data in save area.
 *
 *  in:
 *
 *      save_area       memory where saved data resides
 *      save_addr       points to destination
 *      save_len        number of lines to be returned
 *      save_stat       status and format of save buffer
 *      _v_planes       number of planes in destination
 *      _v_line_wr      line wrap (byte width of form)
 *
 *      clobbered:      d2.w,d3.w,d4.w,d5.w,a0.l,a1.l,a2.l,a3.l,a4.l,a5.l
 */

cur_replace:
        movem.l d2-d5/a2-a5,-(sp)       // save registers used by 'C'
        bclr.b  #F_SAVRDY,save_stat     // does save area contain valid data ?
        beq     hang_it_up              // 0:nothing saved

#if 0 //rbp   //FIXME!
        move.w  save_len,d4             // d4 <- number of lines per plane
        move.w  d4,d2
        subq.w  #1,d2                   // d2 <- adjust counter for dbra
#else
        move.w  save_len,d4             // get number of cursor lines per plane
        subq.w  #1,d4                   // adjust for loop counter
#endif
        move.w  _v_lin_wr,d3            // d3 <- dy wrap value

        move.l  save_addr,a0            // a0 -> destination
        lea     save_area,a1            // a1 -> save area

        move.w  _v_planes,d5            // d5 <- resolution discriminator
#if 0 //rbp   //FIXME - the routine only crashes, so we're using the vme10 routine instead
        subq.w  #2,d5
        bgt     lo_rez
        beq     md_rez

hi_rez:

        btst.b  #F_SAVWID,save_stat     // word or longword ?
        bne     hi_lw_loop

/* hi rez: word wide */

hi_wd_loop:

        move.w  (a1)+,(a0)
        add.w   d3,a0

        dbra    d2,hi_wd_loop

        rts

/* hi rez: long word */

hi_lw_loop:

        move.l  (a1)+,(a0)
        add.w   d3,a0

        dbra    d2,hi_lw_loop

        rts

md_rez:
        move.l  a0,a5
        add.w   d4,d4                   // d4 <- word offset

        move.l  a1,a2                   // a2 -> second plane (word)
        add.w   d4,a2


        btst.b  #F_SAVWID,save_stat     // word or longword ?
        bne     md_lw_enter

/* medium rez: word wide */

md_wd_loop:

        move.w  (a1)+,(a0)+             // store plane 1
        move.w  (a2)+,(a0)              // store plane 2

        add.w   d3,a5
        move.l  a5,a0                   // a0 -> next screen line

        dbra    d2,md_wd_loop

        rts

/* medium rez: longword */

md_lw_enter:

        add.w   d4,a2                   // a2 -> second plane (longword)

md_lw_loop:

        move.w  (a1)+,(a0)+             // 1st plane, hi word
        move.w  (a2)+,(a0)+             // 2nd plane, hi word
        move.w  (a1)+,(a0)+             // 1st plane, lo word
        move.w  (a2)+,(a0)              // 2nd plane, lo word

        add.w   d3,a5
        move.l  a5,a0                   // a0 -> next line

        dbra    d2,md_lw_loop

        rts

lo_rez:

        add.w   d4,d4                   // d4 <- offset to next plane (word wide)

        move.l  a0,a5                   // a5 -> current line in plane 1

        btst.b  #F_SAVWID,save_stat     // word or longword ?
        bne     lo_lw_enter

/* lo rez: word */

        move.l  a1,a2
        add.w   d4,a2                   // a2 -> 2nd plane
        move.l  a2,a3
        add.w   d4,a3                   // a3 -> 3rd plane
        move.l  a3,a4
        add.w   d4,a4                   // a4 -> 4th plane

lo_wd_loop:

        move.w  (a1)+,(a0)+             // store 1st plane
        move.w  (a2)+,(a0)+             // store 2nd plane
        move.w  (a3)+,(a0)+             // store 3rd plane
        move.w  (a4)+,(a0)              // store 4th plane

        add.w   d3,a5
        move.l  a5,a0                   // a0 -> next line

        dbra    d2,lo_wd_loop

        rts

/* lo rez: long words */

lo_lw_enter:

        add.w   d4,d4                   // d4 <- plane offset (longword wide)

        move.l  a1,a2                   // set up pointers to each plane
        add.w   d4,a2
        move.l  a2,a3
        add.w   d4,a3
        move.l  a3,a4
        add.w   d4,a4

lo_lw_loop:

        move.w  (a1)+,(a0)+             // 1st plane, hi word
        move.w  (a2)+,(a0)+             // 2nd plane, hi word
        move.w  (a3)+,(a0)+             // 3rd plane, hi word
        move.w  (a4)+,(a0)+             // 4th plane, hi word
        move.w  (a1)+,(a0)+             // 1st plane, lo word
        move.w  (a2)+,(a0)+             // 2nd plane, lo word
        move.w  (a3)+,(a0)+             // 3rd plane, lo word
        move.w  (a4)+,(a0)              // 4th plane, lo word

        add.w   d3,a5
        move.l  a5,a0                   // a0 -> next line

        dbra    d2,lo_lw_loop

#else

        subq.w  #1,d5                   // adjust plane count for loop
        movea.l a0,a5                   // save destination address
#if vme10
        move.l  _v_pl_dspl,a3           // get # of bytes between gr. planes
#else   // Ugly hack for the Atari:
        move.l  #0,a3
        move.w  _v_planes,a3
        add.w   a3,a3
#endif
        move.w  d4,d2                   // save # of lines/plane
        btst.b  #F_SAVWID,save_stat     // word or longword transfer?
        bne.b   lw_1st_line             // for longword transfer
        bra.b   wd_1st_line             // for word transfer

v_wd_plane:
        adda.l  a3,a5                   // point to next graphics plane
        movea.l a5,a0                   // new destination address
        move.w  d4,d2                   // restore # of lines to be written
        bra.b   wd_1st_line             // restore 1st dest.line for this plane
v_wd_loop:
        adda.w  d3,a0                   // next scan line of destination
wd_1st_line:
        move.w  (a1)+,(a0)              // restore one destination line
        dbra    d2,v_wd_loop            // next line in save buffer
        dbra    d5,v_wd_plane           // set up for next graphics plane
        movem.l (sp)+,d2-d5/a2-a5       // restore 'C' registers
        rts

v_lw_plane:
        adda.l  a3,a5                   // point to next graphics plane
        movea.l a5,a0                   // new destination address
        move.w  d4,d2                   // restore # of lines to be written
        bra.b   lw_1st_line             // restore 1st dest.line for this plane
v_lw_loop:
        adda.w  d3,a0                   // next scan line of destination
lw_1st_line:
        move.l  (a1)+,(a0)              // restore one destination line
        dbra    d2,v_lw_loop            // next line in save buffer
        dbra    d5,v_lw_plane           // set up for next graphics plane
        movem.l (sp)+,d2-d5/a2-a5       // restore 'C' registers
        rts
#endif


hang_it_up:

        rts

/*
 *    hide_cur
 *
 *      This routine hides the mouse cursor if it has not already
 *      been hidden.
 *
 *      Inputs:         None
 *
 *      Outputs:
 *         hide_cnt = hide_cnt + 1
 *         draw_flag = 0
 *
 *      Registers Modified: a0
 *
 *
 *
 *      Increment the counter for the number of hide operations performed.
 *      If this is the first one then remove the cursor from the screen.
 *      If not then do nothing because the cursor isn't on the screen any
 *      more.
 */
_HIDE_CUR:
        add.b   #1,_mouse_flag  // disable mouse redrawing
        lea     _HIDE_CNT,a0    // get address of hide operations counter
        addq.w  #1,(a0)         // increment it
        cmpi.w  #1,(a0)         // is cursor already hidden?
        bne.b   hid_cur1        // yes - done

/*
 *      Remove the mouse cursor from the screen and disable the vertical
 *      blank cursor drawing routine.
 */
        bsr     cur_replace     // remove the cursor
        clr.b   _draw_flag      // disable vb_draw
hid_cur1:
        sub.b   #1,_mouse_flag  // re-enable mouse drawing
        rts



/*
 *    dis_cur
 *
 *      This routine displays the mouse cursor if the number of
 *      hide operations has gone back to 0.
 *
 *      Inputs:         None
 *
 *      Outputs:
 *         hide_cnt = hide_cnt - 1
 *         draw_flag = 0
 *
 *      Registers Modified:     d0, d1
 */

/*
 *      Decrement the counter for the number of hide operations performed.
 *      If this is not the last one then do nothing because the cursor
 *      should remain hidden.
 */
_DIS_CUR:
        add.b   #1,_mouse_flag  // disable mouse redrawing
        subq.w  #1,_HIDE_CNT    // decrement hide operations counter
        bgt.b   dis_cur1        // still greater than 0 - do nothing
        bmi.b   fix_neg         // negative - cursor already displayed

/*
 *      Get the coordinates of the current cursor position and display it.
 */
        move.w  _GCURX,d0       // get cursor x-coordinate
        move.w  _GCURY,d1       // get cursor y-coordinate
        //move.w  50,d0       // get cursor x-coordinate
        //move.w  50,d1       // get cursor y-coordinate
        bsr     cur_display     // display the cursor
        clr.b   _draw_flag      // disable vb_draw
fix_neg:
        clr.w   _HIDE_CNT       // clear the hide operations counter
dis_cur1:
        sub.b   #1,_mouse_flag  // re-enable mouse drawing
        rts



/*
 *      xfm_crfm
 *
 *      This routine transforms a user defined cursor to device
 *      specific format.
 *
 *      Inputs:
 *         intin[0] - x coordinate of hot spot
 *         intin[1] - y coordinate of hot spot
 *         intin[2] - reserved for future use. must be 1
 *         intin[3] - Mask color index
 *         intin[4] - Data color index
 *         intin[5-20]  - 16 words of cursor mask
 *         intin[21-36] - 16 words of cursor data
 *
 *      Outputs:        None
 *
 *      Registers Modified:   d0, a0-a3
 */

/*
 *      Get the new values for the x and y-coordinates of the mouse hot
 *      spot and the new color indices for the mouse mask and data.
 */
_XFM_CRFM:
        movem.l a2-a3,-(sp)
        add.b   #1,_mouse_flag  // disable updates while redefining cursor
        move.l  _INTIN,a0       // get address of new cursor definition block
        lea     mouse_cdb,a1    // get address of old cursor definition block
        move.w  (a0)+,d0        // get x-offset of mouse hot spot
        andi.w  #0x000f,d0      // get offset mod 16
        move.w  d0,m_pos_hx     // save x-offset of mouse hot spot
        move.w  (a0)+,d0        // get y-offset of mouse hot spot
        andi.w  #0x000f,d0      // get offset mod 16
        move.w  d0,m_pos_hy     // save y-offset of mouse hot spot
        addq.w  #2,a0           // skip over plane count
        move.w  (a0)+,d0        // get the mask color index
        lea     _DEV_TAB+26,a3  // get address of maximum color index
        cmp.w   (a3),d0         // is index too high?
        bmi     xfm_bg_col_ok   // nope - accept it as is
        moveq.l #1,d0           // yes - default to color index of 1
xfm_bg_col_ok:
        lea     _MAP_COL,a1     // get address of color index mapping table
        add.w   d0,d0           // convert index to word offset
        move.w  (a1,d0.w),m_cdb_bg      // get bit plane masks for mouse mask
        move.w  (a0)+,d0        // get mouse data color index
        cmp.w   (a3),d0         // is the index too high?
        bmi     xfm_fg_col_ok   // nope - accept it as is
        moveq.l #1,d0           // yes - default to color index of 1
xfm_fg_col_ok:
        add.w   d0,d0           // convert index to word offset
        move.w  (a1,d0.w),m_cdb_fg      // get bit plane masks for mouse data

/*
 *      Move the new mouse defintion into the global mouse cursor definition
 *      table.  The values for the mouse mask and data are supplied as two
 *      separate 16-word entities.  They must be stored as a single array
 *      starting with the first word of the mask followed by the first word
 *      of the data and so on.
 */
        moveq.l #15,d0          // initialize the loop counter
        lea.l   mask_form,a1    // get address of global mouse definition table
        lea.l   32(a0),a2       // get address of mouse data
xfm_move_loop:
        move.w  (a0)+,(a1)+     // get next word of mask
        move.w  (a2)+,(a1)+     // get next word of data
        dbra    d0,xfm_move_loop        // loop until all data is stored
        sub.b   #1,_mouse_flag  // re-enable mouse drawing
        movem.l (sp)+,a2-a3
        rts

        .data



/* Default Mouse Cursor Definition */

arrow_cdb:
        .dc.w   1,0,1,0,1
        .dc.w   0xE000 //%1110000000000000       // background definition
        .dc.w   0xF000 //%1111000000000000
        .dc.w   0xF800 //%1111100000000000
        .dc.w   0xFB00 //%1111110000000000
        .dc.w   0xFE00 //%1111111000000000
        .dc.w   0xFF00 //%1111111100000000
        .dc.w   0xFF80 //%1111111110000000
        .dc.w   0xFFB0 //%1111111111000000
        .dc.w   0xFE00 //%1111111000000000
        .dc.w   0xFE00 //%1111111000000000
        .dc.w   0xEF00 //%1110111100000000
        .dc.w   0x0F00 //%0000111100000000
        .dc.w   0x0780 //%0000011110000000
        .dc.w   0x0780 //%0000011110000000
        .dc.w   0x03B0 //%0000001111000000
        .dc.w   0x0000 //%0000000000000000

        .dc.w   0x4000 //%0100000000000000       // foreground definition
        .dc.w   0x6000 //%0110000000000000
        .dc.w   0x7000 //%0111000000000000
        .dc.w   0x7800 //%0111100000000000
        .dc.w   0x7B00 //%0111110000000000
        .dc.w   0x7E00 //%0111111000000000
        .dc.w   0x7F00 //%0111111100000000
        .dc.w   0x7F80 //%0111111110000000
        .dc.w   0x7B00 //%0111110000000000
        .dc.w   0x6B00 //%0110110000000000
        .dc.w   0x4600 //%0100011000000000
        .dc.w   0x0600 //%0000011000000000
        .dc.w   0x0300 //%0000001100000000
        .dc.w   0x0300 //%0000001100000000
        .dc.w   0x0180 //%0000000110000000
        .dc.w   0x0000 //%0000000000000000



        .end
