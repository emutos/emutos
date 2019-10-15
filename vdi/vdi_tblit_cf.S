
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
| Translated on Tue Oct 15 15:30:29 2019 by version 2.6.0 of PortAsm translator
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
|;;           .globl _scale            | [14]
	.globl	_scale





|;;           .extern _SOURCEX         | [20]
	.extern	_SOURCEX
|;;           .extern _SOURCEY         | [21]
	.extern	_SOURCEY
|;;           .extern _LITEMASK        | [22]
	.extern	_LITEMASK
|;;           .extern _SKEWMASK        | [23]
	.extern	_SKEWMASK
|;;           .extern _SCALDIR         | [24]
	.extern	_SCALDIR
|;;           .extern _XDDA            | [25]
	.extern	_XDDA
|;;           .extern _DDAINC          | [26]
	.extern	_DDAINC
|;;           .extern _SCRPT2          | [27]
	.extern	_SCRPT2
|;;           .extern _SCRTCHP         | [28]
	.extern	_SCRTCHP
| 238 "vdi/vdi_tblit.S"
|;;           .text                    | [30]






_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [38]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [39]
      movea.l  (44+8,%a7),%a0          | [40]
      movea.l  (44+12,%a7),%a1         | [41]
      jbsr     norm_blt                | [42]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [43]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [44]


_outline:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [48]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [49]
      movea.l  (44+8,%a7),%a0          | [50]
      move.w   (44+12,%a7),%d7         | [51]
      jbsr     outlin                  | [52]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [53]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [54]


_scale:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [58]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [59]
      jbsr     replicat                | [60]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [61]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [62]

|;;           .text                    | [64]







|;;                   .even            | [72]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [73]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [74]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [75]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [76]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [77]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [78]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [79]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [80]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [81]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [82]
	.short	0x007F
|;;                   .dc.w 0x003F     | [83]
	.short	0x003F
|;;                   .dc.w 0x001F     | [84]
	.short	0x001F
|;;                   .dc.w 0x000F     | [85]
	.short	0x000F
|;;                   .dc.w 0x0007     | [86]
	.short	0x0007
|;;                   .dc.w 0x0003     | [87]
	.short	0x0003
|;;                   .dc.w 0x0001     | [88]
	.short	0x0001
|;;                   .dc.w 0x0000     | [89]
	.short	0x0000
| 323 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*3,4*3    | [92]
	.byte	4*0,4*0,4*3,4*3
|;;           .dc.b 4*4,4*4,4*7,4*7    | [93]
	.byte	4*4,4*4,4*7,4*7
|;;           .dc.b 4*6,4*6,4*6,4*6    | [94]
	.byte	4*6,4*6,4*6,4*6
|;;           .dc.b 4*1,4*1,4*13,4*13  | [95]
	.byte	4*1,4*1,4*13,4*13

|;;           .dc.b 4*0,4*15,4*0,4*15  | [97]
	.byte	4*0,4*15,4*0,4*15
|;;           .dc.b 4*0,4*14,4*1,4*15  | [98]
	.byte	4*0,4*14,4*1,4*15
|;;           .dc.b 4*0,4*13,4*2,4*15  | [99]
	.byte	4*0,4*13,4*2,4*15
|;;           .dc.b 4*0,4*12,4*3,4*15  | [100]
	.byte	4*0,4*12,4*3,4*15
|;;           .dc.b 4*0,4*11,4*4,4*15  | [101]
	.byte	4*0,4*11,4*4,4*15
|;;           .dc.b 4*0,4*10,4*5,4*15  | [102]
	.byte	4*0,4*10,4*5,4*15
|;;           .dc.b 4*0,4*9,4*6,4*15   | [103]
	.byte	4*0,4*9,4*6,4*15
|;;           .dc.b 4*0,4*8,4*7,4*15   | [104]
	.byte	4*0,4*8,4*7,4*15
|;;           .dc.b 4*0,4*7,4*8,4*15   | [105]
	.byte	4*0,4*7,4*8,4*15
|;;           .dc.b 4*0,4*6,4*9,4*15   | [106]
	.byte	4*0,4*6,4*9,4*15
|;;           .dc.b 4*0,4*5,4*10,4*15  | [107]
	.byte	4*0,4*5,4*10,4*15
|;;           .dc.b 4*0,4*4,4*11,4*15  | [108]
	.byte	4*0,4*4,4*11,4*15
|;;           .dc.b 4*0,4*3,4*12,4*15  | [109]
	.byte	4*0,4*3,4*12,4*15
|;;           .dc.b 4*0,4*2,4*13,4*15  | [110]
	.byte	4*0,4*2,4*13,4*15
|;;           .dc.b 4*0,4*1,4*14,4*15  | [111]
	.byte	4*0,4*1,4*14,4*15
|;;           .dc.b 4*0,4*0,4*15,4*15  | [112]
	.byte	4*0,4*0,4*15,4*15

toptable:
|;;           .dc.l top0,top1,top2,top3,top4,top5,top6,top7 | [115]
	.long	top0,top1,top2,top3,top4,top5,top6,top7
|;;           .dc.l top8,top9,topa,topb,topc,topd,tope,topf | [116]
	.long	top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
|;;           .dc.l twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7 | [119]
	.long	twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
|;;           .dc.l twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf | [120]
	.long	twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

|;;           .text                    | [122]
| 367 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [125]
|;;   sub.w     -24(a6),d1             | [126]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [127 PL]
      jpl      do_rot                  | [128]



|;;   neg.w     d1                     | [132]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [133]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [134]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [136]

|;;   add.w     d0,d0                  | [138]
	move.w   %d0,%d5
	add.l    %d0,%d5
      lea      lf_tab,%a2              | [139]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [140]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [141]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [142]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [147]
|;;   add.w     d0,d0                  | [148]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [149]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [151]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [152]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [155]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [156]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [157]
	andi.l   #(0xFFFF&0xF),%d5
|;;   add.w     d2,d2                  | [158]
	add.l    %d5,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [159]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [160]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [161]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [162]
      move.w   #0x8000,(-80,%a6)       | [163]
      moveq.l  #(-1),%d3               | [164]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [167 HI]
      jhi      doub_des                | [168]



|;;   add.w     d0,d0                  | [172]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [173]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [174]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [175]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [177]
	move.w   %d5,%d1
      jra      msk_done                | [178]



doub_des:
      move.w   %d0,%d1                 | [183]
|;;   lsr.w     #4,d1                  | [184]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [185]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [186 NE]
      jne      mlt_dest                | [187]



      move.w   (-24,%a6),%d1           | [191]
|;;   add.w     -20(a6),d1             | [192]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [193 CC]
      jcc      mlt_dest                | [194]
      moveq.l  #0,%d7                  | [195]
      jra      do_ritem                | [196]

mlt_dest:
      tst.w    (-42,%a6)               | [199 LT]
      jlt      mltleft                 | [200]
      moveq.l  #4,%d7                  | [201]
      jra      do_ritem                | [202]
mltleft:
      moveq.l  #8,%d7                  | [204]
do_ritem:
|;;   andi.w    #0x000F,d0             | [206 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [207]
|;;   subq.w    #1,d3                  | [208]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [209]
	move.w   %d5,%d3
not_null:
|;;   add.w     d0,d0                  | [211]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [212]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [213]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [216]
      move.w   %d3,(-28,%a6)           | [217]
      move.w   %d4,(-36,%a6)           | [218]
|;;   addq.w    #2,d3                  | [219]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [220 CS]
      jcs      msk0                    | [221]
|;;   addq.w    #1,d3                  | [222]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [224]
      move.w   %d3,(-30,%a6)           | [225]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5

plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [228]
|;;   lsr.w     -84(a6)                | [229 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   addx.w    d0,d0                  | [230]
	move.w   %d0,%d1
	addx.l   %d0,%d1
|;;   lsr.w     -76(a6)                | [231 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   addx.w    d0,d0                  | [232]
	addx.l   %d1,%d1



      lea      toptable,%a4            | [236]
	move.w   %d1,%d0
|;;   move.b    wrmappin-toptable(a4,d0.w),d0 | [237]
	move.w   %d0,%a5
	move.b   (wrmappin-toptable,%a4,%a5.l*1),%d0
|;;   movea.l   toptable-toptable(a4,d0.w),a2 | [238]
	move.w   %d0,%a5
	movea.l  (toptable-toptable,%a4,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [239]
      movea.l  %a2,%a5                 | [240]

|;;   movea.l   twoptble-toptable(a4,d0.w),a4 | [242]
	move.w   %d0,%a0
	movea.l  (twoptble-toptable,%a4,%a0.l*1),%a4



      move.w   (-96,%a6),%d0           | [246 EQ]
      jeq      wrt_char                | [247]

      btst     #1,%d0                  | [249 EQ]
      jeq      no_lite                 | [250]



|;;   move.w    _LITEMASK,-78(a6)      | [254]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [255]
      lea      liteop,%a5              | [256]

      move.l   %a2,(-64,%a6)           | [258]
      lea      liteopwf,%a2            | [259]

      move.l   %a4,(-72,%a6)           | [261]
      lea      liteopw,%a4             | [262]

no_lite:
      btst     #0,%d0                  | [265 EQ]
      jeq      no_thick                | [266]



      clr.w    (-82,%a6)               | [270]

      move.l   %a5,(-48,%a6)           | [272]
      lea      thknop,%a5              | [273]

      move.l   %a2,(-60,%a6)           | [275]
      lea      thknopwf,%a2            | [276]

      move.l   %a4,(-68,%a6)           | [278]
      lea      thknopw,%a4             | [279]

no_thick:
      btst     #2,%d0                  | [282 EQ]
      jeq      wrt_char                | [283]



|;;   move.w    _SKEWMASK,-80(a6)      | [287]
	move.w   _SKEWMASK,%d1
	move.w   %d1,(-80,%a6)
      move.l   %a5,(-56,%a6)           | [288]
      lea      skewop,%a5              | [289]

      tst.w    (-124,%a6)              | [291 EQ GT]
      jgt      wrt_char                | [292]
      jeq      not_sngl                | [293]
      clr.w    (-28,%a6)               | [294]
      moveq.l  #0,%d7                  | [295]
      jra      chng_blt                | [296]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [298 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [299]

      tst.w    (-42,%a6)               | [301 LT]
      jlt      chng_left               | [302]
      moveq.l  #4,%d7                  | [303]
      jra      chng_blt                | [304]
chng_left:
      moveq.l  #8,%d7                  | [306]
chng_blt:
      move.w   %d7,(-124,%a6)          | [308]

wrt_char:
      move.w   (-124,%a6),%d7          | [311 LE]
      jle      set_chr_hgt             | [312]
      movea.l  %a2,%a5                 | [313]
      move.w   (-18,%a6),%d5           | [314]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [316]
|;;   subq.w    #1,d3                  | [317]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [319]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [320]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [322 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [323]
      movea.l  (-10,%a6),%a0           | [324]
      movea.l  (-14,%a6),%a1           | [325]
      addq.l   #2,%a1                  | [326]
      move.l   %a1,(-14,%a6)           | [327]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [328 EQ]
      jeq      plane_loop              | [329]
      jra      norm_blt                | [330]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [332]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [334]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [340]
      move.w   (-16,%a6),%d6           | [341]
      move.w   (-18,%a6),%d7           | [342]
      lea      sgl_rtn,%a3             | [343]
      jra      sgl_1st_scan            | [344]

sgl_lp:
|;;   adda.w    d6,a0                  | [347]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [348]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [350]
| 602 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [352]


      move.w   (-42,%a6),%d1           | [355 MI]
      jmi      left_rol                | [356]
      lsr.l    %d1,%d0                 | [357]
      jra      end_ro                  | [358]
left_rol:
      lsl.l    %d1,%d0                 | [360]
end_ro:
      swap     %d0                     | [362]
      move.w   %d0,%d1                 | [363]
      swap     %d0                     | [364]

      jmp      (%a5)                   | [366]

sgl_rtn:
      move.w   %d1,(%a1)               | [369]
|;;   dbra      d3,sgl_lp              | [370]
	tst.w    %d3
	jeq      _PA_369_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_369_:         
      jra      next_plane              | [371]




dbl_loop:
      movea.l  %a3,%a4                 | [377]
      move.w   (-16,%a6),%d6           | [378]
      move.w   (-18,%a6),%d7           | [379]
      lea      dbl_2wrd,%a2            | [380]
      move.l   %a2,%d5                 | [381]
      lea      dbl_1wrd,%a3            | [382]
      jra      dbl_1st_scan            | [383]

dbl_lp:
|;;   adda.w    d6,a0                  | [386]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [387]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [388]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [390]
| 649 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [392]


      move.w   (-42,%a6),%d1           | [395 MI]
      jmi      dbleft                  | [396]
      lsr.l    %d1,%d0                 | [397]
      jra      dblendr                 | [398]
dbleft:
      lsl.l    %d1,%d0                 | [400]
dblendr:
      swap     %d0                     | [402]
      move.w   %d0,%d1                 | [403]
      swap     %d0                     | [404]
      move.w   (-38,%a6),%d2           | [405]

      jmp      (%a5)                   | [407]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [410]
      move.w   %d0,%d1                 | [411]

      movea.w  (-88,%a6),%a2           | [413]
|;;   move.w    (a1,a2.w),d4           | [414]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [416]

|;;   exg       a3,d5                  | [418]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [419]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [422]
|;;   move.w    d1,(a1,a2.w)           | [423]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [425]
	tst.w    %d3
	jeq      _PA_424_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_424_:         
      jra      next_plane              | [426]





left_loop:
      movea.l  %d6,%a0                 | [433]
      movea.l  %d7,%a1                 | [434]
|;;   adda.w    -16(a6),a0             | [435]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [436]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [437]
|;;   rol.w     -78(a6)                | [438]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [440 NE]
      jne      skewopw                 | [441]
mlt_left:
      move.l   %a0,%d6                 | [443]
      move.l   %a1,%d7                 | [444]
      move.w   (-28,%a6),(-44,%a6)     | [445]







      move.l   (%a0)+,%d0              | [453]


      move.w   (%a1),%d4               | [456]

      move.w   (-42,%a6),%d1           | [458]
      lsl.l    %d1,%d0                 | [459]

      swap     %d0                     | [461]
      move.w   %d0,%d1                 | [462]
      swap     %d0                     | [463]

      move.w   (-38,%a6),%d2           | [465]

      lea      lft_lfrng,%a3           | [467]
      jmp      (%a5)                   | [468]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [471]
|;;   adda.w    -88(a6),a1             | [472]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [474]
      lea      lft_wrds,%a3            | [475]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [480]
|;;   and.w     d2,d4                  | [481]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [482]





      swap     %d0                     | [488]
      move.w   %d1,%d0                 | [489]
      swap     %d0                     | [490]

      move.w   (-42,%a6),%d1           | [492]
      lsl.l    %d1,%d0                 | [493]

      swap     %d0                     | [495]
      move.w   %d0,%d1                 | [496]
      swap     %d0                     | [497]

|;;   not.w     d2                     | [499]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [500]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [501]
	not.l    %d5
|;;   eor.w     d4,d1                  | [502]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [504]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [506 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [507]

      jmp      (%a4)                   | [509]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [512]
|;;   adda.w    -88(a6),a1             | [513]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [515]

lef_don:
      move.w   (-36,%a6),%d2           | [518]

      lea      lft_rfrng,%a3           | [520]
      jmp      (%a5)                   | [521]

lft_rfrng:
      move.w   %d1,(%a1)               | [524]

|;;   dbra      d3,left_loop           | [526]
	tst.w    %d3
	jeq      _PA_525_
	subq.l   #1,%d3
	jra      left_loop
	_PA_525_:         
      jra      next_plane              | [527]





rite_loop:
      movea.l  %d6,%a0                 | [534]
      movea.l  %d7,%a1                 | [535]
|;;   adda.w    -16(a6),a0             | [536]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [537]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [538]
|;;   rol.w     -78(a6)                | [539]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [541 NE]
      jne      skewopw                 | [542]
mlt_rite:
      move.l   %a0,%d6                 | [544]
      move.l   %a1,%d7                 | [545]
      move.w   (-28,%a6),(-44,%a6)     | [546]
      move.w   (%a0)+,%d0              | [547]



      swap     %d0                     | [551]

      move.w   (%a1),%d4               | [553]

      move.w   (-42,%a6),%d1           | [555]
      lsr.l    %d1,%d0                 | [556]
      swap     %d0                     | [557]
      move.w   %d0,%d1                 | [558]
      swap     %d0                     | [559]

      move.w   (-38,%a6),%d2           | [561]

      lea      rgt_lfrng,%a3           | [563]
      jmp      (%a5)                   | [564]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [567]
|;;   adda.w    -88(a6),a1             | [568]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [570]
      lea      rgt_wrds,%a3            | [571]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [576]
|;;   and.w     d2,d4                  | [577]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [578]
      move.w   (%a0)+,%d0              | [579]



      swap     %d0                     | [583]

      move.w   (-42,%a6),%d1           | [585]
      lsr.l    %d1,%d0                 | [586]
      swap     %d0                     | [587]
      move.w   %d0,%d1                 | [588]
      swap     %d0                     | [589]

|;;   not.w     d2                     | [591]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [592]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [593]
	not.l    %d5
|;;   eor.w     d4,d1                  | [594]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [596]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [598 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [599]

      jmp      (%a4)                   | [601]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [604]
|;;   adda.w    -88(a6),a1             | [605]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [607]

rite_don:
      move.w   (-36,%a6),%d2           | [610]

      lea      rgt_rfrng,%a3           | [612]
      jmp      (%a5)                   | [613]

rgt_rfrng:
      move.w   %d1,(%a1)               | [616]

|;;   dbra      d3,rite_loop           | [618]
	tst.w    %d3
	jeq      _PA_617_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_617_:         
      jra      next_plane              | [619]
| 891 "vdi/vdi_tblit.S"
top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [622]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [623]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [624]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [625]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [626]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [629]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [630]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [631]
	not.l    %d0
|;;   and.w     d4,d1                  | [632]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [633]

top2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [636]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [637]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [638]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [639]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [640]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [643]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [644]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [645]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [646]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [649]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [650]
	not.l    %d0
|;;   and.w     d4,d1                  | [651]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [652]

top5:
      move.w   %d4,%d1                 | [655]
      jmp      (%a3)                   | [656]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [659]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [660]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [661]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [664]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [665]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [666]

top8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [669]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [670]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [671]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [672]

top9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [675]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [676]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [677]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [678]

topa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [681]
|;;   eor.w     d2,d1                  | [682]
	move.w   %d1,%d0
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [683]

topb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d2,d4                  | [686]
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   and.w     d2,d1                  | [687]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [688]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [689]

topc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [692]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [693]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [694]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [695]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [696]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [699]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [700]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [701]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [702]

tope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [705]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [706]
	and.l    %d2,%d0
|;;   eor.w     d2,d4                  | [707]
	move.w   %d0,%d1
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   or.w      d4,d1                  | [708]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [709]

topf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d2,d4                  | [712]
	mvz.w    %d2,%d0
	or.l     %d0,%d4
      move.w   %d4,%d1                 | [713]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [714]
| 997 "vdi/vdi_tblit.S"
twop0:
      moveq.l  #0,%d1                  | [717]
      jmp      (%a3)                   | [718]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [721]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [722]

twop2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [725]
	move.w   %d4,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [726]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [727]

twop3:
      jmp      (%a3)                   | [730]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [733]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [734]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [735]

twop5:
      move.w   %d4,%d1                 | [738]
      jmp      (%a3)                   | [739]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [742]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [743]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [746]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [747]

twop8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [750]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|;;   not.w     d1                     | [751]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [752]

twop9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [755]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   not.w     d1                     | [756]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [757]

twopa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [760]
|;;   not.w     d1                     | [761]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [762]

twopb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [765]
	move.w   %d4,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [766]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [767]

twopc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [770]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [771]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [774]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [775]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [776]

twope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [779]
	move.w   %d1,%d0
	and.l    %d4,%d0
|;;   not.w     d1                     | [780]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [781]

twopf:
      moveq.l  #(-1),%d1               | [784]
      jmp      (%a3)                   | [785]
| 1084 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [788]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [789]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [790]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [791 EQ]
      jeq      thk00                   | [792]
|;;   ori.b     #0x10,ccr              | [793 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [794 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [795]
      move.w   #0x8000,%d6             | [796]
thk01:
      move.w   %d6,(-34,%a6)           | [798]
thk00:
      tst.w    (-28,%a6)               | [800 MI]
      jmi      thk0                    | [801]
      cmp.w    (-36,%a6),%d6           | [802 CC]
      jcc      thk0                    | [803]
|;;   and.w     d6,d0                  | [804]
	and.l    %d6,%d0
      jra      thk1                    | [805]
thk0:
      clr.w    %d0                     | [807]
|;;   and.w     d6,d1                  | [808]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [810]
      move.w   %d2,%d5                 | [811]
      swap     %d5                     | [812]
      move.w   (-36,%a6),%d5           | [813]
      lsl.l    %d6,%d5                 | [814]

      swap     %d5                     | [816]
      move.w   %d5,%d7                 | [817]
      swap     %d5                     | [818]

|;;   and.w     d7,d1                  | [820]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [821]
	and.l    %d5,%d0
|;;   swap      d1                     | [822]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [823]
|;;   subq.w    #1,d6                  | [824]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [826]
      lsr.l    #1,%d0                  | [827]
      or.l     %d0,%d1                 | [828]
|;;   dbra      d6,thkoplp             | [829]
	tst.w    %d6
	jeq      _PA_828_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_828_:         

      move.w   %d1,%d0                 | [831]
      swap     %d1                     | [832]
|;;   movem.l   (sp)+,d5-d7            | [833]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [834]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [835]
| 1147 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [838]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [839 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [840]
|;;   and.w     -34(a6),d1             | [841]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [843]
      swap     %d1                     | [844]
      clr.w    %d1                     | [845]
      move.l   %d1,%d5                 | [846]
|;;   subq.w    #1,d6                  | [847]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [849]
      or.l     %d5,%d1                 | [850]
|;;   dbra      d6,thkopwlp            | [851]
	tst.w    %d6
	jeq      _PA_850_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_850_:         

      move.w   %d1,%d5                 | [853]
      swap     %d1                     | [854]
|;;   or.w      -82(a6),d1             | [855]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [856]
|;;   movem.l   (sp)+,d5-d6            | [857]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [858]
      jmp      (%a2)                   | [859]
| 1185 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [862]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [863]
|;;   subq.w    #1,-30(a6)             | [864 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [865]
      jeq      thk4                    | [866]

|;;   subq.w    #1,-30(a6)             | [868 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [869]
|;;   and.w     d6,d1                  | [870]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [872]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [873]

thk3:
      clr.w    %d1                     | [876]
      jra      thk5                    | [877]

thk4:
|;;   and.w     d6,d1                  | [880]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [883 MI]
      jmi      thk6                    | [884]
      move.w   (-28,%a6),%d5           | [885]
|;;   addq.w    #2,d5                  | [886]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [887]
|;;   ori.b     #0x10,ccr              | [888 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [889 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [890]
      move.w   #0x8000,%d7             | [891]
|;;   addq.w    #1,d5                  | [892]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [894 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [895]
|;;   subq.w    #1,d5                  | [896]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [898 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [899 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [900]
      move.w   #0x8000,%d6             | [901]
thk31:
      cmp.w    %d6,%d7                 | [903 CS]
      jcs      thk35                   | [904]
|;;   addq.w    #1,d5                  | [905]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [907]
      move.w   %d5,(-32,%a6)           | [908]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [910]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [912]
      swap     %d1                     | [913]
      clr.w    %d1                     | [914]
      move.l   %d1,%d5                 | [915]
|;;   subq.w    #1,d6                  | [916]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [918]
      or.l     %d5,%d1                 | [919]
|;;   dbra      d6,thkopwl             | [920]
	tst.w    %d6
	jeq      _PA_919_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_919_:         

      move.w   %d1,%d5                 | [922]
      swap     %d1                     | [923]
|;;   or.w      -82(a6),d1             | [924]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [925]
|;;   and.w     d2,d1                  | [926]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [927]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [928]
      jmp      (%a2)                   | [929]
| 1266 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [932]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [933]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [934]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [935]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [936]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [939]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [940]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [941]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [944]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [945]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [946]
| 1298 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [949 CC]
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
      jcc      no_shift                | [950]

      swap     %d1                     | [952]
      move.w   %d0,%d1                 | [953]
      lsr.l    #1,%d1                  | [954]

      swap     %d2                     | [956]
      move.w   (-36,%a6),%d2           | [957]
      lsr.l    #1,%d2                  | [958]
      move.w   %d2,(-36,%a6)           | [959]
      swap     %d2                     | [960]
      move.w   %d2,(-38,%a6)           | [961 EQ]
      jeq      nxt_word                | [962]
      move.w   (-42,%a6),%d0           | [963 MI]
      jmi      dec_rol                 | [964]
ror_add:
|;;   addq.w    #1,d0                  | [966]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [968]
      move.w   %d1,%d0                 | [969]
      swap     %d1                     | [970]
no_shift:
      movea.l  (-56,%a6),%a2           | [972]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [973]

dec_rol:
      tst.b    %d0                     | [976 EQ]
      jeq      begn_ror                | [977]
|;;   subq.w    #1,d0                  | [978]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [979]
begn_ror:
      clr.w    %d0                     | [981]
      jra      ror_add                 | [982]



nxt_word:
      move.w   %d2,(-36,%a6)           | [987]
      swap     %d2                     | [988]
      move.w   %d2,(-38,%a6)           | [989]
|;;   adda.w    -88(a6),a1             | [990]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [991]

      moveq.l  #15,%d0                 | [993]
|;;   sub.w     -42(a6),d0             | [994]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [995]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [996]
      jra      no_shift                | [997]






skewopw:
|;;   rol.w     -80(a6)                | [1005 CC]
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
      jcc      do_shift                | [1006]

      tst.w    (-42,%a6)               | [1008 MI]
      jmi      mlt_left                | [1009]
      jra      mlt_rite                | [1010]
do_shift:
|;;   ori       #0x10,ccr              | [1012 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1013]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1015]
      cmp.w    #0xFFFF,%d0             | [1016 EQ]
      jeq      inc_rite                | [1017]

|;;   ori       #0x10,ccr              | [1019 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1020]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1021]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1024]
      tst.b    %d0                     | [1025 NE]
      jne      no_rota                 | [1026]
      move.w   #0x8000,(-40,%a6)       | [1027]
no_rota:
      move.w   (-38,%a6),%d1           | [1029]
|;;   lsr.w     #1,d1                  | [1030 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [1031]

      move.w   %d1,(-38,%a6)           | [1033]

      tst.w    %d0                     | [1035 MI]
      jmi      wdec_rol                | [1036]
|;;   addq.w    #1,-42(a6)             | [1037]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [1038]
wdec_rol:
      tst.b    %d0                     | [1040 EQ]
      jeq      set_msk                 | [1041]
|;;   subq.w    #1,-42(a6)             | [1042]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [1043]
set_msk:
      move.w   #1,(-42,%a6)            | [1045]
      jra      mlt_rite                | [1046]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1049]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1050]
      jra      do_left                 | [1051]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1054]

|;;   subq.w    #1,-28(a6)             | [1056]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1057]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1059]
|;;   sub.w     d0,d2                  | [1060]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1061]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1062]

      jra      mlt_left                | [1064]
| 1424 "vdi/vdi_tblit.S"
getnextbuf:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   %d0,-(%a7)              | [1067]
      movea.l  _SCRTCHP,%a1            | [1068]
      move.w   _SCRPT2,%d0             | [1069]
|;;   sub.w     -2(a6),d0              | [1070]
	move.w   (-2,%a6),%d1
	sub.l    %d1,%d0
      move.w   %d0,(-2,%a6)            | [1071]
|;;   adda.w    d0,a1                  | [1072]
	ext.l    %d0
	adda.l   %d0,%a1
      move.w   (%a7)+,%d0              | [1073]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1074]

currbuf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      movea.l  _SCRTCHP,%a1            | [1077]
|;;   adda.w    -2(a6),a1              | [1078]
	mvs.w    (-2,%a6),%d0
	adda.l   %d0,%a1
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      rts                              | [1079]






replicat:
|***	SAVE	d5-d6	***
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
      move.w   _SOURCEX,%d0            | [1087]
      move.w   %d0,%d4                 | [1088]
|;;   andi.w    #0x0F,d0               | [1089]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1090]
|;;   lsr.w     #4,d4                  | [1091]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   add.w     d4,d4                  | [1092]
	add.l    %d4,%d4

      move.w   _SOURCEY,%d0            | [1094]
      mulu.w   (-16,%a6),%d0           | [1095]
      movea.l  (-10,%a6),%a0           | [1096]
|;;   adda.w    d4,a0                  | [1097]
	mvs.w    %d4,%d3
	adda.l   %d3,%a0
      adda.l   %d0,%a0                 | [1098]

      move.w   (-24,%a6),%d2           | [1100]
      move.w   #0x8000,%d3             | [1101]
      move.w   %d3,%d4                 | [1102]
|;;   lsr.w     d2,d3                  | [1103]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1105]
      move.w   (-102,%a6),%d1          | [1106]

      move.w   %d2,(-22,%a6)           | [1108]
      move.w   %d1,(-20,%a6)           | [1109]
      jbsr     getnextbuf              | [1110]

|;;   lsr.w     #3,d1                  | [1112]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   add.w     d1,d1                  | [1113]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1114 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1116]



      move.w   _SCALDIR,%d7            | [1120]
|;;   roxr.l    #1,d7                  | [1121 X]
	move.l   %d7,%d5
	addx.l   %d5,%d5
	moveq    #32-1,%d6
	lsl.l    %d6,%d5
	lsr.l    #1,%d7
	or.l     %d5,%d7
      moveq.l  #0,%d7                  | [1122]
|;;   roxr.l    #1,d7                  | [1123]
	move.l   %d7,%d5
	addx.l   %d5,%d5
	moveq    #32-1,%d6
	lsl.l    %d6,%d5
	lsr.l    #1,%d7
	or.l     %d5,%d7

      move.w   (-16,%a6),%d7           | [1125]
      move.w   _DDAINC,%d2             | [1126]
|***	RESTORE	AFTER d5	***
	movem.l  (%a7),%d5
      move.w   (-22,%a6),%d5           | [1127]
|;;   subq.w    #1,d5                  | [1128]
	subq.l   #1,%d5
|***	RESTORE	AFTER d6	***
	movem.l  4(%a7),%d6
	lea      8(%a7),%a7
      move.w   #32767,%d6              | [1129]
      tst.l    %d7                     | [1130 MI]
      jmi      rep_ylop                | [1131]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1133 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1134]
      jbsr     yloop                   | [1135]
y_no_drw:
|;;   adda.w    d7,a0                  | [1137]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1138]
	tst.w    %d5
	jeq      _PA_1137_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1137_:         
      jra      y_rep_don               | [1139]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1141 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1142]
      jbsr     yloop                   | [1143]
y_no_rep:
      jbsr     yloop                   | [1145]
|;;   adda.w    d7,a0                  | [1146]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1147]
	tst.w    %d5
	jeq      _PA_1146_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1146_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1149]
|;;   subq.w    #1,d2                  | [1150]
	subq.l   #1,%d2
      move.w   _XDDA,%d1               | [1151]
      move.w   _DDAINC,%d0             | [1152]
      moveq.l  #0,%d3                  | [1153]
repwidcl:
|;;   add.w     d0,d1                  | [1155 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1156]
|;;   addq.w    #1,d3                  | [1157]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1159 PL]
      jpl      nrpndoub                | [1160]
|;;   addq.w    #1,d3                  | [1161]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1163]
	tst.w    %d2
	jeq      _PA_1162_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1162_:         
      move.w   %d1,_XDDA               | [1164]
      move.w   %d3,(-102,%a6)          | [1165]
      move.w   (-92,%a6),(-106,%a6)    | [1166]


      move.w   (-18,%a6),%d1           | [1169]
      move.w   %d1,(-16,%a6)           | [1170]

      jbsr     currbuf                 | [1172]
      move.l   %a1,(-10,%a6)           | [1173]

      clr.w    _SOURCEX                | [1175]
      clr.w    _SOURCEY                | [1176]
      rts                              | [1177]
| 1566 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d7,-(sp)            | [1180]
	lea      -20(%a7),%a7
	movem.l  %d3-%d7,(%a7)
      movea.l  %a0,%a3                 | [1181]
      movea.l  %a1,%a4                 | [1182]
      moveq.l  #0,%d0                  | [1183]
      move.w   (-20,%a6),%d5           | [1184]
|;;   subq.w    #1,d5                  | [1185]
	subq.l   #1,%d5

      move.w   _XDDA,%d7               | [1187]
      jra      nextsrc                 | [1188]
innerlp:
|;;   ror.w     #1,d3                  | [1190 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1191]
nextsrc:
      movea.w  (%a3)+,%a2              | [1193]
reploop:
      move.w   %a2,%d6                 | [1195]
|;;   and.w     d3,d6                  | [1196 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1197]


repnor:
      tst.l    %d7                     | [1201 MI]
      jmi      repnorup                | [1202]
|;;   add.w     d2,d7                  | [1203 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1204]
      jra      ordone                  | [1205]
repnorup:
|;;   add.w     d2,d7                  | [1207 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1208]


|;;   ror.w     #1,d4                  | [1211 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1214]
      move.w   %d0,(%a4)+              | [1215]
      moveq.l  #0,%d0                  | [1216]


      jra      ordone                  | [1219]
nrepnor:


|;;   add.w     d2,d7                  | [1223 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1224]


|;;   or.w      d4,d0                  | [1227]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1228 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1230]
      move.w   %d0,(%a4)+              | [1231]
      moveq.l  #0,%d0                  | [1232]
o_no_rep:
      tst.l    %d7                     | [1234 PL]
      jpl      incsrc                  | [1235]


|;;   or.w      d4,d0                  | [1238]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1240 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1241]
nextdst:
      move.w   %d0,(%a4)+              | [1243]
      moveq.l  #0,%d0                  | [1244]
incsrc:
|;;   dbra      d5,innerlp             | [1246]
	tst.w    %d5
	jeq      _PA_1245_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1245_:         
repdone:
      move.w   %d0,(%a4)               | [1248]
|;;   movem.l   (sp)+,d3-d7            | [1249]
	movem.l  (%a7),%d3-%d7
	lea      20(%a7),%a7
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1250]
	ext.l    %d1
	adda.l   %d1,%a1
      rts                              | [1251]
| 1648 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1254]
	ext.l    %d7
	lea      (0,%a0,%d7.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1256]
	lea      (0,%a1,%d7.l*1),%a2
|;;   lsr.w     #1,d7                  | [1257]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
|;;   subq.w    #1,d7                  | [1258]
	subq.l   #1,%d7
      move.w   %d7,(-44,%a6)           | [1259]
      move.w   (-106,%a6),%d6          | [1260]

      movea.l  %a0,%a5                 | [1262]
|;;   subq.w    #1,d6                  | [1263]
	subq.l   #1,%d6
out_edge:
      movea.w  %d6,%a4                 | [1265]
      movea.l  %a2,%a3                 | [1266]
      moveq.l  #0,%d5                  | [1267]
      moveq.l  #0,%d6                  | [1268]
      move.l   (%a2),%d1               | [1269]
      lsr.l    #1,%d1                  | [1270]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1275]
      move.b   %d5,%d0                 | [1276]
|;;   ror.l     #1,d0                  | [1277]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1279]
      move.b   %d6,%d2                 | [1280]
      move.l   %d2,%d3                 | [1281]
|;;   ror.l     #1,d3                  | [1282]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1283]
|;;   ror.l     #1,d4                  | [1284]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1286]
      move.l   %d0,%d6                 | [1287]
      eor.l    %d2,%d0                 | [1288]
      eor.l    %d3,%d5                 | [1289]
      eor.l    %d4,%d6                 | [1290]
|;;   rol.l     #1,d5                  | [1291]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1292]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1293]
      or.l     %d6,%d0                 | [1294]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1296]
      move.l   %d1,%d6                 | [1297]
      eor.l    %d2,%d1                 | [1298]
      eor.l    %d3,%d5                 | [1299]
      eor.l    %d4,%d6                 | [1300]
|;;   rol.l     #1,d5                  | [1301]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1302]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1303]
      or.l     %d5,%d0                 | [1304]
      or.l     %d6,%d0                 | [1305]

      eor.l    %d3,%d2                 | [1307]
      eor.l    %d3,%d4                 | [1308]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1309]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1310]
      or.l     %d4,%d0                 | [1311]
      swap     %d0                     | [1312]

      move.w   (%a1),%d6               | [1314]
      move.w   %d6,%d5                 | [1315]
|;;   eor.w     d0,d5                  | [1316]
	eor.l    %d0,%d5
|;;   and.w     d0,d5                  | [1317]
	and.l    %d0,%d5

      addq.l   #2,%a2                  | [1319]
      move.l   (%a2),%d1               | [1320]
      move.b   (-1,%a2),%d1            | [1321]
|;;   ror.l     #1,d1                  | [1322]
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1324]
      move.w   (%a0),%d5               | [1325]
      move.w   %d6,(%a0)+              | [1326]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1328]
	tst.w    %d7
	jeq      _PA_1327_
	subq.l   #1,%d7
	jra      out_loop
	_PA_1327_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1330]
      movea.l  %a5,%a0                 | [1331]
      move.w   %a4,%d6                 | [1332]
      movea.l  %a3,%a2                 | [1333]
      movea.l  %a2,%a1                 | [1334]
|;;   adda.w    -16(a6),a2             | [1335]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1336 NE]
      jne      srt_lin                 | [1337]
      movea.l  %a1,%a2                 | [1338]
srt_lin:
|;;   dbra      d6,out_edge            | [1340]
	tst.w    %d6
	jeq      _PA_1339_
	subq.l   #1,%d6
	jra      out_edge
	_PA_1339_:         
      rts                              | [1341]

| End section on aligned boundary
	.p2align 2,0
