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
#include "intmath.h"
#include "gsxdefs.h"
#include "gemdos.h"

#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "deskbind.h"
#include "../bios/videl.h"
#include "../aes/optimize.h"
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

GLOBAL WORD     gl_numics;
GLOBAL WORD     gl_stdrv;

static char     *maskstart;
static char     *datastart;

static BYTE     gl_afile[SIZE_AFILE];
static BYTE     gl_buffer[SIZE_BUFF];


/* When we can't get EMUDESK.INF via shel_get() or by reading from
 * the disk, we create one dynamically from three sources:
 *  desk_inf_data1 below, for the #E and #W lines
 *  the drivemask, for #M lines
 *  desk_inf_data2 below, for the remaining lines
 */
static const char *desk_inf_data1 =
    "#E 1A 61\r\n"                      /* INF_E1_DEFAULT and INF_E2_DEFAULT */
    "#W 00 00 02 06 26 0C 00 @\r\n"
    "#W 00 00 02 08 26 0C 00 @\r\n"
    "#W 00 00 02 0A 26 0C 00 @\r\n"
    "#W 00 00 02 0D 26 0C 00 @\r\n";
static const char *desk_inf_data2 =
    "#F FF 28 @ *.*@\r\n"
    "#D FF 02 @ *.*@\r\n"
    "#Y 08 FF *.GTP@ @\r\n"
    "#G 08 FF *.APP@ @\r\n"
    "#G 08 FF *.PRG@ @\r\n"
    "#P 08 FF *.TTP@ @\r\n"
    "#F 08 FF *.TOS@ @\r\n";


/*
 *  Allocate an application object
 *
 *  Issues an alert if there are none available
 */
ANODE *app_alloc(WORD tohead)
{
    ANODE *pa, *ptmpa;

    pa = G.g_aavail;
    if (pa)
    {
        G.g_aavail = pa->a_next;
        if (tohead || !G.g_ahead)
        {
            pa->a_next = G.g_ahead;
            G.g_ahead = pa;
        }
        else
        {
            ptmpa = G.g_ahead;
            while(ptmpa->a_next)
                ptmpa = ptmpa->a_next;
            ptmpa->a_next = pa;
            pa->a_next = (ANODE *) NULL;
        }
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


/* We're using scan_2 from the AES sources: */
extern BYTE *scan_2(BYTE *pcurr, WORD *pwd);


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
    while(*pcurr == ' ')
        pcurr++;
    *ppstr = G.g_pbuff;
    while(*pcurr && (*pcurr != '\r') && (*pcurr != '@'))
        *G.g_pbuff++ = *pcurr++;
    *G.g_pbuff++ = '\0';
    if (*pcurr)
        pcurr++;

    return pcurr;
}


/*
 *  Parse a single line from the EMUDESK.INF file
 */
static BYTE *app_parse(BYTE *pcurr, ANODE *pa)
{
    WORD temp;

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
    case 'G':                             /* GEM App File         */
        pa->a_type = AT_ISFILE;
        pa->a_flags = AF_ISCRYS;
        break;
    case 'Y':                             /* GEM App needs parms  */
        pa->a_type = AT_ISFILE;
        pa->a_flags = AF_ISCRYS | AF_ISPARM;
        break;
    case 'F':                             /* DOS File no parms    */
        pa->a_type = AT_ISFILE;
        pa->a_flags = NONE;
        break;
    case 'P':                             /* DOS App needs parms  */
        pa->a_type = AT_ISFILE;
        pa->a_flags = AF_ISPARM;
        break;
    case 'D':                             /* Directory (Folder)   */
        pa->a_type = AT_ISFOLD;
        break;
    }
    pcurr++;

    if (pa->a_flags & AF_ISDESK)
    {
        pcurr = scan_2(pcurr, &pa->a_xspot);
        pcurr = scan_2(pcurr, &pa->a_yspot);
    }

    pcurr = scan_2(pcurr, &pa->a_aicon);
    if (pa->a_aicon >= NUM_IBLKS)
        pa->a_aicon = IA_GENERIC_ALT;
    pcurr = scan_2(pcurr, &pa->a_dicon);
    if (pa->a_dicon >= NUM_IBLKS)
        pa->a_dicon = ID_GENERIC_ALT;
    pcurr++;

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
    LONG lpbi;
    BITBLK lb;

    rsrc_gaddr(R_BITBLK, bi_num, &lpbi);

    memcpy(&lb, (BITBLK *)lpbi, sizeof(BITBLK));

    gsx_trans(lb.bi_pdata, lb.bi_wb, lb.bi_pdata, lb.bi_wb, lb.bi_hl);
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
   in icons.c, so that they can be generated by a resource editor - RFB */
static void app_rdicon(void)
{
    WORD mask[NUM_IBLKS], data[NUM_IBLKS];
    char copied[NUM_IBLKS];
    char *p;
    WORD *addr;
    LONG temp;
    WORD i, j, n, iwb, ih;
    WORD num_mask, num_data, num_wds, num_bytes;

    /*
     * First, we copy the ICONBLKs to the g_iblist[] array:
     *  g_iblist[] points to the transformed data/transformed mask
     *  & is referenced by act_chkobj() in deskact.c, insa_icon()
     *  in deskins.c, and win_bldview() in deskwin.c
     */
    memcpy(G.g_iblist, icon_rs_iconblk, NUM_IBLKS*sizeof(ICONBLK));

    /*
     * Then we initialise g_origmask[]:
     *  g_origmask[i] points to the untransformed mask & is
     *  referenced by act_chkobj() in deskact.c
     */
    for (i = 0; i < NUM_IBLKS; i++)
        G.g_origmask[i] = (UWORD *)icon_rs_iconblk[i].ib_pmask;

    /*
     * Determine the number of mask and data icons actually used
     * (different ICONBLKs can point to the same mask and/or data)
     * and set mask[n] = mask# for iconblk n
     *         data[n] = data# for iconblk n
     */
    for (i = 0; i < NUM_IBLKS; i++)
        mask[i] = data[i] = -1;
    for (i = 0, num_mask = num_data = 0; i < NUM_IBLKS; i++)
    {
        if (mask[i] < 0)
        {
            mask[i] = num_mask;
            addr = icon_rs_iconblk[i].ib_pmask;
            for (j = i+1; j < NUM_IBLKS; j++)
                if (icon_rs_iconblk[j].ib_pmask == addr)
                    mask[j] = num_mask;
            num_mask++;
        }
        if (data[i] < 0)
        {
            data[i] = num_data;
            addr = icon_rs_iconblk[i].ib_pdata;
            for (j = i+1; j < NUM_IBLKS; j++)
                if (icon_rs_iconblk[j].ib_pdata == addr)
                    data[j] = num_data;
            num_data++;
        }
    }

    /*
     * Calculate the size of each icon in words & bytes.  We
     * assume that all icons are the same w,h as the first
     */
    num_wds = (icon_rs_iconblk[0].ib_wicon * icon_rs_iconblk[0].ib_hicon) / 16;
    num_bytes = num_wds * 2;

    /*
     * Allocate memory for the mask/data icons, and copy them
     * FIXME: we should check that memory is allocated successfully
     */
    maskstart = dos_alloc(num_mask*num_bytes);
    memset(copied, 0x00, NUM_IBLKS);
    for (i = 0, p = maskstart; i < NUM_IBLKS; i++)
    {
        n = mask[i];
        if (!copied[n])   /* only copy mask once */
        {
            memcpy(p+n*num_bytes, (char *)icon_rs_iconblk[i].ib_pmask, num_bytes);
            copied[n] = 1;
        }
    }

    datastart = dos_alloc(num_data*num_bytes);
    memset(copied, 0x00, NUM_IBLKS);
    for (i = 0, p = datastart; i < NUM_IBLKS; i++)
    {
        n = data[i];
        if (!copied[n])   /* only copy data once */
        {
            memcpy(p+n*num_bytes, icon_rs_iconblk[i].ib_pdata, num_bytes);
            copied[n] = 1;
        }
    }

    /* the number of entries in the icon_rs_fstr[] array in icons.c */
#if HAVE_APPL_IBLKS
    gl_numics = LASTICON + 1;
#else
    gl_numics = 0;
#endif

    /*
     * Fix up the ICONBLKs
     */
    for (i = 0; i < NUM_IBLKS; i++)
    {
        temp = mask[i] * num_bytes;
        G.g_iblist[i].ib_pmask = (WORD *)(maskstart + temp);
        temp = data[i] * num_bytes;
        G.g_iblist[i].ib_pdata = (WORD *)(datastart + temp);
        G.g_iblist[i].ib_ytext = icon_rs_iconblk[0].ib_hicon;
        G.g_iblist[i].ib_wtext = 12 * gl_wschar;
        G.g_iblist[i].ib_htext = gl_hschar + 2;
    }

    /*
     * Finally we do the transforms
     */
    iwb = icon_rs_iconblk[0].ib_wicon / 8;
    ih = icon_rs_iconblk[0].ib_hicon;

    for (i = 0, p = maskstart; i < num_mask; i++, p += num_bytes)
        gsx_trans((LONG)p, iwb, (LONG)p, iwb, ih);
    for (i = 0, p = datastart; i < num_data; i++, p += num_bytes)
        gsx_trans((LONG)p, iwb, (LONG)p, iwb, ih);
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

    G.g_pbuff = gl_buffer;

    for (i = NUM_ANODES - 2; i >= 0; i--)
        G.g_alist[i].a_next = &G.g_alist[i + 1];
    G.g_ahead = (ANODE *) NULL;
    G.g_aavail = G.g_alist;
    G.g_alist[NUM_ANODES - 1].a_next = (ANODE *) NULL;

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
                rsrc_gaddr(R_STRING, STDISK, (LONG *)&text);
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
        rsrc_gaddr(R_STRING, STTRASH, (LONG *)&text);
        sprintf(gl_afile + x, "#T %02X %02X 03 FF   %s@ @\r\n",
                trash_x, trash_y, text);
        G.g_afsize = strlen(gl_afile);
    }

    wincnt = 0;
    pcurr = gl_afile;

    while(*pcurr)
    {
        if (*pcurr++ != '#')            /* look for start of line */
            continue;

        switch(*pcurr)
        {
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
            pa = app_alloc(TRUE);
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
            snap_disk(x, y, &pa->a_xspot, &pa->a_yspot);
        }
    }

    xcent = (G.g_wicon - G.g_iblist[0].ib_wicon) / 2;
    G.g_nmicon = 9;
    G.g_xyicon[0] = xcent;
    G.g_xyicon[1] = 0;
    G.g_xyicon[2] = xcent;
    G.g_xyicon[3] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[4] = 0;
    G.g_xyicon[5] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[6] = 0;
    G.g_xyicon[7] = G.g_hicon;
    G.g_xyicon[8] = G.g_wicon;
    G.g_xyicon[9] = G.g_hicon;
    G.g_xyicon[10] = G.g_wicon;
    G.g_xyicon[11] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[12] = G.g_wicon-xcent;
    G.g_xyicon[13] = G.g_hicon-gl_hschar-2;
    G.g_xyicon[14] = G.g_wicon-xcent;
    G.g_xyicon[15] = 0;
    G.g_xyicon[16] = xcent;
    G.g_xyicon[17] = 0;

    G.g_nmtext = 5;
    G.g_xytext[0] = 0;
    G.g_xytext[1] = 0;
    G.g_xytext[2] = gl_wchar * 12;
    G.g_xytext[3] = 0;
    G.g_xytext[4] = gl_wchar * 12;
    G.g_xytext[5] = gl_hchar;
    G.g_xytext[6] = 0;
    G.g_xytext[7] = gl_hchar;
    G.g_xytext[8] = 0;
    G.g_xytext[9] = 0;
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

    /* save autorun (if any) as first line */
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
#if CONF_WITH_VIDEL
    mode = get_videl_mode();
    if (!mode)                      /* i.e. not videl */
#endif
        mode = 0xff00 | Getrez();
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
     * in, using app_alloc(TRUE) to allocate the ANODEs, the
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
    *pcurr++ = 0x1a;
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
