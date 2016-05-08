/*
 * EmuCON2 header
 *
 * Copyright (c) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef STANDALONE_CONSOLE
 #include "config.h"
 #include <nls.h>
 #include <portab.h>
#else
 #define _(a) a
 #define N_(a) a
 #define gettext(a) a
 typedef short int       WORD;
 typedef unsigned short  UWORD;
 typedef long            LONG;
 typedef unsigned long   ULONG;
 #define MAXPATHLEN      256
#endif


/*
 * system calls
 */
extern LONG jmp_gemdos(WORD, ...);
extern LONG jmp_bios(WORD, ...);
extern LONG jmp_xbios(WORD, ...);

#define jmp_gemdos_v(a)         jmp_gemdos((WORD)(a))
#define jmp_gemdos_w(a,b)       jmp_gemdos((WORD)(a),(WORD)(b))
#define jmp_gemdos_l(a,b)       jmp_gemdos((WORD)(a),(LONG)(b))
#define jmp_gemdos_p(a,b)       jmp_gemdos((WORD)(a),(void*)(b))
#define jmp_gemdos_ww(a,b,c)    jmp_gemdos((WORD)(a),(WORD)(b),(WORD)(c))
#define jmp_gemdos_pw(a,b,c)    jmp_gemdos((WORD)(a),(void *)(b),(WORD)(c))
#define jmp_gemdos_wlp(a,b,c,d) jmp_gemdos((WORD)(a),(WORD)(b),(LONG)(c),(void *)(d))
#define jmp_gemdos_wpp(a,b,c,d) jmp_gemdos((WORD)(a),(WORD)(b),(void *)(c),(void *)(d))
#define jmp_gemdos_pww(a,b,c,d) jmp_gemdos((WORD)(a),(void *)(b),(WORD)(c),(WORD)(d))
#define jmp_gemdos_wppp(a,b,c,d,e)  jmp_gemdos((WORD)(a),(WORD)(b),(void *)(c),(void *)(d),(void *)(e))
#define jmp_bios_w(a,b)         jmp_bios((WORD)(a),(WORD)(b))
#define jmp_bios_ww(a,b,c)      jmp_bios((WORD)(a),(WORD)(b),(WORD)(c))
#define jmp_xbios_l(a,b)        jmp_xbios((WORD)(a),(LONG)(b))
#define jmp_xbios_llww(a,b,c,d,e)   jmp_xbios((WORD)a,(LONG)b,(LONG)c,(WORD)d,(WORD)e)
#define jmp_xbios_ww(a,b,c)     jmp_xbios((WORD)(a),(WORD)(b),(WORD)(c))

#define Dsetdrv(a)          jmp_gemdos_w(0x0e,a)
#define Dgetdrv()           jmp_gemdos_v(0x19)
#define Fgetdta()           jmp_gemdos_v(0x2f)
#define Sversion()          jmp_gemdos_v(0x30)
#define Dfree(a,b)          jmp_gemdos_pw(0x36,a,b)
#define Dcreate(a)          jmp_gemdos_p(0x39,a)
#define Ddelete(a)          jmp_gemdos_p(0x3a,a)
#define Dsetpath(a)         jmp_gemdos_p(0x3b,a)
#define Fcreate(a,b)        jmp_gemdos_pw(0x3c,a,b)
#define Fopen(a,b)          jmp_gemdos_pw(0x3d,a,b)
#define Fclose(a)           jmp_gemdos_w(0x3e,a)
#define Fread(a,b,c)        jmp_gemdos_wlp(0x3f,a,b,c)
#define Fwrite(a,b,c)       jmp_gemdos_wlp(0x40,a,b,c)
#define Fdelete(a)          jmp_gemdos_p(0x41,a)
#define Fattrib(a,b,c)      jmp_gemdos_pww(0x43,a,b,c)
#define Fdup(a)             jmp_gemdos_w(0x45,a)
#define Fforce(a,b)         jmp_gemdos_ww(0x46,a,b)
#define Dgetpath(a,b)       jmp_gemdos_pw(0x47,a,b)
#define Malloc(a)           jmp_gemdos_l(0x48,a)
#define Mfree(a)            jmp_gemdos_p(0x49,a)
#define Pexec(a,b,c,d)      jmp_gemdos_wppp(0x4b,a,b,c,d)
#define Fsfirst(a,b)        jmp_gemdos_pw(0x4e,a,b)
#define Fsnext()            jmp_gemdos_v(0x4f)
#define Frename(a,b,c)      jmp_gemdos_wpp(0x56,a,b,c)

#define Bconstat(a)         jmp_bios_w(0x01,a)
#define Bconin(a)           jmp_bios_w(0x02,a)
#define Bconout(a,b)        jmp_bios_ww(0x03,a,b)

#define Setscreen(a,b,c,d)  jmp_xbios_llww(0x05,a,b,c,d)
#define Cursconf(a,b)       jmp_xbios_ww(0x15,a,b)
#define Kbrate(a,b)         jmp_xbios_ww(0x23,a,b)
#define Supexec(a)          jmp_xbios_l(0x26,a)


/*
 * program parameters
 */
#define CMDLINELEN      128     /* allow for length char etc */
#define MAXCMDLINE      125     /* the most amount of real data allowed */

#define IOBUFSIZE       16384L  /* buffer size */

#define MAX_LINE_SIZE   200     /* must be greater than the largest screen width */
#define HISTORY_SIZE    10      /* number of lines of history */
#define MAX_ARGS        30      /* maximum number of args we can parse */

#define LOCAL           static  /* comment out for testing */
#define PRIVATE         static  /* comment out for testing */

/*
 * date/time display format stuff
 */
#define _IDT_COOKIE     0x5f494454      /* '_IDT' */
#define _IDT_MDY        0               /* date format: month-day-year */
#define _IDT_DMY        1               /*              day-month-year */
#define _IDT_YMD        2               /*              year-month-day */
#define _IDT_YDM        3               /*              year-day-month */
#define _IDT_12H        0               /* time format: 12-hour */
#define _IDT_24H        1               /*              24-hour */

#define DEFAULT_DT_SEPARATOR    '/'
#define DEFAULT_DT_FORMAT   ((_IDT_12H<<12) + (_IDT_YMD<<8) + DEFAULT_DT_SEPARATOR)

/*
 *  typedefs
 */
typedef struct {
    char    d_reserved[21];
    char    d_attrib;
    WORD    d_time;
    WORD    d_date;
    LONG    d_length;
    char    d_fname[14];
} DTA;

/* Type of function run by execute() */
typedef LONG FUNC(WORD argc,char **argv);

/*
 *  return codes from get_next_arg()
 */
#define ARG_NORMAL      1
#define NO_MORE_ARGS    0
#define QUOTING_ERROR   -1

/*
 *  manifest constants
 */
#define EFILNF          -33
#define EPTHNF          -34
#define ENHNDL          -35
#define EACCDN          -36
#define ENSMEM          -39
#define EDRIVE          -46
#define ENMFIL          -49
                                /* additional emucon-only error codes */
#define USER_BREAK      -100        /* user interrupted long output */
#define INVALID_PATH    -101        /* invalid component for PATH command */
#define DISK_FULL       -102
#define CMDLINE_LENGTH  -103
#define DIR_NOT_EMPTY   -104        /* translated from EACCDN for folders */
#define CANT_DELETE     -105        /* translated from EACCDN for files */
#define INVALID_PARAM   -126        /* for builtin commands */
#define WRONG_NUM_ARGS  -127        /* for builtin commands */

#define ESC             0x1b
#define DBLQUOTE        0x22

#define CTL_C           ('C'-0x40)
#define CTL_Q           ('Q'-0x40)
#define CTL_S           ('S'-0x40)

#define blank_line()    escape('l')
#define clear_screen()  escape('E')
#define cursor_left()   escape('D')
#define cursor_right()  escape('C')
#define enable_cursor() escape('e')
#define conin()         Bconin(2)
#define constat()       Bconstat(2)
#define conout(c)       Bconout(2,c)

#define LOOKUP_EXIT     (FUNC *)-1L     /* special return values from lookup_builtin() */
#define LOOKUP_ARGS     (FUNC *)-2L

/*
 *  global variables
 */
extern LONG idt_value;
extern UWORD screen_cols, screen_rows;
extern UWORD linesize;
extern WORD linewrap;
extern DTA *dta;
extern LONG redir_handle;
extern char user_path[MAXPATHLEN];     /* from PATH command */

/*
 *  function prototypes
 */
/* cmdmain.c */
void outlong(ULONG n,WORD width,char filler);

/* cmdedit.c */
WORD init_cmdedit(void);
void insert_char(char *line,WORD pos,WORD len,char c);
WORD read_line(char *line);
void save_history(const char *line);

/* cmdexec.c */
LONG exec_program(WORD argc,char **argv,char *redir_name);

/* cmdint.c */
LONG (*lookup_builtin(WORD argc,char **argv))(WORD,char **);

/* cmdparse.c */
WORD parse_line(char *line,char **argv,char *redir_name);

/* cmdutil.c */
void convulong(char *buf,ULONG n,WORD width,char filler);
WORD decode_date_time(char *s,UWORD date,UWORD time);
void errmsg(LONG rc);
void escape(char c);
WORD getcookie(LONG cookie,LONG *pvalue);
WORD getword(char *buf);
WORD get_path_component(char **pp,char *dest);
WORD has_wildcard(const char *name);
void message(const char *msg);
void messagenl(const char *msg);
const char *program_extension(const DTA *dta);
WORD strequal(const char *s1,const char *s2);
char *strlower(char *str);
char *strupper(char *str);

/* cmdasm.S */
ULONG getwh(void);
WORD getht(void);
