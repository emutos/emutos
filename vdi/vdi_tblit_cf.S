
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
| Translated on Tue Oct 15 11:18:17 2019 by version 2.6.0 of PortAsm translator
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
| 100 "vdi/vdi_tblit.S"
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
| 239 "vdi/vdi_tblit.S"
|;;           .text                    | [31]






_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [39]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [40]
      movea.l  (44+8,%a7),%a0          | [41]
      movea.l  (44+12,%a7),%a1         | [42]
      jbsr     norm_blt                | [43]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [44]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [45]


_outline:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [49]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [50]
      movea.l  (44+8,%a7),%a0          | [51]
      move.w   (44+12,%a7),%d7         | [52]
      jbsr     outlin                  | [53]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [54]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [55]


_rotate:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [59]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [60]
      jbsr     rotation                | [61]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [62]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [63]


_scale:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [67]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [68]
      jbsr     replicat                | [69]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [70]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [71]

|;;           .text                    | [73]







|;;                   .even            | [81]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [82]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [83]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [84]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [85]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [86]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [87]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [88]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [89]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [90]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [91]
	.short	0x007F
|;;                   .dc.w 0x003F     | [92]
	.short	0x003F
|;;                   .dc.w 0x001F     | [93]
	.short	0x001F
|;;                   .dc.w 0x000F     | [94]
	.short	0x000F
|;;                   .dc.w 0x0007     | [95]
	.short	0x0007
|;;                   .dc.w 0x0003     | [96]
	.short	0x0003
|;;                   .dc.w 0x0001     | [97]
	.short	0x0001
|;;                   .dc.w 0x0000     | [98]
	.short	0x0000
| 332 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*3,4*3    | [101]
	.byte	4*0,4*0,4*3,4*3
|;;           .dc.b 4*4,4*4,4*7,4*7    | [102]
	.byte	4*4,4*4,4*7,4*7
|;;           .dc.b 4*6,4*6,4*6,4*6    | [103]
	.byte	4*6,4*6,4*6,4*6
|;;           .dc.b 4*1,4*1,4*13,4*13  | [104]
	.byte	4*1,4*1,4*13,4*13

|;;           .dc.b 4*0,4*15,4*0,4*15  | [106]
	.byte	4*0,4*15,4*0,4*15
|;;           .dc.b 4*0,4*14,4*1,4*15  | [107]
	.byte	4*0,4*14,4*1,4*15
|;;           .dc.b 4*0,4*13,4*2,4*15  | [108]
	.byte	4*0,4*13,4*2,4*15
|;;           .dc.b 4*0,4*12,4*3,4*15  | [109]
	.byte	4*0,4*12,4*3,4*15
|;;           .dc.b 4*0,4*11,4*4,4*15  | [110]
	.byte	4*0,4*11,4*4,4*15
|;;           .dc.b 4*0,4*10,4*5,4*15  | [111]
	.byte	4*0,4*10,4*5,4*15
|;;           .dc.b 4*0,4*9,4*6,4*15   | [112]
	.byte	4*0,4*9,4*6,4*15
|;;           .dc.b 4*0,4*8,4*7,4*15   | [113]
	.byte	4*0,4*8,4*7,4*15
|;;           .dc.b 4*0,4*7,4*8,4*15   | [114]
	.byte	4*0,4*7,4*8,4*15
|;;           .dc.b 4*0,4*6,4*9,4*15   | [115]
	.byte	4*0,4*6,4*9,4*15
|;;           .dc.b 4*0,4*5,4*10,4*15  | [116]
	.byte	4*0,4*5,4*10,4*15
|;;           .dc.b 4*0,4*4,4*11,4*15  | [117]
	.byte	4*0,4*4,4*11,4*15
|;;           .dc.b 4*0,4*3,4*12,4*15  | [118]
	.byte	4*0,4*3,4*12,4*15
|;;           .dc.b 4*0,4*2,4*13,4*15  | [119]
	.byte	4*0,4*2,4*13,4*15
|;;           .dc.b 4*0,4*1,4*14,4*15  | [120]
	.byte	4*0,4*1,4*14,4*15
|;;           .dc.b 4*0,4*0,4*15,4*15  | [121]
	.byte	4*0,4*0,4*15,4*15

toptable:
|;;           .dc.l top0,top1,top2,top3,top4,top5,top6,top7 | [124]
	.long	top0,top1,top2,top3,top4,top5,top6,top7
|;;           .dc.l top8,top9,topa,topb,topc,topd,tope,topf | [125]
	.long	top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
|;;           .dc.l twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7 | [128]
	.long	twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
|;;           .dc.l twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf | [129]
	.long	twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

|;;           .text                    | [131]
| 376 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [134]
|;;   sub.w     -24(a6),d1             | [135]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [136 PL]
      jpl      do_rot                  | [137]



|;;   neg.w     d1                     | [141]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [142]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [143]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [145]

|;;   add.w     d0,d0                  | [147]
	move.w   %d0,%d5
	add.l    %d0,%d5
      lea      lf_tab,%a2              | [148]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [149]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [150]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [151]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [156]
|;;   add.w     d0,d0                  | [157]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [158]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [160]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [161]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [164]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [165]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [166]
	andi.l   #(0xFFFF&0xF),%d5
|;;   add.w     d2,d2                  | [167]
	add.l    %d5,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [168]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [169]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [170]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [171]
      move.w   #0x8000,(-80,%a6)       | [172]
      moveq.l  #(-1),%d3               | [173]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [176 HI]
      jhi      doub_des                | [177]



|;;   add.w     d0,d0                  | [181]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [182]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [183]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [184]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [186]
	move.w   %d5,%d1
      jra      msk_done                | [187]



doub_des:
      move.w   %d0,%d1                 | [192]
|;;   lsr.w     #4,d1                  | [193]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [194]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [195 NE]
      jne      mlt_dest                | [196]



      move.w   (-24,%a6),%d1           | [200]
|;;   add.w     -20(a6),d1             | [201]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [202 CC]
      jcc      mlt_dest                | [203]
      moveq.l  #0,%d7                  | [204]
      jra      do_ritem                | [205]

mlt_dest:
      tst.w    (-42,%a6)               | [208 LT]
      jlt      mltleft                 | [209]
      moveq.l  #4,%d7                  | [210]
      jra      do_ritem                | [211]
mltleft:
      moveq.l  #8,%d7                  | [213]
do_ritem:
|;;   andi.w    #0x000F,d0             | [215 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [216]
|;;   subq.w    #1,d3                  | [217]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [218]
	move.w   %d5,%d3
not_null:
|;;   add.w     d0,d0                  | [220]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [221]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [222]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [225]
      move.w   %d3,(-28,%a6)           | [226]
      move.w   %d4,(-36,%a6)           | [227]
|;;   addq.w    #2,d3                  | [228]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [229 CS]
      jcs      msk0                    | [230]
|;;   addq.w    #1,d3                  | [231]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [233]
      move.w   %d3,(-30,%a6)           | [234]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5

plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [237]
|;;   lsr.w     -84(a6)                | [238 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   addx.w    d0,d0                  | [239]
	move.w   %d0,%d1
	addx.l   %d0,%d1
|;;   lsr.w     -76(a6)                | [240 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   addx.w    d0,d0                  | [241]
	addx.l   %d1,%d1



      lea      toptable,%a4            | [245]
	move.w   %d1,%d0
|;;   move.b    wrmappin-toptable(a4,d0.w),d0 | [246]
	move.w   %d0,%a5
	move.b   (wrmappin-toptable,%a4,%a5.l*1),%d0
|;;   movea.l   toptable-toptable(a4,d0.w),a2 | [247]
	move.w   %d0,%a5
	movea.l  (toptable-toptable,%a4,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [248]
      movea.l  %a2,%a5                 | [249]

|;;   movea.l   twoptble-toptable(a4,d0.w),a4 | [251]
	move.w   %d0,%a0
	movea.l  (twoptble-toptable,%a4,%a0.l*1),%a4



      move.w   (-96,%a6),%d0           | [255 EQ]
      jeq      wrt_char                | [256]

      btst     #1,%d0                  | [258 EQ]
      jeq      no_lite                 | [259]



|;;   move.w    _LITEMASK,-78(a6)      | [263]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [264]
      lea      liteop,%a5              | [265]

      move.l   %a2,(-64,%a6)           | [267]
      lea      liteopwf,%a2            | [268]

      move.l   %a4,(-72,%a6)           | [270]
      lea      liteopw,%a4             | [271]

no_lite:
      btst     #0,%d0                  | [274 EQ]
      jeq      no_thick                | [275]



      clr.w    (-82,%a6)               | [279]

      move.l   %a5,(-48,%a6)           | [281]
      lea      thknop,%a5              | [282]

      move.l   %a2,(-60,%a6)           | [284]
      lea      thknopwf,%a2            | [285]

      move.l   %a4,(-68,%a6)           | [287]
      lea      thknopw,%a4             | [288]

no_thick:
      btst     #2,%d0                  | [291 EQ]
      jeq      wrt_char                | [292]



|;;   move.w    _SKEWMASK,-80(a6)      | [296]
	move.w   _SKEWMASK,%d1
	move.w   %d1,(-80,%a6)
      move.l   %a5,(-56,%a6)           | [297]
      lea      skewop,%a5              | [298]

      tst.w    (-124,%a6)              | [300 EQ GT]
      jgt      wrt_char                | [301]
      jeq      not_sngl                | [302]
      clr.w    (-28,%a6)               | [303]
      moveq.l  #0,%d7                  | [304]
      jra      chng_blt                | [305]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [307 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [308]

      tst.w    (-42,%a6)               | [310 LT]
      jlt      chng_left               | [311]
      moveq.l  #4,%d7                  | [312]
      jra      chng_blt                | [313]
chng_left:
      moveq.l  #8,%d7                  | [315]
chng_blt:
      move.w   %d7,(-124,%a6)          | [317]

wrt_char:
      move.w   (-124,%a6),%d7          | [320 LE]
      jle      set_chr_hgt             | [321]
      movea.l  %a2,%a5                 | [322]
      move.w   (-18,%a6),%d5           | [323]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [325]
|;;   subq.w    #1,d3                  | [326]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [328]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [329]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [331 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [332]
      movea.l  (-10,%a6),%a0           | [333]
      movea.l  (-14,%a6),%a1           | [334]
      addq.l   #2,%a1                  | [335]
      move.l   %a1,(-14,%a6)           | [336]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [337 EQ]
      jeq      plane_loop              | [338]
      jra      norm_blt                | [339]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [341]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [343]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [349]
      move.w   (-16,%a6),%d6           | [350]
      move.w   (-18,%a6),%d7           | [351]
      lea      sgl_rtn,%a3             | [352]
      jra      sgl_1st_scan            | [353]

sgl_lp:
|;;   adda.w    d6,a0                  | [356]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [357]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [359]
| 611 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [361]


      move.w   (-42,%a6),%d1           | [364 MI]
      jmi      left_rol                | [365]
      lsr.l    %d1,%d0                 | [366]
      jra      end_ro                  | [367]
left_rol:
      lsl.l    %d1,%d0                 | [369]
end_ro:
      swap     %d0                     | [371]
      move.w   %d0,%d1                 | [372]
      swap     %d0                     | [373]

      jmp      (%a5)                   | [375]

sgl_rtn:
      move.w   %d1,(%a1)               | [378]
|;;   dbra      d3,sgl_lp              | [379]
	tst.w    %d3
	jeq      _PA_378_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_378_:         
      jra      next_plane              | [380]




dbl_loop:
      movea.l  %a3,%a4                 | [386]
      move.w   (-16,%a6),%d6           | [387]
      move.w   (-18,%a6),%d7           | [388]
      lea      dbl_2wrd,%a2            | [389]
      move.l   %a2,%d5                 | [390]
      lea      dbl_1wrd,%a3            | [391]
      jra      dbl_1st_scan            | [392]

dbl_lp:
|;;   adda.w    d6,a0                  | [395]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [396]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [397]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [399]
| 658 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [401]


      move.w   (-42,%a6),%d1           | [404 MI]
      jmi      dbleft                  | [405]
      lsr.l    %d1,%d0                 | [406]
      jra      dblendr                 | [407]
dbleft:
      lsl.l    %d1,%d0                 | [409]
dblendr:
      swap     %d0                     | [411]
      move.w   %d0,%d1                 | [412]
      swap     %d0                     | [413]
      move.w   (-38,%a6),%d2           | [414]

      jmp      (%a5)                   | [416]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [419]
      move.w   %d0,%d1                 | [420]

      movea.w  (-88,%a6),%a2           | [422]
|;;   move.w    (a1,a2.w),d4           | [423]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [425]

|;;   exg       a3,d5                  | [427]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [428]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [431]
|;;   move.w    d1,(a1,a2.w)           | [432]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [434]
	tst.w    %d3
	jeq      _PA_433_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_433_:         
      jra      next_plane              | [435]





left_loop:
      movea.l  %d6,%a0                 | [442]
      movea.l  %d7,%a1                 | [443]
|;;   adda.w    -16(a6),a0             | [444]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [445]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [446]
|;;   rol.w     -78(a6)                | [447]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [449 NE]
      jne      skewopw                 | [450]
mlt_left:
      move.l   %a0,%d6                 | [452]
      move.l   %a1,%d7                 | [453]
      move.w   (-28,%a6),(-44,%a6)     | [454]







      move.l   (%a0)+,%d0              | [462]


      move.w   (%a1),%d4               | [465]

      move.w   (-42,%a6),%d1           | [467]
      lsl.l    %d1,%d0                 | [468]

      swap     %d0                     | [470]
      move.w   %d0,%d1                 | [471]
      swap     %d0                     | [472]

      move.w   (-38,%a6),%d2           | [474]

      lea      lft_lfrng,%a3           | [476]
      jmp      (%a5)                   | [477]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [480]
|;;   adda.w    -88(a6),a1             | [481]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [483]
      lea      lft_wrds,%a3            | [484]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [489]
|;;   and.w     d2,d4                  | [490]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [491]





      swap     %d0                     | [497]
      move.w   %d1,%d0                 | [498]
      swap     %d0                     | [499]

      move.w   (-42,%a6),%d1           | [501]
      lsl.l    %d1,%d0                 | [502]

      swap     %d0                     | [504]
      move.w   %d0,%d1                 | [505]
      swap     %d0                     | [506]

|;;   not.w     d2                     | [508]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [509]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [510]
	not.l    %d5
|;;   eor.w     d4,d1                  | [511]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [513]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [515 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [516]

      jmp      (%a4)                   | [518]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [521]
|;;   adda.w    -88(a6),a1             | [522]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [524]

lef_don:
      move.w   (-36,%a6),%d2           | [527]

      lea      lft_rfrng,%a3           | [529]
      jmp      (%a5)                   | [530]

lft_rfrng:
      move.w   %d1,(%a1)               | [533]

|;;   dbra      d3,left_loop           | [535]
	tst.w    %d3
	jeq      _PA_534_
	subq.l   #1,%d3
	jra      left_loop
	_PA_534_:         
      jra      next_plane              | [536]





rite_loop:
      movea.l  %d6,%a0                 | [543]
      movea.l  %d7,%a1                 | [544]
|;;   adda.w    -16(a6),a0             | [545]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [546]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [547]
|;;   rol.w     -78(a6)                | [548]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [550 NE]
      jne      skewopw                 | [551]
mlt_rite:
      move.l   %a0,%d6                 | [553]
      move.l   %a1,%d7                 | [554]
      move.w   (-28,%a6),(-44,%a6)     | [555]
      move.w   (%a0)+,%d0              | [556]



      swap     %d0                     | [560]

      move.w   (%a1),%d4               | [562]

      move.w   (-42,%a6),%d1           | [564]
      lsr.l    %d1,%d0                 | [565]
      swap     %d0                     | [566]
      move.w   %d0,%d1                 | [567]
      swap     %d0                     | [568]

      move.w   (-38,%a6),%d2           | [570]

      lea      rgt_lfrng,%a3           | [572]
      jmp      (%a5)                   | [573]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [576]
|;;   adda.w    -88(a6),a1             | [577]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [579]
      lea      rgt_wrds,%a3            | [580]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [585]
|;;   and.w     d2,d4                  | [586]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [587]
      move.w   (%a0)+,%d0              | [588]



      swap     %d0                     | [592]

      move.w   (-42,%a6),%d1           | [594]
      lsr.l    %d1,%d0                 | [595]
      swap     %d0                     | [596]
      move.w   %d0,%d1                 | [597]
      swap     %d0                     | [598]

|;;   not.w     d2                     | [600]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [601]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [602]
	not.l    %d5
|;;   eor.w     d4,d1                  | [603]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [605]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [607 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [608]

      jmp      (%a4)                   | [610]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [613]
|;;   adda.w    -88(a6),a1             | [614]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [616]

rite_don:
      move.w   (-36,%a6),%d2           | [619]

      lea      rgt_rfrng,%a3           | [621]
      jmp      (%a5)                   | [622]

rgt_rfrng:
      move.w   %d1,(%a1)               | [625]

|;;   dbra      d3,rite_loop           | [627]
	tst.w    %d3
	jeq      _PA_626_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_626_:         
      jra      next_plane              | [628]
| 900 "vdi/vdi_tblit.S"
top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [631]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [632]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [633]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [634]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [635]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [638]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [639]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [640]
	not.l    %d0
|;;   and.w     d4,d1                  | [641]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [642]

top2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [645]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [646]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [647]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [648]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [649]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [652]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [653]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [654]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [655]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [658]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [659]
	not.l    %d0
|;;   and.w     d4,d1                  | [660]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [661]

top5:
      move.w   %d4,%d1                 | [664]
      jmp      (%a3)                   | [665]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [668]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [669]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [670]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [673]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [674]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [675]

top8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [678]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [679]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [680]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [681]

top9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [684]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [685]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [686]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [687]

topa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [690]
|;;   eor.w     d2,d1                  | [691]
	move.w   %d1,%d0
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [692]

topb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d2,d4                  | [695]
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   and.w     d2,d1                  | [696]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [697]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [698]

topc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [701]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [702]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [703]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [704]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [705]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [708]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [709]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [710]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [711]

tope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [714]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [715]
	and.l    %d2,%d0
|;;   eor.w     d2,d4                  | [716]
	move.w   %d0,%d1
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   or.w      d4,d1                  | [717]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [718]

topf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d2,d4                  | [721]
	mvz.w    %d2,%d0
	or.l     %d0,%d4
      move.w   %d4,%d1                 | [722]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [723]
| 1006 "vdi/vdi_tblit.S"
twop0:
      moveq.l  #0,%d1                  | [726]
      jmp      (%a3)                   | [727]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [730]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [731]

twop2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [734]
	move.w   %d4,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [735]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [736]

twop3:
      jmp      (%a3)                   | [739]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [742]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [743]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [744]

twop5:
      move.w   %d4,%d1                 | [747]
      jmp      (%a3)                   | [748]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [751]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [752]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [755]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [756]

twop8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [759]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|;;   not.w     d1                     | [760]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [761]

twop9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [764]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   not.w     d1                     | [765]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [766]

twopa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [769]
|;;   not.w     d1                     | [770]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [771]

twopb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [774]
	move.w   %d4,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [775]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [776]

twopc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [779]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [780]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [783]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [784]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [785]

twope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [788]
	move.w   %d1,%d0
	and.l    %d4,%d0
|;;   not.w     d1                     | [789]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [790]

twopf:
      moveq.l  #(-1),%d1               | [793]
      jmp      (%a3)                   | [794]
| 1093 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [797]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [798]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [799]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [800 EQ]
      jeq      thk00                   | [801]
|;;   ori.b     #0x10,ccr              | [802 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [803 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [804]
      move.w   #0x8000,%d6             | [805]
thk01:
      move.w   %d6,(-34,%a6)           | [807]
thk00:
      tst.w    (-28,%a6)               | [809 MI]
      jmi      thk0                    | [810]
      cmp.w    (-36,%a6),%d6           | [811 CC]
      jcc      thk0                    | [812]
|;;   and.w     d6,d0                  | [813]
	and.l    %d6,%d0
      jra      thk1                    | [814]
thk0:
      clr.w    %d0                     | [816]
|;;   and.w     d6,d1                  | [817]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [819]
      move.w   %d2,%d5                 | [820]
      swap     %d5                     | [821]
      move.w   (-36,%a6),%d5           | [822]
      lsl.l    %d6,%d5                 | [823]

      swap     %d5                     | [825]
      move.w   %d5,%d7                 | [826]
      swap     %d5                     | [827]

|;;   and.w     d7,d1                  | [829]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [830]
	and.l    %d5,%d0
|;;   swap      d1                     | [831]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [832]
|;;   subq.w    #1,d6                  | [833]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [835]
      lsr.l    #1,%d0                  | [836]
      or.l     %d0,%d1                 | [837]
|;;   dbra      d6,thkoplp             | [838]
	tst.w    %d6
	jeq      _PA_837_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_837_:         

      move.w   %d1,%d0                 | [840]
      swap     %d1                     | [841]
|;;   movem.l   (sp)+,d5-d7            | [842]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [843]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [844]
| 1156 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [847]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [848 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [849]
|;;   and.w     -34(a6),d1             | [850]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [852]
      swap     %d1                     | [853]
      clr.w    %d1                     | [854]
      move.l   %d1,%d5                 | [855]
|;;   subq.w    #1,d6                  | [856]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [858]
      or.l     %d5,%d1                 | [859]
|;;   dbra      d6,thkopwlp            | [860]
	tst.w    %d6
	jeq      _PA_859_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_859_:         

      move.w   %d1,%d5                 | [862]
      swap     %d1                     | [863]
|;;   or.w      -82(a6),d1             | [864]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [865]
|;;   movem.l   (sp)+,d5-d6            | [866]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [867]
      jmp      (%a2)                   | [868]
| 1194 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [871]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [872]
|;;   subq.w    #1,-30(a6)             | [873 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [874]
      jeq      thk4                    | [875]

|;;   subq.w    #1,-30(a6)             | [877 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [878]
|;;   and.w     d6,d1                  | [879]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [881]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [882]

thk3:
      clr.w    %d1                     | [885]
      jra      thk5                    | [886]

thk4:
|;;   and.w     d6,d1                  | [889]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [892 MI]
      jmi      thk6                    | [893]
      move.w   (-28,%a6),%d5           | [894]
|;;   addq.w    #2,d5                  | [895]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [896]
|;;   ori.b     #0x10,ccr              | [897 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [898 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [899]
      move.w   #0x8000,%d7             | [900]
|;;   addq.w    #1,d5                  | [901]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [903 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [904]
|;;   subq.w    #1,d5                  | [905]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [907 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [908 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [909]
      move.w   #0x8000,%d6             | [910]
thk31:
      cmp.w    %d6,%d7                 | [912 CS]
      jcs      thk35                   | [913]
|;;   addq.w    #1,d5                  | [914]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [916]
      move.w   %d5,(-32,%a6)           | [917]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [919]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [921]
      swap     %d1                     | [922]
      clr.w    %d1                     | [923]
      move.l   %d1,%d5                 | [924]
|;;   subq.w    #1,d6                  | [925]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [927]
      or.l     %d5,%d1                 | [928]
|;;   dbra      d6,thkopwl             | [929]
	tst.w    %d6
	jeq      _PA_928_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_928_:         

      move.w   %d1,%d5                 | [931]
      swap     %d1                     | [932]
|;;   or.w      -82(a6),d1             | [933]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [934]
|;;   and.w     d2,d1                  | [935]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [936]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [937]
      jmp      (%a2)                   | [938]
| 1275 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [941]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [942]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [943]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [944]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [945]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [948]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [949]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [950]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [953]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [954]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [955]
| 1307 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [958 CC]
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
      jcc      no_shift                | [959]

      swap     %d1                     | [961]
      move.w   %d0,%d1                 | [962]
      lsr.l    #1,%d1                  | [963]

      swap     %d2                     | [965]
      move.w   (-36,%a6),%d2           | [966]
      lsr.l    #1,%d2                  | [967]
      move.w   %d2,(-36,%a6)           | [968]
      swap     %d2                     | [969]
      move.w   %d2,(-38,%a6)           | [970 EQ]
      jeq      nxt_word                | [971]
      move.w   (-42,%a6),%d0           | [972 MI]
      jmi      dec_rol                 | [973]
ror_add:
|;;   addq.w    #1,d0                  | [975]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [977]
      move.w   %d1,%d0                 | [978]
      swap     %d1                     | [979]
no_shift:
      movea.l  (-56,%a6),%a2           | [981]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [982]

dec_rol:
      tst.b    %d0                     | [985 EQ]
      jeq      begn_ror                | [986]
|;;   subq.w    #1,d0                  | [987]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [988]
begn_ror:
      clr.w    %d0                     | [990]
      jra      ror_add                 | [991]



nxt_word:
      move.w   %d2,(-36,%a6)           | [996]
      swap     %d2                     | [997]
      move.w   %d2,(-38,%a6)           | [998]
|;;   adda.w    -88(a6),a1             | [999]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1000]

      moveq.l  #15,%d0                 | [1002]
|;;   sub.w     -42(a6),d0             | [1003]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1004]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1005]
      jra      no_shift                | [1006]






skewopw:
|;;   rol.w     -80(a6)                | [1014 CC]
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
      jcc      do_shift                | [1015]

      tst.w    (-42,%a6)               | [1017 MI]
      jmi      mlt_left                | [1018]
      jra      mlt_rite                | [1019]
do_shift:
|;;   ori       #0x10,ccr              | [1021 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1022]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1024]
      cmp.w    #0xFFFF,%d0             | [1025 EQ]
      jeq      inc_rite                | [1026]

|;;   ori       #0x10,ccr              | [1028 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1029]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1030]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1033]
      tst.b    %d0                     | [1034 NE]
      jne      no_rota                 | [1035]
      move.w   #0x8000,(-40,%a6)       | [1036]
no_rota:
      move.w   (-38,%a6),%d1           | [1038]
|;;   lsr.w     #1,d1                  | [1039 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [1040]

      move.w   %d1,(-38,%a6)           | [1042]

      tst.w    %d0                     | [1044 MI]
      jmi      wdec_rol                | [1045]
|;;   addq.w    #1,-42(a6)             | [1046]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [1047]
wdec_rol:
      tst.b    %d0                     | [1049 EQ]
      jeq      set_msk                 | [1050]
|;;   subq.w    #1,-42(a6)             | [1051]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [1052]
set_msk:
      move.w   #1,(-42,%a6)            | [1054]
      jra      mlt_rite                | [1055]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1058]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1059]
      jra      do_left                 | [1060]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1063]

|;;   subq.w    #1,-28(a6)             | [1065]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1066]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1068]
|;;   sub.w     d0,d2                  | [1069]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1070]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1071]

      jra      mlt_left                | [1073]




rotation:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   _SOURCEX,%d1            | [1079]
      move.w   %d1,%d2                 | [1080]
|;;   andi.w    #0x0F,d2               | [1081]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1082]
|;;   lsr.w     #4,d1                  | [1083]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1084]
	add.l    %d1,%d1

      movea.l  (-10,%a6),%a0           | [1086]
|;;   adda.w    d1,a0                  | [1087]
	mvs.w    %d1,%d0
	adda.l   %d0,%a0

      jbsr     getnextbuf              | [1089]

      move.w   (-102,%a6),(-20,%a6)    | [1091]
      move.w   _SOURCEY,%d0            | [1092]
      move.w   (-106,%a6),%d1          | [1093]
      move.w   %d1,(-22,%a6)           | [1094]
      move.w   (-16,%a6),%d2           | [1095]

      move.w   (-110,%a6),%d6          | [1097]
      cmp.w    #1800,%d6               | [1098 EQ]
      jeq      upsd_dwn                | [1099]
rot90:
      cmp.w    #900,%d6                | [1101 EQ]
      jeq      top_src                 | [1102]
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

      cmp.w    #2700,%d6               | [1115 EQ]
      jeq      top_dwn                 | [1116]
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
|;;   cmpi.w    #900,-110(a6)          | [1180 NE]
	move.w   (-110,%a6),%d5
	cmp.w    #900,%d5
      jne      rot_nneg                | [1181]
|;;   neg.w     d0                     | [1182]
	neg.l    %d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1184]
      clr.w    _SOURCEX                | [1185]
      jbsr     currbuf                 | [1186]
      move.l   %a1,(-10,%a6)           | [1187]
      clr.w    _SOURCEY                | [1188]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      rts                              | [1189]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1192]
|;;   add.w     -24(a6),d0             | [1193]
	move.w   (-24,%a6),%d5
	add.l    %d5,%d0
|;;   subq.w    #1,d0                  | [1194]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1195]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   add.w     d0,d0                  | [1196]
	add.l    %d0,%d0
|;;   addq.w    #2,d0                  | [1197]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1199]
      move.w   %d0,%d2                 | [1200]
|;;   lsr.w     #1,d2                  | [1201]
	and.l    #0xffff,%d2
	lsr.l    #1,%d2
|;;   subq.w    #1,d2                  | [1202]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1203]
|;;   adda.w    d0,a1                  | [1204]
	ext.l    %d0
	adda.l   %d0,%a1
      jra      strtflip                | [1205]
upsd_lp:
      movea.l  %a0,%a2                 | [1207]
      move.w   %d2,%d3                 | [1208]
line_lp:
      move.w   (%a2)+,%d0              | [1210]
      moveq.l  #0,%d6                  | [1211]
      moveq.l  #15,%d4                 | [1212]
flip_lp:
|;;   lsr.w     #1,d0                  | [1214 X]
	and.l    #0xffff,%d0
	lsr.l    #1,%d0
|;;   addx.w    d6,d6                  | [1215]
	addx.l   %d6,%d6
|;;   dbra      d4,flip_lp             | [1216]
	subq.l   #1,%d4
	jpl      flip_lp

      move.w   %d6,-(%a1)              | [1218]
|;;   dbra      d3,line_lp             | [1219]
	tst.w    %d3
	jeq      _PA_1218_
	subq.l   #1,%d3
	jra      line_lp
	_PA_1218_:         

|;;   adda.w    -16(a6),a0             | [1221]
	mvs.w    (-16,%a6),%d5
	adda.l   %d5,%a0
strtflip:
|;;   dbra      d1,upsd_lp             | [1223]
	tst.w    %d1
	jeq      _PA_1222_
	subq.l   #1,%d1
	jra      upsd_lp
	_PA_1222_:         

      move.w   (-18,%a6),(-16,%a6)     | [1225]
      jbsr     currbuf                 | [1226]
      move.l   %a1,(-10,%a6)           | [1227]
      move.w   _SOURCEX,%d0            | [1228]
|;;   add.w     -102(a6),d0            | [1229]
	move.w   (-102,%a6),%d5
	add.l    %d5,%d0
|;;   neg.w     d0                     | [1230]
	neg.l    %d0
|;;   andi.w    #0xF,d0                | [1231]
	andi.l   #(0xFFFF&0xF),%d0
      move.w   %d0,_SOURCEX            | [1232]
      clr.w    _SOURCEY                | [1233]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      rts                              | [1234]
| 1594 "vdi/vdi_tblit.S"
getnextbuf:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   %d0,-(%a7)              | [1237]
      movea.l  _SCRTCHP,%a1            | [1238]
      move.w   _SCRPT2,%d0             | [1239]
|;;   sub.w     -2(a6),d0              | [1240]
	move.w   (-2,%a6),%d1
	sub.l    %d1,%d0
      move.w   %d0,(-2,%a6)            | [1241]
|;;   adda.w    d0,a1                  | [1242]
	ext.l    %d0
	adda.l   %d0,%a1
      move.w   (%a7)+,%d0              | [1243]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1244]

currbuf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      movea.l  _SCRTCHP,%a1            | [1247]
|;;   adda.w    -2(a6),a1              | [1248]
	mvs.w    (-2,%a6),%d0
	adda.l   %d0,%a1
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      rts                              | [1249]






replicat:
|***	SAVE	d7	***
	lea      -12(%a7),%a7
	movem.l  %d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1257]
      move.w   %d0,%d4                 | [1258]
|;;   andi.w    #0x0F,d0               | [1259]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1260]
|;;   lsr.w     #4,d4                  | [1261]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   add.w     d4,d4                  | [1262]
	add.l    %d4,%d4

      move.w   _SOURCEY,%d0            | [1264]
      mulu.w   (-16,%a6),%d0           | [1265]
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1266]
|;;   adda.w    d4,a0                  | [1267]
	mvs.w    %d4,%d3
	adda.l   %d3,%a0
      adda.l   %d0,%a0                 | [1268]

      move.w   (-24,%a6),%d2           | [1270]
      move.w   #0x8000,%d3             | [1271]
      move.w   %d3,%d4                 | [1272]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1273]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1275]
      move.w   (-102,%a6),%d1          | [1276]

      move.w   %d2,(-22,%a6)           | [1278]
      move.w   %d1,(-20,%a6)           | [1279]
      jbsr     getnextbuf              | [1280]

      btst     #4,(-96+1,%a6)          | [1282 EQ]
      jeq      noline                  | [1283]



|;;   addq.w    #1,-106(a6)            | [1287]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1288]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   add.w     d1,d1                  | [1290]
	add.l    %d1,%d1
|;;   addq.w    #3,d1                  | [1291]
	addq.l   #3,%d1
|;;   lsr.w     #4,d1                  | [1292]
	and.l    #0xffff,%d1
	lsr.l    #4,%d1
|;;   add.w     d1,d1                  | [1293]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1294]
	addq.l   #2,%d1
      move.w   %d1,%d5                 | [1295]
      moveq.l  #0,%d6                  | [1296]
oklear:
      move.w   %d6,(%a1)+              | [1298]
|;;   dbra      d5,oklear              | [1299]
	tst.w    %d5
	jeq      _PA_1298_
	subq.l   #1,%d5
	jra      oklear
	_PA_1298_:         
      lea      (-2,%a1),%a2            | [1300]
      move.w   %d2,%d5                 | [1301]
|;;   add.w     d5,d5                  | [1302]
	add.l    %d5,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1304]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1305]
|;;   dbra      d5,oklear1             | [1306]
	tst.w    %d5
	jeq      _PA_1305_
	subq.l   #1,%d5
	jra      oklear1
	_PA_1305_:         

|;;   move.w    d1,d5                  | [1308]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1309]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   subq.w    #1,d5                  | [1310 X]
	subq.l   #1,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1312]
|;;   dbra      d5,oklear3             | [1313]
	move     %CCR,%d0
	tst.w    %d5
	jeq      _PA_1312_
	subq.l   #1,%d5
	move     %d0,%CCR
	jra      oklear3
	_PA_1312_:         
	move     %d0,%CCR
      jra      noline1                 | [1314]

noline:
|;;   lsr.w     #3,d1                  | [1317]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   add.w     d1,d1                  | [1318]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1319 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1321]
|***	RESTORE	AFTER d7	***
	movem.l  8(%a7),%d7



      move.w   _SCALDIR,%d7            | [1325]
|;;   roxr.l    #1,d7                  | [1326 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1327]
|;;   roxr.l    #1,d7                  | [1328]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1330]
      move.w   _DDAINC,%d2             | [1331]
      move.w   (-22,%a6),%d5           | [1332]
|;;   subq.w    #1,d5                  | [1333]
	subq.l   #1,%d5
      move.w   #32767,%d6              | [1334]
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      12(%a7),%a7
      tst.l    %d7                     | [1335 MI]
      jmi      rep_ylop                | [1336]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1338 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1339]
      jbsr     yloop                   | [1340]
y_no_drw:
|;;   adda.w    d7,a0                  | [1342]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1343]
	tst.w    %d5
	jeq      _PA_1342_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1342_:         
      jra      y_rep_don               | [1344]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1346 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1347]
      jbsr     yloop                   | [1348]
y_no_rep:
      jbsr     yloop                   | [1350]
|;;   adda.w    d7,a0                  | [1351]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1352]
	tst.w    %d5
	jeq      _PA_1351_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1351_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1354]
|;;   subq.w    #1,d2                  | [1355]
	subq.l   #1,%d2
      move.w   _XDDA,%d1               | [1356]
      move.w   _DDAINC,%d0             | [1357]
      moveq.l  #0,%d3                  | [1358]
repwidcl:
|;;   add.w     d0,d1                  | [1360 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1361]
|;;   addq.w    #1,d3                  | [1362]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1364 PL]
      jpl      nrpndoub                | [1365]
|;;   addq.w    #1,d3                  | [1366]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1368]
	tst.w    %d2
	jeq      _PA_1367_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1367_:         
      move.w   %d1,_XDDA               | [1369]
      move.w   %d3,(-102,%a6)          | [1370]
      move.w   (-92,%a6),(-106,%a6)    | [1371]


      move.w   (-18,%a6),%d1           | [1374]
      move.w   %d1,(-16,%a6)           | [1375]







repexit1:
      btst     #4,(-96+1,%a6)          | [1384 EQ]
      jeq      repexit2                | [1385]
      jbsr     currbuf                 | [1386]
      movea.l  %a1,%a0                 | [1387]
      move.w   %d1,%d7                 | [1388]
|;;   lea       2(a0,d7.w),a1          | [1389]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
      jbsr     outlin1                 | [1390]
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   (-2,%a6),%d0            | [1391]
|;;   add.w     -18(a6),d0             | [1392]
	move.w   (-18,%a6),%d1
	add.l    %d1,%d0
|;;   move.w    #15,_SOURCEX           | [1393]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      jra      repexit3                | [1394]
repexit2:
      move.w   (-2,%a6),%d0            | [1396]
      clr.w    _SOURCEX                | [1397]
repexit3:
      movea.l  _SCRTCHP,%a2            | [1399]
|;;   adda.w    d0,a2                  | [1400]
	ext.l    %d0
	adda.l   %d0,%a2
      move.l   %a2,(-10,%a6)           | [1401]
      clr.w    _SOURCEY                | [1402]
      rts                              | [1403]
| 1792 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1406]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1407]
      movea.l  %a1,%a4                 | [1408]
      movea.l  %d7,%a5                 | [1409]
      move.w   %d5,(-44,%a6)           | [1410]
      moveq.l  #0,%d0                  | [1411]
      move.w   (-20,%a6),%d5           | [1412]
|;;   subq.w    #1,d5                  | [1413]
	subq.l   #1,%d5

      move.w   _XDDA,%d7               | [1415]
      jra      nextsrc                 | [1416]
innerlp:
|;;   ror.w     #1,d3                  | [1418 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1419]
nextsrc:
      movea.w  (%a0)+,%a2              | [1421]
reploop:
      move.w   %a2,%d6                 | [1423]
|;;   and.w     d3,d6                  | [1424 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1425]


repnor:
      tst.l    %d7                     | [1429 MI]
      jmi      repnorup                | [1430]
|;;   add.w     d2,d7                  | [1431 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1432]
      jra      ordone                  | [1433]
repnorup:
|;;   add.w     d2,d7                  | [1435 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1436]


|;;   ror.w     #1,d4                  | [1439 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1442]
      move.w   %d0,(%a1)+              | [1443]
      moveq.l  #0,%d0                  | [1444]


      jra      ordone                  | [1447]
nrepnor:


|;;   add.w     d2,d7                  | [1451 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1452]


|;;   or.w      d4,d0                  | [1455]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1456 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1458]
      move.w   %d0,(%a1)+              | [1459]
      moveq.l  #0,%d0                  | [1460]
o_no_rep:
      tst.l    %d7                     | [1462 PL]
      jpl      incsrc                  | [1463]


|;;   or.w      d4,d0                  | [1466]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1468 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1469]
nextdst:
      move.w   %d0,(%a1)+              | [1471]
      moveq.l  #0,%d0                  | [1472]
incsrc:
|;;   dbra      d5,innerlp             | [1474]
	tst.w    %d5
	jeq      _PA_1473_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1473_:         
repdone:
      move.w   %d0,(%a1)               | [1476]
|;;   movem.l   (sp)+,d3-d4/d6         | [1477]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1478]
      movea.l  %a4,%a1                 | [1479]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1480]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1481]
      move.w   (-44,%a6),%d5           | [1482]
      rts                              | [1483]
| 1880 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1486]
	ext.l    %d7
	lea      (0,%a0,%d7.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1488]
	ext.l    %d7
	lea      (0,%a1,%d7.l*1),%a2
|;;   lsr.w     #1,d7                  | [1489]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
|;;   subq.w    #1,d7                  | [1490]
	subq.l   #1,%d7
      move.w   %d7,(-44,%a6)           | [1491]
      move.w   (-106,%a6),%d6          | [1492]

      movea.l  %a0,%a5                 | [1494]
|;;   subq.w    #1,d6                  | [1495]
	subq.l   #1,%d6
out_edge:
      movea.w  %d6,%a4                 | [1497]
      movea.l  %a2,%a3                 | [1498]
      moveq.l  #0,%d5                  | [1499]
      moveq.l  #0,%d6                  | [1500]
      move.l   (%a2),%d1               | [1501]
      lsr.l    #1,%d1                  | [1502]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1507]
      move.b   %d5,%d0                 | [1508]
|;;   ror.l     #1,d0                  | [1509]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1511]
      move.b   %d6,%d2                 | [1512]
      move.l   %d2,%d3                 | [1513]
|;;   ror.l     #1,d3                  | [1514]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1515]
|;;   ror.l     #1,d4                  | [1516]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1518]
      move.l   %d0,%d6                 | [1519]
      eor.l    %d2,%d0                 | [1520]
      eor.l    %d3,%d5                 | [1521]
      eor.l    %d4,%d6                 | [1522]
|;;   rol.l     #1,d5                  | [1523]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1524]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1525]
      or.l     %d6,%d0                 | [1526]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1528]
      move.l   %d1,%d6                 | [1529]
      eor.l    %d2,%d1                 | [1530]
      eor.l    %d3,%d5                 | [1531]
      eor.l    %d4,%d6                 | [1532]
|;;   rol.l     #1,d5                  | [1533]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1534]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1535]
      or.l     %d5,%d0                 | [1536]
      or.l     %d6,%d0                 | [1537]

      eor.l    %d3,%d2                 | [1539]
      eor.l    %d3,%d4                 | [1540]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1541]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1542]
      or.l     %d4,%d0                 | [1543]
      swap     %d0                     | [1544]

      move.w   (%a1),%d6               | [1546]
      move.w   %d6,%d5                 | [1547]
|;;   eor.w     d0,d5                  | [1548]
	eor.l    %d0,%d5
|;;   and.w     d0,d5                  | [1549]
	and.l    %d0,%d5

      addq.l   #2,%a2                  | [1551]
      move.l   (%a2),%d1               | [1552]
      move.b   (-1,%a2),%d1            | [1553]
|;;   ror.l     #1,d1                  | [1554]
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1556]
      move.w   (%a0),%d5               | [1557]
      move.w   %d6,(%a0)+              | [1558]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1560]
	tst.w    %d7
	jeq      _PA_1559_
	subq.l   #1,%d7
	jra      out_loop
	_PA_1559_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1562]
      movea.l  %a5,%a0                 | [1563]
      move.w   %a4,%d6                 | [1564]
      movea.l  %a3,%a2                 | [1565]
      movea.l  %a2,%a1                 | [1566]
|;;   adda.w    -16(a6),a2             | [1567]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1568 NE]
      jne      srt_lin                 | [1569]
      movea.l  %a1,%a2                 | [1570]
srt_lin:
|;;   dbra      d6,out_edge            | [1572]
	tst.w    %d6
	jeq      _PA_1571_
	subq.l   #1,%d6
	jra      out_edge
	_PA_1571_:         
      rts                              | [1573]

| End section on aligned boundary
	.p2align 2,0
