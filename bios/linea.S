
// ===========================================================================
// ==== linea.s - linea graphics stuff 
// ===========================================================================
//
// Copyright (c) 2001 Martin Doering.
//
// Authors:
//  MAD  Martin Doering
//
// This file is distributed under the GPL, version 2 or at your
// option any later version.  See doc/license.txt for details.
//



// ==== Defines ==============================================================

//
//       font header structure equates.
//

        .equ    FIRST,          36
        .equ    LAST,           38
        .equ    CEL_WD,         52
        .equ    POFF,           72
        .equ    PDAT,           76
        .equ    FRM_WD,         80
        .equ    FRM_HT,         82

// ==== External Declarations ================================================

        .global _linea_init     // initialize linea graphics subsystem
        .global int_linea       // entry for linea exception

// ==== References ===========================================================

        .xdef   _f8x8           // font header
        .xdef   _f8x16          // font header
        .xdef   sshiftmod       // mode/bits of shift/modifyer keys

        .xdef normal_ascii      // start with default function


// ==== Line-A handler ===============================
int_linea:
	move.l  2(sp),a0
	move.l  a0,a1
	clr.l   d0
	move.w  (a0)+,d0
	move.l  a0,2(sp)
	and.w   #0xFFF,d0
	tst.w   d0
	bmi     wrong_linea
	cmp.w   linea_ents,d0
	bpl	wrong_linea
	lea     linea_vecs,a0
	lsl.w   #2,d0
	move.l  0(a0,d0),a0
	jsr     (a0)
linea_dispatch_pc:	
	rte

wrong_linea:
	move.w  d0,-(sp)
	sub.w   #2,a0
	move.l  a0,-(sp)
	pea	wrong_linea_msg
	jsr     _kprintf
	add.w	#10,sp
	rte
wrong_linea_msg:	
	.ascii  "pc=0x%08lx: Line-A call number 0x%03x out of bounds\n\0"
	.even



_linea_0:	
	
//        bsr     _linea_init       // init again global linea and font variables.
//                                // no reset of screen layout patched by emus
        
	lea	line_a_vars,a0  // get base address for line a variables
	move.l  a0,d0

	move.l  font_ring,a1	// get pointer to the three system font headers
	move.l  (a1),a1
	
	lea     linea_vecs,a2	// get pointer to table of the Line-A routines
	rts

//
// These are stubs for linea :
// the stub will print the pc of the caller, whether the function
// was called using the line a opcode, or directly via its address.
//

_linea_1:
	move.w	#1,d0
	bra	linea_stub



_linea_2:
	move.w	#2,d0
	bra	linea_stub



_linea_3:
	move.w	#3,d0
	bra	linea_stub



_linea_4:
	move.w	#4,d0
	bra	linea_stub



_linea_5:
	move.w	#5,d0
	bra	linea_stub



_linea_6:
	move.w	#6,d0
	bra	linea_stub



_linea_7:
	move.w	#7,d0
	bra	linea_stub



_linea_8:
	move.w	#8,d0
	bra	linea_stub



_linea_9:
	move.w	#9,d0
	bra	linea_stub



_linea_a:
	move.w	#0xa,d0
	bra	linea_stub



_linea_b:
	move.w	#0xb,d0
	bra	linea_stub



_linea_c:
	move.w	#0xc,d0
	bra	linea_stub



_linea_d:
	move.w	#0xd,d0
	bra	linea_stub



_linea_e:
	move.w	#0xe,d0
	bra	linea_stub



_linea_f:
	move.w	#0xf,d0
	bra	linea_stub



linea_stub:
	move.l  (sp),d1
	sub.l   #linea_dispatch_pc,d1
	and.l   #0xFFFFFF,d1
	bne	1f
	move.l  a1,a0
	bra     2f
1:	move.l  (sp),a0
2:	move.w  d0,-(sp)
	move.l  a0,-(sp)	
	pea	linea_stub_msg
	jsr	_kprintf
	add.w	#10,sp
	rts
linea_stub_msg:
	.ascii	"pc=0x%08lx: unimplemented Line-A call number 0x%03x\n\0"
	.even
	


linea_vecs:
	dc.l	_linea_0
	dc.l	_linea_1
	dc.l	_linea_2
	dc.l	_linea_3
	dc.l	_linea_4
	dc.l	_linea_5
	dc.l	_linea_6
	dc.l	_linea_7
	dc.l	_linea_8
	dc.l	_linea_9
	dc.l	_linea_a
	dc.l	_linea_b
	dc.l	_linea_c
	dc.l	_linea_d
	dc.l	_linea_e
	dc.l	_linea_f
linea_ents:
	dc.w    (linea_ents-linea_vecs)/4



// gl_f_init - font globals initialization routine ===========================
//
// input:
//   a0 = ptr to system font header
//

gl_f_init:
        move    FRM_HT(a0),d0           // fetch form height.
        move    d0,v_cel_ht             // init cell height.
        move    _v_lin_wr,d1            // fetch bytes/line.
        mulu    d0,d1
        move    d1,v_cel_wr             // init cell wrap.
        moveq.l #0,d1
        move    v_vt_rez,d1             // fetch vertical res.
        divu    d0,d1                   // vert res/cell height.
        subq    #1,d1                   // 0 origin.
        move    d1,v_cel_my             // init cell max y.
        moveq.l #0,d1
        move    v_hz_rez,d1             // fetch horizontal res.
        divu    CEL_WD(a0),d1           // hor res/cell width.
        subq    #1,d1                   // 0 origin.
        move    d1,v_cel_mx             // init cell max x.
        move    FRM_WD(a0),v_fnt_wr     // init font wrap.
        move    FIRST(a0),v_fnt_st      // init font start ADE.
        move    LAST(a0),v_fnt_nd       // init font end ADE.
        move.l  PDAT(a0),v_fnt_ad       // init font data ptr.
        move.l  POFF(a0),v_off_ad       // init font offset ptr.
        rts



// ==== _linea_init - escape initialization routine.============================

	
// LVL:	 checked that this routine only clobbers 'scratch' registers.
// 	 if this routine calls other routines, please ensure this
//        remains true.

_linea_init:
        move.b  sshiftmod, d0           // get video resolution
        and.b   #3, d0                  // isolate bits 0 and 1
        cmp.b   #3, d0                  // is it 3 - color?
        bne     not3                    // no
        move.w  #2, d0                  // set monochrome resolution
not3:
        move.w  d0, -(sp)               // save resolution
        bsr     resolset                // set video resolution
        move.w  (sp)+, d0               // restore resolution
        
vars_init:
        lea     _f8x8, a0               // Get pointer to 8x8 font header
        cmp.b   #2, d0                  // High resolution?
        bne     lowres                  // no, low resolution
        lea     _f8x16, a0              // Get pointer to 8x16 font header
lowres:
        bsr     gl_f_init               // init the global font variables.

        move.w  #-1, v_col_fg           // foreground color := 15.
        moveq.l #0, d0
        move.w  d0, v_col_bg            // background color := 0.
        
        move.w  d0, v_cur_cx            // cursor column 0
        move.w  d0, v_cur_cy            // cursor line 0
        move.w  d0, v_cur_of            // line offset 0
        move.l  _v_bas_ad, a0           // get base address of screen
        move.l  a0, v_cur_ad            // home cursor.
        move.b  #1, v_stat_0            // invisible, flash, nowrap, normal video.
        move.b  #30, v_cur_tim          // .5 second blink counter (@60 Hz vblank).
        move.b  #30, v_period           // .5 second blink rate (@60 Hz vblank).
        move.w  #1, disab_cnt           // cursor disabled 1 level deep.

        // ==== Clear screen =====
        move.w  v_col_bg, d0            // load background color
        move.l  _memtop, a0             // Set start of RAM
scr_loop:
        move.w  d0, (a0)+               // set to background color
        cmp.l   phystop, a0             // End of BSS reached?
        bne     scr_loop                // if not, clear next word

        move.l  #normal_ascii, con_state // Init conout state machine

        rts



// ==== resolset - set video resolution ======================================

resolset:
        moveq.l #0, d1                  

        move.b  splanes(pc, d0.w), d1   // Get the number of planes
        move.w  d1, _v_planes           // save it

        move.b  sbytes(pc, d0.w), d1    // Get the number of bytes per line
        move.w  d1, _v_lin_wr           // Set the line wrap

        asl.w   #1, d0                  // resolution as word index
        move.w  sresoly(pc, d0.w), d1   // Get the vertical resolution
        move.w  d1, v_vt_rez            // Set it

        move.w  sresolx(pc, d0.w), d1   // Get the horizontal resolution
        move.w  d1, v_hz_rez            // Set it

        rts


// ==== screenpar - Screen parameters ========================================

splanes:
        dc.b    4,2,1                   // count of color planes (_v_planes)
sbytes:
        dc.b    160,160,80              // bytes per line (_v_lin_wr)
sresoly:
        dc.w    200,200,400             // screen vertical resolution (v_vt_rez)
sresolx:
        dc.w    320,640,640             // screen horiz resolution (v_hz_rez)
        


// ===========================================================================
// ==== End ==================================================================
// ===========================================================================

        .end




// ==== Set temporary screenmem address 0x10000 to videoshifter ==============
        
        move.b  #0x1, 0xffff8201 // set hw video base high word
        move.b  #0x0, 0xffff8203 // set hw video base low word



// ==== Detect and set graphics resolution ===================================

        move.b 0xffff8260, d0     // Get video resolution from pseudo Hw
        and.b #3,d0             // Isolate bits 0 and 1
        cmp.b #3,d0             // Bits 0 and 1 set = invalid
        bne.s setscrnres        // no -->
        moveq #2,d0             // yes, set highres, make valid
setscrnres:
        move.b d0, sshiftmod    // Set in sysvar

        move.b #2, 0xffff8260     // Hardware set to highres
        move.b #2, sshiftmod    // Set in sysvar

//       jsr 0xfca7c4            // Init screen (video driver???)

        cmp.b #1, sshiftmod             // middle resolution?
        bne.s initmidres                // nein, -->
        move.w 0xffff825e, 0xffff8246   // Copy Color 16->4 kopieren

initmidres:
        move.l  #_main, swv_vec // Set Swv_vec (vector res change) to Reset
        move.w  #1, vblsem      // vblsem: VBL freigeben


//       pea 0xfffffa00  // Print, what's going on
//       bsr _kputb
//       addq #4,sp
//
//       btst    #7,0xfffffa01   // detect b/w-monitor pin
//       beq     low_rez         // if bit set, color monitor 
//
//       move.l  #2,d0           // monochrome mode
//       bra     both_rez
//low_rez:
//       move.l #0,d0
//both_rez:
//       move.b d0, sshiftmod    // set mode sysvar
//       move.w d0, 0xFFFF8260   // and to shifter register

// ==== Set videoshifter address to screenmem ================================
        
        move.l  phystop, a0     // get memory top
        sub.l   #0x8000, a0     // minus screen mem length
        move.l  a0, _v_bas_ad   // set screen base

        move.b  _v_bas_ad+1, 0xffff8201 // set hw video base high word
        move.b  _v_bas_ad+2, 0xffff8203 // set hw video base low word


        pea msg_shift   // Print, what's going on
        bsr _kprint
        addq #4,sp

