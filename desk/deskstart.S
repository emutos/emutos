
.global _deskstart
.global _gemdos
.global _gem


        .text

_deskstart:

        movea.l a7,a5
        movea.l #deskustack,a7
        movea.l 4(a5),a5

        move.l  0xC(a5),d0
        add.l   0x14(a5),d0
        add.l   0x1C(a5),d0
        add.l   #0x100,d0
        move.l  d0,-(a7)
        move.l  a5,-(a7)
        move.w  d0,-(a7)
        move.w  #0x4A,-(a7)
        trap    #1                      // Mshrink
        lea     14(a7),a7

#ifndef NO_ROM
        move.l  #desksstack,-(a7)       // We can't use the old USP as SSP
        move.w  #0x20,-(a7)             // when doing GEM calls in supervisor
        trap    #1                      // mode...
        move.l  d0,old_ssp
#endif

        move.l  #far_draw,d0
        move.l  d0,_drawaddr

        jsr     _deskmain               // Run the desktop

#ifndef NO_ROM
        move.l  old_ssp,-(a7)
        move.w  #0x20,-(a7)
        trap    #1                      // Back to user mode
        addq.l  #6,a7
#endif

        clr.w   -(a7)
        trap    #1                      // Pterm



far_draw:
        move.l  4(a7),d0
        move.l  a7,LD5EA
        movea.l #LD9EE,a7
        move.l  d0,-(a7)
        jsr     _dr_code
        movea.l LD5EA,a7
        rts



#ifdef NO_ROM
_gemdos:
LF2:
        move.l  (SP)+,saveret

        movem.l d2/a2,saveregs

        move.w  #0,_DOS_ERR
        move.w  #0,_DOS_AX
        trap    #1
        cmp.l   #0,D0
        bge     L134
        move.w  #1,_DOS_ERR
        move.w  D0,_DOS_AX
        cmp.w   #0xFFE0,D0
        bgt     L134
        not.w   _DOS_AX
        subi.w  #0x1E,_DOS_AX
L134:

        movem.l saveregs,d2/a2

        move.l  saveret,-(SP)
        rts
#endif


_gem:
        move.l  4(sp),d1
        move.w  #0xc8,d0
        trap    #2
        rts



.bss



// Space for the Stack:

#ifndef NO_ROM
old_ssp:
        .ds.l   1

        .ds.b   0x800
desksstack:
#endif

        .ds.b   0x200
deskustack:


LD5EA:  .ds.l   1

        .ds.b   0x400
LD9EE:



#ifdef NO_ROM
saveret:
        .ds.l   1

saveregs:
        .ds.l   2
#endif

