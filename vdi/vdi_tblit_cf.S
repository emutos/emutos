
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
| Translated on Tue Sep 16 16:53:01 2014 by version 2.6.0 of PortAsm translator
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
| 42 "vdi/vdi_tblit.S"
| 59 "vdi/vdi_tblit.S"
| 158 "vdi/vdi_tblit.S"
|;;           .globl _text_blt         | [18]
	.globl	_text_blt
|;;           .globl concat            | [19]
	.globl	concat





|;;           .extern _act_siz         | [25]
	.extern	_act_siz





|;;           .extern _SOURCEX,_SOURCEY | [31]
	.extern	_SOURCEX
	.extern	_SOURCEY
|;;           .extern _DESTX,_DESTY    | [32]
	.extern	_DESTX
	.extern	_DESTY
|;;           .extern _DELX,_DELY      | [33]
	.extern	_DELX
	.extern	_DELY
|;;           .extern _FBASE           | [34]
	.extern	_FBASE
|;;           .extern _FWIDTH          | [35]
	.extern	_FWIDTH
|;;           .extern _WRT_MODE        | [36]
	.extern	_WRT_MODE
|;;           .extern _STYLE           | [37]
	.extern	_STYLE
|;;           .extern _R_OFF,_L_OFF    | [38]
	.extern	_R_OFF
	.extern	_L_OFF
|;;           .extern _WEIGHT,_LITEMASK,_SKEWMASK | [39]
	.extern	_WEIGHT
	.extern	_LITEMASK
	.extern	_SKEWMASK
|;;           .extern _DOUBLE          | [40]
	.extern	_DOUBLE
|;;           .extern _CHUP            | [41]
	.extern	_CHUP
|;;           .extern _CLIP            | [42]
	.extern	_CLIP
|;;           .extern _MONO_STATUS     | [43]
	.extern	_MONO_STATUS
|;;           .extern _T_SCLSTS        | [44]
	.extern	_T_SCLSTS
|;;           .extern _TEXT_FG         | [45]
	.extern	_TEXT_FG
|;;           .extern _XMN_CLIP,_XMX_CLIP | [46]
	.extern	_XMN_CLIP
	.extern	_XMX_CLIP
|;;           .extern _YMN_CLIP,_YMX_CLIP | [47]
	.extern	_YMN_CLIP
	.extern	_YMX_CLIP
|;;           .extern _XACC_DDA        | [48]
	.extern	_XACC_DDA
|;;           .extern _DDA_INC         | [49]
	.extern	_DDA_INC
|;;           .extern _scrpt2          | [50]
	.extern	_scrpt2
|;;           .extern _scrtchp         | [51]
	.extern	_scrtchp
|;;           .extern _shift_offset    | [52]
	.extern	_shift_offset






|;;           .extern _v_bas_ad        | [59]
	.extern	_v_bas_ad
|;;           .extern _v_lin_wr        | [60]
	.extern	_v_lin_wr
|;;           .extern _v_planes        | [61]
	.extern	_v_planes





|;;           .globl _deftxbuf         | [67]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [68]
	.globl	_scrtsiz

| 327 "vdi/vdi_tblit.S"
| 344 "vdi/vdi_tblit.S"
|;;           .bss                     | [74]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [76]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [78]
	.p2align 2,0
	.text



_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [83]
	.short	(2+6+8+15)/16*2*16*2
| 460 "vdi/vdi_tblit.S"
|;;           .text                    | [85]




	.p2align 1,0
_text_blt:
      link.w   %a6,#(-126)             | [91]
|;;   movem.l   d2-d7/a2-a5,-(sp)      | [92]
	lea      -40(%a7),%a7
	movem.l  %d2-%d7/%a2-%a5,(%a7)

      clr.w    (-94,%a6)               | [94]
      move.w   #0,(-6,%a6)             | [95]
      lea      (-96,%a6),%a1           | [96]
      move.w   _STYLE,(%a1)            | [97]
      move.w   _WRT_MODE,-(%a1)        | [98]
      move.w   _SKEWMASK,-(%a1)        | [99]

      move.w   _DELX,-(%a1)            | [101]
      move.w   _DESTX,-(%a1)           | [102]
      move.w   _DELY,-(%a1)            | [103]
      move.w   _DESTY,-(%a1)           | [104]
      move.w   _CHUP,-(%a1)            | [105]

      movea.w  _scrpt2,%a0             | [107]
      move.w   (-106,%a6),%d3          | [108]
      tst.w    _DOUBLE                 | [109 NE]
      jbne     adr_dbl                 | [110]
      move.w   (-102,%a6),%d1          | [111]
      jbra     adr_nodb                | [112]



adr_dbl:
|;;   movem.l   a0/a1,-(sp)            | [117]
	lea      -8(%a7),%a7
	movem.l  %a0-%a1,(%a7)
      move.w   %d3,-(%a7)              | [118]
      move.l   (8,%a6),-(%a7)          | [119]
      jsr      _act_siz                | [120]
      addq.l   #6,%a7                  | [121]
|;;   movem.l   (sp)+,a0/a1            | [122]
	movem.l  (%a7),%a0-%a1
	lea      8(%a7),%a7
      move.w   %d0,%d3                 | [123]
      move.w   %d3,(-92,%a6)           | [124]
      move.w   (-102,%a6),%d2          | [125]
      move.w   _XACC_DDA,%d4           | [126]
      move.w   _DDA_INC,%d0            | [127]
      moveq.l  #0,%d1                  | [128]
      jbra     clc_n_su                | [129]
doub_wid:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   add.w     d0,d4                  | [131 CC]
	mvz.w    %d0,%d2
	move.w   %d4,%d3
	ori.l    #0xffff0000,%d3
	add.l    %d2,%d3
	move     %ccr,%d5
	move.w   %d3,%d4
	move     %d5,%ccr
      jbcc     no_db_wd                | [132]
|;;   addq.w    #1,d1                  | [133]
	move.w   %d1,%d2
	addq.l   #1,%d2
	move.w   %d2,%d1
no_db_wd:
|***	RESTORE	d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
|;;   btst.b    #0,_T_SCLSTS+1         | [135 EQ]
	moveq.l  #0,%d5
	btst     %d5,_T_SCLSTS+1
      jbeq     clc_n_su                | [136]
|;;   addq.w    #1,d1                  | [137]
	move.w   %d1,%d5
	addq.l   #1,%d5
	move.w   %d5,%d1
clc_n_su:
|;;   dbra      d2,doub_wid            | [139]
	tst.w    %d2
	jbeq     _PA_138_
	subq.l   #1,%d2
	jbra     doub_wid
	_PA_138_:         
      move.w   %d1,(-90,%a6)           | [140]
| 525 "vdi/vdi_tblit.S"
      tst.w    (-110,%a6)              | [142 NE]
      jbne     adr_nodb                | [143]



      movea.w  #0,%a0                  | [147]
adr_nodb:
      move.w   %a0,(-2,%a6)            | [149]
      move.w   (-108,%a6),%d2          | [150]
      move.w   (-104,%a6),%d0          | [151]
      move.w   _L_OFF,%d6              | [152]
      move.w   _R_OFF,%d7              | [153]
      clr.w    (-74,%a6)               | [154]
      btst     #0,(-96+1,%a6)          | [155 EQ]
      jbeq     adr_notk                | [156]
      move.w   _WEIGHT,%d4             | [157 NE]
      jbne     adr_th1                 | [158]
|;;   and.w     #0xFFFF-0x1,-96(a6)    | [159]
	move.w   (-96,%a6),%d5
	andi.l   #(0xFFFF&(0xFFFF-0x1)),%d5
	move.w   %d5,(-96,%a6)
adr_th1:
      tst.w    _MONO_STATUS            | [161 NE]
      jbne     adr_notk                | [162]

|;;   add.w     d4,d1                  | [164]
	move.w   %d1,%d5
	add.l    %d4,%d5
	move.w   %d5,%d1
adr_notk:
      btst     #2,(-96+1,%a6)          | [166 EQ]
      jbeq     chk_addr                | [167]

|;;   add.w     d6,d1                  | [169]
	move.w   %d1,%d5
	add.l    %d6,%d5
|;;   add.w     d7,d1                  | [170]
	add.l    %d7,%d5
	move.w   %d5,%d1







chk_addr:
      move.w   (-110,%a6),%d4          | [179 EQ]
      jbeq     chk_clip                | [180]
      move.w   #0,(-4,%a6)             | [181]
|;;   subi.w    #1800,d4               | [182]
	move.w   %d4,%d5
	subi.l   #1800,%d5
	move.w   %d5,%d4
      move.w   %d4,(-126,%a6)          | [183 EQ GT]
      jbeq     add180                  | [184]
      jbgt     add270                  | [185]



|;;   sub.w     d1,d2                  | [189]
	sub.l    %d1,%d2
      move.w   %d2,(-108,%a6)          | [190]
|;;   exg       d1,d3                  | [191]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3
      jbra     chk_clip                | [192]



add180:
|;;   sub.w     d1,d0                  | [197]
	move.w   %d0,%d5
	sub.l    %d1,%d5
|;;   move.w    d0,-104(a6)            | [198]
	move.w   %d5,(-104,%a6)
	move.w   %d5,%d0
      jbra     chk_clip                | [199]



add270:
|;;   exg       d1,d3                  | [204]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3

| 599 "vdi/vdi_tblit.S"
chk_clip:
      moveq.l  #0,%d5                  | [209]
      move.w   _CLIP,-(%a1)            | [210 EQ]
      jbeq     clp_done                | [211]
      move.w   _XMN_CLIP,-(%a1)        | [212]
      move.w   _YMN_CLIP,-(%a1)        | [213]
      move.w   _XMX_CLIP,-(%a1)        | [214]
      move.w   _YMX_CLIP,-(%a1)        | [215]



      cmp.w    (-116,%a6),%d2          | [219 GE]
      jbge     chk_ymx                 | [220]



|;;   add.w     d3,d2                  | [224]
	add.l    %d3,%d2
      cmp.w    (-116,%a6),%d2          | [225 GT]
      jbgt     chk_xmn                 | [226]
      jbra     upda_dst                | [227]
chk_ymx:
      cmp.w    (-120,%a6),%d2          | [229 GT]
      jbgt     upda_dst                | [230]

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   add.w     d3,d2                  | [232]
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [233]
	subq.l   #1,%d2
|;;   sub.w     -120(a6),d2            | [234 LE]
	ext.l    %d2
	mvs.w    (-120,%a6),%d0
	sub.l    %d0,%d2
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
	lea      4(%a7),%a7
      jble     chk_xmn                 | [235]



|;;   andi.w    #1,d2                  | [239 EQ]
	andi.l   #(0xFFFF&1),%d2
      jbeq     chk_xmn                 | [240]
      btst     #4,(-96+1,%a6)          | [241 NE]
      jbne     chk_xmn                 | [242]
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   rol.w     -100(a6)               | [243]
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
      cmp.w    (-114,%a6),%d0          | [248 GE]
      jbge     chk_xmx                 | [249]



|;;   add.w     d1,d0                  | [253]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   not.w     d5                     | [254]
	move.w   %d2,%d0
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5
      cmp.w    (-114,%a6),%d0          | [255 GT]
      jbgt     clp_done                | [256]
      jbra     upda_dst                | [257]
chk_xmx:
      cmp.w    (-118,%a6),%d0          | [259 GT]
      jbgt     upda_dst                | [260]

|;;   add.w     d1,d0                  | [262]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   subq.w    #1,d0                  | [263]
	subq.l   #1,%d2
	move.w   %d2,%d0
      cmp.w    (-118,%a6),%d0          | [264 LE]
      jble     clp_done                | [265]
|;;   not.w     d5                     | [266]
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5



clp_done:
      clr.w    (-28,%a6)               | [271]

|;;   move.w    _FWIDTH,-16(a6)        | [273]
	move.w   _FWIDTH,%d2
	move.w   %d2,(-16,%a6)
|;;   move.l    _FBASE,-10(a6)         | [274]
	move.l   _FBASE,%d2
	move.l   %d2,(-10,%a6)

chk_skw:
      move.w   (-96,%a6),%d1           | [277]
|;;   andi.w    #0x4 +0x1 +0x10,d1     | [278 EQ]
	andi.l   #(0xFFFF0000!~(0x4+0x1+0x10)),%d1
	tst.w    %d1
      jbeq     chk_rota                | [279]



chk_chup:
      tst.w    (-110,%a6)              | [284 NE]
      jbne     preblt                  | [285]

      btst     #2,%d1                  | [287 EQ]
      jbeq     preskew                 | [288]
      tst.w    %d5                     | [289 NE]
      jbne     preblt                  | [290]
preskew:
      btst     #4,%d1                  | [292 EQ]
      jbeq     chk_rota                | [293]






preblt:
|***	SAVE	d4	***
	lea      -4(%a7),%a7
	movem.l  %d4,(%a7)
      move.w   _SOURCEX,%d0            | [302]
      move.w   %d0,%d2                 | [303]
|;;   andi.w    #0x0F,d2               | [304]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [305]
|;;   lsr.w     #4,d0                  | [306]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [307]
	lsl.l    #1,%d0

      move.w   _SOURCEY,%d2            | [309]
      move.w   (-106,%a6),(-22,%a6)    | [310]
|;;   add.w     -22(a6),d2             | [311]
	move.w   (-22,%a6),%d3
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [312]
	subq.l   #1,%d2

      mulu.w   (-16,%a6),%d2           | [314]
|;;   neg.w     -16(a6)                | [315]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   add.w     d0,d2                  | [316]
	move.w   %d2,%d4
	add.l    %d0,%d4
      movea.l  (-10,%a6),%a0           | [317]
	move.w   %d4,%d2
|;;   lea       (a0,d2.w),a0           | [318]
	move.w   %d2,%a5
	lea      (0,%a0,%a5.l*1),%a0

      move.w   (-102,%a6),%d0          | [320]
      move.w   _WEIGHT,%d1             | [321]
|;;   add.w     d6,d7                  | [322]
	add.l    %d6,%d7
      tst.w    _DOUBLE                 | [323 EQ]
      jbeq     full_wt                 | [324]
|;;   lsr.w     #1,d1                  | [325 NE]
	mvz.w    %d1,%d4
	lsr.l    #1,%d4
	move     %ccr,%d3
	move.w   %d4,%d1
	move     %d3,%ccr
      jbne     ful_wtok                | [326]
|;;   addq.w    #1,d1                  | [327]
	addq.l   #1,%d4
	move.w   %d4,%d1
ful_wtok:
|;;   lsr.w     #1,d7                  | [329]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
full_wt:
      btst     #0,(-96+1,%a6)          | [331 EQ]
      jbeq     no_smear                | [332]

|;;   add.w     d1,d0                  | [334]
	add.l    %d1,%d0
      move.w   %d1,(-74,%a6)           | [335]
no_smear:

      clr.w    (-26,%a6)               | [338]
      move.w   (-106,%a6),%d1          | [339]
      move.w   (-96,%a6),%d2           | [340]
      btst     #4,%d2                  | [341 EQ]
      jbeq     no_out                  | [342]
      tst.w    _DOUBLE                 | [343 NE]
      jbne     no_out                  | [344]

|;;   addq.w    #3,d0                  | [346]
	addq.l   #3,%d0
|;;   addq.w    #1,-26(a6)             | [347]
	move.w   (-26,%a6),%d4
	addq.l   #1,%d4
	move.w   %d4,(-26,%a6)
|;;   addq.w    #2,-106(a6)            | [348]
	move.w   (-106,%a6),%d4
	addq.l   #2,%d4
	move.w   %d4,(-106,%a6)
|;;   addq.w    #3,d1                  | [349]
	move.w   %d1,%d4
	addq.l   #3,%d4
	move.w   %d4,%d1
no_out:

      move.w   %d0,(-20,%a6)           | [352]
|;;   add.w     d7,d0                  | [353]
	add.l    %d7,%d0
      move.w   %d0,(-102,%a6)          | [354]
|;;   lsr.w     #4,d0                  | [355]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [356]
	lsl.l    #1,%d0
|;;   addq.w    #2,d0                  | [357]
	addq.l   #2,%d0
|;;   neg.w     d0                     | [358]
	neg.l    %d0
      move.w   %d0,(-18,%a6)           | [359]
|;;   neg.w     d0                     | [360]
	neg.l    %d0
|;;   subq.w    #1,d1                  | [361]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [362]
	mulu.w   %d4,%d0
      movea.l  _scrtchp,%a1            | [363]
|;;   adda.w    -2(a6),a1              | [364]
	mvs.w    (-2,%a6),%d3
	adda.l   %d3,%a1
      move.l   %a1,(-10,%a6)           | [365]
      btst     #4,%d2                  | [366 NE]
      jbne     do_clear                | [367]
      btst     #2,%d2                  | [368 EQ]
      jbeq     no_clear                | [369]
do_clear:

      movea.l  %a1,%a2                 | [372]
      move.w   %d0,%d3                 | [373]
|;;   sub.w     -18(a6),d3             | [374]
	move.w   (-18,%a6),%d1
	sub.l    %d1,%d3
|;;   lsr.w     #1,d3                  | [375]
	and.l    #0xffff,%d3
	lsr.l    #1,%d3
|;;   subq.w    #1,d3                  | [376]
	subq.l   #1,%d3
      moveq.l  #0,%d1                  | [377]
replp:
      move.w   %d1,(%a2)+              | [379]
|;;   dbra      d3,replp               | [380]
	tst.w    %d3
	jbeq     _PA_379_
	subq.l   #1,%d3
	jbra     replp
	_PA_379_:         

      btst     #4,%d2                  | [382 EQ]
      jbeq     no_clear                | [383]
      tst.w    _DOUBLE                 | [384 NE]
      jbne     no_clear                | [385]

|;;   subq.w    #3,-20(a6)             | [387]
	move.w   (-20,%a6),%d4
	subq.l   #3,%d4
	move.w   %d4,(-20,%a6)
|;;   subq.w    #1,-102(a6)            | [388]
	move.w   (-102,%a6),%d4
	subq.l   #1,%d4
	move.w   %d4,(-102,%a6)
|;;   add.w     -18(a6),d0             | [389]
	move.w   %d0,%d4
	move.w   (-18,%a6),%d3
	add.l    %d3,%d4
	move.w   %d4,%d0



no_clear:
|;;   adda.w    d0,a1                  | [394]
	mvs.w    %d0,%d3
	adda.l   %d3,%a1



      move.w   (-96,%a6),(-122,%a6)    | [398]

      clr.w    (-98,%a6)               | [400]
      move.w   #1,(-84,%a6)            | [401]
      clr.w    (-76,%a6)               | [402]

      move.w   #1,(-86,%a6)            | [404]
      move.w   #2,(-88,%a6)            | [405]
|;;   andi.w    #0x4 +0x1,-96(a6)      | [406]
	move.w   (-96,%a6),%d4
	andi.l   #(0xFFFF&(0x4+0x1)),%d4
	move.w   %d4,(-96,%a6)
|***	RESTORE	AFTER d4	***
	move.l   (%a7)+,%d4

      jbsr     norm_blt                | [408]

      move.w   (-122,%a6),(-96,%a6)    | [410]
|;;   move.w    _WRT_MODE,-98(a6)      | [411]
	move.w   _WRT_MODE,%d2
	move.w   %d2,(-98,%a6)
no_mode:
      move.w   (-18,%a6),%d7           | [413]
|;;   neg.w     d7                     | [414]
	move.w   %d7,%d2
	neg.l    %d2
|;;   move.w    d7,-16(a6)             | [415]
	move.w   %d2,(-16,%a6)
	move.w   %d2,%d7



      btst     #4,(-96+1,%a6)          | [419 EQ]
      jbeq     skip_out                | [420]
      tst.w    _DOUBLE                 | [421 NE]
      jbne     skip_out                | [422]

      movea.l  (-10,%a6),%a0           | [424]
      ext.l    %d7                     | [425]
      add.l    %d7,(-10,%a6)           | [426]
      jbsr     outlin                  | [427]
skip_out:



      clr.w    _SOURCEX                | [432]
      clr.w    _SOURCEY                | [433]


|;;   andi.w    #0xFFFF-(0x4 +0x1),-96(a6) | [436]
	move.w   (-96,%a6),%d2
	andi.l   #(0xFFFF&(0xFFFF-(0x4+0x1))),%d2
	move.w   %d2,(-96,%a6)



chk_rota:
      tst.w    (-110,%a6)              | [441 EQ]
      jbeq     chk_db                  | [442]

      jbsr     rotation                | [444]

chk_db:
      tst.w    _DOUBLE                 | [447 EQ]
      jbeq     do_clip                 | [448]

      jbsr     replicat                | [450]







do_clip:
      btst     #0,(-96+1,%a6)          | [460 EQ]
      jbeq     no_thik                 | [461]
      move.w   _WEIGHT,%d0             | [462]
      tst.w    _MONO_STATUS            | [463 NE]
      jbne     do_cl_nt                | [464]
|;;   add.w     d0,-102(a6)            | [465]
	move.w   (-102,%a6),%d2
	add.l    %d0,%d2
	move.w   %d2,(-102,%a6)
do_cl_nt:
      move.w   %d0,(-74,%a6)           | [467]
no_thik:
      tst.w    (-112,%a6)              | [469 EQ]
      jbeq     scrn_blt                | [470]

      move.w   (-108,%a6),%d0          | [472]
      cmp.w    (-116,%a6),%d0          | [473 GE]
      jbge     ymn_fine                | [474]
|;;   add.w     -106(a6),d0            | [475]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-116,%a6),%d0          | [476 LE]
      jble     upda_dst                | [477]
mn_clipy:
|;;   sub.w     -116(a6),d0            | [479]
	move.w   (-116,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-106,%a6),%d1          | [480]
|;;   move.w    d0,-106(a6)            | [481]
	move.w   %d7,(-106,%a6)
|;;   sub.w     d0,d1                  | [482]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEY            | [483]
	move.w   _SOURCEY,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEY
      move.w   (-116,%a6),%d0          | [484]
      move.w   %d0,(-108,%a6)          | [485]
ymn_fine:
      cmp.w    (-120,%a6),%d0          | [487 GT]
      jbgt     upda_dst                | [488]
mx_clipy:
|;;   add.w     -106(a6),d0            | [490]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
|;;   subq.w    #1,d0                  | [491]
	subq.l   #1,%d7
	move.w   %d7,%d0
      cmp.w    (-120,%a6),%d0          | [492 LE]
      jble     ymx_fine                | [493]

|;;   sub.w     -120(a6),d0            | [495]
	move.w   (-120,%a6),%d2
	sub.l    %d2,%d7
|;;   sub.w     d0,-106(a6)            | [496]
	move.w   (-106,%a6),%d2
	sub.l    %d7,%d2
	move.w   %d2,(-106,%a6)
ymx_fine:
      move.w   (-104,%a6),%d0          | [498]
      cmp.w    (-114,%a6),%d0          | [499 GE]
      jbge     xmn_fine                | [500]
|;;   add.w     -102(a6),d0            | [501]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-114,%a6),%d0          | [502 LE]
      jble     upda_dst                | [503]

mn_clipx:
|;;   sub.w     -114(a6),d0            | [506]
	move.w   (-114,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-102,%a6),%d1          | [507]
|;;   move.w    d0,-102(a6)            | [508]
	move.w   %d7,(-102,%a6)
|;;   sub.w     d0,d1                  | [509]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEX            | [510]
	move.w   _SOURCEX,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEX
      move.w   (-114,%a6),%d0          | [511]
      move.w   %d0,(-104,%a6)          | [512]
xmn_fine:
      cmp.w    (-118,%a6),%d0          | [514 GT]
      jbgt     upda_dst                | [515]
mx_clipx:
|;;   add.w     -102(a6),d0            | [517]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d2
	add.l    %d2,%d7
|;;   subq.w    #1,d0                  | [518]
	subq.l   #1,%d7
	move.w   %d7,%d0
      cmp.w    (-118,%a6),%d0          | [519 LE]
      jble     scrn_blt                | [520]

|;;   sub.w     -118(a6),d0            | [522]
	move.w   (-118,%a6),%d2
	sub.l    %d2,%d7
|;;   sub.w     d0,-102(a6)            | [523]
	move.w   (-102,%a6),%d2
	sub.l    %d7,%d2
	move.w   %d2,(-102,%a6)

      jbra     scrn_blt                | [525]
| 926 "vdi/vdi_tblit.S"
scrn_blt:
|***	SAVE	d3	***
	lea      -4(%a7),%a7
	movem.l  %d3,(%a7)
|;;   move.w    _TEXT_FG,-84(a6)       | [528]
	move.w   _TEXT_FG,%d2
	move.w   %d2,(-84,%a6)
      move.w   (-6,%a6),(-76,%a6)      | [529]
      move.w   _v_planes,%d0           | [530]
      move.w   %d0,(-86,%a6)           | [531]
|;;   add.w     d0,d0                  | [532]
	move.w   %d0,%d3
	add.l    %d0,%d3
|;;   move.w    d0,-88(a6)             | [533]
	move.w   %d3,(-88,%a6)

      move.w   _SOURCEX,%d0            | [535]
      move.w   %d0,%d2                 | [536]
|;;   andi.w    #0x0F,d2               | [537]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [538]
|;;   lsr.w     #3,d0                  | [539]
	mvz.w    %d0,%d3
	lsr.l    #3,%d3
|;;   andi.w    #0xFFFE,d0             | [540]
	andi.l   #(0xFFFF&0xFFFE),%d3
      move.w   _SOURCEY,%d2            | [541]

      move.w   (-106,%a6),(-22,%a6)    | [543]
|;;   add.w     -22(a6),d2             | [544]
	move.w   (-22,%a6),%d7
	add.l    %d7,%d2
|;;   subq.w    #1,d2                  | [545]
	subq.l   #1,%d2
      mulu.w   (-16,%a6),%d2           | [546]
|;;   neg.w     -16(a6)                | [547]
	move.w   %d3,%d0
	move.w   (-16,%a6),%d3
	neg.l    %d3
	move.w   %d3,(-16,%a6)
|;;   add.w     d0,d2                  | [548]
	move.w   %d2,%d3
	add.l    %d0,%d3



      movea.l  (-10,%a6),%a0           | [552]
|;;   adda.w    d2,a0                  | [553]
	ext.l    %d3
	adda.l   %d3,%a0
      move.l   %a0,(-10,%a6)           | [554]

      move.w   (-102,%a6),(-20,%a6)    | [556]

      move.w   (-108,%a6),%d1          | [558]
|;;   add.w     -106(a6),d1            | [559]
	move.w   %d1,%d3
	move.w   (-106,%a6),%d7
	add.l    %d7,%d3
	move.w   %d3,%d1
|***	RESTORE	d3	***
	move.l   (%a7)+,%d3
|;;   subq.w    #1,d1                  | [560 MI]
	move.w   %d1,%d7
	subq.l   #1,%d7
	move.w   %d7,%d1
      bmi.b    upda_dst                | [561]

      move.w   (-104,%a6),%d0          | [563 MI]
      bmi.b    upda_dst                | [564]

      jsr      concat                  | [566]



      movea.l  _v_bas_ad,%a1           | [570]
      adda.l   %d1,%a1                 | [571]

      move.l   %a1,(-14,%a6)           | [573]
      move.w   %d0,(-26,%a6)           | [574]

      move.w   _v_lin_wr,%d0           | [576]
|;;   neg.w     d0                     | [577]
	move.w   %d0,%d3
	neg.l    %d3
|;;   move.w    d0,-18(a6)             | [578]
	move.w   %d3,(-18,%a6)

      jbsr     norm_blt                | [580]

upda_dst:
      move.w   _DELX,%d1               | [583]


      tst.w    _DOUBLE                 | [586 EQ]
      jbeq     upda_ndb                | [587]



      move.w   (-90,%a6),%d1           | [591]
      move.w   (-92,%a6),%d3           | [592]

      tst.w    (-94,%a6)               | [594 EQ]
      jbeq     upda_ndb                | [595]
|;;   exg       d1, d3                 | [596]
	move.l   %d1,%d2
	move.l   %d3,%d1
	move.l   %d2,%d3





upda_ndb:
|;;   btst.b    #0,_STYLE+1            | [603 EQ]
	moveq.l  #0,%d2
	btst     %d2,_STYLE+1
      jbeq     upda_ntk                | [604]
      tst.w    _MONO_STATUS            | [605 NE]
      jbne     upda_ntk                | [606]
|;;   add.w     _WEIGHT,d1             | [607]
	move.w   _WEIGHT,%d2
	add.l    %d2,%d1
upda_ntk:
      tst.w    (-110,%a6)              | [609 GT]
      jbgt     ck90                    | [610]
|;;   add.w     d1,_DESTX              | [611]
	move.w   _DESTX,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [612]
ck90:
      tst.w    (-126,%a6)              | [614 EQ GT]
      jbeq     ck180                   | [615]
      jbgt     ck270                   | [616]
|;;   sub.w     d1,_DESTY              | [617]
	move.w   _DESTY,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTY
      jbra     blt_done                | [618]
ck180:
|;;   sub.w     d1,_DESTX              | [620]
	move.w   _DESTX,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [621]
ck270:
|;;   add.w     d1,_DESTY              | [623]
	move.w   _DESTY,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTY
blt_done:
|;;   movem.l   (sp)+,d2-d7/a2-a5      | [625]
	movem.l  (%a7),%d2-%d7/%a2-%a5
	lea      40(%a7),%a7
      unlk     %a6                     | [626]
      rts                              | [627]



|;;           .text                    | [632]
| 1040 "vdi/vdi_tblit.S"
|;;   lf_tab: .dc.w 0xFFFF             | [634]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [635]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [636]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [637]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [638]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [639]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [640]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [641]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [642]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [643]
	.short	0x007F
|;;                   .dc.w 0x003F     | [644]
	.short	0x003F
|;;                   .dc.w 0x001F     | [645]
	.short	0x001F
|;;                   .dc.w 0x000F     | [646]
	.short	0x000F
|;;                   .dc.w 0x0007     | [647]
	.short	0x0007
|;;                   .dc.w 0x0003     | [648]
	.short	0x0003
|;;                   .dc.w 0x0001     | [649]
	.short	0x0001
|;;                   .dc.w 0x0000     | [650]
	.short	0x0000

|;;           .text                    | [652]
| 1071 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [655]
|;;   sub.w     -24(a6),d1             | [656]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [657 PL]
      bpl.b    do_rot                  | [658]



|;;   neg.w     d1                     | [662]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [663]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [664]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [666]

|;;   lsl.w     #1,d0                  | [668]
	move.w   %d0,%d5
	lsl.l    #1,%d5
      lea      lf_tab,%a2              | [669]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [670]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [671]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [672]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [677]
|;;   lsl.w     #1,d0                  | [678]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [679]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [681]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [682]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [685]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [686]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [687]
	andi.l   #(0xFFFF&0xF),%d5
|;;   lsl.w     #1,d2                  | [688]
	lsl.l    #1,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [689]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [690]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [691]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [692]
      move.w   #0x8000,(-80,%a6)       | [693]
      moveq.l  #(-1),%d3               | [694]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [697 HI]
      bhi.b    doub_des                | [698]



|;;   lsl.w     #1,d0                  | [702]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [703]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [704]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [705]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [707]
	move.w   %d5,%d1
      jbra     msk_done                | [708]



doub_des:
      move.w   %d0,%d1                 | [713]
|;;   lsr.w     #4,d1                  | [714]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [715]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [716 NE]
      jbne     mlt_dest                | [717]



      move.w   (-24,%a6),%d1           | [721]
|;;   add.w     -20(a6),d1             | [722]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [723 CC]
      jbcc     mlt_dest                | [724]
      moveq.l  #0,%d7                  | [725]
      jbra     do_ritem                | [726]

mlt_dest:
      tst.w    (-42,%a6)               | [729 LT]
      jblt     mltleft                 | [730]
      moveq.l  #4,%d7                  | [731]
      jbra     do_ritem                | [732]
mltleft:
      moveq.l  #8,%d7                  | [734]
do_ritem:
|;;   andi.w    #0x000F,d0             | [736 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jbne     not_null                | [737]
|;;   subq.w    #1,d3                  | [738]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [739]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [741]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [742]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [743]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [746]
      move.w   %d3,(-28,%a6)           | [747]
      move.w   %d4,(-36,%a6)           | [748]
|;;   addq.w    #2,d3                  | [749]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [750 CS]
      jbcs     msk0                    | [751]
|;;   addq.w    #1,d3                  | [752]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [754]
      move.w   %d3,(-30,%a6)           | [755]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jbra     plane_loop              | [756]

| 1191 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [761]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [762]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [763]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [764]
	.byte	4*5,4*5,4*6,4*6


toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [768]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [770]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [774]
|;;   lsr.w     -84(a6)                | [775 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [776]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [777 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [778]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [782]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [783]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [784]
      movea.l  %a2,%a5                 | [785]

|;;   movea.l   twoptble(pc,d0.w),a4   | [787]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [791 EQ]
      jbeq     wrt_char                | [792]

      btst     #1,%d0                  | [794 EQ]
      jbeq     no_lite                 | [795]



|;;   move.w    _LITEMASK,-78(a6)      | [799]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [800]
      lea      liteop,%a5              | [801]

      move.l   %a2,(-64,%a6)           | [803]
      lea      liteopwf,%a2            | [804]

      move.l   %a4,(-72,%a6)           | [806]
      lea      liteopw,%a4             | [807]

no_lite:
      btst     #0,%d0                  | [810 EQ]
      jbeq     no_thick                | [811]



      clr.w    (-82,%a6)               | [815]

      move.l   %a5,(-48,%a6)           | [817]
      lea      thknop,%a5              | [818]

      move.l   %a2,(-60,%a6)           | [820]
      lea      thknopwf,%a2            | [821]

      move.l   %a4,(-68,%a6)           | [823]
      lea      thknopw,%a4             | [824]

no_thick:
      btst     #2,%d0                  | [827 EQ]
      jbeq     wrt_char                | [828]



      move.w   (-100,%a6),(-80,%a6)    | [832]
      move.l   %a5,(-56,%a6)           | [833]
      lea      skewop,%a5              | [834]

      tst.w    (-124,%a6)              | [836 EQ GT]
      jbgt     wrt_char                | [837]
      jbeq     not_sngl                | [838]
      clr.w    (-28,%a6)               | [839]
      moveq.l  #0,%d7                  | [840]
      jbra     chng_blt                | [841]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [843 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      bls.b    wrt_char                | [844]

      tst.w    (-42,%a6)               | [846 LT]
      jblt     chng_left               | [847]
      moveq.l  #4,%d7                  | [848]
      jbra     chng_blt                | [849]
chng_left:
      moveq.l  #8,%d7                  | [851]
chng_blt:
      move.w   %d7,(-124,%a6)          | [853]

wrt_char:
      move.w   (-124,%a6),%d7          | [856 LE]
      jble     set_chr_hgt             | [857]
      movea.l  %a2,%a5                 | [858]
      move.w   (-18,%a6),%d5           | [859]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [861]
|;;   subq.w    #1,d3                  | [862]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [864]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [865]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [867 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jble     no_more_planes          | [868]
      movea.l  (-10,%a6),%a0           | [869]
      movea.l  (-14,%a6),%a1           | [870]
      addq.l   #2,%a1                  | [871]
      move.l   %a1,(-14,%a6)           | [872]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [873 EQ]
      jbeq     plane_loop              | [874]
      jbra     norm_blt                | [875]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [877]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [879]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [886]
      move.w   (-16,%a6),%d6           | [887]
      move.w   (-18,%a6),%d7           | [888]
      lea      sgl_rtn,%a3             | [889]
      jbra     sgl_1st_scan            | [890]

sgl_lp:
|;;   adda.w    d6,a0                  | [893]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [894]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [896]
| 1336 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [898]


      move.w   (-42,%a6),%d1           | [901 MI]
      bmi.b    left_rol                | [902]
      lsr.l    %d1,%d0                 | [903]
      jbra     end_ro                  | [904]
left_rol:
      lsl.l    %d1,%d0                 | [906]
end_ro:
      swap     %d0                     | [908]
      move.w   %d0,%d1                 | [909]
      swap     %d0                     | [910]

      jmp      (%a5)                   | [912]

sgl_rtn:
      move.w   %d1,(%a1)               | [915]
|;;   dbra      d3,sgl_lp              | [916]
	tst.w    %d3
	jbeq     _PA_915_
	subq.l   #1,%d3
	jbra     sgl_lp
	_PA_915_:         
      jbra     next_plane              | [917]




dbl_loop:
      movea.l  %a3,%a4                 | [924]
      move.w   (-16,%a6),%d6           | [925]
      move.w   (-18,%a6),%d7           | [926]
      lea      dbl_2wrd,%a2            | [927]
      move.l   %a2,%d5                 | [928]
      lea      dbl_1wrd,%a3            | [929]
      jbra     dbl_1st_scan            | [930]

dbl_lp:
|;;   adda.w    d6,a0                  | [933]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [934]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [935]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [937]
| 1384 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [939]


      move.w   (-42,%a6),%d1           | [942 MI]
      bmi.b    dbleft                  | [943]
      lsr.l    %d1,%d0                 | [944]
      jbra     dblendr                 | [945]
dbleft:
      lsl.l    %d1,%d0                 | [947]
dblendr:
      swap     %d0                     | [949]
      move.w   %d0,%d1                 | [950]
      swap     %d0                     | [951]
      move.w   (-38,%a6),%d2           | [952]

      jmp      (%a5)                   | [954]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [957]
      move.w   %d0,%d1                 | [958]

      movea.w  (-88,%a6),%a2           | [960]
|;;   move.w    (a1,a2.w),d4           | [961]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [963]

|;;   exg       a3,d5                  | [965]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [966]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [969]
|;;   move.w    d1,(a1,a2.w)           | [970]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [972]
	tst.w    %d3
	jbeq     _PA_971_
	subq.l   #1,%d3
	jbra     dbl_lp
	_PA_971_:         
      jbra     next_plane              | [973]





left_loop:
      movea.l  %d6,%a0                 | [981]
      movea.l  %d7,%a1                 | [982]
|;;   adda.w    -16(a6),a0             | [983]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [984]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [985]
|;;   rol.w     -78(a6)                | [986]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [988 NE]
      jbne     skewopw                 | [989]
mlt_left:
      move.l   %a0,%d6                 | [991]
      move.l   %a1,%d7                 | [992]
      move.w   (-28,%a6),(-44,%a6)     | [993]







      move.l   (%a0)+,%d0              | [1001]


      move.w   (%a1),%d4               | [1004]

      move.w   (-42,%a6),%d1           | [1006]
      lsl.l    %d1,%d0                 | [1007]

      swap     %d0                     | [1009]
      move.w   %d0,%d1                 | [1010]
      swap     %d0                     | [1011]

      move.w   (-38,%a6),%d2           | [1013]

      lea      lft_lfrng,%a3           | [1015]
      jmp      (%a5)                   | [1016]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1019]
|;;   adda.w    -88(a6),a1             | [1020]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1022]
      lea      lft_wrds,%a3            | [1023]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1028]
|;;   and.w     d2,d4                  | [1029]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [1030]





      swap     %d0                     | [1036]
      move.w   %d1,%d0                 | [1037]
      swap     %d0                     | [1038]

      move.w   (-42,%a6),%d1           | [1040]
      lsl.l    %d1,%d0                 | [1041]

      swap     %d0                     | [1043]
      move.w   %d0,%d1                 | [1044]
      swap     %d0                     | [1045]

|;;   not.w     d2                     | [1047]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1048]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1049]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1050]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1052]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1054 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     lef_don                 | [1055]

      jmp      (%a4)                   | [1057]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1060]
|;;   adda.w    -88(a6),a1             | [1061]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_lef                | [1063]

lef_don:
      move.w   (-36,%a6),%d2           | [1066]

      lea      lft_rfrng,%a3           | [1068]
      jmp      (%a5)                   | [1069]

lft_rfrng:
      move.w   %d1,(%a1)               | [1072]

|;;   dbra      d3,left_loop           | [1074]
	tst.w    %d3
	jbeq     _PA_1073_
	subq.l   #1,%d3
	jbra     left_loop
	_PA_1073_:         
      jbra     next_plane              | [1075]






rite_loop:
      movea.l  %d6,%a0                 | [1084]
      movea.l  %d7,%a1                 | [1085]
|;;   adda.w    -16(a6),a0             | [1086]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [1087]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [1088]
|;;   rol.w     -78(a6)                | [1089]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [1091 NE]
      jbne     skewopw                 | [1092]
mlt_rite:
      move.l   %a0,%d6                 | [1094]
      move.l   %a1,%d7                 | [1095]
      move.w   (-28,%a6),(-44,%a6)     | [1096]
      move.w   (%a0)+,%d0              | [1097]



      swap     %d0                     | [1101]

      move.w   (%a1),%d4               | [1103]

      move.w   (-42,%a6),%d1           | [1105]
      lsr.l    %d1,%d0                 | [1106]
      swap     %d0                     | [1107]
      move.w   %d0,%d1                 | [1108]
      swap     %d0                     | [1109]

      move.w   (-38,%a6),%d2           | [1111]

      lea      rgt_lfrng,%a3           | [1113]
      jmp      (%a5)                   | [1114]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1117]
|;;   adda.w    -88(a6),a1             | [1118]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1120]
      lea      rgt_wrds,%a3            | [1121]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1126]
|;;   and.w     d2,d4                  | [1127]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [1128]
      move.w   (%a0)+,%d0              | [1129]



      swap     %d0                     | [1133]

      move.w   (-42,%a6),%d1           | [1135]
      lsr.l    %d1,%d0                 | [1136]
      swap     %d0                     | [1137]
      move.w   %d0,%d1                 | [1138]
      swap     %d0                     | [1139]

|;;   not.w     d2                     | [1141]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1142]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1143]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1144]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1146]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1148 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     rite_don                | [1149]

      jmp      (%a4)                   | [1151]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1154]
|;;   adda.w    -88(a6),a1             | [1155]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_rit                | [1157]

rite_don:
      move.w   (-36,%a6),%d2           | [1160]

      lea      rgt_rfrng,%a3           | [1162]
      jmp      (%a5)                   | [1163]

rgt_rfrng:
      move.w   %d1,(%a1)               | [1166]

|;;   dbra      d3,rite_loop           | [1168]
	tst.w    %d3
	jbeq     _PA_1167_
	subq.l   #1,%d3
	jbra     rite_loop
	_PA_1167_:         
      jbra     next_plane              | [1169]






top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1178]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [1179]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [1180]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [1181]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1182]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1185]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [1186]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [1187]
	not.l    %d0
|;;   and.w     d4,d1                  | [1188]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1189]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1192]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [1193]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1194]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1195]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1198]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [1199]
	not.l    %d0
|;;   and.w     d4,d1                  | [1200]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1201]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1204]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1205]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1206]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1209]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1210]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1211]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1214]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [1215]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1216]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1217]





twop0:
      moveq.l  #0,%d1                  | [1225]
      jmp      (%a3)                   | [1226]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [1229]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1230]

twop3:
      jmp      (%a3)                   | [1233]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1236]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [1237]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1238]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1241]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1242]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [1245]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1246]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1249]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [1250]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1251]
| 1716 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [1254]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [1255]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [1256]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [1257 EQ]
      jbeq     thk00                   | [1258]
|;;   ori.b     #0x10,ccr              | [1259 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [1260 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jbcc     thk01                   | [1261]
      move.w   #0x8000,%d6             | [1262]
thk01:
      move.w   %d6,(-34,%a6)           | [1264]
thk00:
      tst.w    (-28,%a6)               | [1266 MI]
      bmi.b    thk0                    | [1267]
      cmp.w    (-36,%a6),%d6           | [1268 CC]
      jbcc     thk0                    | [1269]
|;;   and.w     d6,d0                  | [1270]
	and.l    %d6,%d0
      jbra     thk1                    | [1271]
thk0:
      clr.w    %d0                     | [1273]
|;;   and.w     d6,d1                  | [1274]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [1276]
      move.w   %d2,%d5                 | [1277]
      swap     %d5                     | [1278]
      move.w   (-36,%a6),%d5           | [1279]
      lsl.l    %d6,%d5                 | [1280]

      swap     %d5                     | [1282]
      move.w   %d5,%d7                 | [1283]
      swap     %d5                     | [1284]

|;;   and.w     d7,d1                  | [1286]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [1287]
	and.l    %d5,%d0
|;;   swap      d1                     | [1288]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [1289]
|;;   subq.w    #1,d6                  | [1290]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [1292]
      lsr.l    #1,%d0                  | [1293]
      or.l     %d0,%d1                 | [1294]
|;;   dbra      d6,thkoplp             | [1295]
	tst.w    %d6
	jbeq     _PA_1294_
	subq.l   #1,%d6
	jbra     thkoplp
	_PA_1294_:         

      move.w   %d1,%d0                 | [1297]
      swap     %d1                     | [1298]
|;;   movem.l   (sp)+,d5-d7            | [1299]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [1300]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [1301]

| 1780 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [1306]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [1307 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk2                    | [1308]
|;;   and.w     -34(a6),d1             | [1309]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [1311]
      swap     %d1                     | [1312]
      clr.w    %d1                     | [1313]
      move.l   %d1,%d5                 | [1314]
|;;   subq.w    #1,d6                  | [1315]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [1317]
      or.l     %d5,%d1                 | [1318]
|;;   dbra      d6,thkopwlp            | [1319]
	tst.w    %d6
	jbeq     _PA_1318_
	subq.l   #1,%d6
	jbra     thkopwlp
	_PA_1318_:         

      move.w   %d1,%d5                 | [1321]
      swap     %d1                     | [1322]
|;;   or.w      -82(a6),d1             | [1323]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1324]
|;;   movem.l   (sp)+,d5-d6            | [1325]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [1326]
      jmp      (%a2)                   | [1327]

| 1819 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [1332]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [1333]
|;;   subq.w    #1,-30(a6)             | [1334 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      bmi.b    thk3                    | [1335]
      jbeq     thk4                    | [1336]

|;;   subq.w    #1,-30(a6)             | [1338 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk11                   | [1339]
|;;   and.w     d6,d1                  | [1340]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [1342]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jbra     thk7                    | [1343]

thk3:
      clr.w    %d1                     | [1346]
      jbra     thk5                    | [1347]

thk4:
|;;   and.w     d6,d1                  | [1350]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [1353 MI]
      bmi.b    thk6                    | [1354]
      move.w   (-28,%a6),%d5           | [1355]
|;;   addq.w    #2,d5                  | [1356]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [1357]
|;;   ori.b     #0x10,ccr              | [1358 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [1359 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jbcc     thk33                   | [1360]
      move.w   #0x8000,%d7             | [1361]
|;;   addq.w    #1,d5                  | [1362]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [1364 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jbne     thk34                   | [1365]
|;;   subq.w    #1,d5                  | [1366]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [1368 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [1369 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jbcc     thk31                   | [1370]
      move.w   #0x8000,%d6             | [1371]
thk31:
      cmp.w    %d6,%d7                 | [1373 CS]
      jbcs     thk35                   | [1374]
|;;   addq.w    #1,d5                  | [1375]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [1377]
      move.w   %d5,(-32,%a6)           | [1378]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [1380]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:

      move.w   (-74,%a6),%d6           | [1383]
      swap     %d1                     | [1384]
      clr.w    %d1                     | [1385]
      move.l   %d1,%d5                 | [1386]
|;;   subq.w    #1,d6                  | [1387]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [1389]
      or.l     %d5,%d1                 | [1390]
|;;   dbra      d6,thkopwl             | [1391]
	tst.w    %d6
	jbeq     _PA_1390_
	subq.l   #1,%d6
	jbra     thkopwl
	_PA_1390_:         

      move.w   %d1,%d5                 | [1393]
      swap     %d1                     | [1394]
|;;   or.w      -82(a6),d1             | [1395]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1396]
|;;   and.w     d2,d1                  | [1397]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [1398]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [1399]
      jmp      (%a2)                   | [1400]

| 1902 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [1405]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [1406]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [1407]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [1408]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1409]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1412]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [1413]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1414]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1417]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [1418]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1419]

| 1935 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [1424 CC]
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
      jbcc     no_shift                | [1425]

      swap     %d1                     | [1427]
      move.w   %d0,%d1                 | [1428]
      lsr.l    #1,%d1                  | [1429]

      swap     %d2                     | [1431]
      move.w   (-36,%a6),%d2           | [1432]
      lsr.l    #1,%d2                  | [1433]
      move.w   %d2,(-36,%a6)           | [1434]
      swap     %d2                     | [1435]
      move.w   %d2,(-38,%a6)           | [1436 EQ]
      jbeq     nxt_word                | [1437]
      move.w   (-42,%a6),%d0           | [1438 MI]
      bmi.b    dec_rol                 | [1439]
ror_add:
|;;   addq.w    #1,d0                  | [1441]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [1443]
      move.w   %d1,%d0                 | [1444]
      swap     %d1                     | [1445]
no_shift:
      movea.l  (-56,%a6),%a2           | [1447]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1448]


dec_rol:
      tst.b    %d0                     | [1452 EQ]
      jbeq     begn_ror                | [1453]
|;;   subq.w    #1,d0                  | [1454]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jbra     new_shif                | [1455]
begn_ror:
      clr.w    %d0                     | [1457]
      jbra     ror_add                 | [1458]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1463]
      swap     %d2                     | [1464]
      move.w   %d2,(-38,%a6)           | [1465]
|;;   adda.w    -88(a6),a1             | [1466]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1467]

      moveq.l  #15,%d0                 | [1469]
|;;   sub.w     -42(a6),d0             | [1470]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1471]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1472]
      jbra     no_shift                | [1473]






skewopw:
|;;   rol.w     -80(a6)                | [1482 CC]
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
      jbcc     do_shift                | [1483]

      tst.w    (-42,%a6)               | [1485 MI]
      bmi      mlt_left                | [1486]
      jbra     mlt_rite                | [1487]
do_shift:
|;;   ori       #0x10,ccr              | [1489 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1490]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1492]
      cmp.w    #0xFFFF,%d0             | [1493 EQ]
      jbeq     inc_rite                | [1494]

|;;   ori       #0x10,ccr              | [1496 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1497]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1498]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1501]
      tst.b    %d0                     | [1502 NE]
      jbne     no_rota                 | [1503]
      move.w   #0x8000,(-40,%a6)       | [1504]
no_rota:
      move.w   (-38,%a6),%d1           | [1506]
|;;   lsr.w     #1,d1                  | [1507 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jbeq     wnxt_wrd                | [1508]

      move.w   %d1,(-38,%a6)           | [1510]

      tst.w    %d0                     | [1512 MI]
      bmi.b    wdec_rol                | [1513]
|;;   addq.w    #1,-42(a6)             | [1514]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jbra     mlt_rite                | [1515]
wdec_rol:
      tst.b    %d0                     | [1517 EQ]
      jbeq     set_msk                 | [1518]
|;;   subq.w    #1,-42(a6)             | [1519]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jbra     mlt_left                | [1520]
set_msk:
      move.w   #1,(-42,%a6)            | [1522]
      jbra     mlt_rite                | [1523]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1526]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1527]
      jbra     do_left                 | [1528]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1531]

|;;   subq.w    #1,-28(a6)             | [1533]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1534]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1536]
|;;   sub.w     d0,d2                  | [1537]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1538]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1539]

      jbra     mlt_left                | [1541]




rotation:
|***	SAVE	d4/d6-d7	***
	lea      -28(%a7),%a7
	movem.l  %d4,12(%a7)
	movem.l  %d6-%d7,20(%a7)
      move.w   _SOURCEX,%d1            | [1548]
      move.w   %d1,%d2                 | [1549]
|;;   andi.w    #0x0F,d2               | [1550]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1551]
|;;   lsr.w     #4,d1                  | [1552]
	mvz.w    %d1,%d4
	lsr.l    #4,%d4
      moveq.l  #1,%d5                  | [1553]
|***	SAVE	d5	***
	movem.l  %d5,16(%a7)
|;;   lsl.w     d5,d1                  | [1554]
	lsl.l    %d5,%d4

      movea.l  (-10,%a6),%a0           | [1556]
|;;   adda.w    d1,a0                  | [1557]
	ext.l    %d4
	adda.l   %d4,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1559]
      move.w   _SOURCEY,%d0            | [1560]
      move.w   (-106,%a6),%d1          | [1561]
      move.w   %d1,(-22,%a6)           | [1562]
      move.w   (-16,%a6),%d2           | [1563]

      tst.w    (-126,%a6)              | [1565 EQ LT]
      jbeq     upsd_dwn                | [1566]
rot90:
      jblt     top_src                 | [1568]
|;;   neg.w     -16(a6)                | [1569]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   subq.w    #1,d1                  | [1570]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   add.w     d1,d0                  | [1571]
	add.l    %d4,%d0
      mulu.w   %d2,%d0                 | [1572]
|;;   adda.w    d0,a0                  | [1573]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1575]
|;;   lsr.w     #4,d0                  | [1576]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1577]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1578]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1579]
      movea.l  _scrtchp,%a1            | [1580]
|;;   adda.w    -4(a6),a1              | [1581]
	mvs.w    (-4,%a6),%d4
	adda.l   %d4,%a1

      tst.w    (-126,%a6)              | [1583 GT]
      jbgt     top_dwn                 | [1584]
|;;   neg.w     -18(a6)                | [1585]
	move.w   (-18,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1586]
|;;   subq.w    #1,d1                  | [1587]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [1588]
	mulu.w   %d4,%d0
|;;   adda.w    d0,a1                  | [1589]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1591]
      move.w   #0x8000,%d3             | [1592]
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
      move.w   %d3,%d4                 | [1593]
      movea.w  %d3,%a4                 | [1594]
|;;   lsr.w     d2,d3                  | [1595]
	mvz.w    %d3,%d5
	lsr.l    %d2,%d5
      moveq.l  #0,%d0                  | [1596]
      move.w   (-20,%a6),%d2           | [1597]
|***	RESTORE	AFTER d6	***
	movem.l  20(%a7),%d6
      move.w   (-16,%a6),%d6           | [1598]
      movea.l  %a0,%a2                 | [1599]
      movea.l  %a1,%a3                 | [1600]
	move.w   %d5,%d3
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      jbra     rot_nsrc                | [1601]
rot_ylp:
|***	SAVE	d2	***
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1603]
|***	RESTORE	d3	***
	movem.l  4(%a7),%d3
      jbra     rot_srt                 | [1604]
rot_xlp:
      move.w   (%a0),%d7               | [1606]
|;;   and.w     d3,d7                  | [1607 EQ]
	move.w   %d7,%d2
	and.l    %d3,%d2
	tst.w    %d2
      jbeq     rot_nor                 | [1608]
|;;   or.w      d4,d0                  | [1609]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1611 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1610_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1610_1:         
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      jbcc     rot_isrc                | [1612]
rot_ndst:
      move.w   %d0,(%a1)+              | [1614]
      moveq.l  #0,%d0                  | [1615]
rot_isrc:
|;;   adda.w    d6,a0                  | [1617]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,8(%a7)
|;;   dbra      d1,rot_xlp             | [1619]
	tst.w    %d1
	jbeq     _PA_1618_
	subq.l   #1,%d1
	jbra     rot_xlp
	_PA_1618_:         

      move.w   %d0,(%a1)               | [1621]
      moveq.l  #0,%d0                  | [1622]
|;;   adda.w    -18(a6),a3             | [1623]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1624]
      move.w   %a4,%d4                 | [1625]
|;;   ror.w     d5,d3                  | [1626 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1625_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1625_1:         
      jbcc     rnew_src                | [1627]
      addq.l   #2,%a2                  | [1628]
rnew_src:
      movea.l  %a2,%a0                 | [1630]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2
	movem.l  8(%a7),%d6
	lea      12(%a7),%a7
rot_nsrc:
|***	SAVE	d3	***
	lea      -12(%a7),%a7
	movem.l  %d3,4(%a7)
|;;   dbra      d2,rot_ylp             | [1632]
	tst.w    %d2
	jbeq     _PA_1631_
	subq.l   #1,%d2
	jbra     rot_ylp
	_PA_1631_:         


rot_done:
      move.w   (-102,%a6),%d0          | [1636]
      move.w   (-106,%a6),%d1          | [1637]
      move.w   %d1,(-20,%a6)           | [1638]
      move.w   %d1,(-102,%a6)          | [1639]
      move.w   %d0,(-22,%a6)           | [1640]
      move.w   %d0,(-106,%a6)          | [1641]

      move.w   (-92,%a6),%d0           | [1643]
      move.w   (-90,%a6),(-92,%a6)     | [1644]
      move.w   %d0,(-90,%a6)           | [1645]
      move.w   #1,(-94,%a6)            | [1646]

      move.w   (-18,%a6),%d0           | [1648]
      tst.w    (-126,%a6)              | [1649 GT]
      jbgt     rot_nneg                | [1650]
|;;   neg.w     d0                     | [1651]
	move.w   %d0,%d3
	neg.l    %d3
	move.w   %d3,%d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1653]
      move.w   (-4,%a6),%d0            | [1654]
|***	RESTORE	AFTER d3	***
	movem.l  4(%a7),%d3
	lea      12(%a7),%a7
repexit:
      clr.w    _SOURCEX                | [1656]
repexit3:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      movea.l  _scrtchp,%a2            | [1658]
|;;   adda.w    d0,a2                  | [1659]
	mvs.w    %d0,%d1
	adda.l   %d1,%a2
      move.l   %a2,(-10,%a6)           | [1660]
      clr.w    _SOURCEY                | [1661]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1662]


upsd_dwn:
      move.w   (-102,%a6),%d0          | [1666]
|;;   add.w     -24(a6),d0             | [1667]
	move.w   (-24,%a6),%d4
	add.l    %d4,%d0
|;;   subq.w    #1,d0                  | [1668]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1669]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1670]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1671]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1673]
      move.w   %d0,%d2                 | [1674]
|;;   lsr.w     d5,d2                  | [1675]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1676]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1677]
|;;   add.w     -4(a6),d0              | [1678 X]
	move.w   %d0,%d6
	mvz.w    (-4,%a6),%d4
	ori.l    #0xffff0000,%d6
	add.l    %d4,%d6
      movea.l  _scrtchp,%a1            | [1679]
|;;   adda.w    d0,a1                  | [1680]
	ext.l    %d6
	adda.l   %d6,%a1
|***	RESTORE	d4/d6	***
	movem.l  12(%a7),%d4
	movem.l  20(%a7),%d6
      jbra     strtflip                | [1681]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1683]
      move.w   %d2,%d3                 | [1684]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1686]
      moveq.l  #0,%d6                  | [1687]
      moveq.l  #15,%d4                 | [1688]
flip_lp:
|***	SAVE	d4	***
	movem.l  %d4,12(%a7)
|;;   lsr.w     d5,d0                  | [1690 X]
	mvz.w    %d0,%d1
	lsr.l    %d5,%d1
|;;   roxl.w    d5,d6                  | [1691 X]
	move.w   %d1,%d0
	move.b   %d5,%d1
	move.w   %d6,%d2
	moveq    #16,%d4
	and.l    #0x1f,%d1
	jbeq     _PA_1690_
	move.w   %d2,%d3
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d3,%d2
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d2
	_PA_1690_:         
	addx.l   %d3,%d3
	swap     %d3
	move.w   %d2,%d3
	lsl.l    %d4,%d3
	move.w   %d2,%d6
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
|;;   dbra      d4,flip_lp             | [1692]
	move     %CCR,%d1
	tst.w    %d4
	jbeq     _PA_1691_
	subq.l   #1,%d4
	jbra     flip_lp
	_PA_1691_:         
	move     %d1,%CCR

|;;   move.w    d6,-(a1)               | [1694]
	move.w   %d2,-(%a1)
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1695]
	move     %CCR,%d1
	tst.w    %d3
	jbeq     _PA_1694_
	subq.l   #1,%d3
	move     %d1,%CCR
	jbra     line_lp
	_PA_1694_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1697]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1699]
	move     %CCR,%d7
	tst.w    %d1
	jbeq     _PA_1698_
	subq.l   #1,%d1
	move     %d7,%CCR
	jbra     upsd_lp
	_PA_1698_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1701]
      movea.l  _scrtchp,%a1            | [1702]
|;;   adda.w    -4(a6),a1              | [1703]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1704]
      move.w   _SOURCEX,%d0            | [1705]
|;;   add.w     -102(a6),d0            | [1706]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d5
	add.l    %d5,%d7
|;;   neg.w     d0                     | [1707]
	neg.l    %d7
|;;   andi.w    #0xF,d0                | [1708]
	andi.l   #(0xFFFF&0xF),%d7
|;;   move.w    d0,_SOURCEX            | [1709]
	move.w   %d7,_SOURCEX
      clr.w    _SOURCEY                | [1710]
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      rts                              | [1711]





replicat:
|***	SAVE	d6-d7	***
	lea      -16(%a7),%a7
	movem.l  %d6-%d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1719]
      move.w   %d0,%d4                 | [1720]
|;;   andi.w    #0x0F,d0               | [1721]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1722]
|;;   lsr.w     #4,d4                  | [1723]
	mvz.w    %d4,%d6
	lsr.l    #4,%d6
|;;   lsl.w     #1,d4                  | [1724]
	lsl.l    #1,%d6

      move.w   _SOURCEY,%d0            | [1726]
      mulu.w   (-16,%a6),%d0           | [1727]
|;;   add.w     d4,d0                  | [1728]
	move.w   %d0,%d7
	add.l    %d6,%d7
	move.w   %d7,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1729]
|;;   adda.w    d0,a0                  | [1730]
	mvs.w    %d0,%d3
	adda.l   %d3,%a0

      move.w   (-24,%a6),%d2           | [1732]
      move.w   #0x8000,%d3             | [1733]
      move.w   %d3,%d4                 | [1734]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1735]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1737]
      move.w   (-102,%a6),%d1          | [1738]

      move.w   %d2,(-22,%a6)           | [1740]
      move.w   %d1,(-20,%a6)           | [1741]
      movea.l  _scrtchp,%a1            | [1742]
|;;   adda.w    _scrpt2,a1             | [1743]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1745 EQ]
      jbeq     noline                  | [1746]



|;;   addq.w    #1,-106(a6)            | [1750]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1751]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1753]
	move.w   %d1,%d0
	lsl.l    #1,%d0
|;;   addq.w    #3,d1                  | [1754]
	addq.l   #3,%d0
|;;   lsr.w     #4,d1                  | [1755]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d1                  | [1756]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1757]
	addq.l   #2,%d0
|;;   move.w    d1,d5                  | [1758]
	move.w   %d0,%d5
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      moveq.l  #0,%d6                  | [1759]
	move.w   %d0,%d1
oklear:
      move.w   %d6,(%a1)+              | [1761]
|;;   dbra      d5,oklear              | [1762]
	tst.w    %d5
	jbeq     _PA_1761_
	subq.l   #1,%d5
	jbra     oklear
	_PA_1761_:         
      lea      (-2,%a1),%a2            | [1763]
      move.w   %d2,%d5                 | [1764]
|;;   lsl.w     #1,d5                  | [1765]
	move.w   %d5,%d0
	lsl.l    #1,%d0
	move.w   %d0,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1767]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1768]
|;;   dbra      d5,oklear1             | [1769]
	tst.w    %d5
	jbeq     _PA_1768_
	subq.l   #1,%d5
	jbra     oklear1
	_PA_1768_:         

|;;   move.w    d1,d5                  | [1771]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1772]
	mvz.w    %d5,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d5                  | [1773 X]
	subq.l   #1,%d0
	move.w   %d0,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1775]
|;;   dbra      d5,oklear3             | [1776]
	move     %CCR,%d0
	tst.w    %d5
	jbeq     _PA_1775_
	subq.l   #1,%d5
	move     %d0,%CCR
	jbra     oklear3
	_PA_1775_:         
	move     %d0,%CCR
      jbra     noline1                 | [1777]

noline:
|;;   lsr.w     #3,d1                  | [1780]
	mvz.w    %d1,%d0
	lsr.l    #3,%d0
|;;   lsl.w     #1,d1                  | [1781]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1782 X]
	ori.l    #0xffff0000,%d0
	addq.l   #2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
noline1:
      move.w   %d1,(-18,%a6)           | [1784]
|***	RESTORE	AFTER d7	***
	movem.l  12(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1788]
|;;   roxr.l    #1,d7                  | [1789 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1790]
|;;   roxr.l    #1,d7                  | [1791]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1793]
      move.w   _DDA_INC,%d2            | [1794]
      move.w   (-22,%a6),%d5           | [1795]
|;;   subq.w    #1,d5                  | [1796]
	move.w   %d5,%d0
	subq.l   #1,%d0
      move.w   #32767,%d6              | [1797]
	move.w   %d0,%d5
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      16(%a7),%a7
      tst.l    %d7                     | [1798 MI]
      bmi.b    rep_ylop                | [1799]
y_dwn_lp:
|***	SAVE	d0-d2	***
	lea      -12(%a7),%a7
	movem.l  %d0-%d2,(%a7)
|;;   add.w     d2,d6                  | [1801 CC]
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
      jbcc     y_no_drw                | [1802]
      jbsr     yloop                   | [1803]
y_no_drw:
|;;   adda.w    d7,a0                  | [1805]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  8(%a7),%d2
	lea      12(%a7),%a7
|;;   dbra      d5,y_dwn_lp            | [1806]
	tst.w    %d5
	jbeq     _PA_1805_
	subq.l   #1,%d5
	jbra     y_dwn_lp
	_PA_1805_:         
	move.w   #-1,%d5
      jbra     y_rep_don               | [1807]
rep_ylop:
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
|;;   add.w     d2,d6                  | [1809 CC]
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
      jbcc     y_no_rep                | [1810]
      jbsr     yloop                   | [1811]
y_no_rep:
      jbsr     yloop                   | [1813]
|;;   adda.w    d7,a0                  | [1814]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  4(%a7),%d2
	lea      8(%a7),%a7
|;;   dbra      d5,rep_ylop            | [1815]
	tst.w    %d5
	jbeq     _PA_1814_
	subq.l   #1,%d5
	jbra     rep_ylop
	_PA_1814_:         
	move.w   #-1,%d5
y_rep_don:
|***	SAVE	d4-d6	***
	lea      -12(%a7),%a7
	movem.l  %d4-%d6,(%a7)
      move.w   (-102,%a6),%d2          | [1817]
      move.w   _XACC_DDA,%d1           | [1818]
      move.w   _DDA_INC,%d0            | [1819]
      moveq.l  #0,%d3                  | [1820]
repwidcl:
|;;   add.w     d0,d1                  | [1822 CC]
	mvz.w    %d0,%d4
	move.w   %d1,%d5
	ori.l    #0xffff0000,%d5
	add.l    %d4,%d5
	move     %ccr,%d6
	move.w   %d5,%d1
	move     %d6,%ccr
      jbcc     nrepdoub                | [1823]
|;;   addq.w    #1,d3                  | [1824]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrepdoub:
      tst.l    %d7                     | [1826 PL]
      bpl.b    nrpndoub                | [1827]
|;;   addq.w    #1,d3                  | [1828]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1830]
	tst.w    %d2
	jbeq     _PA_1829_
	subq.l   #1,%d2
	jbra     repwidcl
	_PA_1829_:         
      move.w   %d1,_XACC_DDA           | [1831]
      move.w   %d3,(-102,%a6)          | [1832]
      move.w   (-92,%a6),(-106,%a6)    | [1833]


      move.w   (-18,%a6),%d1           | [1836]
      move.w   %d1,(-16,%a6)           | [1837]







repexit1:
      btst     #4,(-96+1,%a6)          | [1846 EQ]
      jbeq     repexit2                | [1847]
      movea.l  _scrtchp,%a0            | [1848]
|;;   adda.w    _scrpt2,a0             | [1849]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1850]
|;;   lea       2(a0,d7.w),a1          | [1851]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
|***	RESTORE	AFTER d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbsr     outlin1                 | [1852]
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
      move.w   _scrpt2,%d0             | [1853]
|;;   add.w     -18(a6),d0             | [1854]
	move.w   %d0,%d2
	move.w   (-18,%a6),%d1
	add.l    %d1,%d2
|;;   move.w    #15,_SOURCEX           | [1855]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
	move.w   %d2,%d0
|***	RESTORE	d1-d2	***
	movem.l  (%a7),%d1-%d2
	lea      8(%a7),%a7
      jbra     repexit3                | [1856]
repexit2:
      move.w   _scrpt2,%d0             | [1858]
|***	RESTORE	d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbra     repexit                 | [1859]
| 2402 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1862]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1863]
      movea.l  %a1,%a4                 | [1864]
      movea.l  %d7,%a5                 | [1865]
      move.w   %d5,(-44,%a6)           | [1866]
      moveq.l  #0,%d0                  | [1867]
      move.w   (-20,%a6),%d5           | [1868]
|;;   subq.w    #1,d5                  | [1869]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1871]
      jbra     nextsrc                 | [1872]
innerlp:
|;;   ror.w     #1,d3                  | [1874 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jbcc     reploop                 | [1875]
nextsrc:
      movea.w  (%a0)+,%a2              | [1877]
reploop:
      move.w   %a2,%d6                 | [1879]
|;;   and.w     d3,d6                  | [1880 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jbne     nrepnor                 | [1881]


repnor:
      tst.l    %d7                     | [1885 MI]
      bmi.b    repnorup                | [1886]
|;;   add.w     d2,d7                  | [1887 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     incsrc                  | [1888]
      jbra     ordone                  | [1889]
repnorup:
|;;   add.w     d2,d7                  | [1891 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     ordone                  | [1892]


|;;   ror.w     #1,d4                  | [1895 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jbcc     ordone                  | [1898]
      move.w   %d0,(%a1)+              | [1899]
      moveq.l  #0,%d0                  | [1900]


      jbra     ordone                  | [1903]
nrepnor:


|;;   add.w     d2,d7                  | [1907 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     o_no_rep                | [1908]


|;;   or.w      d4,d0                  | [1911]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1912 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jbcc     o_no_rep                | [1914]
      move.w   %d0,(%a1)+              | [1915]
      moveq.l  #0,%d0                  | [1916]
o_no_rep:
      tst.l    %d7                     | [1918 PL]
      bpl.b    incsrc                  | [1919]


|;;   or.w      d4,d0                  | [1922]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1924 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jbcc     incsrc                  | [1925]
nextdst:
      move.w   %d0,(%a1)+              | [1927]
      moveq.l  #0,%d0                  | [1928]
incsrc:
|;;   dbra      d5,innerlp             | [1930]
	tst.w    %d5
	jbeq     _PA_1929_
	subq.l   #1,%d5
	jbra     innerlp
	_PA_1929_:         
repdone:
      move.w   %d0,(%a1)               | [1932]
|;;   movem.l   (sp)+,d3-d4/d6         | [1933]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1934]
      movea.l  %a4,%a1                 | [1935]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1936]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1937]
      move.w   (-44,%a6),%d5           | [1938]
      rts                              | [1939]

| 2491 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1944]
	move.w   %d7,%a5
	lea      (0,%a0,%a5.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1946]
	move.w   %d7,%a5
	lea      (0,%a1,%a5.l*1),%a2
|;;   lsr.w     #1,d7                  | [1947]
	mvz.w    %d7,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d7                  | [1948]
	subq.l   #1,%d0
|;;   move.w    d7,-44(a6)             | [1949]
	move.w   %d0,(-44,%a6)
      move.w   (-106,%a6),%d6          | [1950]

      movea.l  %a0,%a5                 | [1952]
|;;   subq.w    #1,d6                  | [1953]
	subq.l   #1,%d6
	move.w   %d0,%d7
out_edge:
      movea.w  %d6,%a4                 | [1955]
      movea.l  %a2,%a3                 | [1956]
      moveq.l  #0,%d5                  | [1957]
      moveq.l  #0,%d6                  | [1958]
      move.l   (%a2),%d1               | [1959]
      lsr.l    #1,%d1                  | [1960]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1965]
      move.b   %d5,%d0                 | [1966]
|;;   ror.l     #1,d0                  | [1967]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1969]
      move.b   %d6,%d2                 | [1970]
      move.l   %d2,%d3                 | [1971]
|;;   ror.l     #1,d3                  | [1972]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1973]
|;;   ror.l     #1,d4                  | [1974]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1976]
      move.l   %d0,%d6                 | [1977]
      eor.l    %d2,%d0                 | [1978]
      eor.l    %d3,%d5                 | [1979]
      eor.l    %d4,%d6                 | [1980]
|;;   rol.l     #1,d5                  | [1981]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1982]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1983]
      or.l     %d6,%d0                 | [1984]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1986]
      move.l   %d1,%d6                 | [1987]
      eor.l    %d2,%d1                 | [1988]
      eor.l    %d3,%d5                 | [1989]
      eor.l    %d4,%d6                 | [1990]
|;;   rol.l     #1,d5                  | [1991]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1992]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1993]
      or.l     %d5,%d0                 | [1994]
      or.l     %d6,%d0                 | [1995]

      eor.l    %d3,%d2                 | [1997]
      eor.l    %d3,%d4                 | [1998]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [1999]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [2000]
      or.l     %d4,%d0                 | [2001]
      swap     %d0                     | [2002]

      move.w   (%a1),%d6               | [2004]
      move.w   %d6,%d5                 | [2005]
|;;   eor.w     d0,d5                  | [2006]
	move.w   %d5,%d3
	eor.l    %d0,%d3
|;;   and.w     d0,d5                  | [2007]
	and.l    %d0,%d3

      addq.l   #2,%a2                  | [2009]
      move.l   (%a2),%d1               | [2010]
      move.b   (-1,%a2),%d1            | [2011]
|;;   ror.l     #1,d1                  | [2012]
	move.w   %d3,%d5
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [2014]
      move.w   (%a0),%d5               | [2015]
      move.w   %d6,(%a0)+              | [2016]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [2018]
	tst.w    %d7
	jbeq     _PA_2017_
	subq.l   #1,%d7
	jbra     out_loop
	_PA_2017_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [2020]
      movea.l  %a5,%a0                 | [2021]
      move.w   %a4,%d6                 | [2022]
      movea.l  %a3,%a2                 | [2023]
      movea.l  %a2,%a1                 | [2024]
|;;   adda.w    -16(a6),a2             | [2025]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [2026 NE]
      jbne     srt_lin                 | [2027]
      movea.l  %a1,%a2                 | [2028]
srt_lin:
|;;   dbra      d6,out_edge            | [2030]
	tst.w    %d6
	jbeq     _PA_2029_
	subq.l   #1,%d6
	jbra     out_edge
	_PA_2029_:         
	move.w   #-1,%d6
      rts                              | [2031]
| 2602 "vdi/vdi_tblit.S"
concat:
|;;   movem.l   d2/d3/a0,-(sp)         | [2034]
	lea      -12(%a7),%a7
	movem.l  %d2-%d3/%a0,(%a7)



      mulu.w   _v_lin_wr,%d1           | [2038]





      move.w   %d0,%d2                 | [2044]
|;;   andi.w    #0x000f,d0             | [2045]
	andi.l   #(0xFFFF0000!~0x000f),%d0
|;;   andi.w    #0xfff0,d2             | [2046]
	andi.l   #(0xFFFF&0xfff0),%d2
| 2624 "vdi/vdi_tblit.S"
      lea      _shift_offset,%a0       | [2048]
|;;   add.w     _v_planes,a0           | [2049]
	mvs.w    _v_planes,%d3
	adda.l   %d3,%a0
      move.b   (%a0),%d3               | [2050]
|;;   lsr.w     d3,d2                  | [2051]
	lsr.l    %d3,%d2






      ext.l    %d2                     | [2058]
      add.l    %d2,%d1                 | [2059]
|;;   movem.l   (sp)+,d2/d3/a0         | [2060]
	movem.l  (%a7),%d2-%d3/%a0
	lea      12(%a7),%a7
      rts                              | [2061]

| End section on aligned boundary
	.p2align 2,0
