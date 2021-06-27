/*
 * EmuCON2: a command processor for EmuTOS
 *
 * Copyright (C) 2013-2021 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
/*
 * This is a minimalist command processor, with the following
 * features:
 *      builtin commands
 *      execution of standard TOS programs
 *      commandline history & editing
 *      output redirection
 *
 * The following omissions are deliberate:
 *      no batch/shell files
 *      no input redirection or pipes
 */
#include "cmd.h"
#include "version.h"
#include "string.h"

/*
 *  global variables
 */
LONG idt_value;
UWORD screen_cols, screen_rows;
WORD current_res, requested_res;
WORD linewrap;
WORD nflops_copy;
DTA *dta;
char user_path[MAXPATHLEN];
LONG redir_handle;

/*
 * local to this set of functions
 */
LOCAL char input_line[MAX_LINE_SIZE];
LOCAL char *arglist[MAX_ARGS];
LOCAL char redir_name[MAXPATHLEN];
LOCAL WORD original_res;
LOCAL WORD original_color3;
LOCAL LONG vdo_value;

/*
 *  function prototypes
 */
PRIVATE void change_res(WORD res);
PRIVATE void close_redir(void);
PRIVATE void create_redir(const char *name);
PRIVATE WORD execute(WORD argc,char **argv,char *redir);
PRIVATE WORD get_nflops(void);
PRIVATE void strip_quotes(int argc,char **argv);
PRIVATE void getenv(char **ppath, const char *psrch);

int cmdmain(void);      /* called only from cmdasm.S */

int cmdmain(void)
{
WORD argc, rc;

    /*
     *  initialise some global variables
     */
    if (getcookie(_IDT_COOKIE,&idt_value) == 0)
        idt_value = DEFAULT_DT_FORMAT;      /* if not found, make sure it's initialised properly */

    if (getcookie(_VDO_COOKIE,&vdo_value) == 0)
        vdo_value = _VDO_ST;
#if CONF_WITH_TT_SHIFTER
    original_res = (vdo_value < _VDO_VIDEL) ? Getrez() : -1;
#else
    original_res = (vdo_value < _VDO_TT) ? Getrez() : -1;
#endif
    current_res = original_res;
    original_color3 = Setcolor(3,-1);

    nflops_copy = Supexec(get_nflops);      /* number of floppy drives */

    /*
     * start up in ST medium if we are currently in ST low
     */
    if (current_res == ST_LOW)
        change_res(ST_MEDIUM);

    clear_screen();
    enable_cursor();
    message(_("Welcome to EmuCON2 version ")); messagenl(version);
    messagenl(_("Type HELP for builtin commands"));
    messagenl("");

    linewrap = 0;
    dta = (DTA *)Fgetdta();
    redir_name[0] = '\0';
    redir_handle = -1L;

    if (init_cmdedit() < 0)
        messagenl(_("warning: no history buffers"));

    {
        /* Setup path from the PATH environment variable */
        char *largv[2];
        getenv(&largv[1],"PATH=");
        if (largv[1]) {
            if (!largv[1][0])   /* skip NUL after PATH= */
                largv[1]++;
            if (largv[1][0]) {
                /* path ${PATH$} */
                largv[0] = "path";
                execute(2,largv,redir_name);
            }
        }
    }

    while(1) {
        init_screen();      /* init variables for screen size */

        do {
            rc = read_line(input_line);
            save_history(input_line);
            if (rc < 0)         /* user cancelled line */
                continue;
            argc = parse_line(input_line,arglist,redir_name);
            if (argc < 0)       /* parse error */
                continue;
            rc = execute(argc,arglist,redir_name);
            if (rc < 0) {       /* exit EmuCON */
                change_res(original_res);
                return 0;
            }
        } while (rc <= 0);      /* until resolution change */

        change_res(requested_res);
    }

    return 0;
}

/*
 * execute a builtin or external command
 *
 * returns: -1  EmuCON should exit
 *          0   normal
 *          +1  EmuCON should change resolution
 */
PRIVATE WORD execute(WORD argc,char **argv,char *redir)
{
FUNC *func;
LONG rc;

    if (argc == 0)
        return 0;

    func = lookup_builtin(argc,argv);
    if (func == LOOKUP_EXIT)    /* exit/quit */
        return -1;

    if (func == LOOKUP_ARGS)
        rc = WRONG_NUM_ARGS;
    else if (func) {
        create_redir(redir);
        strip_quotes(argc,argv);
        rc = func(argc,argv);
        close_redir();
        if (rc == CHANGE_RES)
            return 1;
    }
    else rc = exec_program(argc,argv,redir);

    errmsg(rc);

    return 0;
}

PRIVATE void create_redir(const char *name)
{
LONG rc;

    redir_handle = -1L;     /* no redirection */

    if (!*name)
        return;

    rc = Fcreate(name,0);
    if (rc < 0)
        errmsg(rc);
    else redir_handle = rc;
}

PRIVATE void close_redir(void)
{
    if (redir_handle >= 0)
        Fclose((WORD)redir_handle);

    redir_name[0] = '\0';
    redir_handle = -1L;
}

/*
 *  strips surrounding quotes from all args
 */
PRIVATE void strip_quotes(int argc,char **argv)
{
char *p;
int i;

    for (i = 0; i < argc; i++, argv++) {
        if (**argv == DBLQUOTE) {
            (*argv)++;
            for (p = *argv; *p; p++)
                ;
            *(p-1) = '\0';
        }
    }
}

/*
 *  Find a variable in the process's environment.
 *  psrch: variable name followed by '=', e.g. "PATH="
 *  ppath: returned address of the first character after the found
 *         variable, or NULL if variable was not found.
 *  Note:  the implementation is the same as that of the AES sh_envrn
 *         which we can't use because EmuCON is not a GEM app.
 */
PRIVATE void getenv(char **ppath, const char *psrch)
{
    /* The implementation is the same as that of the AES sh_envrn */
    char *p;
    WORD len;

    len = strlen(psrch);
    *ppath = NULL;

    /*
     * scan environment string until double nul
     */
    for (p = environment; *p; ) {
        if (strncmp(p, psrch, len) == 0) {
            *ppath = p + len;
            break;
        }
        while(*p++) /* skip to end of current env variable */
            ;
    }
}

/*
 *  change video resolution - assumes new resolution has been validated
 */
PRIVATE void change_res(WORD res)
{
#if CONF_ATARI_HARDWARE
    WORD fgcol, bgcol;

    if (res == current_res)
        return;

    Setscreen(-1L,-1L,res,0);

    /*
     * set readable text color index for ST medium
     *
     *  when switching to ST medium, ensure colour 3 is black
     *  when switching from ST medium, restore original colour
     */
    if (res == ST_MEDIUM)
        Setcolor(3,BLACK);
    else if (current_res == ST_MEDIUM)
        Setcolor(3,original_color3);
    fgcol = 15; /* OS masks color index, so 15 is fine also for mono/medium modes */
    bgcol = 0;

    /*
     * handle ST high (i.e. TT 'Duochrome')
     *
     * this can only happen with the TT shifter, and in this case ST high
     * is implemented via the TT 'Duochrome' mode, so we must reverse
     * foreground & background colours (see comments in bios/screen.c
     * for the gory details)
     */
    if (res == ST_HIGH) {
        fgcol = 0;
        bgcol = 15;
    }

    escape('b');    /* ESC b => set foreground colour */
    conout(fgcol);
    escape('c');    /* ESC c => set background colour */
    conout(bgcol);

    clear_screen();
    enable_cursor();
    current_res = res;
#endif
}

/*
 *  validate new video resolution
 */
int valid_res(WORD res)
{
    if (vdo_value == _VDO_VIDEL)    /* can't change Falcon resolutions */
        return FALSE;

    if (current_res == TT_HIGH)     /* can't change from TT high */
        return FALSE;

    if ((current_res == ST_HIGH) && (vdo_value != _VDO_TT))
        return FALSE;               /* only TTs can change from ST high */

    switch(res) {
#if CONF_WITH_TT_SHIFTER
    case TT_LOW:
    case TT_MEDIUM:
    case ST_HIGH:
        if (vdo_value != _VDO_TT)
            return FALSE;
        FALLTHROUGH;
#endif  /* CONF_WITH_TT_SHIFTER */
    case ST_LOW:
    case ST_MEDIUM:
        return TRUE;
    }

    return FALSE;
}

PRIVATE WORD get_nflops(void)
{
    return *(WORD *)0x4a6;          /* number of floppy drives */
}
