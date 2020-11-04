/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "struct.h"
#include "aesdefs.h"
#include "aesext.h"
#include "aesvars.h"
#include "obdefs.h"
#include "intmath.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemsuper.h"
#include "gempd.h"
#include "gemaplib.h"
#include "geminit.h"
#include "gemevlib.h"
#include "gemmnlib.h"
#include "gemoblib.h"
#include "gemobed.h"
#include "gemfmlib.h"
#include "gemfslib.h"
#include "gemgrlib.h"
#include "gemgsxif.h"
#include "gemsclib.h"
#include "gemwmlib.h"
#include "gemrslib.h"
#include "gemshlib.h"
#include "gemfmalt.h"
#include "gemasm.h"

#include "string.h"


#if CONF_SERIAL_CONSOLE
#define ENABLE_KDEBUG
#endif

LONG super(WORD cx, AESPB *pcrys_blk);  /* called only from gemdosif.S */

GLOBAL WORD     gl_mnclick;

static void     *ad_rso;



#ifdef ENABLE_KDEBUG
static void aestrace(const char* message)
{
    char appname[AP_NAMELEN+1];
    const char *src = rlr->p_name;
    char *dest = appname;

    while (dest < &appname[AP_NAMELEN] && *src != ' ')
        *dest++ = *src++;
    *dest++ = '\0';

    kprintf("AES: %s: %s\n", appname, message);
}
#else
#define aestrace(a)
#endif

static UWORD crysbind(WORD opcode, AESGLOBAL *pglobal, WORD control[], WORD int_in[], WORD int_out[], LONG addr_in[])
{
    LONG    count, buparm;
    OBJECT  *tree;
    WORD    ret;
    WORD    unsupported = FALSE;

    count = 0L;
    ret = TRUE;

    switch(opcode)
    {
    /* Application Manager */
    case APPL_INIT:
        aestrace("appl_init()");
        pglobal->ap_version = AES_VERSION;  /* version number     */
        pglobal->ap_count = 1;              /* # concurrent procs */
        pglobal->ap_id = rlr->p_pid;
        tree = rs_trees[DESKTOP];
        pglobal->ap_private = tree[ROOT].ob_spec;
        pglobal->ap_planes = gl_nplanes;
        pglobal->ap_3resv = (LONG)&D;
        ret = ap_init();
        break;
    case APPL_READ:
    case APPL_WRITE:
        ret = ap_rdwr(opcode == APPL_READ ? MU_MESAG : MU_SDMSG,
                fpdnm(NULL, AP_RWID), AP_LENGTH, (WORD *)AP_PBUFF);
        break;
    case APPL_FIND:
        ret = ap_find((char *)AP_PNAME);
        break;
    case APPL_TPLAY:
        ap_tplay((EVNTREC *)AP_TBUFFER, AP_TLENGTH, AP_TSCALE);
        break;
    case APPL_TRECORD:
        ret = ap_trecd((EVNTREC *)AP_TBUFFER, AP_TLENGTH);
        break;
#if CONF_WITH_PCGEM
    case APPL_YIELD:
        dsptch();
        break;
#endif
    case APPL_EXIT:
        aestrace("appl_exit()");
        ap_exit();
        break;

    /* Event Manager */
    case EVNT_KEYBD:
        ret = ev_block(MU_KEYBD, 0x0L);
        break;
    case EVNT_BUTTON:
        ret = ev_button(B_CLICKS, B_MASK, B_STATE, &EV_MX);
        break;
    case EVNT_MOUSE:
        ev_mouse((MOBLK *)&MO_FLAGS, &EV_MX);
        break;
    case EVNT_MESAG:
        aestrace("evnt_mesag()");
        rlr->p_flags |= AP_MESAG;
        ev_mesag((WORD *)ME_PBUFF);
        if (*(WORD *)ME_PBUFF == AC_CLOSE)
            rlr->p_flags |= AP_ACCLOSE;
        break;
    case EVNT_TIMER:
        ev_timer(MAKE_ULONG(T_HICOUNT, T_LOCOUNT));
        break;
    case EVNT_MULTI:
        aestrace("evnt_multi()");
        if (MU_FLAGS & MU_MESAG)
            rlr->p_flags |= AP_MESAG;
        if (MU_FLAGS & MU_TIMER)
            count = MAKE_ULONG(MT_HICOUNT, MT_LOCOUNT);
        buparm = combine_cms(MB_CLICKS,MB_MASK,MB_STATE);
        ret = ev_multi(MU_FLAGS, (MOBLK *)&MMO1_FLAGS, (MOBLK *)&MMO2_FLAGS,
                        count, buparm, (WORD *)MME_PBUFF, &EV_MX);
        if ((ret & MU_MESAG) && (*(WORD *)MME_PBUFF == AC_CLOSE))
            rlr->p_flags |= AP_ACCLOSE;
        break;
    case EVNT_DCLICK:
        ret = ev_dclick(EV_DCRATE, EV_DCSETIT);
        break;

    /* Menu Manager */
    case MENU_BAR:
        mn_bar((OBJECT *)MM_ITREE, SHOW_IT);
        break;
    case MENU_ICHECK:
        do_chg((OBJECT *)MM_ITREE, ITEM_NUM, CHECKED, CHECK_IT, FALSE, FALSE);
        break;
    case MENU_IENABLE:
        do_chg((OBJECT *)MM_ITREE, (ITEM_NUM & 0x7fff), DISABLED,
                !ENABLE_IT, ((ITEM_NUM & 0x8000) != 0x0), FALSE);
        break;
    case MENU_TNORMAL:
        do_chg((OBJECT *)MM_ITREE, TITLE_NUM, SELECTED, !NORMAL_IT, TRUE, TRUE);
        break;
    case MENU_TEXT:
        tree = (OBJECT *)MM_ITREE;
        strcpy((char *)tree[ITEM_NUM].ob_spec,(char *)MM_PTEXT);
        break;
    case MENU_REGISTER:
        ret = mn_register(MM_PID, (char *)MM_PSTR);
        break;
    case MENU_UNREGISTER:
#if CONF_WITH_PCGEM
        /* distinguish between menu_unregister() and menu_popup() */
        if (IN_LEN == 1)
            mn_unregister( MM_MID );
        else
#endif
            unsupported = TRUE;
        break;
    case MENU_CLICK:
        /* distinguish between menu_click() and menu_attach() */
        /*
         * although menu_click() is PC-GEM only, it's always
         * enabled because the desktop uses it.
         */
        if (IN_LEN == 2) {
            if (MN_SETIT)
                gl_mnclick = MN_CLICK;
            ret = gl_mnclick;
        } else
            unsupported = TRUE;
        break;

    /* Object Manager */
    case OBJC_ADD:
        ob_add((OBJECT *)OB_TREE, OB_PARENT, OB_CHILD);
        break;
    case OBJC_DELETE:
        ret = ob_delete((OBJECT *)OB_TREE, OB_DELOB);
        break;
    case OBJC_DRAW:
        gsx_sclip((GRECT *)&OB_XCLIP);
        ob_draw((OBJECT *)OB_TREE, OB_DRAWOB, OB_DEPTH);
        break;
    case OBJC_FIND:
        ret = ob_find((OBJECT *)OB_TREE, OB_STARTOB, OB_DEPTH, OB_MX, OB_MY);
        break;
    case OBJC_OFFSET:
        ob_offset((OBJECT *)OB_TREE, OB_OBJ, &OB_XOFF, &OB_YOFF);
        break;
    case OBJC_ORDER:
        ob_order((OBJECT *)OB_TREE, OB_OBJ, OB_NEWPOS);
        break;
    case OBJC_EDIT:
        gsx_sclip(&gl_rfull);
        OB_ODX = OB_IDX;
        ret = ob_edit((OBJECT *)OB_TREE, OB_OBJ, OB_CHAR, &OB_ODX, OB_KIND);
        break;
    case OBJC_CHANGE:
        gsx_sclip((GRECT *)&OB_XCLIP);
        ob_change((OBJECT *)OB_TREE, OB_DRAWOB, OB_NEWSTATE, OB_REDRAW);
        break;

    /* Form Manager */
    case FORM_DO:
        ret = fm_do((OBJECT *)FM_FORM, FM_START);
        break;
    case FORM_DIAL:
        ret = fm_dial(FM_TYPE, (GRECT *)&FM_IX, (GRECT *)&FM_X);
        break;
    case FORM_ALERT:
        ret = fm_alert(FM_DEFBUT, (char *)FM_ASTRING);
        break;
    case FORM_ERROR:
        ret = fm_error(FM_ERRNUM);
        break;
    case FORM_CENTER:
        ob_center((OBJECT *)FM_FORM, (GRECT *)&FM_XC);
        break;
    case FORM_KEYBD:
        gsx_sclip(&gl_rfull);
        FM_OCHAR = FM_ICHAR;
        FM_ONXTOB = FM_INXTOB;
        ret = fm_keybd((OBJECT *)FM_FORM, FM_OBJ, &FM_OCHAR, &FM_ONXTOB);
        break;
    case FORM_BUTTON:
        gsx_sclip(&gl_rfull);
        ret = fm_button((OBJECT *)FM_FORM, FM_OBJ, FM_CLKS, &FM_ONXTOB);
        break;

    /* Graphics Manager */
    case GRAF_RUBBOX:
        gr_rubbox(GR_I1, GR_I2, GR_I3, GR_I4, &GR_O1, &GR_O2);
        break;
    case GRAF_DRAGBOX:
        gr_dragbox(GR_I1, GR_I2, GR_I3, GR_I4, (GRECT *)&GR_I5, &GR_O1, &GR_O2);
        break;
    case GRAF_MBOX:
        gr_movebox(GR_I1, GR_I2, GR_I3, GR_I4, GR_I5, GR_I6);
        break;
    case GRAF_GROWBOX:
        gr_growbox((GRECT *)&GR_I1, (GRECT *)&GR_I5);
        break;
    case GRAF_SHRINKBOX:
        gr_shrinkbox((GRECT *)&GR_I1, (GRECT *)&GR_I5);
        break;
    case GRAF_WATCHBOX:
        ret = gr_watchbox((OBJECT *)GR_TREE, GR_OBJ, GR_INSTATE, GR_OUTSTATE);
        break;
    case GRAF_SLIDEBOX:
        ret = gr_slidebox((OBJECT *)GR_TREE, GR_PARENT, GR_OBJ, GR_ISVERT);
        break;
    case GRAF_HANDLE:
        GR_WCHAR = gl_wchar;
        GR_HCHAR = gl_hchar;
        GR_WBOX = gl_wbox;
        GR_HBOX = gl_hbox;
        ret = gl_handle;
        break;
    case GRAF_MOUSE:
        gr_mouse(GR_MNUMBER, (MFORM *)GR_MADDR);
        break;
    case GRAF_MKSTATE:
        gr_mkstate(&GR_MX, &GR_MY, &GR_MSTATE, &GR_KSTATE);
        break;

    /* Scrap Manager */
    case SCRP_READ:
        ret = sc_read((char *)SC_PATH);
        break;
    case SCRP_WRITE:
        ret = sc_write((const char *)SC_PATH);
        break;
#if CONF_WITH_PCGEM
    case SCRP_CLEAR:
        ret = sc_clear();
        break;
#endif

    /* File Selector Manager */
    case FSEL_INPUT:
        ret = fs_input((char *)FS_IPATH, (char *)FS_ISEL, &FS_BUTTON, NULL);
        break;
    case FSEL_EXINPUT:
        ret = fs_input((char *)FS_IPATH, (char *)FS_ISEL, &FS_BUTTON, (char *)FS_TITLE);
        break;

    /* Window Manager */
    case WIND_CREATE:
        ret = wm_create(WM_KIND, (GRECT *)&WM_WX);
        break;
    case WIND_OPEN:
        wm_open(WM_HANDLE, (GRECT *)&WM_WX);
        break;
    case WIND_CLOSE:
        wm_close(WM_HANDLE);
        break;
    case WIND_DELETE:
        wm_delete(WM_HANDLE);
        break;
    case WIND_GET:
        wm_get(WM_HANDLE, WM_WFIELD, &WM_OX);
        break;
    case WIND_SET:
        wm_set(WM_HANDLE, WM_WFIELD, &WM_IX);
        break;
    case WIND_FIND:
        ret = wm_find(WM_MX, WM_MY);
        break;
    case WIND_UPDATE:
        wm_update(WM_BEGUP);
        break;
    case WIND_CALC:
        wm_calc(WM_WCTYPE, WM_WCKIND, WM_WCIX, WM_WCIY, WM_WCIW, WM_WCIH,
                &WM_WCOX, &WM_WCOY, &WM_WCOW, &WM_WCOH);
        break;
    case WIND_NEW:
        wm_new();
        break;

    /* Resource Manager */
    case RSRC_LOAD:
        ret = rs_load(pglobal, (char *)RS_PFNAME);
        break;
    case RSRC_FREE:
        ret = rs_free(pglobal);
        break;
    case RSRC_GADDR:
        ret = rs_gaddr(pglobal, RS_TYPE, RS_INDEX, &ad_rso);
        break;
    case RSRC_SADDR:
        ret = rs_saddr(pglobal, RS_TYPE, RS_INDEX, (void *)RS_INADDR);
        break;
    case RSRC_OBFIX:
        rs_obfix((OBJECT *)RS_TREE, RS_OBJ);
        break;

    /* Shell Manager */
    case SHEL_READ:
        sh_read((char *)SH_PCMD, (char *)SH_PTAIL);
        break;
    case SHEL_WRITE:
        ret = sh_write(SH_DOEX, SH_ISGR, SH_ISCR, (const char *)SH_PCMD, (const char *)SH_PTAIL);
        break;
    case SHEL_GET:
        sh_get((void*)SH_PBUFFER, SH_LEN);
        break;
    case SHEL_PUT:
        sh_put((const void *)SH_PDATA, SH_LEN);
        break;
    case SHEL_FIND:
        ret = sh_find((char *)SH_PATH);
        break;
    case SHEL_ENVRN:
        sh_envrn((char **)SH_PATH, (const char *)SH_SRCH);
        break;
#if CONF_WITH_PCGEM
    case SHEL_RDEF:
        sh_rdef((char *)SH_LPCMD, (char *)SH_LPDIR);
        break;
    case SHEL_WDEF:
        sh_wdef((const char *)SH_LPCMD, (const char *)SH_LPDIR);
        break;
#endif
    default:
        unsupported = TRUE;
        break;
    }

    if (unsupported)
    {
        KDEBUG(("Bad AES function %d\n", opcode));
        if (opcode != 0)    /* Ignore zero since some PRGs are this call */
            fm_show(ALNOFUNC, &opcode, 1);
        ret = -1;
    }

    return ret;
}


/*
 *  Routine that copies input parameters into local buffers, calls the
 *  appropriate routine via a case statement, copies return parameters
 *  from local buffers, and returns to the routine.
 */
static void xif(AESPB *pcrys_blk)
{
    WORD    control[C_SIZE];
    WORD    int_in[I_SIZE];
    WORD    int_out[O_SIZE];
    LONG    addr_in[AI_SIZE];

    memcpy(control, pcrys_blk->control, C_SIZE*sizeof(WORD));
    if (IN_LEN)
        memcpy(int_in, pcrys_blk->intin, min(IN_LEN,I_SIZE)*sizeof(WORD));
    if (AIN_LEN)
        memcpy(addr_in, pcrys_blk->addrin, min(AIN_LEN,AI_SIZE)*sizeof(LONG));

    RET_CODE = crysbind(OP_CODE, (AESGLOBAL *)pcrys_blk->global, control, int_in, int_out,
                                addr_in);

    if (OUT_LEN)
        memcpy(pcrys_blk->intout, int_out, OUT_LEN*sizeof(WORD));
    if (OP_CODE == RSRC_GADDR)
        pcrys_blk->addrout[0] = (LONG)ad_rso;
}


/*
 *  Supervisor entry point, called from gemdosif.S
 */
LONG super(WORD cx, AESPB *pcrys_blk)
{
    switch(cx)
    {
    case 200:
        xif(pcrys_blk);
        FALLTHROUGH;
    case 201:           /* undocumented TOS feature */
        dsptch();
    }

    return 0;
}


#if CONF_DEBUG_AES_STACK

void trapaes_debug_enter(void); /* called from gemdosif.S */
void trapaes_debug_exit(void);

#define MARKER_BYTE 0xaa

UBYTE* check_min; /* Minimum stack address to check */
UBYTE* check_max; /* Maximum stack address to check */
ULONG max_usage = 0; /* Maximum stack usage for an AES call, since the beginning */
UBYTE* min_pointer = NULL; /* Minimum detected stack pointer, since the beginning */

/* Called when entering AES trap #2 */
void trapaes_debug_enter(void)
{
    UBYTE* bottom = (UBYTE*)rlr->p_uda->u_super;
    UBYTE* current = (UBYTE*)rlr->p_uda->u_spsuper;
    UBYTE* top = bottom + sizeof rlr->p_uda->u_super;

    kprintf("AES enter rlr=%p bottom=%p current=%p top=%p free=%ld\n",
        rlr, bottom, current, top, current - bottom);

    if (!min_pointer)
        min_pointer = top;

    /* Fill unused stack stack space with the marker */
    check_min = bottom;
    check_max = current;
    memset(check_min, MARKER_BYTE, check_max - check_min);
}

/* Called when exiting AES trap #2 */
void trapaes_debug_exit(void)
{
    UBYTE* bottom = (UBYTE*)rlr->p_uda->u_super;
    UBYTE* current = (UBYTE*)rlr->p_uda->u_spsuper;
    UBYTE* top = bottom + sizeof rlr->p_uda->u_super;
    UBYTE* p;
    ULONG used;
    ULONG recommended;

    /* Detect the minimum used stack pointer during this AES call */
    for (p = check_min; p < check_max && *p == MARKER_BYTE; p++);
    used = check_max - p;
    if (used > max_usage)
        max_usage = used;

    if (p < min_pointer)
        min_pointer = p;

    recommended = (top - min_pointer) + 512; /* Heuristic */

    kprintf("AES exit  rlr=%p bottom=%p current=%p top=%p free=%ld, used=%ld, max_usage=%ld, recommended AES_STACK_SIZE=%ld\n",
        rlr, bottom, current, top, current - bottom, used, max_usage, recommended/4);
}

#endif /* CONF_DEBUG_AES_STACK */
