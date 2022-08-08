/*
 * EmuCON2 parsing functions
 *
 * Copyright (C) 2013-2022 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "cmd.h"
#include "string.h"

/*
 *  function prototypes
 */
PRIVATE WORD get_next_arg(char **p,char **argv);
PRIVATE WORD get_redirect(char *line,char **redir);

WORD parse_line(char *line,char **argv,char *redir_name)
{
char *p, *temp, *redir_addr;
WORD argc, n, rc;

    redir_addr = NULL;
    n = get_redirect(line,&redir_addr);
    if (n > 1) {
        messagenl(_("more than one redirection"));
        return -1;
    }

    p = line;
    argc = 0;

    while(1) {
        rc = get_next_arg(&p,&temp);
        switch(rc) {
        case ARG_NORMAL:
#ifdef STANDALONE_CONSOLE
            /*
             * we wish to avoid Atari TOS mapping drive B to drive A when
             * we are running as a standalone console on a 1-floppy system.
             * if we see what appears to be a drive letter of B:, we convert
             * it to 9:, which is never valid.  TOS will reject such paths
             * with 'invalid path'.
             *
             * to avoid surprises, we do not convert the args of the 'echo'
             * builtin command.
             */
            if ((nflops_copy == 1) && (temp[1] == DRIVESEP) && ((temp[0]|0x20) == 'b'))
                if (!strequal(argv[0],"echo"))
                    temp[0] = '9';
#endif
            if (redir_addr && (temp > redir_addr)) {
                strcpy(redir_name,temp);
                redir_addr = NULL;
            } else argv[argc++] = temp;
            break;
        case NO_MORE_ARGS:
            if (redir_addr) {
                messagenl(_("no filename for redirection"));
                return -1;
            }
            return argc;
            break;
        case QUOTING_ERROR:
            messagenl(_("error in quoted field"));
            return -1;
            break;
        default:
            messagenl(_("error parsing line"));
            return -1;
            break;
        }
    }

    return -1;
}

/*
 *  scans buffer for '>' (output redirection character)
 *      . replaces '>' with ' '
 *      . updates redir ptr to point to the last '>' found
 *
 *  returns number of redirects found
 */
PRIVATE WORD get_redirect(char *line,char **redir)
{
char *p;
WORD n, inquotes;

    for (p = line, n = 0, inquotes = 0; *p; p++) {
        if (*p == DBLQUOTE)
            inquotes ^= 1;
        if (!inquotes) {
            if (*p == '>') {
                *p = ' ';
                *redir = p;
                n++;
            }
        }
    }

    return n;
}

/*
 *  scans buffer for next arg, handles quoted args
 *
 *  returns:
 *      1   arg is normal
 *      0   no more args
 *      -1  quoting error
 *
 *  the buffer pointer is updated iff return code >= 0
 */
PRIVATE WORD get_next_arg(char **pp,char **arg)
{
char *p;
WORD inquotes = 0;

    /*
     *  look for start of next arg
     */
    for (p = *pp, *arg = NULL; *p; p++)
        if (*p != ' ')
            break;
    if (!*p) {          /* end of buffer */
        *pp = p;
        return NO_MORE_ARGS;
    }

    *arg = p;
    if (*p == DBLQUOTE) {
        inquotes = 1;
        p++;
    }

    for ( ; *p; p++) {
        if (*p == DBLQUOTE) {
            if (!inquotes)
                return QUOTING_ERROR;
            inquotes = 0;
            continue;
        }
        if (inquotes)
            continue;
        if (*p == ' ') {
            *p++ = '\0';
            break;
        }
    }

    if (inquotes)
        return QUOTING_ERROR;

    *pp = p;

    return ARG_NORMAL;
}
