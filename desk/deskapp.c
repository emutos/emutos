/*       DESKAPP.C      06/11/84 - 07/11/85             Lee Lorenzen    */
/*      for 3.0         3/6/86   - 5/6/86               MDF             */
/*      for 2.3         9/25/87                         mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*       Copyright (c) 2002-2015 The EmuTOS development team
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

#include "config.h"
#include <string.h>

#include "portab.h"
#include "obdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "gembind.h"
#include "deskbind.h"
#include "screen.h"
#include "videl.h"
#include "gemsuper.h"
#include "gemdos.h"
#include "rectfunc.h"
#include "optimize.h"
#include "aesbind.h"
#include "deskrsrc.h"
#include "deskfun.h"
#include "deskobj.h"
#include "deskgraf.h"
#include "deskglob.h"
#include "deskmain.h"
#include "deskdir.h"
#include "icons.h"
#include "desk1.h"
#include "xbiosbind.h"
#include "country.h"


GLOBAL WORD     gl_numics;
GLOBAL WORD     gl_stdrv;

static char     *maskstart;
static char     *datastart;

static BYTE     gl_afile[SIZE_AFILE];
static BYTE     gl_buffer[SIZE_BUFF];


/* When we can't get EMUDESK.INF via shel_get() or by reading from
 * the disk, we create one dynamically from three sources:
 *  the desktop preferences, for #E line
 *  desk_inf_data1 below, for #W lines
 *  the drivemask, for #M lines
 *  desk_inf_data2 below, for the remaining lines
 */
static const char *desk_inf_data1 =
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

/************************************************************************/
/* g e t _ d e f d r v                                                  */
/************************************************************************/
static BYTE get_defdrv(UWORD dr_exist, UWORD dr_hard)
{
/* this routine returns the drive letter of the lowest drive: lowest    */
/* lettered hard disk if possible, otherwise lowest lettered floppy     */
/* (which is usually A)                                                 */
/* in dr_exist, MSbit = A                                               */
        UWORD           mask, hd_disk;
        WORD            ii;
        BYTE            drvletr;

        drvletr = 'A';                          /* assume A is always   */
                                                /* lowest floppy        */

        mask = 0x8000;
        hd_disk = dr_exist & dr_hard;

        if (hd_disk)
        {               /* there's a hard disk out there somewhere      */
          for (ii = 0; ii <= 15; ii++)
          {
            if (mask & hd_disk)
            {
              drvletr = ii + 'A';
              break;
            } /* if */
            mask >>= 1;
          } /* for */
        } /* if hd_disk */

        return(drvletr);
} /* get_defdrv */




/*
*       Allocate an application object.
*/
ANODE *app_alloc(WORD tohead)
{
        ANODE           *pa, *ptmpa;

        pa = G.g_aavail;
        if (pa)
        {
          G.g_aavail = pa->a_next;
          if ( (tohead) ||
               (!G.g_ahead) )
          {
            pa->a_next = G.g_ahead;
            G.g_ahead = pa;
          }
          else
          {
            ptmpa = G.g_ahead;
            while( ptmpa->a_next )
              ptmpa = ptmpa->a_next;
            ptmpa->a_next = pa;
            pa->a_next = (ANODE *) NULL;
          }
        }
        return(pa);
}


/*
*       Free an application object.
*/
void app_free(ANODE *pa)
{
        ANODE           *ptmpa;

        if (G.g_ahead == pa)
          G.g_ahead = pa->a_next;
        else
        {
          ptmpa = G.g_ahead;
          while ( (ptmpa) &&
                  (ptmpa->a_next != pa) )
            ptmpa = ptmpa->a_next;
          if (ptmpa)
            ptmpa->a_next = pa->a_next;
        }
        pa->a_next = G.g_aavail;
        G.g_aavail = pa;
}


/*
*       Convert a single hex ASCII digit to a number
*/
#ifdef NO_ROM
WORD hex_dig(BYTE achar)
{
        if ( (achar >= '0') &&
             (achar <= '9') )
          return(achar - '0');
        if ( (achar >= 'A') &&
             (achar <= 'F') )
          return(achar - 'A' + 10);
        return(0);
}
#endif

/*
*       Scan off and convert the next two hex digits and return with
*       pcurr pointing one space past the end of the four hex digits
*/
#ifdef NO_ROM
BYTE *scan_2(BYTE *pcurr, WORD *pwd)
{
        UWORD   temp;

        if( *pcurr==' ' )
          pcurr += 1;

        temp = 0x0;
        temp |= hex_dig(*pcurr++) << 4;
        temp |= hex_dig(*pcurr++);
        if (temp == 0x00ff)
          temp = NIL;
        *pwd = temp;

        return( pcurr );
}
#else
/* We're using scan_2 from the AES sources: */
extern BYTE *scan_2(BYTE *pcurr, WORD *pwd);
#endif


/*
*       Scan off spaces until a string is encountered.  An @ denotes
*       a null string.  Copy the string into a string buffer until
*       a @ is encountered.  This denotes the end of the string.  Advance
*       pcurr past the last byte of the string.
*/
BYTE *scan_str(BYTE *pcurr, BYTE **ppstr)
{
        while(*pcurr == ' ')
          pcurr++;
        *ppstr = G.g_pbuff;
        while(*pcurr != '@')
          *G.g_pbuff++ = *pcurr++;
        *G.g_pbuff++ = NULL;
        pcurr++;
        return(pcurr);
}


/*
*       Parse a single line from the DESKTOP.INF file.
*/
static BYTE *app_parse(BYTE *pcurr, ANODE *pa)
{
        switch(*pcurr)
        {
          case 'T':                             /* Trash */
                pa->a_type  = AT_ISTRSH;
                pa->a_flags = AF_ISCRYS | AF_ISGRAF | AF_ISDESK;
                break;
          case 'M':                             /* Storage Media        */
                pa->a_type = AT_ISDISK;
                pa->a_flags = AF_ISCRYS | AF_ISGRAF | AF_ISDESK;
                break;
          case 'G':                             /* GEM App File         */
                pa->a_type = AT_ISFILE;
                pa->a_flags = AF_ISCRYS | AF_ISGRAF;
                break;
          case 'Y':                             /* GEM App needs parms  */
                pa->a_type = AT_ISFILE;
                pa->a_flags = AF_ISCRYS | AF_ISGRAF | AF_ISPARM;
                break;
          case 'F':                             /* DOS File no parms    */
          case 'f':                             /* (backward compatibility)  */
                pa->a_type = AT_ISFILE;
                pa->a_flags = NONE;
                break;
          case 'P':                             /* DOS App needs parms  */
          case 'p':                             /* (backward compatibility) */
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
          pa->a_letter = (*pcurr == ' ') ? NULL : *pcurr;
          pcurr += 2;
        }
        pcurr = scan_str(pcurr, &pa->a_pappl);
        pcurr = scan_str(pcurr, &pa->a_pdata);

        return(pcurr);
}


void app_tran(WORD bi_num)
{
        LONG            lpbi;
        BITBLK          lb;

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
        WORD            mask[NUM_IBLKS], data[NUM_IBLKS];
        char            copied[NUM_IBLKS];
        char            *p;
        WORD            *addr;
        LONG            temp;
        WORD            i, j, n, iwb, ih;
        WORD            num_mask, num_data, num_wds, num_bytes;

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
        for (i = 0, num_mask = num_data = 0; i < NUM_IBLKS; i++) {
            if (mask[i] < 0) {
                mask[i] = num_mask;
                addr = icon_rs_iconblk[i].ib_pmask;
                for (j = i+1; j < NUM_IBLKS; j++)
                    if (icon_rs_iconblk[j].ib_pmask == addr)
                        mask[j] = num_mask;
                num_mask++;
            }
            if (data[i] < 0) {
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
        maskstart = (char *)dos_alloc(num_mask*num_bytes);
        memset(copied, 0x00, NUM_IBLKS);
        for (i = 0, p = maskstart; i < NUM_IBLKS; i++) {
            n = mask[i];
            if (!copied[n]) {   /* only copy mask once */
                memcpy(p+n*num_bytes, (char *)icon_rs_iconblk[i].ib_pmask, num_bytes);
                copied[n] = 1;
            }
        }

        datastart = (char *)dos_alloc(num_data*num_bytes);
        memset(copied, 0x00, NUM_IBLKS);
        for (i = 0, p = datastart; i < NUM_IBLKS; i++) {
            n = data[i];
            if (!copied[n]) {   /* only copy data once */
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
        for (i = 0; i < NUM_IBLKS; i++) {
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
} /* app_rdicon */


/*
*       Initialize the application list by reading in the DESKTOP.INF
*       file, either from memory or from the disk if the shel_get
*       indicates no message is there.
*/
void app_start(void)
{
        WORD            i, x, y;
        ANODE           *pa;
        WSAVE           *pws;
        BYTE            *pcurr, *ptmp;
        WORD            envr, xcnt, ycnt, xcent, wincnt, dummy;

                                                /* remember start drive */
        gl_stdrv = dos_gdrv();

        G.g_pbuff = &gl_buffer[0];

        for(i=NUM_ANODES - 2; i >= 0; i--)
          G.g_alist[i].a_next = &G.g_alist[i + 1];
        G.g_ahead = (ANODE *) NULL;
        G.g_aavail = &G.g_alist[0];
        G.g_alist[NUM_ANODES - 1].a_next = (ANODE *) NULL;

        app_rdicon();

        G.g_wicon = (12 * gl_wschar) + (2 * G.g_iblist[0].ib_xtext);
        G.g_hicon = G.g_iblist[0].ib_hicon + gl_hschar + 2;

        xcnt = gl_width / (G.g_wicon+MIN_WINT); /* icon count */
        G.g_icw = gl_width / xcnt;              /* width */

        ycnt = (gl_height-gl_hbox) / (G.g_hicon+MIN_HINT);  /* icon count */
        G.g_ich = (gl_height-gl_hbox) / ycnt;   /* height */

        shel_get(gl_afile, SIZE_AFILE);
        if (gl_afile[0] != '#')                 /* invalid signature    */
        {                                       /*   so read from disk  */
          WORD fh;
          char inf_file_name[16];
          strcpy(inf_file_name, INF_FILE_NAME);
          inf_file_name[0] += gl_stdrv;         /* Adjust drive letter  */
          fh = dos_open(inf_file_name, 0x0);
          if( !DOS_ERR )
          {
            G.g_afsize = dos_read(fh, SIZE_AFILE, gl_afile);
            dos_close(fh);
            gl_afile[G.g_afsize] = NULL;
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
          BYTE env1, env2;

          /* Preferences */
          env1 = 0x1a;
          env2 = 0x01;
          if (cookie_idt & IDT_24H)
            env2 |= 0x02; /* 24 hours */
          if (cookie_idt & IDT_BIT_DM)
            env2 |= 0x04; /* day before month */
          sprintf(gl_afile, "#E %02X %02X\r\n", env1, env2);

          /* Windows */
          strcat(gl_afile, desk_inf_data1);

          /* Scan for valid drives: */
          drivemask = dos_sdrv( dos_gdrv() );
          for(i=0; i<26; i++)
            if(drivemask&(1L<<i))
            {
              x = strlen(gl_afile);
              drive_x = icon_index % xcnt; /* x position */
              drive_y = icon_index / xcnt; /* y position */
              icon_type = (i > 1) ? 0 /* Hard disk */ : 1 /* Floppy */;
              drive_letter = 'A' + i;
              rsrc_gaddr(R_STRING, STDISK, (LONG *)&text);
              sprintf(gl_afile + x, "#M %02X %02X %02X FF %c %s %c@ @ \r\n",
                      drive_x, drive_y, icon_type, drive_letter, text, drive_letter);
              icon_index++;
            }

          /* Copy core data part 2 */
          strcat(gl_afile, desk_inf_data2);

          /* add Trash icon to end */
          x = strlen(gl_afile);
          trash_x = 0; /* Left */
          trash_y = ycnt-1; /* Bottom */
          if (drive_y >= trash_y) /* if the last dive icon overflows over the */
            trash_x = xcnt-1;  /* trash row, force trash to right */
          rsrc_gaddr(R_STRING, STTRASH, (LONG *)&text);
          sprintf(gl_afile + x, "#T %02X %02X 03 FF   %s@ @ \r\n",
                  trash_x, trash_y, text);
          G.g_afsize = strlen(gl_afile);
        }

        wincnt = 0;
        pcurr = &gl_afile[0];

        while (*pcurr)
        {
          if (*pcurr != '#')
            pcurr++;
          else
          {
            pcurr++;
            switch(*pcurr)
            {
              case 'T':                         /* Trash */
                        pa = app_alloc(TRUE);
                        pcurr = app_parse(pcurr, pa);
                        break;
              case 'M':                         /* Media (Hard/Floppy)  */
              case 'G':                         /* GEM Application      */
              case 'Y':                         /* GEM App. with parms  */
              case 'F':                         /* File (DOS w/o parms) */
              case 'f':                         /*   use full memory    */
              case 'P':                         /* Parm (DOS w/ parms)  */
              case 'p':                         /*   use full memory    */
              case 'D':                         /* Directory            */
                        pa = app_alloc(TRUE);
                        pcurr = app_parse(pcurr, pa);
                        break;
              case 'W':                         /* Window               */
                        pcurr++;
                        if ( wincnt < NUM_WNODES )
                        {
                          pws = &G.g_cnxsave.win_save[wincnt];
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
                          ptmp = &pws->pth_save[0];
                          pcurr++;
                          while ( *pcurr != '@' )
                            *ptmp++ = *pcurr++;
                          *ptmp = NULL;
                          wincnt += 1;
                        }
                        break;
              case 'E':
                        pcurr++;
                        pcurr = scan_2(pcurr, &envr);
                        G.g_cnxsave.vitem_save = ( (envr & 0x80) != 0);
                        G.g_cnxsave.sitem_save = ( (envr & 0x60) >> 5);
                        G.g_cnxsave.cdele_save = ( (envr & 0x10) != 0);
                        G.g_cnxsave.ccopy_save = ( (envr & 0x08) != 0);
                        G.g_cnxsave.cdclk_save = envr & 0x07;

                        pcurr = scan_2(pcurr, &envr);
                        G.g_cnxsave.covwr_save = ( (envr & 0x10) == 0);
                        G.g_cnxsave.cmclk_save = gl_mnclick = ( (envr & 0x08) != 0);
                        G.g_cnxsave.cdtfm_save = ( (envr & 0x04) == 0);
                        G.g_cnxsave.ctmfm_save = ( (envr & 0x02) == 0);
                        sound(FALSE, !(envr & 0x01), 0);
                        break;
            }
          }
        }

        for (pa = G.g_ahead; pa; pa = pa->a_next)
        {
                x = pa->a_xspot * G.g_icw;
                y = pa->a_yspot * G.g_ich + G.g_ydesk;
                snap_disk(x, y, &pa->a_xspot, &pa->a_yspot);
        }

        xcent = (G.g_wicon - G.g_iblist[0].ib_wicon) / 2;
        G.g_nmicon = 9;
        G.g_xyicon[0] = xcent;  G.g_xyicon[1] = 0;
        G.g_xyicon[2]=xcent; G.g_xyicon[3]=G.g_hicon-gl_hschar-2;
        G.g_xyicon[4] = 0;  G.g_xyicon[5] = G.g_hicon-gl_hschar-2;
        G.g_xyicon[6] = 0;  G.g_xyicon[7] = G.g_hicon;
        G.g_xyicon[8] = G.g_wicon;  G.g_xyicon[9] = G.g_hicon;
        G.g_xyicon[10]=G.g_wicon; G.g_xyicon[11] = G.g_hicon-gl_hschar-2;
        G.g_xyicon[12]=G.g_wicon - xcent; G.g_xyicon[13]=G.g_hicon-gl_hschar-2;
        G.g_xyicon[14] = G.g_wicon - xcent;  G.g_xyicon[15] = 0;
        G.g_xyicon[16] = xcent;  G.g_xyicon[17] = 0;
        G.g_nmtext = 5;
        G.g_xytext[0] = 0;              G.g_xytext[1] = 0;
        G.g_xytext[2] = gl_wchar * 12;  G.g_xytext[3] = 0;
        G.g_xytext[4] = gl_wchar * 12;   G.g_xytext[5] = gl_hchar;
        G.g_xytext[6] = 0;              G.g_xytext[7] = gl_hchar;
        G.g_xytext[8] = 0;              G.g_xytext[9] = 0;
}


/*
*       Reverse list when we write so that we can read it in naturally
*/
static void app_revit(void)
{
        ANODE           *pa;
        ANODE           *pnxtpa;
                                                /* reverse list         */
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
*       Save the current state of all the icons to a file called
*       EMUDESK.INF
*/
void app_save(WORD todisk)
{
        WORD            i, fh, ret;
        WORD            env1, env2, mode;
        BYTE            type;
        BYTE            *pcurr, *ptmp;
        ANODE           *pa;
        WSAVE           *pws;
        WNODE           *w;
        BYTE            inf_file_name[sizeof(INF_FILE_NAME)];

        memset(&gl_afile[0], 0, SIZE_AFILE);
        pcurr = &gl_afile[0];
                                                /* save evironment      */
        env1 = (G.g_cnxsave.vitem_save) ? 0x80 : 0x00;
        env1 |= ((G.g_cnxsave.sitem_save) << 5) & 0x60;
        env1 |= (G.g_cnxsave.cdele_save) ? 0x10 : 0x00;
        env1 |= (G.g_cnxsave.ccopy_save) ? 0x08 : 0x00;
        env1 |= G.g_cnxsave.cdclk_save;
        env2 = (G.g_cnxsave.covwr_save) ? 0x00 : 0x10;
        env2 |= (G.g_cnxsave.cmclk_save) ? 0x08 : 0x00;
        env2 |= (G.g_cnxsave.cdtfm_save) ? 0x00 : 0x04;
        env2 |= (G.g_cnxsave.ctmfm_save) ? 0x00 : 0x02;
        env2 |= sound(FALSE, 0xFFFF, 0)  ? 0x00 : 0x01;
#if CONF_WITH_VIDEL
        mode = get_videl_mode();
        if (!mode)                      /* i.e. not videl */
#endif
            mode = 0xff00 | Getrez();
        pcurr += sprintf(pcurr,"#E %02X %02X %02X %02X\r\n",
                        env1,env2,(mode>>8)&0x00ff,mode&0x00ff);

                                                /* save windows         */
        for(i=0; i<NUM_WNODES; i++)
        {
          pws = &G.g_cnxsave.win_save[i];
          ptmp = pws->pth_save;
          pcurr += sprintf(pcurr,"#W %02X %02X %02X %02X %02X %02X %02X",
                    0,pws->vsl_save,pws->x_save/gl_wchar,
                    pws->y_save/gl_hchar,pws->w_save/gl_wchar,
                    pws->h_save/gl_hchar,pws->obid_save);
          pcurr += sprintf(pcurr," %s@\r\n",(*ptmp!='@')?ptmp:"");
        }

                                                /* reverse ANODE list   */
        app_revit();
                                                /* save ANODE list      */
        for(pa=G.g_ahead; pa; pa=pa->a_next)
        {
          switch(pa->a_type)
          {
            case AT_ISDISK:
                type = 'M';
                break;
            case AT_ISFILE:
                if ( (pa->a_flags & (AF_ISCRYS|AF_ISGRAF)) == (AF_ISCRYS|AF_ISGRAF) )
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
          pcurr += sprintf(pcurr," %s@",pa->a_pappl);
          pcurr += sprintf(pcurr," %s@\r\n",pa->a_pdata);
        }
        *pcurr++ = 0x1a;
        *pcurr++ = 0x0;
                                                /* reverse list back    */
        app_revit();
                                                /* calculate size       */
        G.g_afsize = pcurr - &gl_afile[0];
                                                /* save in memory       */
        shel_put(gl_afile, G.g_afsize);
                                                /* save to disk         */
        if (todisk)
        {
          G.g_afsize--;
          fh = 0;
          while (!fh)
          {
            strcpy(inf_file_name, INF_FILE_NAME);
            inf_file_name[0] += gl_stdrv;         /* Adjust drive letter  */
            fh = dos_create(inf_file_name, 0x0);
            if( DOS_ERR )
            {
              fh = 0;
              ret = fun_alert(1, STNOINF, NULLPTR);
              if (ret == 2)
                return;
            }
          }
          G.g_afsize = dos_write(fh, G.g_afsize, gl_afile);
          dos_close(fh);

          /* now update any open windows for the directory containing the saved file */
          del_fname(inf_file_name);     /* convert to pathname ending in *.* */
          w = fold_wind(inf_file_name); /* scan for matching windows */
          if (w)                        /* got one:                          */
            fun_rebld(w);               /* rebuild all matching open windows */
        }
}


/*
*       Build the desktop list of objects based on this current
*       application list.
*/
BYTE app_blddesk(void)
{
        WORD            obid;
        UWORD           bvdisk, bvhard, bvect;
        ANODE           *pa;
        OBJECT          *pob;
        ICONBLK         *pic;
        LONG            *ptr;
                                                /*   kids and set size  */
        obj_wfree(DROOT, 0, 0, gl_width, gl_height);
        ptr = (LONG *)&global[3];
        G.g_screen[DROOT].ob_spec = *ptr;
        bvdisk = bvhard = 0x0;

        for(pa = G.g_ahead; pa; pa = pa->a_next)
        {
          if (pa->a_flags & AF_ISDESK)
          {
            obid = obj_ialloc(DROOT, pa->a_xspot, pa->a_yspot,
                                        G.g_wicon, G.g_hicon);
            if (!obid)
            {
            /* error case, no more obs */
            }
                                                /* set up disk vector   */
            if (pa->a_type == AT_ISDISK)
            {
              bvect = ((UWORD) 0x8000) >> ((UWORD) (pa->a_letter - 'A'));
              bvdisk |= bvect;
              if (pa->a_aicon == IG_HARD)
                bvhard |= bvect;
            }
                                                /* remember it          */
            pa->a_obid = obid;
                                                /* build object         */
            pob = &G.g_screen[obid];
            pob->ob_state = NORMAL;
            pob->ob_flags = NONE;
            pob->ob_type = G_ICON;
            G.g_index[obid] = pa->a_aicon;
            pic = &gl_icons[obid];
            pob->ob_spec = (LONG)pic;
            memcpy(pic, &G.g_iblist[pa->a_aicon], sizeof(ICONBLK));
            pic->ib_xicon = ((G.g_wicon - pic->ib_wicon) / 2);
            pic->ib_ptext = pa->a_pappl;
            pic->ib_char |= (0x00ff & pa->a_letter);
          } /* if */
        } /* for */
        /*appl_bvset(bvdisk, bvhard);*/ /* This call does not exist in GEM 1.0 - THH */
        return( get_defdrv(bvdisk, bvhard) );
} /* app_blddesk */


/*
*       Find the ANODE that is appropriate for this object.
*/
ANODE *app_afind(WORD isdesk, WORD atype, WORD obid, BYTE *pname, WORD *pisapp)
{
        ANODE           *pa;

        for(pa = G.g_ahead; pa; pa = pa->a_next)
        {
          if (isdesk)
          {
            if (pa->a_obid == obid)
              return(pa);
          }
          else
          {
            if ( (pa->a_type == atype) && !(pa->a_flags & AF_ISDESK) )
            {
              if ( wildcmp(pa->a_pdata, pname) )
              {
                *pisapp = FALSE;
                return(pa);
              }
              if ( wildcmp(pa->a_pappl, pname) )
              {
                *pisapp = TRUE;
                return(pa);
              } /* if */
            } /* if */
          } /* else */
        } /* for */
        return(0);
} /* app_afind */
