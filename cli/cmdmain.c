/*
 * EmuCON2: a command processor for EmuTOS
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
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

/*
 *  global variables
 */
LONG idt_value;
UWORD screen_cols, screen_rows;
UWORD linesize;
WORD linewrap;
DTA *dta;
char user_path[MAXPATHLEN];
LONG redir_handle;

/*
 * local to this set of functions
 */
LOCAL char input_line[MAX_LINE_SIZE];
LOCAL char *arglist[MAX_ARGS];
LOCAL char redir_name[MAXPATHLEN];

/*
 *  function prototypes
 */
PRIVATE void close_redir(void);
PRIVATE void create_redir(const char *name);
PRIVATE WORD execute(WORD argc,char **argv,char *redir);
PRIVATE void strip_quotes(int argc,char **argv);

extern int cmdmain(void); /* called only from cmdasm.S */

int cmdmain(void)
{
WORD argc, rc;
ULONG n;

    clear_screen();
    enable_cursor();
    messagenl(_("Welcome to EmuCON2"));
    messagenl(_("Type HELP for builtin commands"));
    messagenl("");

    /*
     *  initialise some global variables
     */
    if (getcookie(_IDT_COOKIE,&idt_value) == 0)
        idt_value = DEFAULT_DT_FORMAT;      /* if not found, make sure it's initialised properly */

    n = getwh();                            /* get max cell number for x and y */
    screen_cols = (n >> 16) + 1;
    screen_rows = (n & 0xffff) + 1;
    linesize = screen_cols + 1 - 3;         /* allow for trailing NUL and prompt */

    linewrap = 0;
    dta = (DTA *)Fgetdta();
    redir_handle = -1L;

    if (init_cmdedit() < 0)
        messagenl(_("warning: no history buffers"));

    while(1) {
        rc = read_line(input_line);
        save_history(input_line);
        if (rc < 0)         /* user cancelled line */
            continue;
        argc = parse_line(input_line,arglist,redir_name);
        if (argc < 0)       /* parse error */
            continue;
        if (execute(argc,arglist,redir_name) < 0)
            break;
    }

    return 0;
}

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
