
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
| Translated on Mon Sep  5 01:10:46 2011 by version 2.6.0 of PortAsm translator
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
| 1 "vdi/vdi_asm.h" 1
| 15 "vdi/vdi_tblit.S" 2
| 41 "vdi/vdi_tblit.S"
| 58 "vdi/vdi_tblit.S"
| 157 "vdi/vdi_tblit.S"
|;;           .globl _text_blt         | [13]
	.globl	_text_blt
|;;           .globl concat            | [14]
	.globl	concat





|;;           .extern _act_siz         | [20]
	.extern	_act_siz





|;;           .extern _SOURCEX,_SOURCEY | [26]
	.extern	_SOURCEX
	.extern	_SOURCEY
|;;           .extern _DESTX,_DESTY    | [27]
	.extern	_DESTX
	.extern	_DESTY
|;;           .extern _DELX,_DELY      | [28]
	.extern	_DELX
	.extern	_DELY
|;;           .extern _FBASE           | [29]
	.extern	_FBASE
|;;           .extern _FWIDTH          | [30]
	.extern	_FWIDTH
|;;           .extern _WRT_MODE        | [31]
	.extern	_WRT_MODE
|;;           .extern _STYLE           | [32]
	.extern	_STYLE
|;;           .extern _R_OFF,_L_OFF    | [33]
	.extern	_R_OFF
	.extern	_L_OFF
|;;           .extern _WEIGHT,_LITEMASK,_SKEWMASK | [34]
	.extern	_WEIGHT
	.extern	_LITEMASK
	.extern	_SKEWMASK
|;;           .extern _DOUBLE          | [35]
	.extern	_DOUBLE
|;;           .extern _CHUP            | [36]
	.extern	_CHUP
|;;           .extern _CLIP            | [37]
	.extern	_CLIP
|;;           .extern _MONO_STATUS     | [38]
	.extern	_MONO_STATUS
|;;           .extern _T_SCLSTS        | [39]
	.extern	_T_SCLSTS
|;;           .extern _TEXT_FG         | [40]
	.extern	_TEXT_FG
|;;           .extern _XMN_CLIP,_XMX_CLIP | [41]
	.extern	_XMN_CLIP
	.extern	_XMX_CLIP
|;;           .extern _YMN_CLIP,_YMX_CLIP | [42]
	.extern	_YMN_CLIP
	.extern	_YMX_CLIP
|;;           .extern _XACC_DDA        | [43]
	.extern	_XACC_DDA
|;;           .extern _DDA_INC         | [44]
	.extern	_DDA_INC
|;;           .extern _scrpt2          | [45]
	.extern	_scrpt2
|;;           .extern _scrtchp         | [46]
	.extern	_scrtchp
|;;           .extern _shft_off        | [47]
	.extern	_shft_off






|;;           .extern _v_bas_ad        | [54]
	.extern	_v_bas_ad
|;;           .extern _v_lin_wr        | [55]
	.extern	_v_lin_wr
|;;           .extern _v_planes        | [56]
	.extern	_v_planes





|;;           .globl _deftxbuf         | [62]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [63]
	.globl	_scrtsiz

| 326 "vdi/vdi_tblit.S"
| 343 "vdi/vdi_tblit.S"
|;;           .bss                     | [69]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [71]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [73]
	.p2align 2,0
	.text



_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [78]
	.short	(2+6+8+15)/16*2*16*2
| 459 "vdi/vdi_tblit.S"
|;;           .text                    | [80]




	.p2align 1,0
_text_blt:
      link.w   %a6,#(-126)             | [86]
|;;   movem.l   d2-d7/a2-a5,-(sp)      | [87]
	lea      -40(%a7),%a7
	movem.l  %d2-%d7/%a2-%a5,(%a7)

      clr.w    (-94,%a6)               | [89]
      move.w   #0,(-6,%a6)             | [90]
      lea      (-96,%a6),%a1           | [91]
      move.w   _STYLE,(%a1)            | [92]
      move.w   _WRT_MODE,-(%a1)        | [93]
      move.w   _SKEWMASK,-(%a1)        | [94]

      move.w   _DELX,-(%a1)            | [96]
      move.w   _DESTX,-(%a1)           | [97]
      move.w   _DELY,-(%a1)            | [98]
      move.w   _DESTY,-(%a1)           | [99]
      move.w   _CHUP,-(%a1)            | [100]

      movea.w  _scrpt2,%a0             | [102]
      move.w   (-106,%a6),%d3          | [103]
      tst.w    _DOUBLE                 | [104 NE]
      jbne     adr_dbl                 | [105]
      move.w   (-102,%a6),%d1          | [106]
      jbra     adr_nodb                | [107]



adr_dbl:
|;;   movem.l   a0/a1,-(sp)            | [112]
	lea      -8(%a7),%a7
	movem.l  %a0-%a1,(%a7)
      move.w   %d3,-(%a7)              | [113]
      move.l   (8,%a6),-(%a7)          | [114]
      jsr      _act_siz                | [115]
      addq.l   #6,%a7                  | [116]
|;;   movem.l   (sp)+,a0/a1            | [117]
	movem.l  (%a7),%a0-%a1
	lea      8(%a7),%a7
      move.w   %d0,%d3                 | [118]
      move.w   %d3,(-92,%a6)           | [119]
      move.w   (-102,%a6),%d2          | [120]
      move.w   _XACC_DDA,%d4           | [121]
      move.w   _DDA_INC,%d0            | [122]
      moveq.l  #0,%d1                  | [123]
      jbra     clc_n_su                | [124]
doub_wid:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   add.w     d0,d4                  | [126 CC]
	mvz.w    %d0,%d2
	move.w   %d4,%d3
	ori.l    #0xffff0000,%d3
	add.l    %d2,%d3
	move     %ccr,%d5
	move.w   %d3,%d4
	move     %d5,%ccr
      jbcc     no_db_wd                | [127]
|;;   addq.w    #1,d1                  | [128]
	move.w   %d1,%d2
	addq.l   #1,%d2
	move.w   %d2,%d1
no_db_wd:
|***	RESTORE	d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
|;;   btst.b    #0,_T_SCLSTS+1         | [130 EQ]
	moveq.l  #0,%d5
	btst     %d5,_T_SCLSTS+1
      jbeq     clc_n_su                | [131]
|;;   addq.w    #1,d1                  | [132]
	move.w   %d1,%d5
	addq.l   #1,%d5
	move.w   %d5,%d1
clc_n_su:
|;;   dbra      d2,doub_wid            | [134]
	tst.w    %d2
	jbeq     _PA_133_
	subq.l   #1,%d2
	jbra     doub_wid
	_PA_133_:         
      move.w   %d1,(-90,%a6)           | [135]
| 524 "vdi/vdi_tblit.S"
      tst.w    (-110,%a6)              | [137 NE]
      jbne     adr_nodb                | [138]



      movea.w  #0,%a0                  | [142]
adr_nodb:
      move.w   %a0,(-2,%a6)            | [144]
      move.w   (-108,%a6),%d2          | [145]
      move.w   (-104,%a6),%d0          | [146]
      move.w   _L_OFF,%d6              | [147]
      move.w   _R_OFF,%d7              | [148]
      clr.w    (-74,%a6)               | [149]
      btst     #0,(-96+1,%a6)          | [150 EQ]
      jbeq     adr_notk                | [151]
      move.w   _WEIGHT,%d4             | [152 NE]
      jbne     adr_th1                 | [153]
|;;   and.w     #0xFFFF-0x1,-96(a6)    | [154]
	move.w   (-96,%a6),%d5
	andi.l   #(0xFFFF&(0xFFFF-0x1)),%d5
	move.w   %d5,(-96,%a6)
adr_th1:
      tst.w    _MONO_STATUS            | [156 NE]
      jbne     adr_notk                | [157]

|;;   add.w     d4,d1                  | [159]
	move.w   %d1,%d5
	add.l    %d4,%d5
	move.w   %d5,%d1
adr_notk:
      btst     #2,(-96+1,%a6)          | [161 EQ]
      jbeq     chk_addr                | [162]

|;;   add.w     d6,d1                  | [164]
	move.w   %d1,%d5
	add.l    %d6,%d5
|;;   add.w     d7,d1                  | [165]
	add.l    %d7,%d5
	move.w   %d5,%d1







chk_addr:
      move.w   (-110,%a6),%d4          | [174 EQ]
      jbeq     chk_clip                | [175]
      move.w   #0,(-4,%a6)             | [176]
|;;   subi.w    #1800,d4               | [177]
	move.w   %d4,%d5
	subi.l   #1800,%d5
	move.w   %d5,%d4
      move.w   %d4,(-126,%a6)          | [178 EQ GT]
      jbeq     add180                  | [179]
      jbgt     add270                  | [180]



|;;   sub.w     d1,d2                  | [184]
	sub.l    %d1,%d2
      move.w   %d2,(-108,%a6)          | [185]
|;;   exg       d1,d3                  | [186]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3
      jbra     chk_clip                | [187]



add180:
|;;   sub.w     d1,d0                  | [192]
	move.w   %d0,%d5
	sub.l    %d1,%d5
|;;   move.w    d0,-104(a6)            | [193]
	move.w   %d5,(-104,%a6)
	move.w   %d5,%d0
      jbra     chk_clip                | [194]



add270:
|;;   exg       d1,d3                  | [199]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3

| 598 "vdi/vdi_tblit.S"
chk_clip:
      moveq.l  #0,%d5                  | [204]
      move.w   _CLIP,-(%a1)            | [205 EQ]
      jbeq     clp_done                | [206]
      move.w   _XMN_CLIP,-(%a1)        | [207]
      move.w   _YMN_CLIP,-(%a1)        | [208]
      move.w   _XMX_CLIP,-(%a1)        | [209]
      move.w   _YMX_CLIP,-(%a1)        | [210]



      cmp.w    (-116,%a6),%d2          | [214 GE]
      jbge     chk_ymx                 | [215]



|;;   add.w     d3,d2                  | [219]
	add.l    %d3,%d2
      cmp.w    (-116,%a6),%d2          | [220 GT]
      jbgt     chk_xmn                 | [221]
      jbra     upda_dst                | [222]
chk_ymx:
      cmp.w    (-120,%a6),%d2          | [224 GT]
      jbgt     upda_dst                | [225]

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   add.w     d3,d2                  | [227]
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [228]
	subq.l   #1,%d2
|;;   sub.w     -120(a6),d2            | [229 LE]
	ext.l    %d2
	mvs.w    (-120,%a6),%d0
	sub.l    %d0,%d2
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
	lea      4(%a7),%a7
      jble     chk_xmn                 | [230]



|;;   andi.w    #1,d2                  | [234 EQ]
	andi.l   #(0xFFFF&1),%d2
      jbeq     chk_xmn                 | [235]
      btst     #4,(-96+1,%a6)          | [236 NE]
      jbne     chk_xmn                 | [237]
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   rol.w     -100(a6)               | [238]
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
      cmp.w    (-114,%a6),%d0          | [243 GE]
      jbge     chk_xmx                 | [244]



|;;   add.w     d1,d0                  | [248]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   not.w     d5                     | [249]
	move.w   %d2,%d0
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5
      cmp.w    (-114,%a6),%d0          | [250 GT]
      jbgt     clp_done                | [251]
      jbra     upda_dst                | [252]
chk_xmx:
      cmp.w    (-118,%a6),%d0          | [254 GT]
      jbgt     upda_dst                | [255]

|;;   add.w     d1,d0                  | [257]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   subq.w    #1,d0                  | [258]
	subq.l   #1,%d2
	move.w   %d2,%d0
      cmp.w    (-118,%a6),%d0          | [259 LE]
      jble     clp_done                | [260]
|;;   not.w     d5                     | [261]
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5



clp_done:
      clr.w    (-28,%a6)               | [266]

|;;   move.w    _FWIDTH,-16(a6)        | [268]
	move.w   _FWIDTH,%d2
	move.w   %d2,(-16,%a6)
|;;   move.l    _FBASE,-10(a6)         | [269]
	move.l   _FBASE,%d2
	move.l   %d2,(-10,%a6)

chk_skw:
      move.w   (-96,%a6),%d1           | [272]
|;;   andi.w    #0x4 +0x1 +0x10,d1     | [273 EQ]
	andi.l   #(0xFFFF0000!~(0x4+0x1+0x10)),%d1
	tst.w    %d1
      jbeq     chk_rota                | [274]



chk_chup:
      tst.w    (-110,%a6)              | [279 NE]
      jbne     preblt                  | [280]

      btst     #2,%d1                  | [282 EQ]
      jbeq     preskew                 | [283]
      tst.w    %d5                     | [284 NE]
      jbne     preblt                  | [285]
preskew:
      btst     #4,%d1                  | [287 EQ]
      jbeq     chk_rota                | [288]






preblt:
|***	SAVE	d4	***
	lea      -4(%a7),%a7
	movem.l  %d4,(%a7)
      move.w   _SOURCEX,%d0            | [297]
      move.w   %d0,%d2                 | [298]
|;;   andi.w    #0x0F,d2               | [299]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [300]
|;;   lsr.w     #4,d0                  | [301]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [302]
	lsl.l    #1,%d0

      move.w   _SOURCEY,%d2            | [304]
      move.w   (-106,%a6),(-22,%a6)    | [305]
|;;   add.w     -22(a6),d2             | [306]
	move.w   (-22,%a6),%d3
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [307]
	subq.l   #1,%d2

      mulu.w   (-16,%a6),%d2           | [309]
|;;   neg.w     -16(a6)                | [310]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   add.w     d0,d2                  | [311]
	move.w   %d2,%d4
	add.l    %d0,%d4
      movea.l  (-10,%a6),%a0           | [312]
	move.w   %d4,%d2
|;;   lea       (a0,d2.w),a0           | [313]
	move.w   %d2,%a5
	lea      (0,%a0,%a5.l*1),%a0

      move.w   (-102,%a6),%d0          | [315]
      move.w   _WEIGHT,%d1             | [316]
|;;   add.w     d6,d7                  | [317]
	add.l    %d6,%d7
      tst.w    _DOUBLE                 | [318 EQ]
      jbeq     full_wt                 | [319]
|;;   lsr.w     #1,d1                  | [320 NE]
	mvz.w    %d1,%d4
	lsr.l    #1,%d4
	move     %ccr,%d3
	move.w   %d4,%d1
	move     %d3,%ccr
      jbne     ful_wtok                | [321]
|;;   addq.w    #1,d1                  | [322]
	addq.l   #1,%d4
	move.w   %d4,%d1
ful_wtok:
|;;   lsr.w     #1,d7                  | [324]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
full_wt:
      btst     #0,(-96+1,%a6)          | [326 EQ]
      jbeq     no_smear                | [327]

|;;   add.w     d1,d0                  | [329]
	add.l    %d1,%d0
      move.w   %d1,(-74,%a6)           | [330]
no_smear:

      clr.w    (-26,%a6)               | [333]
      move.w   (-106,%a6),%d1          | [334]
      move.w   (-96,%a6),%d2           | [335]
      btst     #4,%d2                  | [336 EQ]
      jbeq     no_out                  | [337]
      tst.w    _DOUBLE                 | [338 NE]
      jbne     no_out                  | [339]

|;;   addq.w    #3,d0                  | [341]
	addq.l   #3,%d0
|;;   addq.w    #1,-26(a6)             | [342]
	move.w   (-26,%a6),%d4
	addq.l   #1,%d4
	move.w   %d4,(-26,%a6)
|;;   addq.w    #2,-106(a6)            | [343]
	move.w   (-106,%a6),%d4
	addq.l   #2,%d4
	move.w   %d4,(-106,%a6)
|;;   addq.w    #3,d1                  | [344]
	move.w   %d1,%d4
	addq.l   #3,%d4
	move.w   %d4,%d1
no_out:

      move.w   %d0,(-20,%a6)           | [347]
|;;   add.w     d7,d0                  | [348]
	add.l    %d7,%d0
      move.w   %d0,(-102,%a6)          | [349]
|;;   lsr.w     #4,d0                  | [350]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [351]
	lsl.l    #1,%d0
|;;   addq.w    #2,d0                  | [352]
	addq.l   #2,%d0
|;;   neg.w     d0                     | [353]
	neg.l    %d0
      move.w   %d0,(-18,%a6)           | [354]
|;;   neg.w     d0                     | [355]
	neg.l    %d0
|;;   subq.w    #1,d1                  | [356]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [357]
	mulu.w   %d4,%d0
      movea.l  _scrtchp,%a1            | [358]
|;;   adda.w    -2(a6),a1              | [359]
	mvs.w    (-2,%a6),%d3
	adda.l   %d3,%a1
      move.l   %a1,(-10,%a6)           | [360]
      btst     #4,%d2                  | [361 NE]
      jbne     do_clear                | [362]
      btst     #2,%d2                  | [363 EQ]
      jbeq     no_clear                | [364]
do_clear:

      movea.l  %a1,%a2                 | [367]
      move.w   %d0,%d3                 | [368]
|;;   sub.w     -18(a6),d3             | [369]
	move.w   (-18,%a6),%d1
	sub.l    %d1,%d3
|;;   lsr.w     #1,d3                  | [370]
	and.l    #0xffff,%d3
	lsr.l    #1,%d3
|;;   subq.w    #1,d3                  | [371]
	subq.l   #1,%d3
      moveq.l  #0,%d1                  | [372]
replp:
      move.w   %d1,(%a2)+              | [374]
|;;   dbra      d3,replp               | [375]
	tst.w    %d3
	jbeq     _PA_374_
	subq.l   #1,%d3
	jbra     replp
	_PA_374_:         

      btst     #4,%d2                  | [377 EQ]
      jbeq     no_clear                | [378]
      tst.w    _DOUBLE                 | [379 NE]
      jbne     no_clear                | [380]

|;;   subq.w    #3,-20(a6)             | [382]
	move.w   (-20,%a6),%d4
	subq.l   #3,%d4
	move.w   %d4,(-20,%a6)
|;;   subq.w    #1,-102(a6)            | [383]
	move.w   (-102,%a6),%d4
	subq.l   #1,%d4
	move.w   %d4,(-102,%a6)
|;;   add.w     -18(a6),d0             | [384]
	move.w   %d0,%d4
	move.w   (-18,%a6),%d3
	add.l    %d3,%d4
	move.w   %d4,%d0



no_clear:
|;;   adda.w    d0,a1                  | [389]
	mvs.w    %d0,%d3
	adda.l   %d3,%a1



      move.w   (-96,%a6),(-122,%a6)    | [393]

      clr.w    (-98,%a6)               | [395]
      move.w   #1,(-84,%a6)            | [396]
      clr.w    (-76,%a6)               | [397]

      move.w   #1,(-86,%a6)            | [399]
      move.w   #2,(-88,%a6)            | [400]
|;;   andi.w    #0x4 +0x1,-96(a6)      | [401]
	move.w   (-96,%a6),%d4
	andi.l   #(0xFFFF&(0x4+0x1)),%d4
	move.w   %d4,(-96,%a6)
|***	RESTORE	AFTER d4	***
	move.l   (%a7)+,%d4

      jbsr     norm_blt                | [403]

      move.w   (-122,%a6),(-96,%a6)    | [405]
|;;   move.w    _WRT_MODE,-98(a6)      | [406]
	move.w   _WRT_MODE,%d2
	move.w   %d2,(-98,%a6)
no_mode:
      move.w   (-18,%a6),%d7           | [408]
|;;   neg.w     d7                     | [409]
	move.w   %d7,%d2
	neg.l    %d2
|;;   move.w    d7,-16(a6)             | [410]
	move.w   %d2,(-16,%a6)
	move.w   %d2,%d7



      btst     #4,(-96+1,%a6)          | [414 EQ]
      jbeq     skip_out                | [415]
      tst.w    _DOUBLE                 | [416 NE]
      jbne     skip_out                | [417]

      movea.l  (-10,%a6),%a0           | [419]
      ext.l    %d7                     | [420]
      add.l    %d7,(-10,%a6)           | [421]
      jbsr     outlin                  | [422]
skip_out:



      clr.w    _SOURCEX                | [427]
      clr.w    _SOURCEY                | [428]


|;;   andi.w    #0xFFFF-(0x4 +0x1),-96(a6) | [431]
	move.w   (-96,%a6),%d2
	andi.l   #(0xFFFF&(0xFFFF-(0x4+0x1))),%d2
	move.w   %d2,(-96,%a6)



chk_rota:
      tst.w    (-110,%a6)              | [436 EQ]
      jbeq     chk_db                  | [437]

      jbsr     rotation                | [439]

chk_db:
      tst.w    _DOUBLE                 | [442 EQ]
      jbeq     do_clip                 | [443]

      jbsr     replicat                | [445]







do_clip:
      btst     #0,(-96+1,%a6)          | [455 EQ]
      jbeq     no_thik                 | [456]
      move.w   _WEIGHT,%d0             | [457]
      tst.w    _MONO_STATUS            | [458 NE]
      jbne     do_cl_nt                | [459]
|;;   add.w     d0,-102(a6)            | [460]
	move.w   (-102,%a6),%d2
	add.l    %d0,%d2
	move.w   %d2,(-102,%a6)
do_cl_nt:
      move.w   %d0,(-74,%a6)           | [462]
no_thik:
      tst.w    (-112,%a6)              | [464 EQ]
      jbeq     scrn_blt                | [465]

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
|;;   add.w     -102(a6),d0            | [512]
	move.w   %d0,%d2
	move.w   (-102,%a6),%d1
	add.l    %d1,%d2
|;;   subq.w    #1,d0                  | [513]
	subq.l   #1,%d2
	move.w   %d2,%d0
      cmp.w    (-118,%a6),%d0          | [514 LE]
      jble     scrn_blt                | [515]

|;;   sub.w     -118(a6),d0            | [517]
	move.w   (-118,%a6),%d1
	sub.l    %d1,%d2
|;;   sub.w     d0,-102(a6)            | [518]
	move.w   (-102,%a6),%d1
	sub.l    %d2,%d1
	move.w   %d1,(-102,%a6)

      jbra     scrn_blt                | [520]



nxtword:
|;;           .dc.b 2,4,0,8            | [525]
	.byte	2,4,0,8
| 931 "vdi/vdi_tblit.S"
	.p2align 1,0
scrn_blt:
|;;   move.w    _TEXT_FG,-84(a6)       | [528]
	move.w   _TEXT_FG,%d1
	move.w   %d1,(-84,%a6)
      move.w   (-6,%a6),(-76,%a6)      | [529]
      move.w   _v_planes,%d0           | [530]
      move.w   %d0,(-86,%a6)           | [531]
|;;   move.b    nxtword-1(pc,d0.w),d0  | [532]
	move.w   %d0,%a5
	movea.l  #nxtword-1,%a0
	move.b   (0,%a0,%a5.l*1),%d0
      move.w   %d0,(-88,%a6)           | [533]

      move.w   _SOURCEX,%d0            | [535]
      move.w   %d0,%d2                 | [536]
|;;   andi.w    #0x0F,d2               | [537]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [538]
|;;   lsr.w     #3,d0                  | [539]
	mvz.w    %d0,%d1
	lsr.l    #3,%d1
|;;   andi.w    #0xFFFE,d0             | [540]
	andi.l   #(0xFFFF&0xFFFE),%d1
      move.w   _SOURCEY,%d2            | [541]

      move.w   (-106,%a6),(-22,%a6)    | [543]
|;;   add.w     -22(a6),d2             | [544]
	move.w   (-22,%a6),%d3
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [545]
	subq.l   #1,%d2
      mulu.w   (-16,%a6),%d2           | [546]
|;;   neg.w     -16(a6)                | [547]
	move.w   (-16,%a6),%d3
	neg.l    %d3
	move.w   %d3,(-16,%a6)
|;;   add.w     d0,d2                  | [548]
	move.w   %d2,%d3
	add.l    %d1,%d3



      movea.l  (-10,%a6),%a0           | [552]
|;;   adda.w    d2,a0                  | [553]
	ext.l    %d3
	adda.l   %d3,%a0
      move.l   %a0,(-10,%a6)           | [554]

      move.w   (-102,%a6),(-20,%a6)    | [556]

      move.w   (-108,%a6),%d1          | [558]
|;;   add.w     -106(a6),d1            | [559]
	move.w   (-106,%a6),%d3
	add.l    %d3,%d1
|;;   subq.w    #1,d1                  | [560]
	subq.l   #1,%d1

      move.w   (-104,%a6),%d0          | [562]

      jsr      concat                  | [564]



      movea.l  _v_bas_ad,%a1           | [568]
      adda.l   %d1,%a1                 | [569]

      move.l   %a1,(-14,%a6)           | [571]
      move.w   %d0,(-26,%a6)           | [572]

      move.w   _v_lin_wr,%d0           | [574]
|;;   neg.w     d0                     | [575]
	move.w   %d0,%d3
	neg.l    %d3
|;;   move.w    d0,-18(a6)             | [576]
	move.w   %d3,(-18,%a6)

      jbsr     norm_blt                | [578]

upda_dst:
      move.w   _DELX,%d1               | [581]


      tst.w    _DOUBLE                 | [584 EQ]
      jbeq     upda_ndb                | [585]



      move.w   (-90,%a6),%d1           | [589]
      move.w   (-92,%a6),%d3           | [590]

      tst.w    (-94,%a6)               | [592 EQ]
      jbeq     upda_ndb                | [593]
|;;   exg       d1, d3                 | [594]
	move.l   %d1,%d2
	move.l   %d3,%d1
	move.l   %d2,%d3





upda_ndb:
|;;   btst.b    #0,_STYLE+1            | [601 EQ]
	moveq.l  #0,%d2
	btst     %d2,_STYLE+1
      jbeq     upda_ntk                | [602]
      tst.w    _MONO_STATUS            | [603 NE]
      jbne     upda_ntk                | [604]
|;;   add.w     _WEIGHT,d1             | [605]
	move.w   _WEIGHT,%d2
	add.l    %d2,%d1
upda_ntk:
      tst.w    (-110,%a6)              | [607 GT]
      jbgt     ck90                    | [608]
|;;   add.w     d1,_DESTX              | [609]
	move.w   _DESTX,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [610]
ck90:
      tst.w    (-126,%a6)              | [612 EQ GT]
      jbeq     ck180                   | [613]
      jbgt     ck270                   | [614]
|;;   sub.w     d1,_DESTY              | [615]
	move.w   _DESTY,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTY
      jbra     blt_done                | [616]
ck180:
|;;   sub.w     d1,_DESTX              | [618]
	move.w   _DESTX,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [619]
ck270:
|;;   add.w     d1,_DESTY              | [621]
	move.w   _DESTY,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTY
blt_done:
|;;   movem.l   (sp)+,d2-d7/a2-a5      | [623]
	movem.l  (%a7),%d2-%d7/%a2-%a5
	lea      40(%a7),%a7
      unlk     %a6                     | [624]
      rts                              | [625]



|;;           .text                    | [630]
| 1043 "vdi/vdi_tblit.S"
|;;   lf_tab: dc.w 0xFFFF              | [632]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: dc.w 0x7FFF              | [633]
rf_tab:
	.short	0x7FFF
|;;                   dc.w 0x3FFF      | [634]
	.short	0x3FFF
|;;                   dc.w 0x1FFF      | [635]
	.short	0x1FFF
|;;                   dc.w 0x0FFF      | [636]
	.short	0x0FFF
|;;                   dc.w 0x07FF      | [637]
	.short	0x07FF
|;;                   dc.w 0x03FF      | [638]
	.short	0x03FF
|;;                   dc.w 0x01FF      | [639]
	.short	0x01FF
|;;                   dc.w 0x00FF      | [640]
	.short	0x00FF
|;;                   dc.w 0x007F      | [641]
	.short	0x007F
|;;                   dc.w 0x003F      | [642]
	.short	0x003F
|;;                   dc.w 0x001F      | [643]
	.short	0x001F
|;;                   dc.w 0x000F      | [644]
	.short	0x000F
|;;                   dc.w 0x0007      | [645]
	.short	0x0007
|;;                   dc.w 0x0003      | [646]
	.short	0x0003
|;;                   dc.w 0x0001      | [647]
	.short	0x0001
|;;                   dc.w 0x0000      | [648]
	.short	0x0000

|;;           .text                    | [650]
| 1074 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [653]
|;;   sub.w     -24(a6),d1             | [654]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [655 PL]
      bpl.b    do_rot                  | [656]



|;;   neg.w     d1                     | [660]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [661]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [662]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [664]

|;;   lsl.w     #1,d0                  | [666]
	move.w   %d0,%d5
	lsl.l    #1,%d5
      lea      lf_tab,%a2              | [667]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [668]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [669]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [670]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [675]
|;;   lsl.w     #1,d0                  | [676]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [677]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [679]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [680]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [683]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [684]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [685]
	andi.l   #(0xFFFF&0xF),%d5
|;;   lsl.w     #1,d2                  | [686]
	lsl.l    #1,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [687]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [688]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [689]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [690]
      move.w   #0x8000,(-80,%a6)       | [691]
      moveq.l  #(-1),%d3               | [692]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [695 HI]
      bhi.b    doub_des                | [696]



|;;   lsl.w     #1,d0                  | [700]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [701]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [702]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [703]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [705]
	move.w   %d5,%d1
      jbra     msk_done                | [706]



doub_des:
      move.w   %d0,%d1                 | [711]
|;;   lsr.w     #4,d1                  | [712]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [713]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [714 NE]
      jbne     mlt_dest                | [715]



      move.w   (-24,%a6),%d1           | [719]
|;;   add.w     -20(a6),d1             | [720]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [721 CC]
      jbcc     mlt_dest                | [722]
      moveq.l  #0,%d7                  | [723]
      jbra     do_ritem                | [724]

mlt_dest:
      tst.w    (-42,%a6)               | [727 LT]
      jblt     mltleft                 | [728]
      moveq.l  #4,%d7                  | [729]
      jbra     do_ritem                | [730]
mltleft:
      moveq.l  #8,%d7                  | [732]
do_ritem:
|;;   andi.w    #0x000F,d0             | [734 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jbne     not_null                | [735]
|;;   subq.w    #1,d3                  | [736]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [737]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [739]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [740]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [741]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [744]
      move.w   %d3,(-28,%a6)           | [745]
      move.w   %d4,(-36,%a6)           | [746]
|;;   addq.w    #2,d3                  | [747]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [748 CS]
      jbcs     msk0                    | [749]
|;;   addq.w    #1,d3                  | [750]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [752]
      move.w   %d3,(-30,%a6)           | [753]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jbra     plane_loop              | [754]

| 1194 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [759]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [760]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [761]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [762]
	.byte	4*5,4*5,4*6,4*6


toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [766]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [768]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [772]
|;;   lsr.w     -84(a6)                | [773 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [774]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [775 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [776]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [780]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [781]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [782]
      movea.l  %a2,%a5                 | [783]

|;;   movea.l   twoptble(pc,d0.w),a4   | [785]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [789 EQ]
      jbeq     wrt_char                | [790]

      btst     #1,%d0                  | [792 EQ]
      jbeq     no_lite                 | [793]



|;;   move.w    _LITEMASK,-78(a6)      | [797]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [798]
      lea      liteop,%a5              | [799]

      move.l   %a2,(-64,%a6)           | [801]
      lea      liteopwf,%a2            | [802]

      move.l   %a4,(-72,%a6)           | [804]
      lea      liteopw,%a4             | [805]

no_lite:
      btst     #0,%d0                  | [808 EQ]
      jbeq     no_thick                | [809]



      clr.w    (-82,%a6)               | [813]

      move.l   %a5,(-48,%a6)           | [815]
      lea      thknop,%a5              | [816]

      move.l   %a2,(-60,%a6)           | [818]
      lea      thknopwf,%a2            | [819]

      move.l   %a4,(-68,%a6)           | [821]
      lea      thknopw,%a4             | [822]

no_thick:
      btst     #2,%d0                  | [825 EQ]
      jbeq     wrt_char                | [826]



      move.w   (-100,%a6),(-80,%a6)    | [830]
      move.l   %a5,(-56,%a6)           | [831]
      lea      skewop,%a5              | [832]

      tst.w    (-124,%a6)              | [834 EQ GT]
      jbgt     wrt_char                | [835]
      jbeq     not_sngl                | [836]
      clr.w    (-28,%a6)               | [837]
      moveq.l  #0,%d7                  | [838]
      jbra     chng_blt                | [839]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [841 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      bls.b    wrt_char                | [842]

      tst.w    (-42,%a6)               | [844 LT]
      jblt     chng_left               | [845]
      moveq.l  #4,%d7                  | [846]
      jbra     chng_blt                | [847]
chng_left:
      moveq.l  #8,%d7                  | [849]
chng_blt:
      move.w   %d7,(-124,%a6)          | [851]

wrt_char:
      move.w   (-124,%a6),%d7          | [854 LE]
      jble     set_chr_hgt             | [855]
      movea.l  %a2,%a5                 | [856]
      move.w   (-18,%a6),%d5           | [857]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [859]
|;;   subq.w    #1,d3                  | [860]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [862]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [863]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [865 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jble     no_more_planes          | [866]
      movea.l  (-10,%a6),%a0           | [867]
      movea.l  (-14,%a6),%a1           | [868]
      addq.l   #2,%a1                  | [869]
      move.l   %a1,(-14,%a6)           | [870]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [871 EQ]
      jbeq     plane_loop              | [872]
      jbra     norm_blt                | [873]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [875]
blttype:
|;;           dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [877]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [884]
      move.w   (-16,%a6),%d6           | [885]
      move.w   (-18,%a6),%d7           | [886]
      lea      sgl_rtn,%a3             | [887]
      jbra     sgl_1st_scan            | [888]

sgl_lp:
|;;   adda.w    d6,a0                  | [891]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [892]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [894]
| 1339 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [896]


      move.w   (-42,%a6),%d1           | [899 MI]
      bmi.b    left_rol                | [900]
      lsr.l    %d1,%d0                 | [901]
      jbra     end_ro                  | [902]
left_rol:
      lsl.l    %d1,%d0                 | [904]
end_ro:
      swap     %d0                     | [906]
      move.w   %d0,%d1                 | [907]
      swap     %d0                     | [908]

      jmp      (%a5)                   | [910]

sgl_rtn:
      move.w   %d1,(%a1)               | [913]
|;;   dbra      d3,sgl_lp              | [914]
	tst.w    %d3
	jbeq     _PA_913_
	subq.l   #1,%d3
	jbra     sgl_lp
	_PA_913_:         
      jbra     next_plane              | [915]




dbl_loop:
      movea.l  %a3,%a4                 | [922]
      move.w   (-16,%a6),%d6           | [923]
      move.w   (-18,%a6),%d7           | [924]
      lea      dbl_2wrd,%a2            | [925]
      move.l   %a2,%d5                 | [926]
      lea      dbl_1wrd,%a3            | [927]
      jbra     dbl_1st_scan            | [928]

dbl_lp:
|;;   adda.w    d6,a0                  | [931]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [932]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [933]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [935]
| 1387 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [937]


      move.w   (-42,%a6),%d1           | [940 MI]
      bmi.b    dbleft                  | [941]
      lsr.l    %d1,%d0                 | [942]
      jbra     dblendr                 | [943]
dbleft:
      lsl.l    %d1,%d0                 | [945]
dblendr:
      swap     %d0                     | [947]
      move.w   %d0,%d1                 | [948]
      swap     %d0                     | [949]
      move.w   (-38,%a6),%d2           | [950]

      jmp      (%a5)                   | [952]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [955]
      move.w   %d0,%d1                 | [956]

      movea.w  (-88,%a6),%a2           | [958]
|;;   move.w    (a1,a2.w),d4           | [959]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [961]

|;;   exg       a3,d5                  | [963]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [964]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [967]
|;;   move.w    d1,(a1,a2.w)           | [968]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [970]
	tst.w    %d3
	jbeq     _PA_969_
	subq.l   #1,%d3
	jbra     dbl_lp
	_PA_969_:         
      jbra     next_plane              | [971]





left_loop:
      movea.l  %d6,%a0                 | [979]
      movea.l  %d7,%a1                 | [980]
|;;   adda.w    -16(a6),a0             | [981]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [982]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [983]
|;;   rol.w     -78(a6)                | [984]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [986 NE]
      jbne     skewopw                 | [987]
mlt_left:
      move.l   %a0,%d6                 | [989]
      move.l   %a1,%d7                 | [990]
      move.w   (-28,%a6),(-44,%a6)     | [991]







      move.l   (%a0)+,%d0              | [999]


      move.w   (%a1),%d4               | [1002]

      move.w   (-42,%a6),%d1           | [1004]
      lsl.l    %d1,%d0                 | [1005]

      swap     %d0                     | [1007]
      move.w   %d0,%d1                 | [1008]
      swap     %d0                     | [1009]

      move.w   (-38,%a6),%d2           | [1011]

      lea      lft_lfrng,%a3           | [1013]
      jmp      (%a5)                   | [1014]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1017]
|;;   adda.w    -88(a6),a1             | [1018]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1020]
      lea      lft_wrds,%a3            | [1021]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1026]
|;;   and.w     d2,d4                  | [1027]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [1028]





      swap     %d0                     | [1034]
      move.w   %d1,%d0                 | [1035]
      swap     %d0                     | [1036]

      move.w   (-42,%a6),%d1           | [1038]
      lsl.l    %d1,%d0                 | [1039]

      swap     %d0                     | [1041]
      move.w   %d0,%d1                 | [1042]
      swap     %d0                     | [1043]

|;;   not.w     d2                     | [1045]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1046]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1047]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1048]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1050]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1052 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     lef_don                 | [1053]

      jmp      (%a4)                   | [1055]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1058]
|;;   adda.w    -88(a6),a1             | [1059]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_lef                | [1061]

lef_don:
      move.w   (-36,%a6),%d2           | [1064]

      lea      lft_rfrng,%a3           | [1066]
      jmp      (%a5)                   | [1067]

lft_rfrng:
      move.w   %d1,(%a1)               | [1070]

|;;   dbra      d3,left_loop           | [1072]
	tst.w    %d3
	jbeq     _PA_1071_
	subq.l   #1,%d3
	jbra     left_loop
	_PA_1071_:         
      jbra     next_plane              | [1073]






rite_loop:
      movea.l  %d6,%a0                 | [1082]
      movea.l  %d7,%a1                 | [1083]
|;;   adda.w    -16(a6),a0             | [1084]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [1085]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [1086]
|;;   rol.w     -78(a6)                | [1087]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [1089 NE]
      jbne     skewopw                 | [1090]
mlt_rite:
      move.l   %a0,%d6                 | [1092]
      move.l   %a1,%d7                 | [1093]
      move.w   (-28,%a6),(-44,%a6)     | [1094]
      move.w   (%a0)+,%d0              | [1095]



      swap     %d0                     | [1099]

      move.w   (%a1),%d4               | [1101]

      move.w   (-42,%a6),%d1           | [1103]
      lsr.l    %d1,%d0                 | [1104]
      swap     %d0                     | [1105]
      move.w   %d0,%d1                 | [1106]
      swap     %d0                     | [1107]

      move.w   (-38,%a6),%d2           | [1109]

      lea      rgt_lfrng,%a3           | [1111]
      jmp      (%a5)                   | [1112]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1115]
|;;   adda.w    -88(a6),a1             | [1116]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1118]
      lea      rgt_wrds,%a3            | [1119]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1124]
|;;   and.w     d2,d4                  | [1125]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [1126]
      move.w   (%a0)+,%d0              | [1127]



      swap     %d0                     | [1131]

      move.w   (-42,%a6),%d1           | [1133]
      lsr.l    %d1,%d0                 | [1134]
      swap     %d0                     | [1135]
      move.w   %d0,%d1                 | [1136]
      swap     %d0                     | [1137]

|;;   not.w     d2                     | [1139]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1140]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1141]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1142]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1144]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1146 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     rite_don                | [1147]

      jmp      (%a4)                   | [1149]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1152]
|;;   adda.w    -88(a6),a1             | [1153]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_rit                | [1155]

rite_don:
      move.w   (-36,%a6),%d2           | [1158]

      lea      rgt_rfrng,%a3           | [1160]
      jmp      (%a5)                   | [1161]

rgt_rfrng:
      move.w   %d1,(%a1)               | [1164]

|;;   dbra      d3,rite_loop           | [1166]
	tst.w    %d3
	jbeq     _PA_1165_
	subq.l   #1,%d3
	jbra     rite_loop
	_PA_1165_:         
      jbra     next_plane              | [1167]






top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1176]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [1177]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [1178]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [1179]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1180]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1183]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [1184]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [1185]
	not.l    %d0
|;;   and.w     d4,d1                  | [1186]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1187]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1190]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [1191]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1192]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1193]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1196]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [1197]
	not.l    %d0
|;;   and.w     d4,d1                  | [1198]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1199]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1202]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1203]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1204]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1207]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1208]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1209]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1212]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [1213]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1214]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1215]





twop0:
      moveq.l  #0,%d1                  | [1223]
      jmp      (%a3)                   | [1224]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [1227]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1228]

twop3:
      jmp      (%a3)                   | [1231]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1234]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [1235]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1236]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1239]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1240]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [1243]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1244]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1247]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [1248]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1249]
| 1719 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [1252]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [1253]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [1254]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [1255 EQ]
      jbeq     thk00                   | [1256]
|;;   ori.b     #0x10,ccr              | [1257 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [1258 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jbcc     thk01                   | [1259]
      move.w   #0x8000,%d6             | [1260]
thk01:
      move.w   %d6,(-34,%a6)           | [1262]
thk00:
      tst.w    (-28,%a6)               | [1264 MI]
      bmi.b    thk0                    | [1265]
      cmp.w    (-36,%a6),%d6           | [1266 CC]
      jbcc     thk0                    | [1267]
|;;   and.w     d6,d0                  | [1268]
	and.l    %d6,%d0
      jbra     thk1                    | [1269]
thk0:
      clr.w    %d0                     | [1271]
|;;   and.w     d6,d1                  | [1272]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [1274]
      move.w   %d2,%d5                 | [1275]
      swap     %d5                     | [1276]
      move.w   (-36,%a6),%d5           | [1277]
      lsl.l    %d6,%d5                 | [1278]

      swap     %d5                     | [1280]
      move.w   %d5,%d7                 | [1281]
      swap     %d5                     | [1282]

|;;   and.w     d7,d1                  | [1284]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [1285]
	and.l    %d5,%d0
|;;   swap      d1                     | [1286]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [1287]
|;;   subq.w    #1,d6                  | [1288]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [1290]
      lsr.l    #1,%d0                  | [1291]
      or.l     %d0,%d1                 | [1292]
|;;   dbra      d6,thkoplp             | [1293]
	tst.w    %d6
	jbeq     _PA_1292_
	subq.l   #1,%d6
	jbra     thkoplp
	_PA_1292_:         

      move.w   %d1,%d0                 | [1295]
      swap     %d1                     | [1296]
|;;   movem.l   (sp)+,d5-d7            | [1297]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [1298]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [1299]

| 1783 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [1304]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [1305 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk2                    | [1306]
|;;   and.w     -34(a6),d1             | [1307]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [1309]
      swap     %d1                     | [1310]
      clr.w    %d1                     | [1311]
      move.l   %d1,%d5                 | [1312]
|;;   subq.w    #1,d6                  | [1313]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [1315]
      or.l     %d5,%d1                 | [1316]
|;;   dbra      d6,thkopwlp            | [1317]
	tst.w    %d6
	jbeq     _PA_1316_
	subq.l   #1,%d6
	jbra     thkopwlp
	_PA_1316_:         

      move.w   %d1,%d5                 | [1319]
      swap     %d1                     | [1320]
|;;   or.w      -82(a6),d1             | [1321]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1322]
|;;   movem.l   (sp)+,d5-d6            | [1323]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [1324]
      jmp      (%a2)                   | [1325]

| 1822 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [1330]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [1331]
|;;   subq.w    #1,-30(a6)             | [1332 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      bmi.b    thk3                    | [1333]
      jbeq     thk4                    | [1334]

|;;   subq.w    #1,-30(a6)             | [1336 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk11                   | [1337]
|;;   and.w     d6,d1                  | [1338]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [1340]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jbra     thk7                    | [1341]

thk3:
      clr.w    %d1                     | [1344]
      jbra     thk5                    | [1345]

thk4:
|;;   and.w     d6,d1                  | [1348]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [1351 MI]
      bmi.b    thk6                    | [1352]
      move.w   (-28,%a6),%d5           | [1353]
|;;   addq.w    #2,d5                  | [1354]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [1355]
|;;   ori.b     #0x10,ccr              | [1356 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [1357 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jbcc     thk33                   | [1358]
      move.w   #0x8000,%d7             | [1359]
|;;   addq.w    #1,d5                  | [1360]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [1362 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jbne     thk34                   | [1363]
|;;   subq.w    #1,d5                  | [1364]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [1366 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [1367 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jbcc     thk31                   | [1368]
      move.w   #0x8000,%d6             | [1369]
thk31:
      cmp.w    %d6,%d7                 | [1371 CS]
      jbcs     thk35                   | [1372]
|;;   addq.w    #1,d5                  | [1373]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [1375]
      move.w   %d5,(-32,%a6)           | [1376]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [1378]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:

      move.w   (-74,%a6),%d6           | [1381]
      swap     %d1                     | [1382]
      clr.w    %d1                     | [1383]
      move.l   %d1,%d5                 | [1384]
|;;   subq.w    #1,d6                  | [1385]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [1387]
      or.l     %d5,%d1                 | [1388]
|;;   dbra      d6,thkopwl             | [1389]
	tst.w    %d6
	jbeq     _PA_1388_
	subq.l   #1,%d6
	jbra     thkopwl
	_PA_1388_:         

      move.w   %d1,%d5                 | [1391]
      swap     %d1                     | [1392]
|;;   or.w      -82(a6),d1             | [1393]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1394]
|;;   and.w     d2,d1                  | [1395]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [1396]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [1397]
      jmp      (%a2)                   | [1398]

| 1905 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [1403]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [1404]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [1405]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [1406]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1407]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1410]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [1411]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1412]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1415]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [1416]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1417]

| 1938 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [1422 CC]
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
      jbcc     no_shift                | [1423]

      swap     %d1                     | [1425]
      move.w   %d0,%d1                 | [1426]
      lsr.l    #1,%d1                  | [1427]

      swap     %d2                     | [1429]
      move.w   (-36,%a6),%d2           | [1430]
      lsr.l    #1,%d2                  | [1431]
      move.w   %d2,(-36,%a6)           | [1432]
      swap     %d2                     | [1433]
      move.w   %d2,(-38,%a6)           | [1434 EQ]
      jbeq     nxt_word                | [1435]
      move.w   (-42,%a6),%d0           | [1436 MI]
      bmi.b    dec_rol                 | [1437]
ror_add:
|;;   addq.w    #1,d0                  | [1439]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [1441]
      move.w   %d1,%d0                 | [1442]
      swap     %d1                     | [1443]
no_shift:
      movea.l  (-56,%a6),%a2           | [1445]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1446]


dec_rol:
      tst.b    %d0                     | [1450 EQ]
      jbeq     begn_ror                | [1451]
|;;   subq.w    #1,d0                  | [1452]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jbra     new_shif                | [1453]
begn_ror:
      clr.w    %d0                     | [1455]
      jbra     ror_add                 | [1456]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1461]
      swap     %d2                     | [1462]
      move.w   %d2,(-38,%a6)           | [1463]
|;;   adda.w    -88(a6),a1             | [1464]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1465]

      moveq.l  #15,%d0                 | [1467]
|;;   sub.w     -42(a6),d0             | [1468]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1469]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1470]
      jbra     no_shift                | [1471]






skewopw:
|;;   rol.w     -80(a6)                | [1480 CC]
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
      jbcc     do_shift                | [1481]

      tst.w    (-42,%a6)               | [1483 MI]
      bmi      mlt_left                | [1484]
      jbra     mlt_rite                | [1485]
do_shift:
|;;   ori       #0x10,ccr              | [1487 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1488]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1490]
      cmp.w    #0xFFFF,%d0             | [1491 EQ]
      jbeq     inc_rite                | [1492]

|;;   ori       #0x10,ccr              | [1494 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1495]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1496]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1499]
      tst.b    %d0                     | [1500 NE]
      jbne     no_rota                 | [1501]
      move.w   #0x8000,(-40,%a6)       | [1502]
no_rota:
      move.w   (-38,%a6),%d1           | [1504]
|;;   lsr.w     #1,d1                  | [1505 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jbeq     wnxt_wrd                | [1506]

      move.w   %d1,(-38,%a6)           | [1508]

      tst.w    %d0                     | [1510 MI]
      bmi.b    wdec_rol                | [1511]
|;;   addq.w    #1,-42(a6)             | [1512]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jbra     mlt_rite                | [1513]
wdec_rol:
      tst.b    %d0                     | [1515 EQ]
      jbeq     set_msk                 | [1516]
|;;   subq.w    #1,-42(a6)             | [1517]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jbra     mlt_left                | [1518]
set_msk:
      move.w   #1,(-42,%a6)            | [1520]
      jbra     mlt_rite                | [1521]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1524]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1525]
      jbra     do_left                 | [1526]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1529]

|;;   subq.w    #1,-28(a6)             | [1531]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1532]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1534]
|;;   sub.w     d0,d2                  | [1535]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1536]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1537]

      jbra     mlt_left                | [1539]




rotation:
|***	SAVE	d4/d6-d7	***
	lea      -28(%a7),%a7
	movem.l  %d4,12(%a7)
	movem.l  %d6-%d7,20(%a7)
      move.w   _SOURCEX,%d1            | [1546]
      move.w   %d1,%d2                 | [1547]
|;;   andi.w    #0x0F,d2               | [1548]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1549]
|;;   lsr.w     #4,d1                  | [1550]
	mvz.w    %d1,%d4
	lsr.l    #4,%d4
      moveq.l  #1,%d5                  | [1551]
|***	SAVE	d5	***
	movem.l  %d5,16(%a7)
|;;   lsl.w     d5,d1                  | [1552]
	lsl.l    %d5,%d4

      movea.l  (-10,%a6),%a0           | [1554]
|;;   adda.w    d1,a0                  | [1555]
	ext.l    %d4
	adda.l   %d4,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1557]
      move.w   _SOURCEY,%d0            | [1558]
      move.w   (-106,%a6),%d1          | [1559]
      move.w   %d1,(-22,%a6)           | [1560]
      move.w   (-16,%a6),%d2           | [1561]

      tst.w    (-126,%a6)              | [1563 EQ LT]
      jbeq     upsd_dwn                | [1564]
rot90:
      jblt     top_src                 | [1566]
|;;   neg.w     -16(a6)                | [1567]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   subq.w    #1,d1                  | [1568]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   add.w     d1,d0                  | [1569]
	add.l    %d4,%d0
      mulu.w   %d2,%d0                 | [1570]
|;;   adda.w    d0,a0                  | [1571]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1573]
|;;   lsr.w     #4,d0                  | [1574]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1575]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1576]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1577]
      movea.l  _scrtchp,%a1            | [1578]
|;;   adda.w    -4(a6),a1              | [1579]
	mvs.w    (-4,%a6),%d4
	adda.l   %d4,%a1

      tst.w    (-126,%a6)              | [1581 GT]
      jbgt     top_dwn                 | [1582]
|;;   neg.w     -18(a6)                | [1583]
	move.w   (-18,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1584]
|;;   subq.w    #1,d1                  | [1585]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [1586]
	mulu.w   %d4,%d0
|;;   adda.w    d0,a1                  | [1587]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1589]
      move.w   #0x8000,%d3             | [1590]
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
      move.w   %d3,%d4                 | [1591]
      movea.w  %d3,%a4                 | [1592]
|;;   lsr.w     d2,d3                  | [1593]
	mvz.w    %d3,%d5
	lsr.l    %d2,%d5
      moveq.l  #0,%d0                  | [1594]
      move.w   (-20,%a6),%d2           | [1595]
|***	RESTORE	AFTER d6	***
	movem.l  20(%a7),%d6
      move.w   (-16,%a6),%d6           | [1596]
      movea.l  %a0,%a2                 | [1597]
      movea.l  %a1,%a3                 | [1598]
	move.w   %d5,%d3
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      jbra     rot_nsrc                | [1599]
rot_ylp:
|***	SAVE	d2	***
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1601]
|***	RESTORE	d3	***
	movem.l  4(%a7),%d3
      jbra     rot_srt                 | [1602]
rot_xlp:
      move.w   (%a0),%d7               | [1604]
|;;   and.w     d3,d7                  | [1605 EQ]
	move.w   %d7,%d2
	and.l    %d3,%d2
	tst.w    %d2
      jbeq     rot_nor                 | [1606]
|;;   or.w      d4,d0                  | [1607]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1609 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1608_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1608_1:         
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      jbcc     rot_isrc                | [1610]
rot_ndst:
      move.w   %d0,(%a1)+              | [1612]
      moveq.l  #0,%d0                  | [1613]
rot_isrc:
|;;   adda.w    d6,a0                  | [1615]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,8(%a7)
|;;   dbra      d1,rot_xlp             | [1617]
	tst.w    %d1
	jbeq     _PA_1616_
	subq.l   #1,%d1
	jbra     rot_xlp
	_PA_1616_:         

      move.w   %d0,(%a1)               | [1619]
      moveq.l  #0,%d0                  | [1620]
|;;   adda.w    -18(a6),a3             | [1621]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1622]
      move.w   %a4,%d4                 | [1623]
|;;   ror.w     d5,d3                  | [1624 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1623_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1623_1:         
      jbcc     rnew_src                | [1625]
      addq.l   #2,%a2                  | [1626]
rnew_src:
      movea.l  %a2,%a0                 | [1628]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2
	movem.l  8(%a7),%d6
	lea      12(%a7),%a7
rot_nsrc:
|***	SAVE	d3	***
	lea      -12(%a7),%a7
	movem.l  %d3,4(%a7)
|;;   dbra      d2,rot_ylp             | [1630]
	tst.w    %d2
	jbeq     _PA_1629_
	subq.l   #1,%d2
	jbra     rot_ylp
	_PA_1629_:         


rot_done:
      move.w   (-102,%a6),%d0          | [1634]
      move.w   (-106,%a6),%d1          | [1635]
      move.w   %d1,(-20,%a6)           | [1636]
      move.w   %d1,(-102,%a6)          | [1637]
      move.w   %d0,(-22,%a6)           | [1638]
      move.w   %d0,(-106,%a6)          | [1639]

      move.w   (-92,%a6),%d0           | [1641]
      move.w   (-90,%a6),(-92,%a6)     | [1642]
      move.w   %d0,(-90,%a6)           | [1643]
      move.w   #1,(-94,%a6)            | [1644]

      move.w   (-18,%a6),%d0           | [1646]
      tst.w    (-126,%a6)              | [1647 GT]
      jbgt     rot_nneg                | [1648]
|;;   neg.w     d0                     | [1649]
	move.w   %d0,%d3
	neg.l    %d3
	move.w   %d3,%d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1651]
      move.w   (-4,%a6),%d0            | [1652]
|***	RESTORE	AFTER d3	***
	movem.l  4(%a7),%d3
	lea      12(%a7),%a7
repexit:
      clr.w    _SOURCEX                | [1654]
repexit3:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      movea.l  _scrtchp,%a2            | [1656]
|;;   adda.w    d0,a2                  | [1657]
	mvs.w    %d0,%d1
	adda.l   %d1,%a2
      move.l   %a2,(-10,%a6)           | [1658]
      clr.w    _SOURCEY                | [1659]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1660]


upsd_dwn:
      move.w   (-102,%a6),%d0          | [1664]
|;;   add.w     -24(a6),d0             | [1665]
	move.w   (-24,%a6),%d4
	add.l    %d4,%d0
|;;   subq.w    #1,d0                  | [1666]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1667]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1668]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1669]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1671]
      move.w   %d0,%d2                 | [1672]
|;;   lsr.w     d5,d2                  | [1673]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1674]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1675]
|;;   add.w     -4(a6),d0              | [1676 X]
	move.w   %d0,%d6
	mvz.w    (-4,%a6),%d4
	ori.l    #0xffff0000,%d6
	add.l    %d4,%d6
      movea.l  _scrtchp,%a1            | [1677]
|;;   adda.w    d0,a1                  | [1678]
	ext.l    %d6
	adda.l   %d6,%a1
|***	RESTORE	d4/d6	***
	movem.l  12(%a7),%d4
	movem.l  20(%a7),%d6
      jbra     strtflip                | [1679]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1681]
      move.w   %d2,%d3                 | [1682]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1684]
      moveq.l  #0,%d6                  | [1685]
      moveq.l  #15,%d4                 | [1686]
flip_lp:
|***	SAVE	d4	***
	movem.l  %d4,12(%a7)
|;;   lsr.w     d5,d0                  | [1688 X]
	mvz.w    %d0,%d1
	lsr.l    %d5,%d1
|;;   roxl.w    d5,d6                  | [1689 X]
	move.w   %d1,%d0
	move.b   %d5,%d1
	move.w   %d6,%d2
	moveq    #16,%d4
	and.l    #0x1f,%d1
	jbeq     _PA_1688_
	move.w   %d2,%d3
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d3,%d2
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d2
	_PA_1688_:         
	addx.l   %d3,%d3
	swap     %d3
	move.w   %d2,%d3
	lsl.l    %d4,%d3
	move.w   %d2,%d6
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
|;;   dbra      d4,flip_lp             | [1690]
	move     %CCR,%d1
	tst.w    %d4
	jbeq     _PA_1689_
	subq.l   #1,%d4
	jbra     flip_lp
	_PA_1689_:         
	move     %d1,%CCR

|;;   move.w    d6,-(a1)               | [1692]
	move.w   %d2,-(%a1)
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1693]
	move     %CCR,%d1
	tst.w    %d3
	jbeq     _PA_1692_
	subq.l   #1,%d3
	move     %d1,%CCR
	jbra     line_lp
	_PA_1692_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1695]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1697]
	move     %CCR,%d7
	tst.w    %d1
	jbeq     _PA_1696_
	subq.l   #1,%d1
	move     %d7,%CCR
	jbra     upsd_lp
	_PA_1696_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1699]
      movea.l  _scrtchp,%a1            | [1700]
|;;   adda.w    -4(a6),a1              | [1701]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1702]
      move.w   _SOURCEX,%d0            | [1703]
|;;   add.w     -102(a6),d0            | [1704]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d5
	add.l    %d5,%d7
|;;   neg.w     d0                     | [1705]
	neg.l    %d7
|;;   andi.w    #0xF,d0                | [1706]
	andi.l   #(0xFFFF&0xF),%d7
|;;   move.w    d0,_SOURCEX            | [1707]
	move.w   %d7,_SOURCEX
      clr.w    _SOURCEY                | [1708]
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      rts                              | [1709]





replicat:
|***	SAVE	d6-d7	***
	lea      -16(%a7),%a7
	movem.l  %d6-%d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1717]
      move.w   %d0,%d4                 | [1718]
|;;   andi.w    #0x0F,d0               | [1719]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1720]
|;;   lsr.w     #4,d4                  | [1721]
	mvz.w    %d4,%d6
	lsr.l    #4,%d6
|;;   lsl.w     #1,d4                  | [1722]
	lsl.l    #1,%d6

      move.w   _SOURCEY,%d0            | [1724]
      mulu.w   (-16,%a6),%d0           | [1725]
|;;   add.w     d4,d0                  | [1726]
	move.w   %d0,%d7
	add.l    %d6,%d7
	move.w   %d7,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1727]
|;;   adda.w    d0,a0                  | [1728]
	mvs.w    %d0,%d3
	adda.l   %d3,%a0

      move.w   (-24,%a6),%d2           | [1730]
      move.w   #0x8000,%d3             | [1731]
      move.w   %d3,%d4                 | [1732]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1733]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1735]
      move.w   (-102,%a6),%d1          | [1736]

      move.w   %d2,(-22,%a6)           | [1738]
      move.w   %d1,(-20,%a6)           | [1739]
      movea.l  _scrtchp,%a1            | [1740]
|;;   adda.w    _scrpt2,a1             | [1741]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1743 EQ]
      jbeq     noline                  | [1744]



|;;   addq.w    #1,-106(a6)            | [1748]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1749]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1751]
	move.w   %d1,%d0
	lsl.l    #1,%d0
|;;   addq.w    #3,d1                  | [1752]
	addq.l   #3,%d0
|;;   lsr.w     #4,d1                  | [1753]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d1                  | [1754]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1755]
	addq.l   #2,%d0
|;;   move.w    d1,d5                  | [1756]
	move.w   %d0,%d5
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      moveq.l  #0,%d6                  | [1757]
	move.w   %d0,%d1
oklear:
      move.w   %d6,(%a1)+              | [1759]
|;;   dbra      d5,oklear              | [1760]
	tst.w    %d5
	jbeq     _PA_1759_
	subq.l   #1,%d5
	jbra     oklear
	_PA_1759_:         
      lea      (-2,%a1),%a2            | [1761]
      move.w   %d2,%d5                 | [1762]
|;;   lsl.w     #1,d5                  | [1763]
	move.w   %d5,%d0
	lsl.l    #1,%d0
	move.w   %d0,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1765]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1766]
|;;   dbra      d5,oklear1             | [1767]
	tst.w    %d5
	jbeq     _PA_1766_
	subq.l   #1,%d5
	jbra     oklear1
	_PA_1766_:         

|;;   move.w    d1,d5                  | [1769]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1770]
	mvz.w    %d5,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d5                  | [1771 X]
	subq.l   #1,%d0
	move.w   %d0,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1773]
|;;   dbra      d5,oklear3             | [1774]
	move     %CCR,%d0
	tst.w    %d5
	jbeq     _PA_1773_
	subq.l   #1,%d5
	move     %d0,%CCR
	jbra     oklear3
	_PA_1773_:         
	move     %d0,%CCR
      jbra     noline1                 | [1775]

noline:
|;;   lsr.w     #3,d1                  | [1778]
	mvz.w    %d1,%d0
	lsr.l    #3,%d0
|;;   lsl.w     #1,d1                  | [1779]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1780 X]
	ori.l    #0xffff0000,%d0
	addq.l   #2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
noline1:
      move.w   %d1,(-18,%a6)           | [1782]
|***	RESTORE	AFTER d7	***
	movem.l  12(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1786]
|;;   roxr.l    #1,d7                  | [1787 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1788]
|;;   roxr.l    #1,d7                  | [1789]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1791]
      move.w   _DDA_INC,%d2            | [1792]
      move.w   (-22,%a6),%d5           | [1793]
|;;   subq.w    #1,d5                  | [1794]
	move.w   %d5,%d0
	subq.l   #1,%d0
      move.w   #32767,%d6              | [1795]
	move.w   %d0,%d5
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      16(%a7),%a7
      tst.l    %d7                     | [1796 MI]
      bmi.b    rep_ylop                | [1797]
y_dwn_lp:
|***	SAVE	d0-d2	***
	lea      -12(%a7),%a7
	movem.l  %d0-%d2,(%a7)
|;;   add.w     d2,d6                  | [1799 CC]
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
      jbcc     y_no_drw                | [1800]
      jbsr     yloop                   | [1801]
y_no_drw:
|;;   adda.w    d7,a0                  | [1803]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  8(%a7),%d2
	lea      12(%a7),%a7
|;;   dbra      d5,y_dwn_lp            | [1804]
	tst.w    %d5
	jbeq     _PA_1803_
	subq.l   #1,%d5
	jbra     y_dwn_lp
	_PA_1803_:         
	move.w   #-1,%d5
      jbra     y_rep_don               | [1805]
rep_ylop:
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
|;;   add.w     d2,d6                  | [1807 CC]
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
      jbcc     y_no_rep                | [1808]
      jbsr     yloop                   | [1809]
y_no_rep:
      jbsr     yloop                   | [1811]
|;;   adda.w    d7,a0                  | [1812]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  4(%a7),%d2
	lea      8(%a7),%a7
|;;   dbra      d5,rep_ylop            | [1813]
	tst.w    %d5
	jbeq     _PA_1812_
	subq.l   #1,%d5
	jbra     rep_ylop
	_PA_1812_:         
	move.w   #-1,%d5
y_rep_don:
|***	SAVE	d4-d6	***
	lea      -12(%a7),%a7
	movem.l  %d4-%d6,(%a7)
      move.w   (-102,%a6),%d2          | [1815]
      move.w   _XACC_DDA,%d1           | [1816]
      move.w   _DDA_INC,%d0            | [1817]
      moveq.l  #0,%d3                  | [1818]
repwidcl:
|;;   add.w     d0,d1                  | [1820 CC]
	mvz.w    %d0,%d4
	move.w   %d1,%d5
	ori.l    #0xffff0000,%d5
	add.l    %d4,%d5
	move     %ccr,%d6
	move.w   %d5,%d1
	move     %d6,%ccr
      jbcc     nrepdoub                | [1821]
|;;   addq.w    #1,d3                  | [1822]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrepdoub:
      tst.l    %d7                     | [1824 PL]
      bpl.b    nrpndoub                | [1825]
|;;   addq.w    #1,d3                  | [1826]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1828]
	tst.w    %d2
	jbeq     _PA_1827_
	subq.l   #1,%d2
	jbra     repwidcl
	_PA_1827_:         
      move.w   %d1,_XACC_DDA           | [1829]
      move.w   %d3,(-102,%a6)          | [1830]
      move.w   (-92,%a6),(-106,%a6)    | [1831]


      move.w   (-18,%a6),%d1           | [1834]
      move.w   %d1,(-16,%a6)           | [1835]







repexit1:
      btst     #4,(-96+1,%a6)          | [1844 EQ]
      jbeq     repexit2                | [1845]
      movea.l  _scrtchp,%a0            | [1846]
|;;   adda.w    _scrpt2,a0             | [1847]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1848]
|;;   lea       2(a0,d7.w),a1          | [1849]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
|***	RESTORE	AFTER d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbsr     outlin1                 | [1850]
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
      move.w   _scrpt2,%d0             | [1851]
|;;   add.w     -18(a6),d0             | [1852]
	move.w   %d0,%d2
	move.w   (-18,%a6),%d1
	add.l    %d1,%d2
|;;   move.w    #15,_SOURCEX           | [1853]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
	move.w   %d2,%d0
|***	RESTORE	d1-d2	***
	movem.l  (%a7),%d1-%d2
	lea      8(%a7),%a7
      jbra     repexit3                | [1854]
repexit2:
      move.w   _scrpt2,%d0             | [1856]
|***	RESTORE	d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbra     repexit                 | [1857]
| 2405 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1860]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1861]
      movea.l  %a1,%a4                 | [1862]
      movea.l  %d7,%a5                 | [1863]
      move.w   %d5,(-44,%a6)           | [1864]
      moveq.l  #0,%d0                  | [1865]
      move.w   (-20,%a6),%d5           | [1866]
|;;   subq.w    #1,d5                  | [1867]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1869]
      jbra     nextsrc                 | [1870]
innerlp:
|;;   ror.w     #1,d3                  | [1872 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jbcc     reploop                 | [1873]
nextsrc:
      movea.w  (%a0)+,%a2              | [1875]
reploop:
      move.w   %a2,%d6                 | [1877]
|;;   and.w     d3,d6                  | [1878 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jbne     nrepnor                 | [1879]


repnor:
      tst.l    %d7                     | [1883 MI]
      bmi.b    repnorup                | [1884]
|;;   add.w     d2,d7                  | [1885 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     incsrc                  | [1886]
      jbra     ordone                  | [1887]
repnorup:
|;;   add.w     d2,d7                  | [1889 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     ordone                  | [1890]


|;;   ror.w     #1,d4                  | [1893 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jbcc     ordone                  | [1896]
      move.w   %d0,(%a1)+              | [1897]
      moveq.l  #0,%d0                  | [1898]


      jbra     ordone                  | [1901]
nrepnor:


|;;   add.w     d2,d7                  | [1905 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     o_no_rep                | [1906]


|;;   or.w      d4,d0                  | [1909]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1910 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jbcc     o_no_rep                | [1912]
      move.w   %d0,(%a1)+              | [1913]
      moveq.l  #0,%d0                  | [1914]
o_no_rep:
      tst.l    %d7                     | [1916 PL]
      bpl.b    incsrc                  | [1917]


|;;   or.w      d4,d0                  | [1920]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1922 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jbcc     incsrc                  | [1923]
nextdst:
      move.w   %d0,(%a1)+              | [1925]
      moveq.l  #0,%d0                  | [1926]
incsrc:
|;;   dbra      d5,innerlp             | [1928]
	tst.w    %d5
	jbeq     _PA_1927_
	subq.l   #1,%d5
	jbra     innerlp
	_PA_1927_:         
repdone:
      move.w   %d0,(%a1)               | [1930]
|;;   movem.l   (sp)+,d3-d4/d6         | [1931]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1932]
      movea.l  %a4,%a1                 | [1933]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1934]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1935]
      move.w   (-44,%a6),%d5           | [1936]
      rts                              | [1937]

| 2494 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1942]
	move.w   %d7,%a5
	lea      (0,%a0,%a5.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1944]
	move.w   %d7,%a5
	lea      (0,%a1,%a5.l*1),%a2
|;;   lsr.w     #1,d7                  | [1945]
	mvz.w    %d7,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d7                  | [1946]
	subq.l   #1,%d0
|;;   move.w    d7,-44(a6)             | [1947]
	move.w   %d0,(-44,%a6)
      move.w   (-106,%a6),%d6          | [1948]

      movea.l  %a0,%a5                 | [1950]
|;;   subq.w    #1,d6                  | [1951]
	subq.l   #1,%d6
	move.w   %d0,%d7
out_edge:
      movea.w  %d6,%a4                 | [1953]
      movea.l  %a2,%a3                 | [1954]
      moveq.l  #0,%d5                  | [1955]
      moveq.l  #0,%d6                  | [1956]
      move.l   (%a2),%d1               | [1957]
      lsr.l    #1,%d1                  | [1958]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1963]
      move.b   %d5,%d0                 | [1964]
|;;   ror.l     #1,d0                  | [1965]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1967]
      move.b   %d6,%d2                 | [1968]
      move.l   %d2,%d3                 | [1969]
|;;   ror.l     #1,d3                  | [1970]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1971]
|;;   ror.l     #1,d4                  | [1972]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1974]
      move.l   %d0,%d6                 | [1975]
      eor.l    %d2,%d0                 | [1976]
      eor.l    %d3,%d5                 | [1977]
      eor.l    %d4,%d6                 | [1978]
|;;   rol.l     #1,d5                  | [1979]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1980]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1981]
      or.l     %d6,%d0                 | [1982]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1984]
      move.l   %d1,%d6                 | [1985]
      eor.l    %d2,%d1                 | [1986]
      eor.l    %d3,%d5                 | [1987]
      eor.l    %d4,%d6                 | [1988]
|;;   rol.l     #1,d5                  | [1989]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1990]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1991]
      or.l     %d5,%d0                 | [1992]
      or.l     %d6,%d0                 | [1993]

      eor.l    %d3,%d2                 | [1995]
      eor.l    %d3,%d4                 | [1996]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1997]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [1998]
      or.l     %d4,%d0                 | [1999]
      swap     %d0                     | [2000]

      move.w   (%a1),%d6               | [2002]
      move.w   %d6,%d5                 | [2003]
|;;   eor.w     d0,d5                  | [2004]
	move.w   %d5,%d3
	eor.l    %d0,%d3
|;;   and.w     d0,d5                  | [2005]
	and.l    %d0,%d3

      addq.l   #2,%a2                  | [2007]
      move.l   (%a2),%d1               | [2008]
      move.b   (-1,%a2),%d1            | [2009]
|;;   ror.l     #1,d1                  | [2010]
	move.w   %d3,%d5
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [2012]
      move.w   (%a0),%d5               | [2013]
      move.w   %d6,(%a0)+              | [2014]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [2016]
	tst.w    %d7
	jbeq     _PA_2015_
	subq.l   #1,%d7
	jbra     out_loop
	_PA_2015_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [2018]
      movea.l  %a5,%a0                 | [2019]
      move.w   %a4,%d6                 | [2020]
      movea.l  %a3,%a2                 | [2021]
      movea.l  %a2,%a1                 | [2022]
|;;   adda.w    -16(a6),a2             | [2023]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [2024 NE]
      jbne     srt_lin                 | [2025]
      movea.l  %a1,%a2                 | [2026]
srt_lin:
|;;   dbra      d6,out_edge            | [2028]
	tst.w    %d6
	jbeq     _PA_2027_
	subq.l   #1,%d6
	jbra     out_edge
	_PA_2027_:         
	move.w   #-1,%d6
      rts                              | [2029]
| 2605 "vdi/vdi_tblit.S"
concat:
|;;   movem.l   d2/d3,-(sp)            | [2032]
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)



      mulu.w   _v_lin_wr,%d1           | [2036]





      move.w   %d0,%d2                 | [2042]
|;;   andi.w    #0x000f,d0             | [2043]
	andi.l   #(0xFFFF0000!~0x000f),%d0
|;;   andi.w    #0xfff0,d2             | [2044]
	andi.l   #(0xFFFF&0xfff0),%d2
| 2627 "vdi/vdi_tblit.S"
      move.b   _shft_off,%d3           | [2046]
|;;   lsr.w     d3,d2                  | [2047]
	lsr.l    %d3,%d2






      ext.l    %d2                     | [2054]
      add.l    %d2,%d1                 | [2055]
|;;   movem.l   (sp)+,d2/d3            | [2056]
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      rts                              | [2057]




| End section on aligned boundary
	.p2align 2,0
