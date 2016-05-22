/*
 * EmuCON2: execute external programs
 *
 * Copyright (C) 2013 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "cmd.h"
#include <string.h>

static UWORD old_stdout;

/*
 *  function prototypes
 */
PRIVATE void add_to_path(char *path,const char *name);
PRIVATE WORD build_cmdline(char *cmdline,WORD argc,char **argv);
PRIVATE WORD check_user_path(char *path,const char *name);
PRIVATE WORD find_executable(char *fullname,const char *name);
PRIVATE WORD is_graphical(const char *name);
PRIVATE LONG redirect_stdout(char *redir);
PRIVATE void restore_stdout(char *redir);


LONG exec_program(WORD argc,char **argv,char *redir)
{
char path[MAXPATHLEN];
char cmdline[CMDLINELEN];
LONG rc;

    if (has_wildcard(argv[0]))
        return INVALID_PATH;

    if (build_cmdline(cmdline,argc,argv) < 0)
        return CMDLINE_LENGTH;

    if (find_executable(path,argv[0]) == 0)
        ;
    else if (check_user_path(path,argv[0]) < 0)
        return EFILNF;

    rc = redirect_stdout(redir);
    if (rc < 0L)
        return rc;

    if (is_graphical(path))
        (void)Cursconf(0,0);
    rc = Pexec(0,path,cmdline,NULL);
    (void)Cursconf(1,0);

    restore_stdout(redir);

    return rc;
}

/*
 *  add filename to path
 */
PRIVATE void add_to_path(char *path,const char *name)
{
char *p;
const char *q;

    for (p = path; *p; p++)
        ;
    if (*(p-1) != '\\')
        *p++ = '\\';

    for (q = name; *q; )
        *p++ = *q++;

    *p = '\0';
}

/*
 *  build command line from args
 *
 *  returns -1 iff the line is too long
 */
PRIVATE WORD build_cmdline(char *cmdline,WORD argc,char **argv)
{
char *p, *q;
WORD i, len;

    for (i = 1, argv++, p = cmdline+1, len = 0; (i < argc) && (len <= MAXCMDLINE); i++, argv++) {
        for (q = *argv; *q && (len <= MAXCMDLINE); len++)
            *p++ = *q++;
        if (i < argc-1) {
            *p++ = ' ';
            len++;
        }
    }
    *p = '\0';

    if (len > MAXCMDLINE)
        return -1;

    cmdline[0] = (char) len;

    return 0;
}

/*
 *  check user_path[] directories for matching filename
 *
 *  if match found, 'path' contains full path, rc = 0
 */
PRIVATE WORD check_user_path(char *path,const char *name)
{
char temp[MAXPATHLEN], *p;

    for (p = user_path; *p; ) {
        if (get_path_component(&p,temp) == 0)
            return -1;
        add_to_path(temp,name);
        if (find_executable(path,temp) == 0)
            return 0;
    }

    return -1;
}

/*
 *  find executable, adding extension if necessary
 */
PRIVATE WORD find_executable(char *fullname,const char *name)
{
char *p, *dot;
const char *q;
LONG rc;

    /*
     *  copy name, adding wildcard if no extension
     */
    for (p = fullname, q = name, dot = NULL; *q; ) {
        switch(*q) {
        case '.':
            dot = p;
            break;
        case '\\':
        case ':':
            dot = NULL;
            break;
        }
        *p++ = *q++;
    }
    if (!dot) {
        dot = p;
        *p++ = '.';
        *p++ = '*';
    }
    *p = '\0';

    /*
     *  look for matching executable
     */
    for (rc = Fsfirst(fullname,0x07); rc == 0; rc = Fsnext()) {
        q = program_extension(dta);
        if (q) {
            strcpy(dot+1,q);
            return 0;
        }
    }

    return -1;
}

/*
 *  test type of executed program
 */
PRIVATE WORD is_graphical(const char *name)
{
const char *p;

    for (p = name; *p; p++)
        ;
    p -= 4;         /* back up to putative period */

    if (p < name)
        return 0;

    if (*p++ != '.')
        return 0;

    if (strequal(p,"app") || strequal(p,"gtp") || strequal(p,"prg"))
        return 1;

    return 0;
}

/*
 *  redirect stdout with Fdup()/Fforce()
 */
PRIVATE LONG redirect_stdout(char *redir)
{
LONG rc;

    redir_handle= -1;

    if (!redir[0])
        return 0L;

    rc = Fcreate(redir,0);
    if (rc < 0L)
        return rc;

    redir_handle = rc;
    old_stdout = Fdup(1);           /* remember current stdout */
    rc = Fforce(1,redir_handle);    /* redirect it */
    if (rc < 0L)
        return rc;

    return 0;
}

PRIVATE void restore_stdout(char *redir)
{
    if (!redir[0])                  /* not redirected ... */
        return;

    Fforce(1,old_stdout);           /* get old stdout back */
    Fclose(old_stdout);             /* release duplicate */
    Fclose(redir_handle);           /*  & original */

    redir[0] = '\0';                /* end redirection */
}
