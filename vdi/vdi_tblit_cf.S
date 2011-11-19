
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
| Translated on Sat Nov 19 23:00:26 2011 by version 2.6.0 of PortAsm translator
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
|;;           .extern _shft_off        | [52]
	.extern	_shft_off






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
	move.w   %d0,%d2
	move.w   (-102,%a6),%d1
	add.l    %d1,%d2
|;;   subq.w    #1,d0                  | [518]
	subq.l   #1,%d2
	move.w   %d2,%d0
      cmp.w    (-118,%a6),%d0          | [519 LE]
      jble     scrn_blt                | [520]

|;;   sub.w     -118(a6),d0            | [522]
	move.w   (-118,%a6),%d1
	sub.l    %d1,%d2
|;;   sub.w     d0,-102(a6)            | [523]
	move.w   (-102,%a6),%d1
	sub.l    %d2,%d1
	move.w   %d1,(-102,%a6)

      jbra     scrn_blt                | [525]



nxtword:
|;;           .dc.b 2,4,0,8            | [530]
	.byte	2,4,0,8
| 932 "vdi/vdi_tblit.S"
	.p2align 1,0
scrn_blt:
|;;   move.w    _TEXT_FG,-84(a6)       | [533]
	move.w   _TEXT_FG,%d1
	move.w   %d1,(-84,%a6)
      move.w   (-6,%a6),(-76,%a6)      | [534]
      move.w   _v_planes,%d0           | [535]
      move.w   %d0,(-86,%a6)           | [536]
|;;   move.b    nxtword-1(pc,d0.w),d0  | [537]
	move.w   %d0,%a5
	movea.l  #nxtword-1,%a0
	move.b   (0,%a0,%a5.l*1),%d0
      move.w   %d0,(-88,%a6)           | [538]

      move.w   _SOURCEX,%d0            | [540]
      move.w   %d0,%d2                 | [541]
|;;   andi.w    #0x0F,d2               | [542]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [543]
|;;   lsr.w     #3,d0                  | [544]
	mvz.w    %d0,%d1
	lsr.l    #3,%d1
|;;   andi.w    #0xFFFE,d0             | [545]
	andi.l   #(0xFFFF&0xFFFE),%d1
      move.w   _SOURCEY,%d2            | [546]

      move.w   (-106,%a6),(-22,%a6)    | [548]
|;;   add.w     -22(a6),d2             | [549]
	move.w   (-22,%a6),%d3
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [550]
	subq.l   #1,%d2
      mulu.w   (-16,%a6),%d2           | [551]
|;;   neg.w     -16(a6)                | [552]
	move.w   (-16,%a6),%d3
	neg.l    %d3
	move.w   %d3,(-16,%a6)
|;;   add.w     d0,d2                  | [553]
	move.w   %d2,%d3
	add.l    %d1,%d3



      movea.l  (-10,%a6),%a0           | [557]
|;;   adda.w    d2,a0                  | [558]
	ext.l    %d3
	adda.l   %d3,%a0
      move.l   %a0,(-10,%a6)           | [559]

      move.w   (-102,%a6),(-20,%a6)    | [561]

      move.w   (-108,%a6),%d1          | [563]
|;;   add.w     -106(a6),d1            | [564]
	move.w   (-106,%a6),%d3
	add.l    %d3,%d1
|;;   subq.w    #1,d1                  | [565]
	subq.l   #1,%d1

      move.w   (-104,%a6),%d0          | [567]

      jsr      concat                  | [569]



      movea.l  _v_bas_ad,%a1           | [573]
      adda.l   %d1,%a1                 | [574]

      move.l   %a1,(-14,%a6)           | [576]
      move.w   %d0,(-26,%a6)           | [577]

      move.w   _v_lin_wr,%d0           | [579]
|;;   neg.w     d0                     | [580]
	move.w   %d0,%d3
	neg.l    %d3
|;;   move.w    d0,-18(a6)             | [581]
	move.w   %d3,(-18,%a6)

      jbsr     norm_blt                | [583]

upda_dst:
      move.w   _DELX,%d1               | [586]


      tst.w    _DOUBLE                 | [589 EQ]
      jbeq     upda_ndb                | [590]



      move.w   (-90,%a6),%d1           | [594]
      move.w   (-92,%a6),%d3           | [595]

      tst.w    (-94,%a6)               | [597 EQ]
      jbeq     upda_ndb                | [598]
|;;   exg       d1, d3                 | [599]
	move.l   %d1,%d2
	move.l   %d3,%d1
	move.l   %d2,%d3





upda_ndb:
|;;   btst.b    #0,_STYLE+1            | [606 EQ]
	moveq.l  #0,%d2
	btst     %d2,_STYLE+1
      jbeq     upda_ntk                | [607]
      tst.w    _MONO_STATUS            | [608 NE]
      jbne     upda_ntk                | [609]
|;;   add.w     _WEIGHT,d1             | [610]
	move.w   _WEIGHT,%d2
	add.l    %d2,%d1
upda_ntk:
      tst.w    (-110,%a6)              | [612 GT]
      jbgt     ck90                    | [613]
|;;   add.w     d1,_DESTX              | [614]
	move.w   _DESTX,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [615]
ck90:
      tst.w    (-126,%a6)              | [617 EQ GT]
      jbeq     ck180                   | [618]
      jbgt     ck270                   | [619]
|;;   sub.w     d1,_DESTY              | [620]
	move.w   _DESTY,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTY
      jbra     blt_done                | [621]
ck180:
|;;   sub.w     d1,_DESTX              | [623]
	move.w   _DESTX,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [624]
ck270:
|;;   add.w     d1,_DESTY              | [626]
	move.w   _DESTY,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTY
blt_done:
|;;   movem.l   (sp)+,d2-d7/a2-a5      | [628]
	movem.l  (%a7),%d2-%d7/%a2-%a5
	lea      40(%a7),%a7
      unlk     %a6                     | [629]
      rts                              | [630]



|;;           .text                    | [635]
| 1044 "vdi/vdi_tblit.S"
|;;   lf_tab: dc.w 0xFFFF              | [637]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: dc.w 0x7FFF              | [638]
rf_tab:
	.short	0x7FFF
|;;                   dc.w 0x3FFF      | [639]
	.short	0x3FFF
|;;                   dc.w 0x1FFF      | [640]
	.short	0x1FFF
|;;                   dc.w 0x0FFF      | [641]
	.short	0x0FFF
|;;                   dc.w 0x07FF      | [642]
	.short	0x07FF
|;;                   dc.w 0x03FF      | [643]
	.short	0x03FF
|;;                   dc.w 0x01FF      | [644]
	.short	0x01FF
|;;                   dc.w 0x00FF      | [645]
	.short	0x00FF
|;;                   dc.w 0x007F      | [646]
	.short	0x007F
|;;                   dc.w 0x003F      | [647]
	.short	0x003F
|;;                   dc.w 0x001F      | [648]
	.short	0x001F
|;;                   dc.w 0x000F      | [649]
	.short	0x000F
|;;                   dc.w 0x0007      | [650]
	.short	0x0007
|;;                   dc.w 0x0003      | [651]
	.short	0x0003
|;;                   dc.w 0x0001      | [652]
	.short	0x0001
|;;                   dc.w 0x0000      | [653]
	.short	0x0000

|;;           .text                    | [655]
| 1075 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [658]
|;;   sub.w     -24(a6),d1             | [659]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [660 PL]
      bpl.b    do_rot                  | [661]



|;;   neg.w     d1                     | [665]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [666]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [667]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [669]

|;;   lsl.w     #1,d0                  | [671]
	move.w   %d0,%d5
	lsl.l    #1,%d5
      lea      lf_tab,%a2              | [672]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [673]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [674]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [675]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [680]
|;;   lsl.w     #1,d0                  | [681]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [682]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [684]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [685]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [688]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [689]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [690]
	andi.l   #(0xFFFF&0xF),%d5
|;;   lsl.w     #1,d2                  | [691]
	lsl.l    #1,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [692]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [693]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [694]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [695]
      move.w   #0x8000,(-80,%a6)       | [696]
      moveq.l  #(-1),%d3               | [697]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [700 HI]
      bhi.b    doub_des                | [701]



|;;   lsl.w     #1,d0                  | [705]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [706]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [707]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [708]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [710]
	move.w   %d5,%d1
      jbra     msk_done                | [711]



doub_des:
      move.w   %d0,%d1                 | [716]
|;;   lsr.w     #4,d1                  | [717]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [718]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [719 NE]
      jbne     mlt_dest                | [720]



      move.w   (-24,%a6),%d1           | [724]
|;;   add.w     -20(a6),d1             | [725]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [726 CC]
      jbcc     mlt_dest                | [727]
      moveq.l  #0,%d7                  | [728]
      jbra     do_ritem                | [729]

mlt_dest:
      tst.w    (-42,%a6)               | [732 LT]
      jblt     mltleft                 | [733]
      moveq.l  #4,%d7                  | [734]
      jbra     do_ritem                | [735]
mltleft:
      moveq.l  #8,%d7                  | [737]
do_ritem:
|;;   andi.w    #0x000F,d0             | [739 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jbne     not_null                | [740]
|;;   subq.w    #1,d3                  | [741]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [742]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [744]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [745]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [746]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [749]
      move.w   %d3,(-28,%a6)           | [750]
      move.w   %d4,(-36,%a6)           | [751]
|;;   addq.w    #2,d3                  | [752]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [753 CS]
      jbcs     msk0                    | [754]
|;;   addq.w    #1,d3                  | [755]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [757]
      move.w   %d3,(-30,%a6)           | [758]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jbra     plane_loop              | [759]

| 1195 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [764]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [765]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [766]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [767]
	.byte	4*5,4*5,4*6,4*6


toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [771]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [773]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [777]
|;;   lsr.w     -84(a6)                | [778 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [779]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [780 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [781]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [785]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [786]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [787]
      movea.l  %a2,%a5                 | [788]

|;;   movea.l   twoptble(pc,d0.w),a4   | [790]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [794 EQ]
      jbeq     wrt_char                | [795]

      btst     #1,%d0                  | [797 EQ]
      jbeq     no_lite                 | [798]



|;;   move.w    _LITEMASK,-78(a6)      | [802]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [803]
      lea      liteop,%a5              | [804]

      move.l   %a2,(-64,%a6)           | [806]
      lea      liteopwf,%a2            | [807]

      move.l   %a4,(-72,%a6)           | [809]
      lea      liteopw,%a4             | [810]

no_lite:
      btst     #0,%d0                  | [813 EQ]
      jbeq     no_thick                | [814]



      clr.w    (-82,%a6)               | [818]

      move.l   %a5,(-48,%a6)           | [820]
      lea      thknop,%a5              | [821]

      move.l   %a2,(-60,%a6)           | [823]
      lea      thknopwf,%a2            | [824]

      move.l   %a4,(-68,%a6)           | [826]
      lea      thknopw,%a4             | [827]

no_thick:
      btst     #2,%d0                  | [830 EQ]
      jbeq     wrt_char                | [831]



      move.w   (-100,%a6),(-80,%a6)    | [835]
      move.l   %a5,(-56,%a6)           | [836]
      lea      skewop,%a5              | [837]

      tst.w    (-124,%a6)              | [839 EQ GT]
      jbgt     wrt_char                | [840]
      jbeq     not_sngl                | [841]
      clr.w    (-28,%a6)               | [842]
      moveq.l  #0,%d7                  | [843]
      jbra     chng_blt                | [844]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [846 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      bls.b    wrt_char                | [847]

      tst.w    (-42,%a6)               | [849 LT]
      jblt     chng_left               | [850]
      moveq.l  #4,%d7                  | [851]
      jbra     chng_blt                | [852]
chng_left:
      moveq.l  #8,%d7                  | [854]
chng_blt:
      move.w   %d7,(-124,%a6)          | [856]

wrt_char:
      move.w   (-124,%a6),%d7          | [859 LE]
      jble     set_chr_hgt             | [860]
      movea.l  %a2,%a5                 | [861]
      move.w   (-18,%a6),%d5           | [862]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [864]
|;;   subq.w    #1,d3                  | [865]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [867]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [868]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [870 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jble     no_more_planes          | [871]
      movea.l  (-10,%a6),%a0           | [872]
      movea.l  (-14,%a6),%a1           | [873]
      addq.l   #2,%a1                  | [874]
      move.l   %a1,(-14,%a6)           | [875]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [876 EQ]
      jbeq     plane_loop              | [877]
      jbra     norm_blt                | [878]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [880]
blttype:
|;;           dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [882]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [889]
      move.w   (-16,%a6),%d6           | [890]
      move.w   (-18,%a6),%d7           | [891]
      lea      sgl_rtn,%a3             | [892]
      jbra     sgl_1st_scan            | [893]

sgl_lp:
|;;   adda.w    d6,a0                  | [896]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [897]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [899]
| 1340 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [901]


      move.w   (-42,%a6),%d1           | [904 MI]
      bmi.b    left_rol                | [905]
      lsr.l    %d1,%d0                 | [906]
      jbra     end_ro                  | [907]
left_rol:
      lsl.l    %d1,%d0                 | [909]
end_ro:
      swap     %d0                     | [911]
      move.w   %d0,%d1                 | [912]
      swap     %d0                     | [913]

      jmp      (%a5)                   | [915]

sgl_rtn:
      move.w   %d1,(%a1)               | [918]
|;;   dbra      d3,sgl_lp              | [919]
	tst.w    %d3
	jbeq     _PA_918_
	subq.l   #1,%d3
	jbra     sgl_lp
	_PA_918_:         
      jbra     next_plane              | [920]




dbl_loop:
      movea.l  %a3,%a4                 | [927]
      move.w   (-16,%a6),%d6           | [928]
      move.w   (-18,%a6),%d7           | [929]
      lea      dbl_2wrd,%a2            | [930]
      move.l   %a2,%d5                 | [931]
      lea      dbl_1wrd,%a3            | [932]
      jbra     dbl_1st_scan            | [933]

dbl_lp:
|;;   adda.w    d6,a0                  | [936]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [937]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [938]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [940]
| 1388 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [942]


      move.w   (-42,%a6),%d1           | [945 MI]
      bmi.b    dbleft                  | [946]
      lsr.l    %d1,%d0                 | [947]
      jbra     dblendr                 | [948]
dbleft:
      lsl.l    %d1,%d0                 | [950]
dblendr:
      swap     %d0                     | [952]
      move.w   %d0,%d1                 | [953]
      swap     %d0                     | [954]
      move.w   (-38,%a6),%d2           | [955]

      jmp      (%a5)                   | [957]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [960]
      move.w   %d0,%d1                 | [961]

      movea.w  (-88,%a6),%a2           | [963]
|;;   move.w    (a1,a2.w),d4           | [964]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [966]

|;;   exg       a3,d5                  | [968]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [969]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [972]
|;;   move.w    d1,(a1,a2.w)           | [973]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [975]
	tst.w    %d3
	jbeq     _PA_974_
	subq.l   #1,%d3
	jbra     dbl_lp
	_PA_974_:         
      jbra     next_plane              | [976]





left_loop:
      movea.l  %d6,%a0                 | [984]
      movea.l  %d7,%a1                 | [985]
|;;   adda.w    -16(a6),a0             | [986]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [987]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [988]
|;;   rol.w     -78(a6)                | [989]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [991 NE]
      jbne     skewopw                 | [992]
mlt_left:
      move.l   %a0,%d6                 | [994]
      move.l   %a1,%d7                 | [995]
      move.w   (-28,%a6),(-44,%a6)     | [996]







      move.l   (%a0)+,%d0              | [1004]


      move.w   (%a1),%d4               | [1007]

      move.w   (-42,%a6),%d1           | [1009]
      lsl.l    %d1,%d0                 | [1010]

      swap     %d0                     | [1012]
      move.w   %d0,%d1                 | [1013]
      swap     %d0                     | [1014]

      move.w   (-38,%a6),%d2           | [1016]

      lea      lft_lfrng,%a3           | [1018]
      jmp      (%a5)                   | [1019]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1022]
|;;   adda.w    -88(a6),a1             | [1023]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1025]
      lea      lft_wrds,%a3            | [1026]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1031]
|;;   and.w     d2,d4                  | [1032]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [1033]





      swap     %d0                     | [1039]
      move.w   %d1,%d0                 | [1040]
      swap     %d0                     | [1041]

      move.w   (-42,%a6),%d1           | [1043]
      lsl.l    %d1,%d0                 | [1044]

      swap     %d0                     | [1046]
      move.w   %d0,%d1                 | [1047]
      swap     %d0                     | [1048]

|;;   not.w     d2                     | [1050]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1051]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1052]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1053]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1055]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1057 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     lef_don                 | [1058]

      jmp      (%a4)                   | [1060]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1063]
|;;   adda.w    -88(a6),a1             | [1064]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_lef                | [1066]

lef_don:
      move.w   (-36,%a6),%d2           | [1069]

      lea      lft_rfrng,%a3           | [1071]
      jmp      (%a5)                   | [1072]

lft_rfrng:
      move.w   %d1,(%a1)               | [1075]

|;;   dbra      d3,left_loop           | [1077]
	tst.w    %d3
	jbeq     _PA_1076_
	subq.l   #1,%d3
	jbra     left_loop
	_PA_1076_:         
      jbra     next_plane              | [1078]






rite_loop:
      movea.l  %d6,%a0                 | [1087]
      movea.l  %d7,%a1                 | [1088]
|;;   adda.w    -16(a6),a0             | [1089]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [1090]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [1091]
|;;   rol.w     -78(a6)                | [1092]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [1094 NE]
      jbne     skewopw                 | [1095]
mlt_rite:
      move.l   %a0,%d6                 | [1097]
      move.l   %a1,%d7                 | [1098]
      move.w   (-28,%a6),(-44,%a6)     | [1099]
      move.w   (%a0)+,%d0              | [1100]



      swap     %d0                     | [1104]

      move.w   (%a1),%d4               | [1106]

      move.w   (-42,%a6),%d1           | [1108]
      lsr.l    %d1,%d0                 | [1109]
      swap     %d0                     | [1110]
      move.w   %d0,%d1                 | [1111]
      swap     %d0                     | [1112]

      move.w   (-38,%a6),%d2           | [1114]

      lea      rgt_lfrng,%a3           | [1116]
      jmp      (%a5)                   | [1117]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1120]
|;;   adda.w    -88(a6),a1             | [1121]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1123]
      lea      rgt_wrds,%a3            | [1124]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1129]
|;;   and.w     d2,d4                  | [1130]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [1131]
      move.w   (%a0)+,%d0              | [1132]



      swap     %d0                     | [1136]

      move.w   (-42,%a6),%d1           | [1138]
      lsr.l    %d1,%d0                 | [1139]
      swap     %d0                     | [1140]
      move.w   %d0,%d1                 | [1141]
      swap     %d0                     | [1142]

|;;   not.w     d2                     | [1144]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1145]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1146]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1147]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1149]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1151 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     rite_don                | [1152]

      jmp      (%a4)                   | [1154]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1157]
|;;   adda.w    -88(a6),a1             | [1158]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_rit                | [1160]

rite_don:
      move.w   (-36,%a6),%d2           | [1163]

      lea      rgt_rfrng,%a3           | [1165]
      jmp      (%a5)                   | [1166]

rgt_rfrng:
      move.w   %d1,(%a1)               | [1169]

|;;   dbra      d3,rite_loop           | [1171]
	tst.w    %d3
	jbeq     _PA_1170_
	subq.l   #1,%d3
	jbra     rite_loop
	_PA_1170_:         
      jbra     next_plane              | [1172]






top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1181]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [1182]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [1183]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [1184]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1185]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1188]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [1189]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [1190]
	not.l    %d0
|;;   and.w     d4,d1                  | [1191]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1192]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1195]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [1196]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1197]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1198]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1201]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [1202]
	not.l    %d0
|;;   and.w     d4,d1                  | [1203]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1204]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1207]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1208]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1209]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1212]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1213]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1214]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1217]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [1218]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1219]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1220]





twop0:
      moveq.l  #0,%d1                  | [1228]
      jmp      (%a3)                   | [1229]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [1232]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1233]

twop3:
      jmp      (%a3)                   | [1236]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1239]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [1240]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1241]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1244]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1245]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [1248]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1249]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1252]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [1253]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1254]
| 1720 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [1257]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [1258]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [1259]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [1260 EQ]
      jbeq     thk00                   | [1261]
|;;   ori.b     #0x10,ccr              | [1262 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [1263 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jbcc     thk01                   | [1264]
      move.w   #0x8000,%d6             | [1265]
thk01:
      move.w   %d6,(-34,%a6)           | [1267]
thk00:
      tst.w    (-28,%a6)               | [1269 MI]
      bmi.b    thk0                    | [1270]
      cmp.w    (-36,%a6),%d6           | [1271 CC]
      jbcc     thk0                    | [1272]
|;;   and.w     d6,d0                  | [1273]
	and.l    %d6,%d0
      jbra     thk1                    | [1274]
thk0:
      clr.w    %d0                     | [1276]
|;;   and.w     d6,d1                  | [1277]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [1279]
      move.w   %d2,%d5                 | [1280]
      swap     %d5                     | [1281]
      move.w   (-36,%a6),%d5           | [1282]
      lsl.l    %d6,%d5                 | [1283]

      swap     %d5                     | [1285]
      move.w   %d5,%d7                 | [1286]
      swap     %d5                     | [1287]

|;;   and.w     d7,d1                  | [1289]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [1290]
	and.l    %d5,%d0
|;;   swap      d1                     | [1291]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [1292]
|;;   subq.w    #1,d6                  | [1293]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [1295]
      lsr.l    #1,%d0                  | [1296]
      or.l     %d0,%d1                 | [1297]
|;;   dbra      d6,thkoplp             | [1298]
	tst.w    %d6
	jbeq     _PA_1297_
	subq.l   #1,%d6
	jbra     thkoplp
	_PA_1297_:         

      move.w   %d1,%d0                 | [1300]
      swap     %d1                     | [1301]
|;;   movem.l   (sp)+,d5-d7            | [1302]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [1303]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [1304]

| 1784 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [1309]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [1310 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk2                    | [1311]
|;;   and.w     -34(a6),d1             | [1312]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [1314]
      swap     %d1                     | [1315]
      clr.w    %d1                     | [1316]
      move.l   %d1,%d5                 | [1317]
|;;   subq.w    #1,d6                  | [1318]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [1320]
      or.l     %d5,%d1                 | [1321]
|;;   dbra      d6,thkopwlp            | [1322]
	tst.w    %d6
	jbeq     _PA_1321_
	subq.l   #1,%d6
	jbra     thkopwlp
	_PA_1321_:         

      move.w   %d1,%d5                 | [1324]
      swap     %d1                     | [1325]
|;;   or.w      -82(a6),d1             | [1326]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1327]
|;;   movem.l   (sp)+,d5-d6            | [1328]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [1329]
      jmp      (%a2)                   | [1330]

| 1823 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [1335]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [1336]
|;;   subq.w    #1,-30(a6)             | [1337 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      bmi.b    thk3                    | [1338]
      jbeq     thk4                    | [1339]

|;;   subq.w    #1,-30(a6)             | [1341 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk11                   | [1342]
|;;   and.w     d6,d1                  | [1343]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [1345]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jbra     thk7                    | [1346]

thk3:
      clr.w    %d1                     | [1349]
      jbra     thk5                    | [1350]

thk4:
|;;   and.w     d6,d1                  | [1353]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [1356 MI]
      bmi.b    thk6                    | [1357]
      move.w   (-28,%a6),%d5           | [1358]
|;;   addq.w    #2,d5                  | [1359]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [1360]
|;;   ori.b     #0x10,ccr              | [1361 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [1362 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jbcc     thk33                   | [1363]
      move.w   #0x8000,%d7             | [1364]
|;;   addq.w    #1,d5                  | [1365]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [1367 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jbne     thk34                   | [1368]
|;;   subq.w    #1,d5                  | [1369]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [1371 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [1372 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jbcc     thk31                   | [1373]
      move.w   #0x8000,%d6             | [1374]
thk31:
      cmp.w    %d6,%d7                 | [1376 CS]
      jbcs     thk35                   | [1377]
|;;   addq.w    #1,d5                  | [1378]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [1380]
      move.w   %d5,(-32,%a6)           | [1381]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [1383]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:

      move.w   (-74,%a6),%d6           | [1386]
      swap     %d1                     | [1387]
      clr.w    %d1                     | [1388]
      move.l   %d1,%d5                 | [1389]
|;;   subq.w    #1,d6                  | [1390]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [1392]
      or.l     %d5,%d1                 | [1393]
|;;   dbra      d6,thkopwl             | [1394]
	tst.w    %d6
	jbeq     _PA_1393_
	subq.l   #1,%d6
	jbra     thkopwl
	_PA_1393_:         

      move.w   %d1,%d5                 | [1396]
      swap     %d1                     | [1397]
|;;   or.w      -82(a6),d1             | [1398]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1399]
|;;   and.w     d2,d1                  | [1400]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [1401]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [1402]
      jmp      (%a2)                   | [1403]

| 1906 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [1408]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [1409]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [1410]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [1411]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1412]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1415]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [1416]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1417]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1420]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [1421]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1422]

| 1939 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [1427 CC]
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
      jbcc     no_shift                | [1428]

      swap     %d1                     | [1430]
      move.w   %d0,%d1                 | [1431]
      lsr.l    #1,%d1                  | [1432]

      swap     %d2                     | [1434]
      move.w   (-36,%a6),%d2           | [1435]
      lsr.l    #1,%d2                  | [1436]
      move.w   %d2,(-36,%a6)           | [1437]
      swap     %d2                     | [1438]
      move.w   %d2,(-38,%a6)           | [1439 EQ]
      jbeq     nxt_word                | [1440]
      move.w   (-42,%a6),%d0           | [1441 MI]
      bmi.b    dec_rol                 | [1442]
ror_add:
|;;   addq.w    #1,d0                  | [1444]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [1446]
      move.w   %d1,%d0                 | [1447]
      swap     %d1                     | [1448]
no_shift:
      movea.l  (-56,%a6),%a2           | [1450]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1451]


dec_rol:
      tst.b    %d0                     | [1455 EQ]
      jbeq     begn_ror                | [1456]
|;;   subq.w    #1,d0                  | [1457]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jbra     new_shif                | [1458]
begn_ror:
      clr.w    %d0                     | [1460]
      jbra     ror_add                 | [1461]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1466]
      swap     %d2                     | [1467]
      move.w   %d2,(-38,%a6)           | [1468]
|;;   adda.w    -88(a6),a1             | [1469]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1470]

      moveq.l  #15,%d0                 | [1472]
|;;   sub.w     -42(a6),d0             | [1473]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1474]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1475]
      jbra     no_shift                | [1476]






skewopw:
|;;   rol.w     -80(a6)                | [1485 CC]
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
      jbcc     do_shift                | [1486]

      tst.w    (-42,%a6)               | [1488 MI]
      bmi      mlt_left                | [1489]
      jbra     mlt_rite                | [1490]
do_shift:
|;;   ori       #0x10,ccr              | [1492 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1493]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1495]
      cmp.w    #0xFFFF,%d0             | [1496 EQ]
      jbeq     inc_rite                | [1497]

|;;   ori       #0x10,ccr              | [1499 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1500]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1501]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1504]
      tst.b    %d0                     | [1505 NE]
      jbne     no_rota                 | [1506]
      move.w   #0x8000,(-40,%a6)       | [1507]
no_rota:
      move.w   (-38,%a6),%d1           | [1509]
|;;   lsr.w     #1,d1                  | [1510 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jbeq     wnxt_wrd                | [1511]

      move.w   %d1,(-38,%a6)           | [1513]

      tst.w    %d0                     | [1515 MI]
      bmi.b    wdec_rol                | [1516]
|;;   addq.w    #1,-42(a6)             | [1517]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jbra     mlt_rite                | [1518]
wdec_rol:
      tst.b    %d0                     | [1520 EQ]
      jbeq     set_msk                 | [1521]
|;;   subq.w    #1,-42(a6)             | [1522]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jbra     mlt_left                | [1523]
set_msk:
      move.w   #1,(-42,%a6)            | [1525]
      jbra     mlt_rite                | [1526]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1529]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1530]
      jbra     do_left                 | [1531]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1534]

|;;   subq.w    #1,-28(a6)             | [1536]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1537]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1539]
|;;   sub.w     d0,d2                  | [1540]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1541]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1542]

      jbra     mlt_left                | [1544]




rotation:
|***	SAVE	d4/d6-d7	***
	lea      -28(%a7),%a7
	movem.l  %d4,12(%a7)
	movem.l  %d6-%d7,20(%a7)
      move.w   _SOURCEX,%d1            | [1551]
      move.w   %d1,%d2                 | [1552]
|;;   andi.w    #0x0F,d2               | [1553]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1554]
|;;   lsr.w     #4,d1                  | [1555]
	mvz.w    %d1,%d4
	lsr.l    #4,%d4
      moveq.l  #1,%d5                  | [1556]
|***	SAVE	d5	***
	movem.l  %d5,16(%a7)
|;;   lsl.w     d5,d1                  | [1557]
	lsl.l    %d5,%d4

      movea.l  (-10,%a6),%a0           | [1559]
|;;   adda.w    d1,a0                  | [1560]
	ext.l    %d4
	adda.l   %d4,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1562]
      move.w   _SOURCEY,%d0            | [1563]
      move.w   (-106,%a6),%d1          | [1564]
      move.w   %d1,(-22,%a6)           | [1565]
      move.w   (-16,%a6),%d2           | [1566]

      tst.w    (-126,%a6)              | [1568 EQ LT]
      jbeq     upsd_dwn                | [1569]
rot90:
      jblt     top_src                 | [1571]
|;;   neg.w     -16(a6)                | [1572]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   subq.w    #1,d1                  | [1573]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   add.w     d1,d0                  | [1574]
	add.l    %d4,%d0
      mulu.w   %d2,%d0                 | [1575]
|;;   adda.w    d0,a0                  | [1576]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1578]
|;;   lsr.w     #4,d0                  | [1579]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1580]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1581]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1582]
      movea.l  _scrtchp,%a1            | [1583]
|;;   adda.w    -4(a6),a1              | [1584]
	mvs.w    (-4,%a6),%d4
	adda.l   %d4,%a1

      tst.w    (-126,%a6)              | [1586 GT]
      jbgt     top_dwn                 | [1587]
|;;   neg.w     -18(a6)                | [1588]
	move.w   (-18,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1589]
|;;   subq.w    #1,d1                  | [1590]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [1591]
	mulu.w   %d4,%d0
|;;   adda.w    d0,a1                  | [1592]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1594]
      move.w   #0x8000,%d3             | [1595]
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
      move.w   %d3,%d4                 | [1596]
      movea.w  %d3,%a4                 | [1597]
|;;   lsr.w     d2,d3                  | [1598]
	mvz.w    %d3,%d5
	lsr.l    %d2,%d5
      moveq.l  #0,%d0                  | [1599]
      move.w   (-20,%a6),%d2           | [1600]
|***	RESTORE	AFTER d6	***
	movem.l  20(%a7),%d6
      move.w   (-16,%a6),%d6           | [1601]
      movea.l  %a0,%a2                 | [1602]
      movea.l  %a1,%a3                 | [1603]
	move.w   %d5,%d3
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      jbra     rot_nsrc                | [1604]
rot_ylp:
|***	SAVE	d2	***
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1606]
|***	RESTORE	d3	***
	movem.l  4(%a7),%d3
      jbra     rot_srt                 | [1607]
rot_xlp:
      move.w   (%a0),%d7               | [1609]
|;;   and.w     d3,d7                  | [1610 EQ]
	move.w   %d7,%d2
	and.l    %d3,%d2
	tst.w    %d2
      jbeq     rot_nor                 | [1611]
|;;   or.w      d4,d0                  | [1612]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1614 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1613_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1613_1:         
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      jbcc     rot_isrc                | [1615]
rot_ndst:
      move.w   %d0,(%a1)+              | [1617]
      moveq.l  #0,%d0                  | [1618]
rot_isrc:
|;;   adda.w    d6,a0                  | [1620]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,8(%a7)
|;;   dbra      d1,rot_xlp             | [1622]
	tst.w    %d1
	jbeq     _PA_1621_
	subq.l   #1,%d1
	jbra     rot_xlp
	_PA_1621_:         

      move.w   %d0,(%a1)               | [1624]
      moveq.l  #0,%d0                  | [1625]
|;;   adda.w    -18(a6),a3             | [1626]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1627]
      move.w   %a4,%d4                 | [1628]
|;;   ror.w     d5,d3                  | [1629 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1628_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1628_1:         
      jbcc     rnew_src                | [1630]
      addq.l   #2,%a2                  | [1631]
rnew_src:
      movea.l  %a2,%a0                 | [1633]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2
	movem.l  8(%a7),%d6
	lea      12(%a7),%a7
rot_nsrc:
|***	SAVE	d3	***
	lea      -12(%a7),%a7
	movem.l  %d3,4(%a7)
|;;   dbra      d2,rot_ylp             | [1635]
	tst.w    %d2
	jbeq     _PA_1634_
	subq.l   #1,%d2
	jbra     rot_ylp
	_PA_1634_:         


rot_done:
      move.w   (-102,%a6),%d0          | [1639]
      move.w   (-106,%a6),%d1          | [1640]
      move.w   %d1,(-20,%a6)           | [1641]
      move.w   %d1,(-102,%a6)          | [1642]
      move.w   %d0,(-22,%a6)           | [1643]
      move.w   %d0,(-106,%a6)          | [1644]

      move.w   (-92,%a6),%d0           | [1646]
      move.w   (-90,%a6),(-92,%a6)     | [1647]
      move.w   %d0,(-90,%a6)           | [1648]
      move.w   #1,(-94,%a6)            | [1649]

      move.w   (-18,%a6),%d0           | [1651]
      tst.w    (-126,%a6)              | [1652 GT]
      jbgt     rot_nneg                | [1653]
|;;   neg.w     d0                     | [1654]
	move.w   %d0,%d3
	neg.l    %d3
	move.w   %d3,%d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1656]
      move.w   (-4,%a6),%d0            | [1657]
|***	RESTORE	AFTER d3	***
	movem.l  4(%a7),%d3
	lea      12(%a7),%a7
repexit:
      clr.w    _SOURCEX                | [1659]
repexit3:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      movea.l  _scrtchp,%a2            | [1661]
|;;   adda.w    d0,a2                  | [1662]
	mvs.w    %d0,%d1
	adda.l   %d1,%a2
      move.l   %a2,(-10,%a6)           | [1663]
      clr.w    _SOURCEY                | [1664]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1665]


upsd_dwn:
      move.w   (-102,%a6),%d0          | [1669]
|;;   add.w     -24(a6),d0             | [1670]
	move.w   (-24,%a6),%d4
	add.l    %d4,%d0
|;;   subq.w    #1,d0                  | [1671]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1672]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1673]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1674]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1676]
      move.w   %d0,%d2                 | [1677]
|;;   lsr.w     d5,d2                  | [1678]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1679]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1680]
|;;   add.w     -4(a6),d0              | [1681 X]
	move.w   %d0,%d6
	mvz.w    (-4,%a6),%d4
	ori.l    #0xffff0000,%d6
	add.l    %d4,%d6
      movea.l  _scrtchp,%a1            | [1682]
|;;   adda.w    d0,a1                  | [1683]
	ext.l    %d6
	adda.l   %d6,%a1
|***	RESTORE	d4/d6	***
	movem.l  12(%a7),%d4
	movem.l  20(%a7),%d6
      jbra     strtflip                | [1684]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1686]
      move.w   %d2,%d3                 | [1687]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1689]
      moveq.l  #0,%d6                  | [1690]
      moveq.l  #15,%d4                 | [1691]
flip_lp:
|***	SAVE	d4	***
	movem.l  %d4,12(%a7)
|;;   lsr.w     d5,d0                  | [1693 X]
	mvz.w    %d0,%d1
	lsr.l    %d5,%d1
|;;   roxl.w    d5,d6                  | [1694 X]
	move.w   %d1,%d0
	move.b   %d5,%d1
	move.w   %d6,%d2
	moveq    #16,%d4
	and.l    #0x1f,%d1
	jbeq     _PA_1693_
	move.w   %d2,%d3
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d3,%d2
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d2
	_PA_1693_:         
	addx.l   %d3,%d3
	swap     %d3
	move.w   %d2,%d3
	lsl.l    %d4,%d3
	move.w   %d2,%d6
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
|;;   dbra      d4,flip_lp             | [1695]
	move     %CCR,%d1
	tst.w    %d4
	jbeq     _PA_1694_
	subq.l   #1,%d4
	jbra     flip_lp
	_PA_1694_:         
	move     %d1,%CCR

|;;   move.w    d6,-(a1)               | [1697]
	move.w   %d2,-(%a1)
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1698]
	move     %CCR,%d1
	tst.w    %d3
	jbeq     _PA_1697_
	subq.l   #1,%d3
	move     %d1,%CCR
	jbra     line_lp
	_PA_1697_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1700]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1702]
	move     %CCR,%d7
	tst.w    %d1
	jbeq     _PA_1701_
	subq.l   #1,%d1
	move     %d7,%CCR
	jbra     upsd_lp
	_PA_1701_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1704]
      movea.l  _scrtchp,%a1            | [1705]
|;;   adda.w    -4(a6),a1              | [1706]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1707]
      move.w   _SOURCEX,%d0            | [1708]
|;;   add.w     -102(a6),d0            | [1709]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d5
	add.l    %d5,%d7
|;;   neg.w     d0                     | [1710]
	neg.l    %d7
|;;   andi.w    #0xF,d0                | [1711]
	andi.l   #(0xFFFF&0xF),%d7
|;;   move.w    d0,_SOURCEX            | [1712]
	move.w   %d7,_SOURCEX
      clr.w    _SOURCEY                | [1713]
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      rts                              | [1714]





replicat:
|***	SAVE	d6-d7	***
	lea      -16(%a7),%a7
	movem.l  %d6-%d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1722]
      move.w   %d0,%d4                 | [1723]
|;;   andi.w    #0x0F,d0               | [1724]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1725]
|;;   lsr.w     #4,d4                  | [1726]
	mvz.w    %d4,%d6
	lsr.l    #4,%d6
|;;   lsl.w     #1,d4                  | [1727]
	lsl.l    #1,%d6

      move.w   _SOURCEY,%d0            | [1729]
      mulu.w   (-16,%a6),%d0           | [1730]
|;;   add.w     d4,d0                  | [1731]
	move.w   %d0,%d7
	add.l    %d6,%d7
	move.w   %d7,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1732]
|;;   adda.w    d0,a0                  | [1733]
	mvs.w    %d0,%d3
	adda.l   %d3,%a0

      move.w   (-24,%a6),%d2           | [1735]
      move.w   #0x8000,%d3             | [1736]
      move.w   %d3,%d4                 | [1737]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1738]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1740]
      move.w   (-102,%a6),%d1          | [1741]

      move.w   %d2,(-22,%a6)           | [1743]
      move.w   %d1,(-20,%a6)           | [1744]
      movea.l  _scrtchp,%a1            | [1745]
|;;   adda.w    _scrpt2,a1             | [1746]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1748 EQ]
      jbeq     noline                  | [1749]



|;;   addq.w    #1,-106(a6)            | [1753]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1754]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1756]
	move.w   %d1,%d0
	lsl.l    #1,%d0
|;;   addq.w    #3,d1                  | [1757]
	addq.l   #3,%d0
|;;   lsr.w     #4,d1                  | [1758]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d1                  | [1759]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1760]
	addq.l   #2,%d0
|;;   move.w    d1,d5                  | [1761]
	move.w   %d0,%d5
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      moveq.l  #0,%d6                  | [1762]
	move.w   %d0,%d1
oklear:
      move.w   %d6,(%a1)+              | [1764]
|;;   dbra      d5,oklear              | [1765]
	tst.w    %d5
	jbeq     _PA_1764_
	subq.l   #1,%d5
	jbra     oklear
	_PA_1764_:         
      lea      (-2,%a1),%a2            | [1766]
      move.w   %d2,%d5                 | [1767]
|;;   lsl.w     #1,d5                  | [1768]
	move.w   %d5,%d0
	lsl.l    #1,%d0
	move.w   %d0,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1770]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1771]
|;;   dbra      d5,oklear1             | [1772]
	tst.w    %d5
	jbeq     _PA_1771_
	subq.l   #1,%d5
	jbra     oklear1
	_PA_1771_:         

|;;   move.w    d1,d5                  | [1774]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1775]
	mvz.w    %d5,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d5                  | [1776 X]
	subq.l   #1,%d0
	move.w   %d0,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1778]
|;;   dbra      d5,oklear3             | [1779]
	move     %CCR,%d0
	tst.w    %d5
	jbeq     _PA_1778_
	subq.l   #1,%d5
	move     %d0,%CCR
	jbra     oklear3
	_PA_1778_:         
	move     %d0,%CCR
      jbra     noline1                 | [1780]

noline:
|;;   lsr.w     #3,d1                  | [1783]
	mvz.w    %d1,%d0
	lsr.l    #3,%d0
|;;   lsl.w     #1,d1                  | [1784]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1785 X]
	ori.l    #0xffff0000,%d0
	addq.l   #2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
noline1:
      move.w   %d1,(-18,%a6)           | [1787]
|***	RESTORE	AFTER d7	***
	movem.l  12(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1791]
|;;   roxr.l    #1,d7                  | [1792 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1793]
|;;   roxr.l    #1,d7                  | [1794]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1796]
      move.w   _DDA_INC,%d2            | [1797]
      move.w   (-22,%a6),%d5           | [1798]
|;;   subq.w    #1,d5                  | [1799]
	move.w   %d5,%d0
	subq.l   #1,%d0
      move.w   #32767,%d6              | [1800]
	move.w   %d0,%d5
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      16(%a7),%a7
      tst.l    %d7                     | [1801 MI]
      bmi.b    rep_ylop                | [1802]
y_dwn_lp:
|***	SAVE	d0-d2	***
	lea      -12(%a7),%a7
	movem.l  %d0-%d2,(%a7)
|;;   add.w     d2,d6                  | [1804 CC]
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
      jbcc     y_no_drw                | [1805]
      jbsr     yloop                   | [1806]
y_no_drw:
|;;   adda.w    d7,a0                  | [1808]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  8(%a7),%d2
	lea      12(%a7),%a7
|;;   dbra      d5,y_dwn_lp            | [1809]
	tst.w    %d5
	jbeq     _PA_1808_
	subq.l   #1,%d5
	jbra     y_dwn_lp
	_PA_1808_:         
	move.w   #-1,%d5
      jbra     y_rep_don               | [1810]
rep_ylop:
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
|;;   add.w     d2,d6                  | [1812 CC]
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
      jbcc     y_no_rep                | [1813]
      jbsr     yloop                   | [1814]
y_no_rep:
      jbsr     yloop                   | [1816]
|;;   adda.w    d7,a0                  | [1817]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  4(%a7),%d2
	lea      8(%a7),%a7
|;;   dbra      d5,rep_ylop            | [1818]
	tst.w    %d5
	jbeq     _PA_1817_
	subq.l   #1,%d5
	jbra     rep_ylop
	_PA_1817_:         
	move.w   #-1,%d5
y_rep_don:
|***	SAVE	d4-d6	***
	lea      -12(%a7),%a7
	movem.l  %d4-%d6,(%a7)
      move.w   (-102,%a6),%d2          | [1820]
      move.w   _XACC_DDA,%d1           | [1821]
      move.w   _DDA_INC,%d0            | [1822]
      moveq.l  #0,%d3                  | [1823]
repwidcl:
|;;   add.w     d0,d1                  | [1825 CC]
	mvz.w    %d0,%d4
	move.w   %d1,%d5
	ori.l    #0xffff0000,%d5
	add.l    %d4,%d5
	move     %ccr,%d6
	move.w   %d5,%d1
	move     %d6,%ccr
      jbcc     nrepdoub                | [1826]
|;;   addq.w    #1,d3                  | [1827]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrepdoub:
      tst.l    %d7                     | [1829 PL]
      bpl.b    nrpndoub                | [1830]
|;;   addq.w    #1,d3                  | [1831]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1833]
	tst.w    %d2
	jbeq     _PA_1832_
	subq.l   #1,%d2
	jbra     repwidcl
	_PA_1832_:         
      move.w   %d1,_XACC_DDA           | [1834]
      move.w   %d3,(-102,%a6)          | [1835]
      move.w   (-92,%a6),(-106,%a6)    | [1836]


      move.w   (-18,%a6),%d1           | [1839]
      move.w   %d1,(-16,%a6)           | [1840]







repexit1:
      btst     #4,(-96+1,%a6)          | [1849 EQ]
      jbeq     repexit2                | [1850]
      movea.l  _scrtchp,%a0            | [1851]
|;;   adda.w    _scrpt2,a0             | [1852]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1853]
|;;   lea       2(a0,d7.w),a1          | [1854]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
|***	RESTORE	AFTER d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbsr     outlin1                 | [1855]
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
      move.w   _scrpt2,%d0             | [1856]
|;;   add.w     -18(a6),d0             | [1857]
	move.w   %d0,%d2
	move.w   (-18,%a6),%d1
	add.l    %d1,%d2
|;;   move.w    #15,_SOURCEX           | [1858]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
	move.w   %d2,%d0
|***	RESTORE	d1-d2	***
	movem.l  (%a7),%d1-%d2
	lea      8(%a7),%a7
      jbra     repexit3                | [1859]
repexit2:
      move.w   _scrpt2,%d0             | [1861]
|***	RESTORE	d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbra     repexit                 | [1862]
| 2406 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1865]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1866]
      movea.l  %a1,%a4                 | [1867]
      movea.l  %d7,%a5                 | [1868]
      move.w   %d5,(-44,%a6)           | [1869]
      moveq.l  #0,%d0                  | [1870]
      move.w   (-20,%a6),%d5           | [1871]
|;;   subq.w    #1,d5                  | [1872]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1874]
      jbra     nextsrc                 | [1875]
innerlp:
|;;   ror.w     #1,d3                  | [1877 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jbcc     reploop                 | [1878]
nextsrc:
      movea.w  (%a0)+,%a2              | [1880]
reploop:
      move.w   %a2,%d6                 | [1882]
|;;   and.w     d3,d6                  | [1883 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jbne     nrepnor                 | [1884]


repnor:
      tst.l    %d7                     | [1888 MI]
      bmi.b    repnorup                | [1889]
|;;   add.w     d2,d7                  | [1890 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     incsrc                  | [1891]
      jbra     ordone                  | [1892]
repnorup:
|;;   add.w     d2,d7                  | [1894 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     ordone                  | [1895]


|;;   ror.w     #1,d4                  | [1898 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jbcc     ordone                  | [1901]
      move.w   %d0,(%a1)+              | [1902]
      moveq.l  #0,%d0                  | [1903]


      jbra     ordone                  | [1906]
nrepnor:


|;;   add.w     d2,d7                  | [1910 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     o_no_rep                | [1911]


|;;   or.w      d4,d0                  | [1914]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1915 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jbcc     o_no_rep                | [1917]
      move.w   %d0,(%a1)+              | [1918]
      moveq.l  #0,%d0                  | [1919]
o_no_rep:
      tst.l    %d7                     | [1921 PL]
      bpl.b    incsrc                  | [1922]


|;;   or.w      d4,d0                  | [1925]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1927 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jbcc     incsrc                  | [1928]
nextdst:
      move.w   %d0,(%a1)+              | [1930]
      moveq.l  #0,%d0                  | [1931]
incsrc:
|;;   dbra      d5,innerlp             | [1933]
	tst.w    %d5
	jbeq     _PA_1932_
	subq.l   #1,%d5
	jbra     innerlp
	_PA_1932_:         
repdone:
      move.w   %d0,(%a1)               | [1935]
|;;   movem.l   (sp)+,d3-d4/d6         | [1936]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1937]
      movea.l  %a4,%a1                 | [1938]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1939]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1940]
      move.w   (-44,%a6),%d5           | [1941]
      rts                              | [1942]

| 2495 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1947]
	move.w   %d7,%a5
	lea      (0,%a0,%a5.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1949]
	move.w   %d7,%a5
	lea      (0,%a1,%a5.l*1),%a2
|;;   lsr.w     #1,d7                  | [1950]
	mvz.w    %d7,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d7                  | [1951]
	subq.l   #1,%d0
|;;   move.w    d7,-44(a6)             | [1952]
	move.w   %d0,(-44,%a6)
      move.w   (-106,%a6),%d6          | [1953]

      movea.l  %a0,%a5                 | [1955]
|;;   subq.w    #1,d6                  | [1956]
	subq.l   #1,%d6
	move.w   %d0,%d7
out_edge:
      movea.w  %d6,%a4                 | [1958]
      movea.l  %a2,%a3                 | [1959]
      moveq.l  #0,%d5                  | [1960]
      moveq.l  #0,%d6                  | [1961]
      move.l   (%a2),%d1               | [1962]
      lsr.l    #1,%d1                  | [1963]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1968]
      move.b   %d5,%d0                 | [1969]
|;;   ror.l     #1,d0                  | [1970]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1972]
      move.b   %d6,%d2                 | [1973]
      move.l   %d2,%d3                 | [1974]
|;;   ror.l     #1,d3                  | [1975]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1976]
|;;   ror.l     #1,d4                  | [1977]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1979]
      move.l   %d0,%d6                 | [1980]
      eor.l    %d2,%d0                 | [1981]
      eor.l    %d3,%d5                 | [1982]
      eor.l    %d4,%d6                 | [1983]
|;;   rol.l     #1,d5                  | [1984]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1985]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1986]
      or.l     %d6,%d0                 | [1987]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1989]
      move.l   %d1,%d6                 | [1990]
      eor.l    %d2,%d1                 | [1991]
      eor.l    %d3,%d5                 | [1992]
      eor.l    %d4,%d6                 | [1993]
|;;   rol.l     #1,d5                  | [1994]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1995]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [1996]
      or.l     %d5,%d0                 | [1997]
      or.l     %d6,%d0                 | [1998]

      eor.l    %d3,%d2                 | [2000]
      eor.l    %d3,%d4                 | [2001]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [2002]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [2003]
      or.l     %d4,%d0                 | [2004]
      swap     %d0                     | [2005]

      move.w   (%a1),%d6               | [2007]
      move.w   %d6,%d5                 | [2008]
|;;   eor.w     d0,d5                  | [2009]
	move.w   %d5,%d3
	eor.l    %d0,%d3
|;;   and.w     d0,d5                  | [2010]
	and.l    %d0,%d3

      addq.l   #2,%a2                  | [2012]
      move.l   (%a2),%d1               | [2013]
      move.b   (-1,%a2),%d1            | [2014]
|;;   ror.l     #1,d1                  | [2015]
	move.w   %d3,%d5
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [2017]
      move.w   (%a0),%d5               | [2018]
      move.w   %d6,(%a0)+              | [2019]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [2021]
	tst.w    %d7
	jbeq     _PA_2020_
	subq.l   #1,%d7
	jbra     out_loop
	_PA_2020_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [2023]
      movea.l  %a5,%a0                 | [2024]
      move.w   %a4,%d6                 | [2025]
      movea.l  %a3,%a2                 | [2026]
      movea.l  %a2,%a1                 | [2027]
|;;   adda.w    -16(a6),a2             | [2028]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [2029 NE]
      jbne     srt_lin                 | [2030]
      movea.l  %a1,%a2                 | [2031]
srt_lin:
|;;   dbra      d6,out_edge            | [2033]
	tst.w    %d6
	jbeq     _PA_2032_
	subq.l   #1,%d6
	jbra     out_edge
	_PA_2032_:         
	move.w   #-1,%d6
      rts                              | [2034]
| 2606 "vdi/vdi_tblit.S"
concat:
|;;   movem.l   d2/d3,-(sp)            | [2037]
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)



      mulu.w   _v_lin_wr,%d1           | [2041]





      move.w   %d0,%d2                 | [2047]
|;;   andi.w    #0x000f,d0             | [2048]
	andi.l   #(0xFFFF0000!~0x000f),%d0
|;;   andi.w    #0xfff0,d2             | [2049]
	andi.l   #(0xFFFF&0xfff0),%d2
| 2628 "vdi/vdi_tblit.S"
      move.b   _shft_off,%d3           | [2051]
|;;   lsr.w     d3,d2                  | [2052]
	lsr.l    %d3,%d2






      ext.l    %d2                     | [2059]
      add.l    %d2,%d1                 | [2060]
|;;   movem.l   (sp)+,d2/d3            | [2061]
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      rts                              | [2062]




| End section on aligned boundary
	.p2align 2,0
