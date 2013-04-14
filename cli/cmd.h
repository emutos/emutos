/*
 * EmuCON2 header
 *
 * Copyright (c) 2013 The EmuTOS development team
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
 #define MAXPATHLEN      (LEN_ZPATH+LEN_ZFNAME+1)
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

#include <osbind.h>

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
#define USER_BREAK      -100        /* user interrupted long output */
#define INVALID_PATH    -101        /* invalid component for PATH command */
#define DISK_FULL       -102
#define CMDLINE_LENGTH  -103

#define TAB             0x0f09
#define BKSP            0x0e08
#define DEL             0x537f
#define UPARROW         0x4800
#define DNARROW         0x5000
#define LTARROW         0x4b00
#define LTARROW_SHFT    0x4b34
#define RTARROW         0x4d00
#define RTARROW_SHFT    0x4d36

#define ESC             0x1b

#define CTL_C           ('C'-0x40)
#define CTL_Q           ('Q'-0x40)
#define CTL_S           ('S'-0x40)

#define clear_screen()  escape('E')
#define cursor_left()   escape('D')
#define cursor_right()  escape('C')
#define conin()         Bconin(2)
#define constat()       Bconstat(2)
#define conout(c)       Bconout(2,c)

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
void output(char *s);

/* cmdedit.c */
WORD init_cmdedit(void);
void insert_char(char *line,WORD pos,WORD len,char c);
void read_line(char *line);
void save_history(char *line);

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
WORD get_path_component(char **pp,char *dest);
WORD has_wildcard(char *name);
void message(char *msg);
void messagenl(char *msg);
void output(char *s);
void outputnl(char *s);
LONG outputbuf(char *s,LONG len);
WORD strequal(char *s1,char *s2);
char *strlower(char *str);
char *strupper(char *str);

/* cmdasm.S */
ULONG getwh(void);
