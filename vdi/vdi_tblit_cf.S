
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
| Translated on Sun Jun 23 23:30:50 2019 by version 2.6.0 of PortAsm translator
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
| 12 "vdi/vdi_tblit.S"
| 1 "include/asmdefs.h" 1
| 20 "include/asmdefs.h"
| 1 "include/config.h" 1
| 21 "include/asmdefs.h" 2
| 13 "vdi/vdi_tblit.S" 2
| 101 "vdi/vdi_tblit.S"
|;;           .globl _normal_blit      | [12]
	.globl	_normal_blit
|;;           .globl _outline          | [13]
	.globl	_outline
|;;           .globl _rotate           | [14]
	.globl	_rotate
|;;           .globl _scale            | [15]
	.globl	_scale





|;;           .extern _SOURCEX         | [21]
	.extern	_SOURCEX
|;;           .extern _SOURCEY         | [22]
	.extern	_SOURCEY
|;;           .extern _LITEMASK        | [23]
	.extern	_LITEMASK
|;;           .extern _SKEWMASK        | [24]
	.extern	_SKEWMASK
|;;           .extern _SCALDIR         | [25]
	.extern	_SCALDIR
|;;           .extern _XDDA            | [26]
	.extern	_XDDA
|;;           .extern _DDAINC          | [27]
	.extern	_DDAINC
|;;           .extern _SCRPT2          | [28]
	.extern	_SCRPT2
|;;           .extern _SCRTCHP         | [29]
	.extern	_SCRTCHP





|;;           .globl _deftxbuf         | [35]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [36]
	.globl	_scrtsiz
| 258 "vdi/vdi_tblit.S"
|;;           .bss                     | [38]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [40]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [42]
	.p2align 2,0
	.text

|;;           .even                    | [44]
	.p2align 1,0
_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [46]
	.short	(2+6+8+15)/16*2*16*2
| 371 "vdi/vdi_tblit.S"
|;;           .text                    | [48]






	.p2align 1,0
_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [56]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [57]
      movea.l  (44+8,%a7),%a0          | [58]
      movea.l  (44+12,%a7),%a1         | [59]
      jbsr     norm_blt                | [60]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [61]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [62]


_outline:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [66]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [67]
      movea.l  (44+8,%a7),%a0          | [68]
      move.w   (44+12,%a7),%d7         | [69]
      jbsr     outlin                  | [70]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [71]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [72]


_rotate:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [76]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [77]
      jbsr     rotation                | [78]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [79]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [80]


_scale:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [84]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [85]
      jbsr     replicat                | [86]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [87]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [88]

|;;           .text                    | [90]







|;;                   .even            | [98]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [99]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [100]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [101]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [102]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [103]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [104]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [105]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [106]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [107]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [108]
	.short	0x007F
|;;                   .dc.w 0x003F     | [109]
	.short	0x003F
|;;                   .dc.w 0x001F     | [110]
	.short	0x001F
|;;                   .dc.w 0x000F     | [111]
	.short	0x000F
|;;                   .dc.w 0x0007     | [112]
	.short	0x0007
|;;                   .dc.w 0x0003     | [113]
	.short	0x0003
|;;                   .dc.w 0x0001     | [114]
	.short	0x0001
|;;                   .dc.w 0x0000     | [115]
	.short	0x0000
| 464 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*3,4*3    | [118]
	.byte	4*0,4*0,4*3,4*3
|;;           .dc.b 4*4,4*4,4*7,4*7    | [119]
	.byte	4*4,4*4,4*7,4*7
|;;           .dc.b 4*6,4*6,4*6,4*6    | [120]
	.byte	4*6,4*6,4*6,4*6
|;;           .dc.b 4*1,4*1,4*13,4*13  | [121]
	.byte	4*1,4*1,4*13,4*13

|;;           .dc.b 4*0,4*15,4*0,4*15  | [123]
	.byte	4*0,4*15,4*0,4*15
|;;           .dc.b 4*0,4*14,4*1,4*15  | [124]
	.byte	4*0,4*14,4*1,4*15
|;;           .dc.b 4*0,4*13,4*2,4*15  | [125]
	.byte	4*0,4*13,4*2,4*15
|;;           .dc.b 4*0,4*12,4*3,4*15  | [126]
	.byte	4*0,4*12,4*3,4*15
|;;           .dc.b 4*0,4*11,4*4,4*15  | [127]
	.byte	4*0,4*11,4*4,4*15
|;;           .dc.b 4*0,4*10,4*5,4*15  | [128]
	.byte	4*0,4*10,4*5,4*15
|;;           .dc.b 4*0,4*9,4*6,4*15   | [129]
	.byte	4*0,4*9,4*6,4*15
|;;           .dc.b 4*0,4*8,4*7,4*15   | [130]
	.byte	4*0,4*8,4*7,4*15
|;;           .dc.b 4*0,4*7,4*8,4*15   | [131]
	.byte	4*0,4*7,4*8,4*15
|;;           .dc.b 4*0,4*6,4*9,4*15   | [132]
	.byte	4*0,4*6,4*9,4*15
|;;           .dc.b 4*0,4*5,4*10,4*15  | [133]
	.byte	4*0,4*5,4*10,4*15
|;;           .dc.b 4*0,4*4,4*11,4*15  | [134]
	.byte	4*0,4*4,4*11,4*15
|;;           .dc.b 4*0,4*3,4*12,4*15  | [135]
	.byte	4*0,4*3,4*12,4*15
|;;           .dc.b 4*0,4*2,4*13,4*15  | [136]
	.byte	4*0,4*2,4*13,4*15
|;;           .dc.b 4*0,4*1,4*14,4*15  | [137]
	.byte	4*0,4*1,4*14,4*15
|;;           .dc.b 4*0,4*0,4*15,4*15  | [138]
	.byte	4*0,4*0,4*15,4*15

toptable:
|;;           .dc.l top0,top1,top2,top3,top4,top5,top6,top7 | [141]
	.long	top0,top1,top2,top3,top4,top5,top6,top7
|;;           .dc.l top8,top9,topa,topb,topc,topd,tope,topf | [142]
	.long	top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
|;;           .dc.l twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7 | [145]
	.long	twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
|;;           .dc.l twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf | [146]
	.long	twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

|;;           .text                    | [148]
| 508 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [151]
|;;   sub.w     -24(a6),d1             | [152]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [153 PL]
      jpl      do_rot                  | [154]



|;;   neg.w     d1                     | [158]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [159]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [160]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [162]

|;;   add.w     d0,d0                  | [164]
	move.w   %d0,%d5
	add.l    %d0,%d5
      lea      lf_tab,%a2              | [165]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [166]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [167]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [168]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [173]
|;;   add.w     d0,d0                  | [174]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [175]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [177]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [178]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [181]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [182]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [183]
	andi.l   #(0xFFFF&0xF),%d5
|;;   add.w     d2,d2                  | [184]
	add.l    %d5,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [185]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [186]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [187]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [188]
      move.w   #0x8000,(-80,%a6)       | [189]
      moveq.l  #(-1),%d3               | [190]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [193 HI]
      jhi      doub_des                | [194]



|;;   add.w     d0,d0                  | [198]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [199]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [200]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [201]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [203]
	move.w   %d5,%d1
      jra      msk_done                | [204]



doub_des:
      move.w   %d0,%d1                 | [209]
|;;   lsr.w     #4,d1                  | [210]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [211]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [212 NE]
      jne      mlt_dest                | [213]



      move.w   (-24,%a6),%d1           | [217]
|;;   add.w     -20(a6),d1             | [218]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [219 CC]
      jcc      mlt_dest                | [220]
      moveq.l  #0,%d7                  | [221]
      jra      do_ritem                | [222]

mlt_dest:
      tst.w    (-42,%a6)               | [225 LT]
      jlt      mltleft                 | [226]
      moveq.l  #4,%d7                  | [227]
      jra      do_ritem                | [228]
mltleft:
      moveq.l  #8,%d7                  | [230]
do_ritem:
|;;   andi.w    #0x000F,d0             | [232 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [233]
|;;   subq.w    #1,d3                  | [234]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [235]
	move.w   %d5,%d3
not_null:
|;;   add.w     d0,d0                  | [237]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [238]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [239]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [242]
      move.w   %d3,(-28,%a6)           | [243]
      move.w   %d4,(-36,%a6)           | [244]
|;;   addq.w    #2,d3                  | [245]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [246 CS]
      jcs      msk0                    | [247]
|;;   addq.w    #1,d3                  | [248]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [250]
      move.w   %d3,(-30,%a6)           | [251]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5

plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [254]
|;;   lsr.w     -84(a6)                | [255 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   addx.w    d0,d0                  | [256]
	move.w   %d0,%d1
	addx.l   %d0,%d1
|;;   lsr.w     -76(a6)                | [257 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   addx.w    d0,d0                  | [258]
	addx.l   %d1,%d1



      lea      toptable,%a4            | [262]
	move.w   %d1,%d0
|;;   move.b    wrmappin-toptable(a4,d0.w),d0 | [263]
	move.w   %d0,%a5
	move.b   (wrmappin-toptable,%a4,%a5.l*1),%d0
|;;   movea.l   toptable-toptable(a4,d0.w),a2 | [264]
	move.w   %d0,%a5
	movea.l  (toptable-toptable,%a4,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [265]
      movea.l  %a2,%a5                 | [266]

|;;   movea.l   twoptble-toptable(a4,d0.w),a4 | [268]
	move.w   %d0,%a0
	movea.l  (twoptble-toptable,%a4,%a0.l*1),%a4



      move.w   (-96,%a6),%d0           | [272 EQ]
      jeq      wrt_char                | [273]

      btst     #1,%d0                  | [275 EQ]
      jeq      no_lite                 | [276]



|;;   move.w    _LITEMASK,-78(a6)      | [280]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [281]
      lea      liteop,%a5              | [282]

      move.l   %a2,(-64,%a6)           | [284]
      lea      liteopwf,%a2            | [285]

      move.l   %a4,(-72,%a6)           | [287]
      lea      liteopw,%a4             | [288]

no_lite:
      btst     #0,%d0                  | [291 EQ]
      jeq      no_thick                | [292]



      clr.w    (-82,%a6)               | [296]

      move.l   %a5,(-48,%a6)           | [298]
      lea      thknop,%a5              | [299]

      move.l   %a2,(-60,%a6)           | [301]
      lea      thknopwf,%a2            | [302]

      move.l   %a4,(-68,%a6)           | [304]
      lea      thknopw,%a4             | [305]

no_thick:
      btst     #2,%d0                  | [308 EQ]
      jeq      wrt_char                | [309]



|;;   move.w    _SKEWMASK,-80(a6)      | [313]
	move.w   _SKEWMASK,%d1
	move.w   %d1,(-80,%a6)
      move.l   %a5,(-56,%a6)           | [314]
      lea      skewop,%a5              | [315]

      tst.w    (-124,%a6)              | [317 EQ GT]
      jgt      wrt_char                | [318]
      jeq      not_sngl                | [319]
      clr.w    (-28,%a6)               | [320]
      moveq.l  #0,%d7                  | [321]
      jra      chng_blt                | [322]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [324 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [325]

      tst.w    (-42,%a6)               | [327 LT]
      jlt      chng_left               | [328]
      moveq.l  #4,%d7                  | [329]
      jra      chng_blt                | [330]
chng_left:
      moveq.l  #8,%d7                  | [332]
chng_blt:
      move.w   %d7,(-124,%a6)          | [334]

wrt_char:
      move.w   (-124,%a6),%d7          | [337 LE]
      jle      set_chr_hgt             | [338]
      movea.l  %a2,%a5                 | [339]
      move.w   (-18,%a6),%d5           | [340]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [342]
|;;   subq.w    #1,d3                  | [343]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [345]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [346]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [348 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [349]
      movea.l  (-10,%a6),%a0           | [350]
      movea.l  (-14,%a6),%a1           | [351]
      addq.l   #2,%a1                  | [352]
      move.l   %a1,(-14,%a6)           | [353]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [354 EQ]
      jeq      plane_loop              | [355]
      jra      norm_blt                | [356]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [358]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [360]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [366]
      move.w   (-16,%a6),%d6           | [367]
      move.w   (-18,%a6),%d7           | [368]
      lea      sgl_rtn,%a3             | [369]
      jra      sgl_1st_scan            | [370]

sgl_lp:
|;;   adda.w    d6,a0                  | [373]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [374]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [376]
| 743 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [378]


      move.w   (-42,%a6),%d1           | [381 MI]
      jmi      left_rol                | [382]
      lsr.l    %d1,%d0                 | [383]
      jra      end_ro                  | [384]
left_rol:
      lsl.l    %d1,%d0                 | [386]
end_ro:
      swap     %d0                     | [388]
      move.w   %d0,%d1                 | [389]
      swap     %d0                     | [390]

      jmp      (%a5)                   | [392]

sgl_rtn:
      move.w   %d1,(%a1)               | [395]
|;;   dbra      d3,sgl_lp              | [396]
	tst.w    %d3
	jeq      _PA_395_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_395_:         
      jra      next_plane              | [397]




dbl_loop:
      movea.l  %a3,%a4                 | [403]
      move.w   (-16,%a6),%d6           | [404]
      move.w   (-18,%a6),%d7           | [405]
      lea      dbl_2wrd,%a2            | [406]
      move.l   %a2,%d5                 | [407]
      lea      dbl_1wrd,%a3            | [408]
      jra      dbl_1st_scan            | [409]

dbl_lp:
|;;   adda.w    d6,a0                  | [412]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [413]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [414]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [416]
| 790 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [418]


      move.w   (-42,%a6),%d1           | [421 MI]
      jmi      dbleft                  | [422]
      lsr.l    %d1,%d0                 | [423]
      jra      dblendr                 | [424]
dbleft:
      lsl.l    %d1,%d0                 | [426]
dblendr:
      swap     %d0                     | [428]
      move.w   %d0,%d1                 | [429]
      swap     %d0                     | [430]
      move.w   (-38,%a6),%d2           | [431]

      jmp      (%a5)                   | [433]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [436]
      move.w   %d0,%d1                 | [437]

      movea.w  (-88,%a6),%a2           | [439]
|;;   move.w    (a1,a2.w),d4           | [440]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [442]

|;;   exg       a3,d5                  | [444]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [445]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [448]
|;;   move.w    d1,(a1,a2.w)           | [449]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [451]
	tst.w    %d3
	jeq      _PA_450_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_450_:         
      jra      next_plane              | [452]





left_loop:
      movea.l  %d6,%a0                 | [459]
      movea.l  %d7,%a1                 | [460]
|;;   adda.w    -16(a6),a0             | [461]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [462]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [463]
|;;   rol.w     -78(a6)                | [464]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [466 NE]
      jne      skewopw                 | [467]
mlt_left:
      move.l   %a0,%d6                 | [469]
      move.l   %a1,%d7                 | [470]
      move.w   (-28,%a6),(-44,%a6)     | [471]







      move.l   (%a0)+,%d0              | [479]


      move.w   (%a1),%d4               | [482]

      move.w   (-42,%a6),%d1           | [484]
      lsl.l    %d1,%d0                 | [485]

      swap     %d0                     | [487]
      move.w   %d0,%d1                 | [488]
      swap     %d0                     | [489]

      move.w   (-38,%a6),%d2           | [491]

      lea      lft_lfrng,%a3           | [493]
      jmp      (%a5)                   | [494]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [497]
|;;   adda.w    -88(a6),a1             | [498]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [500]
      lea      lft_wrds,%a3            | [501]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [506]
|;;   and.w     d2,d4                  | [507]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [508]





      swap     %d0                     | [514]
      move.w   %d1,%d0                 | [515]
      swap     %d0                     | [516]

      move.w   (-42,%a6),%d1           | [518]
      lsl.l    %d1,%d0                 | [519]

      swap     %d0                     | [521]
      move.w   %d0,%d1                 | [522]
      swap     %d0                     | [523]

|;;   not.w     d2                     | [525]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [526]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [527]
	not.l    %d5
|;;   eor.w     d4,d1                  | [528]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [530]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [532 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [533]

      jmp      (%a4)                   | [535]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [538]
|;;   adda.w    -88(a6),a1             | [539]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [541]

lef_don:
      move.w   (-36,%a6),%d2           | [544]

      lea      lft_rfrng,%a3           | [546]
      jmp      (%a5)                   | [547]

lft_rfrng:
      move.w   %d1,(%a1)               | [550]

|;;   dbra      d3,left_loop           | [552]
	tst.w    %d3
	jeq      _PA_551_
	subq.l   #1,%d3
	jra      left_loop
	_PA_551_:         
      jra      next_plane              | [553]





rite_loop:
      movea.l  %d6,%a0                 | [560]
      movea.l  %d7,%a1                 | [561]
|;;   adda.w    -16(a6),a0             | [562]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [563]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [564]
|;;   rol.w     -78(a6)                | [565]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [567 NE]
      jne      skewopw                 | [568]
mlt_rite:
      move.l   %a0,%d6                 | [570]
      move.l   %a1,%d7                 | [571]
      move.w   (-28,%a6),(-44,%a6)     | [572]
      move.w   (%a0)+,%d0              | [573]



      swap     %d0                     | [577]

      move.w   (%a1),%d4               | [579]

      move.w   (-42,%a6),%d1           | [581]
      lsr.l    %d1,%d0                 | [582]
      swap     %d0                     | [583]
      move.w   %d0,%d1                 | [584]
      swap     %d0                     | [585]

      move.w   (-38,%a6),%d2           | [587]

      lea      rgt_lfrng,%a3           | [589]
      jmp      (%a5)                   | [590]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [593]
|;;   adda.w    -88(a6),a1             | [594]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [596]
      lea      rgt_wrds,%a3            | [597]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [602]
|;;   and.w     d2,d4                  | [603]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [604]
      move.w   (%a0)+,%d0              | [605]



      swap     %d0                     | [609]

      move.w   (-42,%a6),%d1           | [611]
      lsr.l    %d1,%d0                 | [612]
      swap     %d0                     | [613]
      move.w   %d0,%d1                 | [614]
      swap     %d0                     | [615]

|;;   not.w     d2                     | [617]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [618]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [619]
	not.l    %d5
|;;   eor.w     d4,d1                  | [620]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [622]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [624 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [625]

      jmp      (%a4)                   | [627]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [630]
|;;   adda.w    -88(a6),a1             | [631]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [633]

rite_don:
      move.w   (-36,%a6),%d2           | [636]

      lea      rgt_rfrng,%a3           | [638]
      jmp      (%a5)                   | [639]

rgt_rfrng:
      move.w   %d1,(%a1)               | [642]

|;;   dbra      d3,rite_loop           | [644]
	tst.w    %d3
	jeq      _PA_643_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_643_:         
      jra      next_plane              | [645]
| 1032 "vdi/vdi_tblit.S"
top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [648]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [649]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [650]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [651]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [652]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [655]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [656]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [657]
	not.l    %d0
|;;   and.w     d4,d1                  | [658]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [659]

top2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [662]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [663]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [664]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [665]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [666]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [669]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [670]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [671]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [672]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [675]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [676]
	not.l    %d0
|;;   and.w     d4,d1                  | [677]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [678]

top5:
      move.w   %d4,%d1                 | [681]
      jmp      (%a3)                   | [682]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [685]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [686]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [687]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [690]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [691]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [692]

top8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [695]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [696]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [697]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [698]

top9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [701]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [702]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [703]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [704]

topa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [707]
|;;   eor.w     d2,d1                  | [708]
	move.w   %d1,%d0
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [709]

topb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d2,d4                  | [712]
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   and.w     d2,d1                  | [713]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [714]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [715]

topc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [718]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [719]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [720]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [721]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [722]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [725]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [726]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [727]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [728]

tope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [731]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [732]
	and.l    %d2,%d0
|;;   eor.w     d2,d4                  | [733]
	move.w   %d0,%d1
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   or.w      d4,d1                  | [734]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [735]

topf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d2,d4                  | [738]
	mvz.w    %d2,%d0
	or.l     %d0,%d4
      move.w   %d4,%d1                 | [739]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [740]
| 1138 "vdi/vdi_tblit.S"
twop0:
      moveq.l  #0,%d1                  | [743]
      jmp      (%a3)                   | [744]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [747]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [748]

twop2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [751]
	move.w   %d4,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [752]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [753]

twop3:
      jmp      (%a3)                   | [756]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [759]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [760]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [761]

twop5:
      move.w   %d4,%d1                 | [764]
      jmp      (%a3)                   | [765]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [768]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [769]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [772]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [773]

twop8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [776]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|;;   not.w     d1                     | [777]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [778]

twop9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [781]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   not.w     d1                     | [782]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [783]

twopa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [786]
|;;   not.w     d1                     | [787]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [788]

twopb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [791]
	move.w   %d4,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [792]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [793]

twopc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [796]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [797]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [800]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [801]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [802]

twope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [805]
	move.w   %d1,%d0
	and.l    %d4,%d0
|;;   not.w     d1                     | [806]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [807]

twopf:
      moveq.l  #(-1),%d1               | [810]
      jmp      (%a3)                   | [811]
| 1225 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [814]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [815]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [816]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [817 EQ]
      jeq      thk00                   | [818]
|;;   ori.b     #0x10,ccr              | [819 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [820 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [821]
      move.w   #0x8000,%d6             | [822]
thk01:
      move.w   %d6,(-34,%a6)           | [824]
thk00:
      tst.w    (-28,%a6)               | [826 MI]
      jmi      thk0                    | [827]
      cmp.w    (-36,%a6),%d6           | [828 CC]
      jcc      thk0                    | [829]
|;;   and.w     d6,d0                  | [830]
	and.l    %d6,%d0
      jra      thk1                    | [831]
thk0:
      clr.w    %d0                     | [833]
|;;   and.w     d6,d1                  | [834]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [836]
      move.w   %d2,%d5                 | [837]
      swap     %d5                     | [838]
      move.w   (-36,%a6),%d5           | [839]
      lsl.l    %d6,%d5                 | [840]

      swap     %d5                     | [842]
      move.w   %d5,%d7                 | [843]
      swap     %d5                     | [844]

|;;   and.w     d7,d1                  | [846]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [847]
	and.l    %d5,%d0
|;;   swap      d1                     | [848]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [849]
|;;   subq.w    #1,d6                  | [850]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [852]
      lsr.l    #1,%d0                  | [853]
      or.l     %d0,%d1                 | [854]
|;;   dbra      d6,thkoplp             | [855]
	tst.w    %d6
	jeq      _PA_854_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_854_:         

      move.w   %d1,%d0                 | [857]
      swap     %d1                     | [858]
|;;   movem.l   (sp)+,d5-d7            | [859]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [860]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [861]
| 1288 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [864]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [865 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [866]
|;;   and.w     -34(a6),d1             | [867]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [869]
      swap     %d1                     | [870]
      clr.w    %d1                     | [871]
      move.l   %d1,%d5                 | [872]
|;;   subq.w    #1,d6                  | [873]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [875]
      or.l     %d5,%d1                 | [876]
|;;   dbra      d6,thkopwlp            | [877]
	tst.w    %d6
	jeq      _PA_876_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_876_:         

      move.w   %d1,%d5                 | [879]
      swap     %d1                     | [880]
|;;   or.w      -82(a6),d1             | [881]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [882]
|;;   movem.l   (sp)+,d5-d6            | [883]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [884]
      jmp      (%a2)                   | [885]
| 1326 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [888]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [889]
|;;   subq.w    #1,-30(a6)             | [890 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [891]
      jeq      thk4                    | [892]

|;;   subq.w    #1,-30(a6)             | [894 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [895]
|;;   and.w     d6,d1                  | [896]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [898]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [899]

thk3:
      clr.w    %d1                     | [902]
      jra      thk5                    | [903]

thk4:
|;;   and.w     d6,d1                  | [906]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [909 MI]
      jmi      thk6                    | [910]
      move.w   (-28,%a6),%d5           | [911]
|;;   addq.w    #2,d5                  | [912]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [913]
|;;   ori.b     #0x10,ccr              | [914 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [915 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [916]
      move.w   #0x8000,%d7             | [917]
|;;   addq.w    #1,d5                  | [918]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [920 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [921]
|;;   subq.w    #1,d5                  | [922]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [924 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [925 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [926]
      move.w   #0x8000,%d6             | [927]
thk31:
      cmp.w    %d6,%d7                 | [929 CS]
      jcs      thk35                   | [930]
|;;   addq.w    #1,d5                  | [931]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [933]
      move.w   %d5,(-32,%a6)           | [934]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [936]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [938]
      swap     %d1                     | [939]
      clr.w    %d1                     | [940]
      move.l   %d1,%d5                 | [941]
|;;   subq.w    #1,d6                  | [942]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [944]
      or.l     %d5,%d1                 | [945]
|;;   dbra      d6,thkopwl             | [946]
	tst.w    %d6
	jeq      _PA_945_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_945_:         

      move.w   %d1,%d5                 | [948]
      swap     %d1                     | [949]
|;;   or.w      -82(a6),d1             | [950]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [951]
|;;   and.w     d2,d1                  | [952]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [953]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [954]
      jmp      (%a2)                   | [955]
| 1407 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [958]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [959]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [960]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [961]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [962]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [965]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [966]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [967]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [970]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [971]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [972]
| 1439 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [975 CC]
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
      jcc      no_shift                | [976]

      swap     %d1                     | [978]
      move.w   %d0,%d1                 | [979]
      lsr.l    #1,%d1                  | [980]

      swap     %d2                     | [982]
      move.w   (-36,%a6),%d2           | [983]
      lsr.l    #1,%d2                  | [984]
      move.w   %d2,(-36,%a6)           | [985]
      swap     %d2                     | [986]
      move.w   %d2,(-38,%a6)           | [987 EQ]
      jeq      nxt_word                | [988]
      move.w   (-42,%a6),%d0           | [989 MI]
      jmi      dec_rol                 | [990]
ror_add:
|;;   addq.w    #1,d0                  | [992]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [994]
      move.w   %d1,%d0                 | [995]
      swap     %d1                     | [996]
no_shift:
      movea.l  (-56,%a6),%a2           | [998]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [999]

dec_rol:
      tst.b    %d0                     | [1002 EQ]
      jeq      begn_ror                | [1003]
|;;   subq.w    #1,d0                  | [1004]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [1005]
begn_ror:
      clr.w    %d0                     | [1007]
      jra      ror_add                 | [1008]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1013]
      swap     %d2                     | [1014]
      move.w   %d2,(-38,%a6)           | [1015]
|;;   adda.w    -88(a6),a1             | [1016]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1017]

      moveq.l  #15,%d0                 | [1019]
|;;   sub.w     -42(a6),d0             | [1020]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1021]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1022]
      jra      no_shift                | [1023]






skewopw:
|;;   rol.w     -80(a6)                | [1031 CC]
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
      jcc      do_shift                | [1032]

      tst.w    (-42,%a6)               | [1034 MI]
      jmi      mlt_left                | [1035]
      jra      mlt_rite                | [1036]
do_shift:
|;;   ori       #0x10,ccr              | [1038 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1039]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1041]
      cmp.w    #0xFFFF,%d0             | [1042 EQ]
      jeq      inc_rite                | [1043]

|;;   ori       #0x10,ccr              | [1045 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1046]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1047]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1050]
      tst.b    %d0                     | [1051 NE]
      jne      no_rota                 | [1052]
      move.w   #0x8000,(-40,%a6)       | [1053]
no_rota:
      move.w   (-38,%a6),%d1           | [1055]
|;;   lsr.w     #1,d1                  | [1056 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [1057]

      move.w   %d1,(-38,%a6)           | [1059]

      tst.w    %d0                     | [1061 MI]
      jmi      wdec_rol                | [1062]
|;;   addq.w    #1,-42(a6)             | [1063]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [1064]
wdec_rol:
      tst.b    %d0                     | [1066 EQ]
      jeq      set_msk                 | [1067]
|;;   subq.w    #1,-42(a6)             | [1068]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [1069]
set_msk:
      move.w   #1,(-42,%a6)            | [1071]
      jra      mlt_rite                | [1072]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1075]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1076]
      jra      do_left                 | [1077]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1080]

|;;   subq.w    #1,-28(a6)             | [1082]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1083]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1085]
|;;   sub.w     d0,d2                  | [1086]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1087]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1088]

      jra      mlt_left                | [1090]




rotation:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   _SOURCEX,%d1            | [1096]
      move.w   %d1,%d2                 | [1097]
|;;   andi.w    #0x0F,d2               | [1098]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1099]
|;;   lsr.w     #4,d1                  | [1100]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1101]
	add.l    %d1,%d1

      movea.l  (-10,%a6),%a0           | [1103]
|;;   adda.w    d1,a0                  | [1104]
	mvs.w    %d1,%d0
	adda.l   %d0,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1106]
      move.w   _SOURCEY,%d0            | [1107]
      move.w   (-106,%a6),%d1          | [1108]
      move.w   %d1,(-22,%a6)           | [1109]
      move.w   (-16,%a6),%d2           | [1110]

      tst.w    (-126,%a6)              | [1112 EQ LT]
      jeq      upsd_dwn                | [1113]
rot90:
      jlt      top_src                 | [1115]
|;;   neg.w     -16(a6)                | [1116]
	move.w   (-16,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-16,%a6)
|;;   subq.w    #1,d1                  | [1117]
	subq.l   #1,%d1
|;;   add.w     d1,d0                  | [1118]
	add.l    %d1,%d0
      mulu.w   %d2,%d0                 | [1119]
      adda.l   %d0,%a0                 | [1120]
top_src:
      move.w   (-106,%a6),%d0          | [1122]
|;;   lsr.w     #4,d0                  | [1123]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1124]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1125]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1126]
      movea.l  _SCRTCHP,%a1            | [1127]
|;;   adda.w    -4(a6),a1              | [1128]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1

      tst.w    (-126,%a6)              | [1130 GT]
      jgt      top_dwn                 | [1131]
|;;   neg.w     -18(a6)                | [1132]
	move.w   (-18,%a6),%d5
	neg.l    %d5
	move.w   %d5,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1133]
|;;   subq.w    #1,d1                  | [1134]
	subq.l   #1,%d1
      mulu.w   %d1,%d0                 | [1135]
      adda.l   %d0,%a1                 | [1136]
top_dwn:
      move.w   (-24,%a6),%d2           | [1138]
      move.w   #0x8000,%d3             | [1139]
      move.w   %d3,%d4                 | [1140]
      movea.w  %d3,%a4                 | [1141]
|;;   lsr.w     d2,d3                  | [1142]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3
      moveq.l  #0,%d0                  | [1143]
      move.w   (-20,%a6),%d2           | [1144]
      move.w   (-16,%a6),%d6           | [1145]
      movea.l  %a0,%a2                 | [1146]
      movea.l  %a1,%a3                 | [1147]
      jra      rot_nsrc                | [1148]
rot_ylp:
      move.w   (-22,%a6),%d1           | [1150]
      jra      rot_srt                 | [1151]
rot_xlp:
      move.w   (%a0),%d7               | [1153]
|;;   and.w     d3,d7                  | [1154 EQ]
	and.l    %d3,%d7
	tst.w    %d7
      jeq      rot_nor                 | [1155]
|;;   or.w      d4,d0                  | [1156]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     #1,d4                  | [1158 CC]
	move.w   %d4,%d5
	swap     %d4
	move.w   %d5,%d4
	lsr.l    #1,%d4
      jcc      rot_isrc                | [1159]
rot_ndst:
      move.w   %d0,(%a1)+              | [1161]
      moveq.l  #0,%d0                  | [1162]
rot_isrc:
|;;   adda.w    d6,a0                  | [1164]
	mvs.w    %d6,%d5
	adda.l   %d5,%a0
rot_srt:
|;;   dbra      d1,rot_xlp             | [1166]
	tst.w    %d1
	jeq      _PA_1165_
	subq.l   #1,%d1
	jra      rot_xlp
	_PA_1165_:         

      move.w   %d0,(%a1)               | [1168]
      moveq.l  #0,%d0                  | [1169]
|;;   adda.w    -18(a6),a3             | [1170]
	mvs.w    (-18,%a6),%d5
	adda.l   %d5,%a3
      movea.l  %a3,%a1                 | [1171]
      move.w   %a4,%d4                 | [1172]
|;;   ror.w     #1,d3                  | [1173 CC]
	move.w   %d3,%d5
	swap     %d3
	move.w   %d5,%d3
	lsr.l    #1,%d3
      jcc      rnew_src                | [1174]
      addq.l   #2,%a2                  | [1175]
rnew_src:
      movea.l  %a2,%a0                 | [1177]
rot_nsrc:
|;;   dbra      d2,rot_ylp             | [1179]
	tst.w    %d2
	jeq      _PA_1178_
	subq.l   #1,%d2
	jra      rot_ylp
	_PA_1178_:         

rot_done:
      move.w   (-102,%a6),%d0          | [1182]
      move.w   (-106,%a6),%d1          | [1183]
      move.w   %d1,(-20,%a6)           | [1184]
      move.w   %d1,(-102,%a6)          | [1185]
      move.w   %d0,(-22,%a6)           | [1186]
      move.w   %d0,(-106,%a6)          | [1187]

      move.w   (-92,%a6),%d0           | [1189]
      move.w   (-90,%a6),(-92,%a6)     | [1190]
      move.w   %d0,(-90,%a6)           | [1191]
      move.w   #1,(-94,%a6)            | [1192]

      move.w   (-18,%a6),%d0           | [1194]
      tst.w    (-126,%a6)              | [1195 GT]
      jgt      rot_nneg                | [1196]
|;;   neg.w     d0                     | [1197]
	neg.l    %d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1199]
      move.w   (-4,%a6),%d0            | [1200]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5
repexit:
      clr.w    _SOURCEX                | [1202]
repexit3:
      movea.l  _SCRTCHP,%a2            | [1204]
|;;   adda.w    d0,a2                  | [1205]
	ext.l    %d0
	adda.l   %d0,%a2
      move.l   %a2,(-10,%a6)           | [1206]
      clr.w    _SOURCEY                | [1207]
      rts                              | [1208]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1211]
|;;   add.w     -24(a6),d0             | [1212]
	move.w   (-24,%a6),%d5
	add.l    %d5,%d0
|;;   subq.w    #1,d0                  | [1213]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1214]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1215]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1216]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1218]
      move.w   %d0,%d2                 | [1219]
|;;   lsr.w     #1,d2                  | [1220]
	lsr.l    #1,%d2
|;;   subq.w    #1,d2                  | [1221]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1222]
|;;   add.w     -4(a6),d0              | [1223]
	move.w   (-4,%a6),%d5
	add.l    %d5,%d0
      movea.l  _SCRTCHP,%a1            | [1224]
|;;   adda.w    d0,a1                  | [1225]
	ext.l    %d0
	adda.l   %d0,%a1
      jra      strtflip                | [1226]
upsd_lp:
      movea.l  %a0,%a2                 | [1228]
      move.w   %d2,%d3                 | [1229]
line_lp:
      move.w   (%a2)+,%d0              | [1231]
      moveq.l  #0,%d6                  | [1232]
      moveq.l  #15,%d4                 | [1233]
flip_lp:
|;;   lsr.w     #1,d0                  | [1235 X]
	and.l    #0xffff,%d0
	lsr.l    #1,%d0
|;;   addx.w    d6,d6                  | [1236]
	addx.l   %d6,%d6
|;;   dbra      d4,flip_lp             | [1237]
	subq.l   #1,%d4
	jpl      flip_lp

      move.w   %d6,-(%a1)              | [1239]
|;;   dbra      d3,line_lp             | [1240]
	tst.w    %d3
	jeq      _PA_1239_
	subq.l   #1,%d3
	jra      line_lp
	_PA_1239_:         

|;;   adda.w    -16(a6),a0             | [1242]
	mvs.w    (-16,%a6),%d5
	adda.l   %d5,%a0
strtflip:
|;;   dbra      d1,upsd_lp             | [1244]
	tst.w    %d1
	jeq      _PA_1243_
	subq.l   #1,%d1
	jra      upsd_lp
	_PA_1243_:         

      move.w   (-18,%a6),(-16,%a6)     | [1246]
      movea.l  _SCRTCHP,%a1            | [1247]
|;;   adda.w    -4(a6),a1              | [1248]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1249]
      move.w   _SOURCEX,%d0            | [1250]
|;;   add.w     -102(a6),d0            | [1251]
	move.w   (-102,%a6),%d5
	add.l    %d5,%d0
|;;   neg.w     d0                     | [1252]
	neg.l    %d0
|;;   andi.w    #0xF,d0                | [1253]
	andi.l   #(0xFFFF&0xF),%d0
      move.w   %d0,_SOURCEX            | [1254]
      clr.w    _SOURCEY                | [1255]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      rts                              | [1256]






replicat:
|***	SAVE	d7	***
	lea      -12(%a7),%a7
	movem.l  %d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1264]
      move.w   %d0,%d4                 | [1265]
|;;   andi.w    #0x0F,d0               | [1266]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1267]
|;;   lsr.w     #4,d4                  | [1268]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   add.w     d4,d4                  | [1269]
	add.l    %d4,%d4

      move.w   _SOURCEY,%d0            | [1271]
      mulu.w   (-16,%a6),%d0           | [1272]
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1273]
|;;   adda.w    d4,a0                  | [1274]
	mvs.w    %d4,%d3
	adda.l   %d3,%a0
      adda.l   %d0,%a0                 | [1275]

      move.w   (-24,%a6),%d2           | [1277]
      move.w   #0x8000,%d3             | [1278]
      move.w   %d3,%d4                 | [1279]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1280]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1282]
      move.w   (-102,%a6),%d1          | [1283]

      move.w   %d2,(-22,%a6)           | [1285]
      move.w   %d1,(-20,%a6)           | [1286]
      movea.l  _SCRTCHP,%a1            | [1287]
|;;   adda.w    _SCRPT2,a1             | [1288]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1290 EQ]
      jeq      noline                  | [1291]



|;;   addq.w    #1,-106(a6)            | [1295]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1296]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   add.w     d1,d1                  | [1298]
	add.l    %d1,%d1
|;;   addq.w    #3,d1                  | [1299]
	addq.l   #3,%d1
|;;   lsr.w     #4,d1                  | [1300]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1301]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1302]
	addq.l   #2,%d1
      move.w   %d1,%d5                 | [1303]
      moveq.l  #0,%d6                  | [1304]
oklear:
      move.w   %d6,(%a1)+              | [1306]
|;;   dbra      d5,oklear              | [1307]
	tst.w    %d5
	jeq      _PA_1306_
	subq.l   #1,%d5
	jra      oklear
	_PA_1306_:         
      lea      (-2,%a1),%a2            | [1308]
      move.w   %d2,%d5                 | [1309]
|;;   add.w     d5,d5                  | [1310]
	add.l    %d5,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1312]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1313]
|;;   dbra      d5,oklear1             | [1314]
	tst.w    %d5
	jeq      _PA_1313_
	subq.l   #1,%d5
	jra      oklear1
	_PA_1313_:         

|;;   move.w    d1,d5                  | [1316]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1317]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   subq.w    #1,d5                  | [1318 X]
	subq.l   #1,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1320]
|;;   dbra      d5,oklear3             | [1321]
	move     %CCR,%d0
	tst.w    %d5
	jeq      _PA_1320_
	subq.l   #1,%d5
	move     %d0,%CCR
	jra      oklear3
	_PA_1320_:         
	move     %d0,%CCR
      jra      noline1                 | [1322]

noline:
|;;   lsr.w     #3,d1                  | [1325]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   add.w     d1,d1                  | [1326]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1327 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1329]
|***	RESTORE	AFTER d7	***
	movem.l  8(%a7),%d7



      move.w   _SCALDIR,%d7            | [1333]
|;;   roxr.l    #1,d7                  | [1334 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1335]
|;;   roxr.l    #1,d7                  | [1336]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1338]
      move.w   _DDAINC,%d2             | [1339]
      move.w   (-22,%a6),%d5           | [1340]
|;;   subq.w    #1,d5                  | [1341]
	subq.l   #1,%d5
      move.w   #32767,%d6              | [1342]
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      12(%a7),%a7
      tst.l    %d7                     | [1343 MI]
      jmi      rep_ylop                | [1344]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1346 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1347]
      jbsr     yloop                   | [1348]
y_no_drw:
|;;   adda.w    d7,a0                  | [1350]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1351]
	tst.w    %d5
	jeq      _PA_1350_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1350_:         
      jra      y_rep_don               | [1352]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1354 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1355]
      jbsr     yloop                   | [1356]
y_no_rep:
      jbsr     yloop                   | [1358]
|;;   adda.w    d7,a0                  | [1359]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1360]
	tst.w    %d5
	jeq      _PA_1359_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1359_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1362]
|;;   subq.w    #1,d2                  | [1363]
	subq.l   #1,%d2
      move.w   _XDDA,%d1               | [1364]
      move.w   _DDAINC,%d0             | [1365]
      moveq.l  #0,%d3                  | [1366]
repwidcl:
|;;   add.w     d0,d1                  | [1368 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1369]
|;;   addq.w    #1,d3                  | [1370]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1372 PL]
      jpl      nrpndoub                | [1373]
|;;   addq.w    #1,d3                  | [1374]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1376]
	tst.w    %d2
	jeq      _PA_1375_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1375_:         
      move.w   %d1,_XDDA               | [1377]
      move.w   %d3,(-102,%a6)          | [1378]
      move.w   (-92,%a6),(-106,%a6)    | [1379]


      move.w   (-18,%a6),%d1           | [1382]
      move.w   %d1,(-16,%a6)           | [1383]







repexit1:
      btst     #4,(-96+1,%a6)          | [1392 EQ]
      jeq      repexit2                | [1393]
      movea.l  _SCRTCHP,%a0            | [1394]
|;;   adda.w    _SCRPT2,a0             | [1395]
	mvs.w    _SCRPT2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1396]
|;;   lea       2(a0,d7.w),a1          | [1397]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
      jbsr     outlin1                 | [1398]
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   _SCRPT2,%d0             | [1399]
|;;   add.w     -18(a6),d0             | [1400]
	move.w   (-18,%a6),%d1
	add.l    %d1,%d0
|;;   move.w    #15,_SOURCEX           | [1401]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      jra      repexit3                | [1402]
repexit2:
      move.w   _SCRPT2,%d0             | [1404]
      jra      repexit                 | [1405]
| 1901 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1408]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1409]
      movea.l  %a1,%a4                 | [1410]
      movea.l  %d7,%a5                 | [1411]
      move.w   %d5,(-44,%a6)           | [1412]
      moveq.l  #0,%d0                  | [1413]
      move.w   (-20,%a6),%d5           | [1414]
|;;   subq.w    #1,d5                  | [1415]
	subq.l   #1,%d5

      move.w   _XDDA,%d7               | [1417]
      jra      nextsrc                 | [1418]
innerlp:
|;;   ror.w     #1,d3                  | [1420 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1421]
nextsrc:
      movea.w  (%a0)+,%a2              | [1423]
reploop:
      move.w   %a2,%d6                 | [1425]
|;;   and.w     d3,d6                  | [1426 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1427]


repnor:
      tst.l    %d7                     | [1431 MI]
      jmi      repnorup                | [1432]
|;;   add.w     d2,d7                  | [1433 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1434]
      jra      ordone                  | [1435]
repnorup:
|;;   add.w     d2,d7                  | [1437 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1438]


|;;   ror.w     #1,d4                  | [1441 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1444]
      move.w   %d0,(%a1)+              | [1445]
      moveq.l  #0,%d0                  | [1446]


      jra      ordone                  | [1449]
nrepnor:


|;;   add.w     d2,d7                  | [1453 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1454]


|;;   or.w      d4,d0                  | [1457]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1458 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1460]
      move.w   %d0,(%a1)+              | [1461]
      moveq.l  #0,%d0                  | [1462]
o_no_rep:
      tst.l    %d7                     | [1464 PL]
      jpl      incsrc                  | [1465]


|;;   or.w      d4,d0                  | [1468]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1470 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1471]
nextdst:
      move.w   %d0,(%a1)+              | [1473]
      moveq.l  #0,%d0                  | [1474]
incsrc:
|;;   dbra      d5,innerlp             | [1476]
	tst.w    %d5
	jeq      _PA_1475_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1475_:         
repdone:
      move.w   %d0,(%a1)               | [1478]
|;;   movem.l   (sp)+,d3-d4/d6         | [1479]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1480]
      movea.l  %a4,%a1                 | [1481]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1482]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1483]
      move.w   (-44,%a6),%d5           | [1484]
      rts                              | [1485]
| 1989 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1488]
	ext.l    %d7
	lea      (0,%a0,%d7.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1490]
	ext.l    %d7
	lea      (0,%a1,%d7.l*1),%a2
|;;   lsr.w     #1,d7                  | [1491]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
|;;   subq.w    #1,d7                  | [1492]
	subq.l   #1,%d7
      move.w   %d7,(-44,%a6)           | [1493]
      move.w   (-106,%a6),%d6          | [1494]

      movea.l  %a0,%a5                 | [1496]
|;;   subq.w    #1,d6                  | [1497]
	subq.l   #1,%d6
out_edge:
      movea.w  %d6,%a4                 | [1499]
      movea.l  %a2,%a3                 | [1500]
      moveq.l  #0,%d5                  | [1501]
      moveq.l  #0,%d6                  | [1502]
      move.l   (%a2),%d1               | [1503]
      lsr.l    #1,%d1                  | [1504]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1509]
      move.b   %d5,%d0                 | [1510]
|;;   ror.l     #1,d0                  | [1511]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1513]
      move.b   %d6,%d2                 | [1514]
      move.l   %d2,%d3                 | [1515]
|;;   ror.l     #1,d3                  | [1516]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1517]
|;;   ror.l     #1,d4                  | [1518]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1520]
      move.l   %d0,%d6                 | [1521]
      eor.l    %d2,%d0                 | [1522]
      eor.l    %d3,%d5                 | [1523]
      eor.l    %d4,%d6                 | [1524]
|;;   rol.l     #1,d5                  | [1525]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1526]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1527]
      or.l     %d6,%d0                 | [1528]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1530]
      move.l   %d1,%d6                 | [1531]
      eor.l    %d2,%d1                 | [1532]
      eor.l    %d3,%d5                 | [1533]
      eor.l    %d4,%d6                 | [1534]
|;;   rol.l     #1,d5                  | [1535]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1536]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1537]
      or.l     %d5,%d0                 | [1538]
      or.l     %d6,%d0                 | [1539]

      eor.l    %d3,%d2                 | [1541]
      eor.l    %d3,%d4                 | [1542]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1543]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1544]
      or.l     %d4,%d0                 | [1545]
      swap     %d0                     | [1546]

      move.w   (%a1),%d6               | [1548]
      move.w   %d6,%d5                 | [1549]
|;;   eor.w     d0,d5                  | [1550]
	eor.l    %d0,%d5
|;;   and.w     d0,d5                  | [1551]
	and.l    %d0,%d5

      addq.l   #2,%a2                  | [1553]
      move.l   (%a2),%d1               | [1554]
      move.b   (-1,%a2),%d1            | [1555]
|;;   ror.l     #1,d1                  | [1556]
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1558]
      move.w   (%a0),%d5               | [1559]
      move.w   %d6,(%a0)+              | [1560]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1562]
	tst.w    %d7
	jeq      _PA_1561_
	subq.l   #1,%d7
	jra      out_loop
	_PA_1561_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1564]
      movea.l  %a5,%a0                 | [1565]
      move.w   %a4,%d6                 | [1566]
      movea.l  %a3,%a2                 | [1567]
      movea.l  %a2,%a1                 | [1568]
|;;   adda.w    -16(a6),a2             | [1569]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1570 NE]
      jne      srt_lin                 | [1571]
      movea.l  %a1,%a2                 | [1572]
srt_lin:
|;;   dbra      d6,out_edge            | [1574]
	tst.w    %d6
	jeq      _PA_1573_
	subq.l   #1,%d6
	jra      out_edge
	_PA_1573_:         
      rts                              | [1575]

| End section on aligned boundary
	.p2align 2,0
