/*
 * command.c - GEMDOS command interpreter
 *
 * Copyright (c) 2001, Lineo, Inc.
 * Copyright (c) 2001, Martin Doering
 *
 * Authors:
 *  JSL Jason S. Loveman
 *  LGT Lou T. Garavaglia
 *  SCC Steven C. Cavender
 *  MAD Martin Doering
 *  LVL Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef NO_ROM
#include "nls.h"
#include "string.h"
#include "setjmp.h"
#else
#define _(a) a
#define N_(a) a
#endif

extern long jmp_gemdos(int, ...);
extern long jmp_bios(int, ...);
extern long jmp_xbios(int, ...);
extern void in_term(void);
extern void rm_term(void);

#define jmp_gemdos_v(a)         jmp_gemdos((WORD)(a))
#define jmp_gemdos_w(a,b)       jmp_gemdos((WORD)(a),(WORD)(b))
#define jmp_gemdos_l(a,b)       jmp_gemdos((WORD)(a),(LONG)(b))
#define jmp_gemdos_p(a,b)       jmp_gemdos((WORD)(a),(void*)(b))
#define jmp_gemdos_ww(a,b,c)    jmp_gemdos((WORD)(a),(WORD)(b),(WORD)(c))
#define jmp_gemdos_lw(a,b,c)    jmp_gemdos((WORD)(a),(LONG)(b),(WORD)(c))
#define jmp_gemdos_pw(a,b,c)    jmp_gemdos((WORD)(a),(void*)(b),(WORD)(c))
#define jmp_gemdos_wpp(a,b,c,d)    jmp_gemdos((WORD)(a),(WORD)(b),(void*)(c),(void*)(d))
#define jmp_gemdos_wlp(a,b,c,d)    jmp_gemdos((WORD)(a),(WORD)(b),(LONG)(c),(void*)(d))
#define jmp_gemdos_lww(a,b,c,d)    jmp_gemdos((WORD)(a),(LONG)(b),(WORD)(c),(WORD)(d))
#define jmp_gemdos_pww(a,b,c,d)    jmp_gemdos((WORD)(a),(void*)(b),(WORD)(c),(WORD)(d))
#define jmp_gemdos_wppp(a,b,c,d,e) jmp_gemdos((WORD)(a),(WORD)(b),(void*)(c),(void*)(d),(void*)(e))

#define xrdchne() jmp_gemdos_v(0x08)
#define xecho(a) jmp_gemdos_w(0x02,a)
#define xread(a,b,c) jmp_gemdos_wlp(0x3f,a,b,c)
#define xwrite(a,b,c) jmp_gemdos_wlp(0x40,a,b,c)
#define xopen(a,b) jmp_gemdos_pw(0x3d,a,b)
#define xclose(a) jmp_gemdos_w(0x3e,a)
#define xcreat(a,b) jmp_gemdos_pw(0x3c,a,b)
#define xforce(a,b) jmp_gemdos_ww(0x46,a,b)
#define xexec(a,b,c,d) jmp_gemdos_wppp(0x4b,a,b,c,d)
#define dup(a) jmp_gemdos_w(0x45,a)
#define xgetdrv() jmp_gemdos_v(0x19)
#define xsetdrv(a) jmp_gemdos_w(0x0e,a)
#define xsetdta(a) jmp_gemdos_p(0x1a,a)
#define xsfirst(a,b) jmp_gemdos_pw(0x4e,a,b)
#define xsnext() jmp_gemdos_v(0x4f)
#define xgetdir(a,b) jmp_gemdos_pw(0x47,a,b)
#define xmkdir(a) jmp_gemdos_p(0x39,a)
#define xrmdir(a) jmp_gemdos_p(0x3a,a)
#define xchdir(a) jmp_gemdos_p(0x3b,a)
#define xunlink(a) jmp_gemdos_p(0x41,a)
#define xrename(a,b,c) jmp_gemdos_wpp(0x56,a,b,c)
#define xgetfree(a,b) jmp_gemdos_pw(0x36,a,b)
#define xterm(a) jmp_gemdos_w(0x4c,a)
#define xf_seek(a,b,c) jmp_gemdos_lww(0x42,a,b,c)
#define xmalloc(a) jmp_gemdos_l(0x48,a);
#define xmfree(a) jmp_gemdos_p(0x49,a);
#define xattrib(a,b,c) jmp_gemdos_pww(0x43,a,b,c)


#define jmp_bios_w(a,b)     jmp_bios((WORD)(a),(WORD)(b))
#define jmp_bios_wpwww(a,b,c,d,e,f) \
          jmp_bios((WORD)(a),(WORD)(b),(void*)(c),(WORD)(d),(WORD)(e),(WORD)(f))
 
#define getbpb(a) jmp_bios_w(7,a)
#define rwabs(a,b,c,d,e) jmp_bios_wpwww(4,a,b,c,d,e)


#define jmp_xbios_ww(a,b,c)   jmp_xbios((WORD)(a),(WORD)(b),(WORD)(c))

#define Cursconf(a,b) jmp_xbios_ww(0x15,a,b)


#define MAXARGS 20

#define BPB struct _bpb
BPB                             /* bios parameter block */
{
    int recsiz;
    int clsiz;
    int clsizb;
    int rdlen;                  /* root directory length in records */
    int fsiz;                   /* fat size in records */
    int fatrec;                 /* first fat record (of last fat) */
    int datrec;                 /* first data record */
    int numcl;                  /* number of data clusters available */
    int b_flags;
};

struct rdb {                    /*IO redirection info block     */
    int nso;
    int nsi;
    int oldso;
    int oldsi;
};

static struct rdb *rd_ptr;

static const char hexch[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

int drv;
int exeflg;
static int prgerr;
static int cmderr;

static int bExitFlag;      /* TRUE when the user wants to leave the CLI */

jmp_buf jb;
static long compl_code;

#define BUFSIZ 10000
char buf[BUFSIZ];

char lin[130];
static char srchb[44];
static const char prgTail[5] = { ".PRG" };
static const char batTail[5] = { ".BAT" };
static const char pthSymb[6] = { "PATH=" };
static char autoBat[13] = { "AUTOEXEC.BAT" };
static char drvch;
static char *basePage;
static char *prntEnvPtr;

/* Declarations for Wild Card processing: */
static char *WSrcReq;
static int WAttCode;
static const char wildExp[4] = { "*.*" };
static char srcFlNm[67];               /* src file name */
static char dstFlNm[67];               /* destination file name */
static char srcDir[67];                /* src dir path */
static char dstDir[67];                /* dst dir path */
static char srcNmPat[13];              /* src file name specified in path */
static char dstNmPat[13];              /* dst file name specified in path */
char path[67];                  /* lst of default path names */

/* Forward declarations */
static void
xCmdLn(char *parm[], int *pipeflg, long *nonStdIn, char *outsd_tl);



/*
 * chk_redirect - determines it input or output has been redirected,
 * if so restoring it to previous value.
 */

static void
chk_redirect(struct rdb *r)
{
    /* if a new standard in specified ... */
    if (r->nsi == -1) {
        xclose(0);
        xforce(0, r->oldsi);
    }
    /* if a new standard out specified. */
    if (r->nso == -1) {
        xclose(1);
        xforce(1, r->oldso);
    }
}



static void
errout(void)
{
    chk_redirect(rd_ptr);
    longjmp(jb, 1);
}


/*
 * xncpmps - compare strings with n length
 */
static int
xncmps(int n, char *s, char *d)
{
    while (n--)
        if (*s++ != *d++)
            return (0);
    return (1);
}



static void
prthex(unsigned h)
{
    unsigned h2;
    if ((h2 = (h >> 4)))
        prthex(h2);
    else
        xwrite(1, 1L, "0");
    xwrite(1, 1L, &hexch[h & 0x0f]);
}


#if NO_ROM
/* LVL using unsigned long int to stop gcc warning about
 * conflicting types for built-in function `strlen'
 */
unsigned long int
strlen(const char *s)
{
    int n;

    for (n = 0; *s++; n++);
    return (n);
}
#endif /* NO_ROM */



/*
 * pdl - print a decimal long value
 */

static void
pdl(long d)
{
    long d2;
    if ((d2 = d / 10))
        pdl(d2);
    xwrite(1, 1L, &hexch[d % 10]);
}



/*
 * prtdecl - print a decimal long value, if it exists
 */

static void
prtdecl(long d)
{
    if (d)
        pdl(d);
    else
        xwrite(1, 1L, "0");
}



static void
prtDclFmt(long d, int cnt, char *ch)
{
    int i;
    long k, j;

    k = (d ? d : 1);
    j = 1;
    for (i = 1; i < cnt; i++)
        j *= 10;
    while (k < j) {
        xwrite(1, 1L, ch);
        k *= 10;
    }
    prtdecl(d);
}



static void
ucase(char *s)
{
    for (; *s; s++) {
        if ((*s >= 'a') && (*s <= 'z'))
            *s &= ~0x20;
    }
}



/*
 *  gtFlNm - get file name of the next file in the directory match a
 *     path\wildcat specification. The first invocation makes a call to
 *     xsfirst.  Each subsequent invocation uses xsnext().  To invoke
 *     the routine, the wildcarded path name is put into WSrcReq and the
 *     routine called.  For this and each subseqent call the descriptor block
 *     block for the found file (if one was found) is pointed to by WThisSrc.
 *
 *
 *  returns 0 if no match
 *  returns -1 if file found
 *
 */

static int
gtFlNm(void)
{
    /* First file request?      */
    if (WSrcReq != NULLPTR) {
        if (xsfirst(WSrcReq, WAttCode))
            return (FALSE);

        WSrcReq = NULLPTR;
    }

    /* Subsequent file request  */
    else {
        if (xsnext())
            return (FALSE);
    }
    return (TRUE);
}



/*
 * chkDir - Evaluates pathExp
 *
 * to determine if it specifies a directory, or a file name. For convenience
 * sake it stuffs the directory part in dirExp and the file name into filExp.
 * If a file name (ambiguous or not) was found, it is place in filExp.
 * If no file name was found, filExp points to "*.*".
 *
 * int chkDir (pathExp, dirExp, filExp)
 *
 * returns -3 if wild cards specified in path name
 *     -2 if pathExp does NOT specify a file, directory, or device
 *     -1 if pathExp evaluates to a subdirectory, or a volume label
 *      0 if pathExp evaluates to a normal, read-only, or system file
 *      1 if pathExp evaluates to a normal, read-only, or system file but
 *        was specified with wild cards
 *
 * pathExp - ptr to path expression, w/wo wild cards to be evaluated
 * dirExp  - ptr to dir part of pathExp
 * filExp  - ptr to file.ext part of pathExp
 */

static int
chkDir(char *pathExp, char *dirExp, char *filExp)
{
    int dirLen;
    int flExsists;
    char c;

    int pathLen = strlen(pathExp);
    int wildLen = strlen(wildExp);
    int wildFnd = FALSE;
    int i = 0;

    /*directory length = path length to start with. */
    dirLen = pathLen;

    /* Loop thru path expression from end looking for delimeters and the 1st char. */
    do {
        if ((c = pathExp[dirLen]) == '*')
            wildFnd = TRUE;
    }
    while ((c != ':') && (c != '\\') && (dirLen--));
    dirLen++;

    /* IF nothing specified default to wild card expression. */
    if ((pathLen == 0) ||
        (pathExp[pathLen - 1] == '\\') || (pathExp[pathLen - 1] == ':')) {
        while ((pathExp[pathLen] = wildExp[i])) {
            pathLen++;
            i++;
        }
        wildFnd = TRUE;
    }

    /* return file not found if not found. */
    flExsists = xsfirst(pathExp, WAttCode) ? -2 : 0;

    /* if wild cards were specified in file name and file exsists return 1 */
    flExsists = wildFnd ? (flExsists ? -2 : 1) : flExsists;

    /* If no wild cards/file name found check if directory. */
    if (!wildFnd) {
        /* if a file structure exsists... */
        if (!flExsists) {
            /* if it is a directory or a volume label... */
            if (srchb[21] & 0x18) {
                /* Set up dirLen to encompas entire path specification. */
                dirLen = pathLen;

                /*Tackon a path seperator. */
                pathExp[dirLen++] = '\\';

                /* copy wild card expression into spec'd path and file name. */
                for (i = 0; i <= wildLen; i++)

                    /* onto end of path expression. */
                    pathExp[i + dirLen] = wildExp[i];

                flExsists = -1;
            }
        }
    }
    /* copy path exp into directory expression. */
    for (pathLen = 0; pathLen < dirLen; pathLen++)
        if ((dirExp[pathLen] = pathExp[pathLen]) == '*')
            return -3;

    /* chop off file nm from dirExp. */
    dirExp[dirLen] = '\0';

    /* copy file name into return var. */
    i = 0;
    while ((filExp[i++] = pathExp[pathLen++]));

    return (flExsists);
}


/*
 *  int chkDst (void);
 *
 *  chkDst - Checks dst file name for validity. If there are any wild cards in
 *     the source file name the only valid dst names are "*", or "*.*".  Any
 *     thing else results in an error.
 *
 *  returns 0 if no error
 *      -1 if unable to make dst file name
 *
 */

static int
chkDst(void)
{
    int i = 0;
    char c;

    /* check for proper use of wild chards. */
    while ((c = srcNmPat[i++])) {
        /* Look for wild card chars. */
        switch (c) {
        case '*':
        case '?':

doDstChk:
            /* If dst file name longer than wild exp, must be error. */
            if (strlen(dstNmPat) > strlen(wildExp))
                return -1;

            /* Loop till end of dst fil nam to see if it matches wild exp. */
            for (i = 0; (c = dstNmPat[i]); i++)
                if (c != wildExp[i])
                    return -1;

            /* return ok. */
            return (0);

        default:
            break;
        }
    }

    /* if any wild cards in dst, check for validity. */
    for (i = 0; (c = dstNmPat[i]); i++)
        if ((c == '*') || (c == '?'))
            goto doDstChk;

    /* return ok. */
    return 0;
}


/*
 *  int mkDst - Make dst file name.
 *
 *  returns 0 if dst other than src
 *  returns -1 if dst same as src
 *
 *  srcFlNm - ptr to string from search first on path name
 *  dstFlNm - ptr to string that will recieve destination file name
 *  srcDir  - ptr to src dir path
 *  dstDir  - ptr to dst dir path
 *  srcNmPat- ptr to string that contains the file name specified in path
 *  dstNmPat- ptr to string that contains the dst pattern
 *
 */

static int
mkDst(void)
{
    int i, k, ndx;
    int srcEqDst;

    i = ndx = 0;

    /* determine If dst dir path = src dir path. */
    while ((srcEqDst = (srcDir[i] == dstDir[i])) && srcDir[i]
           && (dstFlNm[i] = dstDir[i]))
        i++;

    /* if they do... */
    if (srcEqDst) {
        i = 0;

        /* if the dst is not a wild card (in which case auto match)... */
        if (!(srcEqDst = (dstNmPat[0] == '*')))

            /* loop, chk each src=dst file.ext for match, setting srcEqDst. */
            while ((srcEqDst = (srchb[30 + i] == dstNmPat[i]))
                   && srchb[30 + i] && dstNmPat[i])
                i++;
    }

    /* if the entire name matches create a dst file name with '.&&&' as ext */
    if (srcEqDst) {
        i = 0;
        while ((dstFlNm[ndx] = dstDir[ndx]))
            ndx++;

        while ((dstFlNm[ndx + i] = srchb[30 + i])
               && (dstFlNm[ndx + i] != '.'))
            i++;
        i++;

        for (k = 0; k <= 2; dstFlNm[i + (k++)] = '&');
    }

    /* else file names do not match. */
    else {
        i = 0;
        ndx = 0;

        /* copy dst dir path into dst file name. */
        while ((dstFlNm[ndx] = dstDir[ndx]))
            ndx++;

        /* if dst file pat is wild card, copy src file name into dst file name. */
        if (dstNmPat[0] == '*')
            while ((dstFlNm[ndx + i] = srchb[30 + i]))
                i++;

        else
            /* copy dst name pat directly into dst file name. */
            while ((dstFlNm[ndx + i] = dstNmPat[i]))
                i++;
    }
    return (srcEqDst);
}



/*
 *  mkSrc - make source file name from directory path and file name.
 */

static void
mkSrc(void)
{
    int i, j = 0;

    /* copy src directroy into src directory file name. */
    for (i = 0; (srcFlNm[i] = srcDir[i]); i++);

    /* copy source file name from search first/next into src file name. */
    while ((srcFlNm[i + j] = srchb[30 + j]))
        j++;
}



/*
 *  wrt - write to standard output
 */

static void
wrt(const char *msg)
{
    xwrite(1, (long) strlen(msg), (char *)msg);
}

static void
wrtln(const char *msg)
{
    wrt("\r\n");
    wrt(msg);
}

static void
wrtch(char ch)
{
    char str[2];

    str[0] = ch;
    str[1] = 0;
    wrt(str);
}

#if NO_ROM
#define cookie_idt ( 0x100 | '/' )
#else
extern long cookie_idt;         /* in bios/machine.c */
#endif

static void
wrtDate(int j)
{
    int year = ((j >> 9) & 0x7F) + 80;
    int mon = ((j >> 5) & 0xF);
    int day = (j & 0x1F);
    char sep = cookie_idt & 0xFF;
    int a, b, c;

#if 1
    if (year >= 100)
        year -= 100;
#endif

    switch ((cookie_idt >> 8) & 3) {
    default:
    case 0:                     /* MMDDYY */
        a = mon;
        b = day;
        c = year;
        break;
    case 1:                     /* DDMMYY */
        a = day;
        b = mon;
        c = year;
        break;
    case 2:                     /* YYMMDD */
        a = year;
        b = mon;
        c = day;
        break;
    case 3:                     /* YYDDMM */
        a = year;
        b = day;
        c = mon;
        break;
    }
    prtDclFmt((long) a, 2, "0");
    wrtch(sep);
    prtDclFmt((long) b, 2, "0");
    wrtch(sep);
    prtDclFmt((long) c, 2, "0");
}

static void
wrtTime(int j)
{
    int hour = (j >> 11) & 0x1F;
    int min = (j >> 5) & 0x3F;
    int sec = (j & 0x1F) << 1;

    prtDclFmt((long) hour, 2, "0");
    wrt(":");
    prtDclFmt((long) min, 2, "0");
    wrt(":");
    prtDclFmt((long) sec, 2, "0");
}


#if NO_ROM
/* BUILDDATE is needed only for non-ROM command.prg */
static void
wrtbdate(void)
{
    wrt(BUILDDATE);
}
#else
extern int os_dosdate;          /* in bios/startup.S */
static void
wrtbdate(void)
{
    wrtDate(os_dosdate);
}
#endif


/*
 *  dspDir - display directory
 */

static void
dspDir(char *p, char *dir)
{
    int i, j;
    char ch, tmpbuf[100];

    if ((*p) && (p[1] == ':')) {
        ch = *p;
        i = p[0] - 'A';
        j = 2;
    } else {
        ch = (i = xgetdrv()) + 'A';
        j = 0;
    }
    xwrite(1, 1L, &ch);
    wrt(":");
    if (!dir[j]) {
        xgetdir(&tmpbuf, i + 1);
        if (tmpbuf[0] == 0) {
            tmpbuf[0] = '\\';
            tmpbuf[1] = 0;
        }
        wrt(tmpbuf);
    } else
        wrt(dir + j);
}



/*
 *  cr2cont - wait for carriage return before continuing.
 */

static void
cr2cont(void)
{
    wrt(_("CR to continue..."));
    lin[0] = 126;
    jmp_gemdos(10, &lin[0]);
}



/*
 *  dspMsg - display message
 */

static void
dspMsg(int msg)
{
    switch (msg) {
    case 0:
        wrtln(_("Wild cards not allowed in path name."));
        break;
    case 1:
        wrtln(_("File Not Found."));
        break;
    case 2:
        wrtln(_("Destination is not a valid wild card expression."));
        break;
    case 3:
        wrtln("******* TEST  CLI *******");
        break;
    case 4:
        wrtln(_("EmuCON - Compiled on "));
        wrtbdate();
        wrtln(_("Type HELP for a list of commands."));
        wrtln("");
        break;
    case 5:
        wrt(_("Done."));
        break;
    case 6:
        wrtln(_("Command is incompletely specified."));
        break;
    case 7:
        wrt(srcFlNm);
        break;
    case 8:
        wrt(dstFlNm);
        break;
    case 9:
        wrtln(".");
        break;
    case 10:
        wrt(_(" to "));
        break;
    case 11:
        break;
    case 12:
        wrtln("");
        break;
    case 13:
        wrtln("");
    case 14:
        drvch = (drv = xgetdrv()) + 'a';
        xwrite(1, 1L, &drvch);
        wrt(":");
        wrt(" ");
        break;
    case 15:
        wrtln(_("Wild cards not allowed in destination."));
        break;
    case 16:
        drvch = (drv = xgetdrv()) + 'a';
        xwrite(1, 1L, &drvch);
        wrt(":");
        break;
    case 17:
        wrtln(_("\
# in the first non blank column is a comment."));
        wrtln(_("\
CAT or TYPE filenm.ext\r\n\
\tWrites filenm.ext to standard output."));
        wrtln(_("\
CD [pathnm]\r\n\
\tWith pathnm it sets default for working directory.\r\n\
\tWithout pathnm displays current working directory."));
        wrtln(_("\
CHMOD [pathnm/]filenm mode\r\n\
\tChanges the mode of the file specified in filenm to the\r\n\
\tvalue of mode.\tAcceptable values are < 7:\r\n\
\t\t0 - Normal File Entry\r\n\
\t\t1 - File is Read Only\r\n\
\t\t2 - File is Hidden from directory Search\r\n\
\t\t4 - File is System File"));
        wrtln("CLS");
        wrtln(_("\tClears the screen."));
        wrtln(_("\
COPY source_file [destination_file]\r\n\
\tCopies source to destination."));
        wrtln(_("\
DIR or LS [filenm.ext] [-f] [-d] [-t] [-w]\r\n\
\t-f - anything but directories.\r\n\
\t-d - directories only.\r\n\
\t-t - terse: names only.\r\n\
\t-w - wide: names only displayed horizontally."));
        wrtln("");
        cr2cont();
        wrtln("ERR ");
        wrtln(_("\
\tDisplays the value of the Completion Code for the last command."));

        wrtln("EXIT");
        wrtln(_("\tExits the CLI."));

        wrtln(_("\
INIT [drive_spec:]\r\n\
\tReinitializes FAT entries thus wiping the disk."));
        wrtln(_("\
MD [subdirectory name]\r\n\
\tCreates a new subdirectory in current directory."));
        wrtln(_("\
MOVE source_file [destination_file]\r\n\
\tCopies source to destination and deletes source."));
        wrtln("PAUSE");
        wrtln(_("\
\tWrites 'CR to continue...' to standard output\r\n\
\tand waits for a carriage return from standard input."));
        wrtln("PRGERR [ON | OFF]");
        wrtln(_("\
\tTurns command processing abort feature ON/OFF.\r\n\
\tIf PRGERR is ON and a .PRG file returns a non zero completion\r\n\
\tcode, all further processing will stop.  Usefull in .BAT files.\r\n\
\tDefault is ON."));
        wrtln("NOWRAP");
        wrtln(_("\tDisables line wrap."));
        wrtln(_("\
PATH [;[pathnm]...]\r\n\
\tWith path name sets default path for batch and commands.\r\n\
\tWithout path name displays current path"));
        wrtln("");
        cr2cont();
        wrtln(_("\
REM or ECHO [\"string\"]\r\n\
\tStrips quotes and writes string to standard output.\r\n\
\t/r is replaced by 13, /n by 10, /0 by 0x0.\r\n\
\t/c by 13 10, /anything is replaced by anything."));
        wrtln(_("\
REN source_file_nm [destination_file_nm]\r\n\
\tRenames source to destination."));
        wrtln(_("\
RD [pathnm]\r\n\
\tRemoves named directory."));
        wrtln(_("\
RM or DEL or ERA filenm [[filemn]...] [-q]\r\n\
\tRemoves named file from directory.\r\n\
\tIf the -q option is used, the CLI will display the question\r\n\
\tY/CR... and wait for a response."));
        wrtln(_("\
SHOW [drive_spec:]\r\n\
\tDisplays disk status for default drive or drive specified."));
        wrtln("VERSION");
        wrtln(_("\tDisplays current version of OS."));
        wrtln("WRAP");
        wrtln(_("\tEnables line wrap."));
        wrtln("");
        cr2cont();
        break;
    }
}



/*
 * getYes
 */

static int
getYes(void)
{
    char inpStr[30];

    inpStr[0] = xrdchne();
    inpStr[1] = 0;
    ucase(&inpStr[0]);
    if (inpStr[0] == 'Y')
        return -1;
    return 0;
}



/*
 *  copyCmd - copy file.
 *
 *  returns 0 if copied ok
 *     -1 if copy failed
 */
static int
copyCmd(char *src, char *dst, int move)
{
    int i, srcEqDst, fds, fdd;
    long nr, nw;
    char srcSpc[67];
    char dstSpc[67];

    for (i = 0; (srcSpc[i] = src[i]); i++);
    for (i = 0; (dstSpc[i] = dst[i]); i++);

    WSrcReq = (char *) &srcSpc;
    WAttCode = -1;

    compl_code = -1;

    /*If not a valid file name... */
    switch (chkDir((char *) &srcSpc, srcDir, srcNmPat)) {
    case -3:
        goto error5;
    case -2:
        goto error6;
    default:
        {
            /* Check destination directory. */
            if (chkDir((char *) &dstSpc, dstDir, dstNmPat) == -3)
                goto error5;

            else {
                if (chkDst())
                    dspMsg(2);

                else {
                    while (gtFlNm()) {
                        if (!(srchb[21] & 0x18)) {
                            mkSrc();
                            if (!(srcEqDst = mkDst())) {
                                xunlink(dstFlNm);
                                dspMsg(12);
                                dspMsg(7);
                                dspMsg(10);
                                dspMsg(8);
                                if ((fds = xopen(srcFlNm, 0)) <= 0)
                                    goto error0;
                                if ((fdd = xcreat(dstFlNm, 0x20)) <= 0)
                                    goto error1;
                                compl_code = 0;
                                nr = nw = -1;
                                while ((nr) && (nw)) {
                                    if (
                                        (nr =
                                         xread(fds, (long) BUFSIZ,
                                               buf)) > 0) {
                                        if ((nw = xwrite(fdd, nr, buf)) <
                                            nr) {
                                            /* kprintf("nw = %ld\n", nw); */

                                            goto error4;
                                        }
                                    } else if (nr < 0) {
                                        goto error3;
                                    }
                                }
                                xclose(fds);
                                if (move) {
                                    xunlink(srcFlNm);
                                    wrt(_(" DELETING "));
                                    dspMsg(7);
                                }
                                xclose(fdd);
                            } else {
                                goto error2;
                            }
                        }
                    }
                    dspMsg(12);
                    dspMsg(5);
                }
            }
        }
    }
    return (0);

  error0:dspMsg(1);
    return (-1);
  error1:wrtln(_("Error creating file."));
    return (-1);
  error2:wrtln(_("Cannot copy "));
    dspMsg(7);
    wrt(_(" to itself."));
    return (-1);
  error3:wrtln(_("Error reading source file."));
    goto eout;
  error4:wrtln(_("Disk full -- copy failed."));
    goto eout;
  error5:dspMsg(0);
    return (-1);
  error6:dspMsg(1);
    return (-1);

  eout:
    xunlink(dstFlNm);
    wrt(_(" DELETING "));
    wrt(dstFlNm);
    return -1;
}



/*
 *  renmCmd - rename command
 */

static long
renmCmd(char *src, char *dst)
{
    int i;
    char srcSpc[67];
    char dstSpc[67];

    for (i = 0; (srcSpc[i] = src[i]); i++);
    for (i = 0; (dstSpc[i] = dst[i]); i++);

    WSrcReq = (char *) &srcSpc;
    WAttCode = -1;

    /* Set up completion code to show failure */
    compl_code = -1;

    /* IF src not specified err out. */
    if (!(*src))
        dspMsg(6);

    /*If not a valid file name... */
    else
        switch (chkDir((char *) &srcSpc, srcDir, srcNmPat)) {
        case -3:
            dspMsg(0);
            break;
        case -2:
            dspMsg(1);
            break;
        default:
            {
                /* Check destination directory. */
                if (chkDir((char *) &dstSpc, dstDir, dstNmPat) == -3) {
                    if (
                        ((i = chkDir((char *) &srcSpc, srcDir, srcNmPat))
                         == -3) || (i == 1)) {
                        wrt(_("Rename ALL files matching "));
                        wrt(srcDir);
                        wrt(srcNmPat);
                        wrt(_(" (Y/CR)? "));
                        if (!getYes())
                            goto skprnm;
                        wrtln("");
                    }
                } else {
                    if (chkDst()) {
                        dspMsg(2);
                    } else {
                        while (gtFlNm()) {
                            if (!(srchb[21] & 0x18)) {
                                if (!mkDst());
                                {
                                    mkSrc();

                                    dspMsg(12);
                                    dspMsg(7);
                                    dspMsg(10);
                                    dspMsg(8);
                                    compl_code =
                                        xrename(0, srcFlNm, dstFlNm);
                                    if (compl_code < 0) {
                                        wrt(_("  Rename Unsucessfull!"));
                                    }
                                }
                            }
                        }
                        dspMsg(12);
                        dspMsg(5);
                    }
                }
            }
        }

  skprnm:
    return (compl_code);
}



static long
dirCmd(char *argv[])
{
    char srcSpc[67];
    int i, j, k, n, att, *dt, filOnly, dirOnly, terse, wide;
    long compl_code, *pl;

    wide = filOnly = dirOnly = terse = 0;
    i = 1;
    while (*argv[i]) {
        ucase(argv[i]);
        if (*argv[i] == '-') {
            switch (*(argv[i] + 1)) {
            case 'F':
                filOnly = -1;
                dirOnly = 0;
                break;
            case 'D':
                dirOnly = -1;
                filOnly = 0;
                break;
            case 'W':
                wide = -1;
            case 'T':
                terse = -1;
                break;
            default:
                break;
            }
            j = i;
            while (*(argv[j] = argv[j + 1]))
                j++;
        } else
            i++;
    }

    for (i = 0; (srcSpc[i] = argv[1][i]); i++);
    chkDir((char *) &srcSpc, srcDir, srcNmPat);

    if (!terse) {
        wrt(_("Directory of "));
        dspDir((char *) &srcSpc, srcDir);
        dspMsg(12);
    }

    WSrcReq = (char *) &srcSpc;
    WAttCode = -1;
    if (!gtFlNm()) {
        compl_code = -1;
        if (!terse)
            dspMsg(1);
    } else {
        compl_code = 0;
        k = 0;
        do {
            n = strlen(&srchb[30]);
            if ((dirOnly) && (srchb[21] != 0x10))
                goto skip;
            if ((filOnly) && (srchb[21] == 0x10))
                goto skip;
            if ((terse) &&
                ((xncmps(2, &srchb[30], "."))
                 || (xncmps(3, &srchb[30], ".."))))
                goto skip;

            if (wide) {
                wrt(&srchb[30]);
                if (k == 5) {
                    wrtln("");
                    k = 0;
                } else {
                    for (i = n; i < 13; i++)
                        wrt(" ");
                    k++;
                }
            } else
                wrtln(&srchb[30]);

            if (!terse) {
                for (i = n; i < 15; i++)
                    xwrite(1, 1L, " ");

                dt = (int *) &srchb[24];
                j = *dt;
                wrtDate(j);
                wrt("\t");

                dt = (int *) &srchb[22];
                j = *dt;
                wrtTime(j);
                wrt("\t");

                att = srchb[21];
                if (att < 0x10)
                    wrt("0");
                prthex(att);
                xwrite(1, 2L, "  ");

                pl = (long *) &srchb[26];
                prtDclFmt((long) *pl, 6, " ");
            }
          skip: ;
        }
        while (gtFlNm());
        wrtln("");
        if (!terse)
            dspMsg(5);
    }
    return (compl_code);
}



static long
pwdCmd(char *argv[])
{
    int i, j;
    char ch, tmpbuf[100];

    i = xgetdrv();
    ch = i + 'A';
    j = 0;

    xwrite(1, 1L, &ch);
    wrt(":");

    xgetdir(&tmpbuf, i + 1);
    if (tmpbuf[0] == 0) {
        tmpbuf[0] = '\\';
        tmpbuf[1] = 0;
    }
    wrt(tmpbuf);

    dspMsg(12);
    return (compl_code);
}



static int
mknum(char *str)
{
    int num, hex = 0;
    char ch;

    ucase(str);
    if (*str == 'X') {
        hex = 1;
        str++;
    }

    num = 0;
    while ((ch = *str++)) {
        if (hex) {
            num *= 16;
            if (ch > 9)
                num += (ch - 'A' + 10);
            else
                num += ch - '0';
        } else {
            num *= 10;
            num += ch - '0';
        }
    }
    return num;
}



static long
chmodCmd(char *argv[])
{
    char srcSpc[67];
    int i, att;
    long compl_code;

    for (i = 0; (srcSpc[i] = argv[1][i]); i++);

    chkDir((char *) &srcSpc, srcDir, srcNmPat);
    WSrcReq = (char *) &srcSpc;
    WAttCode = -1;
    if (!gtFlNm()) {
        compl_code = -1;
        dspMsg(1);
    } else {
        mkSrc();
        att = srchb[21];
        if (att & 0x18) {
            wrt(_("Unable to change mode on subdirectorys or volumes."));
            compl_code = -1;
        } else {
            compl_code = 0;
            do {
                if (!*argv[2]) {
                    wrt(_("Invalid mode specification."));
                    compl_code = -1;
                } else {
                    i = mknum(argv[2]);
                    if (i & ~0x7) {
                        wrt(_("Invalid mode specification."));
                        compl_code = -1;
                    } else
                        compl_code = xattrib(srcFlNm, 1, i);
                }
            }
            while (gtFlNm());
        }
        if (!compl_code) {
            dspMsg(5);
        }
    }
    return (compl_code);
}



static long
typeCmd(char *argv[])
{
    char srcSpc[67];
    int i, n, fd;
    long compl_code;

    compl_code = 0;
    if (!(*argv[1]))
        dspMsg(6);
    else {
        for (i = 0; (srcSpc[i] = argv[1][i]); i++);
        chkDir((char *) &srcSpc, srcDir, srcNmPat);
        WSrcReq = (char *) &srcSpc;
        WAttCode = -1;
        if (!gtFlNm()) {
            dspMsg(1);
            compl_code = -1;
        } else {
            compl_code = 0;
            do {
                mkSrc();
                fd = xopen(srcFlNm, 0);
                do {
                    n = xread(fd, 1000L, buf);
                    if (n > 0)
                        xwrite(1, (long) n, buf);
                }
                while (n > 0);
            }
            while (gtFlNm());
            xclose(fd);
        }
    }
    return (compl_code);
}



static long
delCmd(char *argv[])
{
    char srcSpc[67];
    int i, j, k, query;
    long compl_code;

    compl_code = 0;
    query = 0;
    i = 1;
    while (*argv[i]) {
        ucase(argv[i]);
        if (*argv[i] == '-') {
            switch (*(argv[i] + 1)) {
            case 'Q':
                query = -1;
                break;
            default:
                break;
            }
            j = i;
            while (*(argv[j] = argv[j + 1]))
                j++;
        } else
            i++;
    }

    if (*argv[1]) {
        k = 1;
        while (*argv[k]) {
            for (i = 0; (srcSpc[i] = argv[k][i]); i++);
            k++;
            WSrcReq = (char *) &srcSpc;
            WAttCode = -1;
            if (((i = chkDir((char *) &srcSpc, srcDir, srcNmPat)) == -3)
                || (i == 1)) {
                wrt(_("Delete ALL files matching "));
                wrt(srcDir);
                wrt(srcNmPat);
                wrt(_(" (Y/CR)? "));
                if (!getYes())
                    goto noera;
                wrtln("");
            }
            if (!gtFlNm()) {
                compl_code = -1;
                dspMsg(1);
                wrtln("");
            } else {
                do {
                    if (!(srchb[21] & 0x18)) {
                        mkSrc();
                        dspMsg(7);
                        if (query) {
                            wrt("? ");
                            i = getYes();
                            wrt("\b\b ");
                            if (i)
                                wrt(_(" << DELETED"));
                            else
                                goto skipdel;
                        }
                        compl_code = xunlink(srcFlNm);
                      skipdel:
                        dspMsg(12);
                    }
                }
                while (gtFlNm());
            }
        }
        dspMsg(5);
    } else
        dspMsg(6);

  noera:
    return (compl_code);
}



/*
 *  dspCL - display command line
 */
static void
dspCL(char *argv[])
{
    int i;
    dspMsg(14);
    i = 0;
    while (*argv[i]) {
        wrt(argv[i++]);
        wrt(" ");
    }
    dspMsg(12);
}



/*
 *  setPath - set execution path
 */
static void
setPath(char *p)
{
    int i = 0;

    if (!*p)
        wrt((char *) &path);
    else if (xncmps(2, p, ";"))
        path[i] = 0;
    else
        while ((path[i] = p[i++]));
}


/*
 * execPrgm - execute program;
 */
static long
execPrgm(char *s, char *cmdtl)
{
    char cmd[100], ch, *cmdptr;
    int k, i, j, gtpath, envLen;
    int tlNtFnd = -1;
    long err;
    char *envPtr;

    /* Add len of path definition + 2 for 00 terminator */
    i = strlen(path);
    envLen = i + (i ? 5 : 0) + 2;

    /* Loop thru environment strings looking for '00' */
    i = 0;
    while ((prntEnvPtr[i] | prntEnvPtr[i + 1]) != 0) {
        /* if a path has been defined, don't count it. */
        if (xncmps(5, &prntEnvPtr[i], "PATH=")) {
            envLen--;
            i += strlen(&prntEnvPtr[i]);
        } else {
            envLen++;
            i++;
        }
    }

    /* Allocate envLen number of bytes for environment strings. */
    envPtr = (char *) xmalloc((long) envLen);

    /* copy path string into env. */
    i = 0;
    if (path[0]) {
        for (i = 0; pthSymb[i]; i++)
            envPtr[i] = pthSymb[i];
        j = 0;
        while ((envPtr[i] = path[j++]))
            i++;
        envPtr[i++] = 0;
    }

    /* Copy parents environment string into childs. */
    envLen = 0;
#if 0
    while ((envPtr[i] = prntEnvPtr[envLen]) | prntEnvPtr[envLen + 1]) {
        /* if a path has been defined, don't copy it. */
        if (xncmps(5, &prntEnvPtr[envLen], "PATH="))
            envLen += (1 + strlen(&prntEnvPtr[envLen]));
        else {
            i++;
            envLen++;
        }
    }
#endif
    /* inc index past 0. */
    i++;

    /* Null terminate. */
    envPtr[i] = 0;

    for (i = 0; (cmd[i] = *s); s++, i++)
        if (*s == '.')
            tlNtFnd = 0;

    /* put ".prg" at the end of command, if not there */
    if (tlNtFnd)
        for (j = 0; (cmd[i] = prgTail[j]); i++, j++);

    /* see, if absolute PATH is given */
    i = 0;
    gtpath = -1;
    while ((ch = cmd[i++]))
        if ((ch == ':') || (ch == '\\'))
            gtpath = 0;

    exeflg = 1;

    rm_term();

    cmdptr = (char *) &cmd;
    j = 0;
    Cursconf(0, 0);             /* XBIOS switch cursor off before command */

    /* Loop through semicolon seperated command line */
    while (((err = xexec(0, cmdptr, cmdtl, envPtr)) == -33) && (gtpath)) {
        k = j;
        if (path[j]) {
            while ((path[j]) && (path[j] != ';'))
                j++;
            for (i = 0; k < j; k++, i++)
                buf[i] = path[k];
            if (buf[i - 1] != '\\')
                buf[i++] = '\\';
            k = 0;
            while (cmd[k])
                buf[i++] = cmd[k++];
            buf[i] = 0;
            cmdptr = &buf[0];
            if (!(path[j]))
                gtpath = 0;
            else
                j++;
        } else
            gtpath = 0;
    }

    in_term();

    exeflg = 0;
    xmfree(envPtr);
    Cursconf(1, 0);             /* XBIOS switch cursor on after command */

    return (err);
}



/*
 *  execBat - execute batch file
 */
static int
execBat(char *s, char *parms[])
{
    long flHnd;
    int i, j, k, gtpath;
    int tlNtFnd = -1;
    char ch, cmd[100], *cmdptr;

    for (i = 0; (cmd[i] = *s); s++, i++)
        if (*s == '.')
            tlNtFnd = 0;

    if (tlNtFnd)
        for (j = 0; (cmd[i] = batTail[j]); i++, j++);

    if (xncmps(3, &cmd[i - 3], "BAT")) {
        i = 0;
        gtpath = -1;
        while ((ch = cmd[i++]))
            if ((ch == ':') || (ch == '\\'))
                gtpath = 0;

        cmdptr = (char *) &cmd;
        j = 0;
        while (((flHnd = xopen(cmdptr, 0)) <= 0) && (gtpath)) {
            k = j;
            if (path[j]) {
                while ((path[j]) && (path[j] != ';'))
                    j++;
                for (i = 0; k < j; k++, i++)
                    buf[i] = path[k];
                if (buf[i - 1] != '\\')
                    buf[i++] = '\\';
                k = 0;
                while (cmd[k])
                    buf[i++] = cmd[k++];
                buf[i] = 0;
                cmdptr = &buf[0];
                if (!(path[j]))
                    gtpath = 0;
                else
                    j++;
            } else
                gtpath = 0;
        }

        if (flHnd >= 0) {
            i = 0;
            xCmdLn(parms, &i, &flHnd, (char *) 0L);
            xclose((int) flHnd);
            compl_code = 0;
            return -1;
        }
    }

    compl_code = -1;
    return 0;
}



static void
chk_sub(char *tl, char **parm)
{
    char ch, tmptl[167], *tmptl_ptr, *tmp_front, *tl_front, *parm_ptr;

    tmptl_ptr = (char *) &tmptl;
    tmp_front = tmptl_ptr;
    tl_front = tl;

    while ((ch = *tl++)) {
        switch (ch) {

        case '/':
            if (*tl == '%') {
                *tmptl_ptr++ = ch;
                *tmptl_ptr++ = *tl++;
            } else {
                *tmptl_ptr = ch;
                tmptl_ptr++;
            }
            break;

        case '%':
            if (*(parm_ptr = parm[*tl++ - '0'])) {
                while ((*tmptl_ptr = *parm_ptr++))
                    tmptl_ptr++;
            }
            break;

        default:
            *tmptl_ptr++ = ch;
            break;
        }
    }
    *tmptl_ptr = 0;

    while ((*tl_front = *tmp_front++))
        tl_front++;
}



static void
chk_str(char *parm[])
{
    int i = 0;
    char *parm_ptr, *tmp_ptr, ch;

    while (*parm[i]) {
        if (*parm[i] == '"') {
            parm[i]++;
            if (*(parm[i] + strlen(parm[i]) - 1) == '"')
                *(parm[i] + strlen(parm[i]) - 1) = 0;

            parm_ptr = parm[i];
            while ((ch = *parm_ptr)) {
                if (ch == '/') {
                    switch (ch = *(parm_ptr + 1)) {
                    case 'c':
                        *parm_ptr++ = 13;
                        *parm_ptr = 10;
                        goto skip;
                    case 'r':
                        *parm_ptr = 13;
                        break;
                    case 'n':
                        *parm_ptr = 10;
                        break;
                    case '0':
                        *parm_ptr = 0;
                        break;
                    default:
                        *parm_ptr = ch;
                        break;
                    }
                    parm_ptr++;
                    tmp_ptr = parm_ptr;
                    while ((*tmp_ptr = *(tmp_ptr + 1)))
                        tmp_ptr++;
                  skip: ;
                }

                else {
                    parm_ptr++;
                }
            }
        }
        i++;
    }
}



/*
 *  readSi - read standard input
 */

static int
readSi(char *lin)
{
    int i, j;

    dspMsg(13);
    for (i = 1; i <= 125; lin[i++] = 0);
    i = j = 0;

    lin[0] = 126;
    jmp_gemdos(10, &lin[0]);

    lin[lin[1] + 2] = 0;

    i = 2;
    while (lin[i]) {
        if ((lin[i] >= ' ') && (lin[i] <= '~'))
            lin[j++] = lin[i];
        i++;
    }

    lin[j] = 0;
    lin[j + 1] = 0;
    return (j);
}



/*
 *  readDsk - read from disk file
 */

static int
readDsk(char *lin, long *flHnd)
{
    int i, j;
    int chrFnd;
    char ch;

    for (i = 1; i <= 125; lin[i++] = 0);
    i = j = 0;

    while ((chrFnd = xread((int) *flHnd, 1L, &ch) > 0) && (ch != '\r'))
        lin[j++] = ch;

    j = 0;
    i = 0;

    while (lin[i]) {
        if ((lin[i] >= ' ') && (lin[i] <= '~'))
            lin[j++] = lin[i];
        i++;
    }

    lin[j] = 0;

    return (j ? j : (chrFnd ? -1 : 0));
}



/*
 *  xCmdLn - execute command line.
 */

static void
xCmdLn(char *parm[], int *pipeflg, long *nonStdIn, char *outsd_tl)
{
    int pipe, bdChrs;
    int fs, fd;
    int i, j, k, argc, f1, f2, nd, rec;
    int concat;
    long newso, newsi;
    long sbuf[4];
    long dskFlHnd;
    char ch, *cmdtl, *tl0, *tl1, *tl, *d, *s, *argv[MAXARGS];
    char *p, ltail[130];

    struct rdb rd;

    BPB *b;

    rd.nso = 0;
    rd.nsi = 0;
    rd_ptr = &rd;

    bdChrs = 0;

    /* while there is input from the disk or standard input */
    while ((long) outsd_tl ? 1
           : (*nonStdIn ? (bdChrs = readDsk((char *) &lin, nonStdIn)) :
              readSi((char *) &lin))) {
        /*Garbage chars in disk file. */
        if ((bdChrs == -1) && *nonStdIn)
            goto again;

        exeflg = 0;             /* not in an exec */
        wrtln("");
        d = &ltail[0];
        argv[0] = d;
        argc = 0;
        concat = 0;
        pipe = 0;
        dskFlHnd = 0;

        /*Set up for input redirection. */
        if (*pipeflg) {
            argv[0] = parm[0];
            argv[1] = d;
            argc = 1;
        }

        /* find command tail */
        if ((long) outsd_tl) {
            tl = outsd_tl + 1;
            /* LVL: cast to avoid "subscript has type `char'" warning */
            tl[(unsigned int) outsd_tl[0]] = 0;
        } else
            tl = &lin[0];

        while (*tl == 0x20)
            tl++;
        chk_sub(tl, &parm[0]);

        /* allow remarks in batch files. */
        if (*tl == '#')
            goto again;
        while ((ch = (*tl++)))
            switch (ch) {
            case ' ':
                *d++ = 0;
                while (*tl == 0x20)
                    tl++;
                argv[++argc] = d;
                break;

            case '"':
                *d++ = ch;
                while ((*tl) && ((*d++ = *tl++) != '"'));
                break;

            case '@':
                for (tl0 = tl; (ch = *tl); tl++) {
                    if (ch == 0x20)
                        break;
                }
                *tl++ = 0;

                if ((dskFlHnd = xopen(tl0, 0)) >= 0) {
                    pipe = -1;
                } else {
                    wrtln(tl0);
                    wrt(_(" not found."));
                }
                break;

            case '<':
                for (tl0 = tl; (ch = *tl); tl++) {
                    if (ch == 0x20)
                        break;
                }
                *tl++ = 0;

                if ((newsi = xopen(tl0, 0)) >= 0) {
                    rd.oldsi = dup(0);
                    xforce(0, (int) newsi);
                    xclose((int) newsi);
                    rd.nsi = -1;
                } else {
                    wrtln(tl0);
                    wrt(_(" not found."));
                }
                break;

            case '>':
                for (tl1 = tl; (ch = *tl); tl++) {
                    if (ch == '>') {
                        concat = -1;
                        tl1++;
                    }
                    if (ch == 0x20)
                        break;
                }
                *tl++ = 0;
                if (concat) {
                    if ((newso = xopen(tl1, 1)) < 0) {
                        newso = xcreat(tl1, 0);
                    }
                } else {
                    xunlink(tl1);
                    newso = xcreat(tl1, 0);
                }
                if (newso < 0) {
                    wrt("Can not open file.\r\n");
                    break;
                }
                if ((rd.oldso = dup(1)) < 0) {
                    /*wrt("Can not dup stdout.\r\n");*/
                    rd.oldso = -1;  /* fake old handle = console */
                }
                if (xforce(1, (int)newso) < 0) {
                    wrt("Can not force stdout to new channel.\r\n");
                    xclose(newso);
                    break;
                }
                xclose((int)newso);
                if (concat)
                    xf_seek(0L, 1, 2);
                rd.nso = -1;
                break;
            default:
                *d++ = ch;
            }

        /* If pipe tack on remaining parms if any. */
        if (*pipeflg) {
            i = 1;
            while (*parm[i]) {
                argv[++argc] = parm[i++];
            }
        }

        *d++ = 0;
        *d = 0;
        i = argc;
        argv[++i] = d;

        s = argv[0];
        p = argv[1];
        ucase(s);

        /*Build command tail. */
        cmdtl = lin;
        j = 1;
        i = 1;
        while (*argv[i]) {
            k = 0;
            while ((cmdtl[++j] = *(argv[i] + k++)));
            cmdtl[j] = ' ';
            i++;
        }
        cmdtl[j] = 0xd;
        cmdtl[j + 1] = 0;
        cmdtl[0] = --j;
        cmdtl[1] = ' ';

        if (pipe) {
            xCmdLn((char **) &argv, &pipe, &dskFlHnd, (char *) 0L);
            xclose((int) dskFlHnd);
        } else {
            if ((strlen(s) == 2) && (s[1] == ':')) {
                xsetdrv(drv = (*s - 'A'));
            } else if (xncmps(3, s, "LS") || xncmps(4, s, "DIR")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                compl_code = dirCmd(&argv[0]);
            } else if (xncmps(4, s, "PWD")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                compl_code = pwdCmd(&argv[0]);
            } else if (xncmps(6, s, "CHMOD")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                compl_code = chmodCmd(&argv[0]);
            } else if (xncmps(4, s, "ERR")) {
                wrt(_("Completion code for previous command = "));
                prthex((int) compl_code);
            } else if (xncmps(5, s, "PATH")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                ucase(p);
                setPath(p);
            } else if (xncmps(4, s, "ENV")) {
                i = 0;
                while ((prntEnvPtr[i] + prntEnvPtr[i + 1]) != 0) {
                    /* if a path has been defined, don't count it. */
                    if (!(xncmps(5, &prntEnvPtr[i], "PATH="))) {
                        wrtln(&prntEnvPtr[i]);
                    }
                    i += strlen(&prntEnvPtr[i]);
                    if (prntEnvPtr[i] + prntEnvPtr[i + 1] == 0)
                        break;
                    i += 1;
                }
                if (path[0]) {
                    wrtln("PATH=");
                    wrt(path);
                }
            }

            else if (xncmps(4, s, "CAT") || xncmps(5, s, "TYPE")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                compl_code = typeCmd(&argv[0]);
            } else if ((xncmps(4, s, "REM")) || (xncmps(5, s, "ECHO"))) {
                chk_str(&argv[1]);
                i = 1;
                while (*argv[i]) {
                    if (i > 1)
                        wrt(" ");
                    wrt(argv[i++]);
                }
                if (!*nonStdIn)             // newline only in interactive mode
                    dspMsg(12);     // aug 2005 RCL
            }

            else if (xncmps(3, s, "CD")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                if (argc == 0) {
                    xgetdir(buf, drv + 1);
                    if (!buf[0]) {
                        buf[0] = '\\';
                        buf[1] = 0;
                    }
                    xwrite(1, (long) strlen(buf), buf);
                } else {
                    if ((compl_code = xchdir(p)) != 0)
                        wrt(_("Directory not found."));
                }
            }

            else if (xncmps(7, s, "CMDERR")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                if (argc == 0) {
                    if (cmderr)
                        wrt("ON");
                    else
                        wrt("OFF");
                } else {
                    ucase(p);
                    if (xncmps(3, p, "ON"))
                        cmderr = -1;
                    else if (xncmps(4, p, "OFF"))
                        cmderr = 0;
                    else
                        wrt(_("Arg must be ON or OFF."));
                }
            }

            else if (xncmps(7, s, "PRGERR")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                if (argc == 0) {
                    if (prgerr)
                        wrt("ON");
                    else
                        wrt("OFF");
                } else {
                    ucase(p);
                    if (xncmps(3, p, "ON"))
                        prgerr = -1;
                    else if (xncmps(4, p, "OFF"))
                        prgerr = 0;
                    else
                        wrt(_("Arg must be ON or OFF."));
                }
            }

            else if (xncmps(3, s, "MD") || xncmps(6, s, "MKDIR")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                if ((compl_code = xmkdir(p)) != 0)
                    wrt(_("Unable to make directory"));
            } else if (xncmps(3, s, "RD") || xncmps(6, s, "RMDIR")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                if ((compl_code = xrmdir(p)) != 0)
                    wrt(_("Unable to remove directory"));
            }
                else if (xncmps(3, s, "RM") || xncmps(4, s, "DEL") ||
                         xncmps(4, s, "ERA")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                compl_code = delCmd(&argv[0]);
            }

            else if (xncmps(4, s, "REN")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                compl_code = renmCmd(argv[1], argv[2]);
            } else if (xncmps(5, s, "SHOW")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                ucase(p);
                if (xgetfree(sbuf, (*p ? *p - 64 : 0)) == 0) {
                    wrt(_("Allocation Information: Drive "));
                    if (!*p)
                        dspMsg(16);
                    else
                        wrt(p);
                    dspMsg(12);
                    wrtln(_("Drive size in BYTES    "));
                    prtDclFmt((long) (sbuf[1] * sbuf[3] * sbuf[2]), 8, " ");
                    wrtln(_("BYTES used on drive    "));
                    prtDclFmt((long) ((sbuf[1] - sbuf[0]) * sbuf[3] * sbuf[2]),
                              8, " ");
                    wrtln(_("BYTES left on drive    "));
                    prtDclFmt((long) (sbuf[0] * sbuf[3] * sbuf[2]), 8, " ");
                    wrtln(_("Total Units on Drive   "));
                    prtDclFmt((long) sbuf[1], 8, " ");
                    wrtln(_("Free Units on Drive    "));
                    prtDclFmt((long) sbuf[0], 8, " ");
                    wrtln(_("Sectors per Unit   "));
                    prtDclFmt((long) sbuf[3], 8, " ");
                    wrtln(_("Bytes per Sector   "));
                    prtDclFmt((long) sbuf[2], 8, " ");
                }
                else {
                    wrtln("No information available for this drive.");
                }
            }

            else if (xncmps(5, s, "INIT") || xncmps(7, s, "FORMAT")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                for (i = 0; i < BUFSIZ; i++)
                    buf[i] = 0;
                ucase(p);
                drv = *p - 'A';
                buf[0] = 0xf7;
                buf[1] = 0xff;
                buf[2] = 0xff;
                b = (BPB *) getbpb(drv);
                if (b->b_flags & 1)
                    buf[3] = 0xFF;
                f1 = b->fatrec - b->fsiz;
                f2 = b->fatrec;
                fs = b->fsiz;
                rwabs(1, buf, fs, f1, drv);
                rwabs(1, buf, fs, f2, drv);
                nd = b->recsiz / 32;
                d = buf;
                for (i = 0; i < nd; i++) {
                    *d++ = 0;
                    for (j = 0; j < 31; j++)
                        *d++ = 0;       /*formerly f6 */
                }
                rec = f2 + fs;
                for (i = 0; i < b->rdlen; i++, rec++)
                    rwabs(1, buf, 1, rec, drv);
                dspMsg(5);
            }

            else if (xncmps(8, s, "GETBOOT")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                ucase(p);
                drv = *p - 'A';
                rwabs(0, &buf[28], 1, 0, drv);
                compl_code = xcreat(argv[2], 0);
                if (compl_code >= 0) {
                    fd = xopen(argv[2], 1);
                    xwrite(fd, 540L, buf);
                    xclose(fd);
                } else
                    wrt(_("Unable to create bootsector file"));

                dspMsg(5);
            }

            else if (xncmps(8, s, "PUTBOOT")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                ucase(p);
                drv = *p - 'A';
                fd = xopen(argv[2], 0);
                if (fd >= 0) {
                    xread(fd, 540L, buf);
                    xclose(fd);
                    rwabs(1, &buf[28], 1, 0, drv);
                } else {
                    compl_code=fd;
                    wrt(_("Unable to read bootsector file"));
                }
                dspMsg(5);
            }

            else if (xncmps(5, s, "COPY") || xncmps(5, s, "MOVE") ||
                     xncmps(3, s, "CP") || xncmps(3, s, "MV")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                if (argc >= 1) {
                    compl_code =
                        copyCmd(p, argv[2], xncmps(5, s, "MOVE") ? 1 : 0);
                } else {
                    compl_code = -1;
                    dspMsg(6);
                }
            } else if (xncmps(6, s, "PAUSE")) {
                cr2cont();
            } else if (xncmps(5, s, "HELP"))
                dspMsg(17);
            else if (xncmps(5, s, "EXIT")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                xclose(rd.oldsi);
                xclose(rd.oldso);
                bExitFlag = 1;
                return;
            }
            else if (xncmps(8, s, "VERSION")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                i = jmp_gemdos(0x30);
                prtdecl((long) (i & 0xFF));
                xwrite(1, 1L, ".");
                prtdecl((long) ((i >> 8) & 0xFF));
            } else if (xncmps(5, s, "WRAP")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                xwrite(1, 2L, "\033v");
                dspMsg(5);
            } else if (xncmps(7, s, "NOWRAP")) {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                xwrite(1, 2L, "\033w");
                dspMsg(5);
            } else if (xncmps(4, s, "CLS") || xncmps(6, s, "CLEAR"))
                xwrite(1, 4L, "\033H\033J");
            else {
                if (*nonStdIn)
                    dspCL(&argv[0]);
                if (!(execBat(s, (char **) &argv))) {
                    if ((compl_code = execPrgm(s, cmdtl)) == -32)
                        errout();
                    else {
                        if ((compl_code > 0) && prgerr)
                            errout();
                        else if (compl_code < 0) {
                            wrt(_("Command not found."));
                            if (prgerr)
                                errout();
                        }
                    }
                }
            }
        }
        chk_redirect(&rd);

again:
        /* if command coming from outside the command int exit */
        if ((long) outsd_tl) {
            if (*nonStdIn)
                dspCL(&argv[0]);
            xclose(rd.oldsi);
            xclose(rd.oldso);
            bExitFlag = 1;
            return;
        }
    }
}

/*
 * cmain
 *
 * bp - Base page address
 */

void
cmain(char *bp)
{
    char *parm[MAXARGS];
    char *tl;
    int i, k, cmd;
    long j;

    basePage = bp;
    prntEnvPtr = *((char **) (basePage + 0x2C));
    tl = basePage + 0x80;
    if (tl[0])
        cmd = -1;
    else
        cmd = 0;

    xsetdta(srchb);
    path[0] = 0;
    compl_code = 0;
    prgerr = -1;
    cmderr = 0;
    if (!cmd)
        dspMsg(4);

    for (i=0; (prntEnvPtr[i] + prntEnvPtr[i + 1]) != 0; i++) {
        /* if a path has been defined, don't count it. */
        if (xncmps(5, &prntEnvPtr[i], "PATH=")) {
            setPath(&prntEnvPtr[i + 5]);
            break;
        }
    }
    if (!cmd)
        execBat((char *) &autoBat, &parm[0]);

    if (setjmp(jb)) {
        for (i = 6; i <= 20; i++)
            xclose(i);
        if (cmd) {
            tl[0] = 4;
            tl[1] = 'e';
            tl[2] = 'x';
            tl[3] = 'i';
            tl[4] = 't';
            tl[5] = 0xd;
            tl[6] = 0;
        }
    }

    bExitFlag = 0;

    Cursconf(1, 0);             /* XBIOS switch cursor on */

    do {
        k = 0;
        j = 0;
        xCmdLn(&parm[0], &k, &j, cmd ? tl : (char *) 0L);
    }
    while(!bExitFlag);

    wrtln("Leaving EmuCON...\r\n");
}

