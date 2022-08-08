/*
 * aesdefs.h - Public definitions for AES system calls
 *
 * Copyright (C) 2019-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _AESDEFS_H
#define _AESDEFS_H

/*
 * AES opcodes
 */
/* Application Manager */
#define APPL_INIT       10
#define APPL_READ       11
#define APPL_WRITE      12
#define APPL_FIND       13
#define APPL_TPLAY      14
#define APPL_TRECORD    15
#define APPL_YIELD      17      /* PC-GEM function */
#define APPL_EXIT       19

/* Event Manager */
#define EVNT_KEYBD      20
#define EVNT_BUTTON     21
#define EVNT_MOUSE      22
#define EVNT_MESAG      23
#define EVNT_TIMER      24
#define EVNT_MULTI      25
#define EVNT_DCLICK     26

/* Menu Manager */
#define MENU_BAR        30
#define MENU_ICHECK     31
#define MENU_IENABLE    32
#define MENU_TNORMAL    33
#define MENU_TEXT       34
#define MENU_REGISTER   35
#define MENU_POPUP      36      /* AES 3.30 function */
#define MENU_ATTACH     37      /* AES 3.30 function */
#define MENU_ISTART     38      /* AES 3.30 function */
#define MENU_SETTINGS   39      /* AES 3.30 function */

/* Object Manager */
#define OBJC_ADD        40
#define OBJC_DELETE     41
#define OBJC_DRAW       42
#define OBJC_FIND       43
#define OBJC_OFFSET     44
#define OBJC_ORDER      45
#define OBJC_EDIT       46
#define OBJC_CHANGE     47
#define OBJC_SYSVAR     48      /* AES 3.40 function */

/* Form Manager */
#define FORM_DO         50
#define FORM_DIAL       51
#define FORM_ALERT      52
#define FORM_ERROR      53
#define FORM_CENTER     54
#define FORM_KEYBD      55
#define FORM_BUTTON     56

/* Graphics Manager */
#define GRAF_RUBBOX     70
#define GRAF_DRAGBOX    71
#define GRAF_MBOX       72
#define GRAF_GROWBOX    73
#define GRAF_SHRINKBOX  74
#define GRAF_WATCHBOX   75
#define GRAF_SLIDEBOX   76
#define GRAF_HANDLE     77
#define GRAF_MOUSE      78
#define GRAF_MKSTATE    79

/* Scrap Manager */
#define SCRP_READ       80
#define SCRP_WRITE      81
#define SCRP_CLEAR      82      /* PC-GEM function */

/* File Selector Manager */
#define FSEL_INPUT      90
#define FSEL_EXINPUT    91

/* Window Manager */
#define WIND_CREATE     100
#define WIND_OPEN       101
#define WIND_CLOSE      102
#define WIND_DELETE     103
#define WIND_GET        104
#define WIND_SET        105
#define WIND_FIND       106
#define WIND_UPDATE     107
#define WIND_CALC       108
#define WIND_NEW        109

/* Resource Manager */
#define RSRC_LOAD       110
#define RSRC_FREE       111
#define RSRC_GADDR      112
#define RSRC_SADDR      113
#define RSRC_OBFIX      114

/* Shell Manager */
#define SHEL_READ       120
#define SHEL_WRITE      121
#define SHEL_GET        122
#define SHEL_PUT        123
#define SHEL_FIND       124
#define SHEL_ENVRN      125
#define SHEL_RDEF       126     /* PC-GEM function */
#define SHEL_WDEF       127     /* PC-GEM function */


/*
 * max sizes for AES arrays
 */
#define C_SIZE      4
#define I_SIZE      16
#define O_SIZE      7
#define AI_SIZE     3
#define AO_SIZE     1


/*
 * control[] array usage
 */
#define OP_CODE     control[0]
#define IN_LEN      control[1]
#define OUT_LEN     control[2]
#define AIN_LEN     control[3]


/*
 * standard return code location
 */
#define RET_CODE    int_out[0]


/*
 * application library parameters
 */
                                        /* AES global array */
#define AP_VERSION  global[0]               /* AES version */
#define AP_COUNT    global[1]               /* max # of concurrent applications */
#define AP_ID       global[2]               /* application id */
#define AP_PRIVATE  ULONG_AT(&global[3])    /* for application use */
#define AP_PTREE    ULONG_AT(&global[5])    /* ptr to array of tree addresses */
                                        /* the following usage is not advertised */
#define AP_1RESV    ULONG_AT(&global[7])    /* address of rsc file in memory */
#define AP_2RESV0   global[9]               /* length of rsc file */
#define AP_2RESV1   global[10]              /* # of colour planes on screen */
#define AP_3RESV    ULONG_AT(&global[11])   /* ptr to AES global area D (struct THEGLO) */
#define AP_4RESV    ULONG_AT(&global[13])   /* used in AES 4.00 */

#define AP_GLSIZE   int_out[1]

#define AP_RWID     int_in[0]
#define AP_LENGTH   int_in[1]
#define AP_PBUFF    addr_in[0]

#define AP_PNAME    addr_in[0]

#define AP_TBUFFER  addr_in[0]
#define AP_TLENGTH  int_in[0]
#define AP_TSCALE   int_in[1]

#define AP_BVDISK   int_in[0]
#define AP_BVHARD   int_in[1]


/*
 * event library parameters
 */
#define B_CLICKS    int_in[0]
#define B_MASK      int_in[1]
#define B_STATE     int_in[2]

#define MO_FLAGS    int_in[0]
#define MO_X        int_in[1]
#define MO_Y        int_in[2]
#define MO_WIDTH    int_in[3]
#define MO_HEIGHT   int_in[4]

#define ME_PBUFF    addr_in[0]

#define T_LOCOUNT   int_in[0]
#define T_HICOUNT   int_in[1]

#define MU_FLAGS    int_in[0]
#define EV_MX       int_out[1]
#define EV_MY       int_out[2]
#define EV_MB       int_out[3]
#define EV_KS       int_out[4]
#define EV_KRET     int_out[5]
#define EV_BRET     int_out[6]


#define MB_CLICKS   int_in[1]
#define MB_MASK     int_in[2]
#define MB_STATE    int_in[3]

#define MMO1_FLAGS  int_in[4]
#define MMO1_X      int_in[5]
#define MMO1_Y      int_in[6]
#define MMO1_WIDTH  int_in[7]
#define MMO1_HEIGHT int_in[8]

#define MMO2_FLAGS  int_in[9]
#define MMO2_X      int_in[10]
#define MMO2_Y      int_in[11]
#define MMO2_WIDTH  int_in[12]
#define MMO2_HEIGHT int_in[13]

#define MME_PBUFF   addr_in[0]

#define MT_LOCOUNT  int_in[14]
#define MT_HICOUNT  int_in[15]

#define EV_DCRATE   int_in[0]
#define EV_DCSETIT  int_in[1]

/* message ids: evnt_mesag(), evnt_multi() */
#define MN_SELECTED 10

#define WM_REDRAW   20
#define WM_TOPPED   21
#define WM_CLOSED   22
#define WM_FULLED   23
#define WM_ARROWED  24
#define WM_HSLID    25
#define WM_VSLID    26
#define WM_SIZED    27
#define WM_MOVED    28
#define WM_NEWTOP   29
#define WM_UNTOPPED 30
#define WM_ONTOP    31

#define AC_OPEN     40
#define AC_CLOSE    41

/* WM_ARROWED message: arrow type */
#define WA_UPPAGE   0
#define WA_DNPAGE   1
#define WA_UPLINE   2
#define WA_DNLINE   3
#define WA_LFPAGE   4
#define WA_RTPAGE   5
#define WA_LFLINE   6
#define WA_RTLINE   7

/* event bitmask: evnt_multi() */
#define MU_KEYBD    0x0001
#define MU_BUTTON   0x0002
#define MU_M1       0x0004
#define MU_M2       0x0008
#define MU_MESAG    0x0010
#define MU_TIMER    0x0020
#define MU_TOSVALID 0x003F      /* valid bits for TOS compatibility */
#define MU_M3       0x0100      /* internal use only */


/*
 * menu library parameters
 */
#define MM_ITREE    addr_in[0]              /* ienable, icheck, tnormal */

#define MM_PSTR     addr_in[0]

#define MM_PTEXT    addr_in[1]

#define SHOW_IT     int_in[0]               /* bar */

#define ITEM_NUM    int_in[0]               /* icheck, ienable */
#define MM_PID      int_in[0]               /* register */
#define CHECK_IT    int_in[1]               /* icheck */
#define ENABLE_IT   int_in[1]               /* ienable */

#define TITLE_NUM   int_in[0]               /* tnormal */
#define NORMAL_IT   int_in[1]               /* tnormal */

#define MPOP_IN     addr_in[0]              /* menu library extensions */
#define MPOP_OUT    addr_in[1]
#define MPOP_XPOS   int_in[0]
#define MPOP_YPOS   int_in[1]

#define MPOP_FLAG   int_in[0]
#define MPOP_ITEM   int_in[1]
#define MPOP_SET    addr_in[0]

#define MPOP_ITEM2  int_in[2]

/* flag: menu_attach() */
#define ME_INQUIRE  0
#define ME_ATTACH   1
#define ME_REMOVE   2


/*
 * form library parameters
 */
#define FM_FORM     addr_in[0]
#define FM_START    int_in[0]

#define FM_TYPE     int_in[0]

#define FM_ERRNUM   int_in[0]

#define FM_DEFBUT   int_in[0]
#define FM_ASTRING  addr_in[0]

#define FM_IX       int_in[1]
#define FM_IY       int_in[2]
#define FM_IW       int_in[3]
#define FM_IH       int_in[4]
#define FM_X        int_in[5]
#define FM_Y        int_in[6]
#define FM_W        int_in[7]
#define FM_H        int_in[8]

#define FM_XC       int_out[1]
#define FM_YC       int_out[2]
#define FM_WC       int_out[3]
#define FM_HC       int_out[4]

#define FM_OBJ      int_in[0]
#define FM_ICHAR    int_in[1]
#define FM_INXTOB   int_in[2]

#define FM_ONXTOB   int_out[1]
#define FM_OCHAR    int_out[2]

#define FM_CLKS     int_in[1]

/* mode: form_dial() */
#define FMD_START   0
#define FMD_GROW    1
#define FMD_SHRINK  2
#define FMD_FINISH  3


/*
 * object library parameters
 */
#define OB_TREE     addr_in[0]              /* all ob procedures (except ob_sysvar) */

#define OB_DELOB    int_in[0]               /* ob_delete */

#define OB_DRAWOB   int_in[0]               /* ob_draw, ob_change */
#define OB_DEPTH    int_in[1]
#define OB_XCLIP    int_in[2]
#define OB_YCLIP    int_in[3]
#define OB_WCLIP    int_in[4]
#define OB_HCLIP    int_in[5]

#define OB_STARTOB  int_in[0]               /* ob_find */
/*#define OB_DEPTH    int_in[1]*/
#define OB_MX       int_in[2]
#define OB_MY       int_in[3]

#define OB_PARENT   int_in[0]               /* ob_add */
#define OB_CHILD    int_in[1]
#define OB_OBJ      int_in[0]               /* ob_offset, ob_order */
#define OB_XOFF     int_out[1]
#define OB_YOFF     int_out[2]
#define OB_NEWPOS   int_in[1]               /* ob_order */

#define OB_CHAR     int_in[1]               /* ob_edit */
#define OB_IDX      int_in[2]
#define OB_KIND     int_in[3]
#define OB_ODX      int_out[1]

#define OB_NEWSTATE int_in[6]               /* ob_change */
#define OB_REDRAW   int_in[7]

#define OB_MODE     int_in[0]               /* ob_sysvar */
#define OB_WHICH    int_in[1]
#define OB_I1       int_in[2]
#define OB_I2       int_in[3]
#define OB_O1       int_out[1]
#define OB_O2       int_out[2]


/*
 * graphics library parameters
 */
#define GR_I1       int_in[0]
#define GR_I2       int_in[1]
#define GR_I3       int_in[2]
#define GR_I4       int_in[3]
#define GR_I5       int_in[4]
#define GR_I6       int_in[5]
#define GR_I7       int_in[6]
#define GR_I8       int_in[7]

#define GR_O1       int_out[1]
#define GR_O2       int_out[2]

#define GR_TREE     addr_in[0]
#define GR_PARENT   int_in[0]
#define GR_OBJ      int_in[1]
#define GR_INSTATE  int_in[2]
#define GR_OUTSTATE int_in[3]

#define GR_ISVERT   int_in[2]

#define GR_MNUMBER  int_in[0]
#define GR_MADDR    addr_in[0]

#define GR_WCHAR    int_out[1]
#define GR_HCHAR    int_out[2]
#define GR_WBOX     int_out[3]
#define GR_HBOX     int_out[4]

#define GR_MX       int_out[1]
#define GR_MY       int_out[2]
#define GR_MSTATE   int_out[3]
#define GR_KSTATE   int_out[4]

/* mode: graf_mouse() */
#define ARROW       0
#define TEXT_CRSR   1
#define HOURGLASS   2
#define POINT_HAND  3
#define FLAT_HAND   4
#define THIN_CROSS  5
#define THICK_CROSS 6
#define OUTLN_CROSS 7
#define USER_DEF    255
#define M_OFF       256
#define M_ON        257
/* the following were added in AES 3.20 */
#define M_SAVE      258     /* save current mouse form in AESPD */
#define M_RESTORE   259     /* restore saved mouse form from AESPD */
#define M_PREVIOUS  260     /* restore previously-used global mouse form */

/* mouse form used by graf_mouse() etc */
typedef struct mform
{
        WORD    mf_xhot;
        WORD    mf_yhot;
        WORD    mf_nplanes;
        WORD    mf_bg;          /* mask colour index */
        WORD    mf_fg;          /* data colour index */
        UWORD   mf_mask[16];
        UWORD   mf_data[16];
} MFORM;


/*
 * scrap library parameters
 */
#define SC_PATH     addr_in[0]


/*
 * file selector library parameters
 */
#define FS_IPATH    addr_in[0]
#define FS_ISEL     addr_in[1]
#define FS_TITLE    addr_in[2]
#define FS_BUTTON   int_out[1]


/*
 * window library parameters
 */
#define WM_KIND     int_in[0]               /* wm_create */

#define WM_HANDLE   int_in[0]               /* wm_open, close, del */

#define WM_WX       int_in[1]               /* wm_open, wm_create */
#define WM_WY       int_in[2]
#define WM_WW       int_in[3]
#define WM_WH       int_in[4]

#define WM_MX       int_in[0]               /* wm_find */
#define WM_MY       int_in[1]

#define WM_WCTYPE   int_in[0]               /* wm_calc */
#define WM_WCKIND   int_in[1]
#define WM_WCIX     int_in[2]
#define WM_WCIY     int_in[3]
#define WM_WCIW     int_in[4]
#define WM_WCIH     int_in[5]
#define WM_WCOX     int_out[1]
#define WM_WCOY     int_out[2]
#define WM_WCOW     int_out[3]
#define WM_WCOH     int_out[4]

#define WM_BEGUP    int_in[0]               /* wm_update */

#define WM_WFIELD   int_in[1]

#define WM_IPRIVATE int_in[2]

#define WM_IKIND    int_in[2]

#define WM_IOTITLE  addr_in[0]              /* for name and info */

#define WM_IX       int_in[2]
#define WM_IY       int_in[3]
#define WM_IW       int_in[4]
#define WM_IH       int_in[5]

#define WM_OX       int_out[1]
#define WM_OY       int_out[2]
#define WM_OW       int_out[3]
#define WM_OH       int_out[4]

#define WM_ISLIDE   int_in[2]

#define WM_IRECTNUM int_in[6]

/* window gadgets: wind_create() */
#define NAME        0x0001
#define CLOSER      0x0002
#define FULLER      0x0004
#define MOVER       0x0008
#define INFO        0x0010
#define SIZER       0x0020
#define UPARROW     0x0040
#define DNARROW     0x0080
#define VSLIDE      0x0100
#define LFARROW     0x0200
#define RTARROW     0x0400
#define HSLIDE      0x0800
#define HOTCLOSE    0x1000      /* PC-GEM feature */

/* window attributes: wind_get(), wind_set() */
#define WF_KIND     1
#define WF_NAME     2
#define WF_INFO     3
#define WF_WXYWH    4
#define WF_CXYWH    5
#define WF_PXYWH    6
#define WF_FXYWH    7
#define WF_HSLIDE   8
#define WF_VSLIDE   9
#define WF_TOP      10
#define WF_FIRSTXYWH 11
#define WF_NEXTXYWH 12
#define WF_NEWDESK  14
#define WF_HSLSIZ   15
#define WF_VSLSIZ   16
#define WF_SCREEN   17
#define WF_COLOR    18
#define WF_DCOLOR   19

/* request type: wind_calc() */
#define WC_BORDER   0
#define WC_WORK     1

/* mode: wind_update() */
#define END_UPDATE 0
#define BEG_UPDATE 1
#define END_MCTRL  2
#define BEG_MCTRL  3


/*
 * resource library parameters
 */
#define RS_PFNAME   addr_in[0]              /* rs_init */
#define RS_TYPE     int_in[0]
#define RS_INDEX    int_in[1]
#define RS_INADDR   addr_in[0]
#define RS_OUTADDR  addr_out[0]

#define RS_TREE     addr_in[0]
#define RS_OBJ      int_in[0]

/* rsrc_gaddr(): type */
#define R_TREE      0
#define R_OBJECT    1
#define R_TEDINFO   2
#define R_ICONBLK   3
#define R_BITBLK    4
#define R_STRING    5
#define R_IMAGEDATA 6
#define R_OBSPEC    7
#define R_TEPTEXT   8                       /* sub ptrs in TEDINFO */
#define R_TEPTMPLT  9
#define R_TEPVALID  10
#define R_IBPMASK   11                      /* sub ptrs in ICONBLK */
#define R_IBPDATA   12
#define R_IBPTEXT   13
#define R_BIPDATA   14                      /* sub ptrs in BITBLK */
#define R_FRSTR     15                      /* free strings */
#define R_FRIMG     16                      /* free images */


/*
 * shell library parameters
 */
#define SH_DOEX     int_in[0]
#define SH_ISGR     int_in[1]
#define SH_ISCR     int_in[2]
#define SH_PCMD     addr_in[0]
#define SH_PTAIL    addr_in[1]

#define SH_PDATA    addr_in[0]
#define SH_PBUFFER  addr_in[0]

#define SH_LEN      int_in[0]

#define SH_PATH     addr_in[0]
#define SH_SRCH     addr_in[1]

#define SH_LPCMD    addr_in[0]
#define SH_LPDIR    addr_in[1]

/* 'doex' (mode): shel_write() */
#define SHW_NOEXEC  0                       /* just return to desktop */
#define SHW_EXEC    1                       /* run another program after this */
#define SHW_SHUTDOWN 4                      /* shutdown system */
#define SHW_RESCHNG 5                       /* change resolution */


/*
 * internal name of console/cli
 */
#define DEF_CONSOLE     "EMUCON"

#endif /* _AESDEFS_H */
