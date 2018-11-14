
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
| Translated on Thu Nov 15 00:57:04 2018 by version 2.6.0 of PortAsm translator
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
| 113 "vdi/vdi_tblit.S"
|;;           .globl _normal_blit      | [12]
	.globl	_normal_blit
|;;           .globl _outline          | [13]
	.globl	_outline
|;;           .globl _rotate           | [14]
	.globl	_rotate
|;;           .globl _scale            | [15]
	.globl	_scale
| 127 "vdi/vdi_tblit.S"
|;;           .extern _SOURCEX,_SOURCEY | [17]
	.extern	_SOURCEX
	.extern	_SOURCEY
|;;           .extern _LITEMASK,_SKEWMASK | [18]
	.extern	_LITEMASK
	.extern	_SKEWMASK
|;;           .extern _SCALDIR         | [19]
	.extern	_SCALDIR
|;;           .extern _XDDA            | [20]
	.extern	_XDDA
|;;           .extern _DDAINC          | [21]
	.extern	_DDAINC
|;;           .extern _SCRPT2          | [22]
	.extern	_SCRPT2
|;;           .extern _SCRTCHP         | [23]
	.extern	_SCRTCHP
| 145 "vdi/vdi_tblit.S"
|;;           .globl _deftxbuf         | [25]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [26]
	.globl	_scrtsiz
| 281 "vdi/vdi_tblit.S"
|;;           .bss                     | [28]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [30]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [32]
	.p2align 2,0
	.text

|;;           .even                    | [34]
	.p2align 1,0
_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [36]
	.short	(2+6+8+15)/16*2*16*2
| 396 "vdi/vdi_tblit.S"
|;;           .text                    | [38]






	.p2align 1,0
_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [46]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [47]
      movea.l  (44+8,%a7),%a0          | [48]
      movea.l  (44+12,%a7),%a1         | [49]
      jbsr     norm_blt                | [50]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [51]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [52]


_outline:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [56]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [57]
      movea.l  (44+8,%a7),%a0          | [58]
      move.w   (44+12,%a7),%d7         | [59]
      jbsr     outlin                  | [60]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [61]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [62]


_rotate:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [66]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [67]
      jbsr     rotation                | [68]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [69]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [70]


_scale:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [74]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [75]
      jbsr     replicat                | [76]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [77]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [78]


|;;           .text                    | [81]
| 448 "vdi/vdi_tblit.S"
|;;                   .even            | [83]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [84]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [85]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [86]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [87]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [88]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [89]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [90]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [91]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [92]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [93]
	.short	0x007F
|;;                   .dc.w 0x003F     | [94]
	.short	0x003F
|;;                   .dc.w 0x001F     | [95]
	.short	0x001F
|;;                   .dc.w 0x000F     | [96]
	.short	0x000F
|;;                   .dc.w 0x0007     | [97]
	.short	0x0007
|;;                   .dc.w 0x0003     | [98]
	.short	0x0003
|;;                   .dc.w 0x0001     | [99]
	.short	0x0001
|;;                   .dc.w 0x0000     | [100]
	.short	0x0000
| 492 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*3,4*3    | [103]
	.byte	4*0,4*0,4*3,4*3
|;;           .dc.b 4*4,4*4,4*7,4*7    | [104]
	.byte	4*4,4*4,4*7,4*7
|;;           .dc.b 4*6,4*6,4*6,4*6    | [105]
	.byte	4*6,4*6,4*6,4*6
|;;           .dc.b 4*1,4*1,4*13,4*13  | [106]
	.byte	4*1,4*1,4*13,4*13

|;;           .dc.b 4*0,4*15,4*0,4*15  | [108]
	.byte	4*0,4*15,4*0,4*15
|;;           .dc.b 4*0,4*14,4*1,4*15  | [109]
	.byte	4*0,4*14,4*1,4*15
|;;           .dc.b 4*0,4*13,4*2,4*15  | [110]
	.byte	4*0,4*13,4*2,4*15
|;;           .dc.b 4*0,4*12,4*3,4*15  | [111]
	.byte	4*0,4*12,4*3,4*15
|;;           .dc.b 4*0,4*11,4*4,4*15  | [112]
	.byte	4*0,4*11,4*4,4*15
|;;           .dc.b 4*0,4*10,4*5,4*15  | [113]
	.byte	4*0,4*10,4*5,4*15
|;;           .dc.b 4*0,4*9,4*6,4*15   | [114]
	.byte	4*0,4*9,4*6,4*15
|;;           .dc.b 4*0,4*8,4*7,4*15   | [115]
	.byte	4*0,4*8,4*7,4*15
|;;           .dc.b 4*0,4*7,4*8,4*15   | [116]
	.byte	4*0,4*7,4*8,4*15
|;;           .dc.b 4*0,4*6,4*9,4*15   | [117]
	.byte	4*0,4*6,4*9,4*15
|;;           .dc.b 4*0,4*5,4*10,4*15  | [118]
	.byte	4*0,4*5,4*10,4*15
|;;           .dc.b 4*0,4*4,4*11,4*15  | [119]
	.byte	4*0,4*4,4*11,4*15
|;;           .dc.b 4*0,4*3,4*12,4*15  | [120]
	.byte	4*0,4*3,4*12,4*15
|;;           .dc.b 4*0,4*2,4*13,4*15  | [121]
	.byte	4*0,4*2,4*13,4*15
|;;           .dc.b 4*0,4*1,4*14,4*15  | [122]
	.byte	4*0,4*1,4*14,4*15
|;;           .dc.b 4*0,4*0,4*15,4*15  | [123]
	.byte	4*0,4*0,4*15,4*15

toptable:
|;;           .dc.l top0,top1,top2,top3,top4,top5,top6,top7 | [126]
	.long	top0,top1,top2,top3,top4,top5,top6,top7
|;;           .dc.l top8,top9,topa,topb,topc,topd,tope,topf | [127]
	.long	top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
|;;           .dc.l twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7 | [130]
	.long	twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
|;;           .dc.l twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf | [131]
	.long	twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

|;;           .text                    | [133]
| 536 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [136]
|;;   sub.w     -24(a6),d1             | [137]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [138 PL]
      jpl      do_rot                  | [139]



|;;   neg.w     d1                     | [143]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [144]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [145]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [147]

|;;   add.w     d0,d0                  | [149]
	move.w   %d0,%d5
	add.l    %d0,%d5
      lea      lf_tab,%a2              | [150]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [151]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [152]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [153]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [158]
|;;   add.w     d0,d0                  | [159]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [160]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [162]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [163]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [166]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [167]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [168]
	andi.l   #(0xFFFF&0xF),%d5
|;;   add.w     d2,d2                  | [169]
	add.l    %d5,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [170]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [171]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [172]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [173]
      move.w   #0x8000,(-80,%a6)       | [174]
      moveq.l  #(-1),%d3               | [175]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [178 HI]
      jhi      doub_des                | [179]



|;;   add.w     d0,d0                  | [183]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [184]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [185]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [186]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [188]
	move.w   %d5,%d1
      jra      msk_done                | [189]



doub_des:
      move.w   %d0,%d1                 | [194]
|;;   lsr.w     #4,d1                  | [195]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [196]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [197 NE]
      jne      mlt_dest                | [198]



      move.w   (-24,%a6),%d1           | [202]
|;;   add.w     -20(a6),d1             | [203]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [204 CC]
      jcc      mlt_dest                | [205]
      moveq.l  #0,%d7                  | [206]
      jra      do_ritem                | [207]

mlt_dest:
      tst.w    (-42,%a6)               | [210 LT]
      jlt      mltleft                 | [211]
      moveq.l  #4,%d7                  | [212]
      jra      do_ritem                | [213]
mltleft:
      moveq.l  #8,%d7                  | [215]
do_ritem:
|;;   andi.w    #0x000F,d0             | [217 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [218]
|;;   subq.w    #1,d3                  | [219]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [220]
	move.w   %d5,%d3
not_null:
|;;   add.w     d0,d0                  | [222]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [223]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [224]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [227]
      move.w   %d3,(-28,%a6)           | [228]
      move.w   %d4,(-36,%a6)           | [229]
|;;   addq.w    #2,d3                  | [230]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [231 CS]
      jcs      msk0                    | [232]
|;;   addq.w    #1,d3                  | [233]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [235]
      move.w   %d3,(-30,%a6)           | [236]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5

plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [239]
|;;   lsr.w     -84(a6)                | [240 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   addx.w    d0,d0                  | [241]
	move.w   %d0,%d1
	addx.l   %d0,%d1
|;;   lsr.w     -76(a6)                | [242 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   addx.w    d0,d0                  | [243]
	addx.l   %d1,%d1



      lea      toptable,%a4            | [247]
	move.w   %d1,%d0
|;;   move.b    wrmappin-toptable(a4,d0.w),d0 | [248]
	move.w   %d0,%a5
	move.b   (wrmappin-toptable,%a4,%a5.l*1),%d0
|;;   movea.l   toptable-toptable(a4,d0.w),a2 | [249]
	move.w   %d0,%a5
	movea.l  (toptable-toptable,%a4,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [250]
      movea.l  %a2,%a5                 | [251]

|;;   movea.l   twoptble-toptable(a4,d0.w),a4 | [253]
	move.w   %d0,%a0
	movea.l  (twoptble-toptable,%a4,%a0.l*1),%a4



      move.w   (-96,%a6),%d0           | [257 EQ]
      jeq      wrt_char                | [258]

      btst     #1,%d0                  | [260 EQ]
      jeq      no_lite                 | [261]



|;;   move.w    _LITEMASK,-78(a6)      | [265]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [266]
      lea      liteop,%a5              | [267]

      move.l   %a2,(-64,%a6)           | [269]
      lea      liteopwf,%a2            | [270]

      move.l   %a4,(-72,%a6)           | [272]
      lea      liteopw,%a4             | [273]

no_lite:
      btst     #0,%d0                  | [276 EQ]
      jeq      no_thick                | [277]



      clr.w    (-82,%a6)               | [281]

      move.l   %a5,(-48,%a6)           | [283]
      lea      thknop,%a5              | [284]

      move.l   %a2,(-60,%a6)           | [286]
      lea      thknopwf,%a2            | [287]

      move.l   %a4,(-68,%a6)           | [289]
      lea      thknopw,%a4             | [290]

no_thick:
      btst     #2,%d0                  | [293 EQ]
      jeq      wrt_char                | [294]



|;;   move.w    _SKEWMASK,-80(a6)      | [298]
	move.w   _SKEWMASK,%d1
	move.w   %d1,(-80,%a6)
      move.l   %a5,(-56,%a6)           | [299]
      lea      skewop,%a5              | [300]

      tst.w    (-124,%a6)              | [302 EQ GT]
      jgt      wrt_char                | [303]
      jeq      not_sngl                | [304]
      clr.w    (-28,%a6)               | [305]
      moveq.l  #0,%d7                  | [306]
      jra      chng_blt                | [307]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [309 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [310]

      tst.w    (-42,%a6)               | [312 LT]
      jlt      chng_left               | [313]
      moveq.l  #4,%d7                  | [314]
      jra      chng_blt                | [315]
chng_left:
      moveq.l  #8,%d7                  | [317]
chng_blt:
      move.w   %d7,(-124,%a6)          | [319]

wrt_char:
      move.w   (-124,%a6),%d7          | [322 LE]
      jle      set_chr_hgt             | [323]
      movea.l  %a2,%a5                 | [324]
      move.w   (-18,%a6),%d5           | [325]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [327]
|;;   subq.w    #1,d3                  | [328]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [330]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [331]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [333 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [334]
      movea.l  (-10,%a6),%a0           | [335]
      movea.l  (-14,%a6),%a1           | [336]
      addq.l   #2,%a1                  | [337]
      move.l   %a1,(-14,%a6)           | [338]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [339 EQ]
      jeq      plane_loop              | [340]
      jra      norm_blt                | [341]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [343]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [345]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [351]
      move.w   (-16,%a6),%d6           | [352]
      move.w   (-18,%a6),%d7           | [353]
      lea      sgl_rtn,%a3             | [354]
      jra      sgl_1st_scan            | [355]

sgl_lp:
|;;   adda.w    d6,a0                  | [358]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [359]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [361]
| 771 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [363]


      move.w   (-42,%a6),%d1           | [366 MI]
      jmi      left_rol                | [367]
      lsr.l    %d1,%d0                 | [368]
      jra      end_ro                  | [369]
left_rol:
      lsl.l    %d1,%d0                 | [371]
end_ro:
      swap     %d0                     | [373]
      move.w   %d0,%d1                 | [374]
      swap     %d0                     | [375]

      jmp      (%a5)                   | [377]

sgl_rtn:
      move.w   %d1,(%a1)               | [380]
|;;   dbra      d3,sgl_lp              | [381]
	tst.w    %d3
	jeq      _PA_380_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_380_:         
      jra      next_plane              | [382]




dbl_loop:
      movea.l  %a3,%a4                 | [388]
      move.w   (-16,%a6),%d6           | [389]
      move.w   (-18,%a6),%d7           | [390]
      lea      dbl_2wrd,%a2            | [391]
      move.l   %a2,%d5                 | [392]
      lea      dbl_1wrd,%a3            | [393]
      jra      dbl_1st_scan            | [394]

dbl_lp:
|;;   adda.w    d6,a0                  | [397]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [398]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [399]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [401]
| 818 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [403]


      move.w   (-42,%a6),%d1           | [406 MI]
      jmi      dbleft                  | [407]
      lsr.l    %d1,%d0                 | [408]
      jra      dblendr                 | [409]
dbleft:
      lsl.l    %d1,%d0                 | [411]
dblendr:
      swap     %d0                     | [413]
      move.w   %d0,%d1                 | [414]
      swap     %d0                     | [415]
      move.w   (-38,%a6),%d2           | [416]

      jmp      (%a5)                   | [418]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [421]
      move.w   %d0,%d1                 | [422]

      movea.w  (-88,%a6),%a2           | [424]
|;;   move.w    (a1,a2.w),d4           | [425]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [427]

|;;   exg       a3,d5                  | [429]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [430]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [433]
|;;   move.w    d1,(a1,a2.w)           | [434]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [436]
	tst.w    %d3
	jeq      _PA_435_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_435_:         
      jra      next_plane              | [437]





left_loop:
      movea.l  %d6,%a0                 | [444]
      movea.l  %d7,%a1                 | [445]
|;;   adda.w    -16(a6),a0             | [446]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [447]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [448]
|;;   rol.w     -78(a6)                | [449]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [451 NE]
      jne      skewopw                 | [452]
mlt_left:
      move.l   %a0,%d6                 | [454]
      move.l   %a1,%d7                 | [455]
      move.w   (-28,%a6),(-44,%a6)     | [456]







      move.l   (%a0)+,%d0              | [464]


      move.w   (%a1),%d4               | [467]

      move.w   (-42,%a6),%d1           | [469]
      lsl.l    %d1,%d0                 | [470]

      swap     %d0                     | [472]
      move.w   %d0,%d1                 | [473]
      swap     %d0                     | [474]

      move.w   (-38,%a6),%d2           | [476]

      lea      lft_lfrng,%a3           | [478]
      jmp      (%a5)                   | [479]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [482]
|;;   adda.w    -88(a6),a1             | [483]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [485]
      lea      lft_wrds,%a3            | [486]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [491]
|;;   and.w     d2,d4                  | [492]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [493]





      swap     %d0                     | [499]
      move.w   %d1,%d0                 | [500]
      swap     %d0                     | [501]

      move.w   (-42,%a6),%d1           | [503]
      lsl.l    %d1,%d0                 | [504]

      swap     %d0                     | [506]
      move.w   %d0,%d1                 | [507]
      swap     %d0                     | [508]

|;;   not.w     d2                     | [510]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [511]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [512]
	not.l    %d5
|;;   eor.w     d4,d1                  | [513]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [515]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [517 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [518]

      jmp      (%a4)                   | [520]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [523]
|;;   adda.w    -88(a6),a1             | [524]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [526]

lef_don:
      move.w   (-36,%a6),%d2           | [529]

      lea      lft_rfrng,%a3           | [531]
      jmp      (%a5)                   | [532]

lft_rfrng:
      move.w   %d1,(%a1)               | [535]

|;;   dbra      d3,left_loop           | [537]
	tst.w    %d3
	jeq      _PA_536_
	subq.l   #1,%d3
	jra      left_loop
	_PA_536_:         
      jra      next_plane              | [538]





rite_loop:
      movea.l  %d6,%a0                 | [545]
      movea.l  %d7,%a1                 | [546]
|;;   adda.w    -16(a6),a0             | [547]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [548]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [549]
|;;   rol.w     -78(a6)                | [550]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [552 NE]
      jne      skewopw                 | [553]
mlt_rite:
      move.l   %a0,%d6                 | [555]
      move.l   %a1,%d7                 | [556]
      move.w   (-28,%a6),(-44,%a6)     | [557]
      move.w   (%a0)+,%d0              | [558]



      swap     %d0                     | [562]

      move.w   (%a1),%d4               | [564]

      move.w   (-42,%a6),%d1           | [566]
      lsr.l    %d1,%d0                 | [567]
      swap     %d0                     | [568]
      move.w   %d0,%d1                 | [569]
      swap     %d0                     | [570]

      move.w   (-38,%a6),%d2           | [572]

      lea      rgt_lfrng,%a3           | [574]
      jmp      (%a5)                   | [575]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [578]
|;;   adda.w    -88(a6),a1             | [579]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [581]
      lea      rgt_wrds,%a3            | [582]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [587]
|;;   and.w     d2,d4                  | [588]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [589]
      move.w   (%a0)+,%d0              | [590]



      swap     %d0                     | [594]

      move.w   (-42,%a6),%d1           | [596]
      lsr.l    %d1,%d0                 | [597]
      swap     %d0                     | [598]
      move.w   %d0,%d1                 | [599]
      swap     %d0                     | [600]

|;;   not.w     d2                     | [602]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [603]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [604]
	not.l    %d5
|;;   eor.w     d4,d1                  | [605]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [607]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [609 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [610]

      jmp      (%a4)                   | [612]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [615]
|;;   adda.w    -88(a6),a1             | [616]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [618]

rite_don:
      move.w   (-36,%a6),%d2           | [621]

      lea      rgt_rfrng,%a3           | [623]
      jmp      (%a5)                   | [624]

rgt_rfrng:
      move.w   %d1,(%a1)               | [627]

|;;   dbra      d3,rite_loop           | [629]
	tst.w    %d3
	jeq      _PA_628_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_628_:         
      jra      next_plane              | [630]
| 1060 "vdi/vdi_tblit.S"
top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [633]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [634]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [635]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [636]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [637]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [640]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [641]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [642]
	not.l    %d0
|;;   and.w     d4,d1                  | [643]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [644]

top2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [647]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [648]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [649]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [650]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [651]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [654]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [655]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [656]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [657]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [660]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [661]
	not.l    %d0
|;;   and.w     d4,d1                  | [662]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [663]

top5:
      move.w   %d4,%d1                 | [666]
      jmp      (%a3)                   | [667]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [670]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [671]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [672]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [675]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [676]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [677]

top8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [680]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [681]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [682]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [683]

top9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [686]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [687]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [688]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [689]

topa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [692]
|;;   eor.w     d2,d1                  | [693]
	move.w   %d1,%d0
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [694]

topb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d2,d4                  | [697]
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   and.w     d2,d1                  | [698]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [699]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [700]

topc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [703]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [704]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [705]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [706]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [707]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [710]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [711]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [712]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [713]

tope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [716]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [717]
	and.l    %d2,%d0
|;;   eor.w     d2,d4                  | [718]
	move.w   %d0,%d1
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   or.w      d4,d1                  | [719]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [720]

topf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d2,d4                  | [723]
	mvz.w    %d2,%d0
	or.l     %d0,%d4
      move.w   %d4,%d1                 | [724]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [725]
| 1166 "vdi/vdi_tblit.S"
twop0:
      moveq.l  #0,%d1                  | [728]
      jmp      (%a3)                   | [729]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [732]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [733]

twop2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [736]
	move.w   %d4,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [737]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [738]

twop3:
      jmp      (%a3)                   | [741]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [744]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [745]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [746]

twop5:
      move.w   %d4,%d1                 | [749]
      jmp      (%a3)                   | [750]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [753]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [754]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [757]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [758]

twop8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [761]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|;;   not.w     d1                     | [762]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [763]

twop9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [766]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   not.w     d1                     | [767]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [768]

twopa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [771]
|;;   not.w     d1                     | [772]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [773]

twopb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [776]
	move.w   %d4,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [777]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [778]

twopc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [781]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [782]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [785]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [786]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [787]

twope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [790]
	move.w   %d1,%d0
	and.l    %d4,%d0
|;;   not.w     d1                     | [791]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [792]

twopf:
      moveq.l  #(-1),%d1               | [795]
      jmp      (%a3)                   | [796]
| 1254 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [799]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [800]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [801]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [802 EQ]
      jeq      thk00                   | [803]
|;;   ori.b     #0x10,ccr              | [804 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [805 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [806]
      move.w   #0x8000,%d6             | [807]
thk01:
      move.w   %d6,(-34,%a6)           | [809]
thk00:
      tst.w    (-28,%a6)               | [811 MI]
      jmi      thk0                    | [812]
      cmp.w    (-36,%a6),%d6           | [813 CC]
      jcc      thk0                    | [814]
|;;   and.w     d6,d0                  | [815]
	and.l    %d6,%d0
      jra      thk1                    | [816]
thk0:
      clr.w    %d0                     | [818]
|;;   and.w     d6,d1                  | [819]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [821]
      move.w   %d2,%d5                 | [822]
      swap     %d5                     | [823]
      move.w   (-36,%a6),%d5           | [824]
      lsl.l    %d6,%d5                 | [825]

      swap     %d5                     | [827]
      move.w   %d5,%d7                 | [828]
      swap     %d5                     | [829]

|;;   and.w     d7,d1                  | [831]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [832]
	and.l    %d5,%d0
|;;   swap      d1                     | [833]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [834]
|;;   subq.w    #1,d6                  | [835]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [837]
      lsr.l    #1,%d0                  | [838]
      or.l     %d0,%d1                 | [839]
|;;   dbra      d6,thkoplp             | [840]
	tst.w    %d6
	jeq      _PA_839_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_839_:         

      move.w   %d1,%d0                 | [842]
      swap     %d1                     | [843]
|;;   movem.l   (sp)+,d5-d7            | [844]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [845]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [846]
| 1317 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [849]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [850 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [851]
|;;   and.w     -34(a6),d1             | [852]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [854]
      swap     %d1                     | [855]
      clr.w    %d1                     | [856]
      move.l   %d1,%d5                 | [857]
|;;   subq.w    #1,d6                  | [858]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [860]
      or.l     %d5,%d1                 | [861]
|;;   dbra      d6,thkopwlp            | [862]
	tst.w    %d6
	jeq      _PA_861_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_861_:         

      move.w   %d1,%d5                 | [864]
      swap     %d1                     | [865]
|;;   or.w      -82(a6),d1             | [866]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [867]
|;;   movem.l   (sp)+,d5-d6            | [868]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [869]
      jmp      (%a2)                   | [870]
| 1355 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [873]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [874]
|;;   subq.w    #1,-30(a6)             | [875 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [876]
      jeq      thk4                    | [877]

|;;   subq.w    #1,-30(a6)             | [879 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [880]
|;;   and.w     d6,d1                  | [881]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [883]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [884]

thk3:
      clr.w    %d1                     | [887]
      jra      thk5                    | [888]

thk4:
|;;   and.w     d6,d1                  | [891]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [894 MI]
      jmi      thk6                    | [895]
      move.w   (-28,%a6),%d5           | [896]
|;;   addq.w    #2,d5                  | [897]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [898]
|;;   ori.b     #0x10,ccr              | [899 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [900 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [901]
      move.w   #0x8000,%d7             | [902]
|;;   addq.w    #1,d5                  | [903]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [905 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [906]
|;;   subq.w    #1,d5                  | [907]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [909 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [910 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [911]
      move.w   #0x8000,%d6             | [912]
thk31:
      cmp.w    %d6,%d7                 | [914 CS]
      jcs      thk35                   | [915]
|;;   addq.w    #1,d5                  | [916]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [918]
      move.w   %d5,(-32,%a6)           | [919]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [921]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [923]
      swap     %d1                     | [924]
      clr.w    %d1                     | [925]
      move.l   %d1,%d5                 | [926]
|;;   subq.w    #1,d6                  | [927]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [929]
      or.l     %d5,%d1                 | [930]
|;;   dbra      d6,thkopwl             | [931]
	tst.w    %d6
	jeq      _PA_930_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_930_:         

      move.w   %d1,%d5                 | [933]
      swap     %d1                     | [934]
|;;   or.w      -82(a6),d1             | [935]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [936]
|;;   and.w     d2,d1                  | [937]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [938]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [939]
      jmp      (%a2)                   | [940]
| 1436 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [943]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [944]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [945]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [946]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [947]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [950]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [951]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [952]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [955]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [956]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [957]
| 1468 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [960 CC]
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
      jcc      no_shift                | [961]

      swap     %d1                     | [963]
      move.w   %d0,%d1                 | [964]
      lsr.l    #1,%d1                  | [965]

      swap     %d2                     | [967]
      move.w   (-36,%a6),%d2           | [968]
      lsr.l    #1,%d2                  | [969]
      move.w   %d2,(-36,%a6)           | [970]
      swap     %d2                     | [971]
      move.w   %d2,(-38,%a6)           | [972 EQ]
      jeq      nxt_word                | [973]
      move.w   (-42,%a6),%d0           | [974 MI]
      jmi      dec_rol                 | [975]
ror_add:
|;;   addq.w    #1,d0                  | [977]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [979]
      move.w   %d1,%d0                 | [980]
      swap     %d1                     | [981]
no_shift:
      movea.l  (-56,%a6),%a2           | [983]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [984]

dec_rol:
      tst.b    %d0                     | [987 EQ]
      jeq      begn_ror                | [988]
|;;   subq.w    #1,d0                  | [989]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [990]
begn_ror:
      clr.w    %d0                     | [992]
      jra      ror_add                 | [993]



nxt_word:
      move.w   %d2,(-36,%a6)           | [998]
      swap     %d2                     | [999]
      move.w   %d2,(-38,%a6)           | [1000]
|;;   adda.w    -88(a6),a1             | [1001]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1002]

      moveq.l  #15,%d0                 | [1004]
|;;   sub.w     -42(a6),d0             | [1005]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1006]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1007]
      jra      no_shift                | [1008]






skewopw:
|;;   rol.w     -80(a6)                | [1016 CC]
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
      jcc      do_shift                | [1017]

      tst.w    (-42,%a6)               | [1019 MI]
      jmi      mlt_left                | [1020]
      jra      mlt_rite                | [1021]
do_shift:
|;;   ori       #0x10,ccr              | [1023 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1024]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1026]
      cmp.w    #0xFFFF,%d0             | [1027 EQ]
      jeq      inc_rite                | [1028]

|;;   ori       #0x10,ccr              | [1030 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1031]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1032]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1035]
      tst.b    %d0                     | [1036 NE]
      jne      no_rota                 | [1037]
      move.w   #0x8000,(-40,%a6)       | [1038]
no_rota:
      move.w   (-38,%a6),%d1           | [1040]
|;;   lsr.w     #1,d1                  | [1041 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [1042]

      move.w   %d1,(-38,%a6)           | [1044]

      tst.w    %d0                     | [1046 MI]
      jmi      wdec_rol                | [1047]
|;;   addq.w    #1,-42(a6)             | [1048]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [1049]
wdec_rol:
      tst.b    %d0                     | [1051 EQ]
      jeq      set_msk                 | [1052]
|;;   subq.w    #1,-42(a6)             | [1053]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [1054]
set_msk:
      move.w   #1,(-42,%a6)            | [1056]
      jra      mlt_rite                | [1057]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1060]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1061]
      jra      do_left                 | [1062]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1065]

|;;   subq.w    #1,-28(a6)             | [1067]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1068]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1070]
|;;   sub.w     d0,d2                  | [1071]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1072]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1073]

      jra      mlt_left                | [1075]




rotation:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   _SOURCEX,%d1            | [1081]
      move.w   %d1,%d2                 | [1082]
|;;   andi.w    #0x0F,d2               | [1083]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1084]
|;;   lsr.w     #4,d1                  | [1085]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1086]
	add.l    %d1,%d1

      movea.l  (-10,%a6),%a0           | [1088]
|;;   adda.w    d1,a0                  | [1089]
	mvs.w    %d1,%d0
	adda.l   %d0,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1091]
      move.w   _SOURCEY,%d0            | [1092]
      move.w   (-106,%a6),%d1          | [1093]
      move.w   %d1,(-22,%a6)           | [1094]
      move.w   (-16,%a6),%d2           | [1095]

      tst.w    (-126,%a6)              | [1097 EQ LT]
      jeq      upsd_dwn                | [1098]
rot90:
      jlt      top_src                 | [1100]
|;;   neg.w     -16(a6)                | [1101]
	move.w   (-16,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-16,%a6)
|;;   subq.w    #1,d1                  | [1102]
	subq.l   #1,%d1
|;;   add.w     d1,d0                  | [1103]
	add.l    %d1,%d0
      mulu.w   %d2,%d0                 | [1104]
      adda.l   %d0,%a0                 | [1105]
top_src:
      move.w   (-106,%a6),%d0          | [1107]
|;;   lsr.w     #4,d0                  | [1108]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1109]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1110]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1111]
      movea.l  _SCRTCHP,%a1            | [1112]
|;;   adda.w    -4(a6),a1              | [1113]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1

      tst.w    (-126,%a6)              | [1115 GT]
      jgt      top_dwn                 | [1116]
|;;   neg.w     -18(a6)                | [1117]
	move.w   (-18,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1118]
|;;   subq.w    #1,d1                  | [1119]
	subq.l   #1,%d1
      mulu.w   %d1,%d0                 | [1120]
      adda.l   %d0,%a1                 | [1121]
top_dwn:
      move.w   (-24,%a6),%d2           | [1123]
      move.w   #0x8000,%d3             | [1124]
      move.w   %d3,%d4                 | [1125]
      movea.w  %d3,%a4                 | [1126]
|;;   lsr.w     d2,d3                  | [1127]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3
      moveq.l  #0,%d0                  | [1128]
      move.w   (-20,%a6),%d2           | [1129]
      move.w   (-16,%a6),%d6           | [1130]
      movea.l  %a0,%a2                 | [1131]
      movea.l  %a1,%a3                 | [1132]
      jra      rot_nsrc                | [1133]
rot_ylp:
      move.w   (-22,%a6),%d1           | [1135]
      jra      rot_srt                 | [1136]
rot_xlp:
      move.w   (%a0),%d7               | [1138]
|;;   and.w     d3,d7                  | [1139 EQ]
	and.l    %d3,%d7
	tst.w    %d7
      jeq      rot_nor                 | [1140]
|;;   or.w      d4,d0                  | [1141]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     #1,d4                  | [1143 CC]
	move.w   %d4,%d5
	swap     %d4
	move.w   %d5,%d4
	lsr.l    #1,%d4
      jcc      rot_isrc                | [1144]
rot_ndst:
      move.w   %d0,(%a1)+              | [1146]
      moveq.l  #0,%d0                  | [1147]
rot_isrc:
|;;   adda.w    d6,a0                  | [1149]
	mvs.w    %d6,%d5
	adda.l   %d5,%a0
rot_srt:
|;;   dbra      d1,rot_xlp             | [1151]
	tst.w    %d1
	jeq      _PA_1150_
	subq.l   #1,%d1
	jra      rot_xlp
	_PA_1150_:         

      move.w   %d0,(%a1)               | [1153]
      moveq.l  #0,%d0                  | [1154]
|;;   adda.w    -18(a6),a3             | [1155]
	mvs.w    (-18,%a6),%d5
	adda.l   %d5,%a3
      movea.l  %a3,%a1                 | [1156]
      move.w   %a4,%d4                 | [1157]
|;;   ror.w     #1,d3                  | [1158 CC]
	move.w   %d3,%d5
	swap     %d3
	move.w   %d5,%d3
	lsr.l    #1,%d3
      jcc      rnew_src                | [1159]
      addq.l   #2,%a2                  | [1160]
rnew_src:
      movea.l  %a2,%a0                 | [1162]
rot_nsrc:
|;;   dbra      d2,rot_ylp             | [1164]
	tst.w    %d2
	jeq      _PA_1163_
	subq.l   #1,%d2
	jra      rot_ylp
	_PA_1163_:         

rot_done:
      move.w   (-102,%a6),%d0          | [1167]
      move.w   (-106,%a6),%d1          | [1168]
      move.w   %d1,(-20,%a6)           | [1169]
      move.w   %d1,(-102,%a6)          | [1170]
      move.w   %d0,(-22,%a6)           | [1171]
      move.w   %d0,(-106,%a6)          | [1172]

      move.w   (-92,%a6),%d0           | [1174]
      move.w   (-90,%a6),(-92,%a6)     | [1175]
      move.w   %d0,(-90,%a6)           | [1176]
      move.w   #1,(-94,%a6)            | [1177]

      move.w   (-18,%a6),%d0           | [1179]
      tst.w    (-126,%a6)              | [1180 GT]
      jgt      rot_nneg                | [1181]
|;;   neg.w     d0                     | [1182]
	neg.l    %d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1184]
      move.w   (-4,%a6),%d0            | [1185]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5
repexit:
      clr.w    _SOURCEX                | [1187]
repexit3:
      movea.l  _SCRTCHP,%a2            | [1189]
|;;   adda.w    d0,a2                  | [1190]
	ext.l    %d0
	adda.l   %d0,%a2
      move.l   %a2,(-10,%a6)           | [1191]
      clr.w    _SOURCEY                | [1192]
      rts                              | [1193]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1196]
|;;   add.w     -24(a6),d0             | [1197]
	move.w   (-24,%a6),%d5
	add.l    %d5,%d0
|;;   subq.w    #1,d0                  | [1198]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1199]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1200]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1201]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1203]
      move.w   %d0,%d2                 | [1204]
|;;   lsr.w     #1,d2                  | [1205]
	lsr.l    #1,%d2
|;;   subq.w    #1,d2                  | [1206]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1207]
|;;   add.w     -4(a6),d0              | [1208]
	move.w   (-4,%a6),%d5
	add.l    %d5,%d0
      movea.l  _SCRTCHP,%a1            | [1209]
|;;   adda.w    d0,a1                  | [1210]
	ext.l    %d0
	adda.l   %d0,%a1
      jra      strtflip                | [1211]
upsd_lp:
      movea.l  %a0,%a2                 | [1213]
      move.w   %d2,%d3                 | [1214]
line_lp:
      move.w   (%a2)+,%d0              | [1216]
      moveq.l  #0,%d6                  | [1217]
      moveq.l  #15,%d4                 | [1218]
flip_lp:
|;;   lsr.w     #1,d0                  | [1220 X]
	and.l    #0xffff,%d0
	lsr.l    #1,%d0
|;;   addx.w    d6,d6                  | [1221]
	addx.l   %d6,%d6
|;;   dbra      d4,flip_lp             | [1222]
	subq.l   #1,%d4
	jpl      flip_lp

      move.w   %d6,-(%a1)              | [1224]
|;;   dbra      d3,line_lp             | [1225]
	tst.w    %d3
	jeq      _PA_1224_
	subq.l   #1,%d3
	jra      line_lp
	_PA_1224_:         

|;;   adda.w    -16(a6),a0             | [1227]
	mvs.w    (-16,%a6),%d5
	adda.l   %d5,%a0
strtflip:
|;;   dbra      d1,upsd_lp             | [1229]
	tst.w    %d1
	jeq      _PA_1228_
	subq.l   #1,%d1
	jra      upsd_lp
	_PA_1228_:         

      move.w   (-18,%a6),(-16,%a6)     | [1231]
      movea.l  _SCRTCHP,%a1            | [1232]
|;;   adda.w    -4(a6),a1              | [1233]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1234]
      move.w   _SOURCEX,%d0            | [1235]
|;;   add.w     -102(a6),d0            | [1236]
	move.w   (-102,%a6),%d5
	add.l    %d5,%d0
|;;   neg.w     d0                     | [1237]
	neg.l    %d0
|;;   andi.w    #0xF,d0                | [1238]
	andi.l   #(0xFFFF&0xF),%d0
      move.w   %d0,_SOURCEX            | [1239]
      clr.w    _SOURCEY                | [1240]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      rts                              | [1241]






replicat:
|***	SAVE	d7	***
	lea      -12(%a7),%a7
	movem.l  %d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1249]
      move.w   %d0,%d4                 | [1250]
|;;   andi.w    #0x0F,d0               | [1251]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1252]
|;;   lsr.w     #4,d4                  | [1253]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   add.w     d4,d4                  | [1254]
	add.l    %d4,%d4

      move.w   _SOURCEY,%d0            | [1256]
      mulu.w   (-16,%a6),%d0           | [1257]
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1258]
|;;   adda.w    d4,a0                  | [1259]
	mvs.w    %d4,%d3
	adda.l   %d3,%a0
      adda.l   %d0,%a0                 | [1260]

      move.w   (-24,%a6),%d2           | [1262]
      move.w   #0x8000,%d3             | [1263]
      move.w   %d3,%d4                 | [1264]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1265]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1267]
      move.w   (-102,%a6),%d1          | [1268]

      move.w   %d2,(-22,%a6)           | [1270]
      move.w   %d1,(-20,%a6)           | [1271]
      movea.l  _SCRTCHP,%a1            | [1272]
|;;   adda.w    _SCRPT2,a1             | [1273]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1275 EQ]
      jeq      noline                  | [1276]



|;;   addq.w    #1,-106(a6)            | [1280]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1281]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   add.w     d1,d1                  | [1283]
	add.l    %d1,%d1
|;;   addq.w    #3,d1                  | [1284]
	addq.l   #3,%d1
|;;   lsr.w     #4,d1                  | [1285]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1286]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1287]
	addq.l   #2,%d1
      move.w   %d1,%d5                 | [1288]
      moveq.l  #0,%d6                  | [1289]
oklear:
      move.w   %d6,(%a1)+              | [1291]
|;;   dbra      d5,oklear              | [1292]
	tst.w    %d5
	jeq      _PA_1291_
	subq.l   #1,%d5
	jra      oklear
	_PA_1291_:         
      lea      (-2,%a1),%a2            | [1293]
      move.w   %d2,%d5                 | [1294]
|;;   add.w     d5,d5                  | [1295]
	add.l    %d5,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1297]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1298]
|;;   dbra      d5,oklear1             | [1299]
	tst.w    %d5
	jeq      _PA_1298_
	subq.l   #1,%d5
	jra      oklear1
	_PA_1298_:         

|;;   move.w    d1,d5                  | [1301]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1302]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   subq.w    #1,d5                  | [1303 X]
	subq.l   #1,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1305]
|;;   dbra      d5,oklear3             | [1306]
	move     %CCR,%d0
	tst.w    %d5
	jeq      _PA_1305_
	subq.l   #1,%d5
	move     %d0,%CCR
	jra      oklear3
	_PA_1305_:         
	move     %d0,%CCR
      jra      noline1                 | [1307]

noline:
|;;   lsr.w     #3,d1                  | [1310]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   add.w     d1,d1                  | [1311]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1312 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1314]
|***	RESTORE	AFTER d7	***
	movem.l  8(%a7),%d7



      move.w   _SCALDIR,%d7            | [1318]
|;;   roxr.l    #1,d7                  | [1319 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1320]
|;;   roxr.l    #1,d7                  | [1321]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1323]
      move.w   _DDAINC,%d2             | [1324]
      move.w   (-22,%a6),%d5           | [1325]
|;;   subq.w    #1,d5                  | [1326]
	subq.l   #1,%d5
      move.w   #32767,%d6              | [1327]
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      12(%a7),%a7
      tst.l    %d7                     | [1328 MI]
      jmi      rep_ylop                | [1329]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1331 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1332]
      jbsr     yloop                   | [1333]
y_no_drw:
|;;   adda.w    d7,a0                  | [1335]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1336]
	tst.w    %d5
	jeq      _PA_1335_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1335_:         
      jra      y_rep_don               | [1337]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1339 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1340]
      jbsr     yloop                   | [1341]
y_no_rep:
      jbsr     yloop                   | [1343]
|;;   adda.w    d7,a0                  | [1344]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1345]
	tst.w    %d5
	jeq      _PA_1344_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1344_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1347]
|;;   subq.w    #1,d2                  | [1348]
	subq.l   #1,%d2
      move.w   _XDDA,%d1               | [1349]
      move.w   _DDAINC,%d0             | [1350]
      moveq.l  #0,%d3                  | [1351]
repwidcl:
|;;   add.w     d0,d1                  | [1353 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1354]
|;;   addq.w    #1,d3                  | [1355]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1357 PL]
      jpl      nrpndoub                | [1358]
|;;   addq.w    #1,d3                  | [1359]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1361]
	tst.w    %d2
	jeq      _PA_1360_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1360_:         
      move.w   %d1,_XDDA               | [1362]
      move.w   %d3,(-102,%a6)          | [1363]
      move.w   (-92,%a6),(-106,%a6)    | [1364]


      move.w   (-18,%a6),%d1           | [1367]
      move.w   %d1,(-16,%a6)           | [1368]







repexit1:
      btst     #4,(-96+1,%a6)          | [1377 EQ]
      jeq      repexit2                | [1378]
      movea.l  _SCRTCHP,%a0            | [1379]
|;;   adda.w    _SCRPT2,a0             | [1380]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1381]
|;;   lea       2(a0,d7.w),a1          | [1382]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
      jbsr     outlin1                 | [1383]
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   _SCRPT2,%d0             | [1384]
|;;   add.w     -18(a6),d0             | [1385]
	move.w   (-18,%a6),%d1
	add.l    %d1,%d0
|;;   move.w    #15,_SOURCEX           | [1386]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      jra      repexit3                | [1387]
repexit2:
      move.w   _SCRPT2,%d0             | [1389]
      jra      repexit                 | [1390]
| 1930 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1393]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1394]
      movea.l  %a1,%a4                 | [1395]
      movea.l  %d7,%a5                 | [1396]
      move.w   %d5,(-44,%a6)           | [1397]
      moveq.l  #0,%d0                  | [1398]
      move.w   (-20,%a6),%d5           | [1399]
|;;   subq.w    #1,d5                  | [1400]
	subq.l   #1,%d5

      move.w   _XDDA,%d7               | [1402]
      jra      nextsrc                 | [1403]
innerlp:
|;;   ror.w     #1,d3                  | [1405 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1406]
nextsrc:
      movea.w  (%a0)+,%a2              | [1408]
reploop:
      move.w   %a2,%d6                 | [1410]
|;;   and.w     d3,d6                  | [1411 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1412]


repnor:
      tst.l    %d7                     | [1416 MI]
      jmi      repnorup                | [1417]
|;;   add.w     d2,d7                  | [1418 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1419]
      jra      ordone                  | [1420]
repnorup:
|;;   add.w     d2,d7                  | [1422 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1423]


|;;   ror.w     #1,d4                  | [1426 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1429]
      move.w   %d0,(%a1)+              | [1430]
      moveq.l  #0,%d0                  | [1431]


      jra      ordone                  | [1434]
nrepnor:


|;;   add.w     d2,d7                  | [1438 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1439]


|;;   or.w      d4,d0                  | [1442]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1443 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1445]
      move.w   %d0,(%a1)+              | [1446]
      moveq.l  #0,%d0                  | [1447]
o_no_rep:
      tst.l    %d7                     | [1449 PL]
      jpl      incsrc                  | [1450]


|;;   or.w      d4,d0                  | [1453]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1455 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1456]
nextdst:
      move.w   %d0,(%a1)+              | [1458]
      moveq.l  #0,%d0                  | [1459]
incsrc:
|;;   dbra      d5,innerlp             | [1461]
	tst.w    %d5
	jeq      _PA_1460_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1460_:         
repdone:
      move.w   %d0,(%a1)               | [1463]
|;;   movem.l   (sp)+,d3-d4/d6         | [1464]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1465]
      movea.l  %a4,%a1                 | [1466]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1467]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1468]
      move.w   (-44,%a6),%d5           | [1469]
      rts                              | [1470]
| 2018 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1473]
	ext.l    %d7
	lea      (0,%a0,%d7.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1475]
	ext.l    %d7
	lea      (0,%a1,%d7.l*1),%a2
|;;   lsr.w     #1,d7                  | [1476]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
|;;   subq.w    #1,d7                  | [1477]
	subq.l   #1,%d7
      move.w   %d7,(-44,%a6)           | [1478]
      move.w   (-106,%a6),%d6          | [1479]

      movea.l  %a0,%a5                 | [1481]
|;;   subq.w    #1,d6                  | [1482]
	subq.l   #1,%d6
out_edge:
      movea.w  %d6,%a4                 | [1484]
      movea.l  %a2,%a3                 | [1485]
      moveq.l  #0,%d5                  | [1486]
      moveq.l  #0,%d6                  | [1487]
      move.l   (%a2),%d1               | [1488]
      lsr.l    #1,%d1                  | [1489]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1494]
      move.b   %d5,%d0                 | [1495]
|;;   ror.l     #1,d0                  | [1496]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1498]
      move.b   %d6,%d2                 | [1499]
      move.l   %d2,%d3                 | [1500]
|;;   ror.l     #1,d3                  | [1501]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1502]
|;;   ror.l     #1,d4                  | [1503]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1505]
      move.l   %d0,%d6                 | [1506]
      eor.l    %d2,%d0                 | [1507]
      eor.l    %d3,%d5                 | [1508]
      eor.l    %d4,%d6                 | [1509]
|;;   rol.l     #1,d5                  | [1510]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1511]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1512]
      or.l     %d6,%d0                 | [1513]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1515]
      move.l   %d1,%d6                 | [1516]
      eor.l    %d2,%d1                 | [1517]
      eor.l    %d3,%d5                 | [1518]
      eor.l    %d4,%d6                 | [1519]
|;;   rol.l     #1,d5                  | [1520]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1521]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1522]
      or.l     %d5,%d0                 | [1523]
      or.l     %d6,%d0                 | [1524]

      eor.l    %d3,%d2                 | [1526]
      eor.l    %d3,%d4                 | [1527]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1528]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1529]
      or.l     %d4,%d0                 | [1530]
      swap     %d0                     | [1531]

      move.w   (%a1),%d6               | [1533]
      move.w   %d6,%d5                 | [1534]
|;;   eor.w     d0,d5                  | [1535]
	eor.l    %d0,%d5
|;;   and.w     d0,d5                  | [1536]
	and.l    %d0,%d5

      addq.l   #2,%a2                  | [1538]
      move.l   (%a2),%d1               | [1539]
      move.b   (-1,%a2),%d1            | [1540]
|;;   ror.l     #1,d1                  | [1541]
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1543]
      move.w   (%a0),%d5               | [1544]
      move.w   %d6,(%a0)+              | [1545]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1547]
	tst.w    %d7
	jeq      _PA_1546_
	subq.l   #1,%d7
	jra      out_loop
	_PA_1546_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1549]
      movea.l  %a5,%a0                 | [1550]
      move.w   %a4,%d6                 | [1551]
      movea.l  %a3,%a2                 | [1552]
      movea.l  %a2,%a1                 | [1553]
|;;   adda.w    -16(a6),a2             | [1554]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1555 NE]
      jne      srt_lin                 | [1556]
      movea.l  %a1,%a2                 | [1557]
srt_lin:
|;;   dbra      d6,out_edge            | [1559]
	tst.w    %d6
	jeq      _PA_1558_
	subq.l   #1,%d6
	jra      out_edge
	_PA_1558_:         
      rts                              | [1560]

| End section on aligned boundary
	.p2align 2,0
