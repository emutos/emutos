
|########################################################################
|                                                                       #
|       "vdi_tblit_cf.S" - Translated from                              #
|               file "vdi_tblit_preprocessed.s"                         #
|                                                                       #
|-----------------------------------------------------------------------#
|                                                                       #
|       PortAsm Code Translator Copyright (c) MicroAPL 1990-2004        #
|                   All Rights Reserved Worldwide                       #
|                                                                       #
|########################################################################
| Translated on Wed Sep 13 20:47:45 2017 by version 2.6.0 of PortAsm translator
| PortAsm Licence Number #3250
| Target assembler:    Gnu 'gas'
|
| Non-default options specified:
|       -a gnu              - Specify target assembler
|       -blanks on          - Allow/Disallow blanks in source operand
|       -core v4            - Specify processor core version number
|       -hardware_divide on
|                           - Target CPU has hardware-divide unit
|       -hardware_mac on    - Target CPU has multiply-accumulate unit
|       -noerrfile on       - Suppress .err file, errors to STDERR only
|       -nowarning 402,502,900,1111,1150
|                           - Suppress specified warnings
|       -o vdi_tblit_cf.S   - Filename for output file
|       -out_syntax standard
|                           - Specify output instruction syntax
|

|##################################################
| Additional equates for translation              #
|##################################################
	.text

|########################################################################
| Start of translated code                                              #
|########################################################################
| 1 "vdi/vdi_tblit.S"
| 1 "<built-in>"
| 1 "<command-line>"
| 1 "vdi/vdi_tblit.S"
| 14 "vdi/vdi_tblit.S"
| 1 "include/asmdefs.h" 1
| 20 "include/asmdefs.h"
| 1 "include/config.h" 1
| 21 "include/asmdefs.h" 2
| 15 "vdi/vdi_tblit.S" 2
| 1 "vdi/vdi_asm.h" 1
| 16 "vdi/vdi_tblit.S" 2
| 155 "vdi/vdi_tblit.S"
|;;           .globl _normal_blit      | [14]
	.globl	_normal_blit
|;;           .globl _outline          | [15]
	.globl	_outline
|;;           .globl _rotate           | [16]
	.globl	_rotate
|;;           .globl _scale            | [17]
	.globl	_scale
| 169 "vdi/vdi_tblit.S"
|;;           .extern _DEV_TAB         | [19]
|;;           .extern _SOURCEX,_SOURCEY | [20]
	.extern	_SOURCEX
	.extern	_SOURCEY
|;;           .extern _DESTX,_DESTY    | [21]
|;;           .extern _DELX,_DELY      | [22]
|;;           .extern _FBASE           | [23]
|;;           .extern _FWIDTH          | [24]
|;;           .extern _WRT_MODE        | [25]
|;;           .extern _STYLE           | [26]
|;;           .extern _R_OFF,_L_OFF    | [27]
|;;           .extern _WEIGHT,_LITEMASK,_SKEWMASK | [28]
	.extern	_LITEMASK
|;;           .extern _SCALE           | [29]
|;;           .extern _CHUP            | [30]
|;;           .extern _CLIP            | [31]
|;;           .extern _MONO_STATUS     | [32]
|;;           .extern _T_SCLSTS        | [33]
	.extern	_T_SCLSTS
|;;           .extern _TEXT_FG         | [34]
|;;           .extern _XMN_CLIP,_XMX_CLIP | [35]
|;;           .extern _YMN_CLIP,_YMX_CLIP | [36]
|;;           .extern _XACC_DDA        | [37]
	.extern	_XACC_DDA
|;;           .extern _DDA_INC         | [38]
	.extern	_DDA_INC
|;;           .extern _SCRPT2          | [39]
	.extern	_SCRPT2
|;;           .extern _SCRTCHP         | [40]
	.extern	_SCRTCHP
|;;           .extern _shift_offset    | [41]






|;;           .extern _v_bas_ad        | [48]
|;;           .extern _v_lin_wr        | [49]
|;;           .extern _v_planes        | [50]





|;;           .globl _deftxbuf         | [56]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [57]
	.globl	_scrtsiz
| 342 "vdi/vdi_tblit.S"
|;;           .bss                     | [59]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [61]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [63]
	.p2align 2,0
	.text



|;;           .even                    | [67]
	.p2align 1,0
_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [69]
	.short	(2+6+8+15)/16*2*16*2
| 459 "vdi/vdi_tblit.S"
|;;           .text                    | [71]






	.p2align 1,0
_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [79]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [80]
      movea.l  (44+8,%a7),%a0          | [81]
      movea.l  (44+12,%a7),%a1         | [82]
      jbsr     norm_blt                | [83]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [84]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [85]


_outline:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [89]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [90]
      movea.l  (44+8,%a7),%a0          | [91]
      move.w   (44+12,%a7),%d7         | [92]
      jbsr     outlin                  | [93]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [94]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [95]


_rotate:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [99]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [100]
      jbsr     rotation                | [101]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [102]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [103]


_scale:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [107]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [108]
      jbsr     replicat                | [109]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [110]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [111]


|;;           .text                    | [114]
| 511 "vdi/vdi_tblit.S"
|;;                   .even            | [116]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [117]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [118]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [119]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [120]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [121]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [122]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [123]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [124]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [125]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [126]
	.short	0x007F
|;;                   .dc.w 0x003F     | [127]
	.short	0x003F
|;;                   .dc.w 0x001F     | [128]
	.short	0x001F
|;;                   .dc.w 0x000F     | [129]
	.short	0x000F
|;;                   .dc.w 0x0007     | [130]
	.short	0x0007
|;;                   .dc.w 0x0003     | [131]
	.short	0x0003
|;;                   .dc.w 0x0001     | [132]
	.short	0x0001
|;;                   .dc.w 0x0000     | [133]
	.short	0x0000

|;;           .text                    | [135]
| 543 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [138]
|;;   sub.w     -24(a6),d1             | [139]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [140 PL]
      jpl      do_rot                  | [141]



|;;   neg.w     d1                     | [145]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [146]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [147]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [149]

|;;   lsl.w     #1,d0                  | [151]
	move.w   %d0,%d5
	lsl.l    #1,%d5
      lea      lf_tab,%a2              | [152]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [153]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [154]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [155]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [160]
|;;   lsl.w     #1,d0                  | [161]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [162]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [164]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [165]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [168]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [169]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [170]
	andi.l   #(0xFFFF&0xF),%d5
|;;   lsl.w     #1,d2                  | [171]
	lsl.l    #1,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [172]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [173]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [174]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [175]
      move.w   #0x8000,(-80,%a6)       | [176]
      moveq.l  #(-1),%d3               | [177]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [180 HI]
      jhi      doub_des                | [181]



|;;   lsl.w     #1,d0                  | [185]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [186]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [187]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [188]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [190]
	move.w   %d5,%d1
      jra      msk_done                | [191]



doub_des:
      move.w   %d0,%d1                 | [196]
|;;   lsr.w     #4,d1                  | [197]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [198]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [199 NE]
      jne      mlt_dest                | [200]



      move.w   (-24,%a6),%d1           | [204]
|;;   add.w     -20(a6),d1             | [205]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [206 CC]
      jcc      mlt_dest                | [207]
      moveq.l  #0,%d7                  | [208]
      jra      do_ritem                | [209]

mlt_dest:
      tst.w    (-42,%a6)               | [212 LT]
      jlt      mltleft                 | [213]
      moveq.l  #4,%d7                  | [214]
      jra      do_ritem                | [215]
mltleft:
      moveq.l  #8,%d7                  | [217]
do_ritem:
|;;   andi.w    #0x000F,d0             | [219 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [220]
|;;   subq.w    #1,d3                  | [221]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [222]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [224]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [225]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [226]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [229]
      move.w   %d3,(-28,%a6)           | [230]
      move.w   %d4,(-36,%a6)           | [231]
|;;   addq.w    #2,d3                  | [232]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [233 CS]
      jcs      msk0                    | [234]
|;;   addq.w    #1,d3                  | [235]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [237]
      move.w   %d3,(-30,%a6)           | [238]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jra      plane_loop              | [239]
| 662 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [242]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [243]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [244]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [245]
	.byte	4*5,4*5,4*6,4*6

toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [248]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [250]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [254]
|;;   lsr.w     -84(a6)                | [255 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [256]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [257 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [258]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [262]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [263]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [264]
      movea.l  %a2,%a5                 | [265]

|;;   movea.l   twoptble(pc,d0.w),a4   | [267]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [271 EQ]
      jeq      wrt_char                | [272]

      btst     #1,%d0                  | [274 EQ]
      jeq      no_lite                 | [275]



|;;   move.w    _LITEMASK,-78(a6)      | [279]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [280]
      lea      liteop,%a5              | [281]

      move.l   %a2,(-64,%a6)           | [283]
      lea      liteopwf,%a2            | [284]

      move.l   %a4,(-72,%a6)           | [286]
      lea      liteopw,%a4             | [287]

no_lite:
      btst     #0,%d0                  | [290 EQ]
      jeq      no_thick                | [291]



      clr.w    (-82,%a6)               | [295]

      move.l   %a5,(-48,%a6)           | [297]
      lea      thknop,%a5              | [298]

      move.l   %a2,(-60,%a6)           | [300]
      lea      thknopwf,%a2            | [301]

      move.l   %a4,(-68,%a6)           | [303]
      lea      thknopw,%a4             | [304]

no_thick:
      btst     #2,%d0                  | [307 EQ]
      jeq      wrt_char                | [308]



      move.w   (-100,%a6),(-80,%a6)    | [312]
      move.l   %a5,(-56,%a6)           | [313]
      lea      skewop,%a5              | [314]

      tst.w    (-124,%a6)              | [316 EQ GT]
      jgt      wrt_char                | [317]
      jeq      not_sngl                | [318]
      clr.w    (-28,%a6)               | [319]
      moveq.l  #0,%d7                  | [320]
      jra      chng_blt                | [321]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [323 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [324]

      tst.w    (-42,%a6)               | [326 LT]
      jlt      chng_left               | [327]
      moveq.l  #4,%d7                  | [328]
      jra      chng_blt                | [329]
chng_left:
      moveq.l  #8,%d7                  | [331]
chng_blt:
      move.w   %d7,(-124,%a6)          | [333]

wrt_char:
      move.w   (-124,%a6),%d7          | [336 LE]
      jle      set_chr_hgt             | [337]
      movea.l  %a2,%a5                 | [338]
      move.w   (-18,%a6),%d5           | [339]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [341]
|;;   subq.w    #1,d3                  | [342]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [344]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [345]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [347 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [348]
      movea.l  (-10,%a6),%a0           | [349]
      movea.l  (-14,%a6),%a1           | [350]
      addq.l   #2,%a1                  | [351]
      move.l   %a1,(-14,%a6)           | [352]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [353 EQ]
      jeq      plane_loop              | [354]
      jra      norm_blt                | [355]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [357]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [359]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [365]
      move.w   (-16,%a6),%d6           | [366]
      move.w   (-18,%a6),%d7           | [367]
      lea      sgl_rtn,%a3             | [368]
      jra      sgl_1st_scan            | [369]

sgl_lp:
|;;   adda.w    d6,a0                  | [372]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [373]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [375]
| 805 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [377]


      move.w   (-42,%a6),%d1           | [380 MI]
      jmi      left_rol                | [381]
      lsr.l    %d1,%d0                 | [382]
      jra      end_ro                  | [383]
left_rol:
      lsl.l    %d1,%d0                 | [385]
end_ro:
      swap     %d0                     | [387]
      move.w   %d0,%d1                 | [388]
      swap     %d0                     | [389]

      jmp      (%a5)                   | [391]

sgl_rtn:
      move.w   %d1,(%a1)               | [394]
|;;   dbra      d3,sgl_lp              | [395]
	tst.w    %d3
	jeq      _PA_394_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_394_:         
      jra      next_plane              | [396]




dbl_loop:
      movea.l  %a3,%a4                 | [402]
      move.w   (-16,%a6),%d6           | [403]
      move.w   (-18,%a6),%d7           | [404]
      lea      dbl_2wrd,%a2            | [405]
      move.l   %a2,%d5                 | [406]
      lea      dbl_1wrd,%a3            | [407]
      jra      dbl_1st_scan            | [408]

dbl_lp:
|;;   adda.w    d6,a0                  | [411]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [412]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [413]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [415]
| 852 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [417]


      move.w   (-42,%a6),%d1           | [420 MI]
      jmi      dbleft                  | [421]
      lsr.l    %d1,%d0                 | [422]
      jra      dblendr                 | [423]
dbleft:
      lsl.l    %d1,%d0                 | [425]
dblendr:
      swap     %d0                     | [427]
      move.w   %d0,%d1                 | [428]
      swap     %d0                     | [429]
      move.w   (-38,%a6),%d2           | [430]

      jmp      (%a5)                   | [432]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [435]
      move.w   %d0,%d1                 | [436]

      movea.w  (-88,%a6),%a2           | [438]
|;;   move.w    (a1,a2.w),d4           | [439]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [441]

|;;   exg       a3,d5                  | [443]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [444]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [447]
|;;   move.w    d1,(a1,a2.w)           | [448]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [450]
	tst.w    %d3
	jeq      _PA_449_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_449_:         
      jra      next_plane              | [451]





left_loop:
      movea.l  %d6,%a0                 | [458]
      movea.l  %d7,%a1                 | [459]
|;;   adda.w    -16(a6),a0             | [460]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [461]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [462]
|;;   rol.w     -78(a6)                | [463]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [465 NE]
      jne      skewopw                 | [466]
mlt_left:
      move.l   %a0,%d6                 | [468]
      move.l   %a1,%d7                 | [469]
      move.w   (-28,%a6),(-44,%a6)     | [470]







      move.l   (%a0)+,%d0              | [478]


      move.w   (%a1),%d4               | [481]

      move.w   (-42,%a6),%d1           | [483]
      lsl.l    %d1,%d0                 | [484]

      swap     %d0                     | [486]
      move.w   %d0,%d1                 | [487]
      swap     %d0                     | [488]

      move.w   (-38,%a6),%d2           | [490]

      lea      lft_lfrng,%a3           | [492]
      jmp      (%a5)                   | [493]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [496]
|;;   adda.w    -88(a6),a1             | [497]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [499]
      lea      lft_wrds,%a3            | [500]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [505]
|;;   and.w     d2,d4                  | [506]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [507]





      swap     %d0                     | [513]
      move.w   %d1,%d0                 | [514]
      swap     %d0                     | [515]

      move.w   (-42,%a6),%d1           | [517]
      lsl.l    %d1,%d0                 | [518]

      swap     %d0                     | [520]
      move.w   %d0,%d1                 | [521]
      swap     %d0                     | [522]

|;;   not.w     d2                     | [524]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [525]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [526]
	not.l    %d5
|;;   eor.w     d4,d1                  | [527]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [529]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [531 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [532]

      jmp      (%a4)                   | [534]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [537]
|;;   adda.w    -88(a6),a1             | [538]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [540]

lef_don:
      move.w   (-36,%a6),%d2           | [543]

      lea      lft_rfrng,%a3           | [545]
      jmp      (%a5)                   | [546]

lft_rfrng:
      move.w   %d1,(%a1)               | [549]

|;;   dbra      d3,left_loop           | [551]
	tst.w    %d3
	jeq      _PA_550_
	subq.l   #1,%d3
	jra      left_loop
	_PA_550_:         
      jra      next_plane              | [552]





rite_loop:
      movea.l  %d6,%a0                 | [559]
      movea.l  %d7,%a1                 | [560]
|;;   adda.w    -16(a6),a0             | [561]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [562]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [563]
|;;   rol.w     -78(a6)                | [564]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [566 NE]
      jne      skewopw                 | [567]
mlt_rite:
      move.l   %a0,%d6                 | [569]
      move.l   %a1,%d7                 | [570]
      move.w   (-28,%a6),(-44,%a6)     | [571]
      move.w   (%a0)+,%d0              | [572]



      swap     %d0                     | [576]

      move.w   (%a1),%d4               | [578]

      move.w   (-42,%a6),%d1           | [580]
      lsr.l    %d1,%d0                 | [581]
      swap     %d0                     | [582]
      move.w   %d0,%d1                 | [583]
      swap     %d0                     | [584]

      move.w   (-38,%a6),%d2           | [586]

      lea      rgt_lfrng,%a3           | [588]
      jmp      (%a5)                   | [589]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [592]
|;;   adda.w    -88(a6),a1             | [593]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [595]
      lea      rgt_wrds,%a3            | [596]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [601]
|;;   and.w     d2,d4                  | [602]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [603]
      move.w   (%a0)+,%d0              | [604]



      swap     %d0                     | [608]

      move.w   (-42,%a6),%d1           | [610]
      lsr.l    %d1,%d0                 | [611]
      swap     %d0                     | [612]
      move.w   %d0,%d1                 | [613]
      swap     %d0                     | [614]

|;;   not.w     d2                     | [616]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [617]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [618]
	not.l    %d5
|;;   eor.w     d4,d1                  | [619]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [621]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [623 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [624]

      jmp      (%a4)                   | [626]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [629]
|;;   adda.w    -88(a6),a1             | [630]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [632]

rite_don:
      move.w   (-36,%a6),%d2           | [635]

      lea      rgt_rfrng,%a3           | [637]
      jmp      (%a5)                   | [638]

rgt_rfrng:
      move.w   %d1,(%a1)               | [641]

|;;   dbra      d3,rite_loop           | [643]
	tst.w    %d3
	jeq      _PA_642_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_642_:         
      jra      next_plane              | [644]





top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [651]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [652]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [653]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [654]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [655]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [658]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [659]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [660]
	not.l    %d0
|;;   and.w     d4,d1                  | [661]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [662]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [665]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [666]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [667]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [668]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [671]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [672]
	not.l    %d0
|;;   and.w     d4,d1                  | [673]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [674]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [677]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [678]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [679]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [682]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [683]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [684]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [687]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [688]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [689]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [690]





twop0:
      moveq.l  #0,%d1                  | [697]
      jmp      (%a3)                   | [698]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [701]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [702]

twop3:
      jmp      (%a3)                   | [705]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [708]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [709]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [710]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [713]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [714]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [717]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [718]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [721]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [722]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [723]
| 1178 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [726]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [727]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [728]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [729 EQ]
      jeq      thk00                   | [730]
|;;   ori.b     #0x10,ccr              | [731 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [732 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [733]
      move.w   #0x8000,%d6             | [734]
thk01:
      move.w   %d6,(-34,%a6)           | [736]
thk00:
      tst.w    (-28,%a6)               | [738 MI]
      jmi      thk0                    | [739]
      cmp.w    (-36,%a6),%d6           | [740 CC]
      jcc      thk0                    | [741]
|;;   and.w     d6,d0                  | [742]
	and.l    %d6,%d0
      jra      thk1                    | [743]
thk0:
      clr.w    %d0                     | [745]
|;;   and.w     d6,d1                  | [746]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [748]
      move.w   %d2,%d5                 | [749]
      swap     %d5                     | [750]
      move.w   (-36,%a6),%d5           | [751]
      lsl.l    %d6,%d5                 | [752]

      swap     %d5                     | [754]
      move.w   %d5,%d7                 | [755]
      swap     %d5                     | [756]

|;;   and.w     d7,d1                  | [758]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [759]
	and.l    %d5,%d0
|;;   swap      d1                     | [760]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [761]
|;;   subq.w    #1,d6                  | [762]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [764]
      lsr.l    #1,%d0                  | [765]
      or.l     %d0,%d1                 | [766]
|;;   dbra      d6,thkoplp             | [767]
	tst.w    %d6
	jeq      _PA_766_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_766_:         

      move.w   %d1,%d0                 | [769]
      swap     %d1                     | [770]
|;;   movem.l   (sp)+,d5-d7            | [771]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [772]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [773]
| 1241 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [776]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [777 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [778]
|;;   and.w     -34(a6),d1             | [779]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [781]
      swap     %d1                     | [782]
      clr.w    %d1                     | [783]
      move.l   %d1,%d5                 | [784]
|;;   subq.w    #1,d6                  | [785]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [787]
      or.l     %d5,%d1                 | [788]
|;;   dbra      d6,thkopwlp            | [789]
	tst.w    %d6
	jeq      _PA_788_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_788_:         

      move.w   %d1,%d5                 | [791]
      swap     %d1                     | [792]
|;;   or.w      -82(a6),d1             | [793]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [794]
|;;   movem.l   (sp)+,d5-d6            | [795]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [796]
      jmp      (%a2)                   | [797]
| 1279 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [800]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [801]
|;;   subq.w    #1,-30(a6)             | [802 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [803]
      jeq      thk4                    | [804]

|;;   subq.w    #1,-30(a6)             | [806 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [807]
|;;   and.w     d6,d1                  | [808]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [810]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [811]

thk3:
      clr.w    %d1                     | [814]
      jra      thk5                    | [815]

thk4:
|;;   and.w     d6,d1                  | [818]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [821 MI]
      jmi      thk6                    | [822]
      move.w   (-28,%a6),%d5           | [823]
|;;   addq.w    #2,d5                  | [824]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [825]
|;;   ori.b     #0x10,ccr              | [826 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [827 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [828]
      move.w   #0x8000,%d7             | [829]
|;;   addq.w    #1,d5                  | [830]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [832 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [833]
|;;   subq.w    #1,d5                  | [834]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [836 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [837 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [838]
      move.w   #0x8000,%d6             | [839]
thk31:
      cmp.w    %d6,%d7                 | [841 CS]
      jcs      thk35                   | [842]
|;;   addq.w    #1,d5                  | [843]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [845]
      move.w   %d5,(-32,%a6)           | [846]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [848]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [850]
      swap     %d1                     | [851]
      clr.w    %d1                     | [852]
      move.l   %d1,%d5                 | [853]
|;;   subq.w    #1,d6                  | [854]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [856]
      or.l     %d5,%d1                 | [857]
|;;   dbra      d6,thkopwl             | [858]
	tst.w    %d6
	jeq      _PA_857_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_857_:         

      move.w   %d1,%d5                 | [860]
      swap     %d1                     | [861]
|;;   or.w      -82(a6),d1             | [862]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [863]
|;;   and.w     d2,d1                  | [864]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [865]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [866]
      jmp      (%a2)                   | [867]
| 1360 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [870]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [871]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [872]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [873]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [874]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [877]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [878]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [879]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [882]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [883]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [884]
| 1392 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [887 CC]
	clr.l    %d5
	move.w   (-80,%a6),%d3
	move.w   %d3,%d5
	lsl.l    #1,%d3
	lsr.l    #8,%d5
	lsr.l    #7,%d5
	or.l     %d5,%d3
	lsr.l    #1,%d5
	move     %ccr,%d5
	move.w   %d3,(-80,%a6)
	move     %d5,%ccr
      jcc      no_shift                | [888]

      swap     %d1                     | [890]
      move.w   %d0,%d1                 | [891]
      lsr.l    #1,%d1                  | [892]

      swap     %d2                     | [894]
      move.w   (-36,%a6),%d2           | [895]
      lsr.l    #1,%d2                  | [896]
      move.w   %d2,(-36,%a6)           | [897]
      swap     %d2                     | [898]
      move.w   %d2,(-38,%a6)           | [899 EQ]
      jeq      nxt_word                | [900]
      move.w   (-42,%a6),%d0           | [901 MI]
      jmi      dec_rol                 | [902]
ror_add:
|;;   addq.w    #1,d0                  | [904]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [906]
      move.w   %d1,%d0                 | [907]
      swap     %d1                     | [908]
no_shift:
      movea.l  (-56,%a6),%a2           | [910]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [911]

dec_rol:
      tst.b    %d0                     | [914 EQ]
      jeq      begn_ror                | [915]
|;;   subq.w    #1,d0                  | [916]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [917]
begn_ror:
      clr.w    %d0                     | [919]
      jra      ror_add                 | [920]



nxt_word:
      move.w   %d2,(-36,%a6)           | [925]
      swap     %d2                     | [926]
      move.w   %d2,(-38,%a6)           | [927]
|;;   adda.w    -88(a6),a1             | [928]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [929]

      moveq.l  #15,%d0                 | [931]
|;;   sub.w     -42(a6),d0             | [932]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [933]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [934]
      jra      no_shift                | [935]






skewopw:
|;;   rol.w     -80(a6)                | [943 CC]
	clr.l    %d7
	move.w   (-80,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	lsr.l    #1,%d7
	move     %ccr,%d7
	move.w   %d6,(-80,%a6)
	move     %d7,%ccr
      jcc      do_shift                | [944]

      tst.w    (-42,%a6)               | [946 MI]
      jmi      mlt_left                | [947]
      jra      mlt_rite                | [948]
do_shift:
|;;   ori       #0x10,ccr              | [950 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [951]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [953]
      cmp.w    #0xFFFF,%d0             | [954 EQ]
      jeq      inc_rite                | [955]

|;;   ori       #0x10,ccr              | [957 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [958]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [959]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [962]
      tst.b    %d0                     | [963 NE]
      jne      no_rota                 | [964]
      move.w   #0x8000,(-40,%a6)       | [965]
no_rota:
      move.w   (-38,%a6),%d1           | [967]
|;;   lsr.w     #1,d1                  | [968 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [969]

      move.w   %d1,(-38,%a6)           | [971]

      tst.w    %d0                     | [973 MI]
      jmi      wdec_rol                | [974]
|;;   addq.w    #1,-42(a6)             | [975]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [976]
wdec_rol:
      tst.b    %d0                     | [978 EQ]
      jeq      set_msk                 | [979]
|;;   subq.w    #1,-42(a6)             | [980]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [981]
set_msk:
      move.w   #1,(-42,%a6)            | [983]
      jra      mlt_rite                | [984]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [987]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [988]
      jra      do_left                 | [989]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [992]

|;;   subq.w    #1,-28(a6)             | [994]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [995]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [997]
|;;   sub.w     d0,d2                  | [998]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [999]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1000]

      jra      mlt_left                | [1002]




rotation:
|***	SAVE	d7	***
	lea      -16(%a7),%a7
	movem.l  %d7,12(%a7)
      move.w   _SOURCEX,%d1            | [1008]
      move.w   %d1,%d2                 | [1009]
|;;   andi.w    #0x0F,d2               | [1010]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1011]
|;;   lsr.w     #4,d1                  | [1012]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
      moveq.l  #1,%d5                  | [1013]
|;;   lsl.w     d5,d1                  | [1014]
	lsl.l    %d5,%d1

      movea.l  (-10,%a6),%a0           | [1016]
|;;   adda.w    d1,a0                  | [1017]
	mvs.w    %d1,%d0
	adda.l   %d0,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1019]
      move.w   _SOURCEY,%d0            | [1020]
      move.w   (-106,%a6),%d1          | [1021]
      move.w   %d1,(-22,%a6)           | [1022]
      move.w   (-16,%a6),%d2           | [1023]

      tst.w    (-126,%a6)              | [1025 EQ LT]
      jeq      upsd_dwn                | [1026]
rot90:
      jlt      top_src                 | [1028]
|;;   neg.w     -16(a6)                | [1029]
	move.w   (-16,%a6),%d7
	neg.l    %d7
	move.w   %d7,(-16,%a6)
|;;   subq.w    #1,d1                  | [1030]
	subq.l   #1,%d1
|;;   add.w     d1,d0                  | [1031]
	add.l    %d1,%d0
      mulu.w   %d2,%d0                 | [1032]
|;;   adda.w    d0,a0                  | [1033]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1035]
|;;   lsr.w     #4,d0                  | [1036]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1037]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1038]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1039]
      movea.l  _SCRTCHP,%a1            | [1040]
|;;   adda.w    -4(a6),a1              | [1041]
	mvs.w    (-4,%a6),%d7
	adda.l   %d7,%a1

      tst.w    (-126,%a6)              | [1043 GT]
      jgt      top_dwn                 | [1044]
|;;   neg.w     -18(a6)                | [1045]
	move.w   (-18,%a6),%d7
	neg.l    %d7
	move.w   %d7,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1046]
|;;   subq.w    #1,d1                  | [1047]
	subq.l   #1,%d1
      mulu.w   %d1,%d0                 | [1048]
|;;   adda.w    d0,a1                  | [1049]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1051]
      move.w   #0x8000,%d3             | [1052]
      move.w   %d3,%d4                 | [1053]
      movea.w  %d3,%a4                 | [1054]
|;;   lsr.w     d2,d3                  | [1055]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3
      moveq.l  #0,%d0                  | [1056]
      move.w   (-20,%a6),%d2           | [1057]
      move.w   (-16,%a6),%d6           | [1058]
      movea.l  %a0,%a2                 | [1059]
      movea.l  %a1,%a3                 | [1060]
|***	RESTORE	d7	***
	movem.l  12(%a7),%d7
	lea      16(%a7),%a7
      jra      rot_nsrc                | [1061]
rot_ylp:
|***	SAVE	d2	***
	lea      -8(%a7),%a7
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1063]
      jra      rot_srt                 | [1064]
rot_xlp:
      move.w   (%a0),%d7               | [1066]
|;;   and.w     d3,d7                  | [1067 EQ]
	and.l    %d3,%d7
	tst.w    %d7
      jeq      rot_nor                 | [1068]
|;;   or.w      d4,d0                  | [1069]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1071 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jeq      _PA_1070_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1070_1:         
|***	RESTORE	AFTER d6	***
	movem.l  4(%a7),%d6
      jcc      rot_isrc                | [1072]
rot_ndst:
      move.w   %d0,(%a1)+              | [1074]
      moveq.l  #0,%d0                  | [1075]
rot_isrc:
|;;   adda.w    d6,a0                  | [1077]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,4(%a7)
|;;   dbra      d1,rot_xlp             | [1079]
	tst.w    %d1
	jeq      _PA_1078_
	subq.l   #1,%d1
	jra      rot_xlp
	_PA_1078_:         

      move.w   %d0,(%a1)               | [1081]
      moveq.l  #0,%d0                  | [1082]
|;;   adda.w    -18(a6),a3             | [1083]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1084]
      move.w   %a4,%d4                 | [1085]
|;;   ror.w     d5,d3                  | [1086 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jeq      _PA_1085_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1085_1:         
      jcc      rnew_src                | [1087]
      addq.l   #2,%a2                  | [1088]
rnew_src:
      movea.l  %a2,%a0                 | [1090]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2/%d6
	lea      8(%a7),%a7
rot_nsrc:
|;;   dbra      d2,rot_ylp             | [1092]
	tst.w    %d2
	jeq      _PA_1091_
	subq.l   #1,%d2
	jra      rot_ylp
	_PA_1091_:         

rot_done:
      move.w   (-102,%a6),%d0          | [1095]
      move.w   (-106,%a6),%d1          | [1096]
      move.w   %d1,(-20,%a6)           | [1097]
      move.w   %d1,(-102,%a6)          | [1098]
      move.w   %d0,(-22,%a6)           | [1099]
      move.w   %d0,(-106,%a6)          | [1100]

      move.w   (-92,%a6),%d0           | [1102]
      move.w   (-90,%a6),(-92,%a6)     | [1103]
      move.w   %d0,(-90,%a6)           | [1104]
      move.w   #1,(-94,%a6)            | [1105]

      move.w   (-18,%a6),%d0           | [1107]
      tst.w    (-126,%a6)              | [1108 GT]
      jgt      rot_nneg                | [1109]
|;;   neg.w     d0                     | [1110]
	neg.l    %d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1112]
      move.w   (-4,%a6),%d0            | [1113]
repexit:
      clr.w    _SOURCEX                | [1115]
repexit3:
      movea.l  _SCRTCHP,%a2            | [1117]
|;;   adda.w    d0,a2                  | [1118]
	ext.l    %d0
	adda.l   %d0,%a2
      move.l   %a2,(-10,%a6)           | [1119]
      clr.w    _SOURCEY                | [1120]
      rts                              | [1121]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1124]
|;;   add.w     -24(a6),d0             | [1125]
	move.w   (-24,%a6),%d7
	add.l    %d7,%d0
|;;   subq.w    #1,d0                  | [1126]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1127]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1128]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1129]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1131]
      move.w   %d0,%d2                 | [1132]
|;;   lsr.w     d5,d2                  | [1133]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1134]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1135]
|;;   add.w     -4(a6),d0              | [1136 X]
	mvz.w    (-4,%a6),%d7
	ori.l    #0xffff0000,%d0
	add.l    %d7,%d0
      movea.l  _SCRTCHP,%a1            | [1137]
|;;   adda.w    d0,a1                  | [1138]
	ext.l    %d0
	adda.l   %d0,%a1
      jra      strtflip                | [1139]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1141]
      move.w   %d2,%d3                 | [1142]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1144]
      moveq.l  #0,%d6                  | [1145]
      moveq.l  #15,%d4                 | [1146]
flip_lp:
|;;   lsr.w     d5,d0                  | [1148 X]
	and.l    #0xffff,%d0
	lsr.l    %d5,%d0
|;;   roxl.w    d5,d6                  | [1149 X]
	move.b   %d5,%d1
	moveq    #16,%d3
	and.l    #0x1f,%d1
	jeq      _PA_1148_
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d6
	_PA_1148_:         
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d6,%d2
	lsl.l    %d3,%d2
|;;   dbra      d4,flip_lp             | [1150]
	move     %CCR,%d1
	tst.w    %d4
	jeq      _PA_1149_
	subq.l   #1,%d4
	jra      flip_lp
	_PA_1149_:         
	move     %d1,%CCR

      move.w   %d6,-(%a1)              | [1152]
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1153]
	move     %CCR,%d1
	tst.w    %d3
	jeq      _PA_1152_
	subq.l   #1,%d3
	move     %d1,%CCR
	jra      line_lp
	_PA_1152_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1155]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1157]
	move     %CCR,%d7
	tst.w    %d1
	jeq      _PA_1156_
	subq.l   #1,%d1
	move     %d7,%CCR
	jra      upsd_lp
	_PA_1156_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1159]
      movea.l  _SCRTCHP,%a1            | [1160]
|;;   adda.w    -4(a6),a1              | [1161]
	mvs.w    (-4,%a6),%d7
	adda.l   %d7,%a1
      move.l   %a1,(-10,%a6)           | [1162]
      move.w   _SOURCEX,%d0            | [1163]
|;;   add.w     -102(a6),d0            | [1164]
	move.w   (-102,%a6),%d7
	add.l    %d7,%d0
|;;   neg.w     d0                     | [1165]
	neg.l    %d0
|;;   andi.w    #0xF,d0                | [1166]
	andi.l   #(0xFFFF&0xF),%d0
      move.w   %d0,_SOURCEX            | [1167]
      clr.w    _SOURCEY                | [1168]
|***	RESTORE	d7	***
	movem.l  12(%a7),%d7
	lea      16(%a7),%a7
      rts                              | [1169]






replicat:
|***	SAVE	d7	***
	lea      -12(%a7),%a7
	movem.l  %d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1177]
      move.w   %d0,%d4                 | [1178]
|;;   andi.w    #0x0F,d0               | [1179]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1180]
|;;   lsr.w     #4,d4                  | [1181]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   lsl.w     #1,d4                  | [1182]
	lsl.l    #1,%d4

      move.w   _SOURCEY,%d0            | [1184]
      mulu.w   (-16,%a6),%d0           | [1185]
|;;   add.w     d4,d0                  | [1186]
	add.l    %d4,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1187]
|;;   adda.w    d0,a0                  | [1188]
	ext.l    %d0
	adda.l   %d0,%a0

      move.w   (-24,%a6),%d2           | [1190]
      move.w   #0x8000,%d3             | [1191]
      move.w   %d3,%d4                 | [1192]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1193]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1195]
      move.w   (-102,%a6),%d1          | [1196]

      move.w   %d2,(-22,%a6)           | [1198]
      move.w   %d1,(-20,%a6)           | [1199]
      movea.l  _SCRTCHP,%a1            | [1200]
|;;   adda.w    _SCRPT2,a1             | [1201]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1203 EQ]
      jeq      noline                  | [1204]



|;;   addq.w    #1,-106(a6)            | [1208]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1209]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1211]
	lsl.l    #1,%d1
|;;   addq.w    #3,d1                  | [1212]
	addq.l   #3,%d1
|;;   lsr.w     #4,d1                  | [1213]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   lsl.w     #1,d1                  | [1214]
	lsl.l    #1,%d1
|;;   addq.w    #2,d1                  | [1215]
	addq.l   #2,%d1
      move.w   %d1,%d5                 | [1216]
      moveq.l  #0,%d6                  | [1217]
oklear:
      move.w   %d6,(%a1)+              | [1219]
|;;   dbra      d5,oklear              | [1220]
	tst.w    %d5
	jeq      _PA_1219_
	subq.l   #1,%d5
	jra      oklear
	_PA_1219_:         
      lea      (-2,%a1),%a2            | [1221]
      move.w   %d2,%d5                 | [1222]
|;;   lsl.w     #1,d5                  | [1223]
	lsl.l    #1,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1225]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1226]
|;;   dbra      d5,oklear1             | [1227]
	tst.w    %d5
	jeq      _PA_1226_
	subq.l   #1,%d5
	jra      oklear1
	_PA_1226_:         

|;;   move.w    d1,d5                  | [1229]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1230]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   subq.w    #1,d5                  | [1231 X]
	subq.l   #1,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1233]
|;;   dbra      d5,oklear3             | [1234]
	move     %CCR,%d0
	tst.w    %d5
	jeq      _PA_1233_
	subq.l   #1,%d5
	move     %d0,%CCR
	jra      oklear3
	_PA_1233_:         
	move     %d0,%CCR
      jra      noline1                 | [1235]

noline:
|;;   lsr.w     #3,d1                  | [1238]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   lsl.w     #1,d1                  | [1239]
	lsl.l    #1,%d1
|;;   addq.w    #2,d1                  | [1240 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1242]
|***	RESTORE	AFTER d7	***
	movem.l  8(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1246]
|;;   roxr.l    #1,d7                  | [1247 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1248]
|;;   roxr.l    #1,d7                  | [1249]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1251]
      move.w   _DDA_INC,%d2            | [1252]
      move.w   (-22,%a6),%d5           | [1253]
|;;   subq.w    #1,d5                  | [1254]
	subq.l   #1,%d5
      move.w   #32767,%d6              | [1255]
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      12(%a7),%a7
      tst.l    %d7                     | [1256 MI]
      jmi      rep_ylop                | [1257]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1259 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1260]
      jbsr     yloop                   | [1261]
y_no_drw:
|;;   adda.w    d7,a0                  | [1263]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1264]
	tst.w    %d5
	jeq      _PA_1263_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1263_:         
      jra      y_rep_don               | [1265]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1267 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1268]
      jbsr     yloop                   | [1269]
y_no_rep:
      jbsr     yloop                   | [1271]
|;;   adda.w    d7,a0                  | [1272]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1273]
	tst.w    %d5
	jeq      _PA_1272_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1272_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1275]
|;;   subq.w    #1,d2                  | [1276]
	subq.l   #1,%d2
      move.w   _XACC_DDA,%d1           | [1277]
      move.w   _DDA_INC,%d0            | [1278]
      moveq.l  #0,%d3                  | [1279]
repwidcl:
|;;   add.w     d0,d1                  | [1281 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1282]
|;;   addq.w    #1,d3                  | [1283]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1285 PL]
      jpl      nrpndoub                | [1286]
|;;   addq.w    #1,d3                  | [1287]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1289]
	tst.w    %d2
	jeq      _PA_1288_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1288_:         
      move.w   %d1,_XACC_DDA           | [1290]
      move.w   %d3,(-102,%a6)          | [1291]
      move.w   (-92,%a6),(-106,%a6)    | [1292]


      move.w   (-18,%a6),%d1           | [1295]
      move.w   %d1,(-16,%a6)           | [1296]







repexit1:
      btst     #4,(-96+1,%a6)          | [1305 EQ]
      jeq      repexit2                | [1306]
      movea.l  _SCRTCHP,%a0            | [1307]
|;;   adda.w    _SCRPT2,a0             | [1308]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1309]
|;;   lea       2(a0,d7.w),a1          | [1310]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
      jbsr     outlin1                 | [1311]
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   _SCRPT2,%d0             | [1312]
|;;   add.w     -18(a6),d0             | [1313]
	move.w   (-18,%a6),%d1
	add.l    %d1,%d0
|;;   move.w    #15,_SOURCEX           | [1314]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      jra      repexit3                | [1315]
repexit2:
      move.w   _SCRPT2,%d0             | [1317]
      jra      repexit                 | [1318]
| 1855 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1321]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1322]
      movea.l  %a1,%a4                 | [1323]
      movea.l  %d7,%a5                 | [1324]
      move.w   %d5,(-44,%a6)           | [1325]
      moveq.l  #0,%d0                  | [1326]
      move.w   (-20,%a6),%d5           | [1327]
|;;   subq.w    #1,d5                  | [1328]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1330]
      jra      nextsrc                 | [1331]
innerlp:
|;;   ror.w     #1,d3                  | [1333 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1334]
nextsrc:
      movea.w  (%a0)+,%a2              | [1336]
reploop:
      move.w   %a2,%d6                 | [1338]
|;;   and.w     d3,d6                  | [1339 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1340]


repnor:
      tst.l    %d7                     | [1344 MI]
      jmi      repnorup                | [1345]
|;;   add.w     d2,d7                  | [1346 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1347]
      jra      ordone                  | [1348]
repnorup:
|;;   add.w     d2,d7                  | [1350 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1351]


|;;   ror.w     #1,d4                  | [1354 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1357]
      move.w   %d0,(%a1)+              | [1358]
      moveq.l  #0,%d0                  | [1359]


      jra      ordone                  | [1362]
nrepnor:


|;;   add.w     d2,d7                  | [1366 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1367]


|;;   or.w      d4,d0                  | [1370]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1371 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1373]
      move.w   %d0,(%a1)+              | [1374]
      moveq.l  #0,%d0                  | [1375]
o_no_rep:
      tst.l    %d7                     | [1377 PL]
      jpl      incsrc                  | [1378]


|;;   or.w      d4,d0                  | [1381]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1383 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1384]
nextdst:
      move.w   %d0,(%a1)+              | [1386]
      moveq.l  #0,%d0                  | [1387]
incsrc:
|;;   dbra      d5,innerlp             | [1389]
	tst.w    %d5
	jeq      _PA_1388_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1388_:         
repdone:
      move.w   %d0,(%a1)               | [1391]
|;;   movem.l   (sp)+,d3-d4/d6         | [1392]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1393]
      movea.l  %a4,%a1                 | [1394]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1395]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1396]
      move.w   (-44,%a6),%d5           | [1397]
      rts                              | [1398]
| 1943 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1401]
	ext.l    %d7
	lea      (0,%a0,%d7.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1403]
	ext.l    %d7
	lea      (0,%a1,%d7.l*1),%a2
|;;   lsr.w     #1,d7                  | [1404]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
|;;   subq.w    #1,d7                  | [1405]
	subq.l   #1,%d7
      move.w   %d7,(-44,%a6)           | [1406]
      move.w   (-106,%a6),%d6          | [1407]

      movea.l  %a0,%a5                 | [1409]
|;;   subq.w    #1,d6                  | [1410]
	subq.l   #1,%d6
out_edge:
      movea.w  %d6,%a4                 | [1412]
      movea.l  %a2,%a3                 | [1413]
      moveq.l  #0,%d5                  | [1414]
      moveq.l  #0,%d6                  | [1415]
      move.l   (%a2),%d1               | [1416]
      lsr.l    #1,%d1                  | [1417]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1422]
      move.b   %d5,%d0                 | [1423]
|;;   ror.l     #1,d0                  | [1424]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1426]
      move.b   %d6,%d2                 | [1427]
      move.l   %d2,%d3                 | [1428]
|;;   ror.l     #1,d3                  | [1429]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1430]
|;;   ror.l     #1,d4                  | [1431]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1433]
      move.l   %d0,%d6                 | [1434]
      eor.l    %d2,%d0                 | [1435]
      eor.l    %d3,%d5                 | [1436]
      eor.l    %d4,%d6                 | [1437]
|;;   rol.l     #1,d5                  | [1438]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1439]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1440]
      or.l     %d6,%d0                 | [1441]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1443]
      move.l   %d1,%d6                 | [1444]
      eor.l    %d2,%d1                 | [1445]
      eor.l    %d3,%d5                 | [1446]
      eor.l    %d4,%d6                 | [1447]
|;;   rol.l     #1,d5                  | [1448]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1449]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1450]
      or.l     %d5,%d0                 | [1451]
      or.l     %d6,%d0                 | [1452]

      eor.l    %d3,%d2                 | [1454]
      eor.l    %d3,%d4                 | [1455]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1456]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1457]
      or.l     %d4,%d0                 | [1458]
      swap     %d0                     | [1459]

      move.w   (%a1),%d6               | [1461]
      move.w   %d6,%d5                 | [1462]
|;;   eor.w     d0,d5                  | [1463]
	eor.l    %d0,%d5
|;;   and.w     d0,d5                  | [1464]
	and.l    %d0,%d5

      addq.l   #2,%a2                  | [1466]
      move.l   (%a2),%d1               | [1467]
      move.b   (-1,%a2),%d1            | [1468]
|;;   ror.l     #1,d1                  | [1469]
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1471]
      move.w   (%a0),%d5               | [1472]
      move.w   %d6,(%a0)+              | [1473]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1475]
	tst.w    %d7
	jeq      _PA_1474_
	subq.l   #1,%d7
	jra      out_loop
	_PA_1474_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1477]
      movea.l  %a5,%a0                 | [1478]
      move.w   %a4,%d6                 | [1479]
      movea.l  %a3,%a2                 | [1480]
      movea.l  %a2,%a1                 | [1481]
|;;   adda.w    -16(a6),a2             | [1482]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1483 NE]
      jne      srt_lin                 | [1484]
      movea.l  %a1,%a2                 | [1485]
srt_lin:
|;;   dbra      d6,out_edge            | [1487]
	tst.w    %d6
	jeq      _PA_1486_
	subq.l   #1,%d6
	jra      out_edge
	_PA_1486_:         
      rts                              | [1488]

| End section on aligned boundary
	.p2align 2,0
