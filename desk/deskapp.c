/*       DESKAPP.C      06/11/84 - 07/11/85             Lee Lorenzen    */
/*      for 3.0         3/6/86   - 5/6/86               MDF             */
/*      for 2.3         9/25/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*       Copyright (C) 2002-2016 The EmuTOS development team
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

#include "config.h"
#include <string.h>

#include "portab.h"
#include "obdefs.h"
#include "rsdefs.h"
#include "intmath.h"
#include "gsxdefs.h"
#include "gemdos.h"
#include "optimize.h"

#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "deskbind.h"
#include "../bios/videl.h"
#include "../bios/amiga.h"
#include "aesbind.h"
#include "deskrsrc.h"
#include "deskfun.h"
#include "deskobj.h"
#include "deskglob.h"
#include "deskmain.h"
#include "deskdir.h"
#include "deskins.h"
#include "icons.h"
#include "desk1.h"
#include "xbiosbind.h"
#include "kprint.h"


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
#define INF_E2_IDTDATE  0x40    /* 1 => get date format from _IDT (ignore INF_E2_DAYMONTH bit) */
#define INF_E2_IDTTIME  0x20    /* 1 => get time format from _IDT (ignore INF_E2_24HCLOCK bit) */
#define INF_E2_ALLOWOVW 0x10    /* 1 => allow overwrites (yes, it's backwards) */
#define INF_E2_MNUCLICK 0x08    /* 1 => click to drop down menus */
#define INF_E2_DAYMONTH 0x04    /* 1 => day before month */
#define INF_E2_24HCLOCK 0x02    /* 1 => 24 hour clock */
#define INF_E2_SOUND    0x01    /* 1 => sound effects on */
#define INF_E2_DEFAULT  (INF_E2_IDTDATE|INF_E2_IDTTIME|INF_E2_SOUND)    /* default if no .INF */

                            /* 'E' bytes 3-4 are video mode (see process_inf1() in aes/geminit.c) */

                            /* 'E' byte 5 */
#define INF_E5_NOSORT   0x80    /* 1 => do not sort folder contents (overrides INF_E1_SORTMASK) */

                            /* application type entries (F/G/P/Y): first byte of 3-byte string */
#define INF_AT_APPDIR   0x01    /* 1 => set current dir to app's dir (else to top window dir) */
#define INF_AT_ISFULL   0x02    /* 1 => pass full path in args (else filename only) */

#define INF_REV_LEVEL   0x01    /* revision level when creating EMUDESK.INF */

static WORD     gl_stdrv;

static WORD     inf_rev_level;  /* revision level of current EMUDESK.INF */

static BYTE     gl_afile[SIZE_AFILE];
static BYTE     *gl_buffer;


/* When we can't get EMUDESK.INF via shel_get() or by reading from
 * the disk, we create one dynamically from three sources:
 *  desk_inf_data1 below, for the #R, #E and #W lines
 *  the drivemask, for #M lines
 *  desk_inf_data2 below, for most of the remaining lines
 * The #T line is added at the end.
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
    "#E 1A 61\r\n"                      /* INF_E1_DEFAULT and INF_E2_DEFAULT */
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
 *  Scans a string into a private buffer used for ANODE text (currently
 *  gl_buffer[]) and ups the string pointer for next time.
 *
 *  Leading spaces are ignored; the scan is terminated by @, '\r', or
 *  the null character.
 *
 *  Returns a pointer to the next character to scan: if the scan was
 *  terminated by @ or '\r', this is the following character; otherwise
 *  this is the terminating character (null).
 */
BYTE *scan_str(BYTE *pcurr, BYTE **ppstr)
{
    BYTE *dest = G.g_pbuff;
    BYTE *end = gl_buffer + SIZE_BUFF - 1;

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
    G.g_pbuff = dest;           /* update buffer ptr for next time */

    return pcurr;               /* next input character */
}


/*
 *  Parse a single line from the EMUDESK.INF file
 */
static BYTE *app_parse(BYTE *pcurr, ANODE *pa)
{
    WORD temp;

    pa->a_flags = 0;

    switch(*pcurr)
    {
    case 'T':                             /* Trash */
        pa->a_type  = AT_ISTRSH;
        pa->a_flags = AF_ISCRYS | AF_ISDESK;
        break;
    case 'M':                             /* Storage Media        */
        pa->a_type = AT_ISDISK;
        pa->a_flags = AF_ISCRYS | AF_ISDESK;
        break;
    case 'Y':                             /* GEM App needs parms  */
        pa->a_flags = AF_ISPARM;
        /* drop thru */
    case 'G':                             /* GEM App no parms     */
        pa->a_type = AT_ISFILE;
        pa->a_flags |= AF_ISCRYS;
        break;
    case 'P':                             /* TOS App needs parms  */
        pa->a_flags = AF_ISPARM;
        /* drop thru */
    case 'F':                             /* TOS App no parms     */
        pa->a_type = AT_ISFILE;
        break;
    case 'D':                             /* Directory (Folder)   */
        pa->a_type = AT_ISFOLD;
        break;
    case 'I':                             /* Executable file      */
        pa->a_flags = AF_ISEXEC;
        /* drop thru */
    case 'N':                             /* Non-executable file  */
        pa->a_type = AT_ISFILE;
        pa->a_flags |= AF_WINDOW;
        break;
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

    return pcurr;
}


void app_tran(WORD bi_num)
{
    BITBLK *pbi;
    BITBLK lb;

    rsrc_gaddr_rom(R_BITBLK, bi_num, (void **)&pbi);

    lb = *pbi;

    gsx_trans(lb.bi_pdata, lb.bi_wb, lb.bi_pdata, lb.bi_wb, lb.bi_hl);
}


/*
 * set up ICONBLK stuff - all the hard work is done here
 *
 * returns -1 iff insufficient memory
 */
static WORD setup_iconblks(const ICONBLK *ibstart, WORD count)
{
    BYTE *maskstart, *datastart;
    void *allocmem;
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
     *  pointers to untranslated masks
     *  icon masks
     *  icon data
     */
    allocmem = dos_alloc_stram(count*(sizeof(ICONBLK)+sizeof(UWORD *)+2*num_bytes));
    if (!allocmem)
    {
        KDEBUG(("insufficient memory for %d desktop icons\n",count));
        return -1;
    }

    G.g_iblist = allocmem;
    allocmem += count * sizeof(ICONBLK);
    G.g_origmask = allocmem;
    allocmem += count * sizeof(UWORD *);
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
     * Then we initialise g_origmask[]:
     *  g_origmask[i] points to the untransformed mask & is
     *  referenced by act_chkobj() in deskact.c
     */
    for (i = 0; i < count; i++)
        G.g_origmask[i] = (UWORD *)ibstart[i].ib_pmask;

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
        G.g_iblist[i].ib_char &= 0xff00;    /* strip any existing char */
        G.g_iblist[i].ib_ytext = ih;
        G.g_iblist[i].ib_wtext = 12 * gl_wschar;
        G.g_iblist[i].ib_htext = gl_hschar + 2;
    }

    /*
     * Finally we do the transforms
     */
    for (i = 0, p = maskstart; i < count; i++, p += num_bytes)
        gsx_trans((LONG)p, iwb, (LONG)p, iwb, ih);
    for (i = 0, p = datastart; i < count; i++, p += num_bytes)
        gsx_trans((LONG)p, iwb, (LONG)p, iwb, ih);

    return 0;
}

/*
 * try to load icons from user-supplied resource
 */
static WORD load_user_icons(void)
{
    RSHDR *hdr;
    ICONBLK *ibptr;
    BYTE *origmask;     /* points to original masks of loaded ICONBLKs */
    char *p;
    WORD i, n, rc, w, h, masksize;
    BYTE icon_rsc_name[sizeof(ICON_RSC_NAME)];

    /*
     * determine the number of icons in the user's icon resource
     */
    strcpy(icon_rsc_name, ICON_RSC_NAME);
    icon_rsc_name[0] += gl_stdrv;   /* Adjust drive letter  */
    if (!rsrc_load(icon_rsc_name))
    {
        KDEBUG(("can't load user desktop icons from %s\n",icon_rsc_name));
        return -1;
    }

    hdr = (RSHDR *)(AP_1RESV);
    if (hdr->rsh_nib < BUILTIN_IBLKS)   /* must have at least the minimum set */
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
    w = ibptr->ib_wicon;
    h = ibptr->ib_hicon;
    if ((w != icon_rs_iconblk[0].ib_wicon) || (h != icon_rs_iconblk[0].ib_hicon))
    {
        KDEBUG(("wrong size user desktop icons (%dx%d)\n",w,h));
        rsrc_free();
        return -1;
    }

    /*
     * copy the original icon masks for the loaded icons &
     * update the ptrs in the ICONBLKs to point to the copies
     */
    masksize = w * h / 8;
    origmask = dos_alloc_stram((LONG)n*masksize);
    if (!origmask)
    {
        KDEBUG(("insufficient memory for icon masks for %d user desktop icons\n",n));
        rsrc_free();
        return -1;
    }

    for (i = 0, p = origmask; i < n; i++, p += masksize)
    {
        memcpy(p, ibptr[i].ib_pmask, masksize);
        ibptr[i].ib_pmask = (WORD *)p;
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
    G.g_pbuff = gl_buffer = dos_alloc_stram(SIZE_BUFF);
    if (!gl_buffer)
        return -1;

    /*
     * allocate space for ANODEs, chain the ANODEs together,
     * and set up the pointers
     */
    G.g_alist = dos_alloc_stram(NUM_ANODES*sizeof(ANODE));
    if (!G.g_alist)
        return -1;

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
 *  Initialize the application list by reading in the EMUDESK.INF
 *  file, either from memory or from the disk if the shel_get
 *  indicates no message is there.
 */
void app_start(void)
{
    WORD i, x, y;
    ANODE *pa;
    WSAVE *pws;
    BYTE *pcurr, *ptmp, *pauto = NULL;
    WORD envr, xcnt, ycnt, xcent, wincnt, dummy;

    /* remember start drive */
    gl_stdrv = dos_gdrv();

    /* initialise the ANODE stuff */
    /* FIXME: handle out-of-memory */
    initialise_anodes();

    /*
     * the following may in theory fail due to lack of memory: what should we do?
     */
    app_rdicon();

    G.g_wicon = (12 * gl_wschar) + (2 * G.g_iblist[0].ib_xtext);
    G.g_hicon = G.g_iblist[0].ib_hicon + gl_hschar + 2;

    xcnt = G.g_wdesk / (G.g_wicon+MIN_WINT);/* icon count */
    G.g_icw = G.g_wdesk / xcnt;             /* width */

    ycnt = G.g_hdesk / (G.g_hicon+MIN_HINT);/* icon count */
    G.g_ich = G.g_hdesk / ycnt;             /* height */

    shel_get(gl_afile, SIZE_AFILE);
    if (gl_afile[0] != '#')                 /* invalid signature    */
    {                                       /*   so read from disk  */
        LONG ret;
        WORD fh;
        char inf_file_name[16];
        strcpy(inf_file_name, INF_FILE_NAME);
        inf_file_name[0] += gl_stdrv;         /* Adjust drive letter  */
        ret = dos_open(inf_file_name, 0x0);
        if (ret >= 0L)
        {
            fh = (WORD) ret;
            ret = dos_read(fh, SIZE_AFILE, gl_afile);
            G.g_afsize = (ret < 0L) ? 0L : ret;
            dos_close(fh);
            gl_afile[G.g_afsize] = '\0';
        }
    }

    /* If there's still no desktop.inf data, use built-in now: */
    if (gl_afile[0] != '#')
    {
        LONG drivemask;
        char *text;
        int icon_index = 0;
        int drive_x = 0, drive_y = 0;
        int trash_x, trash_y;
        int icon_type;
        char drive_letter;

        /* Environment and Windows */
        strcat(gl_afile, desk_inf_data1);

        /* Scan for valid drives: */
        drivemask = dos_sdrv(dos_gdrv());
        for (i = 0; i < BLKDEVNUM; i++)
        {
            if (drivemask&(1L<<i))
            {
                x = strlen(gl_afile);
                drive_x = icon_index % xcnt; /* x position */
                drive_y = icon_index / xcnt; /* y position */
                icon_type = (i > 1) ? 0 /* Hard disk */ : 1 /* Floppy */;
                drive_letter = 'A' + i;
                rsrc_gaddr_rom(R_STRING, STDISK, (void **)&text);
                sprintf(gl_afile + x, "#M %02X %02X %02X FF %c %s %c@ @\r\n",
                        drive_x, drive_y, icon_type, drive_letter, text, drive_letter);
                icon_index++;
            }
        }

        /* Copy core data part 2 */
        strcat(gl_afile, desk_inf_data2);

        /* add Trash icon to end */
        x = strlen(gl_afile);
        trash_x = 0;            /* Left */
        trash_y = ycnt-1;       /* Bottom */
        if (drive_y >= trash_y) /* if the last drive icon overflows over */
            trash_x = xcnt-1;   /*  the trash row, force trash to right  */
        rsrc_gaddr_rom(R_STRING, STTRASH, (void **)&text);
        sprintf(gl_afile + x, "#T %02X %02X 03 FF   %s@ @\r\n",
                trash_x, trash_y, text);
        G.g_afsize = strlen(gl_afile);
    }

    wincnt = 0;
    inf_rev_level = 0;
    pcurr = gl_afile;

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
                pws = &G.g_cnxsave.cs_wnode[wincnt];
                pcurr = scan_2(pcurr, &dummy);
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
                pcurr = scan_2(pcurr, &pws->obid_save);
                ptmp = pws->pth_save;
                pcurr++;
                while(*pcurr != '@')
                    *ptmp++ = *pcurr++;
                *ptmp = '\0';
                wincnt += 1;
            }
            break;
        case 'E':                       /* Environment */
            pcurr++;
            pcurr = scan_2(pcurr, &envr);
            G.g_cnxsave.cs_view = ( (envr & INF_E1_VIEWTEXT) != 0);
            G.g_cnxsave.cs_sort = ( (envr & INF_E1_SORTMASK) >> 5);
            G.g_cnxsave.cs_confdel = ( (envr & INF_E1_CONFDEL) != 0);
            G.g_cnxsave.cs_confcpy = ( (envr & INF_E1_CONFCPY) != 0);
            G.g_cnxsave.cs_dblclick = envr & INF_E1_DCMASK;

            pcurr = scan_2(pcurr, &envr);
            G.g_cnxsave.cs_confovwr = ( (envr & INF_E2_ALLOWOVW) == 0);
            G.g_cnxsave.cs_mnuclick = ( (envr & INF_E2_MNUCLICK) != 0);
            menu_click(G.g_cnxsave.cs_mnuclick, 1); /* tell system */
            if (envr & INF_E2_IDTDATE)
                G.g_cnxsave.cs_datefmt = DATEFORM_IDT;
            else
                G.g_cnxsave.cs_datefmt = (envr & INF_E2_DAYMONTH) ? DATEFORM_DMY : DATEFORM_MDY;
            if (envr & INF_E2_IDTTIME)
                G.g_cnxsave.cs_timefmt = TIMEFORM_IDT;
            else
                G.g_cnxsave.cs_timefmt = (envr & INF_E2_24HCLOCK) ? TIMEFORM_24H : TIMEFORM_12H;
            sound(FALSE, !(envr & INF_E2_SOUND), 0);

            pcurr = scan_2(pcurr, &dummy);  /* skip video stuff */
            pcurr = scan_2(pcurr, &dummy);

            pcurr = scan_2(pcurr, &envr);
            if (envr & INF_E5_NOSORT)
                G.g_cnxsave.cs_sort = CS_NOSORT;
            break;
        }
    }

    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if (pa->a_flags & AF_ISDESK)
        {
            x = pa->a_xspot * G.g_icw;
            y = pa->a_yspot * G.g_ich + G.g_ydesk;
            snap_disk(x, y, &pa->a_xspot, &pa->a_yspot, 0, 0);
        }
    }

    /* set up outlines for dragging files displayed as icons */
    G.g_nmicon = 9;     /* number of points */
    memset(G.g_xyicon, 0, sizeof(G.g_xyicon));
    xcent = (G.g_wicon - G.g_iblist[0].ib_wicon) / 2;
    G.g_xyicon[0] = xcent;
    G.g_xyicon[2] = xcent;
    G.g_xyicon[3] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[5] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[7] = G.g_hicon;
    G.g_xyicon[8] = G.g_wicon;
    G.g_xyicon[9] = G.g_hicon;
    G.g_xyicon[10] = G.g_wicon;
    G.g_xyicon[11] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[12] = G.g_wicon-xcent;
    G.g_xyicon[13] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[14] = G.g_wicon-xcent;
    G.g_xyicon[16] = xcent;

    /* set up outlines for dragging files displayed as text */
    G.g_nmtext = 5;     /* number of points */
    memset(G.g_xytext, 0, sizeof(G.g_xytext));
    G.g_xytext[2] = gl_wchar * DRAG_BOX_WIDTH;
    G.g_xytext[4] = gl_wchar * DRAG_BOX_WIDTH;
    G.g_xytext[5] = gl_hchar;
    G.g_xytext[7] = gl_hchar;
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
static void save_to_disk(void)
{
    LONG ret, len;
    WNODE *w;
    WORD fh = -1;
    BYTE inf_file_name[sizeof(INF_FILE_NAME)];

    /* make sure user really wants to save the desktop */
    if (fun_alert(1, STSVINF) != 1)
        return;

    strcpy(inf_file_name, INF_FILE_NAME);
    inf_file_name[0] += gl_stdrv;   /* Adjust drive letter  */

    ret = dos_create(inf_file_name, 0);
    if (ret >= 0L)
    {
        fh = (WORD) ret;
        len = G.g_afsize - 1;
        ret = dos_write(fh, len, gl_afile);
        dos_close(fh);
        if (ret != len)             /* write error */
            ret = -1L;
    }
    if (ret < 0L)                   /* open error or write error */
        fun_alert(1, STNOINF);

    /*
     * now update any open windows for the directory containing
     * the saved file (as long as the file was created ok).
     */
    if (fh >= 0)                    /* file was created */
    {
        del_fname(inf_file_name);       /* convert to pathname ending in *.* */
        w = fold_wind(inf_file_name);   /* scan for matching windows */
        if (w)                          /* got one:                          */
            fun_rebld(w);               /* rebuild all matching open windows */
    }
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
    WORD i;
    WORD env1, env2, mode, env5;
    BYTE type;
    BYTE *pcurr, *ptmp;
    ANODE *pa;
    WSAVE *pws;

    memset(gl_afile, 0, SIZE_AFILE);
    pcurr = gl_afile;

    /* save revision level */
    pcurr += sprintf(pcurr,"#R %02X\r\n",INF_REV_LEVEL);

    /* save autorun (if any) */
    for (pa = G.g_ahead; pa; pa = pa->a_next)
        if (pa->a_flags & AF_AUTORUN)
            pcurr += sprintf(pcurr,"#Z %02X %s@\r\n",pa->a_flags&AF_ISCRYS,pa->a_pappl);

    /* save environment */
    env1 = (G.g_cnxsave.cs_view) ? INF_E1_VIEWTEXT : 0x00;
    env1 |= ((G.g_cnxsave.cs_sort) << 5) & INF_E1_SORTMASK;
    env1 |= (G.g_cnxsave.cs_confdel) ? INF_E1_CONFDEL : 0x00;
    env1 |= (G.g_cnxsave.cs_confcpy) ? INF_E1_CONFCPY : 0x00;
    env1 |= G.g_cnxsave.cs_dblclick & INF_E1_DCMASK;
    env2 = (G.g_cnxsave.cs_confovwr) ? 0x00 : INF_E2_ALLOWOVW;
    env2 |= (G.g_cnxsave.cs_mnuclick) ? INF_E2_MNUCLICK : 0x00;
    switch(G.g_cnxsave.cs_datefmt)
    {
    case DATEFORM_IDT:
        env2 |= INF_E2_IDTDATE;
        break;
    case DATEFORM_DMY:
        env2 |= INF_E2_DAYMONTH;
        break;
    }
    switch(G.g_cnxsave.cs_timefmt)
    {
    case TIMEFORM_IDT:
        env2 |= INF_E2_IDTTIME;
        break;
    case TIMEFORM_24H:
        env2 |= INF_E2_24HCLOCK;
        break;
    }
    env2 |= sound(FALSE, 0xFFFF, 0)  ? 0x00 : INF_E2_SOUND;
    mode = desk_get_videomode();
    env5 = (G.g_cnxsave.cs_sort == CS_NOSORT) ? INF_E5_NOSORT : 0;
    pcurr += sprintf(pcurr,"#E %02X %02X %02X %02X %02X\r\n",
                    env1,env2,(mode>>8)&0x00ff,mode&0x00ff,env5);

    /* save windows */
    for (i = 0; i < NUM_WNODES; i++)
    {
        pws = &G.g_cnxsave.cs_wnode[i];
        ptmp = pws->pth_save;
        pcurr += sprintf(pcurr,"#W %02X %02X %02X %02X %02X %02X %02X",
                        0,pws->vsl_save,pws->x_save/gl_wchar,
                        pws->y_save/gl_hchar,pws->w_save/gl_wchar,
                        pws->h_save/gl_hchar,pws->obid_save);
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
            if (pa->a_flags & AF_WINDOW)
            {
                type = (pa->a_flags & AF_ISEXEC) ? 'I' : 'N';
                break;
            }
            if (pa->a_flags & AF_ISCRYS)
                type = (pa->a_flags & AF_ISPARM) ? 'Y' : 'G';
            else
                type = (pa->a_flags & AF_ISPARM) ? 'P' : 'F';
            break;
        case AT_ISFOLD:
            type = 'D';
            break;
        case AT_ISTRSH:     /* Trash */
            type = 'T';
            break;
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
        if (pa->a_type == AT_ISFILE)
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
    }
    *pcurr++ = 0x0;

    /* reverse list back */
    app_revit();

    /* calculate size */
    G.g_afsize = pcurr - gl_afile;

    /* save in memory */
    shel_put(gl_afile, G.g_afsize);

    /* save to disk */
    if (todisk)
        save_to_disk();
}


/*
 *  Build the desktop list of objects based on this current application list
 */
void app_blddesk(void)
{
    WORD obid;
    ANODE *pa;
    OBJECT *pob;
    SCREENINFO *si;
    ICONBLK *pic;
    LONG *ptr;

    /* free all this window's kids and set size  */
    obj_wfree(DROOT, 0, 0, gl_width, gl_height);
    ptr = (LONG *)&global[3];
    G.g_screen[DROOT].ob_spec = *ptr;

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

            /* build object */
            pob = &G.g_screen[obid];
            pob->ob_state = NORMAL;
            pob->ob_flags = NONE;
            pob->ob_type = G_ICON;
            si = &G.g_screeninfo[obid];
            si->icon.index = pa->a_aicon;
            pic = &si->icon.block;
            pob->ob_spec = (LONG)pic;
            memcpy(pic, &G.g_iblist[pa->a_aicon], sizeof(ICONBLK));
            pic->ib_xicon = ((G.g_wicon - pic->ib_wicon) / 2);
            pic->ib_ptext = pa->a_pappl;
            pic->ib_char |= pa->a_letter;
        }
    }
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
 *  Returns match type in *pisapp:
 *      TRUE if name matches application name
 *      FALSE if name matches data name
 */
ANODE *app_afind_by_name(WORD atype, BYTE *pspec, BYTE *pname, WORD *pisapp)
{
    ANODE *pa;
    WORD match;
    BYTE pathname[MAXPATHLEN];

    strcpy(pathname,pspec);                 /* build full pathname */
    strcpy(filename_start(pathname),pname);

    for (pa = G.g_ahead; pa; pa = pa->a_next)
    {
        if ((pa->a_type == atype) && !(pa->a_flags & AF_ISDESK))
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
