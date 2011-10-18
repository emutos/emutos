/*      GEMBIND.C       9/11/85                         Lee Lorenzen    */
/*      for 3.0         3/11/86 - 8/26/86               MDF             */
/*      merge source    5/28/87                         mdf             */
/*      menu_click      9/25/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985 - 1987               Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "portab.h"
#include "compat.h"
#include "taddr.h"
#include "obdefs.h"
#include "gembind.h"


#define CTRL_CNT        3


static const BYTE ctrl_cnts[] =
{
/* Application Manager          */
        0, 1, 0,                        /* func 010             */
        2, 1, 1,                        /* func 011             */
        2, 1, 1,                        /* func 012             */
        0, 1, 1,                        /* func 013             */
        2, 1, 1,                        /* func 014             */
        1, 1, 1,                        /* func 015             */
        2, 1, 0,                        /* func 016             */
        0, 0, 0,                        /* func 017             */
        0, 0, 0,                        /* func 008             */
        0, 1, 0,                        /* func 019             */
/* Event Manager                */
        0, 1, 0,                        /* func 020             */
        3, 5, 0,                        /* func 021             */
        5, 5, 0,                        /* func 022             */
        0, 1, 1,                        /* func 023             */
        2, 1, 0,                        /* func 024             */
        16, 7, 1,                       /* func 025             */
        2, 1, 0,                        /* func 026             */
        0, 0, 0,                        /* func 027             */
        0, 0, 0,                        /* func 028             */
        0, 0, 0,                        /* func 009             */
/* Menu Manager                 */
        1, 1, 1,                        /* func 030             */
        2, 1, 1,                        /* func 031             */
        2, 1, 1,                        /* func 032             */
        2, 1, 1,                        /* func 033             */
        1, 1, 2,                        /* func 034             */
        1, 1, 1,                        /* func 005             */
        1, 1, 0,                        /* func 006             */
        2, 1, 0,                        /* func 007             */
        0, 0, 0,                        /* func 008             */
        0, 0, 0,                        /* func 009             */
/* Object Manager               */
        2, 1, 1,                        /* func 040             */
        1, 1, 1,                        /* func 041             */
        6, 1, 1,                        /* func 042             */
        4, 1, 1,                        /* func 043             */
        1, 3, 1,                        /* func 044             */
        2, 1, 1,                        /* func 045             */
        4, 2, 1,                        /* func 046             */
        8, 1, 1,                        /* func 047             */
        0, 0, 0,                        /* func 048             */
        0, 0, 0,                        /* func 049             */
/* Form Manager                 */
        1, 1, 1,                        /* func 050             */
        9, 1, 1,                        /* func 051             */
        1, 1, 1,                        /* func 002             */
        1, 1, 0,                        /* func 003             */
        0, 5, 1,                        /* func 004             */
        3, 3, 1,                        /* func 005             */
        2, 2, 1,                        /* func 006             */
        0, 0, 0,                        /* func 007             */
        0, 0, 0,                        /* func 008             */
        0, 0, 0,                        /* func 009             */
/* Process Manager              */
        2, 2, 2,                        /* func 060             */
        3, 1, 2,                        /* func 061             */
        1, 1, 0,                        /* func 062             */
        1, 3, 0,                        /* func 063             */
        0, 0, 1,                        /* func 064             */
        0, 0, 0,                        /* func 065             */
        1, 1, 0,                        /* func 066             */
        1, 1, 0,                        /* func 067             */
        0, 0, 0,                        /* func 008             */
        0, 0, 0,                        /* func 009             */
/* Graphics Manager     */
        4, 3, 0,                        /* func 070             */
        8, 3, 0,                        /* func 071             */
        6, 1, 0,                        /* func 072             */
        8, 1, 0,                        /* func 073             */
        8, 1, 0,                        /* func 074             */
        4, 1, 1,                        /* func 075             */
        3, 1, 1,                        /* func 076             */
        0, 5, 0,                        /* func 077             */
        1, 1, 1,                        /* func 078             */
        0, 5, 0,                        /* func 009             */
/* Scrap Manager                */
        0, 1, 1,                        /* func 080             */
        0, 1, 1,                        /* func 081             */
        0, 0, 0,                        /* func 082             */
        0, 0, 0,                        /* func 083             */
        0, 0, 0,                        /* func 084             */
        0, 0, 0,                        /* func 005             */
        0, 0, 0,                        /* func 006             */
        0, 0, 0,                        /* func 007             */
        0, 0, 0,                        /* func 008             */
        0, 0, 0,                        /* func 009             */
/* fseler Manager               */
        0, 2, 2,                        /* func 090             */
        0, 0, 0,                        /* func 091             */
        0, 0, 0,                        /* func 092             */
        0, 0, 0,                        /* func 003             */
        0, 0, 0,                        /* func 004             */
        0, 0, 0,                        /* func 005             */
        0, 0, 0,                        /* func 006             */
        0, 0, 0,                        /* func 007             */
        0, 0, 0,                        /* func 008             */
        0, 0, 0,                        /* func 009             */
/* Window Manager               */
        5, 1, 0,                        /* func 100             */
        5, 1, 0,                        /* func 101             */
        1, 1, 0,                        /* func 102             */
        1, 1, 0,                        /* func 103             */
        2, 5, 0,                        /* func 104             */
        6, 1, 0,                        /* func 105             */
        2, 1, 0,                        /* func 106             */
        1, 1, 0,                        /* func 107             */
        6, 5, 0,                        /* func 108             */
        0, 0, 0,                        /* func 009             */
/* Resource Manager             */
        0, 1, 1,                        /* func 110             */
        0, 1, 0,                        /* func 111             */
        2, 1, 0,                        /* func 112             */
        2, 1, 1,                        /* func 113             */
        1, 1, 1,                        /* func 114             */
        0, 0, 0,                        /* func 115             */
        0, 0, 0,                        /* func 006             */
        0, 0, 0,                        /* func 007             */
        0, 0, 0,                        /* func 008             */
        0, 0, 0,                        /* func 009             */
/* Shell Manager                */
        0, 1, 2,                        /* func 120             */
        3, 1, 2,                        /* func 121             */
        1, 1, 1,                        /* func 122             */
        1, 1, 1,                        /* func 123             */
        0, 1, 1,                        /* func 124             */
        0, 1, 2,                        /* func 125             */
        0, 1, 2,                        /* func 126             */
        0, 1, 2                         /* func 127             */
};


typedef struct gemblkstr
{
        LONG            gb_pcontrol;
        LONG            gb_pglobal;
        LONG            gb_pintin;
        LONG            gb_pintout;
        LONG            gb_padrin;
        LONG            gb_padrout;             
} GEMBLK;


extern WORD             gem();                  /* in STARTUP.S */


static GEMBLK           gb;
static LONG             ad_g;

static UWORD            control[C_SIZE];
GLOBAL UWORD            global[G_SIZE];
static UWORD            int_in[I_SIZE];
static UWORD            int_out[O_SIZE];
static LONG             addr_in[AI_SIZE];
static LONG             addr_out[AO_SIZE];





static WORD gem_if(WORD opcode)
{
        WORD            i;
        const BYTE      *pctrl;

        control[0] = opcode;

        pctrl = &ctrl_cnts[(opcode - 10) * 3];
        for(i=1; i<=CTRL_CNT; i++)
          control[i] = *pctrl++;

        gem(ad_g);

        return((WORD) RET_CODE );
}


                                        /* Application Manager          */

WORD appl_init(void)
{
        gb.gb_pcontrol = ADDR((BYTE *) &control[0]); 
        gb.gb_pglobal = ADDR((BYTE *) &global[0]);
        gb.gb_pintin = ADDR((BYTE *) &int_in[0]);
        gb.gb_pintout = ADDR((BYTE *) &int_out[0]);
        gb.gb_padrin = ADDR((BYTE *) &addr_in[0]);
        gb.gb_padrout = ADDR((BYTE *) &addr_out[0]);

        ad_g = ADDR((BYTE *) &gb);
        gem_if(APPL_INIT);
        return((WORD) RET_CODE );
}


WORD appl_exit(void)
{
        gem_if(APPL_EXIT);
        return( TRUE );
}


/* unused
WORD appl_write(WORD rwid, WORD length, LONG pbuff)
{
        AP_RWID = rwid;
        AP_LENGTH = length;
        AP_PBUFF = pbuff;
        return( gem_if(APPL_WRITE) );
}
*/

/*
WORD appl_read(WORD rwid, WORD length, LONG pbuff)
{
        AP_RWID = rwid;
        AP_LENGTH = length;
        AP_PBUFF = pbuff;
        return( gem_if(APPL_READ) );
}
*/

/* unused
WORD appl_find(LONG pname)
{
        AP_PNAME = pname;
        return( gem_if(APPL_FIND) );
}
*/

/* unused
WORD appl_tplay(LONG tbuffer, WORD tlength, WORD tscale)
{
        AP_TBUFFER = tbuffer;
        AP_TLENGTH = tlength;
        AP_TSCALE = tscale;
        return( gem_if(APPL_TPLAY) );
}
*/

/*
WORD appl_trecord(LONG tbuffer, WORD tlength)
{
        AP_TBUFFER = tbuffer;
        AP_TLENGTH = tlength;
        return( gem_if(APPL_TRECORD) );
}
*/

/*
WORD appl_bvset(UWORD bvdisk, UWORD bvhard)
{
        AP_BVDISK = bvdisk;
        AP_BVHARD = bvhard;
        return( gem_if(APPL_BVSET) );
}
*/

                                        /* Event Manager                */
/* not used in desktop
UWORD evnt_keybd(void)
{
        return((UWORD) gem_if(EVNT_KEYBD) );
}
*/


WORD evnt_button(WORD clicks, UWORD mask, UWORD state,
                 WORD *pmx, WORD *pmy, WORD *pmb, WORD *pks)
{
        B_CLICKS = clicks;
        B_MASK = mask;
        B_STATE = state;
        gem_if(EVNT_BUTTON);
        *pmx = EV_MX;
        *pmy = EV_MY;
        *pmb = EV_MB;
        *pks = EV_KS;
        return((WORD) RET_CODE);
}


/* not used in desktop
        WORD
evnt_mouse(flags, x, y, width, height, pmx, pmy, pmb, pks)
        WORD            flags, x, y, width, height;
        WORD            *pmx, *pmy, *pmb, *pks;
{
        MO_FLAGS = flags;
        MO_X = x;
        MO_Y = y;
        MO_WIDTH = width;
        MO_HEIGHT = height;
        gem_if(EVNT_MOUSE);
        *pmx = EV_MX;
        *pmy = EV_MY;
        *pmb = EV_MB;
        *pks = EV_KS;
        return((WORD) RET_CODE);
}
*/


/* not used
        WORD
evnt_mesag(pbuff)
        LONG            pbuff;
{
        ME_PBUFF = pbuff;
        return( gem_if(EVNT_MESAG) );
}
*/


WORD evnt_timer(UWORD locnt, UWORD hicnt)
{
        T_LOCOUNT = locnt;
        T_HICOUNT = hicnt;
        return( gem_if(EVNT_TIMER) );
}


WORD evnt_multi(UWORD flags, UWORD bclk, UWORD bmsk, UWORD bst,
                UWORD m1flags, UWORD m1x, UWORD m1y, UWORD m1w, UWORD m1h, 
                UWORD m2flags, UWORD m2x, UWORD m2y, UWORD m2w, UWORD m2h,
                LONG mepbuff, UWORD tlc, UWORD thc, UWORD *pmx, UWORD *pmy,
                UWORD *pmb, UWORD *pks, UWORD *pkr, UWORD *pbr )
{
        MU_FLAGS = flags;

        MB_CLICKS = bclk;
        MB_MASK = bmsk;
        MB_STATE = bst;

        MMO1_FLAGS = m1flags;
        MMO1_X = m1x;
        MMO1_Y = m1y;
        MMO1_WIDTH = m1w;
        MMO1_HEIGHT = m1h;

        MMO2_FLAGS = m2flags;
        MMO2_X = m2x;
        MMO2_Y = m2y;
        MMO2_WIDTH = m2w;
        MMO2_HEIGHT = m2h;

        MME_PBUFF = mepbuff;

        MT_LOCOUNT = tlc;
        MT_HICOUNT = thc;

        gem_if(EVNT_MULTI);

        *pmx = EV_MX;
        *pmy = EV_MY;
        *pmb = EV_MB;
        *pks = EV_KS;
        *pkr = EV_KRET;
        *pbr = EV_BRET;
        return((WORD) RET_CODE );
}


WORD evnt_dclick(WORD rate, WORD setit)
{
        EV_DCRATE = rate;
        EV_DCSETIT = setit;
        return( gem_if(EVNT_DCLICK) );
}


                                        /* Menu Manager                 */
WORD menu_bar(LONG tree, WORD showit)
{
        MM_ITREE = tree;
        SHOW_IT = showit;
        return( gem_if(MENU_BAR) );
}


WORD menu_icheck(LONG tree, WORD itemnum, WORD checkit)
{
        MM_ITREE = tree;
        ITEM_NUM = itemnum;
        CHECK_IT = checkit;
        return( gem_if(MENU_ICHECK) );
}


WORD menu_ienable(LONG tree, WORD itemnum, WORD enableit)
{
        MM_ITREE = tree;
        ITEM_NUM = itemnum;
        ENABLE_IT = enableit;
        return( gem_if(MENU_IENABLE) );
}


WORD menu_tnormal(LONG tree, WORD titlenum, WORD normalit)
{
        MM_ITREE = tree;
        TITLE_NUM = titlenum;
        NORMAL_IT = normalit;
        return( gem_if( MENU_TNORMAL ) );
}


WORD menu_text(LONG tree, WORD inum, LONG ptext)
{
        MM_ITREE = tree;
        ITEM_NUM = inum;
        MM_PTEXT = ptext;
        return( gem_if( MENU_TEXT ) );
}


/* not used in desktop
        WORD
menu_register(pid, pstr)
        WORD            pid;
        LONG            pstr;
{
        MM_PID = pid;
        MM_PSTR = pstr;
        return( gem_if( MENU_REGISTER ) );
}
*/


/*
        WORD
menu_unregister(mid)
        WORD            mid;
{
        MM_MID = mid;
        return( gem_if( MENU_UNREGISTER ) );
}
*/


WORD menu_click(WORD click, WORD setit)
{
        MN_CLICK = click;
        MN_SETIT = setit;
        return( gem_if( MENU_CLICK ));
}


                                        /* Object Manager               */
WORD objc_add(LONG tree, WORD parent, WORD child)
{
        OB_TREE = tree;
        OB_PARENT = parent;
        OB_CHILD = child;
        return( gem_if( OBJC_ADD ) );
}


/* unused in desktop
WORD objc_delete(LONG tree, WORD delob)
{
        OB_TREE = tree;
        OB_DELOB = delob;
        return( gem_if( OBJC_DELETE ) );
}
*/


WORD objc_draw(LONG tree, WORD drawob, WORD depth, WORD xc, WORD yc,
               WORD wc, WORD hc)
{
        OB_TREE = tree;
        OB_DRAWOB = drawob;
        OB_DEPTH = depth;
        OB_XCLIP = xc;
        OB_YCLIP = yc;
        OB_WCLIP = wc;
        OB_HCLIP = hc;
        return( gem_if( OBJC_DRAW ) );
}


WORD objc_find(LONG tree, WORD startob, WORD depth, WORD mx, WORD my)
{
        OB_TREE = tree;
        OB_STARTOB = startob;
        OB_DEPTH = depth;
        OB_MX = mx;
        OB_MY = my;
        return( gem_if( OBJC_FIND ) );
}


WORD objc_order(LONG tree, WORD mov_obj, WORD newpos)
{
        OB_TREE = tree;
        OB_OBJ = mov_obj;
        OB_NEWPOS = newpos;
        return( gem_if( OBJC_ORDER ) );
}


WORD objc_offset(LONG tree, WORD obj, WORD *poffx, WORD *poffy)
{
        OB_TREE = tree;
        OB_OBJ = obj;
        gem_if(OBJC_OFFSET);
        *poffx = OB_XOFF;
        *poffy = OB_YOFF;
        return((WORD) RET_CODE );
}


/* unused in desktop
WORD objc_edit(LONG tree, WORD obj, WORD inchar, WORD *idx, WORD kind)
{
        OB_TREE = tree;
        OB_OBJ = obj;
        OB_CHAR = inchar;
        OB_IDX = *idx;
        OB_KIND = kind;
        gem_if( OBJC_EDIT );
        *idx = OB_ODX;
        return((WORD) RET_CODE );
}
*/


WORD objc_change(LONG tree, WORD drawob, WORD depth, WORD xc, WORD yc,
                 WORD wc, WORD hc, WORD newstate, WORD redraw)
{
        OB_TREE = tree;
        OB_DRAWOB = drawob;
        OB_DEPTH = depth;
        OB_XCLIP = xc;
        OB_YCLIP = yc;
        OB_WCLIP = wc;
        OB_HCLIP = hc;
        OB_NEWSTATE = newstate;
        OB_REDRAW = redraw;
        return( gem_if( OBJC_CHANGE ) );
}



                                        /* Form Manager                 */
WORD form_do(LONG form, WORD start)
{
        FM_FORM = form;
        FM_START = start;
        return( gem_if( FORM_DO ) );
}


WORD form_dial(WORD dtype, WORD ix, WORD iy, WORD iw, WORD ih,
               WORD x, WORD y, WORD w, WORD h)
{
        FM_TYPE = dtype;
        FM_IX = ix;
        FM_IY = iy;
        FM_IW = iw;
        FM_IH = ih;
        FM_X = x;
        FM_Y = y;
        FM_W = w;
        FM_H = h;
        return( gem_if( FORM_DIAL ) );
}


WORD form_alert(WORD defbut, LONG astring)
{
        FM_DEFBUT = defbut;
        FM_ASTRING = astring;
        return( gem_if( FORM_ALERT ) );
}


WORD form_error(WORD errnum)
{
        FM_ERRNUM = errnum;
        return( gem_if( FORM_ERROR ) );
}


WORD form_center(LONG tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch)
{
        FM_FORM = tree;
        gem_if(FORM_CENTER);
        *pcx = FM_XC;
        *pcy = FM_YC;
        *pcw = FM_WC;
        *pch = FM_HC;
        return((WORD) RET_CODE );
}


/* not used in desktop
        WORD
form_keybd(form, obj, nxt_obj, thechar, pnxt_obj, pchar)
        LONG            form;
        WORD            obj;
        WORD            nxt_obj;
        WORD            thechar;
        WORD            *pnxt_obj;
        WORD            *pchar;
{
        FM_FORM = form;
        FM_OBJ = obj;
        FM_INXTOB = nxt_obj;
        FM_ICHAR = thechar;
        gem_if( FORM_KEYBD );
        *pnxt_obj = FM_ONXTOB;
        *pchar = FM_OCHAR;
        return( RET_CODE );
}
*/


/* not used in desktop
        WORD
form_button(form, obj, clks, pnxt_obj)
        LONG            form;
        WORD            obj;
        WORD            clks;
        WORD            *pnxt_obj;
{
        FM_FORM = form;
        FM_OBJ = obj;
        FM_CLKS = clks;
        gem_if( FORM_BUTTON );
        *pnxt_obj = FM_ONXTOB;
        return( RET_CODE );
}
*/



                                        /* Process Manager              */
/*
        WORD
proc_create(ibegaddr, isize, isswap, isgem, onum)
        LONG            ibegaddr, isize;
        WORD            isswap, isgem;
        WORD            *onum;
{
        WORD    ret;

        PR_IBEGADDR = ibegaddr;
        PR_ISIZE = isize;
        PR_ISSWAP = isswap;
        PR_ISGEM = isgem;
        ret = gem_if(PROC_CREATE);
        *onum = PR_ONUM;
        return(ret);
}

        WORD
proc_run(proc_num, isgraf, isover, pcmd, ptail)
        WORD            proc_num;
        WORD            isgraf, isover;
        LONG            pcmd, ptail;
{
        PR_NUM = proc_num;
        PR_ISGRAF = isgraf;
        PR_ISOVER = isover;
        PR_PCMD = pcmd;
        PR_PTAIL = ptail;
        return( gem_if(PROC_RUN) );
}

        WORD
proc_delete(proc_num)
        WORD            proc_num;
{
        PR_NUM = proc_num;
        return( gem_if(PROC_DELETE) );
}

        WORD
proc_info(num, oisswap, oisgem, obegaddr, ocsize, oendmem, ossize, ointtbl)
        WORD            num;
        WORD            *oisswap;
        WORD            *oisgem;
        LONG            *obegaddr;
        LONG            *ocsize;
        LONG            *oendmem;
        LONG            *ossize;
        LONG            *ointtbl;
{
        WORD            ret;

        PR_NUM = num;
        AOUT_LEN = 5;
        ret = gem_if(PROC_INFO);
        AOUT_LEN = 0;
        *oisswap = PR_OISSWAP;
        *oisgem = PR_OISGEM;
        *obegaddr = PR_OBEGADDR;
        *ocsize = PR_OCSIZE;
        *oendmem = PR_OENDMEM;
        *ossize = PR_OSSIZE;
        *ointtbl = PR_OITBL;
        return(ret);
}

        LONG
proc_malloc(csize, adrcsize)
        LONG            csize;
        LONG            *adrcsize;
{
        PR_IASIZE = csize;
        AOUT_LEN = 2;
        gem_if(PROC_MALLOC);    
        *adrcsize = PR_OCSIZE;
        AOUT_LEN = 0;
        return(PR_OBEGADDR);
}

        WORD
proc_switch(pid)
        WORD            pid;
{
        PR_NUM = pid;
        return(gem_if(PROC_SWITCH));
}


        WORD
proc_shrink(pid)
        WORD            pid;
{
        PR_NUM = pid;
        return(gem_if(PROC_SHRINK));
}
*/

                                        /* Graphics Manager             */
WORD graf_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
                 WORD *pwend, WORD *phend)
{
        GR_I1 = xorigin;
        GR_I2 = yorigin;
        GR_I3 = wmin;
        GR_I4 = hmin;
        gem_if( GRAF_RUBBOX );
        *pwend = GR_O1;
        *phend = GR_O2;
        return((WORD) RET_CODE );
}


WORD graf_dragbox(WORD w, WORD h, WORD sx, WORD sy, WORD xc, WORD yc,
                  WORD wc, WORD hc, WORD *pdx, WORD *pdy)
{
        GR_I1 = w;
        GR_I2 = h;
        GR_I3 = sx;
        GR_I4 = sy;
        GR_I5 = xc;
        GR_I6 = yc;
        GR_I7 = wc;
        GR_I8 = hc;
        gem_if( GRAF_DRAGBOX );
        *pdx = GR_O1;
        *pdy = GR_O2;
        return((WORD) RET_CODE );
}


WORD graf_mbox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty)
{
        GR_I1 = w;
        GR_I2 = h;
        GR_I3 = srcx;
        GR_I4 = srcy;
        GR_I5 = dstx;
        GR_I6 = dsty;
        return( gem_if( GRAF_MBOX ) );
}


WORD graf_growbox(WORD orgx, WORD orgy, WORD orgw, WORD orgh,
                  WORD x, WORD y, WORD w, WORD h)
{
        GR_I1 = orgx;
        GR_I2 = orgy;
        GR_I3 = orgw;
        GR_I4 = orgh;
        GR_I5 = x;
        GR_I6 = y;
        GR_I7 = w;
        GR_I8 = h;
        return( gem_if( GRAF_GROWBOX ) );
}


WORD graf_shrinkbox(WORD orgx, WORD orgy, WORD orgw, WORD orgh,
                    WORD x, WORD y, WORD w, WORD h)
{
        GR_I1 = orgx;
        GR_I2 = orgy;
        GR_I3 = orgw;
        GR_I4 = orgh;
        GR_I5 = x;
        GR_I6 = y;
        GR_I7 = w;
        GR_I8 = h;
        return( gem_if( GRAF_SHRINKBOX ) );
}


WORD graf_watchbox(LONG tree, WORD obj, UWORD instate, UWORD outstate)
{
        GR_TREE = tree;
        GR_OBJ = obj;
        GR_INSTATE = instate;
        GR_OUTSTATE = outstate;
        return( gem_if( GRAF_WATCHBOX ) );
}


WORD graf_slidebox(LONG tree, WORD parent, WORD obj, WORD isvert)
{
        GR_TREE = tree;
        GR_PARENT = parent;
        GR_OBJ = obj;
        GR_ISVERT = isvert;
        return( gem_if( GRAF_SLIDEBOX ) );
}


WORD graf_handle(WORD *pwchar, WORD *phchar, WORD *pwbox, WORD *phbox)
{
        gem_if(GRAF_HANDLE);
        *pwchar = GR_WCHAR ;
        *phchar = GR_HCHAR;
        *pwbox = GR_WBOX;
        *phbox = GR_HBOX;
        return((WORD) RET_CODE);
}


WORD graf_mouse(WORD m_number, WORD m_addr)
{
        GR_MNUMBER = m_number;
        GR_MADDR = m_addr;
        return( gem_if( GRAF_MOUSE ) );
}


void graf_mkstate(WORD *pmx, WORD *pmy, WORD *pmstate, WORD *pkstate)
{
        gem_if( GRAF_MKSTATE );
        *pmx = GR_MX;
        *pmy = GR_MY;
        *pmstate = GR_MSTATE;
        *pkstate = GR_KSTATE;
}


                                        /* Scrap Manager                */
/* unused in desktop
WORD scrp_read(LONG pscrap)
{
        SC_PATH = pscrap;
        return( gem_if( SCRP_READ ) );
}


WORD scrp_write(LONG pscrap)
{
        SC_PATH = pscrap;
        return( gem_if( SCRP_WRITE ) );
}
*/

                                        /* fseler Manager               */
/* unused in desktop
WORD fsel_input(LONG pipath, LONG pisel, WORD *pbutton)
{
        FS_IPATH = pipath;
        FS_ISEL = pisel;
        gem_if( FSEL_INPUT );
        *pbutton = FS_BUTTON;
        return((WORD) RET_CODE );
}
*/

                                        /* Window Manager               */
WORD wind_create(UWORD kind, WORD wx, WORD wy, WORD ww, WORD wh)
{
        WM_KIND = kind;
        WM_WX = wx;
        WM_WY = wy;
        WM_WW = ww;
        WM_WH = wh;
        return( gem_if( WIND_CREATE ) );
}


WORD wind_open(WORD handle, WORD wx, WORD wy, WORD ww, WORD wh)
{
        WM_HANDLE = handle;
        WM_WX = wx;
        WM_WY = wy;
        WM_WW = ww;
        WM_WH = wh;
        //kprintf("wx=%d, wy=%d, ww=%d, wh=%d, handle=%d !\n", wx, wy, ww, wh, handle);
        return( gem_if( WIND_OPEN ) );
}


WORD wind_close(WORD handle)
{
        WM_HANDLE = handle;
        return( gem_if( WIND_CLOSE ) );
}


WORD wind_delete(WORD handle)
{
        WM_HANDLE = handle;
        return( gem_if( WIND_DELETE ) );
}


WORD wind_get(WORD w_handle, WORD w_field, WORD *pw1, WORD *pw2, WORD *pw3, WORD *pw4)
{
        WM_HANDLE = w_handle;
        WM_WFIELD = w_field;
        gem_if( WIND_GET );
        *pw1 = WM_OX;
        *pw2 = WM_OY;
        *pw3 = WM_OW;
        *pw4 = WM_OH;
        return((WORD) RET_CODE );
}


WORD wind_set(WORD w_handle, WORD w_field, WORD w2, WORD w3, WORD w4, WORD w5)
{
        WM_HANDLE = w_handle;
        WM_WFIELD = w_field;
        WM_IX = w2;
        WM_IY = w3;
        WM_IW = w4;
        WM_IH = w5;
        return( gem_if( WIND_SET ) );
}


WORD wind_find(WORD mx, WORD my)
{
        WM_MX = mx;
        WM_MY = my;
        return( gem_if( WIND_FIND ) );
}


WORD wind_update(WORD beg_update)
{
        WM_BEGUP = beg_update;
        return( gem_if( WIND_UPDATE ) );
}


WORD wind_calc(WORD wctype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
               WORD *px, WORD *py, WORD *pw, WORD *ph)
{
        WM_WCTYPE = wctype;
        WM_WCKIND = kind;
        WM_WCIX = x;
        WM_WCIY = y;
        WM_WCIW = w;
        WM_WCIH = h;
        gem_if( WIND_CALC );
        *px = WM_WCOX;
        *py = WM_WCOY;
        *pw = WM_WCOW;
        *ph = WM_WCOH;
        return((WORD) RET_CODE );
}


                                        /* Resource Manager             */
#if 0
/* We only need this when desktop is compiled as a standalone app and uses
   an external DESKTOP.RSC */
WORD rsrc_load(LONG rsname)
{
        RS_PFNAME = rsname;
        return( gem_if(RSRC_LOAD) );
}


WORD rsrc_free(void)
{
        return( gem_if( RSRC_FREE ) );
}


/* Note: We fake this call in desk_rsc.c when desktop is in ROM */
WORD rsrc_gaddr(WORD rstype, WORD rsid, LONG *paddr)
{
        RS_TYPE = rstype;
        RS_INDEX = rsid;
        gem_if(RSRC_GADDR);
        *paddr = RS_OUTADDR;
        return((WORD) RET_CODE );
}
#endif


/* unused
WORD rsrc_saddr(WORD rstype, WORD rsid, LONG lngval)
        WORD            rstype;
        WORD            rsid;
        LONG            lngval;
{
        RS_TYPE = rstype;
        RS_INDEX = rsid;
        RS_INADDR = lngval;
        return( gem_if(RSRC_SADDR) );
}
*/


WORD rsrc_obfix(LONG tree, WORD obj)
{
        RS_TREE = tree;
        RS_OBJ = obj;
        return( gem_if(RSRC_OBFIX) );
}


                                        /* Shell Manager                */
WORD shel_read(LONG pcmd, LONG ptail)
{
        SH_PCMD = pcmd;
        SH_PTAIL = ptail;
        return( gem_if( SHEL_READ ) );
}


WORD shel_write(WORD doex, WORD isgr, WORD iscr, LONG pcmd, LONG ptail)
{
        SH_DOEX = doex;
        SH_ISGR = isgr;
        SH_ISCR = iscr;
        SH_PCMD = pcmd;
        SH_PTAIL = ptail;
        return( gem_if( SHEL_WRITE ) );
}


WORD shel_get(LONG pbuffer, WORD len)
{
        SH_PBUFFER = pbuffer;
        SH_LEN = len;
        return( gem_if( SHEL_GET ) );
}


WORD shel_put(LONG pdata, WORD len)
{
        SH_PDATA = pdata;
        SH_LEN = len;
        return( gem_if( SHEL_PUT ) );
}



WORD shel_find(LONG ppath)
{
        SH_PATH = ppath;
        return( gem_if( SHEL_FIND ) );
}


WORD shel_envrn(LONG ppath, LONG psrch)
{
        SH_PATH = ppath;
        SH_SRCH = psrch;
        return( gem_if( SHEL_ENVRN ) );
}


/*
WORD shel_rdef(LONG lpcmd, LONG lpdir)
{
        SH_LPCMD = lpcmd;
        SH_LPDIR = lpdir;
        return( gem_if( SHEL_RDEF ) );
}
*/

/*
WORD shel_wdef(LONG lpcmd, LONG lpdir)
{
        SH_LPCMD = lpcmd;
        SH_LPDIR = lpdir;
        return( gem_if( SHEL_WDEF ) );
}
*/

