
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
| Translated on Sun Mar 27 00:13:00 2016 by version 2.6.0 of PortAsm translator
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



_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [76]
	.short	(2+6+8+15)/16*2*16*2
| 457 "vdi/vdi_tblit.S"
|;;           .text                    | [78]




	.p2align 1,0
_text_blt:
      link.w   %a6,#(-126)             | [84]
|;;   movem.l   d2-d7/a2-a5,-(sp)      | [85]
	lea      -40(%a7),%a7
	movem.l  %d2-%d7/%a2-%a5,(%a7)

      clr.w    (-94,%a6)               | [87]
      move.w   #0,(-6,%a6)             | [88]
      lea      (-96,%a6),%a1           | [89]
      move.w   _STYLE,(%a1)            | [90]
      move.w   _WRT_MODE,-(%a1)        | [91]
      move.w   _SKEWMASK,-(%a1)        | [92]

      move.w   _DELX,-(%a1)            | [94]
      move.w   _DESTX,-(%a1)           | [95]
      move.w   _DELY,-(%a1)            | [96]
      move.w   _DESTY,-(%a1)           | [97]
      move.w   _CHUP,-(%a1)            | [98]

      movea.w  _scrpt2,%a0             | [100]
      move.w   (-106,%a6),%d3          | [101]
      tst.w    _DOUBLE                 | [102 NE]
      jbne     adr_dbl                 | [103]
      move.w   (-102,%a6),%d1          | [104]
      jbra     adr_nodb                | [105]



adr_dbl:
|;;   movem.l   a0/a1,-(sp)            | [110]
	lea      -8(%a7),%a7
	movem.l  %a0-%a1,(%a7)
      move.w   %d3,-(%a7)              | [111]
      move.l   (8,%a6),-(%a7)          | [112]
      jsr      _act_siz                | [113]
      addq.l   #6,%a7                  | [114]
|;;   movem.l   (sp)+,a0/a1            | [115]
	movem.l  (%a7),%a0-%a1
	lea      8(%a7),%a7
      move.w   %d0,%d3                 | [116]
      move.w   %d3,(-92,%a6)           | [117]
      move.w   (-102,%a6),%d2          | [118]
      move.w   _XACC_DDA,%d4           | [119]
      move.w   _DDA_INC,%d0            | [120]
      moveq.l  #0,%d1                  | [121]
      jbra     clc_n_su                | [122]
doub_wid:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   add.w     d0,d4                  | [124 CC]
	mvz.w    %d0,%d2
	move.w   %d4,%d3
	ori.l    #0xffff0000,%d3
	add.l    %d2,%d3
	move     %ccr,%d5
	move.w   %d3,%d4
	move     %d5,%ccr
      jbcc     no_db_wd                | [125]
|;;   addq.w    #1,d1                  | [126]
	move.w   %d1,%d2
	addq.l   #1,%d2
	move.w   %d2,%d1
no_db_wd:
|***	RESTORE	d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
|;;   btst.b    #0,_T_SCLSTS+1         | [128 EQ]
	moveq.l  #0,%d5
	btst     %d5,_T_SCLSTS+1
      jbeq     clc_n_su                | [129]
|;;   addq.w    #1,d1                  | [130]
	move.w   %d1,%d5
	addq.l   #1,%d5
	move.w   %d5,%d1
clc_n_su:
|;;   dbra      d2,doub_wid            | [132]
	tst.w    %d2
	jbeq     _PA_131_
	subq.l   #1,%d2
	jbra     doub_wid
	_PA_131_:         
      move.w   %d1,(-90,%a6)           | [133]
| 521 "vdi/vdi_tblit.S"
      tst.w    (-110,%a6)              | [135 NE]
      jbne     adr_nodb                | [136]



      movea.w  #0,%a0                  | [140]
adr_nodb:
      move.w   %a0,(-2,%a6)            | [142]
      move.w   (-108,%a6),%d2          | [143]
      move.w   (-104,%a6),%d0          | [144]
      move.w   _L_OFF,%d6              | [145]
      move.w   _R_OFF,%d7              | [146]
      clr.w    (-74,%a6)               | [147]
      btst     #0,(-96+1,%a6)          | [148 EQ]
      jbeq     adr_notk                | [149]
      move.w   _WEIGHT,%d4             | [150 NE]
      jbne     adr_th1                 | [151]
|;;   and.w     #0xFFFF-0x1,-96(a6)    | [152]
	move.w   (-96,%a6),%d5
	andi.l   #(0xFFFF&(0xFFFF-0x1)),%d5
	move.w   %d5,(-96,%a6)
adr_th1:
      tst.w    _MONO_STATUS            | [154 NE]
      jbne     adr_notk                | [155]

|;;   add.w     d4,d1                  | [157]
	move.w   %d1,%d5
	add.l    %d4,%d5
	move.w   %d5,%d1
adr_notk:
      btst     #2,(-96+1,%a6)          | [159 EQ]
      jbeq     chk_addr                | [160]

|;;   add.w     d6,d1                  | [162]
	move.w   %d1,%d5
	add.l    %d6,%d5
|;;   add.w     d7,d1                  | [163]
	add.l    %d7,%d5
	move.w   %d5,%d1







chk_addr:
      move.w   (-110,%a6),%d4          | [172 EQ]
      jbeq     chk_clip                | [173]
      move.w   #0,(-4,%a6)             | [174]
|;;   subi.w    #1800,d4               | [175]
	move.w   %d4,%d5
	subi.l   #1800,%d5
	move.w   %d5,%d4
      move.w   %d4,(-126,%a6)          | [176 EQ GT]
      jbeq     add180                  | [177]
      jbgt     add270                  | [178]



|;;   sub.w     d1,d2                  | [182]
	sub.l    %d1,%d2
      move.w   %d2,(-108,%a6)          | [183]
|;;   exg       d1,d3                  | [184]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3
      jbra     chk_clip                | [185]



add180:
|;;   sub.w     d1,d0                  | [190]
	move.w   %d0,%d5
	sub.l    %d1,%d5
|;;   move.w    d0,-104(a6)            | [191]
	move.w   %d5,(-104,%a6)
	move.w   %d5,%d0
      jbra     chk_clip                | [192]



add270:
|;;   exg       d1,d3                  | [197]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3
| 594 "vdi/vdi_tblit.S"
chk_clip:
      moveq.l  #0,%d5                  | [200]
      move.w   _CLIP,-(%a1)            | [201 EQ]
      jbeq     clp_done                | [202]
      move.w   _XMN_CLIP,-(%a1)        | [203]
      move.w   _YMN_CLIP,-(%a1)        | [204]
      move.w   _XMX_CLIP,-(%a1)        | [205]
      move.w   _YMX_CLIP,-(%a1)        | [206]



      cmp.w    (-116,%a6),%d2          | [210 GE]
      jbge     chk_ymx                 | [211]



|;;   add.w     d3,d2                  | [215]
	add.l    %d3,%d2
      cmp.w    (-116,%a6),%d2          | [216 GT]
      jbgt     chk_xmn                 | [217]
      jbra     upda_dst                | [218]
chk_ymx:
      cmp.w    (-120,%a6),%d2          | [220 GT]
      jbgt     upda_dst                | [221]

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   add.w     d3,d2                  | [223]
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [224]
	subq.l   #1,%d2
|;;   sub.w     -120(a6),d2            | [225 LE]
	ext.l    %d2
	mvs.w    (-120,%a6),%d0
	sub.l    %d0,%d2
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
	lea      4(%a7),%a7
      jble     chk_xmn                 | [226]



|;;   andi.w    #1,d2                  | [230 EQ]
	andi.l   #(0xFFFF&1),%d2
      jbeq     chk_xmn                 | [231]
      btst     #4,(-96+1,%a6)          | [232 NE]
      jbne     chk_xmn                 | [233]
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   rol.w     -100(a6)               | [234]
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
      cmp.w    (-114,%a6),%d0          | [239 GE]
      jbge     chk_xmx                 | [240]



|;;   add.w     d1,d0                  | [244]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   not.w     d5                     | [245]
	move.w   %d2,%d0
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5
      cmp.w    (-114,%a6),%d0          | [246 GT]
      jbgt     clp_done                | [247]
      jbra     upda_dst                | [248]
chk_xmx:
      cmp.w    (-118,%a6),%d0          | [250 GT]
      jbgt     upda_dst                | [251]

|;;   add.w     d1,d0                  | [253]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   subq.w    #1,d0                  | [254]
	subq.l   #1,%d2
	move.w   %d2,%d0
      cmp.w    (-118,%a6),%d0          | [255 LE]
      jble     clp_done                | [256]
|;;   not.w     d5                     | [257]
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5



clp_done:
      clr.w    (-28,%a6)               | [262]

|;;   move.w    _FWIDTH,-16(a6)        | [264]
	move.w   _FWIDTH,%d2
	move.w   %d2,(-16,%a6)
|;;   move.l    _FBASE,-10(a6)         | [265]
	move.l   _FBASE,%d2
	move.l   %d2,(-10,%a6)

chk_skw:
      move.w   (-96,%a6),%d1           | [268]
|;;   andi.w    #0x4 +0x1 +0x10,d1     | [269 EQ]
	andi.l   #(0xFFFF0000!~(0x4+0x1+0x10)),%d1
	tst.w    %d1
      jbeq     chk_rota                | [270]



chk_chup:
      tst.w    (-110,%a6)              | [275 NE]
      jbne     preblt                  | [276]

      btst     #2,%d1                  | [278 EQ]
      jbeq     preskew                 | [279]
      tst.w    %d5                     | [280 NE]
      jbne     preblt                  | [281]
preskew:
      btst     #4,%d1                  | [283 EQ]
      jbeq     chk_rota                | [284]






preblt:
|***	SAVE	d4	***
	lea      -4(%a7),%a7
	movem.l  %d4,(%a7)
      move.w   _SOURCEX,%d0            | [292]
      move.w   %d0,%d2                 | [293]
|;;   andi.w    #0x0F,d2               | [294]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [295]
|;;   lsr.w     #4,d0                  | [296]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [297]
	lsl.l    #1,%d0

      move.w   _SOURCEY,%d2            | [299]
      move.w   (-106,%a6),(-22,%a6)    | [300]
|;;   add.w     -22(a6),d2             | [301]
	move.w   (-22,%a6),%d3
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [302]
	subq.l   #1,%d2

      mulu.w   (-16,%a6),%d2           | [304]
|;;   neg.w     -16(a6)                | [305]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   add.w     d0,d2                  | [306]
	move.w   %d2,%d4
	add.l    %d0,%d4
      movea.l  (-10,%a6),%a0           | [307]
	move.w   %d4,%d2
|;;   lea       (a0,d2.w),a0           | [308]
	move.w   %d2,%a5
	lea      (0,%a0,%a5.l*1),%a0

      move.w   (-102,%a6),%d0          | [310]
      move.w   _WEIGHT,%d1             | [311]
|;;   add.w     d6,d7                  | [312]
	add.l    %d6,%d7
      tst.w    _DOUBLE                 | [313 EQ]
      jbeq     full_wt                 | [314]
|;;   lsr.w     #1,d1                  | [315 NE]
	mvz.w    %d1,%d4
	lsr.l    #1,%d4
	move     %ccr,%d3
	move.w   %d4,%d1
	move     %d3,%ccr
      jbne     ful_wtok                | [316]
|;;   addq.w    #1,d1                  | [317]
	addq.l   #1,%d4
	move.w   %d4,%d1
ful_wtok:
|;;   lsr.w     #1,d7                  | [319]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
full_wt:
      btst     #0,(-96+1,%a6)          | [321 EQ]
      jbeq     no_smear                | [322]

|;;   add.w     d1,d0                  | [324]
	add.l    %d1,%d0
      move.w   %d1,(-74,%a6)           | [325]
no_smear:

      clr.w    (-26,%a6)               | [328]
      move.w   (-106,%a6),%d1          | [329]
      move.w   (-96,%a6),%d2           | [330]
      btst     #4,%d2                  | [331 EQ]
      jbeq     no_out                  | [332]
      tst.w    _DOUBLE                 | [333 NE]
      jbne     no_out                  | [334]

|;;   addq.w    #3,d0                  | [336]
	addq.l   #3,%d0
|;;   addq.w    #1,-26(a6)             | [337]
	move.w   (-26,%a6),%d4
	addq.l   #1,%d4
	move.w   %d4,(-26,%a6)
|;;   addq.w    #2,-106(a6)            | [338]
	move.w   (-106,%a6),%d4
	addq.l   #2,%d4
	move.w   %d4,(-106,%a6)
|;;   addq.w    #3,d1                  | [339]
	move.w   %d1,%d4
	addq.l   #3,%d4
	move.w   %d4,%d1
no_out:

      move.w   %d0,(-20,%a6)           | [342]
|;;   add.w     d7,d0                  | [343]
	add.l    %d7,%d0
      move.w   %d0,(-102,%a6)          | [344]
|;;   lsr.w     #4,d0                  | [345]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [346]
	lsl.l    #1,%d0
|;;   addq.w    #2,d0                  | [347]
	addq.l   #2,%d0
|;;   neg.w     d0                     | [348]
	neg.l    %d0
      move.w   %d0,(-18,%a6)           | [349]
|;;   neg.w     d0                     | [350]
	neg.l    %d0
|;;   subq.w    #1,d1                  | [351]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [352]
	mulu.w   %d4,%d0
      movea.l  _scrtchp,%a1            | [353]
|;;   adda.w    -2(a6),a1              | [354]
	mvs.w    (-2,%a6),%d3
	adda.l   %d3,%a1
      move.l   %a1,(-10,%a6)           | [355]
      btst     #4,%d2                  | [356 NE]
      jbne     do_clear                | [357]
      btst     #2,%d2                  | [358 EQ]
      jbeq     no_clear                | [359]
do_clear:

      movea.l  %a1,%a2                 | [362]
      move.w   %d0,%d3                 | [363]
|;;   sub.w     -18(a6),d3             | [364]
	move.w   (-18,%a6),%d1
	sub.l    %d1,%d3
|;;   lsr.w     #1,d3                  | [365]
	and.l    #0xffff,%d3
	lsr.l    #1,%d3
|;;   subq.w    #1,d3                  | [366]
	subq.l   #1,%d3
      moveq.l  #0,%d1                  | [367]
replp:
      move.w   %d1,(%a2)+              | [369]
|;;   dbra      d3,replp               | [370]
	tst.w    %d3
	jbeq     _PA_369_
	subq.l   #1,%d3
	jbra     replp
	_PA_369_:         

      btst     #4,%d2                  | [372 EQ]
      jbeq     no_clear                | [373]
      tst.w    _DOUBLE                 | [374 NE]
      jbne     no_clear                | [375]

|;;   subq.w    #3,-20(a6)             | [377]
	move.w   (-20,%a6),%d4
	subq.l   #3,%d4
	move.w   %d4,(-20,%a6)
|;;   subq.w    #1,-102(a6)            | [378]
	move.w   (-102,%a6),%d4
	subq.l   #1,%d4
	move.w   %d4,(-102,%a6)
|;;   add.w     -18(a6),d0             | [379]
	move.w   %d0,%d4
	move.w   (-18,%a6),%d3
	add.l    %d3,%d4
	move.w   %d4,%d0



no_clear:
|;;   adda.w    d0,a1                  | [384]
	mvs.w    %d0,%d3
	adda.l   %d3,%a1



      move.w   (-96,%a6),(-122,%a6)    | [388]

      clr.w    (-98,%a6)               | [390]
      move.w   #1,(-84,%a6)            | [391]
      clr.w    (-76,%a6)               | [392]

      move.w   #1,(-86,%a6)            | [394]
      move.w   #2,(-88,%a6)            | [395]
|;;   andi.w    #0x4 +0x1,-96(a6)      | [396]
	move.w   (-96,%a6),%d4
	andi.l   #(0xFFFF&(0x4+0x1)),%d4
	move.w   %d4,(-96,%a6)
|***	RESTORE	AFTER d4	***
	move.l   (%a7)+,%d4

      jbsr     norm_blt                | [398]

      move.w   (-122,%a6),(-96,%a6)    | [400]
|;;   move.w    _WRT_MODE,-98(a6)      | [401]
	move.w   _WRT_MODE,%d2
	move.w   %d2,(-98,%a6)
no_mode:
      move.w   (-18,%a6),%d7           | [403]
|;;   neg.w     d7                     | [404]
	move.w   %d7,%d2
	neg.l    %d2
|;;   move.w    d7,-16(a6)             | [405]
	move.w   %d2,(-16,%a6)
	move.w   %d2,%d7



      btst     #4,(-96+1,%a6)          | [409 EQ]
      jbeq     skip_out                | [410]
      tst.w    _DOUBLE                 | [411 NE]
      jbne     skip_out                | [412]

      movea.l  (-10,%a6),%a0           | [414]
      ext.l    %d7                     | [415]
      add.l    %d7,(-10,%a6)           | [416]
      jbsr     outlin                  | [417]
skip_out:



      clr.w    _SOURCEX                | [422]
      clr.w    _SOURCEY                | [423]


|;;   andi.w    #0xFFFF-(0x4 +0x1),-96(a6) | [426]
	move.w   (-96,%a6),%d2
	andi.l   #(0xFFFF&(0xFFFF-(0x4+0x1))),%d2
	move.w   %d2,(-96,%a6)



chk_rota:
      tst.w    (-110,%a6)              | [431 EQ]
      jbeq     chk_db                  | [432]

      jbsr     rotation                | [434]

chk_db:
      tst.w    _DOUBLE                 | [437 EQ]
      jbeq     do_clip                 | [438]

      jbsr     replicat                | [440]







do_clip:
      btst     #0,(-96+1,%a6)          | [449 EQ]
      jbeq     no_thik                 | [450]
      move.w   _WEIGHT,%d0             | [451]
      tst.w    _MONO_STATUS            | [452 NE]
      jbne     do_cl_nt                | [453]
|;;   add.w     d0,-102(a6)            | [454]
	move.w   (-102,%a6),%d2
	add.l    %d0,%d2
	move.w   %d2,(-102,%a6)
do_cl_nt:
      move.w   %d0,(-74,%a6)           | [456]
no_thik:
      tst.w    (-112,%a6)              | [458 NE]
      jbne     clip_adjust             | [459]
      clr.w    (-114,%a6)              | [460]
      clr.w    (-116,%a6)              | [461]
|;;   move.w    _DEV_TAB,-118(a6)      | [462]
	move.w   _DEV_TAB,%d2
	move.w   %d2,(-118,%a6)
|;;   move.w    _DEV_TAB+2,-120(a6)    | [463]
	move.w   _DEV_TAB+2,%d2
	move.w   %d2,(-120,%a6)

clip_adjust:
      move.w   (-108,%a6),%d0          | [466]
      cmp.w    (-116,%a6),%d0          | [467 GE]
      jbge     ymn_fine                | [468]
|;;   add.w     -106(a6),d0            | [469]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-116,%a6),%d0          | [470 LE]
      jble     upda_dst                | [471]
mn_clipy:
|;;   sub.w     -116(a6),d0            | [473]
	move.w   (-116,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-106,%a6),%d1          | [474]
|;;   move.w    d0,-106(a6)            | [475]
	move.w   %d7,(-106,%a6)
|;;   sub.w     d0,d1                  | [476]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEY            | [477]
	move.w   _SOURCEY,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEY
      move.w   (-116,%a6),%d0          | [478]
      move.w   %d0,(-108,%a6)          | [479]
ymn_fine:
      cmp.w    (-120,%a6),%d0          | [481 GT]
      jbgt     upda_dst                | [482]
mx_clipy:
|;;   add.w     -106(a6),d0            | [484]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
|;;   subq.w    #1,d0                  | [485]
	subq.l   #1,%d7
	move.w   %d7,%d0
      cmp.w    (-120,%a6),%d0          | [486 LE]
      jble     ymx_fine                | [487]

|;;   sub.w     -120(a6),d0            | [489]
	move.w   (-120,%a6),%d2
	sub.l    %d2,%d7
|;;   sub.w     d0,-106(a6)            | [490]
	move.w   (-106,%a6),%d2
	sub.l    %d7,%d2
	move.w   %d2,(-106,%a6)
ymx_fine:
      move.w   (-104,%a6),%d0          | [492]
      cmp.w    (-114,%a6),%d0          | [493 GE]
      jbge     xmn_fine                | [494]
|;;   add.w     -102(a6),d0            | [495]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-114,%a6),%d0          | [496 LE]
      jble     upda_dst                | [497]

mn_clipx:
|;;   sub.w     -114(a6),d0            | [500]
	move.w   (-114,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-102,%a6),%d1          | [501]
|;;   move.w    d0,-102(a6)            | [502]
	move.w   %d7,(-102,%a6)
|;;   sub.w     d0,d1                  | [503]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEX            | [504]
	move.w   _SOURCEX,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEX
      move.w   (-114,%a6),%d0          | [505]
      move.w   %d0,(-104,%a6)          | [506]
xmn_fine:
      cmp.w    (-118,%a6),%d0          | [508 GT]
      jbgt     upda_dst                | [509]
mx_clipx:
|***	SAVE	d3	***
	lea      -4(%a7),%a7
	movem.l  %d3,(%a7)
|;;   add.w     -102(a6),d0            | [511]
	move.w   %d0,%d3
	move.w   (-102,%a6),%d2
	add.l    %d2,%d3
|;;   subq.w    #1,d0                  | [512]
	subq.l   #1,%d3
	move.w   %d3,%d0
      cmp.w    (-118,%a6),%d0          | [513 LE]
      jble     scrn_blt                | [514]

|;;   sub.w     -118(a6),d0            | [516]
	move.w   (-118,%a6),%d2
	sub.l    %d2,%d3
|;;   sub.w     d0,-102(a6)            | [517]
	move.w   (-102,%a6),%d2
	sub.l    %d3,%d2
	move.w   %d2,(-102,%a6)

      jbra     scrn_blt                | [519]
| 924 "vdi/vdi_tblit.S"
scrn_blt:
|;;   move.w    _TEXT_FG,-84(a6)       | [522]
	move.w   _TEXT_FG,%d2
	move.w   %d2,(-84,%a6)
      move.w   (-6,%a6),(-76,%a6)      | [523]
      move.w   _v_planes,%d0           | [524]
      move.w   %d0,(-86,%a6)           | [525]
|;;   add.w     d0,d0                  | [526]
	move.w   %d0,%d3
	add.l    %d0,%d3
|;;   move.w    d0,-88(a6)             | [527]
	move.w   %d3,(-88,%a6)

      move.w   _SOURCEX,%d0            | [529]
      move.w   %d0,%d2                 | [530]
|;;   andi.w    #0x0F,d2               | [531]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [532]
|;;   lsr.w     #3,d0                  | [533]
	mvz.w    %d0,%d3
	lsr.l    #3,%d3
|;;   andi.w    #0xFFFE,d0             | [534]
	andi.l   #(0xFFFF&0xFFFE),%d3
      move.w   _SOURCEY,%d2            | [535]

      move.w   (-106,%a6),(-22,%a6)    | [537]
|;;   add.w     -22(a6),d2             | [538]
	move.w   (-22,%a6),%d7
	add.l    %d7,%d2
|;;   subq.w    #1,d2                  | [539]
	subq.l   #1,%d2
      mulu.w   (-16,%a6),%d2           | [540]
|;;   neg.w     -16(a6)                | [541]
	move.w   %d3,%d0
	move.w   (-16,%a6),%d3
	neg.l    %d3
	move.w   %d3,(-16,%a6)
|;;   add.w     d0,d2                  | [542]
	move.w   %d2,%d3
	add.l    %d0,%d3



      movea.l  (-10,%a6),%a0           | [546]
|;;   adda.w    d2,a0                  | [547]
	ext.l    %d3
	adda.l   %d3,%a0
      move.l   %a0,(-10,%a6)           | [548]

      move.w   (-102,%a6),(-20,%a6)    | [550]

      move.w   (-108,%a6),%d1          | [552]
|;;   add.w     -106(a6),d1            | [553]
	move.w   %d1,%d3
	move.w   (-106,%a6),%d7
	add.l    %d7,%d3
	move.w   %d3,%d1
|***	RESTORE	d3	***
	move.l   (%a7)+,%d3
|;;   subq.w    #1,d1                  | [554 MI]
	move.w   %d1,%d7
	subq.l   #1,%d7
	move.w   %d7,%d1
      bmi.b    upda_dst                | [555]

      move.w   (-104,%a6),%d0          | [557 MI]
      bmi.b    upda_dst                | [558]

      jsr      concat                  | [560]



      movea.l  _v_bas_ad,%a1           | [564]
      adda.l   %d1,%a1                 | [565]

      move.l   %a1,(-14,%a6)           | [567]
      move.w   %d0,(-26,%a6)           | [568]

      move.w   _v_lin_wr,%d0           | [570]
|;;   neg.w     d0                     | [571]
	move.w   %d0,%d3
	neg.l    %d3
|;;   move.w    d0,-18(a6)             | [572]
	move.w   %d3,(-18,%a6)

      jbsr     norm_blt                | [574]

upda_dst:
      move.w   _DELX,%d1               | [577]

      tst.w    _DOUBLE                 | [579 EQ]
      jbeq     upda_ndb                | [580]



      move.w   (-90,%a6),%d1           | [584]
      move.w   (-92,%a6),%d3           | [585]

      tst.w    (-94,%a6)               | [587 EQ]
      jbeq     upda_ndb                | [588]
|;;   exg       d1, d3                 | [589]
	move.l   %d1,%d2
	move.l   %d3,%d1
	move.l   %d2,%d3





upda_ndb:
|;;   btst.b    #0,_STYLE+1            | [596 EQ]
	moveq.l  #0,%d2
	btst     %d2,_STYLE+1
      jbeq     upda_ntk                | [597]
      tst.w    _MONO_STATUS            | [598 NE]
      jbne     upda_ntk                | [599]
|;;   add.w     _WEIGHT,d1             | [600]
	move.w   _WEIGHT,%d2
	add.l    %d2,%d1
upda_ntk:
      tst.w    (-110,%a6)              | [602 GT]
      jbgt     ck90                    | [603]
|;;   add.w     d1,_DESTX              | [604]
	move.w   _DESTX,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [605]
ck90:
      tst.w    (-126,%a6)              | [607 EQ GT]
      jbeq     ck180                   | [608]
      jbgt     ck270                   | [609]
|;;   sub.w     d1,_DESTY              | [610]
	move.w   _DESTY,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTY
      jbra     blt_done                | [611]
ck180:
|;;   sub.w     d1,_DESTX              | [613]
	move.w   _DESTX,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [614]
ck270:
|;;   add.w     d1,_DESTY              | [616]
	move.w   _DESTY,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTY
blt_done:
|;;   movem.l   (sp)+,d2-d7/a2-a5      | [618]
	movem.l  (%a7),%d2-%d7/%a2-%a5
	lea      40(%a7),%a7
      unlk     %a6                     | [619]
      rts                              | [620]

|;;           .text                    | [622]
| 1034 "vdi/vdi_tblit.S"
|;;                   .even            | [624]
	.p2align 1,0
|;;   lf_tab: .dc.w 0xFFFF             | [625]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [626]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [627]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [628]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [629]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [630]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [631]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [632]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [633]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [634]
	.short	0x007F
|;;                   .dc.w 0x003F     | [635]
	.short	0x003F
|;;                   .dc.w 0x001F     | [636]
	.short	0x001F
|;;                   .dc.w 0x000F     | [637]
	.short	0x000F
|;;                   .dc.w 0x0007     | [638]
	.short	0x0007
|;;                   .dc.w 0x0003     | [639]
	.short	0x0003
|;;                   .dc.w 0x0001     | [640]
	.short	0x0001
|;;                   .dc.w 0x0000     | [641]
	.short	0x0000

|;;           .text                    | [643]
| 1066 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [646]
|;;   sub.w     -24(a6),d1             | [647]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [648 PL]
      bpl.b    do_rot                  | [649]



|;;   neg.w     d1                     | [653]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [654]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [655]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [657]

|;;   lsl.w     #1,d0                  | [659]
	move.w   %d0,%d5
	lsl.l    #1,%d5
      lea      lf_tab,%a2              | [660]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [661]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [662]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [663]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [668]
|;;   lsl.w     #1,d0                  | [669]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [670]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [672]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [673]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [676]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [677]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [678]
	andi.l   #(0xFFFF&0xF),%d5
|;;   lsl.w     #1,d2                  | [679]
	lsl.l    #1,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [680]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [681]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [682]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [683]
      move.w   #0x8000,(-80,%a6)       | [684]
      moveq.l  #(-1),%d3               | [685]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [688 HI]
      bhi.b    doub_des                | [689]



|;;   lsl.w     #1,d0                  | [693]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [694]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [695]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [696]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [698]
	move.w   %d5,%d1
      jbra     msk_done                | [699]



doub_des:
      move.w   %d0,%d1                 | [704]
|;;   lsr.w     #4,d1                  | [705]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [706]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [707 NE]
      jbne     mlt_dest                | [708]



      move.w   (-24,%a6),%d1           | [712]
|;;   add.w     -20(a6),d1             | [713]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [714 CC]
      jbcc     mlt_dest                | [715]
      moveq.l  #0,%d7                  | [716]
      jbra     do_ritem                | [717]

mlt_dest:
      tst.w    (-42,%a6)               | [720 LT]
      jblt     mltleft                 | [721]
      moveq.l  #4,%d7                  | [722]
      jbra     do_ritem                | [723]
mltleft:
      moveq.l  #8,%d7                  | [725]
do_ritem:
|;;   andi.w    #0x000F,d0             | [727 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jbne     not_null                | [728]
|;;   subq.w    #1,d3                  | [729]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [730]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [732]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [733]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [734]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [737]
      move.w   %d3,(-28,%a6)           | [738]
      move.w   %d4,(-36,%a6)           | [739]
|;;   addq.w    #2,d3                  | [740]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [741 CS]
      jbcs     msk0                    | [742]
|;;   addq.w    #1,d3                  | [743]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [745]
      move.w   %d3,(-30,%a6)           | [746]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jbra     plane_loop              | [747]
| 1185 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [750]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [751]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [752]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [753]
	.byte	4*5,4*5,4*6,4*6

toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [756]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [758]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [762]
|;;   lsr.w     -84(a6)                | [763 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [764]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [765 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [766]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [770]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [771]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [772]
      movea.l  %a2,%a5                 | [773]

|;;   movea.l   twoptble(pc,d0.w),a4   | [775]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [779 EQ]
      jbeq     wrt_char                | [780]

      btst     #1,%d0                  | [782 EQ]
      jbeq     no_lite                 | [783]



|;;   move.w    _LITEMASK,-78(a6)      | [787]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [788]
      lea      liteop,%a5              | [789]

      move.l   %a2,(-64,%a6)           | [791]
      lea      liteopwf,%a2            | [792]

      move.l   %a4,(-72,%a6)           | [794]
      lea      liteopw,%a4             | [795]

no_lite:
      btst     #0,%d0                  | [798 EQ]
      jbeq     no_thick                | [799]



      clr.w    (-82,%a6)               | [803]

      move.l   %a5,(-48,%a6)           | [805]
      lea      thknop,%a5              | [806]

      move.l   %a2,(-60,%a6)           | [808]
      lea      thknopwf,%a2            | [809]

      move.l   %a4,(-68,%a6)           | [811]
      lea      thknopw,%a4             | [812]

no_thick:
      btst     #2,%d0                  | [815 EQ]
      jbeq     wrt_char                | [816]



      move.w   (-100,%a6),(-80,%a6)    | [820]
      move.l   %a5,(-56,%a6)           | [821]
      lea      skewop,%a5              | [822]

      tst.w    (-124,%a6)              | [824 EQ GT]
      jbgt     wrt_char                | [825]
      jbeq     not_sngl                | [826]
      clr.w    (-28,%a6)               | [827]
      moveq.l  #0,%d7                  | [828]
      jbra     chng_blt                | [829]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [831 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      bls.b    wrt_char                | [832]

      tst.w    (-42,%a6)               | [834 LT]
      jblt     chng_left               | [835]
      moveq.l  #4,%d7                  | [836]
      jbra     chng_blt                | [837]
chng_left:
      moveq.l  #8,%d7                  | [839]
chng_blt:
      move.w   %d7,(-124,%a6)          | [841]

wrt_char:
      move.w   (-124,%a6),%d7          | [844 LE]
      jble     set_chr_hgt             | [845]
      movea.l  %a2,%a5                 | [846]
      move.w   (-18,%a6),%d5           | [847]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [849]
|;;   subq.w    #1,d3                  | [850]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [852]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [853]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [855 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jble     no_more_planes          | [856]
      movea.l  (-10,%a6),%a0           | [857]
      movea.l  (-14,%a6),%a1           | [858]
      addq.l   #2,%a1                  | [859]
      move.l   %a1,(-14,%a6)           | [860]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [861 EQ]
      jbeq     plane_loop              | [862]
      jbra     norm_blt                | [863]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [865]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [867]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [873]
      move.w   (-16,%a6),%d6           | [874]
      move.w   (-18,%a6),%d7           | [875]
      lea      sgl_rtn,%a3             | [876]
      jbra     sgl_1st_scan            | [877]

sgl_lp:
|;;   adda.w    d6,a0                  | [880]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [881]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [883]
| 1328 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [885]


      move.w   (-42,%a6),%d1           | [888 MI]
      bmi.b    left_rol                | [889]
      lsr.l    %d1,%d0                 | [890]
      jbra     end_ro                  | [891]
left_rol:
      lsl.l    %d1,%d0                 | [893]
end_ro:
      swap     %d0                     | [895]
      move.w   %d0,%d1                 | [896]
      swap     %d0                     | [897]

      jmp      (%a5)                   | [899]

sgl_rtn:
      move.w   %d1,(%a1)               | [902]
|;;   dbra      d3,sgl_lp              | [903]
	tst.w    %d3
	jbeq     _PA_902_
	subq.l   #1,%d3
	jbra     sgl_lp
	_PA_902_:         
      jbra     next_plane              | [904]




dbl_loop:
      movea.l  %a3,%a4                 | [910]
      move.w   (-16,%a6),%d6           | [911]
      move.w   (-18,%a6),%d7           | [912]
      lea      dbl_2wrd,%a2            | [913]
      move.l   %a2,%d5                 | [914]
      lea      dbl_1wrd,%a3            | [915]
      jbra     dbl_1st_scan            | [916]

dbl_lp:
|;;   adda.w    d6,a0                  | [919]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [920]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [921]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [923]
| 1375 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [925]


      move.w   (-42,%a6),%d1           | [928 MI]
      bmi.b    dbleft                  | [929]
      lsr.l    %d1,%d0                 | [930]
      jbra     dblendr                 | [931]
dbleft:
      lsl.l    %d1,%d0                 | [933]
dblendr:
      swap     %d0                     | [935]
      move.w   %d0,%d1                 | [936]
      swap     %d0                     | [937]
      move.w   (-38,%a6),%d2           | [938]

      jmp      (%a5)                   | [940]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [943]
      move.w   %d0,%d1                 | [944]

      movea.w  (-88,%a6),%a2           | [946]
|;;   move.w    (a1,a2.w),d4           | [947]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [949]

|;;   exg       a3,d5                  | [951]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [952]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [955]
|;;   move.w    d1,(a1,a2.w)           | [956]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [958]
	tst.w    %d3
	jbeq     _PA_957_
	subq.l   #1,%d3
	jbra     dbl_lp
	_PA_957_:         
      jbra     next_plane              | [959]





left_loop:
      movea.l  %d6,%a0                 | [966]
      movea.l  %d7,%a1                 | [967]
|;;   adda.w    -16(a6),a0             | [968]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [969]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [970]
|;;   rol.w     -78(a6)                | [971]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [973 NE]
      jbne     skewopw                 | [974]
mlt_left:
      move.l   %a0,%d6                 | [976]
      move.l   %a1,%d7                 | [977]
      move.w   (-28,%a6),(-44,%a6)     | [978]







      move.l   (%a0)+,%d0              | [986]


      move.w   (%a1),%d4               | [989]

      move.w   (-42,%a6),%d1           | [991]
      lsl.l    %d1,%d0                 | [992]

      swap     %d0                     | [994]
      move.w   %d0,%d1                 | [995]
      swap     %d0                     | [996]

      move.w   (-38,%a6),%d2           | [998]

      lea      lft_lfrng,%a3           | [1000]
      jmp      (%a5)                   | [1001]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1004]
|;;   adda.w    -88(a6),a1             | [1005]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1007]
      lea      lft_wrds,%a3            | [1008]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1013]
|;;   and.w     d2,d4                  | [1014]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [1015]





      swap     %d0                     | [1021]
      move.w   %d1,%d0                 | [1022]
      swap     %d0                     | [1023]

      move.w   (-42,%a6),%d1           | [1025]
      lsl.l    %d1,%d0                 | [1026]

      swap     %d0                     | [1028]
      move.w   %d0,%d1                 | [1029]
      swap     %d0                     | [1030]

|;;   not.w     d2                     | [1032]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1033]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1034]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1035]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1037]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1039 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     lef_don                 | [1040]

      jmp      (%a4)                   | [1042]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1045]
|;;   adda.w    -88(a6),a1             | [1046]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_lef                | [1048]

lef_don:
      move.w   (-36,%a6),%d2           | [1051]

      lea      lft_rfrng,%a3           | [1053]
      jmp      (%a5)                   | [1054]

lft_rfrng:
      move.w   %d1,(%a1)               | [1057]

|;;   dbra      d3,left_loop           | [1059]
	tst.w    %d3
	jbeq     _PA_1058_
	subq.l   #1,%d3
	jbra     left_loop
	_PA_1058_:         
      jbra     next_plane              | [1060]





rite_loop:
      movea.l  %d6,%a0                 | [1067]
      movea.l  %d7,%a1                 | [1068]
|;;   adda.w    -16(a6),a0             | [1069]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [1070]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [1071]
|;;   rol.w     -78(a6)                | [1072]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [1074 NE]
      jbne     skewopw                 | [1075]
mlt_rite:
      move.l   %a0,%d6                 | [1077]
      move.l   %a1,%d7                 | [1078]
      move.w   (-28,%a6),(-44,%a6)     | [1079]
      move.w   (%a0)+,%d0              | [1080]



      swap     %d0                     | [1084]

      move.w   (%a1),%d4               | [1086]

      move.w   (-42,%a6),%d1           | [1088]
      lsr.l    %d1,%d0                 | [1089]
      swap     %d0                     | [1090]
      move.w   %d0,%d1                 | [1091]
      swap     %d0                     | [1092]

      move.w   (-38,%a6),%d2           | [1094]

      lea      rgt_lfrng,%a3           | [1096]
      jmp      (%a5)                   | [1097]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1100]
|;;   adda.w    -88(a6),a1             | [1101]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1103]
      lea      rgt_wrds,%a3            | [1104]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1109]
|;;   and.w     d2,d4                  | [1110]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [1111]
      move.w   (%a0)+,%d0              | [1112]



      swap     %d0                     | [1116]

      move.w   (-42,%a6),%d1           | [1118]
      lsr.l    %d1,%d0                 | [1119]
      swap     %d0                     | [1120]
      move.w   %d0,%d1                 | [1121]
      swap     %d0                     | [1122]

|;;   not.w     d2                     | [1124]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1125]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1126]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1127]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1129]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1131 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     rite_don                | [1132]

      jmp      (%a4)                   | [1134]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1137]
|;;   adda.w    -88(a6),a1             | [1138]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_rit                | [1140]

rite_don:
      move.w   (-36,%a6),%d2           | [1143]

      lea      rgt_rfrng,%a3           | [1145]
      jmp      (%a5)                   | [1146]

rgt_rfrng:
      move.w   %d1,(%a1)               | [1149]

|;;   dbra      d3,rite_loop           | [1151]
	tst.w    %d3
	jbeq     _PA_1150_
	subq.l   #1,%d3
	jbra     rite_loop
	_PA_1150_:         
      jbra     next_plane              | [1152]





top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1159]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [1160]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [1161]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [1162]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1163]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1166]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [1167]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [1168]
	not.l    %d0
|;;   and.w     d4,d1                  | [1169]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1170]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1173]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [1174]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1175]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1176]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1179]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [1180]
	not.l    %d0
|;;   and.w     d4,d1                  | [1181]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1182]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1185]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1186]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1187]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1190]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1191]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1192]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1195]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [1196]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1197]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1198]





twop0:
      moveq.l  #0,%d1                  | [1205]
      jmp      (%a3)                   | [1206]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [1209]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1210]

twop3:
      jmp      (%a3)                   | [1213]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1216]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [1217]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1218]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1221]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1222]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [1225]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1226]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1229]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [1230]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1231]
| 1701 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [1234]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [1235]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [1236]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [1237 EQ]
      jbeq     thk00                   | [1238]
|;;   ori.b     #0x10,ccr              | [1239 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [1240 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jbcc     thk01                   | [1241]
      move.w   #0x8000,%d6             | [1242]
thk01:
      move.w   %d6,(-34,%a6)           | [1244]
thk00:
      tst.w    (-28,%a6)               | [1246 MI]
      bmi.b    thk0                    | [1247]
      cmp.w    (-36,%a6),%d6           | [1248 CC]
      jbcc     thk0                    | [1249]
|;;   and.w     d6,d0                  | [1250]
	and.l    %d6,%d0
      jbra     thk1                    | [1251]
thk0:
      clr.w    %d0                     | [1253]
|;;   and.w     d6,d1                  | [1254]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [1256]
      move.w   %d2,%d5                 | [1257]
      swap     %d5                     | [1258]
      move.w   (-36,%a6),%d5           | [1259]
      lsl.l    %d6,%d5                 | [1260]

      swap     %d5                     | [1262]
      move.w   %d5,%d7                 | [1263]
      swap     %d5                     | [1264]

|;;   and.w     d7,d1                  | [1266]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [1267]
	and.l    %d5,%d0
|;;   swap      d1                     | [1268]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [1269]
|;;   subq.w    #1,d6                  | [1270]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [1272]
      lsr.l    #1,%d0                  | [1273]
      or.l     %d0,%d1                 | [1274]
|;;   dbra      d6,thkoplp             | [1275]
	tst.w    %d6
	jbeq     _PA_1274_
	subq.l   #1,%d6
	jbra     thkoplp
	_PA_1274_:         

      move.w   %d1,%d0                 | [1277]
      swap     %d1                     | [1278]
|;;   movem.l   (sp)+,d5-d7            | [1279]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [1280]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [1281]
| 1764 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [1284]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [1285 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk2                    | [1286]
|;;   and.w     -34(a6),d1             | [1287]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [1289]
      swap     %d1                     | [1290]
      clr.w    %d1                     | [1291]
      move.l   %d1,%d5                 | [1292]
|;;   subq.w    #1,d6                  | [1293]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [1295]
      or.l     %d5,%d1                 | [1296]
|;;   dbra      d6,thkopwlp            | [1297]
	tst.w    %d6
	jbeq     _PA_1296_
	subq.l   #1,%d6
	jbra     thkopwlp
	_PA_1296_:         

      move.w   %d1,%d5                 | [1299]
      swap     %d1                     | [1300]
|;;   or.w      -82(a6),d1             | [1301]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1302]
|;;   movem.l   (sp)+,d5-d6            | [1303]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [1304]
      jmp      (%a2)                   | [1305]
| 1802 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [1308]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [1309]
|;;   subq.w    #1,-30(a6)             | [1310 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      bmi.b    thk3                    | [1311]
      jbeq     thk4                    | [1312]

|;;   subq.w    #1,-30(a6)             | [1314 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk11                   | [1315]
|;;   and.w     d6,d1                  | [1316]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [1318]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jbra     thk7                    | [1319]

thk3:
      clr.w    %d1                     | [1322]
      jbra     thk5                    | [1323]

thk4:
|;;   and.w     d6,d1                  | [1326]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [1329 MI]
      bmi.b    thk6                    | [1330]
      move.w   (-28,%a6),%d5           | [1331]
|;;   addq.w    #2,d5                  | [1332]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [1333]
|;;   ori.b     #0x10,ccr              | [1334 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [1335 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jbcc     thk33                   | [1336]
      move.w   #0x8000,%d7             | [1337]
|;;   addq.w    #1,d5                  | [1338]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [1340 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jbne     thk34                   | [1341]
|;;   subq.w    #1,d5                  | [1342]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [1344 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [1345 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jbcc     thk31                   | [1346]
      move.w   #0x8000,%d6             | [1347]
thk31:
      cmp.w    %d6,%d7                 | [1349 CS]
      jbcs     thk35                   | [1350]
|;;   addq.w    #1,d5                  | [1351]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [1353]
      move.w   %d5,(-32,%a6)           | [1354]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [1356]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:
      move.w   (-74,%a6),%d6           | [1358]
      swap     %d1                     | [1359]
      clr.w    %d1                     | [1360]
      move.l   %d1,%d5                 | [1361]
|;;   subq.w    #1,d6                  | [1362]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [1364]
      or.l     %d5,%d1                 | [1365]
|;;   dbra      d6,thkopwl             | [1366]
	tst.w    %d6
	jbeq     _PA_1365_
	subq.l   #1,%d6
	jbra     thkopwl
	_PA_1365_:         

      move.w   %d1,%d5                 | [1368]
      swap     %d1                     | [1369]
|;;   or.w      -82(a6),d1             | [1370]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1371]
|;;   and.w     d2,d1                  | [1372]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [1373]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [1374]
      jmp      (%a2)                   | [1375]
| 1883 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [1378]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [1379]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [1380]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [1381]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1382]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1385]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [1386]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1387]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1390]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [1391]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1392]
| 1915 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [1395 CC]
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
      jbcc     no_shift                | [1396]

      swap     %d1                     | [1398]
      move.w   %d0,%d1                 | [1399]
      lsr.l    #1,%d1                  | [1400]

      swap     %d2                     | [1402]
      move.w   (-36,%a6),%d2           | [1403]
      lsr.l    #1,%d2                  | [1404]
      move.w   %d2,(-36,%a6)           | [1405]
      swap     %d2                     | [1406]
      move.w   %d2,(-38,%a6)           | [1407 EQ]
      jbeq     nxt_word                | [1408]
      move.w   (-42,%a6),%d0           | [1409 MI]
      bmi.b    dec_rol                 | [1410]
ror_add:
|;;   addq.w    #1,d0                  | [1412]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [1414]
      move.w   %d1,%d0                 | [1415]
      swap     %d1                     | [1416]
no_shift:
      movea.l  (-56,%a6),%a2           | [1418]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1419]

dec_rol:
      tst.b    %d0                     | [1422 EQ]
      jbeq     begn_ror                | [1423]
|;;   subq.w    #1,d0                  | [1424]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jbra     new_shif                | [1425]
begn_ror:
      clr.w    %d0                     | [1427]
      jbra     ror_add                 | [1428]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1433]
      swap     %d2                     | [1434]
      move.w   %d2,(-38,%a6)           | [1435]
|;;   adda.w    -88(a6),a1             | [1436]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1437]

      moveq.l  #15,%d0                 | [1439]
|;;   sub.w     -42(a6),d0             | [1440]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1441]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1442]
      jbra     no_shift                | [1443]






skewopw:
|;;   rol.w     -80(a6)                | [1451 CC]
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
      jbcc     do_shift                | [1452]

      tst.w    (-42,%a6)               | [1454 MI]
      bmi      mlt_left                | [1455]
      jbra     mlt_rite                | [1456]
do_shift:
|;;   ori       #0x10,ccr              | [1458 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1459]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1461]
      cmp.w    #0xFFFF,%d0             | [1462 EQ]
      jbeq     inc_rite                | [1463]

|;;   ori       #0x10,ccr              | [1465 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1466]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1467]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1470]
      tst.b    %d0                     | [1471 NE]
      jbne     no_rota                 | [1472]
      move.w   #0x8000,(-40,%a6)       | [1473]
no_rota:
      move.w   (-38,%a6),%d1           | [1475]
|;;   lsr.w     #1,d1                  | [1476 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jbeq     wnxt_wrd                | [1477]

      move.w   %d1,(-38,%a6)           | [1479]

      tst.w    %d0                     | [1481 MI]
      bmi.b    wdec_rol                | [1482]
|;;   addq.w    #1,-42(a6)             | [1483]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jbra     mlt_rite                | [1484]
wdec_rol:
      tst.b    %d0                     | [1486 EQ]
      jbeq     set_msk                 | [1487]
|;;   subq.w    #1,-42(a6)             | [1488]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jbra     mlt_left                | [1489]
set_msk:
      move.w   #1,(-42,%a6)            | [1491]
      jbra     mlt_rite                | [1492]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1495]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1496]
      jbra     do_left                 | [1497]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1500]

|;;   subq.w    #1,-28(a6)             | [1502]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1503]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1505]
|;;   sub.w     d0,d2                  | [1506]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1507]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1508]

      jbra     mlt_left                | [1510]




rotation:
|***	SAVE	d4/d6-d7	***
	lea      -28(%a7),%a7
	movem.l  %d4,12(%a7)
	movem.l  %d6-%d7,20(%a7)
      move.w   _SOURCEX,%d1            | [1516]
      move.w   %d1,%d2                 | [1517]
|;;   andi.w    #0x0F,d2               | [1518]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1519]
|;;   lsr.w     #4,d1                  | [1520]
	mvz.w    %d1,%d4
	lsr.l    #4,%d4
      moveq.l  #1,%d5                  | [1521]
|***	SAVE	d5	***
	movem.l  %d5,16(%a7)
|;;   lsl.w     d5,d1                  | [1522]
	lsl.l    %d5,%d4

      movea.l  (-10,%a6),%a0           | [1524]
|;;   adda.w    d1,a0                  | [1525]
	ext.l    %d4
	adda.l   %d4,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1527]
      move.w   _SOURCEY,%d0            | [1528]
      move.w   (-106,%a6),%d1          | [1529]
      move.w   %d1,(-22,%a6)           | [1530]
      move.w   (-16,%a6),%d2           | [1531]

      tst.w    (-126,%a6)              | [1533 EQ LT]
      jbeq     upsd_dwn                | [1534]
rot90:
      jblt     top_src                 | [1536]
|;;   neg.w     -16(a6)                | [1537]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   subq.w    #1,d1                  | [1538]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   add.w     d1,d0                  | [1539]
	add.l    %d4,%d0
      mulu.w   %d2,%d0                 | [1540]
|;;   adda.w    d0,a0                  | [1541]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1543]
|;;   lsr.w     #4,d0                  | [1544]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1545]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1546]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1547]
      movea.l  _scrtchp,%a1            | [1548]
|;;   adda.w    -4(a6),a1              | [1549]
	mvs.w    (-4,%a6),%d4
	adda.l   %d4,%a1

      tst.w    (-126,%a6)              | [1551 GT]
      jbgt     top_dwn                 | [1552]
|;;   neg.w     -18(a6)                | [1553]
	move.w   (-18,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1554]
|;;   subq.w    #1,d1                  | [1555]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [1556]
	mulu.w   %d4,%d0
|;;   adda.w    d0,a1                  | [1557]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1559]
      move.w   #0x8000,%d3             | [1560]
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
      move.w   %d3,%d4                 | [1561]
      movea.w  %d3,%a4                 | [1562]
|;;   lsr.w     d2,d3                  | [1563]
	mvz.w    %d3,%d5
	lsr.l    %d2,%d5
      moveq.l  #0,%d0                  | [1564]
      move.w   (-20,%a6),%d2           | [1565]
|***	RESTORE	AFTER d6	***
	movem.l  20(%a7),%d6
      move.w   (-16,%a6),%d6           | [1566]
      movea.l  %a0,%a2                 | [1567]
      movea.l  %a1,%a3                 | [1568]
	move.w   %d5,%d3
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      jbra     rot_nsrc                | [1569]
rot_ylp:
|***	SAVE	d2	***
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1571]
|***	RESTORE	d3	***
	movem.l  4(%a7),%d3
      jbra     rot_srt                 | [1572]
rot_xlp:
      move.w   (%a0),%d7               | [1574]
|;;   and.w     d3,d7                  | [1575 EQ]
	move.w   %d7,%d2
	and.l    %d3,%d2
	tst.w    %d2
      jbeq     rot_nor                 | [1576]
|;;   or.w      d4,d0                  | [1577]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1579 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1578_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1578_1:         
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      jbcc     rot_isrc                | [1580]
rot_ndst:
      move.w   %d0,(%a1)+              | [1582]
      moveq.l  #0,%d0                  | [1583]
rot_isrc:
|;;   adda.w    d6,a0                  | [1585]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,8(%a7)
|;;   dbra      d1,rot_xlp             | [1587]
	tst.w    %d1
	jbeq     _PA_1586_
	subq.l   #1,%d1
	jbra     rot_xlp
	_PA_1586_:         

      move.w   %d0,(%a1)               | [1589]
      moveq.l  #0,%d0                  | [1590]
|;;   adda.w    -18(a6),a3             | [1591]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1592]
      move.w   %a4,%d4                 | [1593]
|;;   ror.w     d5,d3                  | [1594 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1593_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1593_1:         
      jbcc     rnew_src                | [1595]
      addq.l   #2,%a2                  | [1596]
rnew_src:
      movea.l  %a2,%a0                 | [1598]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2
	movem.l  8(%a7),%d6
	lea      12(%a7),%a7
rot_nsrc:
|***	SAVE	d3	***
	lea      -12(%a7),%a7
	movem.l  %d3,4(%a7)
|;;   dbra      d2,rot_ylp             | [1600]
	tst.w    %d2
	jbeq     _PA_1599_
	subq.l   #1,%d2
	jbra     rot_ylp
	_PA_1599_:         

rot_done:
      move.w   (-102,%a6),%d0          | [1603]
      move.w   (-106,%a6),%d1          | [1604]
      move.w   %d1,(-20,%a6)           | [1605]
      move.w   %d1,(-102,%a6)          | [1606]
      move.w   %d0,(-22,%a6)           | [1607]
      move.w   %d0,(-106,%a6)          | [1608]

      move.w   (-92,%a6),%d0           | [1610]
      move.w   (-90,%a6),(-92,%a6)     | [1611]
      move.w   %d0,(-90,%a6)           | [1612]
      move.w   #1,(-94,%a6)            | [1613]

      move.w   (-18,%a6),%d0           | [1615]
      tst.w    (-126,%a6)              | [1616 GT]
      jbgt     rot_nneg                | [1617]
|;;   neg.w     d0                     | [1618]
	move.w   %d0,%d3
	neg.l    %d3
	move.w   %d3,%d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1620]
      move.w   (-4,%a6),%d0            | [1621]
|***	RESTORE	AFTER d3	***
	movem.l  4(%a7),%d3
	lea      12(%a7),%a7
repexit:
      clr.w    _SOURCEX                | [1623]
repexit3:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      movea.l  _scrtchp,%a2            | [1625]
|;;   adda.w    d0,a2                  | [1626]
	mvs.w    %d0,%d1
	adda.l   %d1,%a2
      move.l   %a2,(-10,%a6)           | [1627]
      clr.w    _SOURCEY                | [1628]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1629]

upsd_dwn:
      move.w   (-102,%a6),%d0          | [1632]
|;;   add.w     -24(a6),d0             | [1633]
	move.w   (-24,%a6),%d4
	add.l    %d4,%d0
|;;   subq.w    #1,d0                  | [1634]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1635]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1636]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1637]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1639]
      move.w   %d0,%d2                 | [1640]
|;;   lsr.w     d5,d2                  | [1641]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1642]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1643]
|;;   add.w     -4(a6),d0              | [1644 X]
	move.w   %d0,%d6
	mvz.w    (-4,%a6),%d4
	ori.l    #0xffff0000,%d6
	add.l    %d4,%d6
      movea.l  _scrtchp,%a1            | [1645]
|;;   adda.w    d0,a1                  | [1646]
	ext.l    %d6
	adda.l   %d6,%a1
|***	RESTORE	d4/d6	***
	movem.l  12(%a7),%d4
	movem.l  20(%a7),%d6
      jbra     strtflip                | [1647]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1649]
      move.w   %d2,%d3                 | [1650]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1652]
      moveq.l  #0,%d6                  | [1653]
      moveq.l  #15,%d4                 | [1654]
flip_lp:
|***	SAVE	d4	***
	movem.l  %d4,12(%a7)
|;;   lsr.w     d5,d0                  | [1656 X]
	mvz.w    %d0,%d1
	lsr.l    %d5,%d1
|;;   roxl.w    d5,d6                  | [1657 X]
	move.w   %d1,%d0
	move.b   %d5,%d1
	move.w   %d6,%d2
	moveq    #16,%d4
	and.l    #0x1f,%d1
	jbeq     _PA_1656_
	move.w   %d2,%d3
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d3,%d2
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d2
	_PA_1656_:         
	addx.l   %d3,%d3
	swap     %d3
	move.w   %d2,%d3
	lsl.l    %d4,%d3
	move.w   %d2,%d6
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
|;;   dbra      d4,flip_lp             | [1658]
	move     %CCR,%d1
	tst.w    %d4
	jbeq     _PA_1657_
	subq.l   #1,%d4
	jbra     flip_lp
	_PA_1657_:         
	move     %d1,%CCR

|;;   move.w    d6,-(a1)               | [1660]
	move.w   %d2,-(%a1)
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1661]
	move     %CCR,%d1
	tst.w    %d3
	jbeq     _PA_1660_
	subq.l   #1,%d3
	move     %d1,%CCR
	jbra     line_lp
	_PA_1660_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1663]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1665]
	move     %CCR,%d7
	tst.w    %d1
	jbeq     _PA_1664_
	subq.l   #1,%d1
	move     %d7,%CCR
	jbra     upsd_lp
	_PA_1664_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1667]
      movea.l  _scrtchp,%a1            | [1668]
|;;   adda.w    -4(a6),a1              | [1669]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1670]
      move.w   _SOURCEX,%d0            | [1671]
|;;   add.w     -102(a6),d0            | [1672]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d5
	add.l    %d5,%d7
|;;   neg.w     d0                     | [1673]
	neg.l    %d7
|;;   andi.w    #0xF,d0                | [1674]
	andi.l   #(0xFFFF&0xF),%d7
|;;   move.w    d0,_SOURCEX            | [1675]
	move.w   %d7,_SOURCEX
      clr.w    _SOURCEY                | [1676]
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      rts                              | [1677]






replicat:
|***	SAVE	d6-d7	***
	lea      -16(%a7),%a7
	movem.l  %d6-%d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1685]
      move.w   %d0,%d4                 | [1686]
|;;   andi.w    #0x0F,d0               | [1687]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1688]
|;;   lsr.w     #4,d4                  | [1689]
	mvz.w    %d4,%d6
	lsr.l    #4,%d6
|;;   lsl.w     #1,d4                  | [1690]
	lsl.l    #1,%d6

      move.w   _SOURCEY,%d0            | [1692]
      mulu.w   (-16,%a6),%d0           | [1693]
|;;   add.w     d4,d0                  | [1694]
	move.w   %d0,%d7
	add.l    %d6,%d7
	move.w   %d7,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1695]
|;;   adda.w    d0,a0                  | [1696]
	mvs.w    %d0,%d3
	adda.l   %d3,%a0

      move.w   (-24,%a6),%d2           | [1698]
      move.w   #0x8000,%d3             | [1699]
      move.w   %d3,%d4                 | [1700]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1701]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1703]
      move.w   (-102,%a6),%d1          | [1704]

      move.w   %d2,(-22,%a6)           | [1706]
      move.w   %d1,(-20,%a6)           | [1707]
      movea.l  _scrtchp,%a1            | [1708]
|;;   adda.w    _scrpt2,a1             | [1709]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1711 EQ]
      jbeq     noline                  | [1712]



|;;   addq.w    #1,-106(a6)            | [1716]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1717]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1719]
	move.w   %d1,%d0
	lsl.l    #1,%d0
|;;   addq.w    #3,d1                  | [1720]
	addq.l   #3,%d0
|;;   lsr.w     #4,d1                  | [1721]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d1                  | [1722]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1723]
	addq.l   #2,%d0
|;;   move.w    d1,d5                  | [1724]
	move.w   %d0,%d5
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      moveq.l  #0,%d6                  | [1725]
	move.w   %d0,%d1
oklear:
      move.w   %d6,(%a1)+              | [1727]
|;;   dbra      d5,oklear              | [1728]
	tst.w    %d5
	jbeq     _PA_1727_
	subq.l   #1,%d5
	jbra     oklear
	_PA_1727_:         
      lea      (-2,%a1),%a2            | [1729]
      move.w   %d2,%d5                 | [1730]
|;;   lsl.w     #1,d5                  | [1731]
	move.w   %d5,%d0
	lsl.l    #1,%d0
	move.w   %d0,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1733]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1734]
|;;   dbra      d5,oklear1             | [1735]
	tst.w    %d5
	jbeq     _PA_1734_
	subq.l   #1,%d5
	jbra     oklear1
	_PA_1734_:         

|;;   move.w    d1,d5                  | [1737]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1738]
	mvz.w    %d5,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d5                  | [1739 X]
	subq.l   #1,%d0
	move.w   %d0,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1741]
|;;   dbra      d5,oklear3             | [1742]
	move     %CCR,%d0
	tst.w    %d5
	jbeq     _PA_1741_
	subq.l   #1,%d5
	move     %d0,%CCR
	jbra     oklear3
	_PA_1741_:         
	move     %d0,%CCR
      jbra     noline1                 | [1743]

noline:
|;;   lsr.w     #3,d1                  | [1746]
	mvz.w    %d1,%d0
	lsr.l    #3,%d0
|;;   lsl.w     #1,d1                  | [1747]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1748 X]
	ori.l    #0xffff0000,%d0
	addq.l   #2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
noline1:
      move.w   %d1,(-18,%a6)           | [1750]
|***	RESTORE	AFTER d7	***
	movem.l  12(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1754]
|;;   roxr.l    #1,d7                  | [1755 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1756]
|;;   roxr.l    #1,d7                  | [1757]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1759]
      move.w   _DDA_INC,%d2            | [1760]
      move.w   (-22,%a6),%d5           | [1761]
|;;   subq.w    #1,d5                  | [1762]
	move.w   %d5,%d0
	subq.l   #1,%d0
      move.w   #32767,%d6              | [1763]
	move.w   %d0,%d5
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      16(%a7),%a7
      tst.l    %d7                     | [1764 MI]
      bmi.b    rep_ylop                | [1765]
y_dwn_lp:
|***	SAVE	d0-d2	***
	lea      -12(%a7),%a7
	movem.l  %d0-%d2,(%a7)
|;;   add.w     d2,d6                  | [1767 CC]
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
      jbcc     y_no_drw                | [1768]
      jbsr     yloop                   | [1769]
y_no_drw:
|;;   adda.w    d7,a0                  | [1771]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  8(%a7),%d2
	lea      12(%a7),%a7
|;;   dbra      d5,y_dwn_lp            | [1772]
	tst.w    %d5
	jbeq     _PA_1771_
	subq.l   #1,%d5
	jbra     y_dwn_lp
	_PA_1771_:         
	move.w   #-1,%d5
      jbra     y_rep_don               | [1773]
rep_ylop:
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
|;;   add.w     d2,d6                  | [1775 CC]
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
      jbcc     y_no_rep                | [1776]
      jbsr     yloop                   | [1777]
y_no_rep:
      jbsr     yloop                   | [1779]
|;;   adda.w    d7,a0                  | [1780]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  4(%a7),%d2
	lea      8(%a7),%a7
|;;   dbra      d5,rep_ylop            | [1781]
	tst.w    %d5
	jbeq     _PA_1780_
	subq.l   #1,%d5
	jbra     rep_ylop
	_PA_1780_:         
	move.w   #-1,%d5
y_rep_don:
|***	SAVE	d4-d6	***
	lea      -12(%a7),%a7
	movem.l  %d4-%d6,(%a7)
      move.w   (-102,%a6),%d2          | [1783]
|;;   subq.w    #1,d2                  | [1784]
	subq.l   #1,%d2
      move.w   _XACC_DDA,%d1           | [1785]
      move.w   _DDA_INC,%d0            | [1786]
      moveq.l  #0,%d3                  | [1787]
repwidcl:
|;;   add.w     d0,d1                  | [1789 CC]
	mvz.w    %d0,%d4
	move.w   %d1,%d5
	ori.l    #0xffff0000,%d5
	add.l    %d4,%d5
	move     %ccr,%d6
	move.w   %d5,%d1
	move     %d6,%ccr
      jbcc     nrepdoub                | [1790]
|;;   addq.w    #1,d3                  | [1791]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrepdoub:
      tst.l    %d7                     | [1793 PL]
      bpl.b    nrpndoub                | [1794]
|;;   addq.w    #1,d3                  | [1795]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1797]
	tst.w    %d2
	jbeq     _PA_1796_
	subq.l   #1,%d2
	jbra     repwidcl
	_PA_1796_:         
      move.w   %d1,_XACC_DDA           | [1798]
      move.w   %d3,(-102,%a6)          | [1799]
      move.w   (-92,%a6),(-106,%a6)    | [1800]


      move.w   (-18,%a6),%d1           | [1803]
      move.w   %d1,(-16,%a6)           | [1804]







repexit1:
      btst     #4,(-96+1,%a6)          | [1813 EQ]
      jbeq     repexit2                | [1814]
      movea.l  _scrtchp,%a0            | [1815]
|;;   adda.w    _scrpt2,a0             | [1816]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1817]
|;;   lea       2(a0,d7.w),a1          | [1818]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
|***	RESTORE	AFTER d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbsr     outlin1                 | [1819]
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
      move.w   _scrpt2,%d0             | [1820]
|;;   add.w     -18(a6),d0             | [1821]
	move.w   %d0,%d2
	move.w   (-18,%a6),%d1
	add.l    %d1,%d2
|;;   move.w    #15,_SOURCEX           | [1822]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
	move.w   %d2,%d0
|***	RESTORE	d1-d2	***
	movem.l  (%a7),%d1-%d2
	lea      8(%a7),%a7
      jbra     repexit3                | [1823]
repexit2:
      move.w   _scrpt2,%d0             | [1825]
|***	RESTORE	d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbra     repexit                 | [1826]
| 2378 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1829]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1830]
      movea.l  %a1,%a4                 | [1831]
      movea.l  %d7,%a5                 | [1832]
      move.w   %d5,(-44,%a6)           | [1833]
      moveq.l  #0,%d0                  | [1834]
      move.w   (-20,%a6),%d5           | [1835]
|;;   subq.w    #1,d5                  | [1836]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1838]
      jbra     nextsrc                 | [1839]
innerlp:
|;;   ror.w     #1,d3                  | [1841 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jbcc     reploop                 | [1842]
nextsrc:
      movea.w  (%a0)+,%a2              | [1844]
reploop:
      move.w   %a2,%d6                 | [1846]
|;;   and.w     d3,d6                  | [1847 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jbne     nrepnor                 | [1848]


repnor:
      tst.l    %d7                     | [1852 MI]
      bmi.b    repnorup                | [1853]
|;;   add.w     d2,d7                  | [1854 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     incsrc                  | [1855]
      jbra     ordone                  | [1856]
repnorup:
|;;   add.w     d2,d7                  | [1858 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     ordone                  | [1859]


|;;   ror.w     #1,d4                  | [1862 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jbcc     ordone                  | [1865]
      move.w   %d0,(%a1)+              | [1866]
      moveq.l  #0,%d0                  | [1867]


      jbra     ordone                  | [1870]
nrepnor:


|;;   add.w     d2,d7                  | [1874 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     o_no_rep                | [1875]


|;;   or.w      d4,d0                  | [1878]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1879 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jbcc     o_no_rep                | [1881]
      move.w   %d0,(%a1)+              | [1882]
      moveq.l  #0,%d0                  | [1883]
o_no_rep:
      tst.l    %d7                     | [1885 PL]
      bpl.b    incsrc                  | [1886]


|;;   or.w      d4,d0                  | [1889]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1891 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jbcc     incsrc                  | [1892]
nextdst:
      move.w   %d0,(%a1)+              | [1894]
      moveq.l  #0,%d0                  | [1895]
incsrc:
|;;   dbra      d5,innerlp             | [1897]
	tst.w    %d5
	jbeq     _PA_1896_
	subq.l   #1,%d5
	jbra     innerlp
	_PA_1896_:         
repdone:
      move.w   %d0,(%a1)               | [1899]
|;;   movem.l   (sp)+,d3-d4/d6         | [1900]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1901]
      movea.l  %a4,%a1                 | [1902]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1903]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1904]
      move.w   (-44,%a6),%d5           | [1905]
      rts                              | [1906]
| 2466 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1909]
	move.w   %d7,%a5
	lea      (0,%a0,%a5.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1911]
	move.w   %d7,%a5
	lea      (0,%a1,%a5.l*1),%a2
|;;   lsr.w     #1,d7                  | [1912]
	mvz.w    %d7,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d7                  | [1913]
	subq.l   #1,%d0
|;;   move.w    d7,-44(a6)             | [1914]
	move.w   %d0,(-44,%a6)
      move.w   (-106,%a6),%d6          | [1915]

      movea.l  %a0,%a5                 | [1917]
|;;   subq.w    #1,d6                  | [1918]
	subq.l   #1,%d6
	move.w   %d0,%d7
out_edge:
      movea.w  %d6,%a4                 | [1920]
      movea.l  %a2,%a3                 | [1921]
      moveq.l  #0,%d5                  | [1922]
      moveq.l  #0,%d6                  | [1923]
      move.l   (%a2),%d1               | [1924]
      lsr.l    #1,%d1                  | [1925]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1930]
      move.b   %d5,%d0                 | [1931]
|;;   ror.l     #1,d0                  | [1932]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1934]
      move.b   %d6,%d2                 | [1935]
      move.l   %d2,%d3                 | [1936]
|;;   ror.l     #1,d3                  | [1937]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1938]
|;;   ror.l     #1,d4                  | [1939]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1941]
      move.l   %d0,%d6                 | [1942]
      eor.l    %d2,%d0                 | [1943]
      eor.l    %d3,%d5                 | [1944]
      eor.l    %d4,%d6                 | [1945]
|;;   rol.l     #1,d5                  | [1946]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1947]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1948]
      or.l     %d6,%d0                 | [1949]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1951]
      move.l   %d1,%d6                 | [1952]
      eor.l    %d2,%d1                 | [1953]
      eor.l    %d3,%d5                 | [1954]
      eor.l    %d4,%d6                 | [1955]
|;;   rol.l     #1,d5                  | [1956]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1957]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1958]
      or.l     %d5,%d0                 | [1959]
      or.l     %d6,%d0                 | [1960]

      eor.l    %d3,%d2                 | [1962]
      eor.l    %d3,%d4                 | [1963]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1964]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1965]
      or.l     %d4,%d0                 | [1966]
      swap     %d0                     | [1967]

      move.w   (%a1),%d6               | [1969]
      move.w   %d6,%d5                 | [1970]
|;;   eor.w     d0,d5                  | [1971]
	move.w   %d5,%d3
	eor.l    %d0,%d3
|;;   and.w     d0,d5                  | [1972]
	and.l    %d0,%d3

      addq.l   #2,%a2                  | [1974]
      move.l   (%a2),%d1               | [1975]
      move.b   (-1,%a2),%d1            | [1976]
|;;   ror.l     #1,d1                  | [1977]
	move.w   %d3,%d5
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1979]
      move.w   (%a0),%d5               | [1980]
      move.w   %d6,(%a0)+              | [1981]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1983]
	tst.w    %d7
	jbeq     _PA_1982_
	subq.l   #1,%d7
	jbra     out_loop
	_PA_1982_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1985]
      movea.l  %a5,%a0                 | [1986]
      move.w   %a4,%d6                 | [1987]
      movea.l  %a3,%a2                 | [1988]
      movea.l  %a2,%a1                 | [1989]
|;;   adda.w    -16(a6),a2             | [1990]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1991 NE]
      jbne     srt_lin                 | [1992]
      movea.l  %a1,%a2                 | [1993]
srt_lin:
|;;   dbra      d6,out_edge            | [1995]
	tst.w    %d6
	jbeq     _PA_1994_
	subq.l   #1,%d6
	jbra     out_edge
	_PA_1994_:         
	move.w   #-1,%d6
      rts                              | [1996]
| 2577 "vdi/vdi_tblit.S"
concat:
|;;   movem.l   d2/d3/a0,-(sp)         | [1999]
	lea      -12(%a7),%a7
	movem.l  %d2-%d3/%a0,(%a7)



      mulu.w   _v_lin_wr,%d1           | [2003]





      move.w   %d0,%d2                 | [2009]
|;;   andi.w    #0x000f,d0             | [2010]
	andi.l   #(0xFFFF0000!~0x000f),%d0
|;;   andi.w    #0xfff0,d2             | [2011]
	andi.l   #(0xFFFF&0xfff0),%d2
| 2599 "vdi/vdi_tblit.S"
      lea      _shift_offset,%a0       | [2013]
|;;   add.w     _v_planes,a0           | [2014]
	mvs.w    _v_planes,%d3
	adda.l   %d3,%a0
      move.b   (%a0),%d3               | [2015]
|;;   lsr.w     d3,d2                  | [2016]
	lsr.l    %d3,%d2






      ext.l    %d2                     | [2023]
      add.l    %d2,%d1                 | [2024]
|;;   movem.l   (sp)+,d2/d3/a0         | [2025]
	movem.l  (%a7),%d2-%d3/%a0
	lea      12(%a7),%a7
      rts                              | [2026]

| End section on aligned boundary
	.p2align 2,0
