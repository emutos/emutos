/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
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

#include "portab.h"
#include "machine.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "taddr.h"
#include "gemlib.h"
#include "crysbind.h"
#include "gem.h"

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
#include "gemgraf.h"
#include "gemgsxif.h"
#include "gemsclib.h"
#include "gemwmlib.h"
#include "gemrslib.h"
#include "gemshlib.h"
#include "gemglobe.h"
#include "gemfmalt.h"
#include "gemdosif.h"
#include "gemasm.h"


#define CONTROL LLGET(pcrys_blk)
#define GGLOBAL LLGET(pcrys_blk+4)
#define INT_IN LLGET(pcrys_blk+8)
#define INT_OUT LLGET(pcrys_blk+12)
#define ADDR_IN LLGET(pcrys_blk+16)
#define ADDR_OUT LLGET(pcrys_blk+20)


GLOBAL WORD     gl_bvdisk;
GLOBAL WORD     gl_bvhard;
GLOBAL WORD     gl_dspcnt;
GLOBAL WORD     gl_mnpds[NUM_PDS];
GLOBAL WORD     gl_mnclick;

MLOCAL LONG     ad_rso;



#if SINGLAPP
UWORD crysbind(WORD opcode, LONG pglobal, UWORD int_in[], UWORD int_out[], LONG addr_in[])
#endif
#if MULTIAPP
UWORD crysbind(WORD opcode, LONG pglobal, UWORD int_in[], UWORD int_out[], LONG addr_in[], LONG addr_out[])
#endif
{
        LONG            maddr;
        LONG            tree;
        REG WORD        ret;

        ret = TRUE;
        switch(opcode)
        {       
                                /* Application Manager                  */
          case APPL_INIT:
#if SINGLAPP
                LWSET(pglobal, 0x0300);         /* version number       */
                LWSET(pglobal+2, 0x0001);       /* num of concurrent procs*/
#endif
#if MULTIAPP
                LWSET(pglobal, 0x0110);         /* version number       */
                LWSET(pglobal+2, NUM_DESKACC-1);/* num of concurrent procs*/
#endif
/*              LLSET(pglobal, 0x00010200L);
*/
                LWSET(pglobal+4, rlr->p_pid);
                sh_deskf(0, pglobal+6);
                LWSET(pglobal+20, gl_nplanes);
                LLSET(pglobal+22, ADDR(&D));
                LWSET(pglobal+26, gl_bvdisk);
                LWSET(pglobal+28, gl_bvhard);
                                                /* reset dispatcher     */
                                                /*  count to let the app*/
                                                /*  run a while.        */
                gl_dspcnt = NULL;
                ret = ap_init();
                break;
          case APPL_READ:
          case APPL_WRITE:
                ap_rdwr(opcode == APPL_READ ? MU_MESAG : MU_SDMSG, 
                        fpdnm(NULLPTR, AP_RWID), AP_LENGTH, AP_PBUFF);
                break;
          case APPL_FIND:
                ret = ap_find( AP_PNAME );
                break;
          case APPL_TPLAY:
                ap_tplay(AP_TBUFFER, AP_TLENGTH, AP_TSCALE);
                break;
          case APPL_TRECORD:
                ret = ap_trecd(AP_TBUFFER, AP_TLENGTH);
                break;
          case APPL_BVSET:
                gl_bvdisk = AP_BVDISK;
                gl_bvhard = AP_BVHARD;
                break;
          case APPL_YIELD:
                dsptch();
                break;
          case APPL_EXIT:
#if MULTIAPP
                ap_exit( TRUE );
#else
                ap_exit();
#endif
                break;
                                /* Event Manager                        */
          case EVNT_KEYBD:
                  ret = ev_block(MU_KEYBD, 0x0L);
                break;
          case EVNT_BUTTON:
                ret = ev_button(B_CLICKS, B_MASK, B_STATE, &EV_MX);
                break;
          case EVNT_MOUSE:
                /* FIXME: Ugly MOBLK pointer typecasting: */
                ret = ev_mouse((MOBLK *)&MO_FLAGS, &EV_MX);
                break;
          case EVNT_MESAG:
#if MULTIAPP
                                                /* standard 16 byte read */
                ev_mesag(MU_MESAG, rlr, ME_PBUFF);
#endif
#if SINGLAPP
                ap_rdwr(MU_MESAG, rlr, 16, ME_PBUFF);
#endif
                break;
          case EVNT_TIMER:
                ev_timer( HW(T_HICOUNT) + LW(T_LOCOUNT) );
                break;
          case EVNT_MULTI:
                  if (MU_FLAGS & MU_TIMER)
                  maddr = HW(MT_HICOUNT) + LW(MT_LOCOUNT);
if ((MB_MASK == 3) && (MB_STATE == 1))
{
  MB_STATE = 0;

}
          tree = HW(MB_CLICKS) | LW((MB_MASK << 8) | MB_STATE);
                /* FIXME: Ugly MOBLK pointer typecasting: */
                ret = ev_multi(MU_FLAGS, (MOBLK *)&MMO1_FLAGS, (MOBLK *)&MMO2_FLAGS, 
                        maddr, tree, MME_PBUFF, &EV_MX);
                break;
          case EVNT_DCLICK:
                ret = ev_dclick(EV_DCRATE, EV_DCSETIT);
                break;
                                /* Menu Manager                         */
          case MENU_BAR:
                if (gl_mnppd == rlr)
                  mn_bar(MM_ITREE, SHOW_IT, rlr->p_pid);
                else
                  menu_tree[rlr->p_pid] = (SHOW_IT) ? MM_ITREE : 0x0L;
                break;
          case MENU_ICHECK:
                do_chg(MM_ITREE, ITEM_NUM, CHECKED, CHECK_IT, FALSE, FALSE);
                break;
          case MENU_IENABLE:
                do_chg(MM_ITREE, (ITEM_NUM & 0x7fff), DISABLED, 
                        !ENABLE_IT, ((ITEM_NUM & 0x8000) != 0x0), FALSE);
                break;
          case MENU_TNORMAL:
                if (gl_mntree == menu_tree[rlr->p_pid])
                  do_chg(MM_ITREE, TITLE_NUM, SELECTED, !NORMAL_IT, 
                                TRUE, TRUE);
                break;
          case MENU_TEXT:
                tree = MM_ITREE;
                if (LHIWD(tree))
                  LSTCPY(LLGET(OB_SPEC(ITEM_NUM)), MM_PTEXT);   
                else
                  LSTCPY(desk_acc[ gl_mnpds[ LLOWD(tree) ] ], MM_PTEXT);
                break;
          case MENU_REGISTER:
                ret = mn_register(MM_PID, MM_PSTR);
                break;
          case MENU_UNREGISTER:
                if (MM_MID == -1) 
                  MM_MID = gl_mnpds[rlr->p_pid];
                mn_unregister( MM_MID );
                break;
                                /* Object Manager                       */
          case MENU_CLICK:
                if (MN_SETIT)
                  gl_mnclick = MN_CLICK;
                ret = gl_mnclick;
                break;

          case OBJC_ADD:
                ob_add(OB_TREE, OB_PARENT, OB_CHILD);
                break;
          case OBJC_DELETE:
                ob_delete(OB_TREE, OB_DELOB);
                break;
          case OBJC_DRAW:
                /* FIXME: Ugly typecasting: */
                gsx_sclip((GRECT *)&OB_XCLIP);
                ob_draw(OB_TREE, OB_DRAWOB, OB_DEPTH);
                break;
          case OBJC_FIND:
                ret = ob_find(OB_TREE, OB_STARTOB, OB_DEPTH, 
                                OB_MX, OB_MY);
                break;
          case OBJC_OFFSET:
                ob_offset(OB_TREE, OB_OBJ, &OB_XOFF, &OB_YOFF);
                break;
          case OBJC_ORDER:
                ob_order(OB_TREE, OB_OBJ, OB_NEWPOS);
                break;
          case OBJC_EDIT:
                gsx_sclip(&gl_rfull);
                OB_ODX = OB_IDX;
                ret = ob_edit(OB_TREE, OB_OBJ, OB_CHAR, &OB_ODX, OB_KIND);
                break;
          case OBJC_CHANGE:
                /* FIXME: Ugly typecasting: */
                gsx_sclip((GRECT *)&OB_XCLIP);
                ob_change(OB_TREE, OB_DRAWOB, OB_NEWSTATE, OB_REDRAW);
                break;
                                /* Form Manager                         */
          case FORM_DO:
                ret = fm_do(FM_FORM, FM_START);
                break;
          case FORM_DIAL:
                /* FIXME: Ugly typecasting: */
                ret = fm_dial(FM_TYPE, (GRECT *)&FM_X);
                break;
          case FORM_ALERT:
                ret = fm_alert(FM_DEFBUT, FM_ASTRING);
                break;
          case FORM_ERROR:
                ret = fm_error(FM_ERRNUM);
                break;
          case FORM_CENTER:
                /* FIXME: Ugly typecasting: */
                ob_center(FM_FORM, (GRECT *)&FM_XC);
                break;
          case FORM_KEYBD:
                gsx_sclip(&gl_rfull);
                FM_OCHAR = FM_ICHAR;
                FM_ONXTOB = FM_INXTOB;
                ret = fm_keybd(FM_FORM, FM_OBJ, &FM_OCHAR, &FM_ONXTOB);
                break;
          case FORM_BUTTON:
                gsx_sclip(&gl_rfull);
                ret = fm_button(FM_FORM, FM_OBJ, FM_CLKS, &FM_ONXTOB);
                break;
                                /* Graphics Manager                     */
#if MULTIAPP
          case PROC_CREATE:
                ret = prc_create(PR_IBEGADDR, PR_ISIZE, PR_ISSWAP, PR_ISGEM,
                                 &PR_ONUM );
                break;
          case PROC_RUN:
                ret = pr_run(PR_NUM, PR_ISGRAF, PR_ISOVER, PR_PCMD, PR_PTAIL);
                break;
          case PROC_DELETE:
                ret = pr_abort(PR_NUM);
                break;
          case PROC_INFO:
                ret = pr_info(PR_NUM, &PR_OISSWAP, &PR_OISGEM, &PR_OBEGADDR,
                        &PR_OCSIZE, &PR_OENDMEM, &PR_OSSIZE, &PR_OINTADDR);
                break;
          case PROC_MALLOC:
                ret = pr_malloc(PR_IBEGADDR, PR_ISIZE);
                break;
          case PROC_MFREE:
                ret = pr_mfree(PR_NUM);
                break;
          case PROC_SWITCH:
                ret = pr_switch(PR_NUM);
                break;
          case PROC_SETBLOCK:
                ret = pr_setblock(PR_NUM);
                break;
#endif
                                /* Graphics Manager                     */
          case GRAF_RUBBOX:
                gr_rubbox(GR_I1, GR_I2, GR_I3, GR_I4, 
                          &GR_O1, &GR_O2);
                break;
          case GRAF_DRAGBOX:
                /* FIXME: Ugly typecasting: */
                gr_dragbox(GR_I1, GR_I2, GR_I3, GR_I4, (GRECT *)&GR_I5, 
                           &GR_O1, &GR_O2);
                break;
          case GRAF_MBOX:
                gr_movebox(GR_I1, GR_I2, GR_I3, GR_I4, GR_I5, GR_I6);
                break;
          case GRAF_GROWBOX:
/*
                gr_growbox(&GR_I1, &GR_I5);
*/
                break;
          case GRAF_SHRINKBOX:
/*
                gr_shrinkbox(&GR_I1, &GR_I5);
*/
                break;
          case GRAF_WATCHBOX:
                ret = gr_watchbox(GR_TREE, GR_OBJ, GR_INSTATE, GR_OUTSTATE);
                break;
          case GRAF_SLIDEBOX:
                ret = gr_slidebox(GR_TREE, GR_PARENT, GR_OBJ, GR_ISVERT);
                break;
          case GRAF_HANDLE:
                GR_WCHAR = gl_wchar;
                GR_HCHAR = gl_hchar;
                GR_WBOX = gl_wbox;
                GR_HBOX = gl_hbox;
                ret = gl_handle;
                break;
          case GRAF_MOUSE:
                if (GR_MNUMBER > 255)
                {
                  if (GR_MNUMBER == M_OFF)
                    gsx_moff();
                  if (GR_MNUMBER == M_ON)
                    gsx_mon();
                }
                else
                {
                  if (GR_MNUMBER != 255)                
                  {
                    rs_gaddr(ad_sysglo, R_BIPDATA, 3 + GR_MNUMBER, &maddr);
                    maddr = LLGET(maddr);
                  }
                  else
                    maddr = GR_MADDR;
                  gsx_mfset(maddr);
                }
                break;
          case GRAF_MKSTATE:
                gr_mkstate(&GR_MX, &GR_MY, &GR_MSTATE, &GR_KSTATE);
                break;
                                /* Scrap Manager                        */
          case SCRP_READ:
                ret = sc_read(SC_PATH);
                break;
          case SCRP_WRITE:
                ret = sc_write(SC_PATH);
                break;
          case SCRP_CLEAR:
                ret = sc_clear();
                break;
                                /* File Selector Manager                */
          case FSEL_INPUT:
                ret = fs_input(FS_IPATH, FS_ISEL, &FS_BUTTON);
                break;
                                /* Window Manager                       */
          case WIND_CREATE:
                /* FIXME: Ugly typecasting: */
                ret = wm_create(WM_KIND, (GRECT *)&WM_WX);
                break;
          case WIND_OPEN:
                /* FIXME: Ugly typecasting: */
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
                wm_calc(WM_WCTYPE, WM_WCKIND, WM_WCIX, WM_WCIY, 
                        WM_WCIW, WM_WCIH, &WM_WCOX, &WM_WCOY, 
                        &WM_WCOW, &WM_WCOH);
                break;
                                /* Resource Manager                     */
          case RSRC_LOAD:
                ret = rs_load(pglobal, RS_PFNAME);
                break;
          case RSRC_FREE:
                ret = rs_free(pglobal);
                break;
          case RSRC_GADDR:
                ret = rs_gaddr(pglobal, RS_TYPE, RS_INDEX, &ad_rso);
                break;
          case RSRC_SADDR:
                ret = rs_saddr(pglobal, RS_TYPE, RS_INDEX, RS_INADDR);
                break;
          case RSRC_OBFIX:
                rs_obfix(RS_TREE, RS_OBJ);
                break;
                                /* Shell Manager                        */
          case SHEL_READ:
                sh_read(SH_PCMD, SH_PTAIL);
                break;
          case SHEL_WRITE:
                ret = sh_write(SH_DOEX, SH_ISGR, SH_ISCR, SH_PCMD, SH_PTAIL);
                break;
          case SHEL_GET:
                sh_get(SH_PBUFFER, SH_LEN);
                break;
          case SHEL_PUT:
                sh_put(SH_PDATA, SH_LEN);
                break;
          case SHEL_FIND:
                ret = sh_find(SH_PATH);
                break;
          case SHEL_ENVRN:
                sh_envrn(SH_PATH, SH_SRCH);
                break;
          case SHEL_RDEF:
                sh_rdef(SH_LPCMD, SH_LPDIR);
                break;
          case SHEL_WDEF:
                sh_wdef(SH_LPCMD, SH_LPDIR);
                break;
          case XGRF_STEPCALC:
                /* FIXME: Ugly GRECT pinter typecast: */
                gr_stepcalc( XGR_I1, XGR_I2, (GRECT *)&XGR_I3, &XGR_O1,
                   &XGR_O2, &XGR_O3, &XGR_O4, &XGR_O5 );
                break;
          case XGRF_2BOX:
                /* FIXME: Ugly GRECT pointer typecasting: */
                gr_2box(XGR_I4, XGR_I1, (GRECT *)&XGR_I6, XGR_I2,
                        XGR_I3, XGR_I5 );
                break;
          default:
                fm_show(ALNOFUNC, NULLPTR, 1);
                ret = -1;
                break;
        }
        return(ret);
}

/*
*       Routine that copies input parameters into local buffers,
*       calls the appropriate routine via a case statement, copies
*       return parameters from local buffers, and returns to the
*       routine.
*/

void xif(LONG pcrys_blk)
{
        UWORD           control[C_SIZE];
        UWORD           int_in[I_SIZE];
        UWORD           int_out[O_SIZE];
        LONG            addr_in[AI_SIZE];
#if MULTIAPP
        LONG            addr_out[AO_SIZE];
#endif

        LWCOPY(ADDR(&control[0]), CONTROL, C_SIZE);
        if (IN_LEN)
          LWCOPY(ADDR(&int_in[0]), INT_IN, IN_LEN);
        if (AIN_LEN)
          LWCOPY(ADDR(&addr_in[0]), ADDR_IN, AIN_LEN*2);

#if SINGLAPP
        int_out[0] = crysbind(OP_CODE, GGLOBAL, &int_in[0], &int_out[0], 
                                &addr_in[0]);
#endif
#if MULTIAPP
        int_out[0] = crysbind(OP_CODE, GGLOBAL, &int_in[0], &int_out[0], 
                                &addr_in[0], &addr_out[0]);
#endif

        if (OUT_LEN)
          LWCOPY(INT_OUT, ADDR(&int_out[0]), OUT_LEN);
        if (OP_CODE == RSRC_GADDR)
          LLSET(ADDR_OUT, ad_rso);
#if MULTIAPP
        if (OP_CODE == PROC_INFO)
          LWCOPY(ADDR_OUT, ADDR(&addr_out[0]), AOUT_LEN * 2);
#endif
}


/*
*       Supervisor entry point.  Stack frame must be exactly like
*       this if supret is to work.
*/
WORD super(LONG dummy, LONG pcrys_blk)
{
        xif(pcrys_blk);
        
        if ( (gl_dspcnt++ % 10) == 0 )
          dsptch();

        return(0);
}


