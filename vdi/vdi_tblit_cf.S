
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
| Translated on Sun Mar 27 00:31:44 2016 by version 2.6.0 of PortAsm translator
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
|;;           .globl _text_blt         | [14]
	.globl	_text_blt
|;;           .globl concat            | [15]
	.globl	concat





|;;           .extern _act_siz         | [21]
	.extern	_act_siz





|;;           .extern _DEV_TAB         | [27]
	.extern	_DEV_TAB
|;;           .extern _SOURCEX,_SOURCEY | [28]
	.extern	_SOURCEX
	.extern	_SOURCEY
|;;           .extern _DESTX,_DESTY    | [29]
	.extern	_DESTX
	.extern	_DESTY
|;;           .extern _DELX,_DELY      | [30]
	.extern	_DELX
	.extern	_DELY
|;;           .extern _FBASE           | [31]
	.extern	_FBASE
|;;           .extern _FWIDTH          | [32]
	.extern	_FWIDTH
|;;           .extern _WRT_MODE        | [33]
	.extern	_WRT_MODE
|;;           .extern _STYLE           | [34]
	.extern	_STYLE
|;;           .extern _R_OFF,_L_OFF    | [35]
	.extern	_R_OFF
	.extern	_L_OFF
|;;           .extern _WEIGHT,_LITEMASK,_SKEWMASK | [36]
	.extern	_WEIGHT
	.extern	_LITEMASK
	.extern	_SKEWMASK
|;;           .extern _DOUBLE          | [37]
	.extern	_DOUBLE
|;;           .extern _CHUP            | [38]
	.extern	_CHUP
|;;           .extern _CLIP            | [39]
	.extern	_CLIP
|;;           .extern _MONO_STATUS     | [40]
	.extern	_MONO_STATUS
|;;           .extern _T_SCLSTS        | [41]
	.extern	_T_SCLSTS
|;;           .extern _TEXT_FG         | [42]
	.extern	_TEXT_FG
|;;           .extern _XMN_CLIP,_XMX_CLIP | [43]
	.extern	_XMN_CLIP
	.extern	_XMX_CLIP
|;;           .extern _YMN_CLIP,_YMX_CLIP | [44]
	.extern	_YMN_CLIP
	.extern	_YMX_CLIP
|;;           .extern _XACC_DDA        | [45]
	.extern	_XACC_DDA
|;;           .extern _DDA_INC         | [46]
	.extern	_DDA_INC
|;;           .extern _scrpt2          | [47]
	.extern	_scrpt2
|;;           .extern _scrtchp         | [48]
	.extern	_scrtchp
|;;           .extern _shift_offset    | [49]
	.extern	_shift_offset






|;;           .extern _v_bas_ad        | [56]
	.extern	_v_bas_ad
|;;           .extern _v_lin_wr        | [57]
	.extern	_v_lin_wr
|;;           .extern _v_planes        | [58]
	.extern	_v_planes





|;;           .globl _deftxbuf         | [64]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [65]
	.globl	_scrtsiz
| 341 "vdi/vdi_tblit.S"
|;;           .bss                     | [67]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [69]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [71]
	.p2align 2,0
	.text



|;;           .even                    | [75]
	.p2align 1,0
_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [77]
	.short	(2+6+8+15)/16*2*16*2
| 458 "vdi/vdi_tblit.S"
|;;           .text                    | [79]




	.p2align 1,0
_text_blt:
      link.w   %a6,#(-126)             | [85]
|;;   movem.l   d2-d7/a2-a5,-(sp)      | [86]
	lea      -40(%a7),%a7
	movem.l  %d2-%d7/%a2-%a5,(%a7)

      clr.w    (-94,%a6)               | [88]
      move.w   #0,(-6,%a6)             | [89]
      lea      (-96,%a6),%a1           | [90]
      move.w   _STYLE,(%a1)            | [91]
      move.w   _WRT_MODE,-(%a1)        | [92]
      move.w   _SKEWMASK,-(%a1)        | [93]

      move.w   _DELX,-(%a1)            | [95]
      move.w   _DESTX,-(%a1)           | [96]
      move.w   _DELY,-(%a1)            | [97]
      move.w   _DESTY,-(%a1)           | [98]
      move.w   _CHUP,-(%a1)            | [99]

      movea.w  _scrpt2,%a0             | [101]
      move.w   (-106,%a6),%d3          | [102]
      tst.w    _DOUBLE                 | [103 NE]
      jbne     adr_dbl                 | [104]
      move.w   (-102,%a6),%d1          | [105]
      jbra     adr_nodb                | [106]



adr_dbl:
|;;   movem.l   a0/a1,-(sp)            | [111]
	lea      -8(%a7),%a7
	movem.l  %a0-%a1,(%a7)
      move.w   %d3,-(%a7)              | [112]
      move.l   (8,%a6),-(%a7)          | [113]
      jsr      _act_siz                | [114]
      addq.l   #6,%a7                  | [115]
|;;   movem.l   (sp)+,a0/a1            | [116]
	movem.l  (%a7),%a0-%a1
	lea      8(%a7),%a7
      move.w   %d0,%d3                 | [117]
      move.w   %d3,(-92,%a6)           | [118]
      move.w   (-102,%a6),%d2          | [119]
      move.w   _XACC_DDA,%d4           | [120]
      move.w   _DDA_INC,%d0            | [121]
      moveq.l  #0,%d1                  | [122]
      jbra     clc_n_su                | [123]
doub_wid:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   add.w     d0,d4                  | [125 CC]
	mvz.w    %d0,%d2
	move.w   %d4,%d3
	ori.l    #0xffff0000,%d3
	add.l    %d2,%d3
	move     %ccr,%d5
	move.w   %d3,%d4
	move     %d5,%ccr
      jbcc     no_db_wd                | [126]
|;;   addq.w    #1,d1                  | [127]
	move.w   %d1,%d2
	addq.l   #1,%d2
	move.w   %d2,%d1
no_db_wd:
|***	RESTORE	d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
|;;   btst.b    #0,_T_SCLSTS+1         | [129 EQ]
	moveq.l  #0,%d5
	btst     %d5,_T_SCLSTS+1
      jbeq     clc_n_su                | [130]
|;;   addq.w    #1,d1                  | [131]
	move.w   %d1,%d5
	addq.l   #1,%d5
	move.w   %d5,%d1
clc_n_su:
|;;   dbra      d2,doub_wid            | [133]
	tst.w    %d2
	jbeq     _PA_132_
	subq.l   #1,%d2
	jbra     doub_wid
	_PA_132_:         
      move.w   %d1,(-90,%a6)           | [134]
| 522 "vdi/vdi_tblit.S"
      tst.w    (-110,%a6)              | [136 NE]
      jbne     adr_nodb                | [137]



      movea.w  #0,%a0                  | [141]
adr_nodb:
      move.w   %a0,(-2,%a6)            | [143]
      move.w   (-108,%a6),%d2          | [144]
      move.w   (-104,%a6),%d0          | [145]
      move.w   _L_OFF,%d6              | [146]
      move.w   _R_OFF,%d7              | [147]
      clr.w    (-74,%a6)               | [148]
      btst     #0,(-96+1,%a6)          | [149 EQ]
      jbeq     adr_notk                | [150]
      move.w   _WEIGHT,%d4             | [151 NE]
      jbne     adr_th1                 | [152]
|;;   and.w     #0xFFFF-0x1,-96(a6)    | [153]
	move.w   (-96,%a6),%d5
	andi.l   #(0xFFFF&(0xFFFF-0x1)),%d5
	move.w   %d5,(-96,%a6)
adr_th1:
      tst.w    _MONO_STATUS            | [155 NE]
      jbne     adr_notk                | [156]

|;;   add.w     d4,d1                  | [158]
	move.w   %d1,%d5
	add.l    %d4,%d5
	move.w   %d5,%d1
adr_notk:
      btst     #2,(-96+1,%a6)          | [160 EQ]
      jbeq     chk_addr                | [161]

|;;   add.w     d6,d1                  | [163]
	move.w   %d1,%d5
	add.l    %d6,%d5
|;;   add.w     d7,d1                  | [164]
	add.l    %d7,%d5
	move.w   %d5,%d1







chk_addr:
      move.w   (-110,%a6),%d4          | [173 EQ]
      jbeq     chk_clip                | [174]
      move.w   #0,(-4,%a6)             | [175]
|;;   subi.w    #1800,d4               | [176]
	move.w   %d4,%d5
	subi.l   #1800,%d5
	move.w   %d5,%d4
      move.w   %d4,(-126,%a6)          | [177 EQ GT]
      jbeq     add180                  | [178]
      jbgt     add270                  | [179]



|;;   sub.w     d1,d2                  | [183]
	sub.l    %d1,%d2
      move.w   %d2,(-108,%a6)          | [184]
|;;   exg       d1,d3                  | [185]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3
      jbra     chk_clip                | [186]



add180:
|;;   sub.w     d1,d0                  | [191]
	move.w   %d0,%d5
	sub.l    %d1,%d5
|;;   move.w    d0,-104(a6)            | [192]
	move.w   %d5,(-104,%a6)
	move.w   %d5,%d0
      jbra     chk_clip                | [193]



add270:
|;;   exg       d1,d3                  | [198]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3
| 595 "vdi/vdi_tblit.S"
chk_clip:
      moveq.l  #0,%d5                  | [201]
      move.w   _CLIP,-(%a1)            | [202 EQ]
      jbeq     clp_done                | [203]
      move.w   _XMN_CLIP,-(%a1)        | [204]
      move.w   _YMN_CLIP,-(%a1)        | [205]
      move.w   _XMX_CLIP,-(%a1)        | [206]
      move.w   _YMX_CLIP,-(%a1)        | [207]



      cmp.w    (-116,%a6),%d2          | [211 GE]
      jbge     chk_ymx                 | [212]



|;;   add.w     d3,d2                  | [216]
	add.l    %d3,%d2
      cmp.w    (-116,%a6),%d2          | [217 GT]
      jbgt     chk_xmn                 | [218]
      jbra     upda_dst                | [219]
chk_ymx:
      cmp.w    (-120,%a6),%d2          | [221 GT]
      jbgt     upda_dst                | [222]

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   add.w     d3,d2                  | [224]
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [225]
	subq.l   #1,%d2
|;;   sub.w     -120(a6),d2            | [226 LE]
	ext.l    %d2
	mvs.w    (-120,%a6),%d0
	sub.l    %d0,%d2
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
	lea      4(%a7),%a7
      jble     chk_xmn                 | [227]



|;;   andi.w    #1,d2                  | [231 EQ]
	andi.l   #(0xFFFF&1),%d2
      jbeq     chk_xmn                 | [232]
      btst     #4,(-96+1,%a6)          | [233 NE]
      jbne     chk_xmn                 | [234]
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   rol.w     -100(a6)               | [235]
	clr.l    %d2
	move.w   (-100,%a6),%d0
	move.w   %d0,%d2
	lsl.l    #1,%d0
	lsr.l    #8,%d2
	lsr.l    #7,%d2
	or.l     %d2,%d0
	move.w   %d0,(-100,%a6)
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



chk_xmn:
      cmp.w    (-114,%a6),%d0          | [240 GE]
      jbge     chk_xmx                 | [241]



|;;   add.w     d1,d0                  | [245]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   not.w     d5                     | [246]
	move.w   %d2,%d0
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5
      cmp.w    (-114,%a6),%d0          | [247 GT]
      jbgt     clp_done                | [248]
      jbra     upda_dst                | [249]
chk_xmx:
      cmp.w    (-118,%a6),%d0          | [251 GT]
      jbgt     upda_dst                | [252]

|;;   add.w     d1,d0                  | [254]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   subq.w    #1,d0                  | [255]
	subq.l   #1,%d2
	move.w   %d2,%d0
      cmp.w    (-118,%a6),%d0          | [256 LE]
      jble     clp_done                | [257]
|;;   not.w     d5                     | [258]
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5



clp_done:
      clr.w    (-28,%a6)               | [263]

|;;   move.w    _FWIDTH,-16(a6)        | [265]
	move.w   _FWIDTH,%d2
	move.w   %d2,(-16,%a6)
|;;   move.l    _FBASE,-10(a6)         | [266]
	move.l   _FBASE,%d2
	move.l   %d2,(-10,%a6)

chk_skw:
      move.w   (-96,%a6),%d1           | [269]
|;;   andi.w    #0x4 +0x1 +0x10,d1     | [270 EQ]
	andi.l   #(0xFFFF0000!~(0x4+0x1+0x10)),%d1
	tst.w    %d1
      jbeq     chk_rota                | [271]



chk_chup:
      tst.w    (-110,%a6)              | [276 NE]
      jbne     preblt                  | [277]

      btst     #2,%d1                  | [279 EQ]
      jbeq     preskew                 | [280]
      tst.w    %d5                     | [281 NE]
      jbne     preblt                  | [282]
preskew:
      btst     #4,%d1                  | [284 EQ]
      jbeq     chk_rota                | [285]






preblt:
|***	SAVE	d4	***
	lea      -4(%a7),%a7
	movem.l  %d4,(%a7)
      move.w   _SOURCEX,%d0            | [293]
      move.w   %d0,%d2                 | [294]
|;;   andi.w    #0x0F,d2               | [295]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [296]
|;;   lsr.w     #4,d0                  | [297]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [298]
	lsl.l    #1,%d0

      move.w   _SOURCEY,%d2            | [300]
      move.w   (-106,%a6),(-22,%a6)    | [301]
|;;   add.w     -22(a6),d2             | [302]
	move.w   (-22,%a6),%d3
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [303]
	subq.l   #1,%d2

      mulu.w   (-16,%a6),%d2           | [305]
|;;   neg.w     -16(a6)                | [306]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   add.w     d0,d2                  | [307]
	move.w   %d2,%d4
	add.l    %d0,%d4
      movea.l  (-10,%a6),%a0           | [308]
	move.w   %d4,%d2
|;;   lea       (a0,d2.w),a0           | [309]
	move.w   %d2,%a5
	lea      (0,%a0,%a5.l*1),%a0

      move.w   (-102,%a6),%d0          | [311]
      move.w   _WEIGHT,%d1             | [312]
|;;   add.w     d6,d7                  | [313]
	add.l    %d6,%d7
      tst.w    _DOUBLE                 | [314 EQ]
      jbeq     full_wt                 | [315]
|;;   lsr.w     #1,d1                  | [316 NE]
	mvz.w    %d1,%d4
	lsr.l    #1,%d4
	move     %ccr,%d3
	move.w   %d4,%d1
	move     %d3,%ccr
      jbne     ful_wtok                | [317]
|;;   addq.w    #1,d1                  | [318]
	addq.l   #1,%d4
	move.w   %d4,%d1
ful_wtok:
|;;   lsr.w     #1,d7                  | [320]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
full_wt:
      btst     #0,(-96+1,%a6)          | [322 EQ]
      jbeq     no_smear                | [323]

|;;   add.w     d1,d0                  | [325]
	add.l    %d1,%d0
      move.w   %d1,(-74,%a6)           | [326]
no_smear:

      clr.w    (-26,%a6)               | [329]
      move.w   (-106,%a6),%d1          | [330]
      move.w   (-96,%a6),%d2           | [331]
      btst     #4,%d2                  | [332 EQ]
      jbeq     no_out                  | [333]
      tst.w    _DOUBLE                 | [334 NE]
      jbne     no_out                  | [335]

|;;   addq.w    #3,d0                  | [337]
	addq.l   #3,%d0
|;;   addq.w    #1,-26(a6)             | [338]
	move.w   (-26,%a6),%d4
	addq.l   #1,%d4
	move.w   %d4,(-26,%a6)
|;;   addq.w    #2,-106(a6)            | [339]
	move.w   (-106,%a6),%d4
	addq.l   #2,%d4
	move.w   %d4,(-106,%a6)
|;;   addq.w    #3,d1                  | [340]
	move.w   %d1,%d4
	addq.l   #3,%d4
	move.w   %d4,%d1
no_out:

      move.w   %d0,(-20,%a6)           | [343]
|;;   add.w     d7,d0                  | [344]
	add.l    %d7,%d0
      move.w   %d0,(-102,%a6)          | [345]
|;;   lsr.w     #4,d0                  | [346]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [347]
	lsl.l    #1,%d0
|;;   addq.w    #2,d0                  | [348]
	addq.l   #2,%d0
|;;   neg.w     d0                     | [349]
	neg.l    %d0
      move.w   %d0,(-18,%a6)           | [350]
|;;   neg.w     d0                     | [351]
	neg.l    %d0
|;;   subq.w    #1,d1                  | [352]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [353]
	mulu.w   %d4,%d0
      movea.l  _scrtchp,%a1            | [354]
|;;   adda.w    -2(a6),a1              | [355]
	mvs.w    (-2,%a6),%d3
	adda.l   %d3,%a1
      move.l   %a1,(-10,%a6)           | [356]
      btst     #4,%d2                  | [357 NE]
      jbne     do_clear                | [358]
      btst     #2,%d2                  | [359 EQ]
      jbeq     no_clear                | [360]
do_clear:

      movea.l  %a1,%a2                 | [363]
      move.w   %d0,%d3                 | [364]
|;;   sub.w     -18(a6),d3             | [365]
	move.w   (-18,%a6),%d1
	sub.l    %d1,%d3
|;;   lsr.w     #1,d3                  | [366]
	and.l    #0xffff,%d3
	lsr.l    #1,%d3
|;;   subq.w    #1,d3                  | [367]
	subq.l   #1,%d3
      moveq.l  #0,%d1                  | [368]
replp:
      move.w   %d1,(%a2)+              | [370]
|;;   dbra      d3,replp               | [371]
	tst.w    %d3
	jbeq     _PA_370_
	subq.l   #1,%d3
	jbra     replp
	_PA_370_:         

      btst     #4,%d2                  | [373 EQ]
      jbeq     no_clear                | [374]
      tst.w    _DOUBLE                 | [375 NE]
      jbne     no_clear                | [376]

|;;   subq.w    #3,-20(a6)             | [378]
	move.w   (-20,%a6),%d4
	subq.l   #3,%d4
	move.w   %d4,(-20,%a6)
|;;   subq.w    #1,-102(a6)            | [379]
	move.w   (-102,%a6),%d4
	subq.l   #1,%d4
	move.w   %d4,(-102,%a6)
|;;   add.w     -18(a6),d0             | [380]
	move.w   %d0,%d4
	move.w   (-18,%a6),%d3
	add.l    %d3,%d4
	move.w   %d4,%d0



no_clear:
|;;   adda.w    d0,a1                  | [385]
	mvs.w    %d0,%d3
	adda.l   %d3,%a1



      move.w   (-96,%a6),(-122,%a6)    | [389]

      clr.w    (-98,%a6)               | [391]
      move.w   #1,(-84,%a6)            | [392]
      clr.w    (-76,%a6)               | [393]

      move.w   #1,(-86,%a6)            | [395]
      move.w   #2,(-88,%a6)            | [396]
|;;   andi.w    #0x4 +0x1,-96(a6)      | [397]
	move.w   (-96,%a6),%d4
	andi.l   #(0xFFFF&(0x4+0x1)),%d4
	move.w   %d4,(-96,%a6)
|***	RESTORE	AFTER d4	***
	move.l   (%a7)+,%d4

      jbsr     norm_blt                | [399]

      move.w   (-122,%a6),(-96,%a6)    | [401]
|;;   move.w    _WRT_MODE,-98(a6)      | [402]
	move.w   _WRT_MODE,%d2
	move.w   %d2,(-98,%a6)
no_mode:
      move.w   (-18,%a6),%d7           | [404]
|;;   neg.w     d7                     | [405]
	move.w   %d7,%d2
	neg.l    %d2
|;;   move.w    d7,-16(a6)             | [406]
	move.w   %d2,(-16,%a6)
	move.w   %d2,%d7



      btst     #4,(-96+1,%a6)          | [410 EQ]
      jbeq     skip_out                | [411]
      tst.w    _DOUBLE                 | [412 NE]
      jbne     skip_out                | [413]

      movea.l  (-10,%a6),%a0           | [415]
      ext.l    %d7                     | [416]
      add.l    %d7,(-10,%a6)           | [417]
      jbsr     outlin                  | [418]
skip_out:



      clr.w    _SOURCEX                | [423]
      clr.w    _SOURCEY                | [424]


|;;   andi.w    #0xFFFF-(0x4 +0x1),-96(a6) | [427]
	move.w   (-96,%a6),%d2
	andi.l   #(0xFFFF&(0xFFFF-(0x4+0x1))),%d2
	move.w   %d2,(-96,%a6)



chk_rota:
      tst.w    (-110,%a6)              | [432 EQ]
      jbeq     chk_db                  | [433]

      jbsr     rotation                | [435]

chk_db:
      tst.w    _DOUBLE                 | [438 EQ]
      jbeq     do_clip                 | [439]

      jbsr     replicat                | [441]







do_clip:
      btst     #0,(-96+1,%a6)          | [450 EQ]
      jbeq     no_thik                 | [451]
      move.w   _WEIGHT,%d0             | [452]
      tst.w    _MONO_STATUS            | [453 NE]
      jbne     do_cl_nt                | [454]
|;;   add.w     d0,-102(a6)            | [455]
	move.w   (-102,%a6),%d2
	add.l    %d0,%d2
	move.w   %d2,(-102,%a6)
do_cl_nt:
      move.w   %d0,(-74,%a6)           | [457]
no_thik:
      tst.w    (-112,%a6)              | [459 NE]
      jbne     clip_adjust             | [460]
      clr.w    (-114,%a6)              | [461]
      clr.w    (-116,%a6)              | [462]
|;;   move.w    _DEV_TAB,-118(a6)      | [463]
	move.w   _DEV_TAB,%d2
	move.w   %d2,(-118,%a6)
|;;   move.w    _DEV_TAB+2,-120(a6)    | [464]
	move.w   _DEV_TAB+2,%d2
	move.w   %d2,(-120,%a6)

clip_adjust:
      move.w   (-108,%a6),%d0          | [467]
      cmp.w    (-116,%a6),%d0          | [468 GE]
      jbge     ymn_fine                | [469]
|;;   add.w     -106(a6),d0            | [470]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-116,%a6),%d0          | [471 LE]
      jble     upda_dst                | [472]
mn_clipy:
|;;   sub.w     -116(a6),d0            | [474]
	move.w   (-116,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-106,%a6),%d1          | [475]
|;;   move.w    d0,-106(a6)            | [476]
	move.w   %d7,(-106,%a6)
|;;   sub.w     d0,d1                  | [477]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEY            | [478]
	move.w   _SOURCEY,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEY
      move.w   (-116,%a6),%d0          | [479]
      move.w   %d0,(-108,%a6)          | [480]
ymn_fine:
      cmp.w    (-120,%a6),%d0          | [482 GT]
      jbgt     upda_dst                | [483]
mx_clipy:
|;;   add.w     -106(a6),d0            | [485]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
|;;   subq.w    #1,d0                  | [486]
	subq.l   #1,%d7
	move.w   %d7,%d0
      cmp.w    (-120,%a6),%d0          | [487 LE]
      jble     ymx_fine                | [488]

|;;   sub.w     -120(a6),d0            | [490]
	move.w   (-120,%a6),%d2
	sub.l    %d2,%d7
|;;   sub.w     d0,-106(a6)            | [491]
	move.w   (-106,%a6),%d2
	sub.l    %d7,%d2
	move.w   %d2,(-106,%a6)
ymx_fine:
      move.w   (-104,%a6),%d0          | [493]
      cmp.w    (-114,%a6),%d0          | [494 GE]
      jbge     xmn_fine                | [495]
|;;   add.w     -102(a6),d0            | [496]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-114,%a6),%d0          | [497 LE]
      jble     upda_dst                | [498]

mn_clipx:
|;;   sub.w     -114(a6),d0            | [501]
	move.w   (-114,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-102,%a6),%d1          | [502]
|;;   move.w    d0,-102(a6)            | [503]
	move.w   %d7,(-102,%a6)
|;;   sub.w     d0,d1                  | [504]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEX            | [505]
	move.w   _SOURCEX,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEX
      move.w   (-114,%a6),%d0          | [506]
      move.w   %d0,(-104,%a6)          | [507]
xmn_fine:
      cmp.w    (-118,%a6),%d0          | [509 GT]
      jbgt     upda_dst                | [510]
mx_clipx:
|***	SAVE	d3	***
	lea      -4(%a7),%a7
	movem.l  %d3,(%a7)
|;;   add.w     -102(a6),d0            | [512]
	move.w   %d0,%d3
	move.w   (-102,%a6),%d2
	add.l    %d2,%d3
|;;   subq.w    #1,d0                  | [513]
	subq.l   #1,%d3
	move.w   %d3,%d0
      cmp.w    (-118,%a6),%d0          | [514 LE]
      jble     scrn_blt                | [515]

|;;   sub.w     -118(a6),d0            | [517]
	move.w   (-118,%a6),%d2
	sub.l    %d2,%d3
|;;   sub.w     d0,-102(a6)            | [518]
	move.w   (-102,%a6),%d2
	sub.l    %d3,%d2
	move.w   %d2,(-102,%a6)

      jbra     scrn_blt                | [520]
| 925 "vdi/vdi_tblit.S"
scrn_blt:
|;;   move.w    _TEXT_FG,-84(a6)       | [523]
	move.w   _TEXT_FG,%d2
	move.w   %d2,(-84,%a6)
      move.w   (-6,%a6),(-76,%a6)      | [524]
      move.w   _v_planes,%d0           | [525]
      move.w   %d0,(-86,%a6)           | [526]
|;;   add.w     d0,d0                  | [527]
	move.w   %d0,%d3
	add.l    %d0,%d3
|;;   move.w    d0,-88(a6)             | [528]
	move.w   %d3,(-88,%a6)

      move.w   _SOURCEX,%d0            | [530]
      move.w   %d0,%d2                 | [531]
|;;   andi.w    #0x0F,d2               | [532]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [533]
|;;   lsr.w     #3,d0                  | [534]
	mvz.w    %d0,%d3
	lsr.l    #3,%d3
|;;   andi.w    #0xFFFE,d0             | [535]
	andi.l   #(0xFFFF&0xFFFE),%d3
      move.w   _SOURCEY,%d2            | [536]

      move.w   (-106,%a6),(-22,%a6)    | [538]
|;;   add.w     -22(a6),d2             | [539]
	move.w   (-22,%a6),%d7
	add.l    %d7,%d2
|;;   subq.w    #1,d2                  | [540]
	subq.l   #1,%d2
      mulu.w   (-16,%a6),%d2           | [541]
|;;   neg.w     -16(a6)                | [542]
	move.w   %d3,%d0
	move.w   (-16,%a6),%d3
	neg.l    %d3
	move.w   %d3,(-16,%a6)
|;;   add.w     d0,d2                  | [543]
	move.w   %d2,%d3
	add.l    %d0,%d3



      movea.l  (-10,%a6),%a0           | [547]
|;;   adda.w    d2,a0                  | [548]
	ext.l    %d3
	adda.l   %d3,%a0
      move.l   %a0,(-10,%a6)           | [549]

      move.w   (-102,%a6),(-20,%a6)    | [551]

      move.w   (-108,%a6),%d1          | [553]
|;;   add.w     -106(a6),d1            | [554]
	move.w   %d1,%d3
	move.w   (-106,%a6),%d7
	add.l    %d7,%d3
	move.w   %d3,%d1
|***	RESTORE	d3	***
	move.l   (%a7)+,%d3
|;;   subq.w    #1,d1                  | [555 MI]
	move.w   %d1,%d7
	subq.l   #1,%d7
	move.w   %d7,%d1
      bmi.b    upda_dst                | [556]

      move.w   (-104,%a6),%d0          | [558 MI]
      bmi.b    upda_dst                | [559]

      jsr      concat                  | [561]



      movea.l  _v_bas_ad,%a1           | [565]
      adda.l   %d1,%a1                 | [566]

      move.l   %a1,(-14,%a6)           | [568]
      move.w   %d0,(-26,%a6)           | [569]

      move.w   _v_lin_wr,%d0           | [571]
|;;   neg.w     d0                     | [572]
	move.w   %d0,%d3
	neg.l    %d3
|;;   move.w    d0,-18(a6)             | [573]
	move.w   %d3,(-18,%a6)

      jbsr     norm_blt                | [575]

upda_dst:
      move.w   _DELX,%d1               | [578]

      tst.w    _DOUBLE                 | [580 EQ]
      jbeq     upda_ndb                | [581]



      move.w   (-90,%a6),%d1           | [585]
      move.w   (-92,%a6),%d3           | [586]

      tst.w    (-94,%a6)               | [588 EQ]
      jbeq     upda_ndb                | [589]
|;;   exg       d1, d3                 | [590]
	move.l   %d1,%d2
	move.l   %d3,%d1
	move.l   %d2,%d3





upda_ndb:
|;;   btst.b    #0,_STYLE+1            | [597 EQ]
	moveq.l  #0,%d2
	btst     %d2,_STYLE+1
      jbeq     upda_ntk                | [598]
      tst.w    _MONO_STATUS            | [599 NE]
      jbne     upda_ntk                | [600]
|;;   add.w     _WEIGHT,d1             | [601]
	move.w   _WEIGHT,%d2
	add.l    %d2,%d1
upda_ntk:
      tst.w    (-110,%a6)              | [603 GT]
      jbgt     ck90                    | [604]
|;;   add.w     d1,_DESTX              | [605]
	move.w   _DESTX,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [606]
ck90:
      tst.w    (-126,%a6)              | [608 EQ GT]
      jbeq     ck180                   | [609]
      jbgt     ck270                   | [610]
|;;   sub.w     d1,_DESTY              | [611]
	move.w   _DESTY,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTY
      jbra     blt_done                | [612]
ck180:
|;;   sub.w     d1,_DESTX              | [614]
	move.w   _DESTX,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [615]
ck270:
|;;   add.w     d1,_DESTY              | [617]
	move.w   _DESTY,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTY
blt_done:
|;;   movem.l   (sp)+,d2-d7/a2-a5      | [619]
	movem.l  (%a7),%d2-%d7/%a2-%a5
	lea      40(%a7),%a7
      unlk     %a6                     | [620]
      rts                              | [621]

|;;           .text                    | [623]
| 1035 "vdi/vdi_tblit.S"
|;;                   .even            | [625]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [626]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [627]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [628]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [629]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [630]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [631]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [632]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [633]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [634]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [635]
	.short	0x007F
|;;                   .dc.w 0x003F     | [636]
	.short	0x003F
|;;                   .dc.w 0x001F     | [637]
	.short	0x001F
|;;                   .dc.w 0x000F     | [638]
	.short	0x000F
|;;                   .dc.w 0x0007     | [639]
	.short	0x0007
|;;                   .dc.w 0x0003     | [640]
	.short	0x0003
|;;                   .dc.w 0x0001     | [641]
	.short	0x0001
|;;                   .dc.w 0x0000     | [642]
	.short	0x0000

|;;           .text                    | [644]
| 1067 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [647]
|;;   sub.w     -24(a6),d1             | [648]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [649 PL]
      bpl.b    do_rot                  | [650]



|;;   neg.w     d1                     | [654]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [655]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [656]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [658]

|;;   lsl.w     #1,d0                  | [660]
	move.w   %d0,%d5
	lsl.l    #1,%d5
      lea      lf_tab,%a2              | [661]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [662]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [663]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [664]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [669]
|;;   lsl.w     #1,d0                  | [670]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [671]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [673]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [674]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [677]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [678]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [679]
	andi.l   #(0xFFFF&0xF),%d5
|;;   lsl.w     #1,d2                  | [680]
	lsl.l    #1,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [681]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [682]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [683]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [684]
      move.w   #0x8000,(-80,%a6)       | [685]
      moveq.l  #(-1),%d3               | [686]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [689 HI]
      bhi.b    doub_des                | [690]



|;;   lsl.w     #1,d0                  | [694]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [695]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [696]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [697]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [699]
	move.w   %d5,%d1
      jbra     msk_done                | [700]



doub_des:
      move.w   %d0,%d1                 | [705]
|;;   lsr.w     #4,d1                  | [706]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [707]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [708 NE]
      jbne     mlt_dest                | [709]



      move.w   (-24,%a6),%d1           | [713]
|;;   add.w     -20(a6),d1             | [714]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [715 CC]
      jbcc     mlt_dest                | [716]
      moveq.l  #0,%d7                  | [717]
      jbra     do_ritem                | [718]

mlt_dest:
      tst.w    (-42,%a6)               | [721 LT]
      jblt     mltleft                 | [722]
      moveq.l  #4,%d7                  | [723]
      jbra     do_ritem                | [724]
mltleft:
      moveq.l  #8,%d7                  | [726]
do_ritem:
|;;   andi.w    #0x000F,d0             | [728 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jbne     not_null                | [729]
|;;   subq.w    #1,d3                  | [730]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [731]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [733]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [734]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [735]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [738]
      move.w   %d3,(-28,%a6)           | [739]
      move.w   %d4,(-36,%a6)           | [740]
|;;   addq.w    #2,d3                  | [741]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [742 CS]
      jbcs     msk0                    | [743]
|;;   addq.w    #1,d3                  | [744]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [746]
      move.w   %d3,(-30,%a6)           | [747]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jbra     plane_loop              | [748]
| 1186 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [751]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [752]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [753]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [754]
	.byte	4*5,4*5,4*6,4*6

toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [757]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [759]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [763]
|;;   lsr.w     -84(a6)                | [764 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [765]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [766 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [767]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [771]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [772]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [773]
      movea.l  %a2,%a5                 | [774]

|;;   movea.l   twoptble(pc,d0.w),a4   | [776]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [780 EQ]
      jbeq     wrt_char                | [781]

      btst     #1,%d0                  | [783 EQ]
      jbeq     no_lite                 | [784]



|;;   move.w    _LITEMASK,-78(a6)      | [788]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [789]
      lea      liteop,%a5              | [790]

      move.l   %a2,(-64,%a6)           | [792]
      lea      liteopwf,%a2            | [793]

      move.l   %a4,(-72,%a6)           | [795]
      lea      liteopw,%a4             | [796]

no_lite:
      btst     #0,%d0                  | [799 EQ]
      jbeq     no_thick                | [800]



      clr.w    (-82,%a6)               | [804]

      move.l   %a5,(-48,%a6)           | [806]
      lea      thknop,%a5              | [807]

      move.l   %a2,(-60,%a6)           | [809]
      lea      thknopwf,%a2            | [810]

      move.l   %a4,(-68,%a6)           | [812]
      lea      thknopw,%a4             | [813]

no_thick:
      btst     #2,%d0                  | [816 EQ]
      jbeq     wrt_char                | [817]



      move.w   (-100,%a6),(-80,%a6)    | [821]
      move.l   %a5,(-56,%a6)           | [822]
      lea      skewop,%a5              | [823]

      tst.w    (-124,%a6)              | [825 EQ GT]
      jbgt     wrt_char                | [826]
      jbeq     not_sngl                | [827]
      clr.w    (-28,%a6)               | [828]
      moveq.l  #0,%d7                  | [829]
      jbra     chng_blt                | [830]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [832 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      bls.b    wrt_char                | [833]

      tst.w    (-42,%a6)               | [835 LT]
      jblt     chng_left               | [836]
      moveq.l  #4,%d7                  | [837]
      jbra     chng_blt                | [838]
chng_left:
      moveq.l  #8,%d7                  | [840]
chng_blt:
      move.w   %d7,(-124,%a6)          | [842]

wrt_char:
      move.w   (-124,%a6),%d7          | [845 LE]
      jble     set_chr_hgt             | [846]
      movea.l  %a2,%a5                 | [847]
      move.w   (-18,%a6),%d5           | [848]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [850]
|;;   subq.w    #1,d3                  | [851]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [853]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [854]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [856 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jble     no_more_planes          | [857]
      movea.l  (-10,%a6),%a0           | [858]
      movea.l  (-14,%a6),%a1           | [859]
      addq.l   #2,%a1                  | [860]
      move.l   %a1,(-14,%a6)           | [861]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [862 EQ]
      jbeq     plane_loop              | [863]
      jbra     norm_blt                | [864]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [866]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [868]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [874]
      move.w   (-16,%a6),%d6           | [875]
      move.w   (-18,%a6),%d7           | [876]
      lea      sgl_rtn,%a3             | [877]
      jbra     sgl_1st_scan            | [878]

sgl_lp:
|;;   adda.w    d6,a0                  | [881]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [882]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [884]
| 1329 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [886]


      move.w   (-42,%a6),%d1           | [889 MI]
      bmi.b    left_rol                | [890]
      lsr.l    %d1,%d0                 | [891]
      jbra     end_ro                  | [892]
left_rol:
      lsl.l    %d1,%d0                 | [894]
end_ro:
      swap     %d0                     | [896]
      move.w   %d0,%d1                 | [897]
      swap     %d0                     | [898]

      jmp      (%a5)                   | [900]

sgl_rtn:
      move.w   %d1,(%a1)               | [903]
|;;   dbra      d3,sgl_lp              | [904]
	tst.w    %d3
	jbeq     _PA_903_
	subq.l   #1,%d3
	jbra     sgl_lp
	_PA_903_:         
      jbra     next_plane              | [905]




dbl_loop:
      movea.l  %a3,%a4                 | [911]
      move.w   (-16,%a6),%d6           | [912]
      move.w   (-18,%a6),%d7           | [913]
      lea      dbl_2wrd,%a2            | [914]
      move.l   %a2,%d5                 | [915]
      lea      dbl_1wrd,%a3            | [916]
      jbra     dbl_1st_scan            | [917]

dbl_lp:
|;;   adda.w    d6,a0                  | [920]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [921]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [922]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [924]
| 1376 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [926]


      move.w   (-42,%a6),%d1           | [929 MI]
      bmi.b    dbleft                  | [930]
      lsr.l    %d1,%d0                 | [931]
      jbra     dblendr                 | [932]
dbleft:
      lsl.l    %d1,%d0                 | [934]
dblendr:
      swap     %d0                     | [936]
      move.w   %d0,%d1                 | [937]
      swap     %d0                     | [938]
      move.w   (-38,%a6),%d2           | [939]

      jmp      (%a5)                   | [941]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [944]
      move.w   %d0,%d1                 | [945]

      movea.w  (-88,%a6),%a2           | [947]
|;;   move.w    (a1,a2.w),d4           | [948]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [950]

|;;   exg       a3,d5                  | [952]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [953]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [956]
|;;   move.w    d1,(a1,a2.w)           | [957]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [959]
	tst.w    %d3
	jbeq     _PA_958_
	subq.l   #1,%d3
	jbra     dbl_lp
	_PA_958_:         
      jbra     next_plane              | [960]





left_loop:
      movea.l  %d6,%a0                 | [967]
      movea.l  %d7,%a1                 | [968]
|;;   adda.w    -16(a6),a0             | [969]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [970]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [971]
|;;   rol.w     -78(a6)                | [972]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [974 NE]
      jbne     skewopw                 | [975]
mlt_left:
      move.l   %a0,%d6                 | [977]
      move.l   %a1,%d7                 | [978]
      move.w   (-28,%a6),(-44,%a6)     | [979]







      move.l   (%a0)+,%d0              | [987]


      move.w   (%a1),%d4               | [990]

      move.w   (-42,%a6),%d1           | [992]
      lsl.l    %d1,%d0                 | [993]

      swap     %d0                     | [995]
      move.w   %d0,%d1                 | [996]
      swap     %d0                     | [997]

      move.w   (-38,%a6),%d2           | [999]

      lea      lft_lfrng,%a3           | [1001]
      jmp      (%a5)                   | [1002]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1005]
|;;   adda.w    -88(a6),a1             | [1006]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1008]
      lea      lft_wrds,%a3            | [1009]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1014]
|;;   and.w     d2,d4                  | [1015]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [1016]





      swap     %d0                     | [1022]
      move.w   %d1,%d0                 | [1023]
      swap     %d0                     | [1024]

      move.w   (-42,%a6),%d1           | [1026]
      lsl.l    %d1,%d0                 | [1027]

      swap     %d0                     | [1029]
      move.w   %d0,%d1                 | [1030]
      swap     %d0                     | [1031]

|;;   not.w     d2                     | [1033]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1034]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1035]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1036]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1038]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1040 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     lef_don                 | [1041]

      jmp      (%a4)                   | [1043]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1046]
|;;   adda.w    -88(a6),a1             | [1047]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_lef                | [1049]

lef_don:
      move.w   (-36,%a6),%d2           | [1052]

      lea      lft_rfrng,%a3           | [1054]
      jmp      (%a5)                   | [1055]

lft_rfrng:
      move.w   %d1,(%a1)               | [1058]

|;;   dbra      d3,left_loop           | [1060]
	tst.w    %d3
	jbeq     _PA_1059_
	subq.l   #1,%d3
	jbra     left_loop
	_PA_1059_:         
      jbra     next_plane              | [1061]





rite_loop:
      movea.l  %d6,%a0                 | [1068]
      movea.l  %d7,%a1                 | [1069]
|;;   adda.w    -16(a6),a0             | [1070]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [1071]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [1072]
|;;   rol.w     -78(a6)                | [1073]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [1075 NE]
      jbne     skewopw                 | [1076]
mlt_rite:
      move.l   %a0,%d6                 | [1078]
      move.l   %a1,%d7                 | [1079]
      move.w   (-28,%a6),(-44,%a6)     | [1080]
      move.w   (%a0)+,%d0              | [1081]



      swap     %d0                     | [1085]

      move.w   (%a1),%d4               | [1087]

      move.w   (-42,%a6),%d1           | [1089]
      lsr.l    %d1,%d0                 | [1090]
      swap     %d0                     | [1091]
      move.w   %d0,%d1                 | [1092]
      swap     %d0                     | [1093]

      move.w   (-38,%a6),%d2           | [1095]

      lea      rgt_lfrng,%a3           | [1097]
      jmp      (%a5)                   | [1098]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1101]
|;;   adda.w    -88(a6),a1             | [1102]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1104]
      lea      rgt_wrds,%a3            | [1105]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1110]
|;;   and.w     d2,d4                  | [1111]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [1112]
      move.w   (%a0)+,%d0              | [1113]



      swap     %d0                     | [1117]

      move.w   (-42,%a6),%d1           | [1119]
      lsr.l    %d1,%d0                 | [1120]
      swap     %d0                     | [1121]
      move.w   %d0,%d1                 | [1122]
      swap     %d0                     | [1123]

|;;   not.w     d2                     | [1125]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1126]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1127]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1128]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1130]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1132 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     rite_don                | [1133]

      jmp      (%a4)                   | [1135]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1138]
|;;   adda.w    -88(a6),a1             | [1139]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_rit                | [1141]

rite_don:
      move.w   (-36,%a6),%d2           | [1144]

      lea      rgt_rfrng,%a3           | [1146]
      jmp      (%a5)                   | [1147]

rgt_rfrng:
      move.w   %d1,(%a1)               | [1150]

|;;   dbra      d3,rite_loop           | [1152]
	tst.w    %d3
	jbeq     _PA_1151_
	subq.l   #1,%d3
	jbra     rite_loop
	_PA_1151_:         
      jbra     next_plane              | [1153]





top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1160]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [1161]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [1162]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [1163]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1164]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1167]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [1168]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [1169]
	not.l    %d0
|;;   and.w     d4,d1                  | [1170]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1171]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1174]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [1175]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1176]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1177]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1180]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [1181]
	not.l    %d0
|;;   and.w     d4,d1                  | [1182]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1183]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1186]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1187]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1188]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1191]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1192]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1193]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1196]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [1197]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1198]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1199]





twop0:
      moveq.l  #0,%d1                  | [1206]
      jmp      (%a3)                   | [1207]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [1210]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1211]

twop3:
      jmp      (%a3)                   | [1214]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1217]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [1218]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1219]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1222]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1223]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [1226]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1227]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1230]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [1231]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1232]
| 1702 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [1235]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [1236]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [1237]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [1238 EQ]
      jbeq     thk00                   | [1239]
|;;   ori.b     #0x10,ccr              | [1240 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [1241 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jbcc     thk01                   | [1242]
      move.w   #0x8000,%d6             | [1243]
thk01:
      move.w   %d6,(-34,%a6)           | [1245]
thk00:
      tst.w    (-28,%a6)               | [1247 MI]
      bmi.b    thk0                    | [1248]
      cmp.w    (-36,%a6),%d6           | [1249 CC]
      jbcc     thk0                    | [1250]
|;;   and.w     d6,d0                  | [1251]
	and.l    %d6,%d0
      jbra     thk1                    | [1252]
thk0:
      clr.w    %d0                     | [1254]
|;;   and.w     d6,d1                  | [1255]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [1257]
      move.w   %d2,%d5                 | [1258]
      swap     %d5                     | [1259]
      move.w   (-36,%a6),%d5           | [1260]
      lsl.l    %d6,%d5                 | [1261]

      swap     %d5                     | [1263]
      move.w   %d5,%d7                 | [1264]
      swap     %d5                     | [1265]

|;;   and.w     d7,d1                  | [1267]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [1268]
	and.l    %d5,%d0
|;;   swap      d1                     | [1269]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [1270]
|;;   subq.w    #1,d6                  | [1271]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [1273]
      lsr.l    #1,%d0                  | [1274]
      or.l     %d0,%d1                 | [1275]
|;;   dbra      d6,thkoplp             | [1276]
	tst.w    %d6
	jbeq     _PA_1275_
	subq.l   #1,%d6
	jbra     thkoplp
	_PA_1275_:         

      move.w   %d1,%d0                 | [1278]
      swap     %d1                     | [1279]
|;;   movem.l   (sp)+,d5-d7            | [1280]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [1281]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [1282]
| 1765 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [1285]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [1286 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk2                    | [1287]
|;;   and.w     -34(a6),d1             | [1288]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [1290]
      swap     %d1                     | [1291]
      clr.w    %d1                     | [1292]
      move.l   %d1,%d5                 | [1293]
|;;   subq.w    #1,d6                  | [1294]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [1296]
      or.l     %d5,%d1                 | [1297]
|;;   dbra      d6,thkopwlp            | [1298]
	tst.w    %d6
	jbeq     _PA_1297_
	subq.l   #1,%d6
	jbra     thkopwlp
	_PA_1297_:         

      move.w   %d1,%d5                 | [1300]
      swap     %d1                     | [1301]
|;;   or.w      -82(a6),d1             | [1302]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1303]
|;;   movem.l   (sp)+,d5-d6            | [1304]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [1305]
      jmp      (%a2)                   | [1306]
| 1803 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [1309]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [1310]
|;;   subq.w    #1,-30(a6)             | [1311 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      bmi.b    thk3                    | [1312]
      jbeq     thk4                    | [1313]

|;;   subq.w    #1,-30(a6)             | [1315 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk11                   | [1316]
|;;   and.w     d6,d1                  | [1317]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [1319]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jbra     thk7                    | [1320]

thk3:
      clr.w    %d1                     | [1323]
      jbra     thk5                    | [1324]

thk4:
|;;   and.w     d6,d1                  | [1327]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [1330 MI]
      bmi.b    thk6                    | [1331]
      move.w   (-28,%a6),%d5           | [1332]
|;;   addq.w    #2,d5                  | [1333]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [1334]
|;;   ori.b     #0x10,ccr              | [1335 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [1336 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jbcc     thk33                   | [1337]
      move.w   #0x8000,%d7             | [1338]
|;;   addq.w    #1,d5                  | [1339]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [1341 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jbne     thk34                   | [1342]
|;;   subq.w    #1,d5                  | [1343]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [1345 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [1346 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jbcc     thk31                   | [1347]
      move.w   #0x8000,%d6             | [1348]
thk31:
      cmp.w    %d6,%d7                 | [1350 CS]
      jbcs     thk35                   | [1351]
|;;   addq.w    #1,d5                  | [1352]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [1354]
      move.w   %d5,(-32,%a6)           | [1355]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [1357]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [1359]
      swap     %d1                     | [1360]
      clr.w    %d1                     | [1361]
      move.l   %d1,%d5                 | [1362]
|;;   subq.w    #1,d6                  | [1363]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [1365]
      or.l     %d5,%d1                 | [1366]
|;;   dbra      d6,thkopwl             | [1367]
	tst.w    %d6
	jbeq     _PA_1366_
	subq.l   #1,%d6
	jbra     thkopwl
	_PA_1366_:         

      move.w   %d1,%d5                 | [1369]
      swap     %d1                     | [1370]
|;;   or.w      -82(a6),d1             | [1371]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1372]
|;;   and.w     d2,d1                  | [1373]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [1374]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [1375]
      jmp      (%a2)                   | [1376]
| 1884 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [1379]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [1380]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [1381]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [1382]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1383]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1386]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [1387]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1388]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1391]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [1392]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1393]
| 1916 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [1396 CC]
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
      jbcc     no_shift                | [1397]

      swap     %d1                     | [1399]
      move.w   %d0,%d1                 | [1400]
      lsr.l    #1,%d1                  | [1401]

      swap     %d2                     | [1403]
      move.w   (-36,%a6),%d2           | [1404]
      lsr.l    #1,%d2                  | [1405]
      move.w   %d2,(-36,%a6)           | [1406]
      swap     %d2                     | [1407]
      move.w   %d2,(-38,%a6)           | [1408 EQ]
      jbeq     nxt_word                | [1409]
      move.w   (-42,%a6),%d0           | [1410 MI]
      bmi.b    dec_rol                 | [1411]
ror_add:
|;;   addq.w    #1,d0                  | [1413]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [1415]
      move.w   %d1,%d0                 | [1416]
      swap     %d1                     | [1417]
no_shift:
      movea.l  (-56,%a6),%a2           | [1419]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1420]

dec_rol:
      tst.b    %d0                     | [1423 EQ]
      jbeq     begn_ror                | [1424]
|;;   subq.w    #1,d0                  | [1425]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jbra     new_shif                | [1426]
begn_ror:
      clr.w    %d0                     | [1428]
      jbra     ror_add                 | [1429]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1434]
      swap     %d2                     | [1435]
      move.w   %d2,(-38,%a6)           | [1436]
|;;   adda.w    -88(a6),a1             | [1437]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1438]

      moveq.l  #15,%d0                 | [1440]
|;;   sub.w     -42(a6),d0             | [1441]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1442]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1443]
      jbra     no_shift                | [1444]






skewopw:
|;;   rol.w     -80(a6)                | [1452 CC]
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
      jbcc     do_shift                | [1453]

      tst.w    (-42,%a6)               | [1455 MI]
      bmi      mlt_left                | [1456]
      jbra     mlt_rite                | [1457]
do_shift:
|;;   ori       #0x10,ccr              | [1459 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1460]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1462]
      cmp.w    #0xFFFF,%d0             | [1463 EQ]
      jbeq     inc_rite                | [1464]

|;;   ori       #0x10,ccr              | [1466 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1467]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1468]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1471]
      tst.b    %d0                     | [1472 NE]
      jbne     no_rota                 | [1473]
      move.w   #0x8000,(-40,%a6)       | [1474]
no_rota:
      move.w   (-38,%a6),%d1           | [1476]
|;;   lsr.w     #1,d1                  | [1477 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jbeq     wnxt_wrd                | [1478]

      move.w   %d1,(-38,%a6)           | [1480]

      tst.w    %d0                     | [1482 MI]
      bmi.b    wdec_rol                | [1483]
|;;   addq.w    #1,-42(a6)             | [1484]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jbra     mlt_rite                | [1485]
wdec_rol:
      tst.b    %d0                     | [1487 EQ]
      jbeq     set_msk                 | [1488]
|;;   subq.w    #1,-42(a6)             | [1489]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jbra     mlt_left                | [1490]
set_msk:
      move.w   #1,(-42,%a6)            | [1492]
      jbra     mlt_rite                | [1493]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1496]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1497]
      jbra     do_left                 | [1498]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1501]

|;;   subq.w    #1,-28(a6)             | [1503]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1504]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1506]
|;;   sub.w     d0,d2                  | [1507]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1508]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1509]

      jbra     mlt_left                | [1511]




rotation:
|***	SAVE	d4/d6-d7	***
	lea      -28(%a7),%a7
	movem.l  %d4,12(%a7)
	movem.l  %d6-%d7,20(%a7)
      move.w   _SOURCEX,%d1            | [1517]
      move.w   %d1,%d2                 | [1518]
|;;   andi.w    #0x0F,d2               | [1519]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1520]
|;;   lsr.w     #4,d1                  | [1521]
	mvz.w    %d1,%d4
	lsr.l    #4,%d4
      moveq.l  #1,%d5                  | [1522]
|***	SAVE	d5	***
	movem.l  %d5,16(%a7)
|;;   lsl.w     d5,d1                  | [1523]
	lsl.l    %d5,%d4

      movea.l  (-10,%a6),%a0           | [1525]
|;;   adda.w    d1,a0                  | [1526]
	ext.l    %d4
	adda.l   %d4,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1528]
      move.w   _SOURCEY,%d0            | [1529]
      move.w   (-106,%a6),%d1          | [1530]
      move.w   %d1,(-22,%a6)           | [1531]
      move.w   (-16,%a6),%d2           | [1532]

      tst.w    (-126,%a6)              | [1534 EQ LT]
      jbeq     upsd_dwn                | [1535]
rot90:
      jblt     top_src                 | [1537]
|;;   neg.w     -16(a6)                | [1538]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   subq.w    #1,d1                  | [1539]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   add.w     d1,d0                  | [1540]
	add.l    %d4,%d0
      mulu.w   %d2,%d0                 | [1541]
|;;   adda.w    d0,a0                  | [1542]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1544]
|;;   lsr.w     #4,d0                  | [1545]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1546]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1547]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1548]
      movea.l  _scrtchp,%a1            | [1549]
|;;   adda.w    -4(a6),a1              | [1550]
	mvs.w    (-4,%a6),%d4
	adda.l   %d4,%a1

      tst.w    (-126,%a6)              | [1552 GT]
      jbgt     top_dwn                 | [1553]
|;;   neg.w     -18(a6)                | [1554]
	move.w   (-18,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1555]
|;;   subq.w    #1,d1                  | [1556]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [1557]
	mulu.w   %d4,%d0
|;;   adda.w    d0,a1                  | [1558]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1560]
      move.w   #0x8000,%d3             | [1561]
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
      move.w   %d3,%d4                 | [1562]
      movea.w  %d3,%a4                 | [1563]
|;;   lsr.w     d2,d3                  | [1564]
	mvz.w    %d3,%d5
	lsr.l    %d2,%d5
      moveq.l  #0,%d0                  | [1565]
      move.w   (-20,%a6),%d2           | [1566]
|***	RESTORE	AFTER d6	***
	movem.l  20(%a7),%d6
      move.w   (-16,%a6),%d6           | [1567]
      movea.l  %a0,%a2                 | [1568]
      movea.l  %a1,%a3                 | [1569]
	move.w   %d5,%d3
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      jbra     rot_nsrc                | [1570]
rot_ylp:
|***	SAVE	d2	***
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1572]
|***	RESTORE	d3	***
	movem.l  4(%a7),%d3
      jbra     rot_srt                 | [1573]
rot_xlp:
      move.w   (%a0),%d7               | [1575]
|;;   and.w     d3,d7                  | [1576 EQ]
	move.w   %d7,%d2
	and.l    %d3,%d2
	tst.w    %d2
      jbeq     rot_nor                 | [1577]
|;;   or.w      d4,d0                  | [1578]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1580 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1579_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1579_1:         
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      jbcc     rot_isrc                | [1581]
rot_ndst:
      move.w   %d0,(%a1)+              | [1583]
      moveq.l  #0,%d0                  | [1584]
rot_isrc:
|;;   adda.w    d6,a0                  | [1586]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,8(%a7)
|;;   dbra      d1,rot_xlp             | [1588]
	tst.w    %d1
	jbeq     _PA_1587_
	subq.l   #1,%d1
	jbra     rot_xlp
	_PA_1587_:         

      move.w   %d0,(%a1)               | [1590]
      moveq.l  #0,%d0                  | [1591]
|;;   adda.w    -18(a6),a3             | [1592]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1593]
      move.w   %a4,%d4                 | [1594]
|;;   ror.w     d5,d3                  | [1595 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1594_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1594_1:         
      jbcc     rnew_src                | [1596]
      addq.l   #2,%a2                  | [1597]
rnew_src:
      movea.l  %a2,%a0                 | [1599]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2
	movem.l  8(%a7),%d6
	lea      12(%a7),%a7
rot_nsrc:
|***	SAVE	d3	***
	lea      -12(%a7),%a7
	movem.l  %d3,4(%a7)
|;;   dbra      d2,rot_ylp             | [1601]
	tst.w    %d2
	jbeq     _PA_1600_
	subq.l   #1,%d2
	jbra     rot_ylp
	_PA_1600_:         

rot_done:
      move.w   (-102,%a6),%d0          | [1604]
      move.w   (-106,%a6),%d1          | [1605]
      move.w   %d1,(-20,%a6)           | [1606]
      move.w   %d1,(-102,%a6)          | [1607]
      move.w   %d0,(-22,%a6)           | [1608]
      move.w   %d0,(-106,%a6)          | [1609]

      move.w   (-92,%a6),%d0           | [1611]
      move.w   (-90,%a6),(-92,%a6)     | [1612]
      move.w   %d0,(-90,%a6)           | [1613]
      move.w   #1,(-94,%a6)            | [1614]

      move.w   (-18,%a6),%d0           | [1616]
      tst.w    (-126,%a6)              | [1617 GT]
      jbgt     rot_nneg                | [1618]
|;;   neg.w     d0                     | [1619]
	move.w   %d0,%d3
	neg.l    %d3
	move.w   %d3,%d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1621]
      move.w   (-4,%a6),%d0            | [1622]
|***	RESTORE	AFTER d3	***
	movem.l  4(%a7),%d3
	lea      12(%a7),%a7
repexit:
      clr.w    _SOURCEX                | [1624]
repexit3:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      movea.l  _scrtchp,%a2            | [1626]
|;;   adda.w    d0,a2                  | [1627]
	mvs.w    %d0,%d1
	adda.l   %d1,%a2
      move.l   %a2,(-10,%a6)           | [1628]
      clr.w    _SOURCEY                | [1629]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1630]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1633]
|;;   add.w     -24(a6),d0             | [1634]
	move.w   (-24,%a6),%d4
	add.l    %d4,%d0
|;;   subq.w    #1,d0                  | [1635]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1636]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1637]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1638]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1640]
      move.w   %d0,%d2                 | [1641]
|;;   lsr.w     d5,d2                  | [1642]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1643]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1644]
|;;   add.w     -4(a6),d0              | [1645 X]
	move.w   %d0,%d6
	mvz.w    (-4,%a6),%d4
	ori.l    #0xffff0000,%d6
	add.l    %d4,%d6
      movea.l  _scrtchp,%a1            | [1646]
|;;   adda.w    d0,a1                  | [1647]
	ext.l    %d6
	adda.l   %d6,%a1
|***	RESTORE	d4/d6	***
	movem.l  12(%a7),%d4
	movem.l  20(%a7),%d6
      jbra     strtflip                | [1648]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1650]
      move.w   %d2,%d3                 | [1651]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1653]
      moveq.l  #0,%d6                  | [1654]
      moveq.l  #15,%d4                 | [1655]
flip_lp:
|***	SAVE	d4	***
	movem.l  %d4,12(%a7)
|;;   lsr.w     d5,d0                  | [1657 X]
	mvz.w    %d0,%d1
	lsr.l    %d5,%d1
|;;   roxl.w    d5,d6                  | [1658 X]
	move.w   %d1,%d0
	move.b   %d5,%d1
	move.w   %d6,%d2
	moveq    #16,%d4
	and.l    #0x1f,%d1
	jbeq     _PA_1657_
	move.w   %d2,%d3
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d3,%d2
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d2
	_PA_1657_:         
	addx.l   %d3,%d3
	swap     %d3
	move.w   %d2,%d3
	lsl.l    %d4,%d3
	move.w   %d2,%d6
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
|;;   dbra      d4,flip_lp             | [1659]
	move     %CCR,%d1
	tst.w    %d4
	jbeq     _PA_1658_
	subq.l   #1,%d4
	jbra     flip_lp
	_PA_1658_:         
	move     %d1,%CCR

|;;   move.w    d6,-(a1)               | [1661]
	move.w   %d2,-(%a1)
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1662]
	move     %CCR,%d1
	tst.w    %d3
	jbeq     _PA_1661_
	subq.l   #1,%d3
	move     %d1,%CCR
	jbra     line_lp
	_PA_1661_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1664]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1666]
	move     %CCR,%d7
	tst.w    %d1
	jbeq     _PA_1665_
	subq.l   #1,%d1
	move     %d7,%CCR
	jbra     upsd_lp
	_PA_1665_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1668]
      movea.l  _scrtchp,%a1            | [1669]
|;;   adda.w    -4(a6),a1              | [1670]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1671]
      move.w   _SOURCEX,%d0            | [1672]
|;;   add.w     -102(a6),d0            | [1673]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d5
	add.l    %d5,%d7
|;;   neg.w     d0                     | [1674]
	neg.l    %d7
|;;   andi.w    #0xF,d0                | [1675]
	andi.l   #(0xFFFF&0xF),%d7
|;;   move.w    d0,_SOURCEX            | [1676]
	move.w   %d7,_SOURCEX
      clr.w    _SOURCEY                | [1677]
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      rts                              | [1678]






replicat:
|***	SAVE	d6-d7	***
	lea      -16(%a7),%a7
	movem.l  %d6-%d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1686]
      move.w   %d0,%d4                 | [1687]
|;;   andi.w    #0x0F,d0               | [1688]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1689]
|;;   lsr.w     #4,d4                  | [1690]
	mvz.w    %d4,%d6
	lsr.l    #4,%d6
|;;   lsl.w     #1,d4                  | [1691]
	lsl.l    #1,%d6

      move.w   _SOURCEY,%d0            | [1693]
      mulu.w   (-16,%a6),%d0           | [1694]
|;;   add.w     d4,d0                  | [1695]
	move.w   %d0,%d7
	add.l    %d6,%d7
	move.w   %d7,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1696]
|;;   adda.w    d0,a0                  | [1697]
	mvs.w    %d0,%d3
	adda.l   %d3,%a0

      move.w   (-24,%a6),%d2           | [1699]
      move.w   #0x8000,%d3             | [1700]
      move.w   %d3,%d4                 | [1701]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1702]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1704]
      move.w   (-102,%a6),%d1          | [1705]

      move.w   %d2,(-22,%a6)           | [1707]
      move.w   %d1,(-20,%a6)           | [1708]
      movea.l  _scrtchp,%a1            | [1709]
|;;   adda.w    _scrpt2,a1             | [1710]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1712 EQ]
      jbeq     noline                  | [1713]



|;;   addq.w    #1,-106(a6)            | [1717]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1718]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1720]
	move.w   %d1,%d0
	lsl.l    #1,%d0
|;;   addq.w    #3,d1                  | [1721]
	addq.l   #3,%d0
|;;   lsr.w     #4,d1                  | [1722]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d1                  | [1723]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1724]
	addq.l   #2,%d0
|;;   move.w    d1,d5                  | [1725]
	move.w   %d0,%d5
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      moveq.l  #0,%d6                  | [1726]
	move.w   %d0,%d1
oklear:
      move.w   %d6,(%a1)+              | [1728]
|;;   dbra      d5,oklear              | [1729]
	tst.w    %d5
	jbeq     _PA_1728_
	subq.l   #1,%d5
	jbra     oklear
	_PA_1728_:         
      lea      (-2,%a1),%a2            | [1730]
      move.w   %d2,%d5                 | [1731]
|;;   lsl.w     #1,d5                  | [1732]
	move.w   %d5,%d0
	lsl.l    #1,%d0
	move.w   %d0,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1734]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1735]
|;;   dbra      d5,oklear1             | [1736]
	tst.w    %d5
	jbeq     _PA_1735_
	subq.l   #1,%d5
	jbra     oklear1
	_PA_1735_:         

|;;   move.w    d1,d5                  | [1738]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1739]
	mvz.w    %d5,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d5                  | [1740 X]
	subq.l   #1,%d0
	move.w   %d0,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1742]
|;;   dbra      d5,oklear3             | [1743]
	move     %CCR,%d0
	tst.w    %d5
	jbeq     _PA_1742_
	subq.l   #1,%d5
	move     %d0,%CCR
	jbra     oklear3
	_PA_1742_:         
	move     %d0,%CCR
      jbra     noline1                 | [1744]

noline:
|;;   lsr.w     #3,d1                  | [1747]
	mvz.w    %d1,%d0
	lsr.l    #3,%d0
|;;   lsl.w     #1,d1                  | [1748]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1749 X]
	ori.l    #0xffff0000,%d0
	addq.l   #2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
noline1:
      move.w   %d1,(-18,%a6)           | [1751]
|***	RESTORE	AFTER d7	***
	movem.l  12(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1755]
|;;   roxr.l    #1,d7                  | [1756 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1757]
|;;   roxr.l    #1,d7                  | [1758]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1760]
      move.w   _DDA_INC,%d2            | [1761]
      move.w   (-22,%a6),%d5           | [1762]
|;;   subq.w    #1,d5                  | [1763]
	move.w   %d5,%d0
	subq.l   #1,%d0
      move.w   #32767,%d6              | [1764]
	move.w   %d0,%d5
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      16(%a7),%a7
      tst.l    %d7                     | [1765 MI]
      bmi.b    rep_ylop                | [1766]
y_dwn_lp:
|***	SAVE	d0-d2	***
	lea      -12(%a7),%a7
	movem.l  %d0-%d2,(%a7)
|;;   add.w     d2,d6                  | [1768 CC]
	mvz.w    %d2,%d0
	move.w   %d6,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
	lea      -8(%a7),%a7
	movem.l  %d0,4(%a7)
	move     %ccr,%d0
	move.l   %d0,(%a7)
	move.l   4(%a7),%d0
	move.w   %d1,%d6
	move.l   %d0,4(%a7)
	move.l   (%a7),%d0
	move     %d0,%ccr
	movem.l  4(%a7),%d0
	lea      8(%a7),%a7
|***	RESTORE	AFTER d0-d1	***
	movem.l  (%a7),%d0-%d1
      jbcc     y_no_drw                | [1769]
      jbsr     yloop                   | [1770]
y_no_drw:
|;;   adda.w    d7,a0                  | [1772]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  8(%a7),%d2
	lea      12(%a7),%a7
|;;   dbra      d5,y_dwn_lp            | [1773]
	tst.w    %d5
	jbeq     _PA_1772_
	subq.l   #1,%d5
	jbra     y_dwn_lp
	_PA_1772_:         
	move.w   #-1,%d5
      jbra     y_rep_don               | [1774]
rep_ylop:
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
|;;   add.w     d2,d6                  | [1776 CC]
	mvz.w    %d2,%d0
	move.w   %d6,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d0,%d1
	lea      -8(%a7),%a7
	movem.l  %d0,4(%a7)
	move     %ccr,%d0
	move.l   %d0,(%a7)
	move.l   4(%a7),%d0
	move.w   %d1,%d6
	move.l   %d0,4(%a7)
	move.l   (%a7),%d0
	move     %d0,%ccr
	movem.l  4(%a7),%d0
	lea      8(%a7),%a7
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
      jbcc     y_no_rep                | [1777]
      jbsr     yloop                   | [1778]
y_no_rep:
      jbsr     yloop                   | [1780]
|;;   adda.w    d7,a0                  | [1781]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  4(%a7),%d2
	lea      8(%a7),%a7
|;;   dbra      d5,rep_ylop            | [1782]
	tst.w    %d5
	jbeq     _PA_1781_
	subq.l   #1,%d5
	jbra     rep_ylop
	_PA_1781_:         
	move.w   #-1,%d5
y_rep_don:
|***	SAVE	d4-d6	***
	lea      -12(%a7),%a7
	movem.l  %d4-%d6,(%a7)
      move.w   (-102,%a6),%d2          | [1784]
|;;   subq.w    #1,d2                  | [1785]
	subq.l   #1,%d2
      move.w   _XACC_DDA,%d1           | [1786]
      move.w   _DDA_INC,%d0            | [1787]
      moveq.l  #0,%d3                  | [1788]
repwidcl:
|;;   add.w     d0,d1                  | [1790 CC]
	mvz.w    %d0,%d4
	move.w   %d1,%d5
	ori.l    #0xffff0000,%d5
	add.l    %d4,%d5
	move     %ccr,%d6
	move.w   %d5,%d1
	move     %d6,%ccr
      jbcc     nrepdoub                | [1791]
|;;   addq.w    #1,d3                  | [1792]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrepdoub:
      tst.l    %d7                     | [1794 PL]
      bpl.b    nrpndoub                | [1795]
|;;   addq.w    #1,d3                  | [1796]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1798]
	tst.w    %d2
	jbeq     _PA_1797_
	subq.l   #1,%d2
	jbra     repwidcl
	_PA_1797_:         
      move.w   %d1,_XACC_DDA           | [1799]
      move.w   %d3,(-102,%a6)          | [1800]
      move.w   (-92,%a6),(-106,%a6)    | [1801]


      move.w   (-18,%a6),%d1           | [1804]
      move.w   %d1,(-16,%a6)           | [1805]







repexit1:
      btst     #4,(-96+1,%a6)          | [1814 EQ]
      jbeq     repexit2                | [1815]
      movea.l  _scrtchp,%a0            | [1816]
|;;   adda.w    _scrpt2,a0             | [1817]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1818]
|;;   lea       2(a0,d7.w),a1          | [1819]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
|***	RESTORE	AFTER d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbsr     outlin1                 | [1820]
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
      move.w   _scrpt2,%d0             | [1821]
|;;   add.w     -18(a6),d0             | [1822]
	move.w   %d0,%d2
	move.w   (-18,%a6),%d1
	add.l    %d1,%d2
|;;   move.w    #15,_SOURCEX           | [1823]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
	move.w   %d2,%d0
|***	RESTORE	d1-d2	***
	movem.l  (%a7),%d1-%d2
	lea      8(%a7),%a7
      jbra     repexit3                | [1824]
repexit2:
      move.w   _scrpt2,%d0             | [1826]
|***	RESTORE	d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbra     repexit                 | [1827]
| 2379 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1830]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1831]
      movea.l  %a1,%a4                 | [1832]
      movea.l  %d7,%a5                 | [1833]
      move.w   %d5,(-44,%a6)           | [1834]
      moveq.l  #0,%d0                  | [1835]
      move.w   (-20,%a6),%d5           | [1836]
|;;   subq.w    #1,d5                  | [1837]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1839]
      jbra     nextsrc                 | [1840]
innerlp:
|;;   ror.w     #1,d3                  | [1842 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jbcc     reploop                 | [1843]
nextsrc:
      movea.w  (%a0)+,%a2              | [1845]
reploop:
      move.w   %a2,%d6                 | [1847]
|;;   and.w     d3,d6                  | [1848 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jbne     nrepnor                 | [1849]


repnor:
      tst.l    %d7                     | [1853 MI]
      bmi.b    repnorup                | [1854]
|;;   add.w     d2,d7                  | [1855 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     incsrc                  | [1856]
      jbra     ordone                  | [1857]
repnorup:
|;;   add.w     d2,d7                  | [1859 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     ordone                  | [1860]


|;;   ror.w     #1,d4                  | [1863 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jbcc     ordone                  | [1866]
      move.w   %d0,(%a1)+              | [1867]
      moveq.l  #0,%d0                  | [1868]


      jbra     ordone                  | [1871]
nrepnor:


|;;   add.w     d2,d7                  | [1875 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     o_no_rep                | [1876]


|;;   or.w      d4,d0                  | [1879]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1880 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jbcc     o_no_rep                | [1882]
      move.w   %d0,(%a1)+              | [1883]
      moveq.l  #0,%d0                  | [1884]
o_no_rep:
      tst.l    %d7                     | [1886 PL]
      bpl.b    incsrc                  | [1887]


|;;   or.w      d4,d0                  | [1890]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1892 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jbcc     incsrc                  | [1893]
nextdst:
      move.w   %d0,(%a1)+              | [1895]
      moveq.l  #0,%d0                  | [1896]
incsrc:
|;;   dbra      d5,innerlp             | [1898]
	tst.w    %d5
	jbeq     _PA_1897_
	subq.l   #1,%d5
	jbra     innerlp
	_PA_1897_:         
repdone:
      move.w   %d0,(%a1)               | [1900]
|;;   movem.l   (sp)+,d3-d4/d6         | [1901]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1902]
      movea.l  %a4,%a1                 | [1903]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1904]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1905]
      move.w   (-44,%a6),%d5           | [1906]
      rts                              | [1907]
| 2467 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1910]
	move.w   %d7,%a5
	lea      (0,%a0,%a5.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1912]
	move.w   %d7,%a5
	lea      (0,%a1,%a5.l*1),%a2
|;;   lsr.w     #1,d7                  | [1913]
	mvz.w    %d7,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d7                  | [1914]
	subq.l   #1,%d0
|;;   move.w    d7,-44(a6)             | [1915]
	move.w   %d0,(-44,%a6)
      move.w   (-106,%a6),%d6          | [1916]

      movea.l  %a0,%a5                 | [1918]
|;;   subq.w    #1,d6                  | [1919]
	subq.l   #1,%d6
	move.w   %d0,%d7
out_edge:
      movea.w  %d6,%a4                 | [1921]
      movea.l  %a2,%a3                 | [1922]
      moveq.l  #0,%d5                  | [1923]
      moveq.l  #0,%d6                  | [1924]
      move.l   (%a2),%d1               | [1925]
      lsr.l    #1,%d1                  | [1926]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1931]
      move.b   %d5,%d0                 | [1932]
|;;   ror.l     #1,d0                  | [1933]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1935]
      move.b   %d6,%d2                 | [1936]
      move.l   %d2,%d3                 | [1937]
|;;   ror.l     #1,d3                  | [1938]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1939]
|;;   ror.l     #1,d4                  | [1940]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1942]
      move.l   %d0,%d6                 | [1943]
      eor.l    %d2,%d0                 | [1944]
      eor.l    %d3,%d5                 | [1945]
      eor.l    %d4,%d6                 | [1946]
|;;   rol.l     #1,d5                  | [1947]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1948]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1949]
      or.l     %d6,%d0                 | [1950]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1952]
      move.l   %d1,%d6                 | [1953]
      eor.l    %d2,%d1                 | [1954]
      eor.l    %d3,%d5                 | [1955]
      eor.l    %d4,%d6                 | [1956]
|;;   rol.l     #1,d5                  | [1957]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1958]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1959]
      or.l     %d5,%d0                 | [1960]
      or.l     %d6,%d0                 | [1961]

      eor.l    %d3,%d2                 | [1963]
      eor.l    %d3,%d4                 | [1964]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1965]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1966]
      or.l     %d4,%d0                 | [1967]
      swap     %d0                     | [1968]

      move.w   (%a1),%d6               | [1970]
      move.w   %d6,%d5                 | [1971]
|;;   eor.w     d0,d5                  | [1972]
	move.w   %d5,%d3
	eor.l    %d0,%d3
|;;   and.w     d0,d5                  | [1973]
	and.l    %d0,%d3

      addq.l   #2,%a2                  | [1975]
      move.l   (%a2),%d1               | [1976]
      move.b   (-1,%a2),%d1            | [1977]
|;;   ror.l     #1,d1                  | [1978]
	move.w   %d3,%d5
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1980]
      move.w   (%a0),%d5               | [1981]
      move.w   %d6,(%a0)+              | [1982]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1984]
	tst.w    %d7
	jbeq     _PA_1983_
	subq.l   #1,%d7
	jbra     out_loop
	_PA_1983_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1986]
      movea.l  %a5,%a0                 | [1987]
      move.w   %a4,%d6                 | [1988]
      movea.l  %a3,%a2                 | [1989]
      movea.l  %a2,%a1                 | [1990]
|;;   adda.w    -16(a6),a2             | [1991]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1992 NE]
      jbne     srt_lin                 | [1993]
      movea.l  %a1,%a2                 | [1994]
srt_lin:
|;;   dbra      d6,out_edge            | [1996]
	tst.w    %d6
	jbeq     _PA_1995_
	subq.l   #1,%d6
	jbra     out_edge
	_PA_1995_:         
	move.w   #-1,%d6
      rts                              | [1997]
| 2578 "vdi/vdi_tblit.S"
concat:
|;;   movem.l   d2/d3/a0,-(sp)         | [2000]
	lea      -12(%a7),%a7
	movem.l  %d2-%d3/%a0,(%a7)



      mulu.w   _v_lin_wr,%d1           | [2004]





      move.w   %d0,%d2                 | [2010]
|;;   andi.w    #0x000f,d0             | [2011]
	andi.l   #(0xFFFF0000!~0x000f),%d0
|;;   andi.w    #0xfff0,d2             | [2012]
	andi.l   #(0xFFFF&0xfff0),%d2
| 2600 "vdi/vdi_tblit.S"
      lea      _shift_offset,%a0       | [2014]
|;;   add.w     _v_planes,a0           | [2015]
	mvs.w    _v_planes,%d3
	adda.l   %d3,%a0
      move.b   (%a0),%d3               | [2016]
|;;   lsr.w     d3,d2                  | [2017]
	lsr.l    %d3,%d2






      ext.l    %d2                     | [2024]
      add.l    %d2,%d1                 | [2025]
|;;   movem.l   (sp)+,d2/d3/a0         | [2026]
	movem.l  (%a7),%d2-%d3/%a0
	lea      12(%a7),%a7
      rts                              | [2027]

| End section on aligned boundary
	.p2align 2,0
