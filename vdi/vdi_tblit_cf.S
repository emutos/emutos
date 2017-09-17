
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
| Translated on Sun Sep 17 18:28:57 2017 by version 2.6.0 of PortAsm translator
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
| 493 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*3,4*3    | [105]
	.byte	4*0,4*0,4*3,4*3
|;;           .dc.b 4*4,4*4,4*7,4*7    | [106]
	.byte	4*4,4*4,4*7,4*7
|;;           .dc.b 4*6,4*6,4*6,4*6    | [107]
	.byte	4*6,4*6,4*6,4*6
|;;           .dc.b 4*1,4*1,4*13,4*13  | [108]
	.byte	4*1,4*1,4*13,4*13

|;;           .dc.b 4*0,4*15,4*0,4*15  | [110]
	.byte	4*0,4*15,4*0,4*15
|;;           .dc.b 4*0,4*14,4*1,4*15  | [111]
	.byte	4*0,4*14,4*1,4*15
|;;           .dc.b 4*0,4*13,4*2,4*15  | [112]
	.byte	4*0,4*13,4*2,4*15
|;;           .dc.b 4*0,4*12,4*3,4*15  | [113]
	.byte	4*0,4*12,4*3,4*15
|;;           .dc.b 4*0,4*11,4*4,4*15  | [114]
	.byte	4*0,4*11,4*4,4*15
|;;           .dc.b 4*0,4*10,4*5,4*15  | [115]
	.byte	4*0,4*10,4*5,4*15
|;;           .dc.b 4*0,4*9,4*6,4*15   | [116]
	.byte	4*0,4*9,4*6,4*15
|;;           .dc.b 4*0,4*8,4*7,4*15   | [117]
	.byte	4*0,4*8,4*7,4*15
|;;           .dc.b 4*0,4*7,4*8,4*15   | [118]
	.byte	4*0,4*7,4*8,4*15
|;;           .dc.b 4*0,4*6,4*9,4*15   | [119]
	.byte	4*0,4*6,4*9,4*15
|;;           .dc.b 4*0,4*5,4*10,4*15  | [120]
	.byte	4*0,4*5,4*10,4*15
|;;           .dc.b 4*0,4*4,4*11,4*15  | [121]
	.byte	4*0,4*4,4*11,4*15
|;;           .dc.b 4*0,4*3,4*12,4*15  | [122]
	.byte	4*0,4*3,4*12,4*15
|;;           .dc.b 4*0,4*2,4*13,4*15  | [123]
	.byte	4*0,4*2,4*13,4*15
|;;           .dc.b 4*0,4*1,4*14,4*15  | [124]
	.byte	4*0,4*1,4*14,4*15
|;;           .dc.b 4*0,4*0,4*15,4*15  | [125]
	.byte	4*0,4*0,4*15,4*15

toptable:
|;;           .dc.l top0,top1,top2,top3,top4,top5,top6,top7 | [128]
	.long	top0,top1,top2,top3,top4,top5,top6,top7
|;;           .dc.l top8,top9,topa,topb,topc,topd,tope,topf | [129]
	.long	top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
|;;           .dc.l twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7 | [132]
	.long	twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
|;;           .dc.l twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf | [133]
	.long	twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

|;;           .text                    | [135]
| 537 "vdi/vdi_tblit.S"
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

|;;   add.w     d0,d0                  | [151]
	move.w   %d0,%d5
	add.l    %d0,%d5
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
|;;   add.w     d0,d0                  | [161]
	move.w   %d0,%d5
	add.l    %d0,%d5
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
|;;   add.w     d2,d2                  | [171]
	add.l    %d5,%d5
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



|;;   add.w     d0,d0                  | [185]
	move.w   %d0,%d5
	add.l    %d0,%d5
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
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5

plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [241]
|;;   lsr.w     -84(a6)                | [242 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [243]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [244 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [245]
	addx.l   %d1,%d1



      lea      toptable,%a4            | [249]
	move.w   %d1,%d0
|;;   move.b    wrmappin-toptable(a4,d0.w),d0 | [250]
	move.w   %d0,%a5
	move.b   (wrmappin-toptable,%a4,%a5.l*1),%d0
|;;   movea.l   toptable-toptable(a4,d0.w),a2 | [251]
	move.w   %d0,%a5
	movea.l  (toptable-toptable,%a4,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [252]
      movea.l  %a2,%a5                 | [253]

|;;   movea.l   twoptble-toptable(a4,d0.w),a4 | [255]
	move.w   %d0,%a0
	movea.l  (twoptble-toptable,%a4,%a0.l*1),%a4



      move.w   (-96,%a6),%d0           | [259 EQ]
      jeq      wrt_char                | [260]

      btst     #1,%d0                  | [262 EQ]
      jeq      no_lite                 | [263]



|;;   move.w    _LITEMASK,-78(a6)      | [267]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [268]
      lea      liteop,%a5              | [269]

      move.l   %a2,(-64,%a6)           | [271]
      lea      liteopwf,%a2            | [272]

      move.l   %a4,(-72,%a6)           | [274]
      lea      liteopw,%a4             | [275]

no_lite:
      btst     #0,%d0                  | [278 EQ]
      jeq      no_thick                | [279]



      clr.w    (-82,%a6)               | [283]

      move.l   %a5,(-48,%a6)           | [285]
      lea      thknop,%a5              | [286]

      move.l   %a2,(-60,%a6)           | [288]
      lea      thknopwf,%a2            | [289]

      move.l   %a4,(-68,%a6)           | [291]
      lea      thknopw,%a4             | [292]

no_thick:
      btst     #2,%d0                  | [295 EQ]
      jeq      wrt_char                | [296]



      move.w   (-100,%a6),(-80,%a6)    | [300]
      move.l   %a5,(-56,%a6)           | [301]
      lea      skewop,%a5              | [302]

      tst.w    (-124,%a6)              | [304 EQ GT]
      jgt      wrt_char                | [305]
      jeq      not_sngl                | [306]
      clr.w    (-28,%a6)               | [307]
      moveq.l  #0,%d7                  | [308]
      jra      chng_blt                | [309]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [311 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [312]

      tst.w    (-42,%a6)               | [314 LT]
      jlt      chng_left               | [315]
      moveq.l  #4,%d7                  | [316]
      jra      chng_blt                | [317]
chng_left:
      moveq.l  #8,%d7                  | [319]
chng_blt:
      move.w   %d7,(-124,%a6)          | [321]

wrt_char:
      move.w   (-124,%a6),%d7          | [324 LE]
      jle      set_chr_hgt             | [325]
      movea.l  %a2,%a5                 | [326]
      move.w   (-18,%a6),%d5           | [327]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [329]
|;;   subq.w    #1,d3                  | [330]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [332]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [333]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [335 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [336]
      movea.l  (-10,%a6),%a0           | [337]
      movea.l  (-14,%a6),%a1           | [338]
      addq.l   #2,%a1                  | [339]
      move.l   %a1,(-14,%a6)           | [340]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [341 EQ]
      jeq      plane_loop              | [342]
      jra      norm_blt                | [343]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [345]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [347]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [353]
      move.w   (-16,%a6),%d6           | [354]
      move.w   (-18,%a6),%d7           | [355]
      lea      sgl_rtn,%a3             | [356]
      jra      sgl_1st_scan            | [357]

sgl_lp:
|;;   adda.w    d6,a0                  | [360]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [361]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [363]
| 772 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [365]


      move.w   (-42,%a6),%d1           | [368 MI]
      jmi      left_rol                | [369]
      lsr.l    %d1,%d0                 | [370]
      jra      end_ro                  | [371]
left_rol:
      lsl.l    %d1,%d0                 | [373]
end_ro:
      swap     %d0                     | [375]
      move.w   %d0,%d1                 | [376]
      swap     %d0                     | [377]

      jmp      (%a5)                   | [379]

sgl_rtn:
      move.w   %d1,(%a1)               | [382]
|;;   dbra      d3,sgl_lp              | [383]
	tst.w    %d3
	jeq      _PA_382_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_382_:         
      jra      next_plane              | [384]




dbl_loop:
      movea.l  %a3,%a4                 | [390]
      move.w   (-16,%a6),%d6           | [391]
      move.w   (-18,%a6),%d7           | [392]
      lea      dbl_2wrd,%a2            | [393]
      move.l   %a2,%d5                 | [394]
      lea      dbl_1wrd,%a3            | [395]
      jra      dbl_1st_scan            | [396]

dbl_lp:
|;;   adda.w    d6,a0                  | [399]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [400]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [401]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [403]
| 819 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [405]


      move.w   (-42,%a6),%d1           | [408 MI]
      jmi      dbleft                  | [409]
      lsr.l    %d1,%d0                 | [410]
      jra      dblendr                 | [411]
dbleft:
      lsl.l    %d1,%d0                 | [413]
dblendr:
      swap     %d0                     | [415]
      move.w   %d0,%d1                 | [416]
      swap     %d0                     | [417]
      move.w   (-38,%a6),%d2           | [418]

      jmp      (%a5)                   | [420]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [423]
      move.w   %d0,%d1                 | [424]

      movea.w  (-88,%a6),%a2           | [426]
|;;   move.w    (a1,a2.w),d4           | [427]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [429]

|;;   exg       a3,d5                  | [431]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [432]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [435]
|;;   move.w    d1,(a1,a2.w)           | [436]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [438]
	tst.w    %d3
	jeq      _PA_437_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_437_:         
      jra      next_plane              | [439]





left_loop:
      movea.l  %d6,%a0                 | [446]
      movea.l  %d7,%a1                 | [447]
|;;   adda.w    -16(a6),a0             | [448]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [449]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [450]
|;;   rol.w     -78(a6)                | [451]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [453 NE]
      jne      skewopw                 | [454]
mlt_left:
      move.l   %a0,%d6                 | [456]
      move.l   %a1,%d7                 | [457]
      move.w   (-28,%a6),(-44,%a6)     | [458]







      move.l   (%a0)+,%d0              | [466]


      move.w   (%a1),%d4               | [469]

      move.w   (-42,%a6),%d1           | [471]
      lsl.l    %d1,%d0                 | [472]

      swap     %d0                     | [474]
      move.w   %d0,%d1                 | [475]
      swap     %d0                     | [476]

      move.w   (-38,%a6),%d2           | [478]

      lea      lft_lfrng,%a3           | [480]
      jmp      (%a5)                   | [481]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [484]
|;;   adda.w    -88(a6),a1             | [485]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [487]
      lea      lft_wrds,%a3            | [488]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [493]
|;;   and.w     d2,d4                  | [494]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [495]





      swap     %d0                     | [501]
      move.w   %d1,%d0                 | [502]
      swap     %d0                     | [503]

      move.w   (-42,%a6),%d1           | [505]
      lsl.l    %d1,%d0                 | [506]

      swap     %d0                     | [508]
      move.w   %d0,%d1                 | [509]
      swap     %d0                     | [510]

|;;   not.w     d2                     | [512]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [513]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [514]
	not.l    %d5
|;;   eor.w     d4,d1                  | [515]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [517]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [519 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [520]

      jmp      (%a4)                   | [522]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [525]
|;;   adda.w    -88(a6),a1             | [526]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [528]

lef_don:
      move.w   (-36,%a6),%d2           | [531]

      lea      lft_rfrng,%a3           | [533]
      jmp      (%a5)                   | [534]

lft_rfrng:
      move.w   %d1,(%a1)               | [537]

|;;   dbra      d3,left_loop           | [539]
	tst.w    %d3
	jeq      _PA_538_
	subq.l   #1,%d3
	jra      left_loop
	_PA_538_:         
      jra      next_plane              | [540]





rite_loop:
      movea.l  %d6,%a0                 | [547]
      movea.l  %d7,%a1                 | [548]
|;;   adda.w    -16(a6),a0             | [549]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [550]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [551]
|;;   rol.w     -78(a6)                | [552]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [554 NE]
      jne      skewopw                 | [555]
mlt_rite:
      move.l   %a0,%d6                 | [557]
      move.l   %a1,%d7                 | [558]
      move.w   (-28,%a6),(-44,%a6)     | [559]
      move.w   (%a0)+,%d0              | [560]



      swap     %d0                     | [564]

      move.w   (%a1),%d4               | [566]

      move.w   (-42,%a6),%d1           | [568]
      lsr.l    %d1,%d0                 | [569]
      swap     %d0                     | [570]
      move.w   %d0,%d1                 | [571]
      swap     %d0                     | [572]

      move.w   (-38,%a6),%d2           | [574]

      lea      rgt_lfrng,%a3           | [576]
      jmp      (%a5)                   | [577]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [580]
|;;   adda.w    -88(a6),a1             | [581]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [583]
      lea      rgt_wrds,%a3            | [584]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [589]
|;;   and.w     d2,d4                  | [590]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [591]
      move.w   (%a0)+,%d0              | [592]



      swap     %d0                     | [596]

      move.w   (-42,%a6),%d1           | [598]
      lsr.l    %d1,%d0                 | [599]
      swap     %d0                     | [600]
      move.w   %d0,%d1                 | [601]
      swap     %d0                     | [602]

|;;   not.w     d2                     | [604]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [605]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [606]
	not.l    %d5
|;;   eor.w     d4,d1                  | [607]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [609]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [611 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [612]

      jmp      (%a4)                   | [614]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [617]
|;;   adda.w    -88(a6),a1             | [618]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [620]

rite_don:
      move.w   (-36,%a6),%d2           | [623]

      lea      rgt_rfrng,%a3           | [625]
      jmp      (%a5)                   | [626]

rgt_rfrng:
      move.w   %d1,(%a1)               | [629]

|;;   dbra      d3,rite_loop           | [631]
	tst.w    %d3
	jeq      _PA_630_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_630_:         
      jra      next_plane              | [632]
| 1061 "vdi/vdi_tblit.S"
top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [635]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [636]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [637]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [638]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [639]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [642]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [643]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [644]
	not.l    %d0
|;;   and.w     d4,d1                  | [645]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [646]

top2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [649]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [650]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [651]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [652]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [653]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [656]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [657]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [658]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [659]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [662]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [663]
	not.l    %d0
|;;   and.w     d4,d1                  | [664]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [665]

top5:
      move.w   %d4,%d1                 | [668]
      jmp      (%a3)                   | [669]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [672]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [673]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [674]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [677]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [678]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [679]

top8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [682]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [683]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [684]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [685]

top9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [688]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [689]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [690]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [691]

topa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [694]
|;;   eor.w     d2,d1                  | [695]
	move.w   %d1,%d0
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [696]

topb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d2,d4                  | [699]
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   and.w     d2,d1                  | [700]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [701]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [702]

topc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [705]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [706]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [707]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [708]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [709]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [712]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [713]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [714]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [715]

tope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [718]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [719]
	and.l    %d2,%d0
|;;   eor.w     d2,d4                  | [720]
	move.w   %d0,%d1
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   or.w      d4,d1                  | [721]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [722]

topf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d2,d4                  | [725]
	mvz.w    %d2,%d0
	or.l     %d0,%d4
      move.w   %d4,%d1                 | [726]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [727]
| 1167 "vdi/vdi_tblit.S"
twop0:
      moveq.l  #0,%d1                  | [730]
      jmp      (%a3)                   | [731]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [734]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [735]

twop2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [738]
	move.w   %d4,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [739]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [740]

twop3:
      jmp      (%a3)                   | [743]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [746]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [747]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [748]

twop5:
      move.w   %d4,%d1                 | [751]
      jmp      (%a3)                   | [752]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [755]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [756]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [759]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [760]

twop8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [763]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|;;   not.w     d1                     | [764]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [765]

twop9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [768]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   not.w     d1                     | [769]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [770]

twopa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [773]
|;;   not.w     d1                     | [774]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [775]

twopb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [778]
	move.w   %d4,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [779]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [780]

twopc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [783]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [784]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [787]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [788]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [789]

twope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [792]
	move.w   %d1,%d0
	and.l    %d4,%d0
|;;   not.w     d1                     | [793]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [794]

twopf:
      moveq.l  #(-1),%d1               | [797]
      jmp      (%a3)                   | [798]
| 1255 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [801]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [802]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [803]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [804 EQ]
      jeq      thk00                   | [805]
|;;   ori.b     #0x10,ccr              | [806 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [807 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [808]
      move.w   #0x8000,%d6             | [809]
thk01:
      move.w   %d6,(-34,%a6)           | [811]
thk00:
      tst.w    (-28,%a6)               | [813 MI]
      jmi      thk0                    | [814]
      cmp.w    (-36,%a6),%d6           | [815 CC]
      jcc      thk0                    | [816]
|;;   and.w     d6,d0                  | [817]
	and.l    %d6,%d0
      jra      thk1                    | [818]
thk0:
      clr.w    %d0                     | [820]
|;;   and.w     d6,d1                  | [821]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [823]
      move.w   %d2,%d5                 | [824]
      swap     %d5                     | [825]
      move.w   (-36,%a6),%d5           | [826]
      lsl.l    %d6,%d5                 | [827]

      swap     %d5                     | [829]
      move.w   %d5,%d7                 | [830]
      swap     %d5                     | [831]

|;;   and.w     d7,d1                  | [833]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [834]
	and.l    %d5,%d0
|;;   swap      d1                     | [835]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [836]
|;;   subq.w    #1,d6                  | [837]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [839]
      lsr.l    #1,%d0                  | [840]
      or.l     %d0,%d1                 | [841]
|;;   dbra      d6,thkoplp             | [842]
	tst.w    %d6
	jeq      _PA_841_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_841_:         

      move.w   %d1,%d0                 | [844]
      swap     %d1                     | [845]
|;;   movem.l   (sp)+,d5-d7            | [846]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [847]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [848]
| 1318 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [851]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [852 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [853]
|;;   and.w     -34(a6),d1             | [854]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [856]
      swap     %d1                     | [857]
      clr.w    %d1                     | [858]
      move.l   %d1,%d5                 | [859]
|;;   subq.w    #1,d6                  | [860]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [862]
      or.l     %d5,%d1                 | [863]
|;;   dbra      d6,thkopwlp            | [864]
	tst.w    %d6
	jeq      _PA_863_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_863_:         

      move.w   %d1,%d5                 | [866]
      swap     %d1                     | [867]
|;;   or.w      -82(a6),d1             | [868]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [869]
|;;   movem.l   (sp)+,d5-d6            | [870]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [871]
      jmp      (%a2)                   | [872]
| 1356 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [875]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [876]
|;;   subq.w    #1,-30(a6)             | [877 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [878]
      jeq      thk4                    | [879]

|;;   subq.w    #1,-30(a6)             | [881 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [882]
|;;   and.w     d6,d1                  | [883]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [885]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [886]

thk3:
      clr.w    %d1                     | [889]
      jra      thk5                    | [890]

thk4:
|;;   and.w     d6,d1                  | [893]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [896 MI]
      jmi      thk6                    | [897]
      move.w   (-28,%a6),%d5           | [898]
|;;   addq.w    #2,d5                  | [899]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [900]
|;;   ori.b     #0x10,ccr              | [901 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [902 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [903]
      move.w   #0x8000,%d7             | [904]
|;;   addq.w    #1,d5                  | [905]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [907 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [908]
|;;   subq.w    #1,d5                  | [909]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [911 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [912 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [913]
      move.w   #0x8000,%d6             | [914]
thk31:
      cmp.w    %d6,%d7                 | [916 CS]
      jcs      thk35                   | [917]
|;;   addq.w    #1,d5                  | [918]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [920]
      move.w   %d5,(-32,%a6)           | [921]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [923]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [925]
      swap     %d1                     | [926]
      clr.w    %d1                     | [927]
      move.l   %d1,%d5                 | [928]
|;;   subq.w    #1,d6                  | [929]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [931]
      or.l     %d5,%d1                 | [932]
|;;   dbra      d6,thkopwl             | [933]
	tst.w    %d6
	jeq      _PA_932_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_932_:         

      move.w   %d1,%d5                 | [935]
      swap     %d1                     | [936]
|;;   or.w      -82(a6),d1             | [937]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [938]
|;;   and.w     d2,d1                  | [939]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [940]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [941]
      jmp      (%a2)                   | [942]
| 1437 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [945]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [946]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [947]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [948]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [949]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [952]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [953]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [954]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [957]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [958]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [959]
| 1469 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [962 CC]
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
      jcc      no_shift                | [963]

      swap     %d1                     | [965]
      move.w   %d0,%d1                 | [966]
      lsr.l    #1,%d1                  | [967]

      swap     %d2                     | [969]
      move.w   (-36,%a6),%d2           | [970]
      lsr.l    #1,%d2                  | [971]
      move.w   %d2,(-36,%a6)           | [972]
      swap     %d2                     | [973]
      move.w   %d2,(-38,%a6)           | [974 EQ]
      jeq      nxt_word                | [975]
      move.w   (-42,%a6),%d0           | [976 MI]
      jmi      dec_rol                 | [977]
ror_add:
|;;   addq.w    #1,d0                  | [979]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [981]
      move.w   %d1,%d0                 | [982]
      swap     %d1                     | [983]
no_shift:
      movea.l  (-56,%a6),%a2           | [985]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [986]

dec_rol:
      tst.b    %d0                     | [989 EQ]
      jeq      begn_ror                | [990]
|;;   subq.w    #1,d0                  | [991]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [992]
begn_ror:
      clr.w    %d0                     | [994]
      jra      ror_add                 | [995]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1000]
      swap     %d2                     | [1001]
      move.w   %d2,(-38,%a6)           | [1002]
|;;   adda.w    -88(a6),a1             | [1003]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1004]

      moveq.l  #15,%d0                 | [1006]
|;;   sub.w     -42(a6),d0             | [1007]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1008]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1009]
      jra      no_shift                | [1010]






skewopw:
|;;   rol.w     -80(a6)                | [1018 CC]
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
      jcc      do_shift                | [1019]

      tst.w    (-42,%a6)               | [1021 MI]
      jmi      mlt_left                | [1022]
      jra      mlt_rite                | [1023]
do_shift:
|;;   ori       #0x10,ccr              | [1025 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1026]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1028]
      cmp.w    #0xFFFF,%d0             | [1029 EQ]
      jeq      inc_rite                | [1030]

|;;   ori       #0x10,ccr              | [1032 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1033]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1034]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1037]
      tst.b    %d0                     | [1038 NE]
      jne      no_rota                 | [1039]
      move.w   #0x8000,(-40,%a6)       | [1040]
no_rota:
      move.w   (-38,%a6),%d1           | [1042]
|;;   lsr.w     #1,d1                  | [1043 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [1044]

      move.w   %d1,(-38,%a6)           | [1046]

      tst.w    %d0                     | [1048 MI]
      jmi      wdec_rol                | [1049]
|;;   addq.w    #1,-42(a6)             | [1050]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [1051]
wdec_rol:
      tst.b    %d0                     | [1053 EQ]
      jeq      set_msk                 | [1054]
|;;   subq.w    #1,-42(a6)             | [1055]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [1056]
set_msk:
      move.w   #1,(-42,%a6)            | [1058]
      jra      mlt_rite                | [1059]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1062]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1063]
      jra      do_left                 | [1064]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1067]

|;;   subq.w    #1,-28(a6)             | [1069]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1070]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1072]
|;;   sub.w     d0,d2                  | [1073]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1074]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1075]

      jra      mlt_left                | [1077]




rotation:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   _SOURCEX,%d1            | [1083]
      move.w   %d1,%d2                 | [1084]
|;;   andi.w    #0x0F,d2               | [1085]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1086]
|;;   lsr.w     #4,d1                  | [1087]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1088]
	add.l    %d1,%d1

      movea.l  (-10,%a6),%a0           | [1090]
|;;   adda.w    d1,a0                  | [1091]
	mvs.w    %d1,%d0
	adda.l   %d0,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1093]
      move.w   _SOURCEY,%d0            | [1094]
      move.w   (-106,%a6),%d1          | [1095]
      move.w   %d1,(-22,%a6)           | [1096]
      move.w   (-16,%a6),%d2           | [1097]

      tst.w    (-126,%a6)              | [1099 EQ LT]
      jeq      upsd_dwn                | [1100]
rot90:
      jlt      top_src                 | [1102]
|;;   neg.w     -16(a6)                | [1103]
	move.w   (-16,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-16,%a6)
|;;   subq.w    #1,d1                  | [1104]
	subq.l   #1,%d1
|;;   add.w     d1,d0                  | [1105]
	add.l    %d1,%d0
      mulu.w   %d2,%d0                 | [1106]
      adda.l   %d0,%a0                 | [1107]
top_src:
      move.w   (-106,%a6),%d0          | [1109]
|;;   lsr.w     #4,d0                  | [1110]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1111]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1112]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1113]
      movea.l  _SCRTCHP,%a1            | [1114]
|;;   adda.w    -4(a6),a1              | [1115]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1

      tst.w    (-126,%a6)              | [1117 GT]
      jgt      top_dwn                 | [1118]
|;;   neg.w     -18(a6)                | [1119]
	move.w   (-18,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1120]
|;;   subq.w    #1,d1                  | [1121]
	subq.l   #1,%d1
      mulu.w   %d1,%d0                 | [1122]
      adda.l   %d0,%a1                 | [1123]
top_dwn:
      move.w   (-24,%a6),%d2           | [1125]
      move.w   #0x8000,%d3             | [1126]
      move.w   %d3,%d4                 | [1127]
      movea.w  %d3,%a4                 | [1128]
|;;   lsr.w     d2,d3                  | [1129]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3
      moveq.l  #0,%d0                  | [1130]
      move.w   (-20,%a6),%d2           | [1131]
      move.w   (-16,%a6),%d6           | [1132]
      movea.l  %a0,%a2                 | [1133]
      movea.l  %a1,%a3                 | [1134]
      jra      rot_nsrc                | [1135]
rot_ylp:
      move.w   (-22,%a6),%d1           | [1137]
      jra      rot_srt                 | [1138]
rot_xlp:
      move.w   (%a0),%d7               | [1140]
|;;   and.w     d3,d7                  | [1141 EQ]
	and.l    %d3,%d7
	tst.w    %d7
      jeq      rot_nor                 | [1142]
|;;   or.w      d4,d0                  | [1143]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     #1,d4                  | [1145 CC]
	move.w   %d4,%d5
	swap     %d4
	move.w   %d5,%d4
	lsr.l    #1,%d4
      jcc      rot_isrc                | [1146]
rot_ndst:
      move.w   %d0,(%a1)+              | [1148]
      moveq.l  #0,%d0                  | [1149]
rot_isrc:
|;;   adda.w    d6,a0                  | [1151]
	mvs.w    %d6,%d5
	adda.l   %d5,%a0
rot_srt:
|;;   dbra      d1,rot_xlp             | [1153]
	tst.w    %d1
	jeq      _PA_1152_
	subq.l   #1,%d1
	jra      rot_xlp
	_PA_1152_:         

      move.w   %d0,(%a1)               | [1155]
      moveq.l  #0,%d0                  | [1156]
|;;   adda.w    -18(a6),a3             | [1157]
	mvs.w    (-18,%a6),%d5
	adda.l   %d5,%a3
      movea.l  %a3,%a1                 | [1158]
      move.w   %a4,%d4                 | [1159]
|;;   ror.w     #1,d3                  | [1160 CC]
	move.w   %d3,%d5
	swap     %d3
	move.w   %d5,%d3
	lsr.l    #1,%d3
      jcc      rnew_src                | [1161]
      addq.l   #2,%a2                  | [1162]
rnew_src:
      movea.l  %a2,%a0                 | [1164]
rot_nsrc:
|;;   dbra      d2,rot_ylp             | [1166]
	tst.w    %d2
	jeq      _PA_1165_
	subq.l   #1,%d2
	jra      rot_ylp
	_PA_1165_:         

rot_done:
      move.w   (-102,%a6),%d0          | [1169]
      move.w   (-106,%a6),%d1          | [1170]
      move.w   %d1,(-20,%a6)           | [1171]
      move.w   %d1,(-102,%a6)          | [1172]
      move.w   %d0,(-22,%a6)           | [1173]
      move.w   %d0,(-106,%a6)          | [1174]

      move.w   (-92,%a6),%d0           | [1176]
      move.w   (-90,%a6),(-92,%a6)     | [1177]
      move.w   %d0,(-90,%a6)           | [1178]
      move.w   #1,(-94,%a6)            | [1179]

      move.w   (-18,%a6),%d0           | [1181]
      tst.w    (-126,%a6)              | [1182 GT]
      jgt      rot_nneg                | [1183]
|;;   neg.w     d0                     | [1184]
	neg.l    %d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1186]
      move.w   (-4,%a6),%d0            | [1187]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5
repexit:
      clr.w    _SOURCEX                | [1189]
repexit3:
      movea.l  _SCRTCHP,%a2            | [1191]
|;;   adda.w    d0,a2                  | [1192]
	ext.l    %d0
	adda.l   %d0,%a2
      move.l   %a2,(-10,%a6)           | [1193]
      clr.w    _SOURCEY                | [1194]
      rts                              | [1195]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1198]
|;;   add.w     -24(a6),d0             | [1199]
	move.w   (-24,%a6),%d5
	add.l    %d5,%d0
|;;   subq.w    #1,d0                  | [1200]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1201]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1202]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1203]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1205]
      move.w   %d0,%d2                 | [1206]
|;;   lsr.w     #1,d2                  | [1207]
	lsr.l    #1,%d2
|;;   subq.w    #1,d2                  | [1208]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1209]
|;;   add.w     -4(a6),d0              | [1210]
	move.w   (-4,%a6),%d5
	add.l    %d5,%d0
      movea.l  _SCRTCHP,%a1            | [1211]
|;;   adda.w    d0,a1                  | [1212]
	ext.l    %d0
	adda.l   %d0,%a1
      jra      strtflip                | [1213]
upsd_lp:
      movea.l  %a0,%a2                 | [1215]
      move.w   %d2,%d3                 | [1216]
line_lp:
      move.w   (%a2)+,%d0              | [1218]
      moveq.l  #0,%d6                  | [1219]
      moveq.l  #15,%d4                 | [1220]
flip_lp:
|;;   lsr.w     #1,d0                  | [1222 X]
	and.l    #0xffff,%d0
	lsr.l    #1,%d0
|;;   roxl.w    #1,d6                  | [1223]
	addx.l   %d6,%d6
|;;   dbra      d4,flip_lp             | [1224]
	subq.l   #1,%d4
	jpl      flip_lp

      move.w   %d6,-(%a1)              | [1226]
|;;   dbra      d3,line_lp             | [1227]
	tst.w    %d3
	jeq      _PA_1226_
	subq.l   #1,%d3
	jra      line_lp
	_PA_1226_:         

|;;   adda.w    -16(a6),a0             | [1229]
	mvs.w    (-16,%a6),%d5
	adda.l   %d5,%a0
strtflip:
|;;   dbra      d1,upsd_lp             | [1231]
	tst.w    %d1
	jeq      _PA_1230_
	subq.l   #1,%d1
	jra      upsd_lp
	_PA_1230_:         

      move.w   (-18,%a6),(-16,%a6)     | [1233]
      movea.l  _SCRTCHP,%a1            | [1234]
|;;   adda.w    -4(a6),a1              | [1235]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1236]
      move.w   _SOURCEX,%d0            | [1237]
|;;   add.w     -102(a6),d0            | [1238]
	move.w   (-102,%a6),%d5
	add.l    %d5,%d0
|;;   neg.w     d0                     | [1239]
	neg.l    %d0
|;;   andi.w    #0xF,d0                | [1240]
	andi.l   #(0xFFFF&0xF),%d0
      move.w   %d0,_SOURCEX            | [1241]
      clr.w    _SOURCEY                | [1242]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      rts                              | [1243]






replicat:
|***	SAVE	d7	***
	lea      -12(%a7),%a7
	movem.l  %d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1251]
      move.w   %d0,%d4                 | [1252]
|;;   andi.w    #0x0F,d0               | [1253]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1254]
|;;   lsr.w     #4,d4                  | [1255]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   add.w     d4,d4                  | [1256]
	add.l    %d4,%d4

      move.w   _SOURCEY,%d0            | [1258]
      mulu.w   (-16,%a6),%d0           | [1259]
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1260]
|;;   adda.w    d4,a0                  | [1261]
	mvs.w    %d4,%d3
	adda.l   %d3,%a0
      adda.l   %d0,%a0                 | [1262]

      move.w   (-24,%a6),%d2           | [1264]
      move.w   #0x8000,%d3             | [1265]
      move.w   %d3,%d4                 | [1266]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1267]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1269]
      move.w   (-102,%a6),%d1          | [1270]

      move.w   %d2,(-22,%a6)           | [1272]
      move.w   %d1,(-20,%a6)           | [1273]
      movea.l  _SCRTCHP,%a1            | [1274]
|;;   adda.w    _SCRPT2,a1             | [1275]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1277 EQ]
      jeq      noline                  | [1278]



|;;   addq.w    #1,-106(a6)            | [1282]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1283]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   add.w     d1,d1                  | [1285]
	add.l    %d1,%d1
|;;   addq.w    #3,d1                  | [1286]
	addq.l   #3,%d1
|;;   lsr.w     #4,d1                  | [1287]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1288]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1289]
	addq.l   #2,%d1
      move.w   %d1,%d5                 | [1290]
      moveq.l  #0,%d6                  | [1291]
oklear:
      move.w   %d6,(%a1)+              | [1293]
|;;   dbra      d5,oklear              | [1294]
	tst.w    %d5
	jeq      _PA_1293_
	subq.l   #1,%d5
	jra      oklear
	_PA_1293_:         
      lea      (-2,%a1),%a2            | [1295]
      move.w   %d2,%d5                 | [1296]
|;;   add.w     d5,d5                  | [1297]
	add.l    %d5,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1299]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1300]
|;;   dbra      d5,oklear1             | [1301]
	tst.w    %d5
	jeq      _PA_1300_
	subq.l   #1,%d5
	jra      oklear1
	_PA_1300_:         

|;;   move.w    d1,d5                  | [1303]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1304]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   subq.w    #1,d5                  | [1305 X]
	subq.l   #1,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1307]
|;;   dbra      d5,oklear3             | [1308]
	move     %CCR,%d0
	tst.w    %d5
	jeq      _PA_1307_
	subq.l   #1,%d5
	move     %d0,%CCR
	jra      oklear3
	_PA_1307_:         
	move     %d0,%CCR
      jra      noline1                 | [1309]

noline:
|;;   lsr.w     #3,d1                  | [1312]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   add.w     d1,d1                  | [1313]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1314 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1316]
|***	RESTORE	AFTER d7	***
	movem.l  8(%a7),%d7



      move.w   _SCALDIR,%d7            | [1320]
|;;   roxr.l    #1,d7                  | [1321 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1322]
|;;   roxr.l    #1,d7                  | [1323]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1325]
      move.w   _DDAINC,%d2             | [1326]
      move.w   (-22,%a6),%d5           | [1327]
|;;   subq.w    #1,d5                  | [1328]
	subq.l   #1,%d5
      move.w   #32767,%d6              | [1329]
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      12(%a7),%a7
      tst.l    %d7                     | [1330 MI]
      jmi      rep_ylop                | [1331]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1333 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1334]
      jbsr     yloop                   | [1335]
y_no_drw:
|;;   adda.w    d7,a0                  | [1337]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1338]
	tst.w    %d5
	jeq      _PA_1337_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1337_:         
      jra      y_rep_don               | [1339]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1341 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1342]
      jbsr     yloop                   | [1343]
y_no_rep:
      jbsr     yloop                   | [1345]
|;;   adda.w    d7,a0                  | [1346]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1347]
	tst.w    %d5
	jeq      _PA_1346_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1346_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1349]
|;;   subq.w    #1,d2                  | [1350]
	subq.l   #1,%d2
      move.w   _XDDA,%d1               | [1351]
      move.w   _DDAINC,%d0             | [1352]
      moveq.l  #0,%d3                  | [1353]
repwidcl:
|;;   add.w     d0,d1                  | [1355 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1356]
|;;   addq.w    #1,d3                  | [1357]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1359 PL]
      jpl      nrpndoub                | [1360]
|;;   addq.w    #1,d3                  | [1361]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1363]
	tst.w    %d2
	jeq      _PA_1362_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1362_:         
      move.w   %d1,_XDDA               | [1364]
      move.w   %d3,(-102,%a6)          | [1365]
      move.w   (-92,%a6),(-106,%a6)    | [1366]


      move.w   (-18,%a6),%d1           | [1369]
      move.w   %d1,(-16,%a6)           | [1370]







repexit1:
      btst     #4,(-96+1,%a6)          | [1379 EQ]
      jeq      repexit2                | [1380]
      movea.l  _SCRTCHP,%a0            | [1381]
|;;   adda.w    _SCRPT2,a0             | [1382]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1383]
|;;   lea       2(a0,d7.w),a1          | [1384]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
      jbsr     outlin1                 | [1385]
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   _SCRPT2,%d0             | [1386]
|;;   add.w     -18(a6),d0             | [1387]
	move.w   (-18,%a6),%d1
	add.l    %d1,%d0
|;;   move.w    #15,_SOURCEX           | [1388]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      jra      repexit3                | [1389]
repexit2:
      move.w   _SCRPT2,%d0             | [1391]
      jra      repexit                 | [1392]
| 1931 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1395]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1396]
      movea.l  %a1,%a4                 | [1397]
      movea.l  %d7,%a5                 | [1398]
      move.w   %d5,(-44,%a6)           | [1399]
      moveq.l  #0,%d0                  | [1400]
      move.w   (-20,%a6),%d5           | [1401]
|;;   subq.w    #1,d5                  | [1402]
	subq.l   #1,%d5

      move.w   _XDDA,%d7               | [1404]
      jra      nextsrc                 | [1405]
innerlp:
|;;   ror.w     #1,d3                  | [1407 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1408]
nextsrc:
      movea.w  (%a0)+,%a2              | [1410]
reploop:
      move.w   %a2,%d6                 | [1412]
|;;   and.w     d3,d6                  | [1413 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1414]


repnor:
      tst.l    %d7                     | [1418 MI]
      jmi      repnorup                | [1419]
|;;   add.w     d2,d7                  | [1420 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1421]
      jra      ordone                  | [1422]
repnorup:
|;;   add.w     d2,d7                  | [1424 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1425]


|;;   ror.w     #1,d4                  | [1428 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1431]
      move.w   %d0,(%a1)+              | [1432]
      moveq.l  #0,%d0                  | [1433]


      jra      ordone                  | [1436]
nrepnor:


|;;   add.w     d2,d7                  | [1440 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1441]


|;;   or.w      d4,d0                  | [1444]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1445 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1447]
      move.w   %d0,(%a1)+              | [1448]
      moveq.l  #0,%d0                  | [1449]
o_no_rep:
      tst.l    %d7                     | [1451 PL]
      jpl      incsrc                  | [1452]


|;;   or.w      d4,d0                  | [1455]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1457 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1458]
nextdst:
      move.w   %d0,(%a1)+              | [1460]
      moveq.l  #0,%d0                  | [1461]
incsrc:
|;;   dbra      d5,innerlp             | [1463]
	tst.w    %d5
	jeq      _PA_1462_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1462_:         
repdone:
      move.w   %d0,(%a1)               | [1465]
|;;   movem.l   (sp)+,d3-d4/d6         | [1466]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1467]
      movea.l  %a4,%a1                 | [1468]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1469]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1470]
      move.w   (-44,%a6),%d5           | [1471]
      rts                              | [1472]
| 2019 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1475]
	ext.l    %d7
	lea      (0,%a0,%d7.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1477]
	ext.l    %d7
	lea      (0,%a1,%d7.l*1),%a2
|;;   lsr.w     #1,d7                  | [1478]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
|;;   subq.w    #1,d7                  | [1479]
	subq.l   #1,%d7
      move.w   %d7,(-44,%a6)           | [1480]
      move.w   (-106,%a6),%d6          | [1481]

      movea.l  %a0,%a5                 | [1483]
|;;   subq.w    #1,d6                  | [1484]
	subq.l   #1,%d6
out_edge:
      movea.w  %d6,%a4                 | [1486]
      movea.l  %a2,%a3                 | [1487]
      moveq.l  #0,%d5                  | [1488]
      moveq.l  #0,%d6                  | [1489]
      move.l   (%a2),%d1               | [1490]
      lsr.l    #1,%d1                  | [1491]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1496]
      move.b   %d5,%d0                 | [1497]
|;;   ror.l     #1,d0                  | [1498]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1500]
      move.b   %d6,%d2                 | [1501]
      move.l   %d2,%d3                 | [1502]
|;;   ror.l     #1,d3                  | [1503]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1504]
|;;   ror.l     #1,d4                  | [1505]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1507]
      move.l   %d0,%d6                 | [1508]
      eor.l    %d2,%d0                 | [1509]
      eor.l    %d3,%d5                 | [1510]
      eor.l    %d4,%d6                 | [1511]
|;;   rol.l     #1,d5                  | [1512]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1513]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1514]
      or.l     %d6,%d0                 | [1515]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1517]
      move.l   %d1,%d6                 | [1518]
      eor.l    %d2,%d1                 | [1519]
      eor.l    %d3,%d5                 | [1520]
      eor.l    %d4,%d6                 | [1521]
|;;   rol.l     #1,d5                  | [1522]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1523]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1524]
      or.l     %d5,%d0                 | [1525]
      or.l     %d6,%d0                 | [1526]

      eor.l    %d3,%d2                 | [1528]
      eor.l    %d3,%d4                 | [1529]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1530]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1531]
      or.l     %d4,%d0                 | [1532]
      swap     %d0                     | [1533]

      move.w   (%a1),%d6               | [1535]
      move.w   %d6,%d5                 | [1536]
|;;   eor.w     d0,d5                  | [1537]
	eor.l    %d0,%d5
|;;   and.w     d0,d5                  | [1538]
	and.l    %d0,%d5

      addq.l   #2,%a2                  | [1540]
      move.l   (%a2),%d1               | [1541]
      move.b   (-1,%a2),%d1            | [1542]
|;;   ror.l     #1,d1                  | [1543]
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1545]
      move.w   (%a0),%d5               | [1546]
      move.w   %d6,(%a0)+              | [1547]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1549]
	tst.w    %d7
	jeq      _PA_1548_
	subq.l   #1,%d7
	jra      out_loop
	_PA_1548_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1551]
      movea.l  %a5,%a0                 | [1552]
      move.w   %a4,%d6                 | [1553]
      movea.l  %a3,%a2                 | [1554]
      movea.l  %a2,%a1                 | [1555]
|;;   adda.w    -16(a6),a2             | [1556]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1557 NE]
      jne      srt_lin                 | [1558]
      movea.l  %a1,%a2                 | [1559]
srt_lin:
|;;   dbra      d6,out_edge            | [1561]
	tst.w    %d6
	jeq      _PA_1560_
	subq.l   #1,%d6
	jra      out_edge
	_PA_1560_:         
      rts                              | [1562]

| End section on aligned boundary
	.p2align 2,0
