/*       DESKAPP.C      06/11/84 - 07/11/85             Lee Lorenzen    */
/*      for 3.0         3/6/86   - 5/6/86               MDF             */
/*      for 2.3         9/25/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*       Copyright (C) 2002-2020 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "string.h"
#include "obdefs.h"
#include "rsdefs.h"
#include "aesdefs.h"
#include "aesext.h"
#include "intmath.h"
#include "gsxdefs.h"
#include "gemdos.h"
#include "gemerror.h"
#include "optimize.h"

#include "deskbind.h"
#include "deskglob.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "aesbind.h"
#include "deskrsrc.h"
#include "deskfun.h"
#include "deskobj.h"
#include "deskmain.h"
#include "deskdir.h"
#include "deskins.h"
#include "desksupp.h"
#include "icons.h"
#include "xbiosbind.h"
#include "biosext.h"


/*
 *  width of drag box (in characters) when dragging a file displayed as text
 *  (the same as Atari TOS 2/3/4)
 */
#define DRAG_BOX_WIDTH  (2 + LEN_ZNODE + 1 + LEN_ZEXT)


/*
 *  maximum number of different desktop icons
 *
 *  this is limited by the lesser of:
 *  . the 2 hex digits used to store the icon number in EMUDESK.INF
 *  . the maximum size of an old-style .RSC file (if we assume that every
 *    ICONBLK has a unique data & mask, this gives a limit of 225)
 */
#define MAX_ICONS       256


/*
 *  maximum allowed text width for icons (excluding trailing null)
 */
#define MAX_ICONTEXT_WIDTH  12


/*
 *  standard ob_spec value for desktop/window border & text colours
 */
#define BORDER_TEXT_COLOURS ((BLACK << 12) | (BLACK << 8))


/*
 *  the following bit masks apply to EMUDESK.INF
 */
                            /* 'E' byte 1 */
#define INF_E1_VIEWTEXT 0x80    /* 1 => view folder contents as text */
#define INF_E1_SORTMASK 0x60    /* mask for folder contents sort (0-3 are valid) */
#define INF_E1_CONFDEL  0x10    /* 1 => confirm deletes */
#define INF_E1_CONFCPY  0x08    /* 1 => confirm copies */
#define INF_E1_DCMASK   0x07    /* mask for double-click speed (0-4 are valid) */
#define INF_E1_DEFAULT  (INF_E1_CONFDEL|INF_E1_CONFCPY|2)   /* default if no .INF */

                            /* 'E' byte 2 */
#define INF_E2_BLITTER  0x80    /* 1 => blitter is enabled */
#define INF_E2_IDTDATE  0x40    /* 1 => get date format from _IDT (ignore INF_E2_DAYMONTH bit) */
#define INF_E2_IDTTIME  0x20    /* 1 => get time format from _IDT (ignore INF_E2_24HCLOCK bit) */
#define INF_E2_ALLOWOVW 0x10    /* 1 => allow overwrites (yes, it's backwards) */
#define INF_E2_MNUCLICK 0x08    /* 1 => click to drop down menus */
#define INF_E2_DAYMONTH 0x04    /* 1 => day before month */
#define INF_E2_24HCLOCK 0x02    /* 1 => 24 hour clock */
                                /* following are defaults if no .INF */
#define INF_E2_DEFAULT  (INF_E2_BLITTER|INF_E2_IDTDATE|INF_E2_IDTTIME)

                            /* 'E' bytes 3-4 are video mode (see process_inf1() in aes/geminit.c) */

                            /* 'E' byte 5 */
#define INF_E5_NOSORT   0x80    /* 1 => do not sort folder contents (overrides INF_E1_SORTMASK) */
    /* the next 2 apply when launching an application that is not an 'installed application' */
#define INF_E5_APPDIR   0x40    /* 1 => set current dir to app's dir (else to top window dir) */
#define INF_E5_ISFULL   0x20    /* 1 => pass full path in args (else filename only) */
#define INF_E5_NOSIZE   0x10    /* 1 => disable 'size to fit' for windows */
#define INF_E5_NOCACHE  0x08    /* 1 => disable cache */
                                /* following are defaults if no .INF */
#define INF_E5_DEFAULT  (INF_E5_APPDIR|INF_E5_ISFULL)

                            /* 'Q' bytes 1-6 (default desktop/window pattern/colour values) */
#define INF_Q1_DEFAULT  (IP_4PATT << 4) | BLACK     /* desktop, 1 plane */
#define INF_Q2_DEFAULT  (IP_4PATT << 4) | WHITE     /* window, 1 plane */
#define INF_Q3_DEFAULT  (IP_4PATT << 4) | GREEN     /* desktop, 2 planes */
#define INF_Q4_DEFAULT  (IP_4PATT << 4) | WHITE     /* window, 2 planes */
#define INF_Q5_DEFAULT  (IP_4PATT << 4) | GREEN     /* desktop, >2 planes */
#define INF_Q6_DEFAULT  (IP_4PATT << 4) | WHITE     /* window, >2 planes */

                            /* application type entries (F/G/P/Y): first byte of 3-byte string */
#define INF_AT_APPDIR   0x01    /* 1 => set current dir to app's dir (else to top window dir) */
#define INF_AT_ISFULL   0x02    /* 1 => pass full path in args (else filename only) */

#define INF_REV_LEVEL   0x01    /* revision level when creating EMUDESK.INF */

/*
 * the following defines the number of bytes reserved for control panel
 * use at the start of the shell buffer.  these bytes are not modified
 * by EmuDesk, and are (currently) not saved to the EMUDESK.INF file.
 */
#define CPDATA_LEN      128

/*
 * the maximum size of an EMUDESK.INF line (see app_save())
 */
#define MAX_SIZE_INF_LINE   (MAXPATHLEN+100)    /* conservative */


static WORD     inf_rev_level;  /* revision level of current EMUDESK.INF */

static char     *atextptr;      /* current pointer within ANODE text buffer */


/* When we can't get EMUDESK.INF via shel_get() or by reading from
 * the disk, we create one dynamically from three sources:
 *  desk_inf_data1 below, for the #R, #E, #Q and #W lines
 *  the drivemask, for #M lines
 *  desk_inf_data2 below, for most of the remaining lines
 * The #T line is added at the end.
 *
 * NOTES re desk_inf_data1:
 *  1. this is essentially the standard starting set of lines for all
 *     EMUDESK.INF files
 *  2. if an auto-run program is defined, the corresponding #Z line will
 *     be created between the #R and the #E lines.
 *  3. the #Z and #E lines must be completely contained within the first
 *     INF_SIZE (currently 300) bytes of EMUDESK.INF - see geminit.c.
 *
 * NOTES re desk_inf_data2:
 *  1. the icon numbers MUST correspond to those in deskapp.h & the
 *     icon.rsc file itself.
 *  2. the first 'F' entry (for *.*) exists for compatibility between
 *     versions of EmuTOS up to 0.9.6 and versions from 0.9.7 on; it is
 *     not otherwise necessary.
 */
static const char desk_inf_data1[] =
    "#R 01\r\n"                         /* INF_REV_LEVEL */
    "#E 1A E0 00 00 60\r\n"             /* INF_E1_DEFAULT, INF_E2_DEFAULT, INF_E5_DEFAULT */
#if CONF_WITH_BACKGROUNDS
    "#Q 41 40 43 40 43 40\r\n"          /* INF_Q1_DEFAULT -> INF_Q6_DEFAULT */
#endif
    "#W 00 00 02 06 26 0C 00 @\r\n"
    "#W 00 00 02 08 26 0C 00 @\r\n"
    "#W 00 00 02 0A 26 0C 00 @\r\n"
    "#W 00 00 02 0D 26 0C 00 @\r\n";
static const char desk_inf_data2[] =
    "#F FF 07 @ *.*@\r\n"               /* 07 = IG_DOCU => document (see note above) */
    "#N FF 07 @ *.*@\r\n"               /* 07 = IG_DOCU => document */
    "#D FF 02 @ *.*@\r\n"               /* 02 = IG_FOLDER => folder */
    "#Y 06 FF *.GTP@ @\r\n"             /* 06 = IG_APPL => application */
    "#G 06 FF *.APP@ @\r\n"
    "#G 06 FF *.PRG@ @\r\n"
    "#P 06 FF *.TTP@ @\r\n"
    "#F 06 FF *.TOS@ @\r\n";


/*
 *  Issue the desktop "out of memory" alert in an infinite loop
 */
void nomem_alert(void)
{
    while(1)
        fun_alert(1,STNOMEM);
}


/*
 *  Allocate an application object
 *
 *  Issues an alert if there are none available
 */
ANODE *app_alloc(void)
{
    ANODE *pa;

    pa = G.g_aavail;
    if (pa)
    {
        G.g_aavail = pa->a_next;
        pa->a_next = G.g_ahead;
        G.g_ahead = pa;
    }
    else
        fun_alert(1, STAPGONE);

    return pa;
}


/*
 *  Free an application object
 */
void app_free(ANODE *pa)
{
    ANODE *ptmpa;

    if (G.g_ahead == pa)
        G.g_ahead = pa->a_next;
    else
    {
        ptmpa = G.g_ahead;
        while(ptmpa && (ptmpa->a_next != pa))
            ptmpa = ptmpa->a_next;
        if (ptmpa)
            ptmpa->a_next = pa->a_next;
    }
    pa->a_next = G.g_aavail;
    G.g_aavail = pa;
}


/*
 *  Scans a string into a buffer used for ANODE text (currently G.g_atext[])
 *  and ups the string pointer for next time.
 *
 *  Leading spaces are ignored; the scan is terminated by @, '\r', or
 *  the null character.
 *
 *  Returns a pointer to the next character to scan: if the scan was
 *  terminated by @ or '\r', this is the following character; otherwise
 *  this is the terminating character (null).
 */
char *scan_str(char *pcurr, char **ppstr)
{
    char *dest = atextptr;
    char *end = G.g_atext + SIZE_BUFF - 1;

    *ppstr = dest;              /* return ptr to start of string in buffer */

    while(*pcurr == ' ')        /* skip over leading spaces */
        pcurr++;

    for ( ; *pcurr; pcurr++)    /* copy string */
    {
        /* check for end markers */
        if ((*pcurr == '\r') || (*pcurr == '@'))
        {
            pcurr++;
            break;
        }
        /* check for buffer overflow */
        if (dest < end)
            *dest++ = *pcurr;
    }
    *dest = '\0';               /* terminate copy */

    if (dest < end)
        dest++;
    else
        KDEBUG(("scan_str(): ANODE string buffer is too small\n"));
    atextptr = dest;            /* update buffer ptr for next time */

    return pcurr;               /* next input character */
}


/*
 *  Parse a single line from the EMUDESK.INF file
 */
static char *app_parse(char *pcurr, ANODE *pa)
{
    WORD temp;

    pa->a_flags = 0;

    switch(*pcurr)
    {
    case 'T':                             /* Trash */
        pa->a_type  = AT_ISTRSH;
        pa->a_flags = AF_ISDESK;
        break;
    case 'M':                             /* Storage Media        */
        pa->a_type = AT_ISDISK;
        pa->a_flags = AF_ISDESK;
        break;
    case 'Y':                             /* GEM App needs parms  */
        pa->a_flags = AF_ISPARM;
        FALLTHROUGH;
    case 'G':                             /* GEM App no parms     */
        pa->a_type = AT_ISFILE;
        pa->a_flags |= AF_ISCRYS;
        break;
    case 'P':                             /* TOS App needs parms  */
        pa->a_flags = AF_ISPARM;
        FALLTHROUGH;
    case 'F':                             /* TOS App no parms     */
        pa->a_type = AT_ISFILE;
        break;
    case 'D':                             /* Directory (Folder)   */
        pa->a_type = AT_ISFOLD;
        break;
    case 'I':                             /* Executable file      */
        pa->a_flags = AF_ISEXEC;
        FALLTHROUGH;
    case 'N':                             /* Non-executable file  */
        pa->a_type = AT_ISFILE;
        pa->a_flags |= AF_WINDOW;
        break;
#if CONF_WITH_DESKTOP_SHORTCUTS
    case 'X':                           /* File shortcut on desktop */
        pa->a_type = AT_ISFILE;
        pa->a_flags = AF_ISDESK;
        break;
    case 'V':                           /* Directory shortcut on desktop */
        pa->a_type = AT_ISFOLD;
        pa->a_flags = AF_ISDESK;
        break;
#endif
#if CONF_WITH_PRINTER_ICON
    case 'O':                           /* Printer */
        pa->a_type  = AT_ISPRNT;
        pa->a_flags = AF_ISDESK;
        break;
#endif
    }
    pcurr++;

    if (pa->a_flags & AF_ISDESK)
    {
        pcurr = scan_2(pcurr, &pa->a_xspot);
        pcurr = scan_2(pcurr, &pa->a_yspot);
    }

    pcurr = scan_2(pcurr, &pa->a_aicon);
    if (pa->a_aicon >= G.g_numiblks)
        pa->a_aicon = IG_APPL;
    pcurr = scan_2(pcurr, &pa->a_dicon);
    if (pa->a_dicon >= G.g_numiblks)
        pa->a_dicon = IG_DOCU;
    pcurr++;

    /*
     * convert icon numbers in EMUDESK.INF revision 0
     */
    if (inf_rev_level == 0)
    {
        if (pa->a_aicon == IG_APPL_REV0)
            pa->a_aicon = IG_APPL;
        if (pa->a_dicon == IG_DOCU_REV0)
            pa->a_dicon = IG_DOCU;
    }

    if (pa->a_flags & AF_ISDESK)
    {
        pa->a_letter = (*pcurr == ' ') ? '\0' : *pcurr;
        pcurr += 2;
    }
    pcurr = scan_str(pcurr, &pa->a_pappl);
    pcurr = scan_str(pcurr, &pa->a_pdata);
    if (*pcurr == ' ')          /* new format */
    {
        pcurr++;
        if (*pcurr & INF_AT_APPDIR)
            pa->a_flags |= AF_APPDIR;
        if (*pcurr & INF_AT_ISFULL)
            pa->a_flags |= AF_ISFULL;
        pcurr = scan_2(pcurr+1, &temp);
        pa->a_funkey = temp;
        pcurr++;
    }
    pcurr = scan_str(pcurr, &pa->a_pargs);

#if CONF_WITH_VIEWER_SUPPORT
    if (is_viewer(pa))
    {
        pa->a_flags |= AF_VIEWER;   /* mark app as default viewer if appropriate */
        KDEBUG(("Found default viewer %s when parsing EMUDESK.INF\n",pa->a_pappl));
    }
#endif

#if CONF_WITH_DESKTOP_SHORTCUTS
    /* mark desktop shortcuts as executable based on file extension */
    if ((pa->a_flags == AF_ISDESK) && (pa->a_type == AT_ISFILE) &&
        is_executable(pa->a_pdata))
    {
        pa->a_flags |= AF_ISEXEC;
    }
#endif

    return pcurr;
}


void app_tran(WORD bi_num)
{
    BITBLK lb;

    lb = desk_rs_bitblk[bi_num];

    gsx_trans(lb.bi_pdata, lb.bi_wb, lb.bi_pdata, lb.bi_wb, lb.bi_hl);
}


/*
 * set up ICONBLK stuff - all the hard work is done here
 *
 * returns -1 iff insufficient memory
 */
static WORD setup_iconblks(const ICONBLK *ibstart, WORD count)
{
    char *maskstart, *datastart, *allocmem;
    char *p;
    WORD i, iwb, ih;
    LONG num_bytes, offset;

    /*
     * Calculate the icon width and size in bytes.  We assume that the width
     * in pixels is a multiple of 8, and that all icons have the same width &
     * height as the first
     */
    iwb = ibstart->ib_wicon / 8;
    ih = ibstart->ib_hicon;
    num_bytes = iwb * ih;

    /*
     * Allocate memory for:
     *  ICONBLKs
     *  icon masks
     *  icon data
     */
    allocmem = dos_alloc_anyram(count*(sizeof(ICONBLK)+2*num_bytes));
    if (!allocmem)
    {
        KDEBUG(("insufficient memory for %d desktop icons\n",count));
        malloc_fail_alert();
        return -1;
    }

    G.g_iblist = (ICONBLK*)allocmem;
    allocmem += count * sizeof(ICONBLK);
    maskstart = allocmem;
    allocmem += count * num_bytes;
    datastart = allocmem;

    /* initialise the count of ICONBLKs */
    G.g_numiblks = count;

    /*
     * Next, we copy the ICONBLKs to the g_iblist[] array:
     *  g_iblist[] points to the transformed data/transformed mask
     *  & is referenced by act_chkobj() in deskact.c, insa_icon()
     *  in deskins.c, and win_bldview() in deskwin.c
     */
    memcpy(G.g_iblist, ibstart, count*sizeof(ICONBLK));

    /*
     * Copy the icons' mask/data
     */
    for (i = 0, p = maskstart; i < count; i++, p += num_bytes)
        memcpy(p, ibstart[i].ib_pmask, num_bytes);
    for (i = 0, p = datastart; i < count; i++, p += num_bytes)
        memcpy(p, ibstart[i].ib_pdata, num_bytes);

    /*
     * Fix up the ICONBLKs
     */
    for (i = 0, offset = 0; i < count; i++, offset += num_bytes)
    {
        G.g_iblist[i].ib_pmask = (WORD *)(maskstart + offset);
        G.g_iblist[i].ib_pdata = (WORD *)(datastart + offset);
        G.g_iblist[i].ib_ptext = "";        /* precautionary */
        G.g_iblist[i].ib_char &= 0xff00;    /* strip any existing char */
        G.g_iblist[i].ib_ytext = ih;
        G.g_iblist[i].ib_wtext = MAX_ICONTEXT_WIDTH * gl_wschar;
        G.g_iblist[i].ib_htext = gl_hschar + 2;
    }

    /*
     * Finally we do the transforms
     */
    for (i = 0, p = maskstart; i < count; i++, p += num_bytes)
        gsx_trans(p, iwb, p, iwb, ih);
    for (i = 0, p = datastart; i < count; i++, p += num_bytes)
        gsx_trans(p, iwb, p, iwb, ih);

    return 0;
}

/*
 * try to load icons from user-supplied resource
 */
static WORD load_user_icons(void)
{
    RSHDR *hdr;
    ICONBLK *ibptr, *ib;
    WORD i, n, rc, w, h;
    char icon_rsc_name[sizeof(ICON_RSC_NAME)];

    /* Do not load user icons if Control was held on startup */
    if (bootflags & BOOTFLAG_SKIP_AUTO_ACC)
        return -1;

    /*
     * determine the number of icons in the user's icon resource
     *
     * note: since the icons must be in the root of the boot drive, we
     * make sure that they are there before calling rsrc_load(), which
     * could otherwise search in multiple directories, taking longer
     * and potentially causing useless form alerts
     */
    strcpy(icon_rsc_name, ICON_RSC_NAME);
    icon_rsc_name[0] += G.g_stdrv;  /* Adjust drive letter  */
    rc = -1;
    if (dos_sfirst(icon_rsc_name, FA_RO|FA_SYSTEM) == 0)
        if (rsrc_load(icon_rsc_name))
            rc = 0;
    if (rc < 0)
    {
        KDEBUG(("can't load user desktop icons from %s\n",icon_rsc_name));
        return -1;
    }

    hdr = (RSHDR *)(AP_1RESV);
    if (hdr->rsh_nib < NUM_GEM_IBLKS)   /* must have at least the minimum set */
    {
        KDEBUG(("too few user desktop icons (%d)\n",hdr->rsh_nib));
        rsrc_free();
        return -1;
    }
    n = min(hdr->rsh_nib,MAX_ICONS);    /* clamp count */

    /* get pointer to start of ICONBLKs in the resource
     * and validate their size
     */
    ibptr = (ICONBLK *)((char *)hdr + hdr->rsh_iconblk);
    w = icon_rs_iconblk[0].ib_wicon;    /* width/height from builtin */
    h = icon_rs_iconblk[0].ib_hicon;

    for (i = 0, ib = ibptr; i < n; i++, ib++)
    {
        if ((ib->ib_wicon != w) || (ib->ib_hicon != h))
        {
            KDEBUG(("user desktop icon %d has wrong size (%dx%d)\n",
                    i,ib->ib_wicon,ib->ib_hicon));
            rsrc_free();
            return -1;
        }
    }

    rc = setup_iconblks(ibptr, n);

    rsrc_free();

    return rc;
}


/*
 *  Initialise everything related to ANODEs
 *
 *  Returns -1 for failure (lack of memory)
 */
static WORD initialise_anodes(void)
{
    WORD i;

    /*
     * allocate buffer for ANODE text
     */
    atextptr = G.g_atext = dos_alloc_anyram(SIZE_BUFF);
    if (!G.g_atext)
    {
        KDEBUG(("insufficient memory for anode text buffer (need %ld bytes)\n",SIZE_BUFF));
        return -1;
    }

    /*
     * allocate space for ANODEs, chain the ANODEs together,
     * and set up the pointers
     */
    G.g_alist = dos_alloc_anyram(NUM_ANODES*sizeof(ANODE));
    if (!G.g_alist)
    {
        KDEBUG(("insufficient memory for %ld anodes\n",NUM_ANODES));
        return -1;
    }
    bzero(G.g_alist,NUM_ANODES*sizeof(ANODE));

    for (i = 0; i < NUM_ANODES-1; i++)
        G.g_alist[i].a_next = &G.g_alist[i+1];
    G.g_alist[i].a_next = (ANODE *) NULL;

    G.g_ahead = (ANODE *) NULL;
    G.g_aavail = G.g_alist;

    return 0;
}


/************************************************************************/
/* a p p _ r d i c o n                                                  */
/************************************************************************/
/* Note: this file originally loaded the icon data from a file (deskhi.icn
   or desklo.icn). But due to endianness problems and for ROM-ing the desktop,
   I changed this behaviour so that the icons are included in the program
   file now. Hope there aren't too much faults in this new version of this
   function. See icons.c, too.  - THH
   Note 2: this function was greatly rewritten in 2012 to use "real" ICONBLKs
   in icons.c, so that they can be generated by a resource editor - RFB
   Note 3: as of december 2016, EmuTOS now loads icons from the resource
   EMUICON.RSC in the root of the boot drive; if this is not available,
   it falls back to a minimal built-in set - RFB
*/
static WORD app_rdicon(void)
{
    /*
     * try to load user icons; if that fails, use builtin
     */
    if (load_user_icons() < 0)
        return setup_iconblks(icon_rs_iconblk, BUILTIN_IBLKS);

    return 0;
}


/*
 *  Read in the EMUDESK.INF file
 */
static void read_inf_file(char *infbuf)
{
    LONG ret;
    char inf_file_name[sizeof(INF_FILE_NAME)];

    strcpy(inf_file_name, INF_FILE_NAME);
    inf_file_name[0] += G.g_stdrv;      /* Adjust drive letter  */

    ret = dos_load_file(inf_file_name, SIZE_SHELBUF-CPDATA_LEN-1, infbuf);
    if (ret < 0L)
        ret = 0L;
    infbuf[ret] = '\0';
}


/*
 *  Create a default minimal internal EMUDESK.INF
 */
static void build_inf(char *infbuf, WORD xcnt, WORD ycnt)
{
    LONG drivemask;
    char *p, *text;
    int icon_index = 0;
    int drive_x = 0, drive_y = 0;
    int icon_x, icon_y;
    int icon_type;
    char drive_letter;
    int i;

    /* Environment and Windows */
    strcpy(infbuf, desk_inf_data1);
    p = infbuf + sizeof(desk_inf_data1) - 1;

    /* Scan for valid drives: */
    drivemask = dos_sdrv(dos_gdrv());
    for (i = 0; i < BLKDEVNUM; i++)
    {
        if (drivemask&(1L<<i))
        {
            drive_x = icon_index % xcnt; /* x position */
            drive_y = icon_index / xcnt; /* y position */
            icon_type = (i > 1) ? IG_HARD : IG_FLOPPY;
            drive_letter = 'A' + i;
            text = desktop_str_addr(STDISK);
            p += sprintf(p, "#M %02X %02X %02X FF %c %s %c@ @\r\n",
                    drive_x, drive_y, icon_type, drive_letter, text, drive_letter);
            icon_index++;
        }
    }

    /* Copy core data part 2 */
    strcpy(p, desk_inf_data2);
    p += sizeof(desk_inf_data2) - 1;

    /* add Trash icon to end */
    icon_x = 0;             /* Left */
    icon_y = ycnt - 1;      /* Bottom */
    if (drive_y >= icon_y)  /* if the last drive icon overflows over */
        icon_x = xcnt - 1;  /*  the trash row, force trash to right  */
    text = desktop_str_addr(STTRASH);
    p += sprintf(p, "#T %02X %02X %02X FF   %s@ @\r\n",
            icon_x, icon_y, IG_TRASH, text);

#if CONF_WITH_PRINTER_ICON
    /* add Printer icon, if room */
    if (icon_x == 0)        /* trash at left of bottom row */
    {
        icon_x = xcnt - 1;
        text = desktop_str_addr(STPRINT);
        sprintf(p, "#O %02X %02X %02X FF   %s@ @\r\n",
                icon_x, icon_y, IG_PRINT, text);
    }
#endif
}


/*
 *  Allocate a temporary buffer
 *
 *  Note: if the buffer cannot be allocated, we never return!
 */
static char *must_alloc_buf(LONG size)
{
    char *buf;

    buf = dos_alloc_anyram(size);
    if (!buf)
    {
        KDEBUG(("insufficient memory for temporary EMUDESK.INF buffer (need %ld bytes)\n",size));
        nomem_alert();          /* infinite loop */
    }

    return buf;
}


/*
 *  Initialize the application list by reading in the EMUDESK.INF
 *  file, either from memory or from the disk if the shel_get
 *  indicates no message is there.
 */
void app_start(void)
{
    WORD i, x, y;
    ANODE *pa;
    WSAVE *pws;
    CSAVE *cnxsave = G.g_cnxsave;
    char *buf, *inf_data;
    char *pcurr, *ptmp, *pauto = NULL;
    WORD envr, xcnt, ycnt, xcent, wincnt, dummy;

    MAYBE_UNUSED(i);

    /* initialise the ANODE stuff */
    if (initialise_anodes() < 0)
    {
        /* initialize_anodes() has already issued a KDEBUG() */
        nomem_alert();          /* infinite loop */
    }

    /* load the user or builtin icons */
    if (app_rdicon() < 0)
    {
        /* app_rdicon() has already issued a KDEBUG() */
        nomem_alert();          /* infinite loop */
    }

    /* allocate a temporary buffer for EMUDESK.INF */
    buf = must_alloc_buf(SIZE_SHELBUF);

    G.g_wicon = (MAX_ICONTEXT_WIDTH * gl_wschar) + (2 * G.g_iblist[0].ib_xtext);
    G.g_hicon = G.g_iblist[0].ib_hicon + gl_hschar + 2;

    xcnt = G.g_wdesk / (G.g_wicon+MIN_WINT);/* icon count */
    G.g_icw = G.g_wdesk / xcnt;             /* width */

    ycnt = G.g_hdesk / (G.g_hicon+MIN_HINT);/* icon count */
    G.g_ich = G.g_hdesk / ycnt;             /* height */

#if CONF_WITH_BACKGROUNDS
    /*
     * set up the default background pattern/colours, in case
     * we have an old saved EMUDESK.INF (without a 'Q' line)
     */
    G.g_patcol[0].desktop = INF_Q1_DEFAULT;
    G.g_patcol[0].window = INF_Q2_DEFAULT;
    G.g_patcol[1].desktop = INF_Q3_DEFAULT;
    G.g_patcol[1].window = INF_Q4_DEFAULT;
    G.g_patcol[2].desktop = INF_Q5_DEFAULT;
    G.g_patcol[2].window = INF_Q6_DEFAULT;
#endif

    shel_get(buf, SIZE_SHELBUF);
    inf_data = buf + CPDATA_LEN;
    if (!(bootflags & BOOTFLAG_SKIP_AUTO_ACC)
     && (inf_data[0] != '#'))               /* invalid signature    */
        read_inf_file(inf_data);            /*   so read from disk  */

    /* If there's still no EMUDESK.INF data, build one now */
    if (inf_data[0] != '#')
        build_inf(inf_data, xcnt, ycnt);

    wincnt = 0;
    inf_rev_level = 0;
    pcurr = inf_data;

    while(*pcurr)
    {
        if (*pcurr++ != '#')            /* look for start of line */
            continue;

        switch(*pcurr)
        {
        case 'R':                       /* revision level */
            pcurr++;
            pcurr = scan_2(pcurr,&inf_rev_level);
            break;
        case 'Z':                       /* autorun: Z nn pathname@ */
            pcurr = scan_str(pcurr+5,&pauto);   /* save pathname in buffer */
            break;                              /* (a bit wasteful)        */
        case 'T':                       /* Trash */
        case 'M':                       /* Media (Hard/Floppy)  */
        case 'G':                       /* GEM Application      */
        case 'Y':                       /* GEM App. with parms  */
        case 'F':                       /* File (DOS w/o parms) */
        case 'P':                       /* Parm (DOS w/ parms)  */
        case 'D':                       /* Directory            */
        case 'I':                       /* Executable file icon     */
        case 'N':                       /* Non-executable file icon */
#if CONF_WITH_DESKTOP_SHORTCUTS
        case 'X':                       /* File shortcut on desktop */
        case 'V':                       /* Directory shortcut on desktop */
#endif
#if CONF_WITH_PRINTER_ICON
        case 'O':                       /* Printer */
#endif
            pa = app_alloc();
            if (!pa)                    /* paranoia */
                return;
            pcurr = app_parse(pcurr, pa);
            if ((pa->a_type == AT_ISFILE) && pauto)
            {                           /* autorun exists & not yet merged */
                if (strcmp(pauto,pa->a_pappl) == 0)
                {
                    pa->a_flags |= AF_AUTORUN;  /* it's this program */
                    pauto = NULL;               /*  (and no other)   */
                }
            }
            break;
        case 'W':                       /* Window               */
            pcurr++;
            if (wincnt < NUM_WNODES)
            {
                pws = &cnxsave->cs_wnode[wincnt];
                pcurr = scan_2(pcurr, &pws->hsl_save);
                pcurr = scan_2(pcurr, &pws->vsl_save);
/* BugFix       */
                pcurr = scan_2(pcurr, &pws->x_save);
                pws->x_save *= gl_wchar;
                pcurr = scan_2(pcurr, &pws->y_save);
                pws->y_save *= gl_hchar;
                pcurr = scan_2(pcurr, &pws->w_save);
                pws->w_save *= gl_wchar;
                pcurr = scan_2(pcurr, &pws->h_save);
                pws->h_save *= gl_hchar;
/* */
                pcurr += 4;             /* skip no-longer-used field */
                ptmp = pws->pth_save;
                while(*pcurr != '@')
                    *ptmp++ = *pcurr++;
                *ptmp = '\0';
                wincnt += 1;
            }
            break;
        case 'E':                       /* Environment */
            pcurr++;
            pcurr = scan_2(pcurr, &envr);
            cnxsave->cs_view = (envr & INF_E1_VIEWTEXT) ? V_TEXT : V_ICON;
            cnxsave->cs_sort = ( (envr & INF_E1_SORTMASK) >> 5);
            cnxsave->cs_confdel = ( (envr & INF_E1_CONFDEL) != 0);
            cnxsave->cs_confcpy = ( (envr & INF_E1_CONFCPY) != 0);
            cnxsave->cs_dblclick = envr & INF_E1_DCMASK;

            pcurr = scan_2(pcurr, &envr);
            cnxsave->cs_blitter = ( (envr & INF_E2_BLITTER) != 0);
            cnxsave->cs_confovwr = ( (envr & INF_E2_ALLOWOVW) == 0);
            cnxsave->cs_mnuclick = ( (envr & INF_E2_MNUCLICK) != 0);
            menu_click(cnxsave->cs_mnuclick, 1);    /* tell system */
            if (envr & INF_E2_IDTDATE)
                cnxsave->cs_datefmt = DATEFORM_IDT;
            else
                cnxsave->cs_datefmt = (envr & INF_E2_DAYMONTH) ? DATEFORM_DMY : DATEFORM_MDY;
            if (envr & INF_E2_IDTTIME)
                cnxsave->cs_timefmt = TIMEFORM_IDT;
            else
                cnxsave->cs_timefmt = (envr & INF_E2_24HCLOCK) ? TIMEFORM_24H : TIMEFORM_12H;

            pcurr = scan_2(pcurr, &dummy);  /* skip video stuff */
            pcurr = scan_2(pcurr, &dummy);

            pcurr = scan_2(pcurr, &envr);
            if (envr & INF_E5_NOSORT)
                cnxsave->cs_sort = CS_NOSORT;
#if CONF_WITH_DESKTOP_CONFIG
            cnxsave->cs_appdir = ((envr & INF_E5_APPDIR) != 0);
            cnxsave->cs_fullpath = ((envr & INF_E5_ISFULL) != 0);
#endif
#if CONF_WITH_SIZE_TO_FIT
            cnxsave->cs_sizefit = ((envr & INF_E5_NOSIZE) == 0);
#endif
#if CONF_WITH_CACHE_CONTROL
            cnxsave->cs_cache = ((envr & INF_E5_NOCACHE) == 0);
#endif
            break;
#if CONF_WITH_BACKGROUNDS
        case 'Q':                       /* desktop/window pattern/colour */
            pcurr++;
            for (i = 0; i < 3; i++)
            {
                pcurr = scan_2(pcurr, &G.g_patcol[i].desktop);
                pcurr = scan_2(pcurr, &G.g_patcol[i].window);
            }
            break;
#endif
        /*
         * Menu item shortcuts can only be saved to EMUDESK.INF if
         * CONF_WITH_DESKTOP_CONFIG is enabled.  However, we allow all
         * versions of the ROM to _load_ menu item shortcuts.
         */
        case 'K':                       /* menu item shortcuts */
            pcurr++;
            for (i = 0; i < NUM_SHORTCUTS; i++)
            {
                WORD temp;
                pcurr = scan_2(pcurr, &temp);
                menu_shortcuts[i] = (UBYTE)temp;
            }
        break;
        }
    }

    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if (pa->a_flags & AF_ISDESK)
        {
            x = pa->a_xspot * G.g_icw;
            y = pa->a_yspot * G.g_ich + G.g_ydesk;
            snap_icon(x, y, &pa->a_xspot, &pa->a_yspot, 0, 0);
        }
    }

    /* set up outlines for dragging files displayed as icons */
    bzero(G.g_xyicon, sizeof(G.g_xyicon));
    xcent = (G.g_wicon - G.g_iblist[0].ib_wicon) / 2;
    G.g_xyicon[0].x = xcent;
    G.g_xyicon[1].x = xcent;
    G.g_xyicon[1].y = G.g_hicon-gl_hschar-2;
    G.g_xyicon[2].y = G.g_hicon-gl_hschar-2;
    G.g_xyicon[3].y = G.g_hicon;
    G.g_xyicon[4].x = G.g_wicon;
    G.g_xyicon[4].y = G.g_hicon;
    G.g_xyicon[5].x = G.g_wicon;
    G.g_xyicon[5].y = G.g_hicon-gl_hschar-2;
    G.g_xyicon[6].x = G.g_wicon-xcent;
    G.g_xyicon[6].y = G.g_hicon-gl_hschar-2;
    G.g_xyicon[7].x = G.g_wicon-xcent;
    G.g_xyicon[8].x = xcent;

    /* set up outlines for dragging files displayed as text */
    bzero(G.g_xytext, sizeof(G.g_xytext));
    G.g_xytext[1].x = gl_wchar * DRAG_BOX_WIDTH;
    G.g_xytext[2].x = gl_wchar * DRAG_BOX_WIDTH;
    G.g_xytext[2].y = gl_hchar;
    G.g_xytext[3].y = gl_hchar;

    dos_free(buf);
}

/*
 *  Reverse the list of ANODEs
 */
static void app_revit(void)
{
    ANODE *pa;
    ANODE *pnxtpa;

    /* reverse list */
    pa = G.g_ahead;
    G.g_ahead = (ANODE *) NULL;
    while(pa)
    {
        pnxtpa = pa->a_next;
        pa->a_next = G.g_ahead;
        G.g_ahead = pa;
        pa = pnxtpa;
    }
}


/*
 *  Perform the actual save of the EMUDESK.INF file
 */
static void save_to_disk(char *buf, WORD len)
{
    LONG ret;
    char inf_file_name[sizeof(INF_FILE_NAME)];

    /* make sure user really wants to save the desktop */
    if (fun_alert(1, STSVINF) != 1)
        return;

    strcpy(inf_file_name, INF_FILE_NAME);
    inf_file_name[0] += G.g_stdrv;  /* Adjust drive letter  */

    while(1)
    {
        ret = dos_create(inf_file_name, 0);
        if (ret >= 0L)
        {
            WORD fh = (WORD) ret;
            ret = dos_write(fh, len, buf);
            dos_close(fh);
            if (ret != len)             /* write error */
                ret = -1L;
            else ret = 0L;
        }
        if (ret == 0L)
            break;
        /* error - prompt user */
        if (fun_alert_merge(1, STCRTFIL, filename_start(inf_file_name)) != 2)
            return;                     /* if not retrying, return now */
    }

    /*
     * we can only be here if the file was created successfully, so update
     * any open windows for the directory containing the saved file
     */
    del_fname(inf_file_name);   /* convert to pathname ending in *.* */
    fun_rebld(inf_file_name);   /* rebuild all matching open windows */
}


/*
 * Get video mode to store into EMUDESK.INF
 */
static WORD desk_get_videomode(void)
{
    WORD mode;

#ifdef MACHINE_AMIGA
    mode = amiga_vgetmode();
#else

#if CONF_WITH_VIDEL
    mode = get_videl_mode();
    if (!mode)                      /* i.e. not videl */
#endif
        mode = 0xff00 | Getrez();

#endif /* MACHINE_AMIGA */

    return mode;
}

/*
 *  Save the current state of all the desktop options/icons/windows
 *  etc to memory and, optionally, to a file called EMUDESK.INF
 */
void app_save(WORD todisk)
{
    WORD i, len;
    WORD env1, env2, mode, env5;
    char type;
    char *outbuf, *inf_data, *pcurr, *ptmp;
    ANODE *pa;
    WSAVE *pws;
    CSAVE *cnxsave = G.g_cnxsave;

    /*
     * allocate a temporary buffer: we make it larger than the size of
     * the shell buffer by at least the length of one line, so that we
     * only need to check for buffer overflow at the end of each line
     */
    outbuf = must_alloc_buf(SIZE_SHELBUF+MAX_SIZE_INF_LINE);

    shel_get(outbuf, CPDATA_LEN);
    inf_data = outbuf + CPDATA_LEN;
    pcurr = inf_data;

    /* save revision level */
    pcurr += sprintf(pcurr,"#R %02X\r\n",INF_REV_LEVEL);

    /* save autorun (if any) */
    for (pa = G.g_ahead; pa; pa = pa->a_next)
        if (pa->a_flags & AF_AUTORUN)
            pcurr += sprintf(pcurr,"#Z %02X %s@\r\n",pa->a_flags&AF_ISCRYS,pa->a_pappl);

    /* save environment */
    env1 = (cnxsave->cs_view == V_TEXT) ? INF_E1_VIEWTEXT : 0x00;
    env1 |= ((cnxsave->cs_sort) << 5) & INF_E1_SORTMASK;
    env1 |= (cnxsave->cs_confdel) ? INF_E1_CONFDEL : 0x00;
    env1 |= (cnxsave->cs_confcpy) ? INF_E1_CONFCPY : 0x00;
    env1 |= cnxsave->cs_dblclick & INF_E1_DCMASK;
    env2 = (cnxsave->cs_blitter) ? INF_E2_BLITTER : 0x00;
    env2 |= (cnxsave->cs_confovwr) ? 0x00 : INF_E2_ALLOWOVW;
    env2 |= (cnxsave->cs_mnuclick) ? INF_E2_MNUCLICK : 0x00;
    switch(cnxsave->cs_datefmt)
    {
    case DATEFORM_IDT:
        env2 |= INF_E2_IDTDATE;
        break;
    case DATEFORM_DMY:
        env2 |= INF_E2_DAYMONTH;
        break;
    }
    switch(cnxsave->cs_timefmt)
    {
    case TIMEFORM_IDT:
        env2 |= INF_E2_IDTTIME;
        break;
    case TIMEFORM_24H:
        env2 |= INF_E2_24HCLOCK;
        break;
    }
    mode = desk_get_videomode();
    env5 = (cnxsave->cs_sort == CS_NOSORT) ? INF_E5_NOSORT : 0;
#if CONF_WITH_DESKTOP_CONFIG
    env5 |= cnxsave->cs_appdir ? INF_E5_APPDIR : 0;
    env5 |= cnxsave->cs_fullpath ? INF_E5_ISFULL : 0;
#endif
#if CONF_WITH_SIZE_TO_FIT
    env5 |= (cnxsave->cs_sizefit) ? 0 : INF_E5_NOSIZE;
#endif
#if CONF_WITH_CACHE_CONTROL
    env5 |= (cnxsave->cs_cache) ? 0 : INF_E5_NOCACHE;
#endif
    pcurr += sprintf(pcurr,"#E %02X %02X %02X %02X %02X\r\n",
                    env1,env2,HIBYTE(mode),LOBYTE(mode),env5);

#if CONF_WITH_BACKGROUNDS
    /* save desktop/window colour/patterns */
    pcurr += sprintf(pcurr,"#Q %02X %02X %02X %02X %02X %02X\r\n",
                    G.g_patcol[0].desktop,G.g_patcol[0].window,
                    G.g_patcol[1].desktop,G.g_patcol[1].window,
                    G.g_patcol[2].desktop,G.g_patcol[2].window);
#endif

#if CONF_WITH_DESKTOP_CONFIG
    /* save menu item shortcuts */
    *pcurr++ = '#';
    *pcurr++ = 'K';
    for (i = 0; i < NUM_SHORTCUTS; i++)
        pcurr += sprintf(pcurr," %02X", menu_shortcuts[i]);
    *pcurr++= '\r';
    *pcurr++= '\n';
#endif

    /* save windows */
    for (i = 0; i < NUM_WNODES; i++)
    {
        pws = &cnxsave->cs_wnode[i];
        ptmp = pws->pth_save;
        pcurr += sprintf(pcurr,"#W %02X %02X %02X %02X %02X %02X %02X",
                        pws->hsl_save,pws->vsl_save,pws->x_save/gl_wchar,
                        pws->y_save/gl_hchar,pws->w_save/gl_wchar,
                        pws->h_save/gl_hchar,0);
        pcurr += sprintf(pcurr," %s@\r\n",(*ptmp!='@')?ptmp:"");
    }

    /*
     * reverse the ANODE list before we write it.  this ensures
     * that the generic ANODEs are written ahead of the ones
     * created for installed applications.  when we read it back
     * in, using app_alloc() to allocate the ANODEs, the
     * generic ANODEs will once again be at the end of the list.
     */
    app_revit();

    /* save ANODE list */
    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        switch(pa->a_type)
        {
        case AT_ISDISK:
            type = 'M';
            break;
        case AT_ISFILE:
#if CONF_WITH_DESKTOP_SHORTCUTS
            if (pa->a_flags & AF_ISDESK)
                type = 'X';
            else
#endif
            {
                if (pa->a_flags & AF_WINDOW)
                    type = (pa->a_flags & AF_ISEXEC) ? 'I' : 'N';
                else if (pa->a_flags & AF_ISCRYS)
                    type = (pa->a_flags & AF_ISPARM) ? 'Y' : 'G';
                else
                    type = (pa->a_flags & AF_ISPARM) ? 'P' : 'F';
            }
            break;
        case AT_ISFOLD:
#if CONF_WITH_DESKTOP_SHORTCUTS
            if (pa->a_flags & AF_ISDESK)
                type = 'V';
            else
#endif
                type = 'D';
            break;
        case AT_ISTRSH:     /* Trash */
            type = 'T';
            break;
#if CONF_WITH_PRINTER_ICON
        case AT_ISPRNT:     /* Printer */
            type = 'O';
            break;
#endif
        default:
            type = ' ';
        }
        pcurr += sprintf(pcurr,"#%c",type);
        if (pa->a_flags & AF_ISDESK)
            pcurr += sprintf(pcurr," %02X %02X",(pa->a_xspot/G.g_icw)&0x00ff,
                            (max(0,(pa->a_yspot-G.g_ydesk))/G.g_ich)&0x00ff);
        pcurr += sprintf(pcurr," %02X %02X",pa->a_aicon&0x00ff,pa->a_dicon&0x00ff);
        if (pa->a_flags & AF_ISDESK)
            pcurr += sprintf(pcurr," %c",pa->a_letter?pa->a_letter:' ');
        pcurr += sprintf(pcurr," %s@ %s@",pa->a_pappl,pa->a_pdata);
        if ((pa->a_type == AT_ISFILE) && !(pa->a_flags & AF_ISDESK))
        {
            type = 0;
            if (pa->a_flags & AF_APPDIR)
                type |= INF_AT_APPDIR;
            if (pa->a_flags & AF_ISFULL)
                type |= INF_AT_ISFULL;
            pcurr += sprintf(pcurr," %X%02X %s@",type,pa->a_funkey,pa->a_pargs);
        }
        *pcurr++ = '\r';
        *pcurr++ = '\n';
        if (pcurr-outbuf >= SIZE_SHELBUF)   /* overflow check */
        {
            for (pcurr -= 2; *pcurr != '\n'; pcurr--)
                ;
            pcurr++;        /* point after previous line */
            break;
        }
    }
    *pcurr = 0x00;

    /* reverse list back */
    app_revit();

    /* calculate size */
    len = pcurr - outbuf;

    /* save in memory */
    shel_put(outbuf, len+1);  /* also save terminating NUL */

    /* save to disk */
    if (todisk)
        save_to_disk(inf_data, len-CPDATA_LEN);

    dos_free(outbuf);
}


/*
 *  Set desktop (& window) background pattern/colour
 */
static void set_background(void)
{
#if CONF_WITH_BACKGROUNDS
    int i, n;
    OBJECT *tree;

    switch(gl_nplanes)
    {
    case 1:
        n = 0;
        break;
    case 2:
        n = 1;
        break;
    default:
        n = 2;
        break;
    }

    for (i = DROOT+1, tree = G.g_screen+i; i < WOBS_START; i++, tree++)
        tree->ob_spec = G.g_patcol[n].window | BORDER_TEXT_COLOURS;
    G.g_screen[DROOT].ob_spec = G.g_patcol[n].desktop | BORDER_TEXT_COLOURS;
#else
    G.g_screen[DROOT].ob_spec = AP_PRIVATE;
#endif
}


/*
 *  Build and redraw the desktop list of objects
 */
void app_blddesk(void)
{
    WORD obid, icon;
    ANODE *pa;
    OBJECT *pob;
    SCREENINFO *si;
    ICONBLK *pic;

    /* free all this window's kids and set size  */
    obj_wfree(DROOT, 0, 0, gl_width, gl_height);

    /* set background pattern/colours */
    set_background();

    /*
     * reverse the order of the ANODEs, so that the disk icons will be
     * allocated in the same sequence as they appear on the desktop.
     * this is important for 'Show info'.
     */
    app_revit();

    for(pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if (pa->a_flags & AF_ISDESK)
        {
            obid = obj_ialloc(DROOT, pa->a_xspot, pa->a_yspot,
                                        G.g_wicon, G.g_hicon);
            if (!obid)      /* can't allocate item object */
            {
                KDEBUG(("app_blddesk(): can't create desktop item object\n"));
                break;
            }

            /* remember it */
            pa->a_obid = obid;

            /* choose appropriate icon */
            if ((pa->a_dicon < 0)
             || ((pa->a_type == AT_ISFILE) && (pa->a_flags & AF_ISEXEC)))
                icon = pa->a_aicon;
            else
                icon = pa->a_dicon;

            /* build object */
            pob = &G.g_screen[obid];
            pob->ob_state = NORMAL;
            pob->ob_flags = NONE;
            pob->ob_type = G_ICON;
            si = &G.g_screeninfo[obid];
            si->fnptr = NULL;
            si->u.icon.index = icon;
            pic = &si->u.icon.block;
            pob->ob_spec = (LONG)pic;
            memcpy(pic, &G.g_iblist[icon], sizeof(ICONBLK));
            pic->ib_xicon = ((G.g_wicon - pic->ib_wicon) / 2);
            pic->ib_ptext = pa->a_pappl;
            pic->ib_char |= pa->a_letter;
        }
    }

    app_revit();    /* back to normal ... */

    do_wredraw(DESKWH, (GRECT *)&G.g_xdesk);
}


/*
 *  Find ANODE by object id
 */
ANODE *app_afind_by_id(WORD obid)
{
    ANODE *pa;

    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if (pa->a_obid == obid)
            return pa;
    }

    return NULL;
}


/*
 *  Find ANODE by name & type
 *
 *  Note: ANODEs with a flag bit matching the 'ignore' parameter are
 *  ignored during the search.
 *
 *  Returns match type in *pisapp:
 *      TRUE if name matches application name
 *      FALSE if name matches data name
 */
ANODE *app_afind_by_name(WORD atype, WORD ignore, char *pspec, char *pname, WORD *pisapp)
{
    ANODE *pa;
    WORD match;
    char pathname[MAXPATHLEN];

    strcpy(pathname,pspec);                 /* build full pathname */
    strcpy(filename_start(pathname),pname);

    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if (pa->a_flags & ignore)
            continue;
        if (pa->a_type == atype)
        {
            if (wildcmp(pa->a_pdata, pname))
            {
                *pisapp = FALSE;
                return pa;
            }
            if ((pa->a_pappl[0] == '*') || (pa->a_pappl[0] == '?'))
                match = wildcmp(pa->a_pappl, pname);
            else match = !strcmp(pa->a_pappl, pathname);
            if (match)
            {
                *pisapp = TRUE;
                return pa;
            }
        }
    }

    return NULL;
}


#if CONF_WITH_VIEWER_SUPPORT
ANODE *app_afind_viewer(void)
{
    ANODE *pa;

    for (pa = G.g_ahead; pa; pa = pa->a_next)
        if (pa->a_flags & AF_VIEWER)
            break;

    return pa;
}
#endif


#if CONF_WITH_READ_INF
/*
 *  Implements "Read .INF file" function
 *
 *  Returns TRUE iff file was successfully read into shell buffer
 */
BOOL app_read_inf(void)
{
    LONG rc;
    WORD button;
    BOOL ret = FALSE;
    char *p, *buf;
    char path[MAXPATHLEN], fname[LEN_ZFNAME];

    /* prompt for filename */
    strcpy(path, INF_FILE_WILD);
    *path += G.g_stdrv;
    strcpy(fname, INF_FILE_NAME+3);     /* excluding "X:\" */
    p = desktop_str_addr(STRDINF);
    rc = fsel_exinput(path, fname, &button, p);
    if ((rc == 0) || (button == 0))
        return FALSE;
    strcpy(filename_start(path), fname);

    /* allocate temporary buffer */
    buf = dos_alloc_anyram(SIZE_SHELBUF);
    if (!buf)
    {
        malloc_fail_alert();
        return FALSE;
    }

    /* load in file & validate contents */
    p = buf + CPDATA_LEN;
    rc = dos_load_file(path, SIZE_SHELBUF-CPDATA_LEN-1, p);
    if (rc > 0L)
    {
        if ((*p == '#')     /* must start with #R or #E */
         && ((*(p+1) == 'R') || (*(p+1) == 'E')))
            ;
        else rc = 0L;
        p[rc] = '\0';
    }

    if (rc > 0L)            /* still OK: merge contents into shell buffer */
    {
        shel_get(buf, CPDATA_LEN);
        shel_put(buf, CPDATA_LEN+rc+1); /* also save terminating nul */
        ret = TRUE;
    }
    else if (rc == EFILNF)  /* issue alert for file not found */
    {
        fun_alert_merge(1, STFILENF, fname);
    }
    else                    /* issue alert for invalid inf file */
    {
        fun_alert(1, STINVINF);
    }

    dos_free(buf);

    return ret;
}
#endif
