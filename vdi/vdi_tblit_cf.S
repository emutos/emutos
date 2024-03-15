
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
| Translated on Tue Oct 15 17:11:50 2019 by version 2.6.0 of PortAsm translator
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





|;;           .extern _SOURCEX         | [18]
|;;           .extern _SOURCEY         | [19]
|;;           .extern _LITEMASK        | [20]
	.extern	_LITEMASK
|;;           .extern _SKEWMASK        | [21]
	.extern	_SKEWMASK
|;;           .extern _SCALDIR         | [22]
|;;           .extern _XDDA            | [23]
|;;           .extern _DDAINC          | [24]
|;;           .extern _SCRPT2          | [25]
|;;           .extern _SCRTCHP         | [26]
| 236 "vdi/vdi_tblit.S"
|;;           .text                    | [28]






_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [36]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [37]
      movea.l  (44+8,%a7),%a0          | [38]
      movea.l  (44+12,%a7),%a1         | [39]
      jbsr     norm_blt                | [40]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [41]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [42]

|;;           .text                    | [44]







|;;                   .even            | [52]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [53]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [54]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [55]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [56]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [57]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [58]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [59]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [60]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [61]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [62]
	.short	0x007F
|;;                   .dc.w 0x003F     | [63]
	.short	0x003F
|;;                   .dc.w 0x001F     | [64]
	.short	0x001F
|;;                   .dc.w 0x000F     | [65]
	.short	0x000F
|;;                   .dc.w 0x0007     | [66]
	.short	0x0007
|;;                   .dc.w 0x0003     | [67]
	.short	0x0003
|;;                   .dc.w 0x0001     | [68]
	.short	0x0001
|;;                   .dc.w 0x0000     | [69]
	.short	0x0000
| 303 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*3,4*3    | [72]
	.byte	4*0,4*0,4*3,4*3
|;;           .dc.b 4*4,4*4,4*7,4*7    | [73]
	.byte	4*4,4*4,4*7,4*7
|;;           .dc.b 4*6,4*6,4*6,4*6    | [74]
	.byte	4*6,4*6,4*6,4*6
|;;           .dc.b 4*1,4*1,4*13,4*13  | [75]
	.byte	4*1,4*1,4*13,4*13

|;;           .dc.b 4*0,4*15,4*0,4*15  | [77]
	.byte	4*0,4*15,4*0,4*15
|;;           .dc.b 4*0,4*14,4*1,4*15  | [78]
	.byte	4*0,4*14,4*1,4*15
|;;           .dc.b 4*0,4*13,4*2,4*15  | [79]
	.byte	4*0,4*13,4*2,4*15
|;;           .dc.b 4*0,4*12,4*3,4*15  | [80]
	.byte	4*0,4*12,4*3,4*15
|;;           .dc.b 4*0,4*11,4*4,4*15  | [81]
	.byte	4*0,4*11,4*4,4*15
|;;           .dc.b 4*0,4*10,4*5,4*15  | [82]
	.byte	4*0,4*10,4*5,4*15
|;;           .dc.b 4*0,4*9,4*6,4*15   | [83]
	.byte	4*0,4*9,4*6,4*15
|;;           .dc.b 4*0,4*8,4*7,4*15   | [84]
	.byte	4*0,4*8,4*7,4*15
|;;           .dc.b 4*0,4*7,4*8,4*15   | [85]
	.byte	4*0,4*7,4*8,4*15
|;;           .dc.b 4*0,4*6,4*9,4*15   | [86]
	.byte	4*0,4*6,4*9,4*15
|;;           .dc.b 4*0,4*5,4*10,4*15  | [87]
	.byte	4*0,4*5,4*10,4*15
|;;           .dc.b 4*0,4*4,4*11,4*15  | [88]
	.byte	4*0,4*4,4*11,4*15
|;;           .dc.b 4*0,4*3,4*12,4*15  | [89]
	.byte	4*0,4*3,4*12,4*15
|;;           .dc.b 4*0,4*2,4*13,4*15  | [90]
	.byte	4*0,4*2,4*13,4*15
|;;           .dc.b 4*0,4*1,4*14,4*15  | [91]
	.byte	4*0,4*1,4*14,4*15
|;;           .dc.b 4*0,4*0,4*15,4*15  | [92]
	.byte	4*0,4*0,4*15,4*15

toptable:
|;;           .dc.l top0,top1,top2,top3,top4,top5,top6,top7 | [95]
	.long	top0,top1,top2,top3,top4,top5,top6,top7
|;;           .dc.l top8,top9,topa,topb,topc,topd,tope,topf | [96]
	.long	top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
|;;           .dc.l twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7 | [99]
	.long	twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
|;;           .dc.l twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf | [100]
	.long	twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

|;;           .text                    | [102]
| 347 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [105]
|;;   sub.w     -24(a6),d1             | [106]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [107 PL]
      jpl      do_rot                  | [108]



|;;   neg.w     d1                     | [112]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [113]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [114]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [116]

|;;   add.w     d0,d0                  | [118]
	move.w   %d0,%d5
	add.l    %d0,%d5
      lea      lf_tab,%a2              | [119]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [120]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [121]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [122]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [127]
|;;   add.w     d0,d0                  | [128]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [129]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [131]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [132]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [135]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [136]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [137]
	andi.l   #(0xFFFF&0xF),%d5
|;;   add.w     d2,d2                  | [138]
	add.l    %d5,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [139]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [140]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [141]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [142]
      move.w   #0x8000,(-80,%a6)       | [143]
      moveq.l  #(-1),%d3               | [144]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [147 HI]
      jhi      doub_des                | [148]



|;;   add.w     d0,d0                  | [152]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [153]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [154]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [155]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [157]
	move.w   %d5,%d1
      jra      msk_done                | [158]



doub_des:
      move.w   %d0,%d1                 | [163]
|;;   lsr.w     #4,d1                  | [164]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [165]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [166 NE]
      jne      mlt_dest                | [167]



      move.w   (-24,%a6),%d1           | [171]
|;;   add.w     -20(a6),d1             | [172]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [173 CC]
      jcc      mlt_dest                | [174]
      moveq.l  #0,%d7                  | [175]
      jra      do_ritem                | [176]

mlt_dest:
      tst.w    (-42,%a6)               | [179 LT]
      jlt      mltleft                 | [180]
      moveq.l  #4,%d7                  | [181]
      jra      do_ritem                | [182]
mltleft:
      moveq.l  #8,%d7                  | [184]
do_ritem:
|;;   andi.w    #0x000F,d0             | [186 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [187]
|;;   subq.w    #1,d3                  | [188]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [189]
	move.w   %d5,%d3
not_null:
|;;   add.w     d0,d0                  | [191]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [192]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [193]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [196]
      move.w   %d3,(-28,%a6)           | [197]
      move.w   %d4,(-36,%a6)           | [198]
|;;   addq.w    #2,d3                  | [199]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [200 CS]
      jcs      msk0                    | [201]
|;;   addq.w    #1,d3                  | [202]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [204]
      move.w   %d3,(-30,%a6)           | [205]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5

plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [208]
|;;   lsr.w     -84(a6)                | [209 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   addx.w    d0,d0                  | [210]
	move.w   %d0,%d1
	addx.l   %d0,%d1
|;;   lsr.w     -76(a6)                | [211 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   addx.w    d0,d0                  | [212]
	addx.l   %d1,%d1



      lea      toptable,%a4            | [216]
	move.w   %d1,%d0
|;;   move.b    wrmappin-toptable(a4,d0.w),d0 | [217]
	move.w   %d0,%a5
	move.b   (wrmappin-toptable,%a4,%a5.l*1),%d0
|;;   movea.l   toptable-toptable(a4,d0.w),a2 | [218]
	move.w   %d0,%a5
	movea.l  (toptable-toptable,%a4,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [219]
      movea.l  %a2,%a5                 | [220]

|;;   movea.l   twoptble-toptable(a4,d0.w),a4 | [222]
	move.w   %d0,%a0
	movea.l  (twoptble-toptable,%a4,%a0.l*1),%a4



      move.w   (-96,%a6),%d0           | [226 EQ]
      jeq      wrt_char                | [227]

      btst     #1,%d0                  | [229 EQ]
      jeq      no_lite                 | [230]



|;;   move.w    _LITEMASK,-78(a6)      | [234]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [235]
      lea      liteop,%a5              | [236]

      move.l   %a2,(-64,%a6)           | [238]
      lea      liteopwf,%a2            | [239]

      move.l   %a4,(-72,%a6)           | [241]
      lea      liteopw,%a4             | [242]

no_lite:
      btst     #0,%d0                  | [245 EQ]
      jeq      no_thick                | [246]



      clr.w    (-82,%a6)               | [250]

      move.l   %a5,(-48,%a6)           | [252]
      lea      thknop,%a5              | [253]

      move.l   %a2,(-60,%a6)           | [255]
      lea      thknopwf,%a2            | [256]

      move.l   %a4,(-68,%a6)           | [258]
      lea      thknopw,%a4             | [259]

no_thick:
      btst     #2,%d0                  | [262 EQ]
      jeq      wrt_char                | [263]



|;;   move.w    _SKEWMASK,-80(a6)      | [267]
	move.w   _SKEWMASK,%d1
	move.w   %d1,(-80,%a6)
      move.l   %a5,(-56,%a6)           | [268]
      lea      skewop,%a5              | [269]

      tst.w    (-124,%a6)              | [271 EQ GT]
      jgt      wrt_char                | [272]
      jeq      not_sngl                | [273]
      clr.w    (-28,%a6)               | [274]
      moveq.l  #0,%d7                  | [275]
      jra      chng_blt                | [276]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [278 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [279]

      tst.w    (-42,%a6)               | [281 LT]
      jlt      chng_left               | [282]
      moveq.l  #4,%d7                  | [283]
      jra      chng_blt                | [284]
chng_left:
      moveq.l  #8,%d7                  | [286]
chng_blt:
      move.w   %d7,(-124,%a6)          | [288]

wrt_char:
      move.w   (-124,%a6),%d7          | [291 LE]
      jle      set_chr_hgt             | [292]
      movea.l  %a2,%a5                 | [293]
      move.w   (-18,%a6),%d5           | [294]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [296]
|;;   subq.w    #1,d3                  | [297]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [299]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [300]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [302 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [303]
      movea.l  (-10,%a6),%a0           | [304]
      movea.l  (-14,%a6),%a1           | [305]
      addq.l   #2,%a1                  | [306]
      move.l   %a1,(-14,%a6)           | [307]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [308 EQ]
      jeq      plane_loop              | [309]
      jra      norm_blt                | [310]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [312]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [314]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [320]
      move.w   (-16,%a6),%d6           | [321]
      move.w   (-18,%a6),%d7           | [322]
      lea      sgl_rtn,%a3             | [323]
      jra      sgl_1st_scan            | [324]

sgl_lp:
|;;   adda.w    d6,a0                  | [327]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [328]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [330]
| 582 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [332]


      move.w   (-42,%a6),%d1           | [335 MI]
      jmi      left_rol                | [336]
      lsr.l    %d1,%d0                 | [337]
      jra      end_ro                  | [338]
left_rol:
      lsl.l    %d1,%d0                 | [340]
end_ro:
      swap     %d0                     | [342]
      move.w   %d0,%d1                 | [343]
      swap     %d0                     | [344]

      jmp      (%a5)                   | [346]

sgl_rtn:
      move.w   %d1,(%a1)               | [349]
|;;   dbra      d3,sgl_lp              | [350]
	tst.w    %d3
	jeq      _PA_349_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_349_:         
      jra      next_plane              | [351]




dbl_loop:
      movea.l  %a3,%a4                 | [357]
      move.w   (-16,%a6),%d6           | [358]
      move.w   (-18,%a6),%d7           | [359]
      lea      dbl_2wrd,%a2            | [360]
      move.l   %a2,%d5                 | [361]
      lea      dbl_1wrd,%a3            | [362]
      jra      dbl_1st_scan            | [363]

dbl_lp:
|;;   adda.w    d6,a0                  | [366]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [367]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [368]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [370]
| 629 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [372]


      move.w   (-42,%a6),%d1           | [375 MI]
      jmi      dbleft                  | [376]
      lsr.l    %d1,%d0                 | [377]
      jra      dblendr                 | [378]
dbleft:
      lsl.l    %d1,%d0                 | [380]
dblendr:
      swap     %d0                     | [382]
      move.w   %d0,%d1                 | [383]
      swap     %d0                     | [384]
      move.w   (-38,%a6),%d2           | [385]

      jmp      (%a5)                   | [387]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [390]
      move.w   %d0,%d1                 | [391]

      movea.w  (-88,%a6),%a2           | [393]
|;;   move.w    (a1,a2.w),d4           | [394]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [396]

|;;   exg       a3,d5                  | [398]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [399]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [402]
|;;   move.w    d1,(a1,a2.w)           | [403]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [405]
	tst.w    %d3
	jeq      _PA_404_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_404_:         
      jra      next_plane              | [406]





left_loop:
      movea.l  %d6,%a0                 | [413]
      movea.l  %d7,%a1                 | [414]
|;;   adda.w    -16(a6),a0             | [415]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [416]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [417]
|;;   rol.w     -78(a6)                | [418]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [420 NE]
      jne      skewopw                 | [421]
mlt_left:
      move.l   %a0,%d6                 | [423]
      move.l   %a1,%d7                 | [424]
      move.w   (-28,%a6),(-44,%a6)     | [425]







      move.l   (%a0)+,%d0              | [433]


      move.w   (%a1),%d4               | [436]

      move.w   (-42,%a6),%d1           | [438]
      lsl.l    %d1,%d0                 | [439]

      swap     %d0                     | [441]
      move.w   %d0,%d1                 | [442]
      swap     %d0                     | [443]

      move.w   (-38,%a6),%d2           | [445]

      lea      lft_lfrng,%a3           | [447]
      jmp      (%a5)                   | [448]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [451]
|;;   adda.w    -88(a6),a1             | [452]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [454]
      lea      lft_wrds,%a3            | [455]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [460]
|;;   and.w     d2,d4                  | [461]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [462]





      swap     %d0                     | [468]
      move.w   %d1,%d0                 | [469]
      swap     %d0                     | [470]

      move.w   (-42,%a6),%d1           | [472]
      lsl.l    %d1,%d0                 | [473]

      swap     %d0                     | [475]
      move.w   %d0,%d1                 | [476]
      swap     %d0                     | [477]

|;;   not.w     d2                     | [479]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [480]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [481]
	not.l    %d5
|;;   eor.w     d4,d1                  | [482]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [484]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [486 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [487]

      jmp      (%a4)                   | [489]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [492]
|;;   adda.w    -88(a6),a1             | [493]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [495]

lef_don:
      move.w   (-36,%a6),%d2           | [498]

      lea      lft_rfrng,%a3           | [500]
      jmp      (%a5)                   | [501]

lft_rfrng:
      move.w   %d1,(%a1)               | [504]

|;;   dbra      d3,left_loop           | [506]
	tst.w    %d3
	jeq      _PA_505_
	subq.l   #1,%d3
	jra      left_loop
	_PA_505_:         
      jra      next_plane              | [507]





rite_loop:
      movea.l  %d6,%a0                 | [514]
      movea.l  %d7,%a1                 | [515]
|;;   adda.w    -16(a6),a0             | [516]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [517]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [518]
|;;   rol.w     -78(a6)                | [519]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [521 NE]
      jne      skewopw                 | [522]
mlt_rite:
      move.l   %a0,%d6                 | [524]
      move.l   %a1,%d7                 | [525]
      move.w   (-28,%a6),(-44,%a6)     | [526]
      move.w   (%a0)+,%d0              | [527]



      swap     %d0                     | [531]

      move.w   (%a1),%d4               | [533]

      move.w   (-42,%a6),%d1           | [535]
      lsr.l    %d1,%d0                 | [536]
      swap     %d0                     | [537]
      move.w   %d0,%d1                 | [538]
      swap     %d0                     | [539]

      move.w   (-38,%a6),%d2           | [541]

      lea      rgt_lfrng,%a3           | [543]
      jmp      (%a5)                   | [544]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [547]
|;;   adda.w    -88(a6),a1             | [548]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [550]
      lea      rgt_wrds,%a3            | [551]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [556]
|;;   and.w     d2,d4                  | [557]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [558]
      move.w   (%a0)+,%d0              | [559]



      swap     %d0                     | [563]

      move.w   (-42,%a6),%d1           | [565]
      lsr.l    %d1,%d0                 | [566]
      swap     %d0                     | [567]
      move.w   %d0,%d1                 | [568]
      swap     %d0                     | [569]

|;;   not.w     d2                     | [571]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [572]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [573]
	not.l    %d5
|;;   eor.w     d4,d1                  | [574]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [576]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [578 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [579]

      jmp      (%a4)                   | [581]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [584]
|;;   adda.w    -88(a6),a1             | [585]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [587]

rite_don:
      move.w   (-36,%a6),%d2           | [590]

      lea      rgt_rfrng,%a3           | [592]
      jmp      (%a5)                   | [593]

rgt_rfrng:
      move.w   %d1,(%a1)               | [596]

|;;   dbra      d3,rite_loop           | [598]
	tst.w    %d3
	jeq      _PA_597_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_597_:         
      jra      next_plane              | [599]
| 871 "vdi/vdi_tblit.S"
top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [602]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [603]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [604]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [605]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [606]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [609]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [610]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [611]
	not.l    %d0
|;;   and.w     d4,d1                  | [612]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [613]

top2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [616]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [617]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [618]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [619]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [620]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [623]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [624]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [625]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [626]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [629]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [630]
	not.l    %d0
|;;   and.w     d4,d1                  | [631]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [632]

top5:
      move.w   %d4,%d1                 | [635]
      jmp      (%a3)                   | [636]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [639]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [640]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [641]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [644]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [645]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [646]

top8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [649]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [650]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [651]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [652]

top9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [655]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [656]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [657]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [658]

topa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [661]
|;;   eor.w     d2,d1                  | [662]
	move.w   %d1,%d0
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [663]

topb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d2,d4                  | [666]
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   and.w     d2,d1                  | [667]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [668]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [669]

topc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [672]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [673]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [674]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [675]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [676]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [679]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [680]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [681]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [682]

tope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [685]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [686]
	and.l    %d2,%d0
|;;   eor.w     d2,d4                  | [687]
	move.w   %d0,%d1
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   or.w      d4,d1                  | [688]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [689]

topf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d2,d4                  | [692]
	mvz.w    %d2,%d0
	or.l     %d0,%d4
      move.w   %d4,%d1                 | [693]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [694]
| 977 "vdi/vdi_tblit.S"
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

twop2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [705]
	move.w   %d4,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [706]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [707]

twop3:
      jmp      (%a3)                   | [710]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [713]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [714]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [715]

twop5:
      move.w   %d4,%d1                 | [718]
      jmp      (%a3)                   | [719]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [722]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [723]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [726]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [727]

twop8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [730]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|;;   not.w     d1                     | [731]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [732]

twop9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [735]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   not.w     d1                     | [736]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [737]

twopa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [740]
|;;   not.w     d1                     | [741]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [742]

twopb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [745]
	move.w   %d4,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [746]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [747]

twopc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [750]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [751]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [754]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [755]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [756]

twope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [759]
	move.w   %d1,%d0
	and.l    %d4,%d0
|;;   not.w     d1                     | [760]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [761]

twopf:
      moveq.l  #(-1),%d1               | [764]
      jmp      (%a3)                   | [765]
| 1064 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [768]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [769]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [770]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [771 EQ]
      jeq      thk00                   | [772]
|;;   ori.b     #0x10,ccr              | [773 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [774 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [775]
      move.w   #0x8000,%d6             | [776]
thk01:
      move.w   %d6,(-34,%a6)           | [778]
thk00:
      tst.w    (-28,%a6)               | [780 MI]
      jmi      thk0                    | [781]
      cmp.w    (-36,%a6),%d6           | [782 CC]
      jcc      thk0                    | [783]
|;;   and.w     d6,d0                  | [784]
	and.l    %d6,%d0
      jra      thk1                    | [785]
thk0:
      clr.w    %d0                     | [787]
|;;   and.w     d6,d1                  | [788]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [790]
      move.w   %d2,%d5                 | [791]
      swap     %d5                     | [792]
      move.w   (-36,%a6),%d5           | [793]
      lsl.l    %d6,%d5                 | [794]

      swap     %d5                     | [796]
      move.w   %d5,%d7                 | [797]
      swap     %d5                     | [798]

|;;   and.w     d7,d1                  | [800]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [801]
	and.l    %d5,%d0
|;;   swap      d1                     | [802]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [803]
|;;   subq.w    #1,d6                  | [804]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [806]
      lsr.l    #1,%d0                  | [807]
      or.l     %d0,%d1                 | [808]
|;;   dbra      d6,thkoplp             | [809]
	tst.w    %d6
	jeq      _PA_808_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_808_:         

      move.w   %d1,%d0                 | [811]
      swap     %d1                     | [812]
|;;   movem.l   (sp)+,d5-d7            | [813]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [814]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [815]
| 1127 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [818]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [819 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [820]
|;;   and.w     -34(a6),d1             | [821]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [823]
      swap     %d1                     | [824]
      clr.w    %d1                     | [825]
      move.l   %d1,%d5                 | [826]
|;;   subq.w    #1,d6                  | [827]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [829]
      or.l     %d5,%d1                 | [830]
|;;   dbra      d6,thkopwlp            | [831]
	tst.w    %d6
	jeq      _PA_830_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_830_:         

      move.w   %d1,%d5                 | [833]
      swap     %d1                     | [834]
|;;   or.w      -82(a6),d1             | [835]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [836]
|;;   movem.l   (sp)+,d5-d6            | [837]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [838]
      jmp      (%a2)                   | [839]
| 1165 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [842]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [843]
|;;   subq.w    #1,-30(a6)             | [844 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [845]
      jeq      thk4                    | [846]

|;;   subq.w    #1,-30(a6)             | [848 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [849]
|;;   and.w     d6,d1                  | [850]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [852]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [853]

thk3:
      clr.w    %d1                     | [856]
      jra      thk5                    | [857]

thk4:
|;;   and.w     d6,d1                  | [860]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [863 MI]
      jmi      thk6                    | [864]
      move.w   (-28,%a6),%d5           | [865]
|;;   addq.w    #2,d5                  | [866]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [867]
|;;   ori.b     #0x10,ccr              | [868 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [869 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [870]
      move.w   #0x8000,%d7             | [871]
|;;   addq.w    #1,d5                  | [872]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [874 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [875]
|;;   subq.w    #1,d5                  | [876]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [878 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [879 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [880]
      move.w   #0x8000,%d6             | [881]
thk31:
      cmp.w    %d6,%d7                 | [883 CS]
      jcs      thk35                   | [884]
|;;   addq.w    #1,d5                  | [885]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [887]
      move.w   %d5,(-32,%a6)           | [888]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [890]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [892]
      swap     %d1                     | [893]
      clr.w    %d1                     | [894]
      move.l   %d1,%d5                 | [895]
|;;   subq.w    #1,d6                  | [896]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [898]
      or.l     %d5,%d1                 | [899]
|;;   dbra      d6,thkopwl             | [900]
	tst.w    %d6
	jeq      _PA_899_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_899_:         

      move.w   %d1,%d5                 | [902]
      swap     %d1                     | [903]
|;;   or.w      -82(a6),d1             | [904]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [905]
|;;   and.w     d2,d1                  | [906]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [907]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [908]
      jmp      (%a2)                   | [909]
| 1246 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [912]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [913]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [914]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [915]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [916]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [919]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [920]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [921]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [924]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [925]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [926]
| 1278 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [929 CC]
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
      jcc      no_shift                | [930]

      swap     %d1                     | [932]
      move.w   %d0,%d1                 | [933]
      lsr.l    #1,%d1                  | [934]

      swap     %d2                     | [936]
      move.w   (-36,%a6),%d2           | [937]
      lsr.l    #1,%d2                  | [938]
      move.w   %d2,(-36,%a6)           | [939]
      swap     %d2                     | [940]
      move.w   %d2,(-38,%a6)           | [941 EQ]
      jeq      nxt_word                | [942]
      move.w   (-42,%a6),%d0           | [943 MI]
      jmi      dec_rol                 | [944]
ror_add:
|;;   addq.w    #1,d0                  | [946]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [948]
      move.w   %d1,%d0                 | [949]
      swap     %d1                     | [950]
no_shift:
      movea.l  (-56,%a6),%a2           | [952]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [953]

dec_rol:
      tst.b    %d0                     | [956 EQ]
      jeq      begn_ror                | [957]
|;;   subq.w    #1,d0                  | [958]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [959]
begn_ror:
      clr.w    %d0                     | [961]
      jra      ror_add                 | [962]



nxt_word:
      move.w   %d2,(-36,%a6)           | [967]
      swap     %d2                     | [968]
      move.w   %d2,(-38,%a6)           | [969]
|;;   adda.w    -88(a6),a1             | [970]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [971]

      moveq.l  #15,%d0                 | [973]
|;;   sub.w     -42(a6),d0             | [974]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [975]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [976]
      jra      no_shift                | [977]






skewopw:
|;;   rol.w     -80(a6)                | [985 CC]
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
      jcc      do_shift                | [986]

      tst.w    (-42,%a6)               | [988 MI]
      jmi      mlt_left                | [989]
      jra      mlt_rite                | [990]
do_shift:
|;;   ori       #0x10,ccr              | [992 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [993]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [995]
      cmp.w    #0xFFFF,%d0             | [996 EQ]
      jeq      inc_rite                | [997]

|;;   ori       #0x10,ccr              | [999 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1000]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1001]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1004]
      tst.b    %d0                     | [1005 NE]
      jne      no_rota                 | [1006]
      move.w   #0x8000,(-40,%a6)       | [1007]
no_rota:
      move.w   (-38,%a6),%d1           | [1009]
|;;   lsr.w     #1,d1                  | [1010 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [1011]

      move.w   %d1,(-38,%a6)           | [1013]

      tst.w    %d0                     | [1015 MI]
      jmi      wdec_rol                | [1016]
|;;   addq.w    #1,-42(a6)             | [1017]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [1018]
wdec_rol:
      tst.b    %d0                     | [1020 EQ]
      jeq      set_msk                 | [1021]
|;;   subq.w    #1,-42(a6)             | [1022]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [1023]
set_msk:
      move.w   #1,(-42,%a6)            | [1025]
      jra      mlt_rite                | [1026]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1029]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1030]
      jra      do_left                 | [1031]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1034]

|;;   subq.w    #1,-28(a6)             | [1036]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1037]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1039]
|;;   sub.w     d0,d2                  | [1040]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1041]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1042]

      jra      mlt_left                | [1044]

| End section on aligned boundary
	.p2align 2,0
