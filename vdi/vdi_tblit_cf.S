
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
| Translated on Fri Sep 15 15:36:14 2017 by version 2.6.0 of PortAsm translator
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
| 114 "vdi/vdi_tblit.S"
|;;           .globl _normal_blit      | [14]
	.globl	_normal_blit
|;;           .globl _outline          | [15]
	.globl	_outline
|;;           .globl _rotate           | [16]
	.globl	_rotate
|;;           .globl _scale            | [17]
	.globl	_scale
| 128 "vdi/vdi_tblit.S"
|;;           .extern _SOURCEX,_SOURCEY | [19]
	.extern	_SOURCEX
	.extern	_SOURCEY
|;;           .extern _LITEMASK        | [20]
	.extern	_LITEMASK
|;;           .extern _SCALDIR         | [21]
	.extern	_SCALDIR
|;;           .extern _XDDA            | [22]
	.extern	_XDDA
|;;           .extern _DDAINC          | [23]
	.extern	_DDAINC
|;;           .extern _SCRPT2          | [24]
	.extern	_SCRPT2
|;;           .extern _SCRTCHP         | [25]
	.extern	_SCRTCHP
| 146 "vdi/vdi_tblit.S"
|;;           .globl _deftxbuf         | [27]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [28]
	.globl	_scrtsiz
| 282 "vdi/vdi_tblit.S"
|;;           .bss                     | [30]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [32]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [34]
	.p2align 2,0
	.text

|;;           .even                    | [36]
	.p2align 1,0
_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [38]
	.short	(2+6+8+15)/16*2*16*2
| 397 "vdi/vdi_tblit.S"
|;;           .text                    | [40]






	.p2align 1,0
_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [48]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [49]
      movea.l  (44+8,%a7),%a0          | [50]
      movea.l  (44+12,%a7),%a1         | [51]
      jbsr     norm_blt                | [52]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [53]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [54]


_outline:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [58]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [59]
      movea.l  (44+8,%a7),%a0          | [60]
      move.w   (44+12,%a7),%d7         | [61]
      jbsr     outlin                  | [62]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [63]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [64]


_rotate:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [68]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [69]
      jbsr     rotation                | [70]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [71]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [72]


_scale:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [76]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [77]
      jbsr     replicat                | [78]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [79]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [80]


|;;           .text                    | [83]
| 449 "vdi/vdi_tblit.S"
|;;                   .even            | [85]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [86]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [87]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [88]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [89]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [90]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [91]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [92]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [93]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [94]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [95]
	.short	0x007F
|;;                   .dc.w 0x003F     | [96]
	.short	0x003F
|;;                   .dc.w 0x001F     | [97]
	.short	0x001F
|;;                   .dc.w 0x000F     | [98]
	.short	0x000F
|;;                   .dc.w 0x0007     | [99]
	.short	0x0007
|;;                   .dc.w 0x0003     | [100]
	.short	0x0003
|;;                   .dc.w 0x0001     | [101]
	.short	0x0001
|;;                   .dc.w 0x0000     | [102]
	.short	0x0000

|;;           .text                    | [104]
| 481 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [107]
|;;   sub.w     -24(a6),d1             | [108]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [109 PL]
      jpl      do_rot                  | [110]



|;;   neg.w     d1                     | [114]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [115]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [116]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [118]

|;;   add.w     d0,d0                  | [120]
	move.w   %d0,%d5
	add.l    %d0,%d5
      lea      lf_tab,%a2              | [121]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [122]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [123]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [124]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [129]
|;;   add.w     d0,d0                  | [130]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [131]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [133]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [134]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [137]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [138]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [139]
	andi.l   #(0xFFFF&0xF),%d5
|;;   add.w     d2,d2                  | [140]
	add.l    %d5,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [141]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [142]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [143]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [144]
      move.w   #0x8000,(-80,%a6)       | [145]
      moveq.l  #(-1),%d3               | [146]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [149 HI]
      jhi      doub_des                | [150]



|;;   add.w     d0,d0                  | [154]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [155]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [156]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [157]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [159]
	move.w   %d5,%d1
      jra      msk_done                | [160]



doub_des:
      move.w   %d0,%d1                 | [165]
|;;   lsr.w     #4,d1                  | [166]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [167]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [168 NE]
      jne      mlt_dest                | [169]



      move.w   (-24,%a6),%d1           | [173]
|;;   add.w     -20(a6),d1             | [174]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [175 CC]
      jcc      mlt_dest                | [176]
      moveq.l  #0,%d7                  | [177]
      jra      do_ritem                | [178]

mlt_dest:
      tst.w    (-42,%a6)               | [181 LT]
      jlt      mltleft                 | [182]
      moveq.l  #4,%d7                  | [183]
      jra      do_ritem                | [184]
mltleft:
      moveq.l  #8,%d7                  | [186]
do_ritem:
|;;   andi.w    #0x000F,d0             | [188 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [189]
|;;   subq.w    #1,d3                  | [190]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [191]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [193]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [194]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [195]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [198]
      move.w   %d3,(-28,%a6)           | [199]
      move.w   %d4,(-36,%a6)           | [200]
|;;   addq.w    #2,d3                  | [201]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [202 CS]
      jcs      msk0                    | [203]
|;;   addq.w    #1,d3                  | [204]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [206]
      move.w   %d3,(-30,%a6)           | [207]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jra      plane_loop              | [208]
| 600 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [211]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [212]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [213]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [214]
	.byte	4*5,4*5,4*6,4*6

toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [217]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [219]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [223]
|;;   lsr.w     -84(a6)                | [224 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [225]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [226 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [227]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [231]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [232]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [233]
      movea.l  %a2,%a5                 | [234]

|;;   movea.l   twoptble(pc,d0.w),a4   | [236]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [240 EQ]
      jeq      wrt_char                | [241]

      btst     #1,%d0                  | [243 EQ]
      jeq      no_lite                 | [244]



|;;   move.w    _LITEMASK,-78(a6)      | [248]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [249]
      lea      liteop,%a5              | [250]

      move.l   %a2,(-64,%a6)           | [252]
      lea      liteopwf,%a2            | [253]

      move.l   %a4,(-72,%a6)           | [255]
      lea      liteopw,%a4             | [256]

no_lite:
      btst     #0,%d0                  | [259 EQ]
      jeq      no_thick                | [260]



      clr.w    (-82,%a6)               | [264]

      move.l   %a5,(-48,%a6)           | [266]
      lea      thknop,%a5              | [267]

      move.l   %a2,(-60,%a6)           | [269]
      lea      thknopwf,%a2            | [270]

      move.l   %a4,(-68,%a6)           | [272]
      lea      thknopw,%a4             | [273]

no_thick:
      btst     #2,%d0                  | [276 EQ]
      jeq      wrt_char                | [277]



      move.w   (-100,%a6),(-80,%a6)    | [281]
      move.l   %a5,(-56,%a6)           | [282]
      lea      skewop,%a5              | [283]

      tst.w    (-124,%a6)              | [285 EQ GT]
      jgt      wrt_char                | [286]
      jeq      not_sngl                | [287]
      clr.w    (-28,%a6)               | [288]
      moveq.l  #0,%d7                  | [289]
      jra      chng_blt                | [290]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [292 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [293]

      tst.w    (-42,%a6)               | [295 LT]
      jlt      chng_left               | [296]
      moveq.l  #4,%d7                  | [297]
      jra      chng_blt                | [298]
chng_left:
      moveq.l  #8,%d7                  | [300]
chng_blt:
      move.w   %d7,(-124,%a6)          | [302]

wrt_char:
      move.w   (-124,%a6),%d7          | [305 LE]
      jle      set_chr_hgt             | [306]
      movea.l  %a2,%a5                 | [307]
      move.w   (-18,%a6),%d5           | [308]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [310]
|;;   subq.w    #1,d3                  | [311]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [313]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [314]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [316 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [317]
      movea.l  (-10,%a6),%a0           | [318]
      movea.l  (-14,%a6),%a1           | [319]
      addq.l   #2,%a1                  | [320]
      move.l   %a1,(-14,%a6)           | [321]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [322 EQ]
      jeq      plane_loop              | [323]
      jra      norm_blt                | [324]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [326]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [328]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [334]
      move.w   (-16,%a6),%d6           | [335]
      move.w   (-18,%a6),%d7           | [336]
      lea      sgl_rtn,%a3             | [337]
      jra      sgl_1st_scan            | [338]

sgl_lp:
|;;   adda.w    d6,a0                  | [341]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [342]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [344]
| 743 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [346]


      move.w   (-42,%a6),%d1           | [349 MI]
      jmi      left_rol                | [350]
      lsr.l    %d1,%d0                 | [351]
      jra      end_ro                  | [352]
left_rol:
      lsl.l    %d1,%d0                 | [354]
end_ro:
      swap     %d0                     | [356]
      move.w   %d0,%d1                 | [357]
      swap     %d0                     | [358]

      jmp      (%a5)                   | [360]

sgl_rtn:
      move.w   %d1,(%a1)               | [363]
|;;   dbra      d3,sgl_lp              | [364]
	tst.w    %d3
	jeq      _PA_363_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_363_:         
      jra      next_plane              | [365]




dbl_loop:
      movea.l  %a3,%a4                 | [371]
      move.w   (-16,%a6),%d6           | [372]
      move.w   (-18,%a6),%d7           | [373]
      lea      dbl_2wrd,%a2            | [374]
      move.l   %a2,%d5                 | [375]
      lea      dbl_1wrd,%a3            | [376]
      jra      dbl_1st_scan            | [377]

dbl_lp:
|;;   adda.w    d6,a0                  | [380]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [381]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [382]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [384]
| 790 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [386]


      move.w   (-42,%a6),%d1           | [389 MI]
      jmi      dbleft                  | [390]
      lsr.l    %d1,%d0                 | [391]
      jra      dblendr                 | [392]
dbleft:
      lsl.l    %d1,%d0                 | [394]
dblendr:
      swap     %d0                     | [396]
      move.w   %d0,%d1                 | [397]
      swap     %d0                     | [398]
      move.w   (-38,%a6),%d2           | [399]

      jmp      (%a5)                   | [401]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [404]
      move.w   %d0,%d1                 | [405]

      movea.w  (-88,%a6),%a2           | [407]
|;;   move.w    (a1,a2.w),d4           | [408]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [410]

|;;   exg       a3,d5                  | [412]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [413]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [416]
|;;   move.w    d1,(a1,a2.w)           | [417]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [419]
	tst.w    %d3
	jeq      _PA_418_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_418_:         
      jra      next_plane              | [420]





left_loop:
      movea.l  %d6,%a0                 | [427]
      movea.l  %d7,%a1                 | [428]
|;;   adda.w    -16(a6),a0             | [429]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [430]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [431]
|;;   rol.w     -78(a6)                | [432]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [434 NE]
      jne      skewopw                 | [435]
mlt_left:
      move.l   %a0,%d6                 | [437]
      move.l   %a1,%d7                 | [438]
      move.w   (-28,%a6),(-44,%a6)     | [439]







      move.l   (%a0)+,%d0              | [447]


      move.w   (%a1),%d4               | [450]

      move.w   (-42,%a6),%d1           | [452]
      lsl.l    %d1,%d0                 | [453]

      swap     %d0                     | [455]
      move.w   %d0,%d1                 | [456]
      swap     %d0                     | [457]

      move.w   (-38,%a6),%d2           | [459]

      lea      lft_lfrng,%a3           | [461]
      jmp      (%a5)                   | [462]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [465]
|;;   adda.w    -88(a6),a1             | [466]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [468]
      lea      lft_wrds,%a3            | [469]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [474]
|;;   and.w     d2,d4                  | [475]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [476]





      swap     %d0                     | [482]
      move.w   %d1,%d0                 | [483]
      swap     %d0                     | [484]

      move.w   (-42,%a6),%d1           | [486]
      lsl.l    %d1,%d0                 | [487]

      swap     %d0                     | [489]
      move.w   %d0,%d1                 | [490]
      swap     %d0                     | [491]

|;;   not.w     d2                     | [493]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [494]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [495]
	not.l    %d5
|;;   eor.w     d4,d1                  | [496]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [498]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [500 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [501]

      jmp      (%a4)                   | [503]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [506]
|;;   adda.w    -88(a6),a1             | [507]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [509]

lef_don:
      move.w   (-36,%a6),%d2           | [512]

      lea      lft_rfrng,%a3           | [514]
      jmp      (%a5)                   | [515]

lft_rfrng:
      move.w   %d1,(%a1)               | [518]

|;;   dbra      d3,left_loop           | [520]
	tst.w    %d3
	jeq      _PA_519_
	subq.l   #1,%d3
	jra      left_loop
	_PA_519_:         
      jra      next_plane              | [521]





rite_loop:
      movea.l  %d6,%a0                 | [528]
      movea.l  %d7,%a1                 | [529]
|;;   adda.w    -16(a6),a0             | [530]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [531]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [532]
|;;   rol.w     -78(a6)                | [533]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [535 NE]
      jne      skewopw                 | [536]
mlt_rite:
      move.l   %a0,%d6                 | [538]
      move.l   %a1,%d7                 | [539]
      move.w   (-28,%a6),(-44,%a6)     | [540]
      move.w   (%a0)+,%d0              | [541]



      swap     %d0                     | [545]

      move.w   (%a1),%d4               | [547]

      move.w   (-42,%a6),%d1           | [549]
      lsr.l    %d1,%d0                 | [550]
      swap     %d0                     | [551]
      move.w   %d0,%d1                 | [552]
      swap     %d0                     | [553]

      move.w   (-38,%a6),%d2           | [555]

      lea      rgt_lfrng,%a3           | [557]
      jmp      (%a5)                   | [558]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [561]
|;;   adda.w    -88(a6),a1             | [562]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [564]
      lea      rgt_wrds,%a3            | [565]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [570]
|;;   and.w     d2,d4                  | [571]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [572]
      move.w   (%a0)+,%d0              | [573]



      swap     %d0                     | [577]

      move.w   (-42,%a6),%d1           | [579]
      lsr.l    %d1,%d0                 | [580]
      swap     %d0                     | [581]
      move.w   %d0,%d1                 | [582]
      swap     %d0                     | [583]

|;;   not.w     d2                     | [585]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [586]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [587]
	not.l    %d5
|;;   eor.w     d4,d1                  | [588]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [590]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [592 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [593]

      jmp      (%a4)                   | [595]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [598]
|;;   adda.w    -88(a6),a1             | [599]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [601]

rite_don:
      move.w   (-36,%a6),%d2           | [604]

      lea      rgt_rfrng,%a3           | [606]
      jmp      (%a5)                   | [607]

rgt_rfrng:
      move.w   %d1,(%a1)               | [610]

|;;   dbra      d3,rite_loop           | [612]
	tst.w    %d3
	jeq      _PA_611_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_611_:         
      jra      next_plane              | [613]





top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [620]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [621]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [622]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [623]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [624]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [627]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [628]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [629]
	not.l    %d0
|;;   and.w     d4,d1                  | [630]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [631]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [634]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [635]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [636]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [637]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [640]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [641]
	not.l    %d0
|;;   and.w     d4,d1                  | [642]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [643]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [646]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [647]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [648]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [651]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [652]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [653]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [656]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [657]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [658]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [659]





twop0:
      moveq.l  #0,%d1                  | [666]
      jmp      (%a3)                   | [667]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [670]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [671]

twop3:
      jmp      (%a3)                   | [674]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [677]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [678]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [679]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [682]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [683]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [686]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [687]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [690]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [691]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [692]
| 1116 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [695]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [696]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [697]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [698 EQ]
      jeq      thk00                   | [699]
|;;   ori.b     #0x10,ccr              | [700 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [701 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [702]
      move.w   #0x8000,%d6             | [703]
thk01:
      move.w   %d6,(-34,%a6)           | [705]
thk00:
      tst.w    (-28,%a6)               | [707 MI]
      jmi      thk0                    | [708]
      cmp.w    (-36,%a6),%d6           | [709 CC]
      jcc      thk0                    | [710]
|;;   and.w     d6,d0                  | [711]
	and.l    %d6,%d0
      jra      thk1                    | [712]
thk0:
      clr.w    %d0                     | [714]
|;;   and.w     d6,d1                  | [715]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [717]
      move.w   %d2,%d5                 | [718]
      swap     %d5                     | [719]
      move.w   (-36,%a6),%d5           | [720]
      lsl.l    %d6,%d5                 | [721]

      swap     %d5                     | [723]
      move.w   %d5,%d7                 | [724]
      swap     %d5                     | [725]

|;;   and.w     d7,d1                  | [727]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [728]
	and.l    %d5,%d0
|;;   swap      d1                     | [729]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [730]
|;;   subq.w    #1,d6                  | [731]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [733]
      lsr.l    #1,%d0                  | [734]
      or.l     %d0,%d1                 | [735]
|;;   dbra      d6,thkoplp             | [736]
	tst.w    %d6
	jeq      _PA_735_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_735_:         

      move.w   %d1,%d0                 | [738]
      swap     %d1                     | [739]
|;;   movem.l   (sp)+,d5-d7            | [740]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [741]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [742]
| 1179 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [745]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [746 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [747]
|;;   and.w     -34(a6),d1             | [748]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [750]
      swap     %d1                     | [751]
      clr.w    %d1                     | [752]
      move.l   %d1,%d5                 | [753]
|;;   subq.w    #1,d6                  | [754]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [756]
      or.l     %d5,%d1                 | [757]
|;;   dbra      d6,thkopwlp            | [758]
	tst.w    %d6
	jeq      _PA_757_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_757_:         

      move.w   %d1,%d5                 | [760]
      swap     %d1                     | [761]
|;;   or.w      -82(a6),d1             | [762]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [763]
|;;   movem.l   (sp)+,d5-d6            | [764]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [765]
      jmp      (%a2)                   | [766]
| 1217 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [769]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [770]
|;;   subq.w    #1,-30(a6)             | [771 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [772]
      jeq      thk4                    | [773]

|;;   subq.w    #1,-30(a6)             | [775 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [776]
|;;   and.w     d6,d1                  | [777]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [779]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [780]

thk3:
      clr.w    %d1                     | [783]
      jra      thk5                    | [784]

thk4:
|;;   and.w     d6,d1                  | [787]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [790 MI]
      jmi      thk6                    | [791]
      move.w   (-28,%a6),%d5           | [792]
|;;   addq.w    #2,d5                  | [793]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [794]
|;;   ori.b     #0x10,ccr              | [795 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [796 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [797]
      move.w   #0x8000,%d7             | [798]
|;;   addq.w    #1,d5                  | [799]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [801 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [802]
|;;   subq.w    #1,d5                  | [803]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [805 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [806 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [807]
      move.w   #0x8000,%d6             | [808]
thk31:
      cmp.w    %d6,%d7                 | [810 CS]
      jcs      thk35                   | [811]
|;;   addq.w    #1,d5                  | [812]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [814]
      move.w   %d5,(-32,%a6)           | [815]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [817]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [819]
      swap     %d1                     | [820]
      clr.w    %d1                     | [821]
      move.l   %d1,%d5                 | [822]
|;;   subq.w    #1,d6                  | [823]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [825]
      or.l     %d5,%d1                 | [826]
|;;   dbra      d6,thkopwl             | [827]
	tst.w    %d6
	jeq      _PA_826_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_826_:         

      move.w   %d1,%d5                 | [829]
      swap     %d1                     | [830]
|;;   or.w      -82(a6),d1             | [831]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [832]
|;;   and.w     d2,d1                  | [833]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [834]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [835]
      jmp      (%a2)                   | [836]
| 1298 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [839]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [840]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [841]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [842]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [843]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [846]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [847]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [848]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [851]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [852]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [853]
| 1330 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [856 CC]
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
      jcc      no_shift                | [857]

      swap     %d1                     | [859]
      move.w   %d0,%d1                 | [860]
      lsr.l    #1,%d1                  | [861]

      swap     %d2                     | [863]
      move.w   (-36,%a6),%d2           | [864]
      lsr.l    #1,%d2                  | [865]
      move.w   %d2,(-36,%a6)           | [866]
      swap     %d2                     | [867]
      move.w   %d2,(-38,%a6)           | [868 EQ]
      jeq      nxt_word                | [869]
      move.w   (-42,%a6),%d0           | [870 MI]
      jmi      dec_rol                 | [871]
ror_add:
|;;   addq.w    #1,d0                  | [873]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [875]
      move.w   %d1,%d0                 | [876]
      swap     %d1                     | [877]
no_shift:
      movea.l  (-56,%a6),%a2           | [879]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [880]

dec_rol:
      tst.b    %d0                     | [883 EQ]
      jeq      begn_ror                | [884]
|;;   subq.w    #1,d0                  | [885]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [886]
begn_ror:
      clr.w    %d0                     | [888]
      jra      ror_add                 | [889]



nxt_word:
      move.w   %d2,(-36,%a6)           | [894]
      swap     %d2                     | [895]
      move.w   %d2,(-38,%a6)           | [896]
|;;   adda.w    -88(a6),a1             | [897]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [898]

      moveq.l  #15,%d0                 | [900]
|;;   sub.w     -42(a6),d0             | [901]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [902]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [903]
      jra      no_shift                | [904]






skewopw:
|;;   rol.w     -80(a6)                | [912 CC]
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
      jcc      do_shift                | [913]

      tst.w    (-42,%a6)               | [915 MI]
      jmi      mlt_left                | [916]
      jra      mlt_rite                | [917]
do_shift:
|;;   ori       #0x10,ccr              | [919 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [920]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [922]
      cmp.w    #0xFFFF,%d0             | [923 EQ]
      jeq      inc_rite                | [924]

|;;   ori       #0x10,ccr              | [926 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [927]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [928]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [931]
      tst.b    %d0                     | [932 NE]
      jne      no_rota                 | [933]
      move.w   #0x8000,(-40,%a6)       | [934]
no_rota:
      move.w   (-38,%a6),%d1           | [936]
|;;   lsr.w     #1,d1                  | [937 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [938]

      move.w   %d1,(-38,%a6)           | [940]

      tst.w    %d0                     | [942 MI]
      jmi      wdec_rol                | [943]
|;;   addq.w    #1,-42(a6)             | [944]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [945]
wdec_rol:
      tst.b    %d0                     | [947 EQ]
      jeq      set_msk                 | [948]
|;;   subq.w    #1,-42(a6)             | [949]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [950]
set_msk:
      move.w   #1,(-42,%a6)            | [952]
      jra      mlt_rite                | [953]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [956]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [957]
      jra      do_left                 | [958]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [961]

|;;   subq.w    #1,-28(a6)             | [963]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [964]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [966]
|;;   sub.w     d0,d2                  | [967]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [968]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [969]

      jra      mlt_left                | [971]




rotation:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   _SOURCEX,%d1            | [977]
      move.w   %d1,%d2                 | [978]
|;;   andi.w    #0x0F,d2               | [979]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [980]
|;;   lsr.w     #4,d1                  | [981]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [982]
	add.l    %d1,%d1

      movea.l  (-10,%a6),%a0           | [984]
|;;   adda.w    d1,a0                  | [985]
	mvs.w    %d1,%d0
	adda.l   %d0,%a0

      move.w   (-102,%a6),(-20,%a6)    | [987]
      move.w   _SOURCEY,%d0            | [988]
      move.w   (-106,%a6),%d1          | [989]
      move.w   %d1,(-22,%a6)           | [990]
      move.w   (-16,%a6),%d2           | [991]

      tst.w    (-126,%a6)              | [993 EQ LT]
      jeq      upsd_dwn                | [994]
rot90:
      jlt      top_src                 | [996]
|;;   neg.w     -16(a6)                | [997]
	move.w   (-16,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-16,%a6)
|;;   subq.w    #1,d1                  | [998]
	subq.l   #1,%d1
|;;   add.w     d1,d0                  | [999]
	add.l    %d1,%d0
      mulu.w   %d2,%d0                 | [1000]
      adda.l   %d0,%a0                 | [1001]
top_src:
      move.w   (-106,%a6),%d0          | [1003]
|;;   lsr.w     #4,d0                  | [1004]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1005]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1006]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1007]
      movea.l  _SCRTCHP,%a1            | [1008]
|;;   adda.w    -4(a6),a1              | [1009]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1

      tst.w    (-126,%a6)              | [1011 GT]
      jgt      top_dwn                 | [1012]
|;;   neg.w     -18(a6)                | [1013]
	move.w   (-18,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1014]
|;;   subq.w    #1,d1                  | [1015]
	subq.l   #1,%d1
      mulu.w   %d1,%d0                 | [1016]
      adda.l   %d0,%a1                 | [1017]
top_dwn:
      move.w   (-24,%a6),%d2           | [1019]
      move.w   #0x8000,%d3             | [1020]
      move.w   %d3,%d4                 | [1021]
      movea.w  %d3,%a4                 | [1022]
|;;   lsr.w     d2,d3                  | [1023]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3
      moveq.l  #0,%d0                  | [1024]
      move.w   (-20,%a6),%d2           | [1025]
      move.w   (-16,%a6),%d6           | [1026]
      movea.l  %a0,%a2                 | [1027]
      movea.l  %a1,%a3                 | [1028]
      jra      rot_nsrc                | [1029]
rot_ylp:
      move.w   (-22,%a6),%d1           | [1031]
      jra      rot_srt                 | [1032]
rot_xlp:
      move.w   (%a0),%d7               | [1034]
|;;   and.w     d3,d7                  | [1035 EQ]
	and.l    %d3,%d7
	tst.w    %d7
      jeq      rot_nor                 | [1036]
|;;   or.w      d4,d0                  | [1037]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     #1,d4                  | [1039 CC]
	move.w   %d4,%d5
	swap     %d4
	move.w   %d5,%d4
	lsr.l    #1,%d4
      jcc      rot_isrc                | [1040]
rot_ndst:
      move.w   %d0,(%a1)+              | [1042]
      moveq.l  #0,%d0                  | [1043]
rot_isrc:
|;;   adda.w    d6,a0                  | [1045]
	mvs.w    %d6,%d5
	adda.l   %d5,%a0
rot_srt:
|;;   dbra      d1,rot_xlp             | [1047]
	tst.w    %d1
	jeq      _PA_1046_
	subq.l   #1,%d1
	jra      rot_xlp
	_PA_1046_:         

      move.w   %d0,(%a1)               | [1049]
      moveq.l  #0,%d0                  | [1050]
|;;   adda.w    -18(a6),a3             | [1051]
	mvs.w    (-18,%a6),%d5
	adda.l   %d5,%a3
      movea.l  %a3,%a1                 | [1052]
      move.w   %a4,%d4                 | [1053]
|;;   ror.w     #1,d3                  | [1054 CC]
	move.w   %d3,%d5
	swap     %d3
	move.w   %d5,%d3
	lsr.l    #1,%d3
      jcc      rnew_src                | [1055]
      addq.l   #2,%a2                  | [1056]
rnew_src:
      movea.l  %a2,%a0                 | [1058]
rot_nsrc:
|;;   dbra      d2,rot_ylp             | [1060]
	tst.w    %d2
	jeq      _PA_1059_
	subq.l   #1,%d2
	jra      rot_ylp
	_PA_1059_:         

rot_done:
      move.w   (-102,%a6),%d0          | [1063]
      move.w   (-106,%a6),%d1          | [1064]
      move.w   %d1,(-20,%a6)           | [1065]
      move.w   %d1,(-102,%a6)          | [1066]
      move.w   %d0,(-22,%a6)           | [1067]
      move.w   %d0,(-106,%a6)          | [1068]

      move.w   (-92,%a6),%d0           | [1070]
      move.w   (-90,%a6),(-92,%a6)     | [1071]
      move.w   %d0,(-90,%a6)           | [1072]
      move.w   #1,(-94,%a6)            | [1073]

      move.w   (-18,%a6),%d0           | [1075]
      tst.w    (-126,%a6)              | [1076 GT]
      jgt      rot_nneg                | [1077]
|;;   neg.w     d0                     | [1078]
	neg.l    %d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1080]
      move.w   (-4,%a6),%d0            | [1081]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5
repexit:
      clr.w    _SOURCEX                | [1083]
repexit3:
      movea.l  _SCRTCHP,%a2            | [1085]
|;;   adda.w    d0,a2                  | [1086]
	ext.l    %d0
	adda.l   %d0,%a2
      move.l   %a2,(-10,%a6)           | [1087]
      clr.w    _SOURCEY                | [1088]
      rts                              | [1089]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1092]
|;;   add.w     -24(a6),d0             | [1093]
	move.w   (-24,%a6),%d5
	add.l    %d5,%d0
|;;   subq.w    #1,d0                  | [1094]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1095]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1096]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1097]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1099]
      move.w   %d0,%d2                 | [1100]
|;;   lsr.w     #1,d2                  | [1101]
	lsr.l    #1,%d2
|;;   subq.w    #1,d2                  | [1102]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1103]
|;;   add.w     -4(a6),d0              | [1104]
	move.w   (-4,%a6),%d5
	add.l    %d5,%d0
      movea.l  _SCRTCHP,%a1            | [1105]
|;;   adda.w    d0,a1                  | [1106]
	ext.l    %d0
	adda.l   %d0,%a1
      jra      strtflip                | [1107]
upsd_lp:
      movea.l  %a0,%a2                 | [1109]
      move.w   %d2,%d3                 | [1110]
line_lp:
      move.w   (%a2)+,%d0              | [1112]
      moveq.l  #0,%d6                  | [1113]
      moveq.l  #15,%d4                 | [1114]
flip_lp:
|;;   lsr.w     #1,d0                  | [1116 X]
	and.l    #0xffff,%d0
	lsr.l    #1,%d0
|;;   roxl.w    #1,d6                  | [1117]
	addx.l   %d6,%d6
|;;   dbra      d4,flip_lp             | [1118]
	subq.l   #1,%d4
	jpl      flip_lp

      move.w   %d6,-(%a1)              | [1120]
|;;   dbra      d3,line_lp             | [1121]
	tst.w    %d3
	jeq      _PA_1120_
	subq.l   #1,%d3
	jra      line_lp
	_PA_1120_:         

|;;   adda.w    -16(a6),a0             | [1123]
	mvs.w    (-16,%a6),%d5
	adda.l   %d5,%a0
strtflip:
|;;   dbra      d1,upsd_lp             | [1125]
	tst.w    %d1
	jeq      _PA_1124_
	subq.l   #1,%d1
	jra      upsd_lp
	_PA_1124_:         

      move.w   (-18,%a6),(-16,%a6)     | [1127]
      movea.l  _SCRTCHP,%a1            | [1128]
|;;   adda.w    -4(a6),a1              | [1129]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1130]
      move.w   _SOURCEX,%d0            | [1131]
|;;   add.w     -102(a6),d0            | [1132]
	move.w   (-102,%a6),%d5
	add.l    %d5,%d0
|;;   neg.w     d0                     | [1133]
	neg.l    %d0
|;;   andi.w    #0xF,d0                | [1134]
	andi.l   #(0xFFFF&0xF),%d0
      move.w   %d0,_SOURCEX            | [1135]
      clr.w    _SOURCEY                | [1136]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      rts                              | [1137]






replicat:
|***	SAVE	d7	***
	lea      -12(%a7),%a7
	movem.l  %d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1145]
      move.w   %d0,%d4                 | [1146]
|;;   andi.w    #0x0F,d0               | [1147]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1148]
|;;   lsr.w     #4,d4                  | [1149]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   add.w     d4,d4                  | [1150]
	add.l    %d4,%d4

      move.w   _SOURCEY,%d0            | [1152]
      mulu.w   (-16,%a6),%d0           | [1153]
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1154]
|;;   adda.w    d4,a0                  | [1155]
	mvs.w    %d4,%d3
	adda.l   %d3,%a0
      adda.l   %d0,%a0                 | [1156]

      move.w   (-24,%a6),%d2           | [1158]
      move.w   #0x8000,%d3             | [1159]
      move.w   %d3,%d4                 | [1160]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1161]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1163]
      move.w   (-102,%a6),%d1          | [1164]

      move.w   %d2,(-22,%a6)           | [1166]
      move.w   %d1,(-20,%a6)           | [1167]
      movea.l  _SCRTCHP,%a1            | [1168]
|;;   adda.w    _SCRPT2,a1             | [1169]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1171 EQ]
      jeq      noline                  | [1172]



|;;   addq.w    #1,-106(a6)            | [1176]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1177]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   add.w     d1,d1                  | [1179]
	add.l    %d1,%d1
|;;   addq.w    #3,d1                  | [1180]
	addq.l   #3,%d1
|;;   lsr.w     #4,d1                  | [1181]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1182]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1183]
	addq.l   #2,%d1
      move.w   %d1,%d5                 | [1184]
      moveq.l  #0,%d6                  | [1185]
oklear:
      move.w   %d6,(%a1)+              | [1187]
|;;   dbra      d5,oklear              | [1188]
	tst.w    %d5
	jeq      _PA_1187_
	subq.l   #1,%d5
	jra      oklear
	_PA_1187_:         
      lea      (-2,%a1),%a2            | [1189]
      move.w   %d2,%d5                 | [1190]
|;;   add.w     d5,d5                  | [1191]
	add.l    %d5,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1193]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1194]
|;;   dbra      d5,oklear1             | [1195]
	tst.w    %d5
	jeq      _PA_1194_
	subq.l   #1,%d5
	jra      oklear1
	_PA_1194_:         

|;;   move.w    d1,d5                  | [1197]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1198]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   subq.w    #1,d5                  | [1199 X]
	subq.l   #1,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1201]
|;;   dbra      d5,oklear3             | [1202]
	move     %CCR,%d0
	tst.w    %d5
	jeq      _PA_1201_
	subq.l   #1,%d5
	move     %d0,%CCR
	jra      oklear3
	_PA_1201_:         
	move     %d0,%CCR
      jra      noline1                 | [1203]

noline:
|;;   lsr.w     #3,d1                  | [1206]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   add.w     d1,d1                  | [1207]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1208 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1210]
|***	RESTORE	AFTER d7	***
	movem.l  8(%a7),%d7



      move.w   _SCALDIR,%d7            | [1214]
|;;   roxr.l    #1,d7                  | [1215 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1216]
|;;   roxr.l    #1,d7                  | [1217]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1219]
      move.w   _DDAINC,%d2             | [1220]
      move.w   (-22,%a6),%d5           | [1221]
|;;   subq.w    #1,d5                  | [1222]
	subq.l   #1,%d5
      move.w   #32767,%d6              | [1223]
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      12(%a7),%a7
      tst.l    %d7                     | [1224 MI]
      jmi      rep_ylop                | [1225]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1227 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1228]
      jbsr     yloop                   | [1229]
y_no_drw:
|;;   adda.w    d7,a0                  | [1231]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1232]
	tst.w    %d5
	jeq      _PA_1231_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1231_:         
      jra      y_rep_don               | [1233]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1235 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1236]
      jbsr     yloop                   | [1237]
y_no_rep:
      jbsr     yloop                   | [1239]
|;;   adda.w    d7,a0                  | [1240]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1241]
	tst.w    %d5
	jeq      _PA_1240_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1240_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1243]
|;;   subq.w    #1,d2                  | [1244]
	subq.l   #1,%d2
      move.w   _XDDA,%d1               | [1245]
      move.w   _DDAINC,%d0             | [1246]
      moveq.l  #0,%d3                  | [1247]
repwidcl:
|;;   add.w     d0,d1                  | [1249 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1250]
|;;   addq.w    #1,d3                  | [1251]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1253 PL]
      jpl      nrpndoub                | [1254]
|;;   addq.w    #1,d3                  | [1255]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1257]
	tst.w    %d2
	jeq      _PA_1256_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1256_:         
      move.w   %d1,_XDDA               | [1258]
      move.w   %d3,(-102,%a6)          | [1259]
      move.w   (-92,%a6),(-106,%a6)    | [1260]


      move.w   (-18,%a6),%d1           | [1263]
      move.w   %d1,(-16,%a6)           | [1264]







repexit1:
      btst     #4,(-96+1,%a6)          | [1273 EQ]
      jeq      repexit2                | [1274]
      movea.l  _SCRTCHP,%a0            | [1275]
|;;   adda.w    _SCRPT2,a0             | [1276]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1277]
|;;   lea       2(a0,d7.w),a1          | [1278]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
      jbsr     outlin1                 | [1279]
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   _SCRPT2,%d0             | [1280]
|;;   add.w     -18(a6),d0             | [1281]
	move.w   (-18,%a6),%d1
	add.l    %d1,%d0
|;;   move.w    #15,_SOURCEX           | [1282]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      jra      repexit3                | [1283]
repexit2:
      move.w   _SCRPT2,%d0             | [1285]
      jra      repexit                 | [1286]
| 1792 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1289]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1290]
      movea.l  %a1,%a4                 | [1291]
      movea.l  %d7,%a5                 | [1292]
      move.w   %d5,(-44,%a6)           | [1293]
      moveq.l  #0,%d0                  | [1294]
      move.w   (-20,%a6),%d5           | [1295]
|;;   subq.w    #1,d5                  | [1296]
	subq.l   #1,%d5

      move.w   _XDDA,%d7               | [1298]
      jra      nextsrc                 | [1299]
innerlp:
|;;   ror.w     #1,d3                  | [1301 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1302]
nextsrc:
      movea.w  (%a0)+,%a2              | [1304]
reploop:
      move.w   %a2,%d6                 | [1306]
|;;   and.w     d3,d6                  | [1307 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1308]


repnor:
      tst.l    %d7                     | [1312 MI]
      jmi      repnorup                | [1313]
|;;   add.w     d2,d7                  | [1314 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1315]
      jra      ordone                  | [1316]
repnorup:
|;;   add.w     d2,d7                  | [1318 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1319]


|;;   ror.w     #1,d4                  | [1322 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1325]
      move.w   %d0,(%a1)+              | [1326]
      moveq.l  #0,%d0                  | [1327]


      jra      ordone                  | [1330]
nrepnor:


|;;   add.w     d2,d7                  | [1334 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1335]


|;;   or.w      d4,d0                  | [1338]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1339 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1341]
      move.w   %d0,(%a1)+              | [1342]
      moveq.l  #0,%d0                  | [1343]
o_no_rep:
      tst.l    %d7                     | [1345 PL]
      jpl      incsrc                  | [1346]


|;;   or.w      d4,d0                  | [1349]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1351 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1352]
nextdst:
      move.w   %d0,(%a1)+              | [1354]
      moveq.l  #0,%d0                  | [1355]
incsrc:
|;;   dbra      d5,innerlp             | [1357]
	tst.w    %d5
	jeq      _PA_1356_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1356_:         
repdone:
      move.w   %d0,(%a1)               | [1359]
|;;   movem.l   (sp)+,d3-d4/d6         | [1360]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1361]
      movea.l  %a4,%a1                 | [1362]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1363]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1364]
      move.w   (-44,%a6),%d5           | [1365]
      rts                              | [1366]
| 1880 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1369]
	ext.l    %d7
	lea      (0,%a0,%d7.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1371]
	ext.l    %d7
	lea      (0,%a1,%d7.l*1),%a2
|;;   lsr.w     #1,d7                  | [1372]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
|;;   subq.w    #1,d7                  | [1373]
	subq.l   #1,%d7
      move.w   %d7,(-44,%a6)           | [1374]
      move.w   (-106,%a6),%d6          | [1375]

      movea.l  %a0,%a5                 | [1377]
|;;   subq.w    #1,d6                  | [1378]
	subq.l   #1,%d6
out_edge:
      movea.w  %d6,%a4                 | [1380]
      movea.l  %a2,%a3                 | [1381]
      moveq.l  #0,%d5                  | [1382]
      moveq.l  #0,%d6                  | [1383]
      move.l   (%a2),%d1               | [1384]
      lsr.l    #1,%d1                  | [1385]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1390]
      move.b   %d5,%d0                 | [1391]
|;;   ror.l     #1,d0                  | [1392]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1394]
      move.b   %d6,%d2                 | [1395]
      move.l   %d2,%d3                 | [1396]
|;;   ror.l     #1,d3                  | [1397]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1398]
|;;   ror.l     #1,d4                  | [1399]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1401]
      move.l   %d0,%d6                 | [1402]
      eor.l    %d2,%d0                 | [1403]
      eor.l    %d3,%d5                 | [1404]
      eor.l    %d4,%d6                 | [1405]
|;;   rol.l     #1,d5                  | [1406]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1407]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1408]
      or.l     %d6,%d0                 | [1409]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1411]
      move.l   %d1,%d6                 | [1412]
      eor.l    %d2,%d1                 | [1413]
      eor.l    %d3,%d5                 | [1414]
      eor.l    %d4,%d6                 | [1415]
|;;   rol.l     #1,d5                  | [1416]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1417]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1418]
      or.l     %d5,%d0                 | [1419]
      or.l     %d6,%d0                 | [1420]

      eor.l    %d3,%d2                 | [1422]
      eor.l    %d3,%d4                 | [1423]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1424]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1425]
      or.l     %d4,%d0                 | [1426]
      swap     %d0                     | [1427]

      move.w   (%a1),%d6               | [1429]
      move.w   %d6,%d5                 | [1430]
|;;   eor.w     d0,d5                  | [1431]
	eor.l    %d0,%d5
|;;   and.w     d0,d5                  | [1432]
	and.l    %d0,%d5

      addq.l   #2,%a2                  | [1434]
      move.l   (%a2),%d1               | [1435]
      move.b   (-1,%a2),%d1            | [1436]
|;;   ror.l     #1,d1                  | [1437]
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1439]
      move.w   (%a0),%d5               | [1440]
      move.w   %d6,(%a0)+              | [1441]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1443]
	tst.w    %d7
	jeq      _PA_1442_
	subq.l   #1,%d7
	jra      out_loop
	_PA_1442_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1445]
      movea.l  %a5,%a0                 | [1446]
      move.w   %a4,%d6                 | [1447]
      movea.l  %a3,%a2                 | [1448]
      movea.l  %a2,%a1                 | [1449]
|;;   adda.w    -16(a6),a2             | [1450]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1451 NE]
      jne      srt_lin                 | [1452]
      movea.l  %a1,%a2                 | [1453]
srt_lin:
|;;   dbra      d6,out_edge            | [1455]
	tst.w    %d6
	jeq      _PA_1454_
	subq.l   #1,%d6
	jra      out_edge
	_PA_1454_:         
      rts                              | [1456]

| End section on aligned boundary
	.p2align 2,0
