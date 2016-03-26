
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
| Translated on Sun Mar 27 00:01:35 2016 by version 2.6.0 of PortAsm translator
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
| 522 "vdi/vdi_tblit.S"
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
| 595 "vdi/vdi_tblit.S"
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
| 925 "vdi/vdi_tblit.S"
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
| 1036 "vdi/vdi_tblit.S"
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
| 1068 "vdi/vdi_tblit.S"
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
| 1187 "vdi/vdi_tblit.S"
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
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [758]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [760]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [764]
|;;   lsr.w     -84(a6)                | [765 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [766]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [767 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [768]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [772]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [773]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [774]
      movea.l  %a2,%a5                 | [775]

|;;   movea.l   twoptble(pc,d0.w),a4   | [777]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [781 EQ]
      jbeq     wrt_char                | [782]

      btst     #1,%d0                  | [784 EQ]
      jbeq     no_lite                 | [785]



|;;   move.w    _LITEMASK,-78(a6)      | [789]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [790]
      lea      liteop,%a5              | [791]

      move.l   %a2,(-64,%a6)           | [793]
      lea      liteopwf,%a2            | [794]

      move.l   %a4,(-72,%a6)           | [796]
      lea      liteopw,%a4             | [797]

no_lite:
      btst     #0,%d0                  | [800 EQ]
      jbeq     no_thick                | [801]



      clr.w    (-82,%a6)               | [805]

      move.l   %a5,(-48,%a6)           | [807]
      lea      thknop,%a5              | [808]

      move.l   %a2,(-60,%a6)           | [810]
      lea      thknopwf,%a2            | [811]

      move.l   %a4,(-68,%a6)           | [813]
      lea      thknopw,%a4             | [814]

no_thick:
      btst     #2,%d0                  | [817 EQ]
      jbeq     wrt_char                | [818]



      move.w   (-100,%a6),(-80,%a6)    | [822]
      move.l   %a5,(-56,%a6)           | [823]
      lea      skewop,%a5              | [824]

      tst.w    (-124,%a6)              | [826 EQ GT]
      jbgt     wrt_char                | [827]
      jbeq     not_sngl                | [828]
      clr.w    (-28,%a6)               | [829]
      moveq.l  #0,%d7                  | [830]
      jbra     chng_blt                | [831]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [833 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      bls.b    wrt_char                | [834]

      tst.w    (-42,%a6)               | [836 LT]
      jblt     chng_left               | [837]
      moveq.l  #4,%d7                  | [838]
      jbra     chng_blt                | [839]
chng_left:
      moveq.l  #8,%d7                  | [841]
chng_blt:
      move.w   %d7,(-124,%a6)          | [843]

wrt_char:
      move.w   (-124,%a6),%d7          | [846 LE]
      jble     set_chr_hgt             | [847]
      movea.l  %a2,%a5                 | [848]
      move.w   (-18,%a6),%d5           | [849]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [851]
|;;   subq.w    #1,d3                  | [852]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [854]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [855]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [857 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jble     no_more_planes          | [858]
      movea.l  (-10,%a6),%a0           | [859]
      movea.l  (-14,%a6),%a1           | [860]
      addq.l   #2,%a1                  | [861]
      move.l   %a1,(-14,%a6)           | [862]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [863 EQ]
      jbeq     plane_loop              | [864]
      jbra     norm_blt                | [865]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [867]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [869]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [875]
      move.w   (-16,%a6),%d6           | [876]
      move.w   (-18,%a6),%d7           | [877]
      lea      sgl_rtn,%a3             | [878]
      jbra     sgl_1st_scan            | [879]

sgl_lp:
|;;   adda.w    d6,a0                  | [882]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [883]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [885]
| 1331 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [887]


      move.w   (-42,%a6),%d1           | [890 MI]
      bmi.b    left_rol                | [891]
      lsr.l    %d1,%d0                 | [892]
      jbra     end_ro                  | [893]
left_rol:
      lsl.l    %d1,%d0                 | [895]
end_ro:
      swap     %d0                     | [897]
      move.w   %d0,%d1                 | [898]
      swap     %d0                     | [899]

      jmp      (%a5)                   | [901]

sgl_rtn:
      move.w   %d1,(%a1)               | [904]
|;;   dbra      d3,sgl_lp              | [905]
	tst.w    %d3
	jbeq     _PA_904_
	subq.l   #1,%d3
	jbra     sgl_lp
	_PA_904_:         
      jbra     next_plane              | [906]




dbl_loop:
      movea.l  %a3,%a4                 | [912]
      move.w   (-16,%a6),%d6           | [913]
      move.w   (-18,%a6),%d7           | [914]
      lea      dbl_2wrd,%a2            | [915]
      move.l   %a2,%d5                 | [916]
      lea      dbl_1wrd,%a3            | [917]
      jbra     dbl_1st_scan            | [918]

dbl_lp:
|;;   adda.w    d6,a0                  | [921]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [922]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [923]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [925]
| 1378 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [927]


      move.w   (-42,%a6),%d1           | [930 MI]
      bmi.b    dbleft                  | [931]
      lsr.l    %d1,%d0                 | [932]
      jbra     dblendr                 | [933]
dbleft:
      lsl.l    %d1,%d0                 | [935]
dblendr:
      swap     %d0                     | [937]
      move.w   %d0,%d1                 | [938]
      swap     %d0                     | [939]
      move.w   (-38,%a6),%d2           | [940]

      jmp      (%a5)                   | [942]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [945]
      move.w   %d0,%d1                 | [946]

      movea.w  (-88,%a6),%a2           | [948]
|;;   move.w    (a1,a2.w),d4           | [949]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [951]

|;;   exg       a3,d5                  | [953]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [954]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [957]
|;;   move.w    d1,(a1,a2.w)           | [958]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [960]
	tst.w    %d3
	jbeq     _PA_959_
	subq.l   #1,%d3
	jbra     dbl_lp
	_PA_959_:         
      jbra     next_plane              | [961]





left_loop:
      movea.l  %d6,%a0                 | [968]
      movea.l  %d7,%a1                 | [969]
|;;   adda.w    -16(a6),a0             | [970]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [971]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [972]
|;;   rol.w     -78(a6)                | [973]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [975 NE]
      jbne     skewopw                 | [976]
mlt_left:
      move.l   %a0,%d6                 | [978]
      move.l   %a1,%d7                 | [979]
      move.w   (-28,%a6),(-44,%a6)     | [980]







      move.l   (%a0)+,%d0              | [988]


      move.w   (%a1),%d4               | [991]

      move.w   (-42,%a6),%d1           | [993]
      lsl.l    %d1,%d0                 | [994]

      swap     %d0                     | [996]
      move.w   %d0,%d1                 | [997]
      swap     %d0                     | [998]

      move.w   (-38,%a6),%d2           | [1000]

      lea      lft_lfrng,%a3           | [1002]
      jmp      (%a5)                   | [1003]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1006]
|;;   adda.w    -88(a6),a1             | [1007]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1009]
      lea      lft_wrds,%a3            | [1010]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1015]
|;;   and.w     d2,d4                  | [1016]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [1017]





      swap     %d0                     | [1023]
      move.w   %d1,%d0                 | [1024]
      swap     %d0                     | [1025]

      move.w   (-42,%a6),%d1           | [1027]
      lsl.l    %d1,%d0                 | [1028]

      swap     %d0                     | [1030]
      move.w   %d0,%d1                 | [1031]
      swap     %d0                     | [1032]

|;;   not.w     d2                     | [1034]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1035]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1036]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1037]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1039]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1041 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     lef_don                 | [1042]

      jmp      (%a4)                   | [1044]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1047]
|;;   adda.w    -88(a6),a1             | [1048]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_lef                | [1050]

lef_don:
      move.w   (-36,%a6),%d2           | [1053]

      lea      lft_rfrng,%a3           | [1055]
      jmp      (%a5)                   | [1056]

lft_rfrng:
      move.w   %d1,(%a1)               | [1059]

|;;   dbra      d3,left_loop           | [1061]
	tst.w    %d3
	jbeq     _PA_1060_
	subq.l   #1,%d3
	jbra     left_loop
	_PA_1060_:         
      jbra     next_plane              | [1062]





rite_loop:
      movea.l  %d6,%a0                 | [1069]
      movea.l  %d7,%a1                 | [1070]
|;;   adda.w    -16(a6),a0             | [1071]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [1072]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [1073]
|;;   rol.w     -78(a6)                | [1074]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [1076 NE]
      jbne     skewopw                 | [1077]
mlt_rite:
      move.l   %a0,%d6                 | [1079]
      move.l   %a1,%d7                 | [1080]
      move.w   (-28,%a6),(-44,%a6)     | [1081]
      move.w   (%a0)+,%d0              | [1082]



      swap     %d0                     | [1086]

      move.w   (%a1),%d4               | [1088]

      move.w   (-42,%a6),%d1           | [1090]
      lsr.l    %d1,%d0                 | [1091]
      swap     %d0                     | [1092]
      move.w   %d0,%d1                 | [1093]
      swap     %d0                     | [1094]

      move.w   (-38,%a6),%d2           | [1096]

      lea      rgt_lfrng,%a3           | [1098]
      jmp      (%a5)                   | [1099]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1102]
|;;   adda.w    -88(a6),a1             | [1103]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1105]
      lea      rgt_wrds,%a3            | [1106]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1111]
|;;   and.w     d2,d4                  | [1112]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [1113]
      move.w   (%a0)+,%d0              | [1114]



      swap     %d0                     | [1118]

      move.w   (-42,%a6),%d1           | [1120]
      lsr.l    %d1,%d0                 | [1121]
      swap     %d0                     | [1122]
      move.w   %d0,%d1                 | [1123]
      swap     %d0                     | [1124]

|;;   not.w     d2                     | [1126]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1127]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1128]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1129]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1131]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1133 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     rite_don                | [1134]

      jmp      (%a4)                   | [1136]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1139]
|;;   adda.w    -88(a6),a1             | [1140]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_rit                | [1142]

rite_don:
      move.w   (-36,%a6),%d2           | [1145]

      lea      rgt_rfrng,%a3           | [1147]
      jmp      (%a5)                   | [1148]

rgt_rfrng:
      move.w   %d1,(%a1)               | [1151]

|;;   dbra      d3,rite_loop           | [1153]
	tst.w    %d3
	jbeq     _PA_1152_
	subq.l   #1,%d3
	jbra     rite_loop
	_PA_1152_:         
      jbra     next_plane              | [1154]





top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1161]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [1162]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [1163]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [1164]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1165]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1168]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [1169]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [1170]
	not.l    %d0
|;;   and.w     d4,d1                  | [1171]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1172]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1175]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [1176]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1177]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1178]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1181]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [1182]
	not.l    %d0
|;;   and.w     d4,d1                  | [1183]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1184]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1187]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1188]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1189]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1192]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1193]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1194]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1197]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [1198]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1199]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1200]





twop0:
      moveq.l  #0,%d1                  | [1207]
      jmp      (%a3)                   | [1208]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [1211]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1212]

twop3:
      jmp      (%a3)                   | [1215]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1218]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [1219]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1220]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1223]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1224]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [1227]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1228]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1231]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [1232]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1233]
| 1704 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [1236]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [1237]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [1238]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [1239 EQ]
      jbeq     thk00                   | [1240]
|;;   ori.b     #0x10,ccr              | [1241 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [1242 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jbcc     thk01                   | [1243]
      move.w   #0x8000,%d6             | [1244]
thk01:
      move.w   %d6,(-34,%a6)           | [1246]
thk00:
      tst.w    (-28,%a6)               | [1248 MI]
      bmi.b    thk0                    | [1249]
      cmp.w    (-36,%a6),%d6           | [1250 CC]
      jbcc     thk0                    | [1251]
|;;   and.w     d6,d0                  | [1252]
	and.l    %d6,%d0
      jbra     thk1                    | [1253]
thk0:
      clr.w    %d0                     | [1255]
|;;   and.w     d6,d1                  | [1256]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [1258]
      move.w   %d2,%d5                 | [1259]
      swap     %d5                     | [1260]
      move.w   (-36,%a6),%d5           | [1261]
      lsl.l    %d6,%d5                 | [1262]

      swap     %d5                     | [1264]
      move.w   %d5,%d7                 | [1265]
      swap     %d5                     | [1266]

|;;   and.w     d7,d1                  | [1268]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [1269]
	and.l    %d5,%d0
|;;   swap      d1                     | [1270]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [1271]
|;;   subq.w    #1,d6                  | [1272]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [1274]
      lsr.l    #1,%d0                  | [1275]
      or.l     %d0,%d1                 | [1276]
|;;   dbra      d6,thkoplp             | [1277]
	tst.w    %d6
	jbeq     _PA_1276_
	subq.l   #1,%d6
	jbra     thkoplp
	_PA_1276_:         

      move.w   %d1,%d0                 | [1279]
      swap     %d1                     | [1280]
|;;   movem.l   (sp)+,d5-d7            | [1281]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [1282]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [1283]
| 1767 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [1286]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [1287 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk2                    | [1288]
|;;   and.w     -34(a6),d1             | [1289]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [1291]
      swap     %d1                     | [1292]
      clr.w    %d1                     | [1293]
      move.l   %d1,%d5                 | [1294]
|;;   subq.w    #1,d6                  | [1295]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [1297]
      or.l     %d5,%d1                 | [1298]
|;;   dbra      d6,thkopwlp            | [1299]
	tst.w    %d6
	jbeq     _PA_1298_
	subq.l   #1,%d6
	jbra     thkopwlp
	_PA_1298_:         

      move.w   %d1,%d5                 | [1301]
      swap     %d1                     | [1302]
|;;   or.w      -82(a6),d1             | [1303]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1304]
|;;   movem.l   (sp)+,d5-d6            | [1305]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [1306]
      jmp      (%a2)                   | [1307]
| 1805 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [1310]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [1311]
|;;   subq.w    #1,-30(a6)             | [1312 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      bmi.b    thk3                    | [1313]
      jbeq     thk4                    | [1314]

|;;   subq.w    #1,-30(a6)             | [1316 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk11                   | [1317]
|;;   and.w     d6,d1                  | [1318]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [1320]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jbra     thk7                    | [1321]

thk3:
      clr.w    %d1                     | [1324]
      jbra     thk5                    | [1325]

thk4:
|;;   and.w     d6,d1                  | [1328]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [1331 MI]
      bmi.b    thk6                    | [1332]
      move.w   (-28,%a6),%d5           | [1333]
|;;   addq.w    #2,d5                  | [1334]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [1335]
|;;   ori.b     #0x10,ccr              | [1336 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [1337 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jbcc     thk33                   | [1338]
      move.w   #0x8000,%d7             | [1339]
|;;   addq.w    #1,d5                  | [1340]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [1342 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jbne     thk34                   | [1343]
|;;   subq.w    #1,d5                  | [1344]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [1346 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [1347 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jbcc     thk31                   | [1348]
      move.w   #0x8000,%d6             | [1349]
thk31:
      cmp.w    %d6,%d7                 | [1351 CS]
      jbcs     thk35                   | [1352]
|;;   addq.w    #1,d5                  | [1353]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [1355]
      move.w   %d5,(-32,%a6)           | [1356]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [1358]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:

      move.w   (-74,%a6),%d6           | [1361]
      swap     %d1                     | [1362]
      clr.w    %d1                     | [1363]
      move.l   %d1,%d5                 | [1364]
|;;   subq.w    #1,d6                  | [1365]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [1367]
      or.l     %d5,%d1                 | [1368]
|;;   dbra      d6,thkopwl             | [1369]
	tst.w    %d6
	jbeq     _PA_1368_
	subq.l   #1,%d6
	jbra     thkopwl
	_PA_1368_:         

      move.w   %d1,%d5                 | [1371]
      swap     %d1                     | [1372]
|;;   or.w      -82(a6),d1             | [1373]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1374]
|;;   and.w     d2,d1                  | [1375]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [1376]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [1377]
      jmp      (%a2)                   | [1378]
| 1887 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [1381]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [1382]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [1383]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [1384]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1385]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1388]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [1389]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1390]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1393]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [1394]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1395]
| 1919 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [1398 CC]
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
      jbcc     no_shift                | [1399]

      swap     %d1                     | [1401]
      move.w   %d0,%d1                 | [1402]
      lsr.l    #1,%d1                  | [1403]

      swap     %d2                     | [1405]
      move.w   (-36,%a6),%d2           | [1406]
      lsr.l    #1,%d2                  | [1407]
      move.w   %d2,(-36,%a6)           | [1408]
      swap     %d2                     | [1409]
      move.w   %d2,(-38,%a6)           | [1410 EQ]
      jbeq     nxt_word                | [1411]
      move.w   (-42,%a6),%d0           | [1412 MI]
      bmi.b    dec_rol                 | [1413]
ror_add:
|;;   addq.w    #1,d0                  | [1415]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [1417]
      move.w   %d1,%d0                 | [1418]
      swap     %d1                     | [1419]
no_shift:
      movea.l  (-56,%a6),%a2           | [1421]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1422]


dec_rol:
      tst.b    %d0                     | [1426 EQ]
      jbeq     begn_ror                | [1427]
|;;   subq.w    #1,d0                  | [1428]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jbra     new_shif                | [1429]
begn_ror:
      clr.w    %d0                     | [1431]
      jbra     ror_add                 | [1432]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1437]
      swap     %d2                     | [1438]
      move.w   %d2,(-38,%a6)           | [1439]
|;;   adda.w    -88(a6),a1             | [1440]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1441]

      moveq.l  #15,%d0                 | [1443]
|;;   sub.w     -42(a6),d0             | [1444]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1445]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1446]
      jbra     no_shift                | [1447]






skewopw:
|;;   rol.w     -80(a6)                | [1455 CC]
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
      jbcc     do_shift                | [1456]

      tst.w    (-42,%a6)               | [1458 MI]
      bmi      mlt_left                | [1459]
      jbra     mlt_rite                | [1460]
do_shift:
|;;   ori       #0x10,ccr              | [1462 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1463]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1465]
      cmp.w    #0xFFFF,%d0             | [1466 EQ]
      jbeq     inc_rite                | [1467]

|;;   ori       #0x10,ccr              | [1469 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1470]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1471]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1474]
      tst.b    %d0                     | [1475 NE]
      jbne     no_rota                 | [1476]
      move.w   #0x8000,(-40,%a6)       | [1477]
no_rota:
      move.w   (-38,%a6),%d1           | [1479]
|;;   lsr.w     #1,d1                  | [1480 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jbeq     wnxt_wrd                | [1481]

      move.w   %d1,(-38,%a6)           | [1483]

      tst.w    %d0                     | [1485 MI]
      bmi.b    wdec_rol                | [1486]
|;;   addq.w    #1,-42(a6)             | [1487]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jbra     mlt_rite                | [1488]
wdec_rol:
      tst.b    %d0                     | [1490 EQ]
      jbeq     set_msk                 | [1491]
|;;   subq.w    #1,-42(a6)             | [1492]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jbra     mlt_left                | [1493]
set_msk:
      move.w   #1,(-42,%a6)            | [1495]
      jbra     mlt_rite                | [1496]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1499]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1500]
      jbra     do_left                 | [1501]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1504]

|;;   subq.w    #1,-28(a6)             | [1506]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1507]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1509]
|;;   sub.w     d0,d2                  | [1510]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1511]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1512]

      jbra     mlt_left                | [1514]




rotation:
|***	SAVE	d4/d6-d7	***
	lea      -28(%a7),%a7
	movem.l  %d4,12(%a7)
	movem.l  %d6-%d7,20(%a7)
      move.w   _SOURCEX,%d1            | [1520]
      move.w   %d1,%d2                 | [1521]
|;;   andi.w    #0x0F,d2               | [1522]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1523]
|;;   lsr.w     #4,d1                  | [1524]
	mvz.w    %d1,%d4
	lsr.l    #4,%d4
      moveq.l  #1,%d5                  | [1525]
|***	SAVE	d5	***
	movem.l  %d5,16(%a7)
|;;   lsl.w     d5,d1                  | [1526]
	lsl.l    %d5,%d4

      movea.l  (-10,%a6),%a0           | [1528]
|;;   adda.w    d1,a0                  | [1529]
	ext.l    %d4
	adda.l   %d4,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1531]
      move.w   _SOURCEY,%d0            | [1532]
      move.w   (-106,%a6),%d1          | [1533]
      move.w   %d1,(-22,%a6)           | [1534]
      move.w   (-16,%a6),%d2           | [1535]

      tst.w    (-126,%a6)              | [1537 EQ LT]
      jbeq     upsd_dwn                | [1538]
rot90:
      jblt     top_src                 | [1540]
|;;   neg.w     -16(a6)                | [1541]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   subq.w    #1,d1                  | [1542]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   add.w     d1,d0                  | [1543]
	add.l    %d4,%d0
      mulu.w   %d2,%d0                 | [1544]
|;;   adda.w    d0,a0                  | [1545]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1547]
|;;   lsr.w     #4,d0                  | [1548]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1549]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1550]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1551]
      movea.l  _scrtchp,%a1            | [1552]
|;;   adda.w    -4(a6),a1              | [1553]
	mvs.w    (-4,%a6),%d4
	adda.l   %d4,%a1

      tst.w    (-126,%a6)              | [1555 GT]
      jbgt     top_dwn                 | [1556]
|;;   neg.w     -18(a6)                | [1557]
	move.w   (-18,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1558]
|;;   subq.w    #1,d1                  | [1559]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [1560]
	mulu.w   %d4,%d0
|;;   adda.w    d0,a1                  | [1561]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1563]
      move.w   #0x8000,%d3             | [1564]
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
      move.w   %d3,%d4                 | [1565]
      movea.w  %d3,%a4                 | [1566]
|;;   lsr.w     d2,d3                  | [1567]
	mvz.w    %d3,%d5
	lsr.l    %d2,%d5
      moveq.l  #0,%d0                  | [1568]
      move.w   (-20,%a6),%d2           | [1569]
|***	RESTORE	AFTER d6	***
	movem.l  20(%a7),%d6
      move.w   (-16,%a6),%d6           | [1570]
      movea.l  %a0,%a2                 | [1571]
      movea.l  %a1,%a3                 | [1572]
	move.w   %d5,%d3
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      jbra     rot_nsrc                | [1573]
rot_ylp:
|***	SAVE	d2	***
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1575]
|***	RESTORE	d3	***
	movem.l  4(%a7),%d3
      jbra     rot_srt                 | [1576]
rot_xlp:
      move.w   (%a0),%d7               | [1578]
|;;   and.w     d3,d7                  | [1579 EQ]
	move.w   %d7,%d2
	and.l    %d3,%d2
	tst.w    %d2
      jbeq     rot_nor                 | [1580]
|;;   or.w      d4,d0                  | [1581]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1583 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1582_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1582_1:         
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      jbcc     rot_isrc                | [1584]
rot_ndst:
      move.w   %d0,(%a1)+              | [1586]
      moveq.l  #0,%d0                  | [1587]
rot_isrc:
|;;   adda.w    d6,a0                  | [1589]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,8(%a7)
|;;   dbra      d1,rot_xlp             | [1591]
	tst.w    %d1
	jbeq     _PA_1590_
	subq.l   #1,%d1
	jbra     rot_xlp
	_PA_1590_:         

      move.w   %d0,(%a1)               | [1593]
      moveq.l  #0,%d0                  | [1594]
|;;   adda.w    -18(a6),a3             | [1595]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1596]
      move.w   %a4,%d4                 | [1597]
|;;   ror.w     d5,d3                  | [1598 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1597_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1597_1:         
      jbcc     rnew_src                | [1599]
      addq.l   #2,%a2                  | [1600]
rnew_src:
      movea.l  %a2,%a0                 | [1602]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2
	movem.l  8(%a7),%d6
	lea      12(%a7),%a7
rot_nsrc:
|***	SAVE	d3	***
	lea      -12(%a7),%a7
	movem.l  %d3,4(%a7)
|;;   dbra      d2,rot_ylp             | [1604]
	tst.w    %d2
	jbeq     _PA_1603_
	subq.l   #1,%d2
	jbra     rot_ylp
	_PA_1603_:         


rot_done:
      move.w   (-102,%a6),%d0          | [1608]
      move.w   (-106,%a6),%d1          | [1609]
      move.w   %d1,(-20,%a6)           | [1610]
      move.w   %d1,(-102,%a6)          | [1611]
      move.w   %d0,(-22,%a6)           | [1612]
      move.w   %d0,(-106,%a6)          | [1613]

      move.w   (-92,%a6),%d0           | [1615]
      move.w   (-90,%a6),(-92,%a6)     | [1616]
      move.w   %d0,(-90,%a6)           | [1617]
      move.w   #1,(-94,%a6)            | [1618]

      move.w   (-18,%a6),%d0           | [1620]
      tst.w    (-126,%a6)              | [1621 GT]
      jbgt     rot_nneg                | [1622]
|;;   neg.w     d0                     | [1623]
	move.w   %d0,%d3
	neg.l    %d3
	move.w   %d3,%d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1625]
      move.w   (-4,%a6),%d0            | [1626]
|***	RESTORE	AFTER d3	***
	movem.l  4(%a7),%d3
	lea      12(%a7),%a7
repexit:
      clr.w    _SOURCEX                | [1628]
repexit3:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      movea.l  _scrtchp,%a2            | [1630]
|;;   adda.w    d0,a2                  | [1631]
	mvs.w    %d0,%d1
	adda.l   %d1,%a2
      move.l   %a2,(-10,%a6)           | [1632]
      clr.w    _SOURCEY                | [1633]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1634]


upsd_dwn:
      move.w   (-102,%a6),%d0          | [1638]
|;;   add.w     -24(a6),d0             | [1639]
	move.w   (-24,%a6),%d4
	add.l    %d4,%d0
|;;   subq.w    #1,d0                  | [1640]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1641]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1642]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1643]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1645]
      move.w   %d0,%d2                 | [1646]
|;;   lsr.w     d5,d2                  | [1647]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1648]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1649]
|;;   add.w     -4(a6),d0              | [1650 X]
	move.w   %d0,%d6
	mvz.w    (-4,%a6),%d4
	ori.l    #0xffff0000,%d6
	add.l    %d4,%d6
      movea.l  _scrtchp,%a1            | [1651]
|;;   adda.w    d0,a1                  | [1652]
	ext.l    %d6
	adda.l   %d6,%a1
|***	RESTORE	d4/d6	***
	movem.l  12(%a7),%d4
	movem.l  20(%a7),%d6
      jbra     strtflip                | [1653]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1655]
      move.w   %d2,%d3                 | [1656]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1658]
      moveq.l  #0,%d6                  | [1659]
      moveq.l  #15,%d4                 | [1660]
flip_lp:
|***	SAVE	d4	***
	movem.l  %d4,12(%a7)
|;;   lsr.w     d5,d0                  | [1662 X]
	mvz.w    %d0,%d1
	lsr.l    %d5,%d1
|;;   roxl.w    d5,d6                  | [1663 X]
	move.w   %d1,%d0
	move.b   %d5,%d1
	move.w   %d6,%d2
	moveq    #16,%d4
	and.l    #0x1f,%d1
	jbeq     _PA_1662_
	move.w   %d2,%d3
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d3,%d2
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d2
	_PA_1662_:         
	addx.l   %d3,%d3
	swap     %d3
	move.w   %d2,%d3
	lsl.l    %d4,%d3
	move.w   %d2,%d6
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
|;;   dbra      d4,flip_lp             | [1664]
	move     %CCR,%d1
	tst.w    %d4
	jbeq     _PA_1663_
	subq.l   #1,%d4
	jbra     flip_lp
	_PA_1663_:         
	move     %d1,%CCR

|;;   move.w    d6,-(a1)               | [1666]
	move.w   %d2,-(%a1)
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1667]
	move     %CCR,%d1
	tst.w    %d3
	jbeq     _PA_1666_
	subq.l   #1,%d3
	move     %d1,%CCR
	jbra     line_lp
	_PA_1666_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1669]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1671]
	move     %CCR,%d7
	tst.w    %d1
	jbeq     _PA_1670_
	subq.l   #1,%d1
	move     %d7,%CCR
	jbra     upsd_lp
	_PA_1670_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1673]
      movea.l  _scrtchp,%a1            | [1674]
|;;   adda.w    -4(a6),a1              | [1675]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1676]
      move.w   _SOURCEX,%d0            | [1677]
|;;   add.w     -102(a6),d0            | [1678]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d5
	add.l    %d5,%d7
|;;   neg.w     d0                     | [1679]
	neg.l    %d7
|;;   andi.w    #0xF,d0                | [1680]
	andi.l   #(0xFFFF&0xF),%d7
|;;   move.w    d0,_SOURCEX            | [1681]
	move.w   %d7,_SOURCEX
      clr.w    _SOURCEY                | [1682]
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      rts                              | [1683]






replicat:
|***	SAVE	d6-d7	***
	lea      -16(%a7),%a7
	movem.l  %d6-%d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1691]
      move.w   %d0,%d4                 | [1692]
|;;   andi.w    #0x0F,d0               | [1693]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1694]
|;;   lsr.w     #4,d4                  | [1695]
	mvz.w    %d4,%d6
	lsr.l    #4,%d6
|;;   lsl.w     #1,d4                  | [1696]
	lsl.l    #1,%d6

      move.w   _SOURCEY,%d0            | [1698]
      mulu.w   (-16,%a6),%d0           | [1699]
|;;   add.w     d4,d0                  | [1700]
	move.w   %d0,%d7
	add.l    %d6,%d7
	move.w   %d7,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1701]
|;;   adda.w    d0,a0                  | [1702]
	mvs.w    %d0,%d3
	adda.l   %d3,%a0

      move.w   (-24,%a6),%d2           | [1704]
      move.w   #0x8000,%d3             | [1705]
      move.w   %d3,%d4                 | [1706]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1707]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1709]
      move.w   (-102,%a6),%d1          | [1710]

      move.w   %d2,(-22,%a6)           | [1712]
      move.w   %d1,(-20,%a6)           | [1713]
      movea.l  _scrtchp,%a1            | [1714]
|;;   adda.w    _scrpt2,a1             | [1715]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1717 EQ]
      jbeq     noline                  | [1718]



|;;   addq.w    #1,-106(a6)            | [1722]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1723]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1725]
	move.w   %d1,%d0
	lsl.l    #1,%d0
|;;   addq.w    #3,d1                  | [1726]
	addq.l   #3,%d0
|;;   lsr.w     #4,d1                  | [1727]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d1                  | [1728]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1729]
	addq.l   #2,%d0
|;;   move.w    d1,d5                  | [1730]
	move.w   %d0,%d5
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      moveq.l  #0,%d6                  | [1731]
	move.w   %d0,%d1
oklear:
      move.w   %d6,(%a1)+              | [1733]
|;;   dbra      d5,oklear              | [1734]
	tst.w    %d5
	jbeq     _PA_1733_
	subq.l   #1,%d5
	jbra     oklear
	_PA_1733_:         
      lea      (-2,%a1),%a2            | [1735]
      move.w   %d2,%d5                 | [1736]
|;;   lsl.w     #1,d5                  | [1737]
	move.w   %d5,%d0
	lsl.l    #1,%d0
	move.w   %d0,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1739]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1740]
|;;   dbra      d5,oklear1             | [1741]
	tst.w    %d5
	jbeq     _PA_1740_
	subq.l   #1,%d5
	jbra     oklear1
	_PA_1740_:         

|;;   move.w    d1,d5                  | [1743]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1744]
	mvz.w    %d5,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d5                  | [1745 X]
	subq.l   #1,%d0
	move.w   %d0,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1747]
|;;   dbra      d5,oklear3             | [1748]
	move     %CCR,%d0
	tst.w    %d5
	jbeq     _PA_1747_
	subq.l   #1,%d5
	move     %d0,%CCR
	jbra     oklear3
	_PA_1747_:         
	move     %d0,%CCR
      jbra     noline1                 | [1749]

noline:
|;;   lsr.w     #3,d1                  | [1752]
	mvz.w    %d1,%d0
	lsr.l    #3,%d0
|;;   lsl.w     #1,d1                  | [1753]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1754 X]
	ori.l    #0xffff0000,%d0
	addq.l   #2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
noline1:
      move.w   %d1,(-18,%a6)           | [1756]
|***	RESTORE	AFTER d7	***
	movem.l  12(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1760]
|;;   roxr.l    #1,d7                  | [1761 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1762]
|;;   roxr.l    #1,d7                  | [1763]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1765]
      move.w   _DDA_INC,%d2            | [1766]
      move.w   (-22,%a6),%d5           | [1767]
|;;   subq.w    #1,d5                  | [1768]
	move.w   %d5,%d0
	subq.l   #1,%d0
      move.w   #32767,%d6              | [1769]
	move.w   %d0,%d5
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      16(%a7),%a7
      tst.l    %d7                     | [1770 MI]
      bmi.b    rep_ylop                | [1771]
y_dwn_lp:
|***	SAVE	d0-d2	***
	lea      -12(%a7),%a7
	movem.l  %d0-%d2,(%a7)
|;;   add.w     d2,d6                  | [1773 CC]
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
      jbcc     y_no_drw                | [1774]
      jbsr     yloop                   | [1775]
y_no_drw:
|;;   adda.w    d7,a0                  | [1777]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  8(%a7),%d2
	lea      12(%a7),%a7
|;;   dbra      d5,y_dwn_lp            | [1778]
	tst.w    %d5
	jbeq     _PA_1777_
	subq.l   #1,%d5
	jbra     y_dwn_lp
	_PA_1777_:         
	move.w   #-1,%d5
      jbra     y_rep_don               | [1779]
rep_ylop:
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
|;;   add.w     d2,d6                  | [1781 CC]
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
      jbcc     y_no_rep                | [1782]
      jbsr     yloop                   | [1783]
y_no_rep:
      jbsr     yloop                   | [1785]
|;;   adda.w    d7,a0                  | [1786]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  4(%a7),%d2
	lea      8(%a7),%a7
|;;   dbra      d5,rep_ylop            | [1787]
	tst.w    %d5
	jbeq     _PA_1786_
	subq.l   #1,%d5
	jbra     rep_ylop
	_PA_1786_:         
	move.w   #-1,%d5
y_rep_don:
|***	SAVE	d4-d6	***
	lea      -12(%a7),%a7
	movem.l  %d4-%d6,(%a7)
      move.w   (-102,%a6),%d2          | [1789]
|;;   subq.w    #1,d2                  | [1790]
	subq.l   #1,%d2
      move.w   _XACC_DDA,%d1           | [1791]
      move.w   _DDA_INC,%d0            | [1792]
      moveq.l  #0,%d3                  | [1793]
repwidcl:
|;;   add.w     d0,d1                  | [1795 CC]
	mvz.w    %d0,%d4
	move.w   %d1,%d5
	ori.l    #0xffff0000,%d5
	add.l    %d4,%d5
	move     %ccr,%d6
	move.w   %d5,%d1
	move     %d6,%ccr
      jbcc     nrepdoub                | [1796]
|;;   addq.w    #1,d3                  | [1797]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrepdoub:
      tst.l    %d7                     | [1799 PL]
      bpl.b    nrpndoub                | [1800]
|;;   addq.w    #1,d3                  | [1801]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1803]
	tst.w    %d2
	jbeq     _PA_1802_
	subq.l   #1,%d2
	jbra     repwidcl
	_PA_1802_:         
      move.w   %d1,_XACC_DDA           | [1804]
      move.w   %d3,(-102,%a6)          | [1805]
      move.w   (-92,%a6),(-106,%a6)    | [1806]


      move.w   (-18,%a6),%d1           | [1809]
      move.w   %d1,(-16,%a6)           | [1810]







repexit1:
      btst     #4,(-96+1,%a6)          | [1819 EQ]
      jbeq     repexit2                | [1820]
      movea.l  _scrtchp,%a0            | [1821]
|;;   adda.w    _scrpt2,a0             | [1822]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1823]
|;;   lea       2(a0,d7.w),a1          | [1824]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
|***	RESTORE	AFTER d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbsr     outlin1                 | [1825]
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
      move.w   _scrpt2,%d0             | [1826]
|;;   add.w     -18(a6),d0             | [1827]
	move.w   %d0,%d2
	move.w   (-18,%a6),%d1
	add.l    %d1,%d2
|;;   move.w    #15,_SOURCEX           | [1828]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
	move.w   %d2,%d0
|***	RESTORE	d1-d2	***
	movem.l  (%a7),%d1-%d2
	lea      8(%a7),%a7
      jbra     repexit3                | [1829]
repexit2:
      move.w   _scrpt2,%d0             | [1831]
|***	RESTORE	d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbra     repexit                 | [1832]
| 2385 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1835]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1836]
      movea.l  %a1,%a4                 | [1837]
      movea.l  %d7,%a5                 | [1838]
      move.w   %d5,(-44,%a6)           | [1839]
      moveq.l  #0,%d0                  | [1840]
      move.w   (-20,%a6),%d5           | [1841]
|;;   subq.w    #1,d5                  | [1842]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1844]
      jbra     nextsrc                 | [1845]
innerlp:
|;;   ror.w     #1,d3                  | [1847 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jbcc     reploop                 | [1848]
nextsrc:
      movea.w  (%a0)+,%a2              | [1850]
reploop:
      move.w   %a2,%d6                 | [1852]
|;;   and.w     d3,d6                  | [1853 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jbne     nrepnor                 | [1854]


repnor:
      tst.l    %d7                     | [1858 MI]
      bmi.b    repnorup                | [1859]
|;;   add.w     d2,d7                  | [1860 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     incsrc                  | [1861]
      jbra     ordone                  | [1862]
repnorup:
|;;   add.w     d2,d7                  | [1864 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     ordone                  | [1865]


|;;   ror.w     #1,d4                  | [1868 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jbcc     ordone                  | [1871]
      move.w   %d0,(%a1)+              | [1872]
      moveq.l  #0,%d0                  | [1873]


      jbra     ordone                  | [1876]
nrepnor:


|;;   add.w     d2,d7                  | [1880 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     o_no_rep                | [1881]


|;;   or.w      d4,d0                  | [1884]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1885 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jbcc     o_no_rep                | [1887]
      move.w   %d0,(%a1)+              | [1888]
      moveq.l  #0,%d0                  | [1889]
o_no_rep:
      tst.l    %d7                     | [1891 PL]
      bpl.b    incsrc                  | [1892]


|;;   or.w      d4,d0                  | [1895]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1897 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jbcc     incsrc                  | [1898]
nextdst:
      move.w   %d0,(%a1)+              | [1900]
      moveq.l  #0,%d0                  | [1901]
incsrc:
|;;   dbra      d5,innerlp             | [1903]
	tst.w    %d5
	jbeq     _PA_1902_
	subq.l   #1,%d5
	jbra     innerlp
	_PA_1902_:         
repdone:
      move.w   %d0,(%a1)               | [1905]
|;;   movem.l   (sp)+,d3-d4/d6         | [1906]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1907]
      movea.l  %a4,%a1                 | [1908]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1909]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1910]
      move.w   (-44,%a6),%d5           | [1911]
      rts                              | [1912]
| 2473 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1915]
	move.w   %d7,%a5
	lea      (0,%a0,%a5.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1917]
	move.w   %d7,%a5
	lea      (0,%a1,%a5.l*1),%a2
|;;   lsr.w     #1,d7                  | [1918]
	mvz.w    %d7,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d7                  | [1919]
	subq.l   #1,%d0
|;;   move.w    d7,-44(a6)             | [1920]
	move.w   %d0,(-44,%a6)
      move.w   (-106,%a6),%d6          | [1921]

      movea.l  %a0,%a5                 | [1923]
|;;   subq.w    #1,d6                  | [1924]
	subq.l   #1,%d6
	move.w   %d0,%d7
out_edge:
      movea.w  %d6,%a4                 | [1926]
      movea.l  %a2,%a3                 | [1927]
      moveq.l  #0,%d5                  | [1928]
      moveq.l  #0,%d6                  | [1929]
      move.l   (%a2),%d1               | [1930]
      lsr.l    #1,%d1                  | [1931]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1936]
      move.b   %d5,%d0                 | [1937]
|;;   ror.l     #1,d0                  | [1938]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1940]
      move.b   %d6,%d2                 | [1941]
      move.l   %d2,%d3                 | [1942]
|;;   ror.l     #1,d3                  | [1943]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1944]
|;;   ror.l     #1,d4                  | [1945]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1947]
      move.l   %d0,%d6                 | [1948]
      eor.l    %d2,%d0                 | [1949]
      eor.l    %d3,%d5                 | [1950]
      eor.l    %d4,%d6                 | [1951]
|;;   rol.l     #1,d5                  | [1952]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1953]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1954]
      or.l     %d6,%d0                 | [1955]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1957]
      move.l   %d1,%d6                 | [1958]
      eor.l    %d2,%d1                 | [1959]
      eor.l    %d3,%d5                 | [1960]
      eor.l    %d4,%d6                 | [1961]
|;;   rol.l     #1,d5                  | [1962]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1963]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1964]
      or.l     %d5,%d0                 | [1965]
      or.l     %d6,%d0                 | [1966]

      eor.l    %d3,%d2                 | [1968]
      eor.l    %d3,%d4                 | [1969]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1970]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1971]
      or.l     %d4,%d0                 | [1972]
      swap     %d0                     | [1973]

      move.w   (%a1),%d6               | [1975]
      move.w   %d6,%d5                 | [1976]
|;;   eor.w     d0,d5                  | [1977]
	move.w   %d5,%d3
	eor.l    %d0,%d3
|;;   and.w     d0,d5                  | [1978]
	and.l    %d0,%d3

      addq.l   #2,%a2                  | [1980]
      move.l   (%a2),%d1               | [1981]
      move.b   (-1,%a2),%d1            | [1982]
|;;   ror.l     #1,d1                  | [1983]
	move.w   %d3,%d5
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [1985]
      move.w   (%a0),%d5               | [1986]
      move.w   %d6,(%a0)+              | [1987]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [1989]
	tst.w    %d7
	jbeq     _PA_1988_
	subq.l   #1,%d7
	jbra     out_loop
	_PA_1988_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [1991]
      movea.l  %a5,%a0                 | [1992]
      move.w   %a4,%d6                 | [1993]
      movea.l  %a3,%a2                 | [1994]
      movea.l  %a2,%a1                 | [1995]
|;;   adda.w    -16(a6),a2             | [1996]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [1997 NE]
      jbne     srt_lin                 | [1998]
      movea.l  %a1,%a2                 | [1999]
srt_lin:
|;;   dbra      d6,out_edge            | [2001]
	tst.w    %d6
	jbeq     _PA_2000_
	subq.l   #1,%d6
	jbra     out_edge
	_PA_2000_:         
	move.w   #-1,%d6
      rts                              | [2002]
| 2584 "vdi/vdi_tblit.S"
concat:
|;;   movem.l   d2/d3/a0,-(sp)         | [2005]
	lea      -12(%a7),%a7
	movem.l  %d2-%d3/%a0,(%a7)



      mulu.w   _v_lin_wr,%d1           | [2009]





      move.w   %d0,%d2                 | [2015]
|;;   andi.w    #0x000f,d0             | [2016]
	andi.l   #(0xFFFF0000!~0x000f),%d0
|;;   andi.w    #0xfff0,d2             | [2017]
	andi.l   #(0xFFFF&0xfff0),%d2
| 2606 "vdi/vdi_tblit.S"
      lea      _shift_offset,%a0       | [2019]
|;;   add.w     _v_planes,a0           | [2020]
	mvs.w    _v_planes,%d3
	adda.l   %d3,%a0
      move.b   (%a0),%d3               | [2021]
|;;   lsr.w     d3,d2                  | [2022]
	lsr.l    %d3,%d2






      ext.l    %d2                     | [2029]
      add.l    %d2,%d1                 | [2030]
|;;   movem.l   (sp)+,d2/d3/a0         | [2031]
	movem.l  (%a7),%d2-%d3/%a0
	lea      12(%a7),%a7
      rts                              | [2032]

| End section on aligned boundary
	.p2align 2,0
