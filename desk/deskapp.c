/*       DESKAPP.C      06/11/84 - 07/11/85             Lee Lorenzen    */
/*      for 3.0         3/6/86   - 5/6/86               MDF             */
/*      for 2.3         9/25/87                         mdf             */

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
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include <string.h>

#include "portab.h"
#include "compat.h"
#include "obdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "deskwin.h"
#include "infodef.h"
#include "gembind.h"
#include "deskbind.h"

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
#include "icons.h"
#include "desk1.h"


#define MIN_WINT 4
#define MIN_HINT 2


GLOBAL WORD     gl_numics;
GLOBAL WORD     gl_stdrv;

static BYTE     gl_afile[SIZE_AFILE];
static BYTE     gl_buffer[SIZE_BUFF];


/* We use this DESKTOP.INF here when we can't load that file from disk: */

static const char *desk_inf_data1 =
    "#E 1A 01\r\n"
    "#W 00 00 02 06 26 0C 00 @\r\n"
    "#W 00 00 02 08 26 0C 00 @\r\n"
    "#W 00 00 02 0A 26 0C 00 @\r\n"
    "#W 00 00 02 0D 26 0C 00 @\r\n";

static const char *desk_inf_data2 =
    "#F FF 28 @ *.*@ \r\n"
    "#D FF 02 @ *.*@ \r\n"
    "#Y 08 FF *.GTP@ @ \r\n"
    "#G 08 FF *.APP@ @ \r\n"
    "#G 08 FF *.PRG@ @ \r\n"
    "#P 08 FF *.TTP@ @ \r\n"
    "#F 08 FF *.TOS@ @ \r\n";


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
#ifdef DESK1
          case 'T':                             /* Trash */
                pa->a_type  = AT_ISTRSH;
                pa->a_flags = AF_ISCRYS | AF_ISGRAF | AF_ISDESK;
                break;
#endif
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
        pcurr = scan_2(pcurr, &pa->a_dicon);
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

        LBCOPY(ADDR(&lb), lpbi, sizeof(BITBLK));

        gsx_trans(lb.bi_pdata, lb.bi_wb, lb.bi_pdata, lb.bi_wb, lb.bi_hl);
}



/************************************************************************/
/* a p p _ r d i c o n                                                  */
/************************************************************************/
/* Note: this file originally loaded the icon data from a file (deskhi.icn
   or desklo.icn). But due to endianess problems and for ROM-ing the desktop,
   I changed this behaviour so that the icons are included in the program
   file now. Hope there aren't too much faults in this new version of this
   function. See icons.c, too.  - THH */
static void app_rdicon(void)
{
#if CONF_WITH_DESKTOP_ICONS
        LONG            temp, stmp, dtmp;
        WORD            length, i, iwb, ih;
        WORD            num_icons, num_masks, last_icon, num_wds, 
                        num_bytes, msk_bytes, tmp;

        length = NUM_IBLKS * sizeof(ICONBLK);

        /* Copy ICONBLK data: */
        memcpy((void *)ADDR(&G.g_idlist[0]), gl_ilist, length);
                                                /* find no. of icons    */
                                                /*   actually used      */
        num_icons = last_icon = 0;
        while ( (last_icon < NUM_IBLKS) &&
                (G.g_idlist[last_icon].ib_pmask != -1L) )
        {
          tmp = max( LLOWD(G.g_idlist[last_icon].ib_pmask),
                     LLOWD(G.g_idlist[last_icon].ib_pdata) );
          num_icons = max(num_icons, tmp);

          last_icon++;
        }
        num_icons++;

        movs(length, &G.g_idlist[0], &G.g_iblist[0]);
                                                /* how many words of    */
                                                /*   data to read?      */
                                                /* assume all icons are */
                                                /*   same w,h as first  */
        num_wds = (G.g_idlist[0].ib_wicon * G.g_idlist[0].ib_hicon) / 16;
        num_bytes = num_wds * 2;

                                                /* allocate some memory */
                                                /*   in bytes           */
       length = (LONG)icondata_end - (LONG)icondata_start;

        G.a_datastart = dos_alloc( length );
                                                /* copy it              */
        memcpy((void *)G.a_datastart, icondata_start, length);

       gl_numics = 18;
                                                /* figure out which are */
                                                /*   mask & which data  */
        for (i=0; i<last_icon; i++)
        {
          G.g_ismask[ (WORD) G.g_idlist[i].ib_pmask] = TRUE;
          G.g_ismask[ (WORD) G.g_idlist[i].ib_pdata] = FALSE;
        }
                                                /* fix up mask ptrs     */
        num_masks = 0;
        for (i=0; i<num_icons; i++)
        {
          if (G.g_ismask[i])
          {
            G.g_ismask[i] = num_masks;
            num_masks++;
          }
          else
            G.g_ismask[i] = -1;
        }
                                                /* allocate memory for  */
                                                /*   transformed mask   */
                                                /*   forms              */
        msk_bytes = num_masks * num_bytes;
        G.a_buffstart = dos_alloc( LW(msk_bytes) );
                                                /* fix up icon pointers */
        for (i=0; i<last_icon; i++)
        {
                                                /* first the mask       */
          temp = ( G.g_ismask[ G.g_idlist[i].ib_pmask ] * ((LONG) num_bytes));
          G.g_iblist[i].ib_pmask = G.a_buffstart + LW(temp);
          temp = ( G.g_idlist[i].ib_pmask * ((LONG) num_bytes));
          G.g_idlist[i].ib_pmask = G.a_datastart + LW(temp);
                                                /* now the data         */
          temp = ( G.g_idlist[i].ib_pdata * ((LONG) num_bytes));
          G.g_iblist[i].ib_pdata = G.g_idlist[i].ib_pdata = 
                G.a_datastart + LW(temp);
                                                /* now the text ptrs    */
          G.g_idlist[i].ib_ytext = G.g_iblist[i].ib_ytext = 
                        G.g_idlist[0].ib_hicon;
          G.g_idlist[i].ib_wtext = G.g_iblist[i].ib_wtext = 12 * gl_wschar;
          G.g_idlist[i].ib_htext = G.g_iblist[i].ib_htext = gl_hschar + 2;
        }
                                                /* transform forms      */
        iwb = G.g_idlist[0].ib_wicon / 8;
        ih = G.g_idlist[0].ib_hicon;

        for (i=0; i<num_icons; i++)
        {
          if (G.g_ismask[i] != -1)
          {
                                                /* preserve standard    */
                                                /*   form of masks      */
            stmp = G.a_datastart + (i * num_bytes);
            dtmp = G.a_buffstart + (G.g_ismask[i] * num_bytes);
            LWCOPY(dtmp, stmp, num_wds);
          }
          else
          {
                                                /* transform over std.  */
                                                /*   form of datas      */
            dtmp = G.a_datastart + (i * num_bytes);
          }
          gsx_trans(dtmp, iwb, dtmp, iwb, ih);
        }

#if 0   /* The icon mask are wrong when this is enabled. - THH */
        for (i=0; i<last_icon; i++)
        {
          if ( i == IG_FOLDER )
            G.g_iblist[i].ib_pmask = G.g_iblist[IG_TRASH].ib_pmask;
          if ( ( i == IG_FLOPPY ) ||
               ( i == IG_HARD ) )
            G.g_iblist[i].ib_pmask = G.g_iblist[IG_TRASH].ib_pdata;
          if ( (i >= IA_GENERIC) &&
               (i < ID_GENERIC) )
            G.g_iblist[i].ib_pmask = G.g_iblist[IA_GENERIC].ib_pdata;
          if ( (i >= ID_GENERIC) &&
               (i < (NUM_ANODES - 1)) )
            G.g_iblist[i].ib_pmask = G.g_iblist[ID_GENERIC].ib_pdata;
        }
#endif

#endif /* CONF_WITH_DESKTOP_ICONS */
} /* app_rdicon */


/*
*       Initialize the application list by reading in the DESKTOP.INF
*       file, either from memory or from the disk if the shel_get
*       indicates no message is there.
*/
void app_start(void)
{
        WORD            i, x, y;
#ifndef DESK1
        WORD            w, h;
#endif
        ANODE           *pa;
        WSAVE           *pws;
        BYTE            *pcurr, *ptmp, prevdisk;
        WORD            envr, xcnt, ycnt, xcent, wincnt;

                                                /* remember start drive */
        gl_stdrv = dos_gdrv();

        G.g_pbuff = &gl_buffer[0];
        
        for(i=NUM_ANODES - 2; i >= 0; i--)
          G.g_alist[i].a_next = &G.g_alist[i + 1];
        G.g_ahead = (ANODE *) NULL;
        G.g_aavail = &G.g_alist[0];
        G.g_alist[NUM_ANODES - 1].a_next = (ANODE *) NULL;

        app_rdicon();

        G.g_wicon = (12 * gl_wschar) + (2 * G.g_idlist[0].ib_xtext);
        G.g_hicon = G.g_idlist[0].ib_hicon + gl_hschar + 2;

        G.g_icw = (gl_height <= 300) ? 0 : 8;
        G.g_icw += G.g_wicon;
        xcnt = (gl_width/G.g_icw);
        G.g_icw += (gl_width % G.g_icw) / xcnt;
        G.g_ich = G.g_hicon + MIN_HINT;
        ycnt = ((gl_height-gl_hbox) / G.g_ich);
        G.g_ich += ((gl_height-gl_hbox) % G.g_ich) / ycnt;

        shel_get(ADDR(&gl_afile[0]), SIZE_AFILE);
        if (gl_afile[0] != '#')                 /* invalid signature    */
        {                                       /*   so read from disk  */
          WORD fh;
          char inf_file_name[16];
          strcpy(inf_file_name, ini_str(STGEMAPP));
          inf_file_name[0] += gl_stdrv;         /* Adjust drive letter  */
          fh = dos_open(inf_file_name, 0x0);
          if( !DOS_ERR )
          {
            G.g_afsize = dos_read(fh, SIZE_AFILE, ADDR(&gl_afile[0]));
            dos_close(fh);
            gl_afile[G.g_afsize] = NULL;
          }
        }
        /* If there's still no desktop.inf data, use built-in now: */
        if (gl_afile[0] != '#')
        {
          LONG drivemask;
          int icon_index = 0;
          int drive_x = 0, drive_y = 0;
          int trash_x, trash_y;
          int icon_type;
          char drive_letter;

          drivemask = dos_sdrv( dos_gdrv() ); 
          strcpy(gl_afile, desk_inf_data1);  /* Copy core data part 1*/
          /* Scan for valid drives: */
          for(i=0; i<26; i++)
            if(drivemask&(1L<<i))
            {
              x = strlen(gl_afile);
              drive_x = icon_index % xcnt; /* x position */
              drive_y = icon_index / xcnt; /* y position */
              icon_type = (i > 1) ? 0 /* Hard disk */ : 1 /* Floppy */;
              drive_letter = 'A' + i;
              sprintf(gl_afile + x, "#M %02X %02X %02X FF %c DISK %c@ @ \r\n",
                drive_x, drive_y, icon_type, drive_letter, drive_letter);
              icon_index++;
            }
          strcat(gl_afile, desk_inf_data2);  /* Copy core data part 2 */
          /* add Trash icon to end */
          x = strlen(gl_afile);
          trash_x = 0; /* Left */
          trash_y = ycnt-1; /* Bottom */
          if (drive_y >= trash_y) /* if the last dive icon overflows over the */
            trash_x = xcnt-1;  /* trash row, force trash to right */
          sprintf(gl_afile + x, "#T %02X %02X 03 FF   TRASH@ @ \r\n",
            trash_x, trash_y);
          G.g_afsize = strlen(gl_afile);
        }

        wincnt = 0;
        pcurr = &gl_afile[0];
        prevdisk = ' ';

        while (*pcurr)
        {
          if (*pcurr != '#')
            pcurr++;
          else
          {
            pcurr++;
            switch(*pcurr)
            {
#ifdef DESK1
              case 'T':                         /* Trash */
                        pa = app_alloc(TRUE);
                        pcurr = app_parse(pcurr, pa);

                        for (i = 0; i < 7; i++)
                        {
                            pa = app_alloc(TRUE);
                            app_parse(ini_str(ST1STD+i)+1, pa);
                        } /* for */
                        break;
#endif
              case 'M':                         /* Media (Hard/Floppy)  */
              case 'G':                         /* GEM Application      */
              case 'Y':                         /* GEM App. with parms  */
              case 'F':                         /* File (DOS w/o parms) */
              case 'f':                         /*   use full memory    */
              case 'P':                         /* Parm (DOS w/ parms)  */
              case 'p':                         /*   use full memory    */
              case 'D':                         /* Directory            */
                        if ( *pcurr == 'M' )
                          prevdisk = 'M';
#ifndef DESK1
                        else
                        {
                                                /* rest of standards    */
                                                /*   after last disk    */
                          if (prevdisk == 'M') 
                          {
                            for (i = 0; i < 6; i++)
                            {
                              pa = app_alloc(TRUE);
                              app_parse(ini_str(ST1STD+i)+1, pa);
                            } /* for */
                          } /* if */
                          prevdisk = ' ';
                        }
#endif
                        (void)prevdisk; /* silent warning */
                        pa = app_alloc(TRUE);
                        pcurr = app_parse(pcurr, pa);
                        break;
              case 'W':                         /* Window               */
                        pcurr++;
                        if ( wincnt < NUM_WNODES )
                        {
                          pws = &G.g_cnxsave.win_save[wincnt];
                          pcurr = scan_2(pcurr, &pws->hsl_save);
                          pcurr = scan_2(pcurr, &pws->vsl_save);
/* BugFix       */
#ifdef DESK1
                          pcurr = scan_2(pcurr, &pws->x_save);
                          pws->x_save *= gl_wchar;
                          pcurr = scan_2(pcurr, &pws->y_save);
                          pws->y_save *= gl_hchar;
                          pcurr = scan_2(pcurr, &pws->w_save);
                          pws->w_save *= gl_wchar;
                          pcurr = scan_2(pcurr, &pws->h_save);
                          pws->h_save *= gl_hchar;
#else
                          pcurr = scan_2(pcurr, &x);
                          pcurr = scan_2(pcurr, &y);
                          pcurr = scan_2(pcurr, &w);
                          pcurr = scan_2(pcurr, &h);
#endif
/* */
                          pcurr = scan_2(pcurr, &pws->obid_save);
                          ptmp = &pws->pth_save[0];
                          pcurr++;
                          while ( *pcurr != '@' )
                            *ptmp++ = *pcurr++;
                          *ptmp = NULL;
#ifndef DESK1
                          gl_savewin[wincnt].g_x = x * gl_wchar;
                          gl_savewin[wincnt].g_y = y * gl_hchar;
                          gl_savewin[wincnt].g_w = w * gl_wchar;
                          gl_savewin[wincnt].g_h = h * gl_hchar;
#endif
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
                        G.g_cnxsave.cmclk_save = ( (envr & 0x08) != 0);
                        G.g_cnxsave.cdtfm_save = ( (envr & 0x04) == 0);
                        G.g_cnxsave.ctmfm_save = ( (envr & 0x02) == 0);
                        sound(FALSE, !(envr & 0x01), 0);
                        break;
            }
          }
        }

#ifdef DESK1
        for (pa = G.g_ahead; pa; pa = pa->a_next)
        {
                x = pa->a_xspot * G.g_icw;
                y = pa->a_yspot * G.g_ich + G.g_ydesk;
                snap_disk(x, y, &pa->a_xspot, &pa->a_yspot);            
        }
#endif

        xcent = (G.g_wicon - G.g_idlist[0].ib_wicon) / 2;
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
        mode = get_videl_mode();
        pcurr += sprintf(pcurr,"#E %02X %02X %02X %02X %02X\r\n",
                    env1,env2,getrez()&0x00ff,(mode>>16)&0x00ff,mode&0x00ff);

                                                /* save windows         */
        for(i=0; i<NUM_WNODES; i++)
        {
          pws = &G.g_cnxsave.win_save[i];
          ptmp = pws->pth_save;
          pcurr += sprintf(pcurr,"#W %02X %02X %02X %02X %02X %02X %02X",
                    pws->hsl_save,pws->vsl_save,pws->x_save/gl_wchar,
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
			pcurr += sprintf(pcurr," %02X %02X",
			            pa->a_xspot/G.g_icw,(pa->a_yspot-G.g_ydesk)/G.g_ich);
          pcurr += sprintf(pcurr," %02X %02X",pa->a_aicon&0x00ff,pa->a_dicon&0x00ff);
          if (pa->a_flags & AF_ISDESK)
            pcurr += sprintf(pcurr," %c",pa->a_letter?pa->a_letter:' ');
          pcurr += sprintf(pcurr," %s@",pa->a_pappl);
          pcurr += sprintf(pcurr," %s@\r\n",pa->a_pdata);
                                                /* skip standards       */
#ifndef DESK1
          if ( (pa->a_type == AT_ISDISK) && 
               (pa->a_next->a_type != AT_ISDISK) )
#else
          if ( (pa->a_type == AT_ISTRSH))       /* DESKTOP v1.2 */
#endif
          {
            for(i=0; i<7 && pa!=0; i++)
              pa = pa->a_next;
          }
        }
        *pcurr++ = 0x1a;
        *pcurr++ = 0x0;
                                                /* reverse list back    */
        app_revit();
                                                /* calculate size       */
        G.g_afsize = pcurr - &gl_afile[0];
                                                /* save in memory       */
        shel_put(ADDR(&gl_afile[0]), G.g_afsize);
                                                /* save to disk         */
        if (todisk)
        {
          G.g_afsize--;
          fh = 0;
          while (!fh)
          {
            char inf_file_name[16];
            strcpy(inf_file_name, ini_str(STGEMAPP));
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
          G.g_afsize = dos_write(fh, G.g_afsize, ADDR(&gl_afile[0]));
          dos_close(fh);
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
#if ALCYON
        LONG            *ptr;
#endif                                          /* free all this windows*/
                                                /*   kids and set size  */
        obj_wfree(DROOT, 0, 0, gl_width, gl_height);
#if ALCYON
        ptr = (LONG *)&global[3];
        G.g_screen[DROOT].ob_spec = LLGET(ptr);
#else
        G.g_screen[DROOT].ob_spec = LW(global[3]) + HW(global[4]);
#endif
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
            pob->ob_spec = ADDR( pic = &gl_icons[obid] );
            movs(sizeof(ICONBLK), &G.g_iblist[pa->a_aicon], pic);
            pic->ib_xicon = ((G.g_wicon - pic->ib_wicon) / 2);
            pic->ib_ptext = ADDR(pa->a_pappl);
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

