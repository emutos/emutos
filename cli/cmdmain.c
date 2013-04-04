/*
 * EmuCON2: a command processor for EmuTOS
 *
 * Copyright (c) 2013 The EmuTOS development team
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
LOCAL char *argv[MAX_ARGS];
LOCAL char redir_name[MAXPATHLEN];

/*
 *  function prototypes
 */
PRIVATE void close_redir(void);
PRIVATE void create_redir(char *name);
PRIVATE WORD execute(WORD argc,char **argv,char *redir);
PRIVATE void strip_quotes(int argc,char **argv);

int cmdmain(void)
{
WORD argc;
ULONG n;

    clear_screen();
    messagenl(_("Welcome to EmuCON2: type HELP for builtin commands"));

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
        read_line(input_line);
        save_history(input_line);
        argc = parse_line(input_line,argv,redir_name);
        if (argc < 0)       /* parse error */
            continue;
        if (execute(argc,argv,redir_name) < 0)
            break;
    }

    return 0;
}

PRIVATE WORD execute(WORD argc,char **argv,char *redir)
{
LONG (*func)(WORD argc,char **argv);
LONG rc;

    if (argc == 0)
        return 0;

    func = lookup_builtin(argc,argv);
    if ((LONG)func == -1L)          /* exit/quit */
        return -1;

    if (func) {
        create_redir(redir);
        strip_quotes(argc,argv);
        rc = func(argc,argv);
        close_redir();
    }
    else rc = exec_program(argc,argv,redir);

    errmsg(rc);

    return 0;
}

PRIVATE void create_redir(char *name)
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

PRIVATE void close_redir()
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
        if (**argv == '"') {
            (*argv)++;
            for (p = *argv; *p; p++)
                ;
            *(p-1) = '\0';
        }
    }
}
