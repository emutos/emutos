/*
 * gemfslib.c - 
 *
 * Copyright 1999, Caldera Thin Clients, Inc.
 *           2002 by EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "compat.h"
#include "struct.h"
#include "obdefs.h"
#include "dos.h"
#include "tosvars.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemdos.h"
#include "gemoblib.h"
#include "gemgraf.h"
#include "gemfmlib.h"
#include "gemgsxif.h"
#include "gemgrlib.h"
#include "geminit.h"
#include "gemsuper.h"
#include "gemshlib.h"
#include "gemfslib.h"
#include "gemrslib.h"
#include "gsx2.h"
#include "optimize.h"
#include "optimopt.h"
#include "rectfunc.h"

#include "string.h"
#include "intmath.h"

#define NM_NAMES (F9NAME-F1NAME+1)
#define NAME_OFFSET F1NAME
#define NM_DRIVES (FSLSTDRV-FS1STDRV+1)
#define DRIVE_OFFSET FS1STDRV

#define LEN_FSNAME (LEN_ZFNAME+1)   /* includes leading flag byte & trailing nul */

                            /* max number of files/directory that we can handle */
#define MAX_NM_FILES 1600L          /*  ... if we have enough memory */
#define MIN_NM_FILES 100L           /*  ... if memory is tight */


GLOBAL LONG     ad_fstree;
GLOBAL GRECT    gl_rfs;

static const BYTE gl_fsobj[4] = {FTITLE, FILEBOX, SCRLBAR, 0x0};

static BYTE     *ad_fsnames;    /* holds filenames in currently-displayed directory */
static LONG     *g_fslist;      /* offsets of filenames within ad_fsnames */
static LONG     nm_files;       /* total number of slots in g_fslist[] */

static BYTE     gl_tmp1[LEN_FSNAME];
static BYTE     gl_tmp2[LEN_FSNAME];



/*
*       Routine to back off the end of a path string, stopping at the
*       first backslash or colon encountered.  The second argument
*       specifies the end of the string; if NULL, the end is determined
*       via strlen().  If the scan is stopped by a colon, the routine
*       inserts a backslash in the string immediately following the colon.
*
*       Returns a pointer to the beginning of the string (if no colon or
*       backslash found), or to the last backslash.
*/

static BYTE *fs_back(BYTE *pstr, BYTE *pend)
{
        if (!pend)
          pend = pstr + strlen(pstr);
                                                /* back off to last     */
                                                /*   slash              */
        while ( (*pend != ':') &&
                (*pend != '\\') &&
                (pend != pstr) )
          pend--;
                                                /* if a : then insert   */
                                                /*   a backslash        */
        if (*pend == ':')
        {
          pend++;
          ins_char(pend, 0, '\\', LEN_ZPATH-3);
        }
        return(pend);
}


/*
*       Routine to back up a path and return the pointer to the beginning
*       of the file specification part
*/

static BYTE *fs_pspec(BYTE *pstr, BYTE *pend)
{
        pend = fs_back(pstr, pend);
        if (*pend == '\\')
          pend++;
        else
        {
          strcpy(pstr, "A:\\*.*");
          pstr[0] += (BYTE) dos_gdrv();
          pend = pstr + 3;
        }
        return(pend);
}

/*
*       Routine to compare files based on name
*       Note: folders always sort lowest because the first character is \007
*/

static WORD fs_comp(BYTE *name1, BYTE *name2)
{
        return ( strcmp(name1, name2) );
}



static LONG fs_add(WORD thefile, LONG fs_index)
{
        WORD            len;

        len = strlencpy(ad_fsnames + fs_index, &D.g_dta[29]);
        g_fslist[thefile] = fs_index;
        fs_index += len + 1;
        return(fs_index);
}


/*
*       Make a particular path the active path.  This involves
*       reading its directory, initializing a file list, and filling
*       out the information in the path node.  Then sort the files.
*/

static WORD fs_active(BYTE *ppath, BYTE *pspec, WORD *pcount)
{
        WORD            ret;
        LONG            thefile, fs_index, temp;
        register WORD   i, j, gap;
        BYTE            *fname, allpath[LEN_ZPATH+1];
        
        gsx_mfset(ad_hgmice);

        thefile = 0L;
        fs_index = 0L;

        strcpy(allpath, ppath);               /* 'allpath' gets all files */
        fname = fs_back(allpath,NULL);
        strcpy(fname+1,"*.*");

        dos_sdta((LONG)D.g_dta);
        ret = dos_sfirst(allpath, F_SUBDIR);
        while ( ret )
        {
                                                /* if it is a real file */
                                                /*   or directory then  */
                                                /*   save it and set    */
                                                /*   first byte to tell */
                                                /*   which              */
          if (D.g_dta[30] != '.')
          {
            D.g_dta[29] = (D.g_dta[21] & F_SUBDIR) ? 0x07 : ' ';
            if ( (D.g_dta[29] == 0x07) ||
                 (wildcmp(pspec, &D.g_dta[30])) )
            {
              fs_index = fs_add(thefile, fs_index);
              thefile++;
            }
          }
          ret = dos_snext();

          if (thefile >= nm_files)
          {
            ret = FALSE;
            sound(TRUE, 660, 4);
          }
        }
        *pcount = thefile;
                                                /* sort files using shell*/
                                                /*   sort on page 108 of */
                                                /*   K&R C Prog. Lang.  */
        for(gap = thefile/2; gap > 0; gap /= 2)
        {
          for(i = gap; i < thefile; i++)
          {
            for (j = i-gap; j >= 0; j -= gap)
            {
              if ( fs_comp(ad_fsnames+g_fslist[j],ad_fsnames+g_fslist[j+gap]) <= 0 )
                break;
              temp = g_fslist[j];
              g_fslist[j] = g_fslist[j+gap];
              g_fslist[j+gap] = temp;
            }
          }
        }

        gsx_mfset( ad_armice );
        return(TRUE);
}


/*
*       Routine to adjust the scroll counters by one in either
*       direction, being careful not to overrun or underrun the
*       tail and heads of the list
*/

static WORD fs_1scroll(WORD curr, WORD count, WORD touchob)
{
        register WORD   newcurr;

        newcurr = (touchob == FUPAROW) ? (curr - 1) : (curr + 1);
        if (newcurr < 0)
          newcurr++;
        if ( (count - newcurr) < NM_NAMES )
          newcurr--;
        return( (count > NM_NAMES) ? newcurr : curr );
}


/*
*       Routine to take the filenames that will appear in the window, 
*       based on the current scrolled position, and point at them 
*       with the sub-tree of G_STRINGs that makes up the window box.
*/
static void fs_format(LONG tree, WORD currtop, WORD count)
{
        register WORD   i, cnt;
        register WORD   y, h, th;
        OBJECT          *obj, *treeptr = (OBJECT *)tree;
                                                /* build in real text   */
                                                /*   strings            */
        cnt = min(NM_NAMES, count - currtop);
        for(i=0, obj=treeptr+NAME_OFFSET; i<NM_NAMES; i++, obj++)
        {
          if (i < cnt)
          {
            strcpy(gl_tmp2, ad_fsnames + g_fslist[currtop+i]);
            fmt_str(&gl_tmp2[1], &gl_tmp1[1]);
            gl_tmp1[0] = gl_tmp2[0];
          }
          else
          {
            gl_tmp1[0] = ' ';
            gl_tmp1[1] = NULL;
          }
          inf_sset(tree, NAME_OFFSET+i, gl_tmp1);
          obj->ob_type = G_FBOXTEXT;
          obj->ob_state = NORMAL;
        }
                                                /* size and position the*/
                                                /*   elevator           */
        y = 0;
        obj = treeptr + FSVSLID;
        th = h = obj->ob_height;
        if ( count > NM_NAMES)
        {
          h = mul_div(NM_NAMES, h, count);
          h = max(gl_hbox/2, h);                /* min size elevator    */
          y = mul_div(currtop, th-h, count-NM_NAMES);
        }
        obj = treeptr + FSVELEV;
        obj->ob_y = y;
        obj->ob_height = h;
}


/*
*       Routine to select or deselect a file name in the scrollable 
*       list.
*/
static void fs_sel(WORD sel, WORD state)
{
        if (sel)
          ob_change(ad_fstree, F1NAME + sel - 1, state, TRUE);
}


/*
*       Routine to handle scrolling the directory window a certain number
*       of file names.
*/
static WORD fs_nscroll(LONG tree, WORD *psel, WORD curr, WORD count, 
                       WORD touchob, WORD n)
{
        register WORD   i, newcurr, diffcurr;
        WORD            sy, dy, neg;
        GRECT           r[2];
                                                /* single scroll n times*/
        newcurr = curr;
        for (i=0; i<n; i++)
          newcurr = fs_1scroll(newcurr, count, touchob);
                                                /* if things changed    */
                                                /*   then redraw        */
        diffcurr = newcurr - curr;
        if (diffcurr)
        {
          curr = newcurr;
          fs_sel(*psel, NORMAL);
          *psel = 0;
          fs_format(tree, curr, count);
          gsx_gclip(&r[1]);
          ob_actxywh(tree, F1NAME, &r[0]);

          if (( neg = (diffcurr < 0)) != 0 )
            diffcurr = -diffcurr;

          if (diffcurr < NM_NAMES)
          {
            sy = r[0].g_y + (r[0].g_h * diffcurr);
            dy = r[0].g_y;

            if (neg)
            {
              dy = sy;
              sy = r[0].g_y;
            }

            bb_screen(S_ONLY, r[0].g_x, sy, r[0].g_x, dy, r[0].g_w, 
                                r[0].g_h * (NM_NAMES - diffcurr) );
            if ( !neg )
              r[0].g_y += r[0].g_h * (NM_NAMES - diffcurr);
          }
          else
            diffcurr = NM_NAMES;

          r[0].g_h *= diffcurr;
          for(i=0; i<2; i++)
          {
            gsx_sclip(&r[i]);
            ob_draw(tree, ((i) ? FSVSLID : FILEBOX), MAX_DEPTH);
          }
        }
        return(curr);
}


/*
*       Routine to call when a new directory has been specified.  This
*       will activate the directory, format it, and display ir[0].
*/
        
static WORD fs_newdir(BYTE *fpath, 
                      BYTE *pspec, 
                      LONG tree, 
                      WORD *pcount)
{
        const BYTE      *ptmp;
        BYTE            *ftitle;
        OBJECT          *obj;
        TEDINFO         *tedinfo;
        WORD            len, len_ftitle;
                                        /* BUGFIX 2.1 added len calculation*/
                                        /*  so FTITLE doesn't run over into*/
                                        /*  F1NAME.                     */
        ob_draw(tree, FSDIRECT, MAX_DEPTH);
        fs_active(fpath, pspec, pcount);
        fs_format(tree, 0, *pcount);

        obj = ((OBJECT *)tree) + FTITLE;
        tedinfo = (TEDINFO *)obj->ob_spec;
        ftitle = (BYTE *)tedinfo->te_ptext;

        len_ftitle = tedinfo->te_txtlen - 1;
        len = strlen(pspec);
        len = (len > len_ftitle) ? len_ftitle : len;

        *ftitle++ = ' ';
        memcpy(ftitle, pspec, len);
        ftitle += len;
        *ftitle++ = ' ';
        *ftitle = '\0';

        ptmp = &gl_fsobj[0];    /* redraw file selector objects */
        while(*ptmp)
          ob_draw(tree, *ptmp++, MAX_DEPTH);
        return(TRUE);
}


/*
 * sets wildcard mask from string
 * if no mask, uses default *.* & adds it to string
 */
static void set_mask(BYTE *mask,BYTE *path)
{
        BYTE            *pend;

        pend = fs_back(path, NULL);
        if (!*++pend)
          strcpy(pend, "*.*");
        pend[LEN_ZFNAME] = '\0';    /* avoid possibility of overflow on strcpy() */
        strcpy(mask, pend);
}



/*
 *      Marks object corresponding to specified drive as selected,
 *      and all others as deselected.  Optionally, if the selected
 *      drive has changed, the affected drive buttons are redrawn.
 */
static void select_drive(LONG treeaddr, WORD drive, WORD redraw)
{
        WORD            i, olddrive = -1;
        OBJECT          *obj, *start = (OBJECT *)treeaddr+DRIVE_OFFSET;

        for (i = 0, obj = start; i < NM_DRIVES; i++, obj++)
        {
          if (obj->ob_state & SELECTED)
          {
            obj->ob_state &= ~SELECTED;
            olddrive = i;
          }
        }
        (start+drive)->ob_state |= SELECTED;

        if (redraw && (drive != olddrive))
        {
          if (olddrive >= 0)
            ob_draw(treeaddr,olddrive+DRIVE_OFFSET,MAX_DEPTH);
          ob_draw(treeaddr,drive+DRIVE_OFFSET,MAX_DEPTH);
        }
}



/*
 *      Compares specified path to FSDIRECT TEDINFO text and
 *      returns 1 iff it is different in the first n characters,
 *      where n is the maximum text length from the TEDINFO.
 */
static WORD path_changed(char *path)
{
        OBJECT          *obj;
        TEDINFO         *ted;

        obj = ((OBJECT *)ad_fstree) + FSDIRECT;
        ted = (TEDINFO *)obj->ob_spec;

        if (strncmp(path,(BYTE *)ted->te_ptext,ted->te_txtlen-1))
          return 1;

        return 0;
}



/*
*       File Selector input routine that takes control of the mouse
*       and keyboard, searchs and sort the directory, draws the file 
*       selector, interacts with the user to determine a selection
*       or change of path, and returns to the application with
*       the selected path, filename, and exit button.
*/
WORD fs_input(BYTE *pipath, BYTE *pisel, WORD *pbutton, BYTE *pilabel)
{
        register WORD   touchob, value, fnum;
        WORD            curr, count, sel;
        WORD            mx, my;
        LONG            tree;
        ULONG           bitmask;
        BYTE            *ad_fpath, *ad_fname, *ad_ftitle;
        WORD            drive; 
        WORD            dclkret, cont, newlist, newsel, newdrive;
        register BYTE   *pstr;
        GRECT           pt;
        BYTE            locstr[LEN_ZPATH+1], mask[LEN_ZFNAME+1];
        OBJECT          *obj;
        TEDINFO         *tedinfo;

        curr = 0;
        count = 0;
                                        /* get out quick if path is     */
                                        /*   nullptr or if pts to null. */
        if (pipath == NULL)
          return(FALSE);
        if ( *pipath == '\0')
          return(FALSE);
                                        /* get memory for the filename buffer */
                                        /*  & the array that points to it     */
        for (nm_files = MAX_NM_FILES; nm_files >= MIN_NM_FILES; nm_files /= 2)
        {
          ad_fsnames = (BYTE *)dos_alloc(nm_files*(LEN_FSNAME+sizeof(BYTE *)));
          if (ad_fsnames)
            break;
        }
        if (!ad_fsnames)
          return(FALSE);
        g_fslist = (LONG *)(ad_fsnames+nm_files*LEN_FSNAME);

        strcpy(locstr, pipath);

        tree = ad_fstree;
                                                /* init strings in form */
        obj = ((OBJECT *)tree) + FTITLE;
        tedinfo = (TEDINFO *)obj->ob_spec;
        ad_ftitle = (BYTE *)tedinfo->te_ptext;
        set_mask(mask, locstr);                 /* save caller's mask */
        strcpy(ad_ftitle, mask);                /*  & copy to title line */

        obj = ((OBJECT *)tree) + FSDIRECT;
        tedinfo = (TEDINFO *)obj->ob_spec;
        ad_fpath = (BYTE *)tedinfo->te_ptext;
        inf_sset(tree, FSDIRECT, locstr);

        obj = ((OBJECT *)tree) + FSSELECT;
        tedinfo = (TEDINFO *)obj->ob_spec;
        ad_fname = (BYTE *)tedinfo->te_ptext;
        fmt_str(pisel, gl_tmp2);                /* gl_tmp2[] is without dot */
        inf_sset(tree, FSSELECT, gl_tmp2);

        obj = ((OBJECT *)tree) + FSTITLE;
        obj->ob_spec = pilabel ? (LONG)pilabel : (LONG)rs_str(ITEMSLCT);

                                                /* set drive buttons */
        obj = ((OBJECT *)tree) + DRIVE_OFFSET;
        for (drive = 0, bitmask = 1; drive < NM_DRIVES; drive++, bitmask <<= 1, obj++)
        {
          if (drvbits & bitmask)
            obj->ob_state &= ~DISABLED;
          else
            obj->ob_state |= DISABLED;
        }
        select_drive(tree,locstr[0]-'A',0);
                                                /* set clip and start   */
                                                /*   form fill-in by    */
                                                /*   drawing the form   */
        gsx_sclip(&gl_rfs);     
        fm_dial(FMD_START, &gl_rfs);
        ob_draw(tree, ROOT, 2);
                                                /* init for while loop  */
                                                /*   by forcing initial */
                                                /*   fs_newdir call     */
        sel = 0;
        newsel = FALSE;
        cont = newlist = TRUE;
        while( cont )
        {
          touchob = (newlist) ? 0x0 : fm_do(tree, FSSELECT);
          gsx_mxmy(&mx, &my);
        
          if ( newlist )
          {
            fs_sel(sel, NORMAL);
            if ( (touchob == FSOK) ||
                 (touchob == FSCANCEL) )
              ob_change(tree, touchob, NORMAL, TRUE);
            inf_sset(tree, FSDIRECT, locstr);
            pstr = fs_pspec(locstr, NULL);        
            strcpy(pstr, mask);
            fs_newdir(locstr, mask, tree, &count);
            curr = 0;
            sel = touchob = 0;
            newlist = FALSE;
          }

          value = 0;
          dclkret = ((touchob & 0x8000) != 0);
          switch( (touchob &= 0x7fff) )
          {
            case FSOK:
            case FSCANCEL:
                cont = FALSE;
                break;
            case FUPAROW:
            case FDNAROW:
                value = 1;
                break;
            case FSVSLID:
                ob_actxywh(tree, FSVELEV, &pt);
                /* anemic slidebars
                  pt.g_x -= 3;
                  pt.g_w += 6;
                */
                if ( !inside(mx, my, &pt) )
                  {
                  touchob = (my <= pt.g_y) ? FUPAROW : FDNAROW;
                  value = NM_NAMES;
                  break;
                  }
                /* drop through */
            case FSVELEV:
                fm_own(TRUE);
                value = gr_slidebox(tree, FSVSLID, FSVELEV, TRUE);
                fm_own(FALSE);
                value = curr - mul_div(value, count-NM_NAMES, 1000);
                if (value >= 0)
                  touchob = FUPAROW;
                else
                {
                  touchob = FDNAROW;
                  value = -value;
                }
                break;
            case F1NAME:
            case F2NAME:
            case F3NAME:
            case F4NAME:
            case F5NAME:
            case F6NAME:
            case F7NAME:
            case F8NAME:
            case F9NAME:
                fnum = touchob - F1NAME + 1;
                if ( fnum > count )
                  break;
                if ( (sel) && (sel != fnum) )
                  fs_sel(sel, NORMAL);
                if ( sel != fnum)
                {
                  sel = fnum;
                  fs_sel(sel, SELECTED);
                }
                                                /* get string and see   */
                                                /*   if file or folder  */
                inf_sget(tree, touchob, gl_tmp1);
                if (gl_tmp1[0] == ' ')          /* file selected */
                {                               /* copy to selection    */
                  newsel = TRUE;
                  if (dclkret)
                    cont = FALSE;
                }
                else                            /* folder selected */
                {
                                                /* append in folder name*/
                  pstr = fs_pspec(locstr, NULL);
                  unfmt_str(&gl_tmp1[1], pstr);
                  pstr += strlen(pstr);
                  *pstr++ = '\\';
                  strcpy(pstr, mask);
                  newlist = TRUE;
                }
                break;
            case FCLSBOX:
                pstr = fs_back(locstr, NULL);
                if (*pstr-- != '\\')    /* ignore strange path string */
                  break;
                if (*pstr != ':')       /* not at root of drive, so back up */
                {
                  pstr = fs_back(locstr, pstr);
                  if (*pstr == '\\')    /* we must have at least X:\ */
                    strcpy(pstr+1, mask);
                }
                newlist = TRUE;
                break;
            default:
                drive = touchob - DRIVE_OFFSET;
                if ((drive < 0) || (drive >= NM_DRIVES))/* not for us */
                  break;
                if (drive == locstr[0] - 'A')           /* no change */
                  break;
                obj = ((OBJECT *)tree) + touchob;
                if (obj->ob_state & DISABLED)           /* non-existent drive */
                  break;
                strcpy(locstr, "A:\\*.*");
                locstr[0] += drive;
                newdrive = TRUE;
                break;
          }
          if (!newlist && !newdrive
           && path_changed(locstr))                     /* path changed manually */
          {
            if (ad_fpath[0] != locstr[0])               /* drive has changed */
              newdrive = TRUE;
            else
              newlist = TRUE;
            strcpy(locstr, ad_fpath);
          }
          if (newdrive)
          {
            select_drive(tree, touchob-DRIVE_OFFSET,1);
            newdrive = FALSE;
            newlist = TRUE;
          }
          if (newlist)
          {
            inf_sset(tree, FSDIRECT, locstr);
            set_mask(mask, locstr);                 /* set mask         */
            gl_tmp1[1] = NULL;
            newsel = TRUE;
          }
          if (newsel)
          {
            strcpy(ad_fname, gl_tmp1 + 1);
            ob_draw(tree, FSSELECT, MAX_DEPTH);
            if (!cont)
              ob_change(tree, FSOK, SELECTED, TRUE);
            newsel = FALSE;
          }
          if (value)
            curr = fs_nscroll(tree, &sel, curr, count, touchob, value);
        }
                                                /* return path and      */
                                                /*   file name to app   */
        strcpy(pipath, locstr);
        unfmt_str(ad_fname, gl_tmp2);
        strcpy(pisel, gl_tmp2);
                                                /* start the redraw     */
        fm_dial(FMD_FINISH, &gl_rfs);
                                                /* return exit button   */
        *pbutton = inf_what(tree, FSOK, FSCANCEL);
        dos_free((LONG)ad_fsnames);

        return( TRUE );
}

