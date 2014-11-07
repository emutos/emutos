
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
| Translated on Thu Nov  6 22:40:40 2014 by version 2.6.0 of PortAsm translator
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





|;;           .extern _DEV_TAB         | [31]
	.extern	_DEV_TAB
|;;           .extern _SOURCEX,_SOURCEY | [32]
	.extern	_SOURCEX
	.extern	_SOURCEY
|;;           .extern _DESTX,_DESTY    | [33]
	.extern	_DESTX
	.extern	_DESTY
|;;           .extern _DELX,_DELY      | [34]
	.extern	_DELX
	.extern	_DELY
|;;           .extern _FBASE           | [35]
	.extern	_FBASE
|;;           .extern _FWIDTH          | [36]
	.extern	_FWIDTH
|;;           .extern _WRT_MODE        | [37]
	.extern	_WRT_MODE
|;;           .extern _STYLE           | [38]
	.extern	_STYLE
|;;           .extern _R_OFF,_L_OFF    | [39]
	.extern	_R_OFF
	.extern	_L_OFF
|;;           .extern _WEIGHT,_LITEMASK,_SKEWMASK | [40]
	.extern	_WEIGHT
	.extern	_LITEMASK
	.extern	_SKEWMASK
|;;           .extern _DOUBLE          | [41]
	.extern	_DOUBLE
|;;           .extern _CHUP            | [42]
	.extern	_CHUP
|;;           .extern _CLIP            | [43]
	.extern	_CLIP
|;;           .extern _MONO_STATUS     | [44]
	.extern	_MONO_STATUS
|;;           .extern _T_SCLSTS        | [45]
	.extern	_T_SCLSTS
|;;           .extern _TEXT_FG         | [46]
	.extern	_TEXT_FG
|;;           .extern _XMN_CLIP,_XMX_CLIP | [47]
	.extern	_XMN_CLIP
	.extern	_XMX_CLIP
|;;           .extern _YMN_CLIP,_YMX_CLIP | [48]
	.extern	_YMN_CLIP
	.extern	_YMX_CLIP
|;;           .extern _XACC_DDA        | [49]
	.extern	_XACC_DDA
|;;           .extern _DDA_INC         | [50]
	.extern	_DDA_INC
|;;           .extern _scrpt2          | [51]
	.extern	_scrpt2
|;;           .extern _scrtchp         | [52]
	.extern	_scrtchp
|;;           .extern _shift_offset    | [53]
	.extern	_shift_offset






|;;           .extern _v_bas_ad        | [60]
	.extern	_v_bas_ad
|;;           .extern _v_lin_wr        | [61]
	.extern	_v_lin_wr
|;;           .extern _v_planes        | [62]
	.extern	_v_planes





|;;           .globl _deftxbuf         | [68]
	.globl	_deftxbuf
|;;           .globl _scrtsiz          | [69]
	.globl	_scrtsiz

| 328 "vdi/vdi_tblit.S"
| 345 "vdi/vdi_tblit.S"
|;;           .bss                     | [75]
	.p2align 2,0
	.bss
_deftxbuf:
|;;           .ds.b ((2 +6 +8 +15)/16)*2*16*2 +(((2*(2 +6 +8))+3+15)/16)*2*(2*16)+2 +(((2*(2 +6 +8))+3+15)/16)*2 +2 | [77]
	.space	(2+6+8+15)/16*2*16*2+(2*(2+6+8)+3+15)/16*2*(2*16)+2+(2*(2+6+8)+3+15)/16*2+2

|;;           .text                    | [79]
	.p2align 2,0
	.text



_scrtsiz:
|;;           .dc.w ((2 +6 +8 +15)/16)*2*16*2 | [84]
	.short	(2+6+8+15)/16*2*16*2
| 461 "vdi/vdi_tblit.S"
|;;           .text                    | [86]




	.p2align 1,0
_text_blt:
      link.w   %a6,#(-126)             | [92]
|;;   movem.l   d2-d7/a2-a5,-(sp)      | [93]
	lea      -40(%a7),%a7
	movem.l  %d2-%d7/%a2-%a5,(%a7)

      clr.w    (-94,%a6)               | [95]
      move.w   #0,(-6,%a6)             | [96]
      lea      (-96,%a6),%a1           | [97]
      move.w   _STYLE,(%a1)            | [98]
      move.w   _WRT_MODE,-(%a1)        | [99]
      move.w   _SKEWMASK,-(%a1)        | [100]

      move.w   _DELX,-(%a1)            | [102]
      move.w   _DESTX,-(%a1)           | [103]
      move.w   _DELY,-(%a1)            | [104]
      move.w   _DESTY,-(%a1)           | [105]
      move.w   _CHUP,-(%a1)            | [106]

      movea.w  _scrpt2,%a0             | [108]
      move.w   (-106,%a6),%d3          | [109]
      tst.w    _DOUBLE                 | [110 NE]
      jbne     adr_dbl                 | [111]
      move.w   (-102,%a6),%d1          | [112]
      jbra     adr_nodb                | [113]



adr_dbl:
|;;   movem.l   a0/a1,-(sp)            | [118]
	lea      -8(%a7),%a7
	movem.l  %a0-%a1,(%a7)
      move.w   %d3,-(%a7)              | [119]
      move.l   (8,%a6),-(%a7)          | [120]
      jsr      _act_siz                | [121]
      addq.l   #6,%a7                  | [122]
|;;   movem.l   (sp)+,a0/a1            | [123]
	movem.l  (%a7),%a0-%a1
	lea      8(%a7),%a7
      move.w   %d0,%d3                 | [124]
      move.w   %d3,(-92,%a6)           | [125]
      move.w   (-102,%a6),%d2          | [126]
      move.w   _XACC_DDA,%d4           | [127]
      move.w   _DDA_INC,%d0            | [128]
      moveq.l  #0,%d1                  | [129]
      jbra     clc_n_su                | [130]
doub_wid:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   add.w     d0,d4                  | [132 CC]
	mvz.w    %d0,%d2
	move.w   %d4,%d3
	ori.l    #0xffff0000,%d3
	add.l    %d2,%d3
	move     %ccr,%d5
	move.w   %d3,%d4
	move     %d5,%ccr
      jbcc     no_db_wd                | [133]
|;;   addq.w    #1,d1                  | [134]
	move.w   %d1,%d2
	addq.l   #1,%d2
	move.w   %d2,%d1
no_db_wd:
|***	RESTORE	d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
|;;   btst.b    #0,_T_SCLSTS+1         | [136 EQ]
	moveq.l  #0,%d5
	btst     %d5,_T_SCLSTS+1
      jbeq     clc_n_su                | [137]
|;;   addq.w    #1,d1                  | [138]
	move.w   %d1,%d5
	addq.l   #1,%d5
	move.w   %d5,%d1
clc_n_su:
|;;   dbra      d2,doub_wid            | [140]
	tst.w    %d2
	jbeq     _PA_139_
	subq.l   #1,%d2
	jbra     doub_wid
	_PA_139_:         
      move.w   %d1,(-90,%a6)           | [141]
| 526 "vdi/vdi_tblit.S"
      tst.w    (-110,%a6)              | [143 NE]
      jbne     adr_nodb                | [144]



      movea.w  #0,%a0                  | [148]
adr_nodb:
      move.w   %a0,(-2,%a6)            | [150]
      move.w   (-108,%a6),%d2          | [151]
      move.w   (-104,%a6),%d0          | [152]
      move.w   _L_OFF,%d6              | [153]
      move.w   _R_OFF,%d7              | [154]
      clr.w    (-74,%a6)               | [155]
      btst     #0,(-96+1,%a6)          | [156 EQ]
      jbeq     adr_notk                | [157]
      move.w   _WEIGHT,%d4             | [158 NE]
      jbne     adr_th1                 | [159]
|;;   and.w     #0xFFFF-0x1,-96(a6)    | [160]
	move.w   (-96,%a6),%d5
	andi.l   #(0xFFFF&(0xFFFF-0x1)),%d5
	move.w   %d5,(-96,%a6)
adr_th1:
      tst.w    _MONO_STATUS            | [162 NE]
      jbne     adr_notk                | [163]

|;;   add.w     d4,d1                  | [165]
	move.w   %d1,%d5
	add.l    %d4,%d5
	move.w   %d5,%d1
adr_notk:
      btst     #2,(-96+1,%a6)          | [167 EQ]
      jbeq     chk_addr                | [168]

|;;   add.w     d6,d1                  | [170]
	move.w   %d1,%d5
	add.l    %d6,%d5
|;;   add.w     d7,d1                  | [171]
	add.l    %d7,%d5
	move.w   %d5,%d1







chk_addr:
      move.w   (-110,%a6),%d4          | [180 EQ]
      jbeq     chk_clip                | [181]
      move.w   #0,(-4,%a6)             | [182]
|;;   subi.w    #1800,d4               | [183]
	move.w   %d4,%d5
	subi.l   #1800,%d5
	move.w   %d5,%d4
      move.w   %d4,(-126,%a6)          | [184 EQ GT]
      jbeq     add180                  | [185]
      jbgt     add270                  | [186]



|;;   sub.w     d1,d2                  | [190]
	sub.l    %d1,%d2
      move.w   %d2,(-108,%a6)          | [191]
|;;   exg       d1,d3                  | [192]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3
      jbra     chk_clip                | [193]



add180:
|;;   sub.w     d1,d0                  | [198]
	move.w   %d0,%d5
	sub.l    %d1,%d5
|;;   move.w    d0,-104(a6)            | [199]
	move.w   %d5,(-104,%a6)
	move.w   %d5,%d0
      jbra     chk_clip                | [200]



add270:
|;;   exg       d1,d3                  | [205]
	move.l   %d1,%d5
	move.l   %d3,%d1
	move.l   %d5,%d3

| 600 "vdi/vdi_tblit.S"
chk_clip:
      moveq.l  #0,%d5                  | [210]
      move.w   _CLIP,-(%a1)            | [211 EQ]
      jbeq     clp_done                | [212]
      move.w   _XMN_CLIP,-(%a1)        | [213]
      move.w   _YMN_CLIP,-(%a1)        | [214]
      move.w   _XMX_CLIP,-(%a1)        | [215]
      move.w   _YMX_CLIP,-(%a1)        | [216]



      cmp.w    (-116,%a6),%d2          | [220 GE]
      jbge     chk_ymx                 | [221]



|;;   add.w     d3,d2                  | [225]
	add.l    %d3,%d2
      cmp.w    (-116,%a6),%d2          | [226 GT]
      jbgt     chk_xmn                 | [227]
      jbra     upda_dst                | [228]
chk_ymx:
      cmp.w    (-120,%a6),%d2          | [230 GT]
      jbgt     upda_dst                | [231]

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   add.w     d3,d2                  | [233]
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [234]
	subq.l   #1,%d2
|;;   sub.w     -120(a6),d2            | [235 LE]
	ext.l    %d2
	mvs.w    (-120,%a6),%d0
	sub.l    %d0,%d2
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
	lea      4(%a7),%a7
      jble     chk_xmn                 | [236]



|;;   andi.w    #1,d2                  | [240 EQ]
	andi.l   #(0xFFFF&1),%d2
      jbeq     chk_xmn                 | [241]
      btst     #4,(-96+1,%a6)          | [242 NE]
      jbne     chk_xmn                 | [243]
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   rol.w     -100(a6)               | [244]
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
      cmp.w    (-114,%a6),%d0          | [249 GE]
      jbge     chk_xmx                 | [250]



|;;   add.w     d1,d0                  | [254]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   not.w     d5                     | [255]
	move.w   %d2,%d0
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5
      cmp.w    (-114,%a6),%d0          | [256 GT]
      jbgt     clp_done                | [257]
      jbra     upda_dst                | [258]
chk_xmx:
      cmp.w    (-118,%a6),%d0          | [260 GT]
      jbgt     upda_dst                | [261]

|;;   add.w     d1,d0                  | [263]
	move.w   %d0,%d2
	add.l    %d1,%d2
|;;   subq.w    #1,d0                  | [264]
	subq.l   #1,%d2
	move.w   %d2,%d0
      cmp.w    (-118,%a6),%d0          | [265 LE]
      jble     clp_done                | [266]
|;;   not.w     d5                     | [267]
	move.w   %d5,%d2
	not.l    %d2
	move.w   %d2,%d5



clp_done:
      clr.w    (-28,%a6)               | [272]

|;;   move.w    _FWIDTH,-16(a6)        | [274]
	move.w   _FWIDTH,%d2
	move.w   %d2,(-16,%a6)
|;;   move.l    _FBASE,-10(a6)         | [275]
	move.l   _FBASE,%d2
	move.l   %d2,(-10,%a6)

chk_skw:
      move.w   (-96,%a6),%d1           | [278]
|;;   andi.w    #0x4 +0x1 +0x10,d1     | [279 EQ]
	andi.l   #(0xFFFF0000!~(0x4+0x1+0x10)),%d1
	tst.w    %d1
      jbeq     chk_rota                | [280]



chk_chup:
      tst.w    (-110,%a6)              | [285 NE]
      jbne     preblt                  | [286]

      btst     #2,%d1                  | [288 EQ]
      jbeq     preskew                 | [289]
      tst.w    %d5                     | [290 NE]
      jbne     preblt                  | [291]
preskew:
      btst     #4,%d1                  | [293 EQ]
      jbeq     chk_rota                | [294]






preblt:
|***	SAVE	d4	***
	lea      -4(%a7),%a7
	movem.l  %d4,(%a7)
      move.w   _SOURCEX,%d0            | [303]
      move.w   %d0,%d2                 | [304]
|;;   andi.w    #0x0F,d2               | [305]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [306]
|;;   lsr.w     #4,d0                  | [307]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [308]
	lsl.l    #1,%d0

      move.w   _SOURCEY,%d2            | [310]
      move.w   (-106,%a6),(-22,%a6)    | [311]
|;;   add.w     -22(a6),d2             | [312]
	move.w   (-22,%a6),%d3
	add.l    %d3,%d2
|;;   subq.w    #1,d2                  | [313]
	subq.l   #1,%d2

      mulu.w   (-16,%a6),%d2           | [315]
|;;   neg.w     -16(a6)                | [316]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   add.w     d0,d2                  | [317]
	move.w   %d2,%d4
	add.l    %d0,%d4
      movea.l  (-10,%a6),%a0           | [318]
	move.w   %d4,%d2
|;;   lea       (a0,d2.w),a0           | [319]
	move.w   %d2,%a5
	lea      (0,%a0,%a5.l*1),%a0

      move.w   (-102,%a6),%d0          | [321]
      move.w   _WEIGHT,%d1             | [322]
|;;   add.w     d6,d7                  | [323]
	add.l    %d6,%d7
      tst.w    _DOUBLE                 | [324 EQ]
      jbeq     full_wt                 | [325]
|;;   lsr.w     #1,d1                  | [326 NE]
	mvz.w    %d1,%d4
	lsr.l    #1,%d4
	move     %ccr,%d3
	move.w   %d4,%d1
	move     %d3,%ccr
      jbne     ful_wtok                | [327]
|;;   addq.w    #1,d1                  | [328]
	addq.l   #1,%d4
	move.w   %d4,%d1
ful_wtok:
|;;   lsr.w     #1,d7                  | [330]
	and.l    #0xffff,%d7
	lsr.l    #1,%d7
full_wt:
      btst     #0,(-96+1,%a6)          | [332 EQ]
      jbeq     no_smear                | [333]

|;;   add.w     d1,d0                  | [335]
	add.l    %d1,%d0
      move.w   %d1,(-74,%a6)           | [336]
no_smear:

      clr.w    (-26,%a6)               | [339]
      move.w   (-106,%a6),%d1          | [340]
      move.w   (-96,%a6),%d2           | [341]
      btst     #4,%d2                  | [342 EQ]
      jbeq     no_out                  | [343]
      tst.w    _DOUBLE                 | [344 NE]
      jbne     no_out                  | [345]

|;;   addq.w    #3,d0                  | [347]
	addq.l   #3,%d0
|;;   addq.w    #1,-26(a6)             | [348]
	move.w   (-26,%a6),%d4
	addq.l   #1,%d4
	move.w   %d4,(-26,%a6)
|;;   addq.w    #2,-106(a6)            | [349]
	move.w   (-106,%a6),%d4
	addq.l   #2,%d4
	move.w   %d4,(-106,%a6)
|;;   addq.w    #3,d1                  | [350]
	move.w   %d1,%d4
	addq.l   #3,%d4
	move.w   %d4,%d1
no_out:

      move.w   %d0,(-20,%a6)           | [353]
|;;   add.w     d7,d0                  | [354]
	add.l    %d7,%d0
      move.w   %d0,(-102,%a6)          | [355]
|;;   lsr.w     #4,d0                  | [356]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d0                  | [357]
	lsl.l    #1,%d0
|;;   addq.w    #2,d0                  | [358]
	addq.l   #2,%d0
|;;   neg.w     d0                     | [359]
	neg.l    %d0
      move.w   %d0,(-18,%a6)           | [360]
|;;   neg.w     d0                     | [361]
	neg.l    %d0
|;;   subq.w    #1,d1                  | [362]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [363]
	mulu.w   %d4,%d0
      movea.l  _scrtchp,%a1            | [364]
|;;   adda.w    -2(a6),a1              | [365]
	mvs.w    (-2,%a6),%d3
	adda.l   %d3,%a1
      move.l   %a1,(-10,%a6)           | [366]
      btst     #4,%d2                  | [367 NE]
      jbne     do_clear                | [368]
      btst     #2,%d2                  | [369 EQ]
      jbeq     no_clear                | [370]
do_clear:

      movea.l  %a1,%a2                 | [373]
      move.w   %d0,%d3                 | [374]
|;;   sub.w     -18(a6),d3             | [375]
	move.w   (-18,%a6),%d1
	sub.l    %d1,%d3
|;;   lsr.w     #1,d3                  | [376]
	and.l    #0xffff,%d3
	lsr.l    #1,%d3
|;;   subq.w    #1,d3                  | [377]
	subq.l   #1,%d3
      moveq.l  #0,%d1                  | [378]
replp:
      move.w   %d1,(%a2)+              | [380]
|;;   dbra      d3,replp               | [381]
	tst.w    %d3
	jbeq     _PA_380_
	subq.l   #1,%d3
	jbra     replp
	_PA_380_:         

      btst     #4,%d2                  | [383 EQ]
      jbeq     no_clear                | [384]
      tst.w    _DOUBLE                 | [385 NE]
      jbne     no_clear                | [386]

|;;   subq.w    #3,-20(a6)             | [388]
	move.w   (-20,%a6),%d4
	subq.l   #3,%d4
	move.w   %d4,(-20,%a6)
|;;   subq.w    #1,-102(a6)            | [389]
	move.w   (-102,%a6),%d4
	subq.l   #1,%d4
	move.w   %d4,(-102,%a6)
|;;   add.w     -18(a6),d0             | [390]
	move.w   %d0,%d4
	move.w   (-18,%a6),%d3
	add.l    %d3,%d4
	move.w   %d4,%d0



no_clear:
|;;   adda.w    d0,a1                  | [395]
	mvs.w    %d0,%d3
	adda.l   %d3,%a1



      move.w   (-96,%a6),(-122,%a6)    | [399]

      clr.w    (-98,%a6)               | [401]
      move.w   #1,(-84,%a6)            | [402]
      clr.w    (-76,%a6)               | [403]

      move.w   #1,(-86,%a6)            | [405]
      move.w   #2,(-88,%a6)            | [406]
|;;   andi.w    #0x4 +0x1,-96(a6)      | [407]
	move.w   (-96,%a6),%d4
	andi.l   #(0xFFFF&(0x4+0x1)),%d4
	move.w   %d4,(-96,%a6)
|***	RESTORE	AFTER d4	***
	move.l   (%a7)+,%d4

      jbsr     norm_blt                | [409]

      move.w   (-122,%a6),(-96,%a6)    | [411]
|;;   move.w    _WRT_MODE,-98(a6)      | [412]
	move.w   _WRT_MODE,%d2
	move.w   %d2,(-98,%a6)
no_mode:
      move.w   (-18,%a6),%d7           | [414]
|;;   neg.w     d7                     | [415]
	move.w   %d7,%d2
	neg.l    %d2
|;;   move.w    d7,-16(a6)             | [416]
	move.w   %d2,(-16,%a6)
	move.w   %d2,%d7



      btst     #4,(-96+1,%a6)          | [420 EQ]
      jbeq     skip_out                | [421]
      tst.w    _DOUBLE                 | [422 NE]
      jbne     skip_out                | [423]

      movea.l  (-10,%a6),%a0           | [425]
      ext.l    %d7                     | [426]
      add.l    %d7,(-10,%a6)           | [427]
      jbsr     outlin                  | [428]
skip_out:



      clr.w    _SOURCEX                | [433]
      clr.w    _SOURCEY                | [434]


|;;   andi.w    #0xFFFF-(0x4 +0x1),-96(a6) | [437]
	move.w   (-96,%a6),%d2
	andi.l   #(0xFFFF&(0xFFFF-(0x4+0x1))),%d2
	move.w   %d2,(-96,%a6)



chk_rota:
      tst.w    (-110,%a6)              | [442 EQ]
      jbeq     chk_db                  | [443]

      jbsr     rotation                | [445]

chk_db:
      tst.w    _DOUBLE                 | [448 EQ]
      jbeq     do_clip                 | [449]

      jbsr     replicat                | [451]







do_clip:
      btst     #0,(-96+1,%a6)          | [461 EQ]
      jbeq     no_thik                 | [462]
      move.w   _WEIGHT,%d0             | [463]
      tst.w    _MONO_STATUS            | [464 NE]
      jbne     do_cl_nt                | [465]
|;;   add.w     d0,-102(a6)            | [466]
	move.w   (-102,%a6),%d2
	add.l    %d0,%d2
	move.w   %d2,(-102,%a6)
do_cl_nt:
      move.w   %d0,(-74,%a6)           | [468]
no_thik:
      tst.w    (-112,%a6)              | [470 NE]
      jbne     clip_adjust             | [471]
      clr.w    (-114,%a6)              | [472]
      clr.w    (-116,%a6)              | [473]
|;;   move.w    _DEV_TAB,-118(a6)      | [474]
	move.w   _DEV_TAB,%d2
	move.w   %d2,(-118,%a6)
|;;   move.w    _DEV_TAB+2,-120(a6)    | [475]
	move.w   _DEV_TAB+2,%d2
	move.w   %d2,(-120,%a6)

clip_adjust:
      move.w   (-108,%a6),%d0          | [478]
      cmp.w    (-116,%a6),%d0          | [479 GE]
      jbge     ymn_fine                | [480]
|;;   add.w     -106(a6),d0            | [481]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-116,%a6),%d0          | [482 LE]
      jble     upda_dst                | [483]
mn_clipy:
|;;   sub.w     -116(a6),d0            | [485]
	move.w   (-116,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-106,%a6),%d1          | [486]
|;;   move.w    d0,-106(a6)            | [487]
	move.w   %d7,(-106,%a6)
|;;   sub.w     d0,d1                  | [488]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEY            | [489]
	move.w   _SOURCEY,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEY
      move.w   (-116,%a6),%d0          | [490]
      move.w   %d0,(-108,%a6)          | [491]
ymn_fine:
      cmp.w    (-120,%a6),%d0          | [493 GT]
      jbgt     upda_dst                | [494]
mx_clipy:
|;;   add.w     -106(a6),d0            | [496]
	move.w   %d0,%d7
	move.w   (-106,%a6),%d2
	add.l    %d2,%d7
|;;   subq.w    #1,d0                  | [497]
	subq.l   #1,%d7
	move.w   %d7,%d0
      cmp.w    (-120,%a6),%d0          | [498 LE]
      jble     ymx_fine                | [499]

|;;   sub.w     -120(a6),d0            | [501]
	move.w   (-120,%a6),%d2
	sub.l    %d2,%d7
|;;   sub.w     d0,-106(a6)            | [502]
	move.w   (-106,%a6),%d2
	sub.l    %d7,%d2
	move.w   %d2,(-106,%a6)
ymx_fine:
      move.w   (-104,%a6),%d0          | [504]
      cmp.w    (-114,%a6),%d0          | [505 GE]
      jbge     xmn_fine                | [506]
|;;   add.w     -102(a6),d0            | [507]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d2
	add.l    %d2,%d7
	move.w   %d7,%d0
      cmp.w    (-114,%a6),%d0          | [508 LE]
      jble     upda_dst                | [509]

mn_clipx:
|;;   sub.w     -114(a6),d0            | [512]
	move.w   (-114,%a6),%d2
	sub.l    %d2,%d7
      move.w   (-102,%a6),%d1          | [513]
|;;   move.w    d0,-102(a6)            | [514]
	move.w   %d7,(-102,%a6)
|;;   sub.w     d0,d1                  | [515]
	move.w   %d1,%d2
	sub.l    %d7,%d2
|;;   add.w     d1,_SOURCEX            | [516]
	move.w   _SOURCEX,%d7
	add.l    %d2,%d7
	move.w   %d7,_SOURCEX
      move.w   (-114,%a6),%d0          | [517]
      move.w   %d0,(-104,%a6)          | [518]
xmn_fine:
      cmp.w    (-118,%a6),%d0          | [520 GT]
      jbgt     upda_dst                | [521]
mx_clipx:
|***	SAVE	d3	***
	lea      -4(%a7),%a7
	movem.l  %d3,(%a7)
|;;   add.w     -102(a6),d0            | [523]
	move.w   %d0,%d3
	move.w   (-102,%a6),%d2
	add.l    %d2,%d3
|;;   subq.w    #1,d0                  | [524]
	subq.l   #1,%d3
	move.w   %d3,%d0
      cmp.w    (-118,%a6),%d0          | [525 LE]
      jble     scrn_blt                | [526]

|;;   sub.w     -118(a6),d0            | [528]
	move.w   (-118,%a6),%d2
	sub.l    %d2,%d3
|;;   sub.w     d0,-102(a6)            | [529]
	move.w   (-102,%a6),%d2
	sub.l    %d3,%d2
	move.w   %d2,(-102,%a6)

      jbra     scrn_blt                | [531]
| 932 "vdi/vdi_tblit.S"
scrn_blt:
|;;   move.w    _TEXT_FG,-84(a6)       | [534]
	move.w   _TEXT_FG,%d2
	move.w   %d2,(-84,%a6)
      move.w   (-6,%a6),(-76,%a6)      | [535]
      move.w   _v_planes,%d0           | [536]
      move.w   %d0,(-86,%a6)           | [537]
|;;   add.w     d0,d0                  | [538]
	move.w   %d0,%d3
	add.l    %d0,%d3
|;;   move.w    d0,-88(a6)             | [539]
	move.w   %d3,(-88,%a6)

      move.w   _SOURCEX,%d0            | [541]
      move.w   %d0,%d2                 | [542]
|;;   andi.w    #0x0F,d2               | [543]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [544]
|;;   lsr.w     #3,d0                  | [545]
	mvz.w    %d0,%d3
	lsr.l    #3,%d3
|;;   andi.w    #0xFFFE,d0             | [546]
	andi.l   #(0xFFFF&0xFFFE),%d3
      move.w   _SOURCEY,%d2            | [547]

      move.w   (-106,%a6),(-22,%a6)    | [549]
|;;   add.w     -22(a6),d2             | [550]
	move.w   (-22,%a6),%d7
	add.l    %d7,%d2
|;;   subq.w    #1,d2                  | [551]
	subq.l   #1,%d2
      mulu.w   (-16,%a6),%d2           | [552]
|;;   neg.w     -16(a6)                | [553]
	move.w   %d3,%d0
	move.w   (-16,%a6),%d3
	neg.l    %d3
	move.w   %d3,(-16,%a6)
|;;   add.w     d0,d2                  | [554]
	move.w   %d2,%d3
	add.l    %d0,%d3



      movea.l  (-10,%a6),%a0           | [558]
|;;   adda.w    d2,a0                  | [559]
	ext.l    %d3
	adda.l   %d3,%a0
      move.l   %a0,(-10,%a6)           | [560]

      move.w   (-102,%a6),(-20,%a6)    | [562]

      move.w   (-108,%a6),%d1          | [564]
|;;   add.w     -106(a6),d1            | [565]
	move.w   %d1,%d3
	move.w   (-106,%a6),%d7
	add.l    %d7,%d3
	move.w   %d3,%d1
|***	RESTORE	d3	***
	move.l   (%a7)+,%d3
|;;   subq.w    #1,d1                  | [566 MI]
	move.w   %d1,%d7
	subq.l   #1,%d7
	move.w   %d7,%d1
      bmi.b    upda_dst                | [567]

      move.w   (-104,%a6),%d0          | [569 MI]
      bmi.b    upda_dst                | [570]

      jsr      concat                  | [572]



      movea.l  _v_bas_ad,%a1           | [576]
      adda.l   %d1,%a1                 | [577]

      move.l   %a1,(-14,%a6)           | [579]
      move.w   %d0,(-26,%a6)           | [580]

      move.w   _v_lin_wr,%d0           | [582]
|;;   neg.w     d0                     | [583]
	move.w   %d0,%d3
	neg.l    %d3
|;;   move.w    d0,-18(a6)             | [584]
	move.w   %d3,(-18,%a6)

      jbsr     norm_blt                | [586]

upda_dst:
      move.w   _DELX,%d1               | [589]


      tst.w    _DOUBLE                 | [592 EQ]
      jbeq     upda_ndb                | [593]



      move.w   (-90,%a6),%d1           | [597]
      move.w   (-92,%a6),%d3           | [598]

      tst.w    (-94,%a6)               | [600 EQ]
      jbeq     upda_ndb                | [601]
|;;   exg       d1, d3                 | [602]
	move.l   %d1,%d2
	move.l   %d3,%d1
	move.l   %d2,%d3





upda_ndb:
|;;   btst.b    #0,_STYLE+1            | [609 EQ]
	moveq.l  #0,%d2
	btst     %d2,_STYLE+1
      jbeq     upda_ntk                | [610]
      tst.w    _MONO_STATUS            | [611 NE]
      jbne     upda_ntk                | [612]
|;;   add.w     _WEIGHT,d1             | [613]
	move.w   _WEIGHT,%d2
	add.l    %d2,%d1
upda_ntk:
      tst.w    (-110,%a6)              | [615 GT]
      jbgt     ck90                    | [616]
|;;   add.w     d1,_DESTX              | [617]
	move.w   _DESTX,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [618]
ck90:
      tst.w    (-126,%a6)              | [620 EQ GT]
      jbeq     ck180                   | [621]
      jbgt     ck270                   | [622]
|;;   sub.w     d1,_DESTY              | [623]
	move.w   _DESTY,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTY
      jbra     blt_done                | [624]
ck180:
|;;   sub.w     d1,_DESTX              | [626]
	move.w   _DESTX,%d2
	sub.l    %d1,%d2
	move.w   %d2,_DESTX
      jbra     blt_done                | [627]
ck270:
|;;   add.w     d1,_DESTY              | [629]
	move.w   _DESTY,%d2
	add.l    %d1,%d2
	move.w   %d2,_DESTY
blt_done:
|;;   movem.l   (sp)+,d2-d7/a2-a5      | [631]
	movem.l  (%a7),%d2-%d7/%a2-%a5
	lea      40(%a7),%a7
      unlk     %a6                     | [632]
      rts                              | [633]



|;;           .text                    | [638]
| 1046 "vdi/vdi_tblit.S"
|;;   lf_tab: .dc.w 0xFFFF             | [640]
lf_tab:
	.short	0xFFFF
|;;   rf_tab: .dc.w 0x7FFF             | [641]
rf_tab:
	.short	0x7FFF
|;;                   .dc.w 0x3FFF     | [642]
	.short	0x3FFF
|;;                   .dc.w 0x1FFF     | [643]
	.short	0x1FFF
|;;                   .dc.w 0x0FFF     | [644]
	.short	0x0FFF
|;;                   .dc.w 0x07FF     | [645]
	.short	0x07FF
|;;                   .dc.w 0x03FF     | [646]
	.short	0x03FF
|;;                   .dc.w 0x01FF     | [647]
	.short	0x01FF
|;;                   .dc.w 0x00FF     | [648]
	.short	0x00FF
|;;                   .dc.w 0x007F     | [649]
	.short	0x007F
|;;                   .dc.w 0x003F     | [650]
	.short	0x003F
|;;                   .dc.w 0x001F     | [651]
	.short	0x001F
|;;                   .dc.w 0x000F     | [652]
	.short	0x000F
|;;                   .dc.w 0x0007     | [653]
	.short	0x0007
|;;                   .dc.w 0x0003     | [654]
	.short	0x0003
|;;                   .dc.w 0x0001     | [655]
	.short	0x0001
|;;                   .dc.w 0x0000     | [656]
	.short	0x0000

|;;           .text                    | [658]
| 1077 "vdi/vdi_tblit.S"
	.p2align 1,0
norm_blt:
|***	SAVE	d5	***
	lea      -4(%a7),%a7
	movem.l  %d5,(%a7)
      move.w   (-26,%a6),%d1           | [661]
|;;   sub.w     -24(a6),d1             | [662]
	move.w   %d1,%d5
	move.w   (-24,%a6),%d3
	sub.l    %d3,%d5
	move.w   %d5,%d1
      move.w   %d1,%d0                 | [663 PL]
      bpl.b    do_rot                  | [664]



|;;   neg.w     d1                     | [668]
	neg.l    %d5
|;;   ori.w     #0x8000,d1             | [669]
	move.w   %d5,%d1
	ori.l    #(0xFFFF&0x8000),%d1
|;;   addi.w    #16,d0                 | [670]
	move.w   %d0,%d5
	addi.l   #16,%d5
	move.w   %d5,%d0
do_rot:
      move.w   %d1,(-42,%a6)           | [672]

|;;   lsl.w     #1,d0                  | [674]
	move.w   %d0,%d5
	lsl.l    #1,%d5
      lea      lf_tab,%a2              | [675]
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d0           | [676]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d0
|;;   not.w     d0                     | [677]
	move.w   %d0,%d5
	not.l    %d5
|;;   move.w    d0,-40(a6)             | [678]
	move.w   %d5,(-40,%a6)



get_mask:
      move.w   (-26,%a6),%d0           | [683]
|;;   lsl.w     #1,d0                  | [684]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),-38(a6)      | [685]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d3
	move.w   %d3,(-38,%a6)

|;;   lsr.w     #1,d0                  | [687]
	and.l    #0xffff,%d5
	lsr.l    #1,%d5
|;;   add.w     -20(a6),d0             | [688]
	move.w   (-20,%a6),%d3
	add.l    %d3,%d5


|;;   move.w    d0,d2                  | [691]
	move.w   %d5,%d2
|;;   sub.w     -74(a6),d2             | [692]
	move.w   %d5,%d0
	move.w   %d2,%d5
	move.w   (-74,%a6),%d3
	sub.l    %d3,%d5
|;;   andi.w    #0xF,d2                | [693]
	andi.l   #(0xFFFF&0xF),%d5
|;;   lsl.w     #1,d2                  | [694]
	lsl.l    #1,%d5
	move.w   %d5,%d2
|;;   move.w    (a2,d2.w),d2           | [695]
	move.w   %d2,%a5
	move.w   (0,%a2,%a5.l*1),%d2
|;;   not.w     d2                     | [696]
	move.w   %d2,%d5
	not.l    %d5
|;;   move.w    d2,-34(a6)             | [697]
	move.w   %d5,(-34,%a6)
      clr.w    %d4                     | [698]
      move.w   #0x8000,(-80,%a6)       | [699]
      moveq.l  #(-1),%d3               | [700]
	move.w   %d5,%d2


      cmp.w    #0x10,%d0               | [703 HI]
      bhi.b    doub_des                | [704]



|;;   lsl.w     #1,d0                  | [708]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d1           | [709]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d1
|;;   not.w     d1                     | [710]
	move.w   %d1,%d5
	not.l    %d5
|;;   and.w     d1,-38(a6)             | [711]
	move.w   (-38,%a6),%d7
	and.l    %d5,%d7
	move.w   %d7,(-38,%a6)

      moveq.l  #(-4),%d7               | [713]
	move.w   %d5,%d1
      jbra     msk_done                | [714]



doub_des:
      move.w   %d0,%d1                 | [719]
|;;   lsr.w     #4,d1                  | [720]
	mvz.w    %d1,%d5
	lsr.l    #4,%d5
|;;   subq.w    #1,d1                  | [721]
	subq.l   #1,%d5
	move.w   %d5,%d1
      move.w   %d1,%d3                 | [722 NE]
      jbne     mlt_dest                | [723]



      move.w   (-24,%a6),%d1           | [727]
|;;   add.w     -20(a6),d1             | [728]
	move.w   %d1,%d5
	move.w   (-20,%a6),%d7
	add.l    %d7,%d5
	move.w   %d5,%d1
      cmp.w    #0x20,%d1               | [729 CC]
      jbcc     mlt_dest                | [730]
      moveq.l  #0,%d7                  | [731]
      jbra     do_ritem                | [732]

mlt_dest:
      tst.w    (-42,%a6)               | [735 LT]
      jblt     mltleft                 | [736]
      moveq.l  #4,%d7                  | [737]
      jbra     do_ritem                | [738]
mltleft:
      moveq.l  #8,%d7                  | [740]
do_ritem:
|;;   andi.w    #0x000F,d0             | [742 NE]
	andi.l   #(0xFFFF0000!~0x000F),%d0
	tst.w    %d0
      jbne     not_null                | [743]
|;;   subq.w    #1,d3                  | [744]
	move.w   %d3,%d5
	subq.l   #1,%d5
      moveq.l  #0x10,%d0               | [745]
	move.w   %d5,%d3
not_null:
|;;   lsl       #1,d0                  | [747]
	move.w   %d0,%d5
	lsl.l    #1,%d5
	move.w   %d5,%d0
|;;   move.w    (a2,d0.w),d4           | [748]
	move.w   %d0,%a5
	move.w   (0,%a2,%a5.l*1),%d4
|;;   not.w     d4                     | [749]
	move.w   %d4,%d5
	not.l    %d5
	move.w   %d5,%d4

msk_done:
      move.w   %d7,(-124,%a6)          | [752]
      move.w   %d3,(-28,%a6)           | [753]
      move.w   %d4,(-36,%a6)           | [754]
|;;   addq.w    #2,d3                  | [755]
	move.w   %d3,%d5
	addq.l   #2,%d5
	move.w   %d5,%d3
      cmp.w    %d2,%d4                 | [756 CS]
      jbcs     msk0                    | [757]
|;;   addq.w    #1,d3                  | [758]
	addq.l   #1,%d5
	move.w   %d5,%d3
msk0:
      move.w   %d3,(-32,%a6)           | [760]
      move.w   %d3,(-30,%a6)           | [761]
|***	RESTORE	d5	***
	move.l   (%a7)+,%d5
      jbra     plane_loop              | [762]

| 1197 "vdi/vdi_tblit.S"
wrmappin:
|;;           .dc.b 4*0,4*0,4*1,4*1    | [767]
	.byte	4*0,4*0,4*1,4*1
|;;           .dc.b 4*2,4*2,4*3,4*3    | [768]
	.byte	4*2,4*2,4*3,4*3
|;;           .dc.b 4*4,4*4,4*4,4*4    | [769]
	.byte	4*4,4*4,4*4,4*4
|;;           .dc.b 4*5,4*5,4*6,4*6    | [770]
	.byte	4*5,4*5,4*6,4*6


toptable:
|;;           .dc.l top0,top3,top4,top7,top6,top1,topd | [774]
	.long	top0,top3,top4,top7,top6,top1,topd
twoptble:
|;;           .dc.l twop0,twop3,twop4,twop7,twop6,twop1,twopd | [776]
	.long	twop0,twop3,twop4,twop7,twop6,twop1,twopd


	.p2align 1,0
plane_loop:
|***	SAVE	d1-d2/a0	***
	lea      -12(%a7),%a7
	movem.l  %d1-%d2/%a0,(%a7)
      move.w   (-98,%a6),%d0           | [780]
|;;   lsr.w     -84(a6)                | [781 X]
	mvz.w    (-84,%a6),%d1
	lsr.l    #1,%d1
	move.w   %d1,(-84,%a6)
|;;   roxl.w    #1,d0                  | [782]
	move.w   %d0,%d1
	addx.l   %d1,%d1
|;;   lsr.w     -76(a6)                | [783 X]
	mvz.w    (-76,%a6),%d2
	lsr.l    #1,%d2
	move.w   %d2,(-76,%a6)
|;;   roxl.w    #1,d0                  | [784]
	addx.l   %d1,%d1
	move.w   %d1,%d0



|;;   move.b    wrmappin(pc,d0.w),d0   | [788]
	move.w   %d0,%a5
	movea.l  #wrmappin,%a0
	move.b   (0,%a0,%a5.l*1),%d0
|;;   move.l    toptable(pc,d0.w),a2   | [789]
	move.w   %d0,%a5
	movea.l  #toptable,%a0
	movea.l  (0,%a0,%a5.l*1),%a2
      movea.l  %a2,%a3                 | [790]
      movea.l  %a2,%a5                 | [791]

|;;   movea.l   twoptble(pc,d0.w),a4   | [793]
	move.w   %d0,%a4
	movea.l  #twoptble,%a0
	movea.l  (0,%a0,%a4.l*1),%a4



      move.w   (-96,%a6),%d0           | [797 EQ]
      jbeq     wrt_char                | [798]

      btst     #1,%d0                  | [800 EQ]
      jbeq     no_lite                 | [801]



|;;   move.w    _LITEMASK,-78(a6)      | [805]
	move.w   _LITEMASK,%d1
	move.w   %d1,(-78,%a6)
      move.l   %a5,(-52,%a6)           | [806]
      lea      liteop,%a5              | [807]

      move.l   %a2,(-64,%a6)           | [809]
      lea      liteopwf,%a2            | [810]

      move.l   %a4,(-72,%a6)           | [812]
      lea      liteopw,%a4             | [813]

no_lite:
      btst     #0,%d0                  | [816 EQ]
      jbeq     no_thick                | [817]



      clr.w    (-82,%a6)               | [821]

      move.l   %a5,(-48,%a6)           | [823]
      lea      thknop,%a5              | [824]

      move.l   %a2,(-60,%a6)           | [826]
      lea      thknopwf,%a2            | [827]

      move.l   %a4,(-68,%a6)           | [829]
      lea      thknopw,%a4             | [830]

no_thick:
      btst     #2,%d0                  | [833 EQ]
      jbeq     wrt_char                | [834]



      move.w   (-100,%a6),(-80,%a6)    | [838]
      move.l   %a5,(-56,%a6)           | [839]
      lea      skewop,%a5              | [840]

      tst.w    (-124,%a6)              | [842 EQ GT]
      jbgt     wrt_char                | [843]
      jbeq     not_sngl                | [844]
      clr.w    (-28,%a6)               | [845]
      moveq.l  #0,%d7                  | [846]
      jbra     chng_blt                | [847]
not_sngl:
|;;   cmpi.w    #0x10,-20(a6)          | [849 LS]
	move.w   (-20,%a6),%d1
	cmp.w    #0x10,%d1
      bls.b    wrt_char                | [850]

      tst.w    (-42,%a6)               | [852 LT]
      jblt     chng_left               | [853]
      moveq.l  #4,%d7                  | [854]
      jbra     chng_blt                | [855]
chng_left:
      moveq.l  #8,%d7                  | [857]
chng_blt:
      move.w   %d7,(-124,%a6)          | [859]

wrt_char:
      move.w   (-124,%a6),%d7          | [862 LE]
      jble     set_chr_hgt             | [863]
      movea.l  %a2,%a5                 | [864]
      move.w   (-18,%a6),%d5           | [865]
set_chr_hgt:
      move.w   (-22,%a6),%d3           | [867]
|;;   subq.w    #1,d3                  | [868]
	move.w   %d3,%d1
	subq.l   #1,%d1

|;;   movea.l   blttype+4(pc,d7.w),a2  | [870]
	move.w   %d7,%a2
	movea.l  #blttype+4,%a0
	movea.l  (0,%a0,%a2.l*1),%a2
	move.w   %d1,%d3
|***	RESTORE	AFTER d1-d2/a0	***
	movem.l  (%a7),%d1-%d2/%a0
	lea      12(%a7),%a7
      jmp      (%a2)                   | [871]
next_plane:
|***	SAVE	d0-d1	***
	lea      -8(%a7),%a7
	movem.l  %d0-%d1,(%a7)
|;;   subq.w    #1,-86(a6)             | [873 LE]
	mvs.w    (-86,%a6),%d0
	subq.l   #1,%d0
	move     %ccr,%d1
	move.w   %d0,(-86,%a6)
	move     %d1,%ccr
      jble     no_more_planes          | [874]
      movea.l  (-10,%a6),%a0           | [875]
      movea.l  (-14,%a6),%a1           | [876]
      addq.l   #2,%a1                  | [877]
      move.l   %a1,(-14,%a6)           | [878]
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      btst     #2,(-96+1,%a6)          | [879 EQ]
      jbeq     plane_loop              | [880]
      jbra     norm_blt                | [881]
no_more_planes:
|***	RESTORE	d0-d1	***
	movem.l  (%a7),%d0-%d1
	lea      8(%a7),%a7
      rts                              | [883]
blttype:
|;;           .dc.l sgl_loop, dbl_loop, mlt_rite, mlt_left | [885]
	.long	sgl_loop,dbl_loop,mlt_rite,mlt_left




	.p2align 1,0
sgl_loop:
      move.w   (-38,%a6),%d2           | [892]
      move.w   (-16,%a6),%d6           | [893]
      move.w   (-18,%a6),%d7           | [894]
      lea      sgl_rtn,%a3             | [895]
      jbra     sgl_1st_scan            | [896]

sgl_lp:
|;;   adda.w    d6,a0                  | [899]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [900]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
sgl_1st_scan:
      move.w   (%a1),%d4               | [902]
| 1342 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [904]


      move.w   (-42,%a6),%d1           | [907 MI]
      bmi.b    left_rol                | [908]
      lsr.l    %d1,%d0                 | [909]
      jbra     end_ro                  | [910]
left_rol:
      lsl.l    %d1,%d0                 | [912]
end_ro:
      swap     %d0                     | [914]
      move.w   %d0,%d1                 | [915]
      swap     %d0                     | [916]

      jmp      (%a5)                   | [918]

sgl_rtn:
      move.w   %d1,(%a1)               | [921]
|;;   dbra      d3,sgl_lp              | [922]
	tst.w    %d3
	jbeq     _PA_921_
	subq.l   #1,%d3
	jbra     sgl_lp
	_PA_921_:         
      jbra     next_plane              | [923]




dbl_loop:
      movea.l  %a3,%a4                 | [930]
      move.w   (-16,%a6),%d6           | [931]
      move.w   (-18,%a6),%d7           | [932]
      lea      dbl_2wrd,%a2            | [933]
      move.l   %a2,%d5                 | [934]
      lea      dbl_1wrd,%a3            | [935]
      jbra     dbl_1st_scan            | [936]

dbl_lp:
|;;   adda.w    d6,a0                  | [939]
	mvs.w    %d6,%d0
	adda.l   %d0,%a0
|;;   adda.w    d7,a1                  | [940]
	mvs.w    %d7,%d0
	adda.l   %d0,%a1
|;;   exg       a3,d5                  | [941]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
dbl_1st_scan:
      move.w   (%a1),%d4               | [943]
| 1390 "vdi/vdi_tblit.S"
      move.l   (%a0),%d0               | [945]


      move.w   (-42,%a6),%d1           | [948 MI]
      bmi.b    dbleft                  | [949]
      lsr.l    %d1,%d0                 | [950]
      jbra     dblendr                 | [951]
dbleft:
      lsl.l    %d1,%d0                 | [953]
dblendr:
      swap     %d0                     | [955]
      move.w   %d0,%d1                 | [956]
      swap     %d0                     | [957]
      move.w   (-38,%a6),%d2           | [958]

      jmp      (%a5)                   | [960]

dbl_1wrd:
|***	SAVE	d0/a0	***
	lea      -8(%a7),%a7
	movem.l  %d0/%a0,(%a7)
      move.w   %d1,(%a1)               | [963]
      move.w   %d0,%d1                 | [964]

      movea.w  (-88,%a6),%a2           | [966]
|;;   move.w    (a1,a2.w),d4           | [967]
	move.w   %a2,%a0
	move.w   (0,%a1,%a0.l*1),%d4

      move.w   (-36,%a6),%d2           | [969]

|;;   exg       a3,d5                  | [971]
	move.l   %a3,%d0
	move.l   %d5,%a3
	move.l   %d0,%d5
|***	RESTORE	AFTER d0/a0	***
	movem.l  (%a7),%d0/%a0
	lea      8(%a7),%a7
      jmp      (%a4)                   | [972]

dbl_2wrd:
|***	SAVE	a0	***
	lea      -4(%a7),%a7
	movem.l  %a0,(%a7)
      movea.w  (-88,%a6),%a2           | [975]
|;;   move.w    d1,(a1,a2.w)           | [976]
	move.w   %a2,%a0
	move.w   %d1,(0,%a1,%a0.l*1)
|***	RESTORE	AFTER a0	***
	move.l   (%a7)+,%a0

|;;   dbra      d3,dbl_lp              | [978]
	tst.w    %d3
	jbeq     _PA_977_
	subq.l   #1,%d3
	jbra     dbl_lp
	_PA_977_:         
      jbra     next_plane              | [979]





left_loop:
      movea.l  %d6,%a0                 | [987]
      movea.l  %d7,%a1                 | [988]
|;;   adda.w    -16(a6),a0             | [989]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [990]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [991]
|;;   rol.w     -78(a6)                | [992]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [994 NE]
      jbne     skewopw                 | [995]
mlt_left:
      move.l   %a0,%d6                 | [997]
      move.l   %a1,%d7                 | [998]
      move.w   (-28,%a6),(-44,%a6)     | [999]







      move.l   (%a0)+,%d0              | [1007]


      move.w   (%a1),%d4               | [1010]

      move.w   (-42,%a6),%d1           | [1012]
      lsl.l    %d1,%d0                 | [1013]

      swap     %d0                     | [1015]
      move.w   %d0,%d1                 | [1016]
      swap     %d0                     | [1017]

      move.w   (-38,%a6),%d2           | [1019]

      lea      lft_lfrng,%a3           | [1021]
      jmp      (%a5)                   | [1022]

lft_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1025]
|;;   adda.w    -88(a6),a1             | [1026]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1028]
      lea      lft_wrds,%a3            | [1029]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_lef:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1034]
|;;   and.w     d2,d4                  | [1035]
	move.w   %d4,%d3
	and.l    %d2,%d3
      move.w   (%a0)+,%d0              | [1036]





      swap     %d0                     | [1042]
      move.w   %d1,%d0                 | [1043]
      swap     %d0                     | [1044]

      move.w   (-42,%a6),%d1           | [1046]
      lsl.l    %d1,%d0                 | [1047]

      swap     %d0                     | [1049]
      move.w   %d0,%d1                 | [1050]
      swap     %d0                     | [1051]

|;;   not.w     d2                     | [1053]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1054]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1055]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1056]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1058]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1060 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     lef_don                 | [1061]

      jmp      (%a4)                   | [1063]

lft_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1066]
|;;   adda.w    -88(a6),a1             | [1067]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_lef                | [1069]

lef_don:
      move.w   (-36,%a6),%d2           | [1072]

      lea      lft_rfrng,%a3           | [1074]
      jmp      (%a5)                   | [1075]

lft_rfrng:
      move.w   %d1,(%a1)               | [1078]

|;;   dbra      d3,left_loop           | [1080]
	tst.w    %d3
	jbeq     _PA_1079_
	subq.l   #1,%d3
	jbra     left_loop
	_PA_1079_:         
      jbra     next_plane              | [1081]






rite_loop:
      movea.l  %d6,%a0                 | [1090]
      movea.l  %d7,%a1                 | [1091]
|;;   adda.w    -16(a6),a0             | [1092]
	mvs.w    (-16,%a6),%d6
	adda.l   %d6,%a0
|;;   adda.w    d5,a1                  | [1093]
	mvs.w    %d5,%d6
	adda.l   %d6,%a1
      clr.w    (-82,%a6)               | [1094]
|;;   rol.w     -78(a6)                | [1095]
	clr.l    %d7
	move.w   (-78,%a6),%d6
	move.w   %d6,%d7
	lsl.l    #1,%d6
	lsr.l    #8,%d7
	lsr.l    #7,%d7
	or.l     %d7,%d6
	move.w   %d6,(-78,%a6)

      btst     #2,(-96+1,%a6)          | [1097 NE]
      jbne     skewopw                 | [1098]
mlt_rite:
      move.l   %a0,%d6                 | [1100]
      move.l   %a1,%d7                 | [1101]
      move.w   (-28,%a6),(-44,%a6)     | [1102]
      move.w   (%a0)+,%d0              | [1103]



      swap     %d0                     | [1107]

      move.w   (%a1),%d4               | [1109]

      move.w   (-42,%a6),%d1           | [1111]
      lsr.l    %d1,%d0                 | [1112]
      swap     %d0                     | [1113]
      move.w   %d0,%d1                 | [1114]
      swap     %d0                     | [1115]

      move.w   (-38,%a6),%d2           | [1117]

      lea      rgt_lfrng,%a3           | [1119]
      jmp      (%a5)                   | [1120]

rgt_lfrng:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1123]
|;;   adda.w    -88(a6),a1             | [1124]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

      move.w   (-40,%a6),%d2           | [1126]
      lea      rgt_wrds,%a3            | [1127]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0



word_rit:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
      move.w   %d0,%d4                 | [1132]
|;;   and.w     d2,d4                  | [1133]
	move.w   %d4,%d3
	and.l    %d2,%d3
      swap     %d0                     | [1134]
      move.w   (%a0)+,%d0              | [1135]



      swap     %d0                     | [1139]

      move.w   (-42,%a6),%d1           | [1141]
      lsr.l    %d1,%d0                 | [1142]
      swap     %d0                     | [1143]
      move.w   %d0,%d1                 | [1144]
      swap     %d0                     | [1145]

|;;   not.w     d2                     | [1147]
	move.w   %d2,%d5
	not.l    %d5
|;;   and.w     d2,d1                  | [1148]
	move.w   %d3,%d4
	move.w   %d1,%d3
	and.l    %d5,%d3
|;;   not.w     d2                     | [1149]
	not.l    %d5
|;;   eor.w     d4,d1                  | [1150]
	eor.l    %d4,%d3

      move.w   (%a1),%d4               | [1152]
	move.w   %d3,%d1
	move.w   %d5,%d2

|;;   subq.w    #1,-44(a6)             | [1154 LT]
	mvs.w    (-44,%a6),%d3
	subq.l   #1,%d3
	move     %ccr,%d5
	move.w   %d3,(-44,%a6)
	move     %d5,%ccr
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jblt     rite_don                | [1155]

      jmp      (%a4)                   | [1157]

rgt_wrds:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   %d1,(%a1)               | [1160]
|;;   adda.w    -88(a6),a1             | [1161]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1

|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      jbra     word_rit                | [1163]

rite_don:
      move.w   (-36,%a6),%d2           | [1166]

      lea      rgt_rfrng,%a3           | [1168]
      jmp      (%a5)                   | [1169]

rgt_rfrng:
      move.w   %d1,(%a1)               | [1172]

|;;   dbra      d3,rite_loop           | [1174]
	tst.w    %d3
	jbeq     _PA_1173_
	subq.l   #1,%d3
	jbra     rite_loop
	_PA_1173_:         
      jbra     next_plane              | [1175]






top0:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1184]
	move.w   %d2,%d0
	not.l    %d0
|;;   and.w     d2,d4                  | [1185]
	move.w   %d0,%d2
	move.w   %d4,%d0
	and.l    %d2,%d0
|;;   not.w     d2                     | [1186]
	move.w   %d0,%d4
	move.w   %d2,%d0
	not.l    %d0
      move.w   %d4,%d1                 | [1187]
	move.w   %d0,%d2
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1188]

top1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d2                     | [1191]
	move.w   %d2,%d0
	not.l    %d0
|;;   or.w      d2,d1                  | [1192]
	move.w   %d0,%d2
	and.l    #0xffff,%d0
	or.l     %d0,%d1
|;;   not.w     d2                     | [1193]
	not.l    %d0
|;;   and.w     d4,d1                  | [1194]
	move.w   %d0,%d2
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1195]

top3:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1198]
	move.w   %d1,%d0
	eor.l    %d4,%d0
|;;   and.w     d2,d1                  | [1199]
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1200]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1201]

top4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1204]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   not.w     d1                     | [1205]
	not.l    %d0
|;;   and.w     d4,d1                  | [1206]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1207]

top6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1210]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   eor.w     d4,d1                  | [1211]
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1212]

top7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d2,d1                  | [1215]
	move.w   %d1,%d0
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1216]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1217]

topd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1220]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d2,d1                  | [1221]
	and.l    %d2,%d0
|;;   or.w      d4,d1                  | [1222]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1223]





twop0:
      moveq.l  #0,%d1                  | [1231]
      jmp      (%a3)                   | [1232]

twop1:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   and.w     d4,d1                  | [1235]
	move.w   %d1,%d0
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1236]

twop3:
      jmp      (%a3)                   | [1239]

twop4:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1242]
	move.w   %d1,%d0
	not.l    %d0
|;;   and.w     d4,d1                  | [1243]
	and.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1244]

twop6:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   eor.w     d4,d1                  | [1247]
	move.w   %d1,%d0
	eor.l    %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1248]

twop7:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   or.w      d4,d1                  | [1251]
	mvz.w    %d4,%d0
	or.l     %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1252]

twopd:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
|;;   not.w     d1                     | [1255]
	move.w   %d1,%d0
	not.l    %d0
|;;   or.w      d4,d1                  | [1256]
	or.l     %d4,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
      jmp      (%a3)                   | [1257]
| 1722 "vdi/vdi_tblit.S"
thknop:
|***	SAVE	d2	***
	lea      -4(%a7),%a7
	movem.l  %d2,(%a7)
|;;   movem.l   d5-d7,-(sp)            | [1260]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
|;;   and.w     d2,d1                  | [1261]
	move.w   %d1,%d6
	and.l    %d2,%d6
|;;   move.w    -34(a6),d6             | [1262]
	move.w   %d6,%d1
	move.w   (-34,%a6),%d6
      btst     #0,(-80+1,%a6)          | [1263 EQ]
      jbeq     thk00                   | [1264]
|;;   ori.b     #0x10,ccr              | [1265 X]
	move.w   %CCR,%d2
	ori.l    #(0x1F&0x10),%d2
	move.w   %d2,%CCR
|;;   roxr.w    #1,d6                  | [1266 CC]
	move.w   %d6,%d2
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d2,%d6
	lsr.l    #1,%d6
      jbcc     thk01                   | [1267]
      move.w   #0x8000,%d6             | [1268]
thk01:
      move.w   %d6,(-34,%a6)           | [1270]
thk00:
      tst.w    (-28,%a6)               | [1272 MI]
      bmi.b    thk0                    | [1273]
      cmp.w    (-36,%a6),%d6           | [1274 CC]
      jbcc     thk0                    | [1275]
|;;   and.w     d6,d0                  | [1276]
	and.l    %d6,%d0
      jbra     thk1                    | [1277]
thk0:
      clr.w    %d0                     | [1279]
|;;   and.w     d6,d1                  | [1280]
	move.w   %d1,%d7
	and.l    %d6,%d7
	move.w   %d7,%d1
thk1:
      move.w   (-74,%a6),%d6           | [1282]
      move.w   %d2,%d5                 | [1283]
      swap     %d5                     | [1284]
      move.w   (-36,%a6),%d5           | [1285]
      lsl.l    %d6,%d5                 | [1286]

      swap     %d5                     | [1288]
      move.w   %d5,%d7                 | [1289]
      swap     %d5                     | [1290]

|;;   and.w     d7,d1                  | [1292]
	move.w   %d1,%d2
	and.l    %d7,%d2
|;;   and.w     d5,d0                  | [1293]
	and.l    %d5,%d0
|;;   swap      d1                     | [1294]
	move.w   %d2,%d1
	swap     %d1
      move.w   %d0,%d1                 | [1295]
|;;   subq.w    #1,d6                  | [1296]
	subq.l   #1,%d6
thkoplp:
      move.l   %d1,%d0                 | [1298]
      lsr.l    #1,%d0                  | [1299]
      or.l     %d0,%d1                 | [1300]
|;;   dbra      d6,thkoplp             | [1301]
	tst.w    %d6
	jbeq     _PA_1300_
	subq.l   #1,%d6
	jbra     thkoplp
	_PA_1300_:         

      move.w   %d1,%d0                 | [1303]
      swap     %d1                     | [1304]
|;;   movem.l   (sp)+,d5-d7            | [1305]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-48,%a6),%a2           | [1306]
|***	RESTORE	AFTER d2	***
	move.l   (%a7)+,%d2
      jmp      (%a2)                   | [1307]

| 1786 "vdi/vdi_tblit.S"
thknopw:
|;;   movem.l   d5-d6,-(sp)            | [1312]
	lea      -8(%a7),%a7
	movem.l  %d5-%d6,(%a7)
|;;   subq.w    #1,-30(a6)             | [1313 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk2                    | [1314]
|;;   and.w     -34(a6),d1             | [1315]
	move.w   %d1,%d6
	move.w   (-34,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
thk2:
      move.w   (-74,%a6),%d6           | [1317]
      swap     %d1                     | [1318]
      clr.w    %d1                     | [1319]
      move.l   %d1,%d5                 | [1320]
|;;   subq.w    #1,d6                  | [1321]
	subq.l   #1,%d6
thkopwlp:
      lsr.l    #1,%d5                  | [1323]
      or.l     %d5,%d1                 | [1324]
|;;   dbra      d6,thkopwlp            | [1325]
	tst.w    %d6
	jbeq     _PA_1324_
	subq.l   #1,%d6
	jbra     thkopwlp
	_PA_1324_:         

      move.w   %d1,%d5                 | [1327]
      swap     %d1                     | [1328]
|;;   or.w      -82(a6),d1             | [1329]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1330]
|;;   movem.l   (sp)+,d5-d6            | [1331]
	movem.l  (%a7),%d5-%d6
	lea      8(%a7),%a7
      movea.l  (-68,%a6),%a2           | [1332]
      jmp      (%a2)                   | [1333]

| 1825 "vdi/vdi_tblit.S"
thknopwf:
|;;   movem.l   d5-d7,-(sp)            | [1338]
	lea      -12(%a7),%a7
	movem.l  %d5-%d7,(%a7)
      move.w   (-34,%a6),%d6           | [1339]
|;;   subq.w    #1,-30(a6)             | [1340 EQ MI]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      bmi.b    thk3                    | [1341]
      jbeq     thk4                    | [1342]

|;;   subq.w    #1,-30(a6)             | [1344 NE]
	move.w   (-30,%a6),%d5
	subq.l   #1,%d5
	move.w   %d5,(-30,%a6)
      jbne     thk11                   | [1345]
|;;   and.w     d6,d1                  | [1346]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1
thk11:
|;;   and.w     -38(a6),d1             | [1348]
	move.w   %d1,%d6
	move.w   (-38,%a6),%d5
	and.l    %d5,%d6
	move.w   %d6,%d1
      jbra     thk7                    | [1349]

thk3:
      clr.w    %d1                     | [1352]
      jbra     thk5                    | [1353]

thk4:
|;;   and.w     d6,d1                  | [1356]
	move.w   %d1,%d5
	and.l    %d6,%d5
	move.w   %d5,%d1

thk5:
|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      tst.w    (-80,%a6)               | [1359 MI]
      bmi.b    thk6                    | [1360]
      move.w   (-28,%a6),%d5           | [1361]
|;;   addq.w    #2,d5                  | [1362]
	addq.l   #2,%d5
      move.w   (-36,%a6),%d7           | [1363]
|;;   ori.b     #0x10,ccr              | [1364 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d7                  | [1365 CC]
	move.w   %d7,%d0
	addx.l   %d7,%d7
	swap     %d7
	move.w   %d0,%d7
	lsr.l    #1,%d7
      jbcc     thk33                   | [1366]
      move.w   #0x8000,%d7             | [1367]
|;;   addq.w    #1,d5                  | [1368]
	addq.l   #1,%d5
thk33:
|;;   cmpi.w    #1,-38(a6)             | [1370 NE]
	move.w   (-38,%a6),%d0
	cmp.w    #1,%d0
      jbne     thk34                   | [1371]
|;;   subq.w    #1,d5                  | [1372]
	subq.l   #1,%d5
thk34:
|;;   ori.b     #0x10,ccr              | [1374 X]
	move.w   %CCR,%d0
	ori.l    #(0x1F&0x10),%d0
	move.w   %d0,%CCR
|;;   roxr.w    #1,d6                  | [1375 CC]
	move.w   %d6,%d0
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d0,%d6
	lsr.l    #1,%d6
      jbcc     thk31                   | [1376]
      move.w   #0x8000,%d6             | [1377]
thk31:
      cmp.w    %d6,%d7                 | [1379 CS]
      jbcs     thk35                   | [1380]
|;;   addq.w    #1,d5                  | [1381]
	addq.l   #1,%d5
thk35:
      move.w   %d6,(-34,%a6)           | [1383]
      move.w   %d5,(-32,%a6)           | [1384]
thk6:
      move.w   (-32,%a6),(-30,%a6)     | [1386]
|***	RESTORE	AFTER d0	***
	move.l   (%a7)+,%d0
thk7:

      move.w   (-74,%a6),%d6           | [1389]
      swap     %d1                     | [1390]
      clr.w    %d1                     | [1391]
      move.l   %d1,%d5                 | [1392]
|;;   subq.w    #1,d6                  | [1393]
	subq.l   #1,%d6
thkopwl:
      lsr.l    #1,%d5                  | [1395]
      or.l     %d5,%d1                 | [1396]
|;;   dbra      d6,thkopwl             | [1397]
	tst.w    %d6
	jbeq     _PA_1396_
	subq.l   #1,%d6
	jbra     thkopwl
	_PA_1396_:         

      move.w   %d1,%d5                 | [1399]
      swap     %d1                     | [1400]
|;;   or.w      -82(a6),d1             | [1401]
	mvz.w    (-82,%a6),%d6
	or.l     %d6,%d1
      move.w   %d5,(-82,%a6)           | [1402]
|;;   and.w     d2,d1                  | [1403]
	move.w   %d1,%d5
	and.l    %d2,%d5
	move.w   %d5,%d1
|;;   movem.l   (sp)+,d5-d7            | [1404]
	movem.l  (%a7),%d5-%d7
	lea      12(%a7),%a7
      movea.l  (-60,%a6),%a2           | [1405]
      jmp      (%a2)                   | [1406]

| 1908 "vdi/vdi_tblit.S"
liteop:
|***	SAVE	d2-d3	***
	lea      -8(%a7),%a7
	movem.l  %d2-%d3,(%a7)
|;;   and.w     -78(a6),d1             | [1411]
	move.w   %d1,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   and.w     -78(a6),d0             | [1412]
	move.w   %d3,%d1
	move.w   %d0,%d3
	move.w   (-78,%a6),%d2
	and.l    %d2,%d3
|;;   rol.w     -78(a6)                | [1413]
	move.w   %d3,%d0
	clr.l    %d3
	move.w   (-78,%a6),%d2
	move.w   %d2,%d3
	lsl.l    #1,%d2
	lsr.l    #8,%d3
	lsr.l    #7,%d3
	or.l     %d3,%d2
	move.w   %d2,(-78,%a6)
      movea.l  (-52,%a6),%a2           | [1414]
|***	RESTORE	AFTER d2-d3	***
	movem.l  (%a7),%d2-%d3
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1415]

liteopw:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1418]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-72,%a6),%a2           | [1419]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1420]

liteopwf:
|***	SAVE	d0/d2	***
	lea      -8(%a7),%a7
	movem.l  %d0/%d2,(%a7)
|;;   and.w     -78(a6),d1             | [1423]
	move.w   %d1,%d2
	move.w   (-78,%a6),%d0
	and.l    %d0,%d2
      movea.l  (-64,%a6),%a2           | [1424]
	move.w   %d2,%d1
|***	RESTORE	AFTER d0/d2	***
	movem.l  (%a7),%d0/%d2
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1425]

| 1941 "vdi/vdi_tblit.S"
skewop:
|***	SAVE	d3/d5	***
	lea      -8(%a7),%a7
	movem.l  %d3/%d5,(%a7)
|;;   rol.w     -80(a6)                | [1430 CC]
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
      jbcc     no_shift                | [1431]

      swap     %d1                     | [1433]
      move.w   %d0,%d1                 | [1434]
      lsr.l    #1,%d1                  | [1435]

      swap     %d2                     | [1437]
      move.w   (-36,%a6),%d2           | [1438]
      lsr.l    #1,%d2                  | [1439]
      move.w   %d2,(-36,%a6)           | [1440]
      swap     %d2                     | [1441]
      move.w   %d2,(-38,%a6)           | [1442 EQ]
      jbeq     nxt_word                | [1443]
      move.w   (-42,%a6),%d0           | [1444 MI]
      bmi.b    dec_rol                 | [1445]
ror_add:
|;;   addq.w    #1,d0                  | [1447]
	move.w   %d0,%d3
	addq.l   #1,%d3
	move.w   %d3,%d0
new_shif:
      move.w   %d0,(-42,%a6)           | [1449]
      move.w   %d1,%d0                 | [1450]
      swap     %d1                     | [1451]
no_shift:
      movea.l  (-56,%a6),%a2           | [1453]
|***	RESTORE	AFTER d3/d5	***
	movem.l  (%a7),%d3/%d5
	lea      8(%a7),%a7
      jmp      (%a2)                   | [1454]


dec_rol:
      tst.b    %d0                     | [1458 EQ]
      jbeq     begn_ror                | [1459]
|;;   subq.w    #1,d0                  | [1460]
	move.w   %d0,%d3
	subq.l   #1,%d3
	move.w   %d3,%d0
      jbra     new_shif                | [1461]
begn_ror:
      clr.w    %d0                     | [1463]
      jbra     ror_add                 | [1464]



nxt_word:
      move.w   %d2,(-36,%a6)           | [1469]
      swap     %d2                     | [1470]
      move.w   %d2,(-38,%a6)           | [1471]
|;;   adda.w    -88(a6),a1             | [1472]
	mvs.w    (-88,%a6),%d0
	adda.l   %d0,%a1
      move.w   (%a1),%d4               | [1473]

      moveq.l  #15,%d0                 | [1475]
|;;   sub.w     -42(a6),d0             | [1476]
	move.w   %d0,%d5
	move.w   (-42,%a6),%d3
	sub.l    %d3,%d5
|;;   ori.w     #0x8000,d0             | [1477]
	move.w   %d5,%d0
	ori.l    #(0xFFFF&0x8000),%d0
      move.w   %d0,(-42,%a6)           | [1478]
      jbra     no_shift                | [1479]






skewopw:
|;;   rol.w     -80(a6)                | [1488 CC]
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
      jbcc     do_shift                | [1489]

      tst.w    (-42,%a6)               | [1491 MI]
      bmi      mlt_left                | [1492]
      jbra     mlt_rite                | [1493]
do_shift:
|;;   ori       #0x10,ccr              | [1495 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    -40(a6)                | [1496]
	clr.l    %d7
	move.w   (-40,%a6),%d6
	subx.l   %d7,%d7
	move.w   %d6,%d7
	move.l   %d7,%d6
	asr.l    #1,%d6
	move.w   %d6,(-40,%a6)

      move.w   (-36,%a6),%d0           | [1498]
      cmp.w    #0xFFFF,%d0             | [1499 EQ]
      jbeq     inc_rite                | [1500]

|;;   ori       #0x10,ccr              | [1502 X]
	move.w   %CCR,%d6
	ori.l    #(0x1F&0x10),%d6
	move.w   %d6,%CCR
|;;   roxr.w    #1,d0                  | [1503]
	move.w   %d0,%d6
	move.w   %d6,%d7
	addx.l   %d6,%d6
	swap     %d6
	move.w   %d7,%d6
	lsr.l    #1,%d6
|;;   move.w    d0,-36(a6)             | [1504]
	move.w   %d6,(-36,%a6)

do_left:
      move.w   (-42,%a6),%d0           | [1507]
      tst.b    %d0                     | [1508 NE]
      jbne     no_rota                 | [1509]
      move.w   #0x8000,(-40,%a6)       | [1510]
no_rota:
      move.w   (-38,%a6),%d1           | [1512]
|;;   lsr.w     #1,d1                  | [1513 EQ]
	mvz.w    %d1,%d6
	lsr.l    #1,%d6
	move     %ccr,%d7
	move.w   %d6,%d1
	move     %d7,%ccr
      jbeq     wnxt_wrd                | [1514]

      move.w   %d1,(-38,%a6)           | [1516]

      tst.w    %d0                     | [1518 MI]
      bmi.b    wdec_rol                | [1519]
|;;   addq.w    #1,-42(a6)             | [1520]
	move.w   (-42,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-42,%a6)
      jbra     mlt_rite                | [1521]
wdec_rol:
      tst.b    %d0                     | [1523 EQ]
      jbeq     set_msk                 | [1524]
|;;   subq.w    #1,-42(a6)             | [1525]
	move.w   (-42,%a6),%d0
	subq.l   #1,%d0
	move.w   %d0,(-42,%a6)
      jbra     mlt_left                | [1526]
set_msk:
      move.w   #1,(-42,%a6)            | [1528]
      jbra     mlt_rite                | [1529]

inc_rite:
|;;   addq.w    #1,-28(a6)             | [1532]
	move.w   (-28,%a6),%d6
	addq.l   #1,%d6
	move.w   %d6,(-28,%a6)
      move.w   #0x8000,(-36,%a6)       | [1533]
      jbra     do_left                 | [1534]

wnxt_wrd:
      move.w   #0xFFFF,(-38,%a6)       | [1537]

|;;   subq.w    #1,-28(a6)             | [1539]
	move.w   (-28,%a6),%d2
	subq.l   #1,%d2
	move.w   %d2,(-28,%a6)
|;;   adda.w    -88(a6),a1             | [1540]
	mvs.w    (-88,%a6),%d2
	adda.l   %d2,%a1

      moveq.l  #15,%d2                 | [1542]
|;;   sub.w     d0,d2                  | [1543]
	move.w   %d2,%d6
	sub.l    %d0,%d6
|;;   ori.w     #0x8000,d2             | [1544]
	move.w   %d6,%d2
	ori.l    #(0xFFFF&0x8000),%d2
      move.w   %d2,(-42,%a6)           | [1545]

      jbra     mlt_left                | [1547]




rotation:
|***	SAVE	d4/d6-d7	***
	lea      -28(%a7),%a7
	movem.l  %d4,12(%a7)
	movem.l  %d6-%d7,20(%a7)
      move.w   _SOURCEX,%d1            | [1554]
      move.w   %d1,%d2                 | [1555]
|;;   andi.w    #0x0F,d2               | [1556]
	andi.l   #(0xFFFF&0x0F),%d2
      move.w   %d2,(-24,%a6)           | [1557]
|;;   lsr.w     #4,d1                  | [1558]
	mvz.w    %d1,%d4
	lsr.l    #4,%d4
      moveq.l  #1,%d5                  | [1559]
|***	SAVE	d5	***
	movem.l  %d5,16(%a7)
|;;   lsl.w     d5,d1                  | [1560]
	lsl.l    %d5,%d4

      movea.l  (-10,%a6),%a0           | [1562]
|;;   adda.w    d1,a0                  | [1563]
	ext.l    %d4
	adda.l   %d4,%a0

      move.w   (-102,%a6),(-20,%a6)    | [1565]
      move.w   _SOURCEY,%d0            | [1566]
      move.w   (-106,%a6),%d1          | [1567]
      move.w   %d1,(-22,%a6)           | [1568]
      move.w   (-16,%a6),%d2           | [1569]

      tst.w    (-126,%a6)              | [1571 EQ LT]
      jbeq     upsd_dwn                | [1572]
rot90:
      jblt     top_src                 | [1574]
|;;   neg.w     -16(a6)                | [1575]
	move.w   (-16,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-16,%a6)
|;;   subq.w    #1,d1                  | [1576]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   add.w     d1,d0                  | [1577]
	add.l    %d4,%d0
      mulu.w   %d2,%d0                 | [1578]
|;;   adda.w    d0,a0                  | [1579]
	ext.l    %d0
	adda.l   %d0,%a0
top_src:
      move.w   (-106,%a6),%d0          | [1581]
|;;   lsr.w     #4,d0                  | [1582]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1583]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1584]
	addq.l   #2,%d0
      move.w   %d0,(-18,%a6)           | [1585]
      movea.l  _scrtchp,%a1            | [1586]
|;;   adda.w    -4(a6),a1              | [1587]
	mvs.w    (-4,%a6),%d4
	adda.l   %d4,%a1

      tst.w    (-126,%a6)              | [1589 GT]
      jbgt     top_dwn                 | [1590]
|;;   neg.w     -18(a6)                | [1591]
	move.w   (-18,%a6),%d4
	neg.l    %d4
	move.w   %d4,(-18,%a6)
      move.w   (-102,%a6),%d1          | [1592]
|;;   subq.w    #1,d1                  | [1593]
	move.w   %d1,%d4
	subq.l   #1,%d4
|;;   mulu      d1,d0                  | [1594]
	mulu.w   %d4,%d0
|;;   adda.w    d0,a1                  | [1595]
	ext.l    %d0
	adda.l   %d0,%a1
top_dwn:
      move.w   (-24,%a6),%d2           | [1597]
      move.w   #0x8000,%d3             | [1598]
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
      move.w   %d3,%d4                 | [1599]
      movea.w  %d3,%a4                 | [1600]
|;;   lsr.w     d2,d3                  | [1601]
	mvz.w    %d3,%d5
	lsr.l    %d2,%d5
      moveq.l  #0,%d0                  | [1602]
      move.w   (-20,%a6),%d2           | [1603]
|***	RESTORE	AFTER d6	***
	movem.l  20(%a7),%d6
      move.w   (-16,%a6),%d6           | [1604]
      movea.l  %a0,%a2                 | [1605]
      movea.l  %a1,%a3                 | [1606]
	move.w   %d5,%d3
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      jbra     rot_nsrc                | [1607]
rot_ylp:
|***	SAVE	d2	***
	movem.l  %d2,(%a7)
      move.w   (-22,%a6),%d1           | [1609]
|***	RESTORE	d3	***
	movem.l  4(%a7),%d3
      jbra     rot_srt                 | [1610]
rot_xlp:
      move.w   (%a0),%d7               | [1612]
|;;   and.w     d3,d7                  | [1613 EQ]
	move.w   %d7,%d2
	and.l    %d3,%d2
	tst.w    %d2
      jbeq     rot_nor                 | [1614]
|;;   or.w      d4,d0                  | [1615]
	or.l     %d4,%d0
rot_nor:
|;;   ror.w     d5,d4                  | [1617 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1616_1
	and.l    #15,%d2
	move.w   %d4,%d6
	swap     %d6
	move.w   %d4,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d4
	swap     %d6
	lsl.l    #1,%d6
	_PA_1616_1:         
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      jbcc     rot_isrc                | [1618]
rot_ndst:
      move.w   %d0,(%a1)+              | [1620]
      moveq.l  #0,%d0                  | [1621]
rot_isrc:
|;;   adda.w    d6,a0                  | [1623]
	mvs.w    %d6,%d2
	adda.l   %d2,%a0
rot_srt:
|***	SAVE	d6	***
	movem.l  %d6,8(%a7)
|;;   dbra      d1,rot_xlp             | [1625]
	tst.w    %d1
	jbeq     _PA_1624_
	subq.l   #1,%d1
	jbra     rot_xlp
	_PA_1624_:         

      move.w   %d0,(%a1)               | [1627]
      moveq.l  #0,%d0                  | [1628]
|;;   adda.w    -18(a6),a3             | [1629]
	mvs.w    (-18,%a6),%d2
	adda.l   %d2,%a3
      movea.l  %a3,%a1                 | [1630]
      move.w   %a4,%d4                 | [1631]
|;;   ror.w     d5,d3                  | [1632 CC]
	move.b   %d5,%d2
	tst.b    %d2
	jbeq     _PA_1631_1
	and.l    #15,%d2
	move.w   %d3,%d6
	swap     %d6
	move.w   %d3,%d6
	lsr.l    %d2,%d6
	move.w   %d6,%d3
	swap     %d6
	lsl.l    #1,%d6
	_PA_1631_1:         
      jbcc     rnew_src                | [1633]
      addq.l   #2,%a2                  | [1634]
rnew_src:
      movea.l  %a2,%a0                 | [1636]
|***	RESTORE	AFTER d2/d6	***
	movem.l  (%a7),%d2
	movem.l  8(%a7),%d6
	lea      12(%a7),%a7
rot_nsrc:
|***	SAVE	d3	***
	lea      -12(%a7),%a7
	movem.l  %d3,4(%a7)
|;;   dbra      d2,rot_ylp             | [1638]
	tst.w    %d2
	jbeq     _PA_1637_
	subq.l   #1,%d2
	jbra     rot_ylp
	_PA_1637_:         


rot_done:
      move.w   (-102,%a6),%d0          | [1642]
      move.w   (-106,%a6),%d1          | [1643]
      move.w   %d1,(-20,%a6)           | [1644]
      move.w   %d1,(-102,%a6)          | [1645]
      move.w   %d0,(-22,%a6)           | [1646]
      move.w   %d0,(-106,%a6)          | [1647]

      move.w   (-92,%a6),%d0           | [1649]
      move.w   (-90,%a6),(-92,%a6)     | [1650]
      move.w   %d0,(-90,%a6)           | [1651]
      move.w   #1,(-94,%a6)            | [1652]

      move.w   (-18,%a6),%d0           | [1654]
      tst.w    (-126,%a6)              | [1655 GT]
      jbgt     rot_nneg                | [1656]
|;;   neg.w     d0                     | [1657]
	move.w   %d0,%d3
	neg.l    %d3
	move.w   %d3,%d0
rot_nneg:
      move.w   %d0,(-16,%a6)           | [1659]
      move.w   (-4,%a6),%d0            | [1660]
|***	RESTORE	AFTER d3	***
	movem.l  4(%a7),%d3
	lea      12(%a7),%a7
repexit:
      clr.w    _SOURCEX                | [1662]
repexit3:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
      movea.l  _scrtchp,%a2            | [1664]
|;;   adda.w    d0,a2                  | [1665]
	mvs.w    %d0,%d1
	adda.l   %d1,%a2
      move.l   %a2,(-10,%a6)           | [1666]
      clr.w    _SOURCEY                | [1667]
|***	RESTORE	d1	***
	move.l   (%a7)+,%d1
      rts                              | [1668]


upsd_dwn:
      move.w   (-102,%a6),%d0          | [1672]
|;;   add.w     -24(a6),d0             | [1673]
	move.w   (-24,%a6),%d4
	add.l    %d4,%d0
|;;   subq.w    #1,d0                  | [1674]
	subq.l   #1,%d0
|;;   lsr.w     #4,d0                  | [1675]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     d5,d0                  | [1676]
	lsl.l    %d5,%d0
|;;   addq.w    #2,d0                  | [1677]
	addq.l   #2,%d0

      move.w   %d0,(-18,%a6)           | [1679]
      move.w   %d0,%d2                 | [1680]
|;;   lsr.w     d5,d2                  | [1681]
	lsr.l    %d5,%d2
|;;   subq.w    #1,d2                  | [1682]
	subq.l   #1,%d2
      mulu.w   %d1,%d0                 | [1683]
|;;   add.w     -4(a6),d0              | [1684 X]
	move.w   %d0,%d6
	mvz.w    (-4,%a6),%d4
	ori.l    #0xffff0000,%d6
	add.l    %d4,%d6
      movea.l  _scrtchp,%a1            | [1685]
|;;   adda.w    d0,a1                  | [1686]
	ext.l    %d6
	adda.l   %d6,%a1
|***	RESTORE	d4/d6	***
	movem.l  12(%a7),%d4
	movem.l  20(%a7),%d6
      jbra     strtflip                | [1687]
upsd_lp:
|***	SAVE	d1	***
	movem.l  %d1,(%a7)
      movea.l  %a0,%a2                 | [1689]
      move.w   %d2,%d3                 | [1690]
line_lp:
|***	SAVE	d2-d3	***
	movem.l  %d2-%d3,4(%a7)
      move.w   (%a2)+,%d0              | [1692]
      moveq.l  #0,%d6                  | [1693]
      moveq.l  #15,%d4                 | [1694]
flip_lp:
|***	SAVE	d4	***
	movem.l  %d4,12(%a7)
|;;   lsr.w     d5,d0                  | [1696 X]
	mvz.w    %d0,%d1
	lsr.l    %d5,%d1
|;;   roxl.w    d5,d6                  | [1697 X]
	move.w   %d1,%d0
	move.b   %d5,%d1
	move.w   %d6,%d2
	moveq    #16,%d4
	and.l    #0x1f,%d1
	jbeq     _PA_1696_
	move.w   %d2,%d3
	addx.l   %d2,%d2
	swap     %d2
	move.w   %d3,%d2
	move.w   #17,%d7
	sub.l    %d1,%d7
	lsr.l    %d7,%d2
	_PA_1696_:         
	addx.l   %d3,%d3
	swap     %d3
	move.w   %d2,%d3
	lsl.l    %d4,%d3
	move.w   %d2,%d6
|***	RESTORE	AFTER d4	***
	movem.l  12(%a7),%d4
|;;   dbra      d4,flip_lp             | [1698]
	move     %CCR,%d1
	tst.w    %d4
	jbeq     _PA_1697_
	subq.l   #1,%d4
	jbra     flip_lp
	_PA_1697_:         
	move     %d1,%CCR

|;;   move.w    d6,-(a1)               | [1700]
	move.w   %d2,-(%a1)
|***	RESTORE	AFTER d2-d3	***
	movem.l  4(%a7),%d2-%d3
|;;   dbra      d3,line_lp             | [1701]
	move     %CCR,%d1
	tst.w    %d3
	jbeq     _PA_1700_
	subq.l   #1,%d3
	move     %d1,%CCR
	jbra     line_lp
	_PA_1700_:         
	move     %d1,%CCR

|;;   adda.w    -16(a6),a0             | [1703]
	mvs.w    (-16,%a6),%d1
	adda.l   %d1,%a0
|***	RESTORE	AFTER d1	***
	movem.l  (%a7),%d1
strtflip:
|;;   dbra      d1,upsd_lp             | [1705]
	move     %CCR,%d7
	tst.w    %d1
	jbeq     _PA_1704_
	subq.l   #1,%d1
	move     %d7,%CCR
	jbra     upsd_lp
	_PA_1704_:         
	move.w   #-1,%d1
	move     %d7,%CCR

      move.w   (-18,%a6),(-16,%a6)     | [1707]
      movea.l  _scrtchp,%a1            | [1708]
|;;   adda.w    -4(a6),a1              | [1709]
	mvs.w    (-4,%a6),%d5
	adda.l   %d5,%a1
      move.l   %a1,(-10,%a6)           | [1710]
      move.w   _SOURCEX,%d0            | [1711]
|;;   add.w     -102(a6),d0            | [1712]
	move.w   %d0,%d7
	move.w   (-102,%a6),%d5
	add.l    %d5,%d7
|;;   neg.w     d0                     | [1713]
	neg.l    %d7
|;;   andi.w    #0xF,d0                | [1714]
	andi.l   #(0xFFFF&0xF),%d7
|;;   move.w    d0,_SOURCEX            | [1715]
	move.w   %d7,_SOURCEX
      clr.w    _SOURCEY                | [1716]
|***	RESTORE	d5/d7	***
	movem.l  16(%a7),%d5
	movem.l  24(%a7),%d7
	lea      28(%a7),%a7
      rts                              | [1717]





replicat:
|***	SAVE	d6-d7	***
	lea      -16(%a7),%a7
	movem.l  %d6-%d7,8(%a7)
      move.w   _SOURCEX,%d0            | [1725]
      move.w   %d0,%d4                 | [1726]
|;;   andi.w    #0x0F,d0               | [1727]
	andi.l   #(0xFFFF&0x0F),%d0
      move.w   %d0,(-24,%a6)           | [1728]
|;;   lsr.w     #4,d4                  | [1729]
	mvz.w    %d4,%d6
	lsr.l    #4,%d6
|;;   lsl.w     #1,d4                  | [1730]
	lsl.l    #1,%d6

      move.w   _SOURCEY,%d0            | [1732]
      mulu.w   (-16,%a6),%d0           | [1733]
|;;   add.w     d4,d0                  | [1734]
	move.w   %d0,%d7
	add.l    %d6,%d7
	move.w   %d7,%d0
|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      movea.l  (-10,%a6),%a0           | [1735]
|;;   adda.w    d0,a0                  | [1736]
	mvs.w    %d0,%d3
	adda.l   %d3,%a0

      move.w   (-24,%a6),%d2           | [1738]
      move.w   #0x8000,%d3             | [1739]
      move.w   %d3,%d4                 | [1740]
|***	SAVE	d4	***
	movem.l  %d4,4(%a7)
|;;   lsr.w     d2,d3                  | [1741]
	and.l    #0xffff,%d3
	lsr.l    %d2,%d3

      move.w   (-106,%a6),%d2          | [1743]
      move.w   (-102,%a6),%d1          | [1744]

      move.w   %d2,(-22,%a6)           | [1746]
      move.w   %d1,(-20,%a6)           | [1747]
      movea.l  _scrtchp,%a1            | [1748]
|;;   adda.w    _scrpt2,a1             | [1749]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a1

      btst     #4,(-96+1,%a6)          | [1751 EQ]
      jbeq     noline                  | [1752]



|;;   addq.w    #1,-106(a6)            | [1756]
	move.w   (-106,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-106,%a6)
|;;   addq.w    #1,-102(a6)            | [1757]
	move.w   (-102,%a6),%d0
	addq.l   #1,%d0
	move.w   %d0,(-102,%a6)

|;;   lsl.w     #1,d1                  | [1759]
	move.w   %d1,%d0
	lsl.l    #1,%d0
|;;   addq.w    #3,d1                  | [1760]
	addq.l   #3,%d0
|;;   lsr.w     #4,d1                  | [1761]
	and.l    #0xffff,%d0
	lsr.l    #4,%d0
|;;   lsl.w     #1,d1                  | [1762]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1763]
	addq.l   #2,%d0
|;;   move.w    d1,d5                  | [1764]
	move.w   %d0,%d5
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
      moveq.l  #0,%d6                  | [1765]
	move.w   %d0,%d1
oklear:
      move.w   %d6,(%a1)+              | [1767]
|;;   dbra      d5,oklear              | [1768]
	tst.w    %d5
	jbeq     _PA_1767_
	subq.l   #1,%d5
	jbra     oklear
	_PA_1767_:         
      lea      (-2,%a1),%a2            | [1769]
      move.w   %d2,%d5                 | [1770]
|;;   lsl.w     #1,d5                  | [1771]
	move.w   %d5,%d0
	lsl.l    #1,%d0
	move.w   %d0,%d5
oklear1:
|;;   adda.w    d1,a2                  | [1773]
	mvs.w    %d1,%d0
	adda.l   %d0,%a2
      move.w   %d6,(%a2)               | [1774]
|;;   dbra      d5,oklear1             | [1775]
	tst.w    %d5
	jbeq     _PA_1774_
	subq.l   #1,%d5
	jbra     oklear1
	_PA_1774_:         

|;;   move.w    d1,d5                  | [1777]
	move.w   %d0,%d5
|;;   lsr.w     #1,d5                  | [1778]
	mvz.w    %d5,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d5                  | [1779 X]
	subq.l   #1,%d0
	move.w   %d0,%d5
oklear3:
      move.w   %d6,-(%a2)              | [1781]
|;;   dbra      d5,oklear3             | [1782]
	move     %CCR,%d0
	tst.w    %d5
	jbeq     _PA_1781_
	subq.l   #1,%d5
	move     %d0,%CCR
	jbra     oklear3
	_PA_1781_:         
	move     %d0,%CCR
      jbra     noline1                 | [1783]

noline:
|;;   lsr.w     #3,d1                  | [1786]
	mvz.w    %d1,%d0
	lsr.l    #3,%d0
|;;   lsl.w     #1,d1                  | [1787]
	lsl.l    #1,%d0
|;;   addq.w    #2,d1                  | [1788 X]
	ori.l    #0xffff0000,%d0
	addq.l   #2,%d0
	move.w   %d0,%d1
|***	RESTORE	AFTER d6	***
	movem.l  8(%a7),%d6
noline1:
      move.w   %d1,(-18,%a6)           | [1790]
|***	RESTORE	AFTER d7	***
	movem.l  12(%a7),%d7



      move.w   _T_SCLSTS,%d7           | [1794]
|;;   roxr.l    #1,d7                  | [1795 X]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7
      moveq.l  #0,%d7                  | [1796]
|;;   roxr.l    #1,d7                  | [1797]
	move.l   %d7,%d0
	addx.l   %d0,%d0
	moveq    #32-1,%d4
	lsl.l    %d4,%d0
	lsr.l    #1,%d7
	or.l     %d0,%d7

      move.w   (-16,%a6),%d7           | [1799]
      move.w   _DDA_INC,%d2            | [1800]
      move.w   (-22,%a6),%d5           | [1801]
|;;   subq.w    #1,d5                  | [1802]
	move.w   %d5,%d0
	subq.l   #1,%d0
      move.w   #32767,%d6              | [1803]
	move.w   %d0,%d5
|***	RESTORE	d0/d4	***
	movem.l  (%a7),%d0/%d4
	lea      16(%a7),%a7
      tst.l    %d7                     | [1804 MI]
      bmi.b    rep_ylop                | [1805]
y_dwn_lp:
|***	SAVE	d0-d2	***
	lea      -12(%a7),%a7
	movem.l  %d0-%d2,(%a7)
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
|***	RESTORE	AFTER d0-d1	***
	movem.l  (%a7),%d0-%d1
      jbcc     y_no_drw                | [1808]
      jbsr     yloop                   | [1809]
y_no_drw:
|;;   adda.w    d7,a0                  | [1811]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  8(%a7),%d2
	lea      12(%a7),%a7
|;;   dbra      d5,y_dwn_lp            | [1812]
	tst.w    %d5
	jbeq     _PA_1811_
	subq.l   #1,%d5
	jbra     y_dwn_lp
	_PA_1811_:         
	move.w   #-1,%d5
      jbra     y_rep_don               | [1813]
rep_ylop:
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
|;;   add.w     d2,d6                  | [1815 CC]
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
      jbcc     y_no_rep                | [1816]
      jbsr     yloop                   | [1817]
y_no_rep:
      jbsr     yloop                   | [1819]
|;;   adda.w    d7,a0                  | [1820]
	mvs.w    %d7,%d2
	adda.l   %d2,%a0
|***	RESTORE	AFTER d2	***
	movem.l  4(%a7),%d2
	lea      8(%a7),%a7
|;;   dbra      d5,rep_ylop            | [1821]
	tst.w    %d5
	jbeq     _PA_1820_
	subq.l   #1,%d5
	jbra     rep_ylop
	_PA_1820_:         
	move.w   #-1,%d5
y_rep_don:
|***	SAVE	d4-d6	***
	lea      -12(%a7),%a7
	movem.l  %d4-%d6,(%a7)
      move.w   (-102,%a6),%d2          | [1823]
|;;   subq.w    #1,d2                  | [1824]
	subq.l   #1,%d2
      move.w   _XACC_DDA,%d1           | [1825]
      move.w   _DDA_INC,%d0            | [1826]
      moveq.l  #0,%d3                  | [1827]
repwidcl:
|;;   add.w     d0,d1                  | [1829 CC]
	mvz.w    %d0,%d4
	move.w   %d1,%d5
	ori.l    #0xffff0000,%d5
	add.l    %d4,%d5
	move     %ccr,%d6
	move.w   %d5,%d1
	move     %d6,%ccr
      jbcc     nrepdoub                | [1830]
|;;   addq.w    #1,d3                  | [1831]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrepdoub:
      tst.l    %d7                     | [1833 PL]
      bpl.b    nrpndoub                | [1834]
|;;   addq.w    #1,d3                  | [1835]
	move.w   %d3,%d4
	addq.l   #1,%d4
	move.w   %d4,%d3
nrpndoub:
|;;   dbra      d2,repwidcl            | [1837]
	tst.w    %d2
	jbeq     _PA_1836_
	subq.l   #1,%d2
	jbra     repwidcl
	_PA_1836_:         
      move.w   %d1,_XACC_DDA           | [1838]
      move.w   %d3,(-102,%a6)          | [1839]
      move.w   (-92,%a6),(-106,%a6)    | [1840]


      move.w   (-18,%a6),%d1           | [1843]
      move.w   %d1,(-16,%a6)           | [1844]







repexit1:
      btst     #4,(-96+1,%a6)          | [1853 EQ]
      jbeq     repexit2                | [1854]
      movea.l  _scrtchp,%a0            | [1855]
|;;   adda.w    _scrpt2,a0             | [1856]
	mvs.w    _scrpt2,%d0
	adda.l   %d0,%a0
      move.w   %d1,%d7                 | [1857]
|;;   lea       2(a0,d7.w),a1          | [1858]
	ext.l    %d7
	lea      (2,%a0,%d7.l*1),%a1
|***	RESTORE	AFTER d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbsr     outlin1                 | [1859]
|***	SAVE	d1-d2	***
	lea      -8(%a7),%a7
	movem.l  %d1-%d2,(%a7)
      move.w   _scrpt2,%d0             | [1860]
|;;   add.w     -18(a6),d0             | [1861]
	move.w   %d0,%d2
	move.w   (-18,%a6),%d1
	add.l    %d1,%d2
|;;   move.w    #15,_SOURCEX           | [1862]
	moveq.l  #15,%d1
	move.w   %d1,_SOURCEX
	move.w   %d2,%d0
|***	RESTORE	d1-d2	***
	movem.l  (%a7),%d1-%d2
	lea      8(%a7),%a7
      jbra     repexit3                | [1863]
repexit2:
      move.w   _scrpt2,%d0             | [1865]
|***	RESTORE	d4-d6	***
	movem.l  (%a7),%d4-%d6
	lea      12(%a7),%a7
      jbra     repexit                 | [1866]
| 2409 "vdi/vdi_tblit.S"
yloop:
|***	SAVE	d1	***
	lea      -4(%a7),%a7
	movem.l  %d1,(%a7)
|;;   movem.l   d3-d4/d6,-(sp)         | [1869]
	lea      -12(%a7),%a7
	movem.l  %d3-%d4/%d6,(%a7)
      movea.l  %a0,%a3                 | [1870]
      movea.l  %a1,%a4                 | [1871]
      movea.l  %d7,%a5                 | [1872]
      move.w   %d5,(-44,%a6)           | [1873]
      moveq.l  #0,%d0                  | [1874]
      move.w   (-20,%a6),%d5           | [1875]
|;;   subq.w    #1,d5                  | [1876]
	subq.l   #1,%d5

      move.w   _XACC_DDA,%d7           | [1878]
      jbra     nextsrc                 | [1879]
innerlp:
|;;   ror.w     #1,d3                  | [1881 CC]
	move.w   %d3,%d1
	swap     %d3
	move.w   %d1,%d3
	lsr.l    #1,%d3
      jbcc     reploop                 | [1882]
nextsrc:
      movea.w  (%a0)+,%a2              | [1884]
reploop:
      move.w   %a2,%d6                 | [1886]
|;;   and.w     d3,d6                  | [1887 NE]
	and.l    %d3,%d6
	tst.w    %d6
      jbne     nrepnor                 | [1888]


repnor:
      tst.l    %d7                     | [1892 MI]
      bmi.b    repnorup                | [1893]
|;;   add.w     d2,d7                  | [1894 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     incsrc                  | [1895]
      jbra     ordone                  | [1896]
repnorup:
|;;   add.w     d2,d7                  | [1898 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     ordone                  | [1899]


|;;   ror.w     #1,d4                  | [1902 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4


      jbcc     ordone                  | [1905]
      move.w   %d0,(%a1)+              | [1906]
      moveq.l  #0,%d0                  | [1907]


      jbra     ordone                  | [1910]
nrepnor:


|;;   add.w     d2,d7                  | [1914 CC]
	and.l    #0xffff,%d2
	move.w   %d7,%d1
	ori.l    #0xffff0000,%d1
	add.l    %d2,%d1
	move     %ccr,%d6
	move.w   %d1,%d7
	move     %d6,%ccr
      jbcc     o_no_rep                | [1915]


|;;   or.w      d4,d0                  | [1918]
	or.l     %d4,%d0
|;;   ror.w     #1,d4                  | [1919 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4

      jbcc     o_no_rep                | [1921]
      move.w   %d0,(%a1)+              | [1922]
      moveq.l  #0,%d0                  | [1923]
o_no_rep:
      tst.l    %d7                     | [1925 PL]
      bpl.b    incsrc                  | [1926]


|;;   or.w      d4,d0                  | [1929]
	or.l     %d4,%d0
ordone:
|;;   ror.w     #1,d4                  | [1931 CC]
	move.w   %d4,%d1
	swap     %d4
	move.w   %d1,%d4
	lsr.l    #1,%d4
      jbcc     incsrc                  | [1932]
nextdst:
      move.w   %d0,(%a1)+              | [1934]
      moveq.l  #0,%d0                  | [1935]
incsrc:
|;;   dbra      d5,innerlp             | [1937]
	tst.w    %d5
	jbeq     _PA_1936_
	subq.l   #1,%d5
	jbra     innerlp
	_PA_1936_:         
repdone:
      move.w   %d0,(%a1)               | [1939]
|;;   movem.l   (sp)+,d3-d4/d6         | [1940]
	movem.l  (%a7),%d3-%d4/%d6
	lea      12(%a7),%a7
      movea.l  %a3,%a0                 | [1941]
      movea.l  %a4,%a1                 | [1942]
|***	RESTORE	AFTER d1	***
	move.l   (%a7)+,%d1
|;;   adda.w    d1,a1                  | [1943]
	ext.l    %d1
	adda.l   %d1,%a1
      move.l   %a5,%d7                 | [1944]
      move.w   (-44,%a6),%d5           | [1945]
      rts                              | [1946]

| 2498 "vdi/vdi_tblit.S"
outlin:
|;;   lea       (a0,d7.w),a1           | [1951]
	move.w   %d7,%a5
	lea      (0,%a0,%a5.l*1),%a1
outlin1:
|;;   lea       (a1,d7.w),a2           | [1953]
	move.w   %d7,%a5
	lea      (0,%a1,%a5.l*1),%a2
|;;   lsr.w     #1,d7                  | [1954]
	mvz.w    %d7,%d0
	lsr.l    #1,%d0
|;;   subq.w    #1,d7                  | [1955]
	subq.l   #1,%d0
|;;   move.w    d7,-44(a6)             | [1956]
	move.w   %d0,(-44,%a6)
      move.w   (-106,%a6),%d6          | [1957]

      movea.l  %a0,%a5                 | [1959]
|;;   subq.w    #1,d6                  | [1960]
	subq.l   #1,%d6
	move.w   %d0,%d7
out_edge:
      movea.w  %d6,%a4                 | [1962]
      movea.l  %a2,%a3                 | [1963]
      moveq.l  #0,%d5                  | [1964]
      moveq.l  #0,%d6                  | [1965]
      move.l   (%a2),%d1               | [1966]
      lsr.l    #1,%d1                  | [1967]



out_loop:
|***	SAVE	d1/d7	***
	lea      -16(%a7),%a7
	movem.l  %d1,4(%a7)
	movem.l  %d7,12(%a7)
      move.l   (%a0),%d0               | [1972]
      move.b   %d5,%d0                 | [1973]
|;;   ror.l     #1,d0                  | [1974]
	move.l   %d0,%d1
	lsr.l    #1,%d0
	moveq    #32-1,%d2
	lsl.l    %d2,%d1
	or.l     %d1,%d0

      move.l   (%a1),%d2               | [1976]
      move.b   %d6,%d2                 | [1977]
      move.l   %d2,%d3                 | [1978]
|;;   ror.l     #1,d3                  | [1979]
	move.l   %d3,%d1
	lsr.l    #1,%d3
	moveq    #32-1,%d4
	lsl.l    %d4,%d1
	or.l     %d1,%d3
      move.l   %d3,%d4                 | [1980]
|;;   ror.l     #1,d4                  | [1981]
	move.l   %d4,%d1
	lsr.l    #1,%d4
	moveq    #32-1,%d5
	lsl.l    %d5,%d1
	or.l     %d1,%d4

      move.l   %d0,%d5                 | [1983]
      move.l   %d0,%d6                 | [1984]
      eor.l    %d2,%d0                 | [1985]
      eor.l    %d3,%d5                 | [1986]
      eor.l    %d4,%d6                 | [1987]
|;;   rol.l     #1,d5                  | [1988]
	clr.l    %d1
	lsl.l    #1,%d5
	addx.l   %d1,%d5
|;;   rol.l     #2,d6                  | [1989]
	move.l   %d6,%d1
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d1
	or.l     %d1,%d6
      or.l     %d5,%d0                 | [1990]
      or.l     %d6,%d0                 | [1991]
|***	RESTORE	AFTER d1	***
	movem.l  4(%a7),%d1

|***	SAVE	d0	***
	movem.l  %d0,(%a7)
      move.l   %d1,%d5                 | [1993]
      move.l   %d1,%d6                 | [1994]
      eor.l    %d2,%d1                 | [1995]
      eor.l    %d3,%d5                 | [1996]
      eor.l    %d4,%d6                 | [1997]
|;;   rol.l     #1,d5                  | [1998]
	clr.l    %d0
	lsl.l    #1,%d5
	addx.l   %d0,%d5
|;;   rol.l     #2,d6                  | [1999]
	move.l   %d6,%d0
	lsl.l    #2,%d6
	moveq    #32-2,%d7
	lsr.l    %d7,%d0
	or.l     %d0,%d6
|***	RESTORE	AFTER d0	***
	movem.l  (%a7),%d0
      or.l     %d1,%d0                 | [2000]
      or.l     %d5,%d0                 | [2001]
      or.l     %d6,%d0                 | [2002]

      eor.l    %d3,%d2                 | [2004]
      eor.l    %d3,%d4                 | [2005]
|***	SAVE	d3	***
	movem.l  %d3,8(%a7)
|;;   rol.l     #2,d4                  | [2006]
	move.l   %d4,%d1
	lsl.l    #2,%d4
	moveq    #32-2,%d3
	lsr.l    %d3,%d1
	or.l     %d1,%d4
      or.l     %d2,%d0                 | [2007]
      or.l     %d4,%d0                 | [2008]
      swap     %d0                     | [2009]

      move.w   (%a1),%d6               | [2011]
      move.w   %d6,%d5                 | [2012]
|;;   eor.w     d0,d5                  | [2013]
	move.w   %d5,%d3
	eor.l    %d0,%d3
|;;   and.w     d0,d5                  | [2014]
	and.l    %d0,%d3

      addq.l   #2,%a2                  | [2016]
      move.l   (%a2),%d1               | [2017]
      move.b   (-1,%a2),%d1            | [2018]
|;;   ror.l     #1,d1                  | [2019]
	move.w   %d3,%d5
	move.l   %d1,%d3
	lsr.l    #1,%d1
	moveq    #32-1,%d7
	lsl.l    %d7,%d3
	or.l     %d3,%d1

      move.w   %d5,(%a1)+              | [2021]
      move.w   (%a0),%d5               | [2022]
      move.w   %d6,(%a0)+              | [2023]
|***	RESTORE	AFTER d3/d7	***
	movem.l  8(%a7),%d3/%d7
	lea      16(%a7),%a7

|;;   dbra      d7,out_loop            | [2025]
	tst.w    %d7
	jbeq     _PA_2024_
	subq.l   #1,%d7
	jbra     out_loop
	_PA_2024_:         

|***	SAVE	d0	***
	lea      -4(%a7),%a7
	movem.l  %d0,(%a7)
      move.w   (-44,%a6),%d7           | [2027]
      movea.l  %a5,%a0                 | [2028]
      move.w   %a4,%d6                 | [2029]
      movea.l  %a3,%a2                 | [2030]
      movea.l  %a2,%a1                 | [2031]
|;;   adda.w    -16(a6),a2             | [2032]
	mvs.w    (-16,%a6),%d0
	adda.l   %d0,%a2
|***	RESTORE	d0	***
	move.l   (%a7)+,%d0
      cmp.w    #1,%d6                  | [2033 NE]
      jbne     srt_lin                 | [2034]
      movea.l  %a1,%a2                 | [2035]
srt_lin:
|;;   dbra      d6,out_edge            | [2037]
	tst.w    %d6
	jbeq     _PA_2036_
	subq.l   #1,%d6
	jbra     out_edge
	_PA_2036_:         
	move.w   #-1,%d6
      rts                              | [2038]
| 2609 "vdi/vdi_tblit.S"
concat:
|;;   movem.l   d2/d3/a0,-(sp)         | [2041]
	lea      -12(%a7),%a7
	movem.l  %d2-%d3/%a0,(%a7)



      mulu.w   _v_lin_wr,%d1           | [2045]





      move.w   %d0,%d2                 | [2051]
|;;   andi.w    #0x000f,d0             | [2052]
	andi.l   #(0xFFFF0000!~0x000f),%d0
|;;   andi.w    #0xfff0,d2             | [2053]
	andi.l   #(0xFFFF&0xfff0),%d2
| 2631 "vdi/vdi_tblit.S"
      lea      _shift_offset,%a0       | [2055]
|;;   add.w     _v_planes,a0           | [2056]
	mvs.w    _v_planes,%d3
	adda.l   %d3,%a0
      move.b   (%a0),%d3               | [2057]
|;;   lsr.w     d3,d2                  | [2058]
	lsr.l    %d3,%d2






      ext.l    %d2                     | [2065]
      add.l    %d2,%d1                 | [2066]
|;;   movem.l   (sp)+,d2/d3/a0         | [2067]
	movem.l  (%a7),%d2-%d3/%a0
	lea      12(%a7),%a7
      rts                              | [2068]

| End section on aligned boundary
	.p2align 2,0
