
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
| Translated on Tue Oct 15 15:51:00 2019 by version 2.6.0 of PortAsm translator
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
|;;           .globl _scale            | [13]
	.globl	_scale





|;;           .extern _SOURCEX         | [19]
	.extern	_SOURCEX
|;;           .extern _SOURCEY         | [20]
	.extern	_SOURCEY
|;;           .extern _LITEMASK        | [21]
	.extern	_LITEMASK
|;;           .extern _SKEWMASK        | [22]
	.extern	_SKEWMASK
|;;           .extern _SCALDIR         | [23]
	.extern	_SCALDIR
|;;           .extern _XDDA            | [24]
	.extern	_XDDA
|;;           .extern _DDAINC          | [25]
	.extern	_DDAINC
|;;           .extern _SCRPT2          | [26]
	.extern	_SCRPT2
|;;           .extern _SCRTCHP         | [27]
	.extern	_SCRTCHP
| 237 "vdi/vdi_tblit.S"
|;;           .text                    | [29]






_normal_blit:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [37]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [38]
      movea.l  (44+8,%a7),%a0          | [39]
      movea.l  (44+12,%a7),%a1         | [40]
      jbsr     norm_blt                | [41]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [42]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [43]


_scale:
|;;   movem.l   d2-d7/a2-a6,-(sp)      | [47]
	lea      -44(%a7),%a7
	movem.l  %d2-%d7/%a2-%a6,(%a7)
      movea.l  (44+4,%a7),%a6          | [48]
      jbsr     replicat                | [49]
|;;   movem.l   (sp)+,d2-d7/a2-a6      | [50]
	movem.l  (%a7),%d2-%d7/%a2-%a6
	lea      44(%a7),%a7
      rts                              | [51]

|;;           .text                    | [53]







|;;                   .even            | [61]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [62]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [63]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [64]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [65]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [66]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [67]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [68]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [69]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [70]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [71]
	.short	0x007F
|;;                   .dc.w 0x003F     | [72]
	.short	0x003F
|;;                   .dc.w 0x001F     | [73]
	.short	0x001F
|;;                   .dc.w 0x000F     | [74]
	.short	0x000F
|;;                   .dc.w 0x0007     | [75]
	.short	0x0007
|;;                   .dc.w 0x0003     | [76]
	.short	0x0003
|;;                   .dc.w 0x0001     | [77]
	.short	0x0001
|;;                   .dc.w 0x0000     | [78]
	.short	0x0000
| 312 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*3,4*3    | [81]
	.byte	4*0,4*0,4*3,4*3
|;;           .dc.b 4*4,4*4,4*7,4*7    | [82]
	.byte	4*4,4*4,4*7,4*7
|;;           .dc.b 4*6,4*6,4*6,4*6    | [83]
	.byte	4*6,4*6,4*6,4*6
|;;           .dc.b 4*1,4*1,4*13,4*13  | [84]
	.byte	4*1,4*1,4*13,4*13

|;;           .dc.b 4*0,4*15,4*0,4*15  | [86]
	.byte	4*0,4*15,4*0,4*15
|;;           .dc.b 4*0,4*14,4*1,4*15  | [87]
	.byte	4*0,4*14,4*1,4*15
|;;           .dc.b 4*0,4*13,4*2,4*15  | [88]
	.byte	4*0,4*13,4*2,4*15
|;;           .dc.b 4*0,4*12,4*3,4*15  | [89]
	.byte	4*0,4*12,4*3,4*15
|;;           .dc.b 4*0,4*11,4*4,4*15  | [90]
	.byte	4*0,4*11,4*4,4*15
|;;           .dc.b 4*0,4*10,4*5,4*15  | [91]
	.byte	4*0,4*10,4*5,4*15
|;;           .dc.b 4*0,4*9,4*6,4*15   | [92]
	.byte	4*0,4*9,4*6,4*15
|;;           .dc.b 4*0,4*8,4*7,4*15   | [93]
	.byte	4*0,4*8,4*7,4*15
|;;           .dc.b 4*0,4*7,4*8,4*15   | [94]
	.byte	4*0,4*7,4*8,4*15
|;;           .dc.b 4*0,4*6,4*9,4*15   | [95]
	.byte	4*0,4*6,4*9,4*15
|;;           .dc.b 4*0,4*5,4*10,4*15  | [96]
	.byte	4*0,4*5,4*10,4*15
|;;           .dc.b 4*0,4*4,4*11,4*15  | [97]
	.byte	4*0,4*4,4*11,4*15
|;;           .dc.b 4*0,4*3,4*12,4*15  | [98]
	.byte	4*0,4*3,4*12,4*15
|;;           .dc.b 4*0,4*2,4*13,4*15  | [99]
	.byte	4*0,4*2,4*13,4*15
|;;           .dc.b 4*0,4*1,4*14,4*15  | [100]
	.byte	4*0,4*1,4*14,4*15
|;;           .dc.b 4*0,4*0,4*15,4*15  | [101]
	.byte	4*0,4*0,4*15,4*15

toptable:
|;;           .dc.l top0,top1,top2,top3,top4,top5,top6,top7 | [104]
	.long	top0,top1,top2,top3,top4,top5,top6,top7
|;;           .dc.l top8,top9,topa,topb,topc,topd,tope,topf | [105]
	.long	top8,top9,topa,topb,topc,topd,tope,topf

twoptble:
|;;           .dc.l twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7 | [108]
	.long	twop0,twop1,twop2,twop3,twop4,twop5,twop6,twop7
|;;           .dc.l twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf | [109]
	.long	twop8,twop9,twopa,twopb,twopc,twopd,twope,twopf

|;;           .text                    | [111]
| 356 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [114]
|;;   sub.w     -24(a6),d1             | [115]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [116 PL]
      jpl      do_rot                  | [117]



|;;   neg.w     d1                     | [121]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [122]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [123]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [125]

|;;   add.w     d0,d0                  | [127]
	move.w   %d0,%d5
	add.l    %d0,%d5
      lea      lf_tab,%a2              | [128]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [129]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [130]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [131]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [136]
|;;   add.w     d0,d0                  | [137]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [138]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [140]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [141]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [144]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [145]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [146]
	andi.l   #(0xFFFF&0xF),%d5
|;;   add.w     d2,d2                  | [147]
	add.l    %d5,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [148]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [149]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [150]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [151]
      move.w   #0x8000,(-80,%a6)       | [152]
      moveq.l  #(-1),%d3               | [153]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [156 HI]
      jhi      doub_des                | [157]



|;;   add.w     d0,d0                  | [161]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [162]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [163]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [164]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [166]
	move.w   %d5,%d1
      jra      msk_done                | [167]



doub_des:
      move.w   %d0,%d1                 | [172]
|;;   lsr.w     #4,d1                  | [173]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [174]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [175 NE]
      jne      mlt_dest                | [176]



      move.w   (-24,%a6),%d1           | [180]
|;;   add.w     -20(a6),d1             | [181]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [182 CC]
      jcc      mlt_dest                | [183]
      moveq.l  #0,%d7                  | [184]
      jra      do_ritem                | [185]

mlt_dest:
      tst.w    (-42,%a6)               | [188 LT]
      jlt      mltleft                 | [189]
      moveq.l  #4,%d7                  | [190]
      jra      do_ritem                | [191]
mltleft:
      moveq.l  #8,%d7                  | [193]
do_ritem:
|;;   andi.w    #0x000F,d0             | [195 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jne      not_null                | [196]
|;;   subq.w    #1,d3                  | [197]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [198]
	move.w   %d5,%d3
not_null:
|;;   add.w     d0,d0                  | [200]
	move.w   %d0,%d5
	add.l    %d0,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [201]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [202]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [205]
      move.w   %d3,(-28,%a6)           | [206]
      move.w   %d4,(-36,%a6)           | [207]
|;;   addq.w    #2,d3                  | [208]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [209 CS]
      jcs      msk0                    | [210]
|;;   addq.w    #1,d3                  | [211]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [213]
      move.w   %d3,(-30,%a6)           | [214]
|***	RESTORE	AFTER d5	***
	move.l   (%a7)+,%d5

plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [217]
|;;   lsr.w     -84(a6)                | [218 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   addx.w    d0,d0                  | [219]
	move.w   %d0,%d1
	addx.l   %d0,%d1
|;;   lsr.w     -76(a6)                | [220 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   addx.w    d0,d0                  | [221]
	addx.l   %d1,%d1



      lea      toptable,%a4            | [225]
	move.w   %d1,%d0
|;;   move.b    wrmappin-toptable(a4,d0.w),d0 | [226]
	move.w   %d0,%a5
	move.b   (wrmappin-toptable,%a4,%a5.l*1),%d0
|;;   movea.l   toptable-toptable(a4,d0.w),a2 | [227]
	move.w   %d0,%a5
	movea.l  (toptable-toptable,%a4,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [228]
      movea.l  %a2,%a5                 | [229]

|;;   movea.l   twoptble-toptable(a4,d0.w),a4 | [231]
	move.w   %d0,%a0
	movea.l  (twoptble-toptable,%a4,%a0.l*1),%a4



      move.w   (-96,%a6),%d0           | [235 EQ]
      jeq      wrt_char                | [236]

      btst     #1,%d0                  | [238 EQ]
      jeq      no_lite                 | [239]



|;;   move.w    _LITEMASK,-78(a6)      | [243]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [244]
      lea      liteop,%a5              | [245]

      move.l   %a2,(-64,%a6)           | [247]
      lea      liteopwf,%a2            | [248]

      move.l   %a4,(-72,%a6)           | [250]
      lea      liteopw,%a4             | [251]

no_lite:
      btst     #0,%d0                  | [254 EQ]
      jeq      no_thick                | [255]



      clr.w    (-82,%a6)               | [259]

      move.l   %a5,(-48,%a6)           | [261]
      lea      thknop,%a5              | [262]

      move.l   %a2,(-60,%a6)           | [264]
      lea      thknopwf,%a2            | [265]

      move.l   %a4,(-68,%a6)           | [267]
      lea      thknopw,%a4             | [268]

no_thick:
      btst     #2,%d0                  | [271 EQ]
      jeq      wrt_char                | [272]



|;;   move.w    _SKEWMASK,-80(a6)      | [276]
	move.w   _SKEWMASK,%d1
	move.w   %d1,(-80,%a6)
      move.l   %a5,(-56,%a6)           | [277]
      lea      skewop,%a5              | [278]

      tst.w    (-124,%a6)              | [280 EQ GT]
      jgt      wrt_char                | [281]
      jeq      not_sngl                | [282]
      clr.w    (-28,%a6)               | [283]
      moveq.l  #0,%d7                  | [284]
      jra      chng_blt                | [285]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [287 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      jls      wrt_char                | [288]

      tst.w    (-42,%a6)               | [290 LT]
      jlt      chng_left               | [291]
      moveq.l  #4,%d7                  | [292]
      jra      chng_blt                | [293]
chng_left:
      moveq.l  #8,%d7                  | [295]
chng_blt:
      move.w   %d7,(-124,%a6)          | [297]

wrt_char:
      move.w   (-124,%a6),%d7          | [300 LE]
      jle      set_chr_hgt             | [301]
      movea.l  %a2,%a5                 | [302]
      move.w   (-18,%a6),%d5           | [303]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [305]
|;;   subq.w    #1,d3                  | [306]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [308]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [309]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [311 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jle      no_more_planes          | [312]
      movea.l  (-10,%a6),%a0           | [313]
      movea.l  (-14,%a6),%a1           | [314]
      addq.l   #2,%a1                  | [315]
      move.l   %a1,(-14,%a6)           | [316]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [317 EQ]
      jeq      plane_loop              | [318]
      jra      norm_blt                | [319]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [321]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [323]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [329]
      move.w   (-16,%a6),%d6           | [330]
      move.w   (-18,%a6),%d7           | [331]
      lea      sgl_rtn,%a3             | [332]
      jra      sgl_1st_scan            | [333]

sgl_lp:
|;;   adda.w    d6,a0                  | [336]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [337]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [339]
| 591 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [341]


      move.w   (-42,%a6),%d1           | [344 MI]
      jmi      left_rol                | [345]
      lsr.l    %d1,%d0                 | [346]
      jra      end_ro                  | [347]
left_rol:
      lsl.l    %d1,%d0                 | [349]
end_ro:
      swap     %d0                     | [351]
      move.w   %d0,%d1                 | [352]
      swap     %d0                     | [353]

      jmp      (%a5)                   | [355]

sgl_rtn:
      move.w   %d1,(%a1)               | [358]
|;;   dbra      d3,sgl_lp              | [359]
	tst.w    %d3
	jeq      _PA_358_
	subq.l   #1,%d3
	jra      sgl_lp
	_PA_358_:         
      jra      next_plane              | [360]




dbl_loop:
      movea.l  %a3,%a4                 | [366]
      move.w   (-16,%a6),%d6           | [367]
      move.w   (-18,%a6),%d7           | [368]
      lea      dbl_2wrd,%a2            | [369]
      move.l   %a2,%d5                 | [370]
      lea      dbl_1wrd,%a3            | [371]
      jra      dbl_1st_scan            | [372]

dbl_lp:
|;;   adda.w    d6,a0                  | [375]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [376]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [377]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [379]
| 638 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [381]


      move.w   (-42,%a6),%d1           | [384 MI]
      jmi      dbleft                  | [385]
      lsr.l    %d1,%d0                 | [386]
      jra      dblendr                 | [387]
dbleft:
      lsl.l    %d1,%d0                 | [389]
dblendr:
      swap     %d0                     | [391]
      move.w   %d0,%d1                 | [392]
      swap     %d0                     | [393]
      move.w   (-38,%a6),%d2           | [394]

      jmp      (%a5)                   | [396]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [399]
      move.w   %d0,%d1                 | [400]

      movea.w  (-88,%a6),%a2           | [402]
|;;   move.w    (a1,a2.w),d4           | [403]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [405]

|;;   exg       a3,d5                  | [407]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [408]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [411]
|;;   move.w    d1,(a1,a2.w)           | [412]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [414]
	tst.w    %d3
	jeq      _PA_413_
	subq.l   #1,%d3
	jra      dbl_lp
	_PA_413_:         
      jra      next_plane              | [415]





left_loop:
      movea.l  %d6,%a0                 | [422]
      movea.l  %d7,%a1                 | [423]
|;;   adda.w    -16(a6),a0             | [424]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [425]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [426]
|;;   rol.w     -78(a6)                | [427]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [429 NE]
      jne      skewopw                 | [430]
mlt_left:
      move.l   %a0,%d6                 | [432]
      move.l   %a1,%d7                 | [433]
      move.w   (-28,%a6),(-44,%a6)     | [434]







      move.l   (%a0)+,%d0              | [442]


      move.w   (%a1),%d4               | [445]

      move.w   (-42,%a6),%d1           | [447]
      lsl.l    %d1,%d0                 | [448]

      swap     %d0                     | [450]
      move.w   %d0,%d1                 | [451]
      swap     %d0                     | [452]

      move.w   (-38,%a6),%d2           | [454]

      lea      lft_lfrng,%a3           | [456]
      jmp      (%a5)                   | [457]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [460]
|;;   adda.w    -88(a6),a1             | [461]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [463]
      lea      lft_wrds,%a3            | [464]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [469]
|;;   and.w     d2,d4                  | [470]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [471]





      swap     %d0                     | [477]
      move.w   %d1,%d0                 | [478]
      swap     %d0                     | [479]

      move.w   (-42,%a6),%d1           | [481]
      lsl.l    %d1,%d0                 | [482]

      swap     %d0                     | [484]
      move.w   %d0,%d1                 | [485]
      swap     %d0                     | [486]

|;;   not.w     d2                     | [488]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [489]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [490]
	not.l    %d5
|;;   eor.w     d4,d1                  | [491]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [493]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [495 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      lef_don                 | [496]

      jmp      (%a4)                   | [498]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [501]
|;;   adda.w    -88(a6),a1             | [502]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_lef                | [504]

lef_don:
      move.w   (-36,%a6),%d2           | [507]

      lea      lft_rfrng,%a3           | [509]
      jmp      (%a5)                   | [510]

lft_rfrng:
      move.w   %d1,(%a1)               | [513]

|;;   dbra      d3,left_loop           | [515]
	tst.w    %d3
	jeq      _PA_514_
	subq.l   #1,%d3
	jra      left_loop
	_PA_514_:         
      jra      next_plane              | [516]





rite_loop:
      movea.l  %d6,%a0                 | [523]
      movea.l  %d7,%a1                 | [524]
|;;   adda.w    -16(a6),a0             | [525]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [526]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [527]
|;;   rol.w     -78(a6)                | [528]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [530 NE]
      jne      skewopw                 | [531]
mlt_rite:
      move.l   %a0,%d6                 | [533]
      move.l   %a1,%d7                 | [534]
      move.w   (-28,%a6),(-44,%a6)     | [535]
      move.w   (%a0)+,%d0              | [536]



      swap     %d0                     | [540]

      move.w   (%a1),%d4               | [542]

      move.w   (-42,%a6),%d1           | [544]
      lsr.l    %d1,%d0                 | [545]
      swap     %d0                     | [546]
      move.w   %d0,%d1                 | [547]
      swap     %d0                     | [548]

      move.w   (-38,%a6),%d2           | [550]

      lea      rgt_lfrng,%a3           | [552]
      jmp      (%a5)                   | [553]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [556]
|;;   adda.w    -88(a6),a1             | [557]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [559]
      lea      rgt_wrds,%a3            | [560]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [565]
|;;   and.w     d2,d4                  | [566]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [567]
      move.w   (%a0)+,%d0              | [568]



      swap     %d0                     | [572]

      move.w   (-42,%a6),%d1           | [574]
      lsr.l    %d1,%d0                 | [575]
      swap     %d0                     | [576]
      move.w   %d0,%d1                 | [577]
      swap     %d0                     | [578]

|;;   not.w     d2                     | [580]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [581]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [582]
	not.l    %d5
|;;   eor.w     d4,d1                  | [583]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [585]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [587 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jlt      rite_don                | [588]

      jmp      (%a4)                   | [590]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [593]
|;;   adda.w    -88(a6),a1             | [594]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jra      word_rit                | [596]

rite_don:
      move.w   (-36,%a6),%d2           | [599]

      lea      rgt_rfrng,%a3           | [601]
      jmp      (%a5)                   | [602]

rgt_rfrng:
      move.w   %d1,(%a1)               | [605]

|;;   dbra      d3,rite_loop           | [607]
	tst.w    %d3
	jeq      _PA_606_
	subq.l   #1,%d3
	jra      rite_loop
	_PA_606_:         
      jra      next_plane              | [608]
| 880 "vdi/vdi_tblit.S"
top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [611]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [612]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [613]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [614]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [615]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [618]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [619]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [620]
	not.l    %d0
|;;   and.w     d4,d1                  | [621]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [622]

top2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [625]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [626]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [627]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [628]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [629]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [632]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [633]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [634]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [635]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [638]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [639]
	not.l    %d0
|;;   and.w     d4,d1                  | [640]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [641]

top5:
      move.w   %d4,%d1                 | [644]
      jmp      (%a3)                   | [645]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [648]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [649]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [650]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [653]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [654]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [655]

top8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [658]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [659]
	or.l     %d4,%d0
|;;   eor.w     d2,d1                  | [660]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [661]

top9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [664]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [665]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [666]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [667]

topa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [670]
|;;   eor.w     d2,d1                  | [671]
	move.w   %d1,%d0
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [672]

topb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d2,d4                  | [675]
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   and.w     d2,d1                  | [676]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [677]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [678]

topc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [681]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [682]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [683]
	eor.l    %d4,%d0
|;;   eor.w     d2,d1                  | [684]
	eor.l    %d2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [685]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [688]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [689]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [690]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [691]

tope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [694]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [695]
	and.l    %d2,%d0
|;;   eor.w     d2,d4                  | [696]
	move.w   %d0,%d1
	move.w   %d4,%d0
	eor.l    %d2,%d0
|;;   or.w      d4,d1                  | [697]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [698]

topf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d2,d4                  | [701]
	mvz.w    %d2,%d0
	or.l     %d0,%d4
      move.w   %d4,%d1                 | [702]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [703]
| 986 "vdi/vdi_tblit.S"
twop0:
      moveq.l  #0,%d1                  | [706]
      jmp      (%a3)                   | [707]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [710]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [711]

twop2:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [714]
	move.w   %d4,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [715]
	move.w   %d0,%d4
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [716]

twop3:
      jmp      (%a3)                   | [719]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [722]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [723]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [724]

twop5:
      move.w   %d4,%d1                 | [727]
      jmp      (%a3)                   | [728]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [731]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [732]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [735]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [736]

twop8:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [739]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|;;   not.w     d1                     | [740]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [741]

twop9:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [744]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   not.w     d1                     | [745]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [746]

twopa:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d4,%d1                 | [749]
|;;   not.w     d1                     | [750]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [751]

twopb:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d4                     | [754]
	move.w   %d4,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [755]
	move.w   %d0,%d4
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [756]

twopc:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [759]
	move.w   %d1,%d0
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [760]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [763]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [764]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [765]

twope:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [768]
	move.w   %d1,%d0
	and.l    %d4,%d0
|;;   not.w     d1                     | [769]
	not.l    %d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [770]

twopf:
      moveq.l  #(-1),%d1               | [773]
      jmp      (%a3)                   | [774]
| 1073 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [777]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [778]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [779]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [780 EQ]
      jeq      thk00                   | [781]
|;;   ori.b     #0x10,ccr              | [782 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [783 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jcc      thk01                   | [784]
      move.w   #0x8000,%d6             | [785]
thk01:
      move.w   %d6,(-34,%a6)           | [787]
thk00:
      tst.w    (-28,%a6)               | [789 MI]
      jmi      thk0                    | [790]
      cmp.w    (-36,%a6),%d6           | [791 CC]
      jcc      thk0                    | [792]
|;;   and.w     d6,d0                  | [793]
	and.l    %d6,%d0
      jra      thk1                    | [794]
thk0:
      clr.w    %d0                     | [796]
|;;   and.w     d6,d1                  | [797]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [799]
      move.w   %d2,%d5                 | [800]
      swap     %d5                     | [801]
      move.w   (-36,%a6),%d5           | [802]
      lsl.l    %d6,%d5                 | [803]

      swap     %d5                     | [805]
      move.w   %d5,%d7                 | [806]
      swap     %d5                     | [807]

|;;   and.w     d7,d1                  | [809]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [810]
	and.l    %d5,%d0
|;;   swap      d1                     | [811]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [812]
|;;   subq.w    #1,d6                  | [813]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [815]
      lsr.l    #1,%d0                  | [816]
      or.l     %d0,%d1                 | [817]
|;;   dbra      d6,thkoplp             | [818]
	tst.w    %d6
	jeq      _PA_817_
	subq.l   #1,%d6
	jra      thkoplp
	_PA_817_:         

      move.w   %d1,%d0                 | [820]
      swap     %d1                     | [821]
|;;   movem.l   (sp)+,d5-d7            | [822]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [823]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [824]
| 1136 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [827]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [828 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk2                    | [829]
|;;   and.w     -34(a6),d1             | [830]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [832]
      swap     %d1                     | [833]
      clr.w    %d1                     | [834]
      move.l   %d1,%d5                 | [835]
|;;   subq.w    #1,d6                  | [836]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [838]
      or.l     %d5,%d1                 | [839]
|;;   dbra      d6,thkopwlp            | [840]
	tst.w    %d6
	jeq      _PA_839_
	subq.l   #1,%d6
	jra      thkopwlp
	_PA_839_:         

      move.w   %d1,%d5                 | [842]
      swap     %d1                     | [843]
|;;   or.w      -82(a6),d1             | [844]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [845]
|;;   movem.l   (sp)+,d5-d6            | [846]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [847]
      jmp      (%a2)                   | [848]
| 1174 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [851]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [852]
|;;   subq.w    #1,-30(a6)             | [853 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jmi      thk3                    | [854]
      jeq      thk4                    | [855]

|;;   subq.w    #1,-30(a6)             | [857 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jne      thk11                   | [858]
|;;   and.w     d6,d1                  | [859]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [861]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jra      thk7                    | [862]

thk3:
      clr.w    %d1                     | [865]
      jra      thk5                    | [866]

thk4:
|;;   and.w     d6,d1                  | [869]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [872 MI]
      jmi      thk6                    | [873]
      move.w   (-28,%a6),%d5           | [874]
|;;   addq.w    #2,d5                  | [875]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [876]
|;;   ori.b     #0x10,ccr              | [877 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [878 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jcc      thk33                   | [879]
      move.w   #0x8000,%d7             | [880]
|;;   addq.w    #1,d5                  | [881]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [883 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jne      thk34                   | [884]
|;;   subq.w    #1,d5                  | [885]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [887 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [888 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jcc      thk31                   | [889]
      move.w   #0x8000,%d6             | [890]
thk31:
      cmp.w    %d6,%d7                 | [892 CS]
      jcs      thk35                   | [893]
|;;   addq.w    #1,d5                  | [894]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [896]
      move.w   %d5,(-32,%a6)           | [897]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [899]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [901]
      swap     %d1                     | [902]
      clr.w    %d1                     | [903]
      move.l   %d1,%d5                 | [904]
|;;   subq.w    #1,d6                  | [905]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [907]
      or.l     %d5,%d1                 | [908]
|;;   dbra      d6,thkopwl             | [909]
	tst.w    %d6
	jeq      _PA_908_
	subq.l   #1,%d6
	jra      thkopwl
	_PA_908_:         

      move.w   %d1,%d5                 | [911]
      swap     %d1                     | [912]
|;;   or.w      -82(a6),d1             | [913]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [914]
|;;   and.w     d2,d1                  | [915]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [916]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [917]
      jmp      (%a2)                   | [918]
| 1255 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [921]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [922]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [923]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [924]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [925]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [928]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [929]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [930]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [933]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [934]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [935]
| 1287 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [938 CC]
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
      jcc      no_shift                | [939]

      swap     %d1                     | [941]
      move.w   %d0,%d1                 | [942]
      lsr.l    #1,%d1                  | [943]

      swap     %d2                     | [945]
      move.w   (-36,%a6),%d2           | [946]
      lsr.l    #1,%d2                  | [947]
      move.w   %d2,(-36,%a6)           | [948]
      swap     %d2                     | [949]
      move.w   %d2,(-38,%a6)           | [950 EQ]
      jeq      nxt_word                | [951]
      move.w   (-42,%a6),%d0           | [952 MI]
      jmi      dec_rol                 | [953]
ror_add:
|;;   addq.w    #1,d0                  | [955]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [957]
      move.w   %d1,%d0                 | [958]
      swap     %d1                     | [959]
no_shift:
      movea.l  (-56,%a6),%a2           | [961]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [962]

dec_rol:
      tst.b    %d0                     | [965 EQ]
      jeq      begn_ror                | [966]
|;;   subq.w    #1,d0                  | [967]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jra      new_shif                | [968]
begn_ror:
      clr.w    %d0                     | [970]
      jra      ror_add                 | [971]



nxt_word:
      move.w   %d2,(-36,%a6)           | [976]
      swap     %d2                     | [977]
      move.w   %d2,(-38,%a6)           | [978]
|;;   adda.w    -88(a6),a1             | [979]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [980]

      moveq.l  #15,%d0                 | [982]
|;;   sub.w     -42(a6),d0             | [983]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [984]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [985]
      jra      no_shift                | [986]






skewopw:
|;;   rol.w     -80(a6)                | [994 CC]
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
      jcc      do_shift                | [995]

      tst.w    (-42,%a6)               | [997 MI]
      jmi      mlt_left                | [998]
      jra      mlt_rite                | [999]
do_shift:
|;;   ori       #0x10,ccr              | [1001 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1002]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1004]
      cmp.w    #0xFFFF,%d0             | [1005 EQ]
      jeq      inc_rite                | [1006]

|;;   ori       #0x10,ccr              | [1008 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1009]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1010]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1013]
      tst.b    %d0                     | [1014 NE]
      jne      no_rota                 | [1015]
      move.w   #0x8000,(-40,%a6)       | [1016]
no_rota:
      move.w   (-38,%a6),%d1           | [1018]
|;;   lsr.w     #1,d1                  | [1019 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jeq      wnxt_wrd                | [1020]

      move.w   %d1,(-38,%a6)           | [1022]

      tst.w    %d0                     | [1024 MI]
      jmi      wdec_rol                | [1025]
|;;   addq.w    #1,-42(a6)             | [1026]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jra      mlt_rite                | [1027]
wdec_rol:
      tst.b    %d0                     | [1029 EQ]
      jeq      set_msk                 | [1030]
|;;   subq.w    #1,-42(a6)             | [1031]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jra      mlt_left                | [1032]
set_msk:
      move.w   #1,(-42,%a6)            | [1034]
      jra      mlt_rite                | [1035]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1038]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1039]
      jra      do_left                 | [1040]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1043]

|;;   subq.w    #1,-28(a6)             | [1045]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1046]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1048]
|;;   sub.w     d0,d2                  | [1049]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1050]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1051]

      jra      mlt_left                | [1053]
| 1413 "vdi/vdi_tblit.S"
getnextbuf:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      move.w   %d0,-(%a7)              | [1056]
      movea.l  _SCRTCHP,%a1            | [1057]
      move.w   _SCRPT2,%d0             | [1058]
|;;   sub.w     -2(a6),d0              | [1059]
	move.w   (-2,%a6),%d1
	sub.l    %d1,%d0
      move.w   %d0,(-2,%a6)            | [1060]
|;;   adda.w    d0,a1                  | [1061]
	ext.l    %d0
	adda.l   %d0,%a1
      move.w   (%a7)+,%d0              | [1062]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1063]

currbuf:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      movea.l  _SCRTCHP,%a1            | [1066]
|;;   adda.w    -2(a6),a1              | [1067]
	mvs.w    (-2,%a6),%d0
	adda.l   %d0,%a1
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      rts                              | [1068]






replicat:
|***	SAVE	d5-d6	***
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
      move.w   _SOURCEX,%d0            | [1076]
      move.w   %d0,%d4                 | [1077]
|;;   andi.w    #0x0F,d0               | [1078]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1079]
|;;   lsr.w     #4,d4                  | [1080]
	and.l    #0xffff,%d4
	lsr.l    #4,%d4
|;;   add.w     d4,d4                  | [1081]
	add.l    %d4,%d4

      move.w   _SOURCEY,%d0            | [1083]
      mulu.w   (-16,%a6),%d0           | [1084]
      movea.l  (-10,%a6),%a0           | [1085]
|;;   adda.w    d4,a0                  | [1086]
	mvs.w    %d4,%d3
	adda.l   %d3,%a0
      adda.l   %d0,%a0                 | [1087]

      move.w   (-24,%a6),%d2           | [1089]
      move.w   #0x8000,%d3             | [1090]
      move.w   %d3,%d4                 | [1091]
|;;   lsr.w     d2,d3                  | [1092]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1094]
      move.w   (-102,%a6),%d1          | [1095]

      move.w   %d2,(-22,%a6)           | [1097]
      move.w   %d1,(-20,%a6)           | [1098]
      jbsr     getnextbuf              | [1099]

|;;   lsr.w     #3,d1                  | [1101]
	and.l    #0xffff,%d1
	lsr.l    #3,%d1
|;;   add.w     d1,d1                  | [1102]
	add.l    %d1,%d1
|;;   addq.w    #2,d1                  | [1103 X]
	ori.l    #0xffff0000,%d1
	addq.l   #2,%d1
noline1:
      move.w   %d1,(-18,%a6)           | [1105]



      move.w   _SCALDIR,%d7            | [1109]
|;;   roxr.l    #1,d7                  | [1110 X]
	move.l   %d7,%d5
	addx.l   %d5,%d5
	moveq    #32-1,%d6
	lsl.l    %d6,%d5
	lsr.l    #1,%d7
	or.l     %d5,%d7
      moveq.l  #0,%d7                  | [1111]
|;;   roxr.l    #1,d7                  | [1112]
	move.l   %d7,%d5
	addx.l   %d5,%d5
	moveq    #32-1,%d6
	lsl.l    %d6,%d5
	lsr.l    #1,%d7
	or.l     %d5,%d7

      move.w   (-16,%a6),%d7           | [1114]
      move.w   _DDAINC,%d2             | [1115]
|***	RESTORE	AFTER d5	***
	movem.l  (%a7),%d5
      move.w   (-22,%a6),%d5           | [1116]
|;;   subq.w    #1,d5                  | [1117]
	subq.l   #1,%d5
|***	RESTORE	AFTER d6	***
	movem.l  4(%a7),%d6
	lea      8(%a7),%a7
      move.w   #32767,%d6              | [1118]
      tst.l    %d7                     | [1119 MI]
      jmi      rep_ylop                | [1120]
y_dwn_lp:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1122 CC]
	and.l    #0xffff,%d2
	ori.l    #0xffff0000,%d6
	add.l    %d2,%d6
      jcc      y_no_drw                | [1123]
      jbsr     yloop                   | [1124]
y_no_drw:
|;;   adda.w    d7,a0                  | [1126]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,y_dwn_lp            | [1127]
	tst.w    %d5
	jeq      _PA_1126_
	subq.l   #1,%d5
	jra      y_dwn_lp
	_PA_1126_:         
      jra      y_rep_don               | [1128]
rep_ylop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   add.w     d2,d6                  | [1130 CC]
	mvz.w    %d2,%d0
	ori.l    #0xffff0000,%d6
	add.l    %d0,%d6
      jcc      y_no_rep                | [1131]
      jbsr     yloop                   | [1132]
y_no_rep:
      jbsr     yloop                   | [1134]
|;;   adda.w    d7,a0                  | [1135]
	mvs.w    %d7,%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   dbra      d5,rep_ylop            | [1136]
	tst.w    %d5
	jeq      _PA_1135_
	subq.l   #1,%d5
	jra      rep_ylop
	_PA_1135_:         
y_rep_don:
      move.w   (-102,%a6),%d2          | [1138]
|;;   subq.w    #1,d2                  | [1139]
	subq.l   #1,%d2
      move.w   _XDDA,%d1               | [1140]
      move.w   _DDAINC,%d0             | [1141]
      moveq.l  #0,%d3                  | [1142]
repwidcl:
|;;   add.w     d0,d1                  | [1144 CC]
	and.l    #0xffff,%d0
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
      jcc      nrepdoub                | [1145]
|;;   addq.w    #1,d3                  | [1146]
	addq.l   #1,%d3
nrepdoub:
      tst.l    %d7                     | [1148 PL]
      jpl      nrpndoub                | [1149]
|;;   addq.w    #1,d3                  | [1150]
	addq.l   #1,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1152]
	tst.w    %d2
	jeq      _PA_1151_
	subq.l   #1,%d2
	jra      repwidcl
	_PA_1151_:         
      move.w   %d1,_XDDA               | [1153]
      move.w   %d3,(-102,%a6)          | [1154]
      move.w   (-92,%a6),(-106,%a6)    | [1155]


      move.w   (-18,%a6),%d1           | [1158]
      move.w   %d1,(-16,%a6)           | [1159]

      jbsr     currbuf                 | [1161]
      move.l   %a1,(-10,%a6)           | [1162]

      clr.w    _SOURCEX                | [1164]
      clr.w    _SOURCEY                | [1165]
      rts                              | [1166]
| 1555 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d7,-(sp)            | [1169]
	lea      -20(%a7),%a7
	movem.l  %d3-%d7,(%a7)
      movea.l  %a0,%a3                 | [1170]
      movea.l  %a1,%a4                 | [1171]
      moveq.l  #0,%d0                  | [1172]
      move.w   (-20,%a6),%d5           | [1173]
|;;   subq.w    #1,d5                  | [1174]
	subq.l   #1,%d5

      move.w   _XDDA,%d7               | [1176]
      jra      nextsrc                 | [1177]
innerlp:
|;;   ror.w     #1,d3                  | [1179 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jcc      reploop                 | [1180]
nextsrc:
      movea.w  (%a3)+,%a2              | [1182]
reploop:
      move.w   %a2,%d6                 | [1184]
|;;   and.w     d3,d6                  | [1185 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jne      nrepnor                 | [1186]


repnor:
      tst.l    %d7                     | [1190 MI]
      jmi      repnorup                | [1191]
|;;   add.w     d2,d7                  | [1192 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      incsrc                  | [1193]
      jra      ordone                  | [1194]
repnorup:
|;;   add.w     d2,d7                  | [1196 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      ordone                  | [1197]


|;;   ror.w     #1,d4                  | [1200 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jcc      ordone                  | [1203]
      move.w   %d0,(%a4)+              | [1204]
      moveq.l  #0,%d0                  | [1205]


      jra      ordone                  | [1208]
nrepnor:


|;;   add.w     d2,d7                  | [1212 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jcc      o_no_rep                | [1213]


|;;   or.w      d4,d0                  | [1216]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1217 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jcc      o_no_rep                | [1219]
      move.w   %d0,(%a4)+              | [1220]
      moveq.l  #0,%d0                  | [1221]
o_no_rep:
      tst.l    %d7                     | [1223 PL]
      jpl      incsrc                  | [1224]


|;;   or.w      d4,d0                  | [1227]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1229 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jcc      incsrc                  | [1230]
nextdst:
      move.w   %d0,(%a4)+              | [1232]
      moveq.l  #0,%d0                  | [1233]
incsrc:
|;;   dbra      d5,innerlp             | [1235]
	tst.w    %d5
	jeq      _PA_1234_
	subq.l   #1,%d5
	jra      innerlp
	_PA_1234_:         
repdone:
      move.w   %d0,(%a4)               | [1237]
|;;   movem.l   (sp)+,d3-d7            | [1238]
	movem.l  (%a7),%d3-%d7
	lea      20(%a7),%a7
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1239]
	ext.l    %d1
	adda.l   %d1,%a1
      rts                              | [1240]

| End section on aligned boundary
	.p2align 2,0
