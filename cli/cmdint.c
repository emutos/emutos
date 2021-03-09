/*
 * EmuCON2 builtin commands
 *
 * Copyright (C) 2013-2020 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#include "cmd.h"
#include "string.h"
#include "shellutl.h"

typedef struct {
    const char *name;
    const char *synonym;
    WORD minargs;
    WORD maxargs;
    FUNC *func;
    const char * const *help;
} COMMAND;

/*
 *  function prototypes
 */
PRIVATE LONG check_path_component(char *component);
PRIVATE LONG copy_move(WORD argc,char **argv,WORD delete);
PRIVATE void display_dta_detail(void);
PRIVATE char *extract_path(char *dest,const char *src);
PRIVATE void fixup_filespec(char *filespec);
PRIVATE char getyn(void);
PRIVATE void help_display(const COMMAND *p);
PRIVATE WORD help_lines(const COMMAND *p);
PRIVATE WORD help_pause(void);
PRIVATE WORD help_wanted(const COMMAND *p,char *cmd);
PRIVATE LONG is_valid_drive(char drive_letter);
PRIVATE void output(const char *s);
PRIVATE void outputnl(const char *s);
PRIVATE LONG outputbuf(const char *s,LONG len,WORD paging);
PRIVATE LONG output_files(WORD argc,char **argv,WORD paging);
PRIVATE void padname(char *buf,const char *name);
PRIVATE void show_line(const char *title,ULONG n);
PRIVATE WORD user_break(void);
PRIVATE WORD user_input(WORD c);

PRIVATE LONG run_cat(WORD argc,char **argv);
PRIVATE LONG run_cd(WORD argc,char **argv);
PRIVATE LONG run_chmod(WORD argc,char **argv);
PRIVATE LONG run_cls(WORD argc,char **argv);
PRIVATE LONG run_cp(WORD argc,char **argv);
PRIVATE LONG run_echo(WORD argc,char **argv);
PRIVATE LONG run_help(WORD argc,char **argv);
PRIVATE LONG run_ls(WORD argc,char **argv);
PRIVATE LONG run_mkdir(WORD argc,char **argv);
PRIVATE LONG run_more(WORD argc,char **argv);
PRIVATE LONG run_mode(WORD argc,char **argv);
PRIVATE LONG run_path(WORD argc,char **argv);
PRIVATE LONG run_pwd(WORD argc,char **argv);
PRIVATE LONG run_mv(WORD argc,char **argv);
PRIVATE LONG run_ren(WORD argc,char **argv);
PRIVATE LONG run_rm(WORD argc,char **argv);
PRIVATE LONG run_rmdir(WORD argc,char **argv);
PRIVATE LONG run_setdrv(WORD argc,char **argv);
PRIVATE LONG run_show(WORD argc,char **argv);
PRIVATE LONG run_version(WORD argc,char **argv);
PRIVATE LONG run_wrap(WORD argc,char **argv);

/*
 *  help strings
 */
LOCAL const char * const help_cat[] = { "<filespec> ...",
    N_("Copy <filespec> ... to standard output"), NULL };
LOCAL const char * const help_cd[] = { "[<dir>]",
    N_("Change current directory to <dir>"),
    N_("or display current directory"), NULL };
LOCAL const char * const help_chmod[] = { "<mode> <filename>",
    N_("Change attributes for <filename>"),
    N_("<mode> specifies the new attribute(s):"),
    N_("r=read-only  h=hidden  s=system  -=none"), NULL };
LOCAL const char * const help_cls[] = { "",
    N_("Clear screen"), NULL };
LOCAL const char * const help_cp[] = { "<filespec> <dest>",
    N_("Copy files matching <filespec> to <dest>"),
    N_("If <filespec> matches multiple files,"),
    N_("<dest> must be a directory"), NULL };
LOCAL const char * const help_echo[] = { "<string> ...",
    N_("Copy <string> ... to standard output"),
    N_("Strings may be surrounded by \"\""), NULL };
LOCAL const char * const help_exit[] = { "",
    N_("Exit EmuCON2"), NULL };
LOCAL const char * const help_help[] = { "[<cmd>]",
    N_("Get help about <cmd> or list available commands"),
    N_("Use HELP ALL for help on all commands"),
    N_("Use HELP EDIT for help on line editing"), NULL };
LOCAL const char * const help_ls[] = { "[-l] <path>",
    N_("List files (default terse, horizontal)"),
    N_("Specify -l for detailed list"), NULL };
LOCAL const char * const help_mkdir[] = { "<dir>",
    N_("Create directory <dir>"), NULL };
LOCAL const char * const help_mode[] = { "con[:] [res=<r>] [delay=<m>] [rate=<n>]",
    N_("Set or display console settings:"),
    N_("res for screen resolution (0,1)[ST] (0,1,2,4,7)[TT];"),
    N_("delay/rate for the keyboard"), NULL };
LOCAL const char * const help_more[] = { "<filespec>",
    N_("Copy <filespec> to standard output,"),
    N_("pausing every screenful"), NULL };
LOCAL const char * const help_mv[] = { "<filespec> <dir>",
    N_("Copy files matching <filespec> to <dir>,"),
    N_("then delete input files"), NULL };
LOCAL const char * const help_path[] = { "[<searchpath>]",
    N_("Set search path for external programs"),
    N_("or display current search path"), NULL };
LOCAL const char * const help_pwd[] = { "",
    N_("Display current drive and directory"), NULL };
LOCAL const char * const help_ren[] = { "<oldname> <newname>",
    N_("Rename <oldname> to <newname>"), NULL };
LOCAL const char * const help_rm[] = { " [-q] <filespec>",
    N_("Delete files matching <filespec>"),
    N_("Specify -q to be prompted each time"), NULL };
LOCAL const char * const help_rmdir[] = { "<dir>",
    N_("Delete directory <dir>"), NULL };
LOCAL const char * const help_show[] = { "[<drive>]",
    N_("Show info for <drive> or current drive"), NULL };
LOCAL const char * const help_version[] = { "",
    N_("Display GEMDOS version"), NULL };
LOCAL const char * const help_wrap[] = { "[on|off]",
    N_("Set line wrap or show current status"), NULL };


LOCAL const char * const help_edit[] = {
 N_("up/down arrow = previous/next line in history"),
 N_("left/right arrow = previous/next character"),
 N_("control-left/right arrow = previous/next word"), NULL };


/*
 *  command table
 */
LOCAL const COMMAND cmdtable[] = {
    { "cat", "type", 1, 255, run_cat, help_cat },
    { "cd", NULL, 0, 1, run_cd, help_cd },
    { "chmod", NULL, 2, 2, run_chmod, help_chmod },
    { "cls", "clear", 0, 0, run_cls, help_cls },
    { "cp", "copy", 2, 2, run_cp, help_cp },
    { "echo", NULL, 0, 255, run_echo, help_echo },
    { "exit", NULL, 0, 0, LOOKUP_EXIT, help_exit },
    { "help", NULL, 0, 1, run_help, help_help },
    { "ls", "dir", 0, 2, run_ls, help_ls },
    { "mkdir", "md", 1, 1, run_mkdir, help_mkdir },
    { "mode", NULL, 1, 4, run_mode, help_mode },
    { "more", NULL, 1, 1, run_more, help_more },
    { "mv", "move", 2, 2, run_mv, help_mv },
    { "path", NULL, 0, 1, run_path, help_path },
    { "pwd", NULL, 0, 0, run_pwd, help_pwd },
    { "ren", NULL, 2, 2, run_ren, help_ren },
    { "rm", "del", 1, 2, run_rm, help_rm },
    { "rmdir", "rd", 1, 1, run_rmdir, help_rmdir },
    { "show", NULL, 0, 1, run_show, help_show },
    { "version", NULL, 0, 0, run_version, help_version },
    { "wrap", NULL, 0, 1, run_wrap, help_wrap },
    { "", NULL, 0, 255, NULL, NULL }                    /* end marker */
};

static LONG linecount;  /* used by 'more' command */

LONG (*lookup_builtin(WORD argc,char **argv))(WORD,char **)
{
const COMMAND *p;

    /*
     *  handle drive change
     */
    if ((argc == 1) && (strlen(argv[0]) == 2))
        if (argv[0][1] == ':')
            return run_setdrv;

    /*
     *  allow -h with any command to provide help
     */
    if ((argc == 2) && strequal(argv[1],"-h")) {
        argv[1] = argv[0];
        argv[0] = "help";
    }

    /*
     *  scan command table
     */
    for (p = cmdtable; p->func; p++) {
        if (strequal(argv[0],p->name))
            break;
        if (p->synonym)
            if (strequal(argv[0],p->synonym))
                break;
    }

    argc--;
    if ((argc < p->minargs) || (argc > p->maxargs))
        return LOOKUP_ARGS;

    return p->func;
}

PRIVATE LONG run_cat(WORD argc,char **argv)
{
    return output_files(argc,argv,0);
}

PRIVATE LONG run_cd(WORD argc,char **argv)
{
char path[MAXPATHLEN];
LONG rc;
char *p;
WORD current_drive, temp_drive;

    if (argc == 1) {                /* just output current path */
        rc = get_path(path,0);
        outputnl(path);
        return rc;
    }

    /*
     * we may be running under a BDOS that mishandles path specifications
     * containing a drive letter (e.g. when running as standalone EmuCON
     * under TOS 1/2/3, or when referencing a 'GEMDOS emulation' drive
     * under an emulator).  therefore we temporarily switch to the
     * specified drive before calling Dsetpath().
     */
    current_drive = Dgetdrv();      /* remember current drive */

    /*
     * if path contains drive letter, validate it
     *
     * if the path is just the drive letter, display the current path
     * for that drive; else attempt to change the path on that drive
     */
    p = argv[1];
    if (*(p+1) == ':') {
        if (!is_valid_drive(*p))
            return EDRIVE;
        temp_drive = (*p|0x20) - 'a';
        if (*(p+2) == '\0') {       /* cd x: */
            rc = get_path(path,temp_drive+1);
            outputnl(path);
            return rc;
        }
        Dsetdrv(temp_drive);
        p += 2;                     /* point past drive letter */
    }

    rc = Dsetpath(p);
    Dsetdrv(current_drive);

    return rc;
}

PRIVATE LONG run_chmod(WORD argc,char **argv)
{
LONG rc;
UWORD attr = 0x00;
char c, *p;

    p = argv[1];
    while((c=*p++)) {
        switch(c) {
        case 'r':
            attr |= 0x01;
            break;
        case 'h':
            attr |= 0x02;
            break;
        case 's':
            attr |= 0x04;
            break;
        case '-':
            break;
        default:
            messagenl(_("invalid mode argument"));
            return 0L;
        }
    }

    rc = Fattrib(argv[2],1,attr);

    return (rc < 0) ? rc : 0L;
}

PRIVATE LONG run_cls(WORD argc,char **argv)
{
    escape('E');

    return 0L;
}

PRIVATE LONG run_cp(WORD argc,char **argv)
{
    return copy_move(argc,argv,0);
}

PRIVATE LONG run_echo(WORD argc,char **argv)
{
WORD i;

    for (i = argc-1; i > 0; i--) {
        output(*++argv);
        if (i > 1)
            output(" ");
    }
    outputnl("");

    return 0L;
}

PRIVATE LONG run_help(WORD argc,char **argv)
{
const COMMAND *p;
WORD lines;
const char * const *s;

    if (argc == 1) {
        outputnl(_("Builtin commands:"));
        for (p = cmdtable; p->func; p++) {
            output("  ");
            if (p->synonym) {
                output(p->name);
                output("/");
                outputnl(p->synonym);
            } else outputnl(p->name);
        }
        return 0L;
    }

    if (strequal(argv[1],"edit")) {
        for (s = &help_edit[0]; *s; s++) {
            output("  ");
            outputnl(gettext(*s));
        }
        return 0L;
    }

    for (p = cmdtable, lines = 0; p->func; p++) {
        if (help_wanted(p,argv[1])) {
            if (redir_handle < 0L) {    /* not redirecting output: */
                lines += help_lines(p); /*  see if this help will fit on screen */
                if (lines >= screen_rows) {
                    if (help_pause() < 0)
                        break;
                    lines = 0;
                }
            }
            help_display(p);
        }
    }

    return 0L;
}

PRIVATE LONG run_ls(WORD argc,char **argv)
{
char filespec[MAXPATHLEN];
char buf[20];
LONG rc;
WORD names_per_line, n;
WORD detail = 0;

    if (argc > 1) {
        if (strequal(argv[1],"-l")) {
            detail = 1;
            argc--;
            argv++;
        }
    }

    names_per_line = screen_cols / 13;

    filespec[0] = '\0';
    if (argc > 1)
        strcpy(filespec,argv[1]);
    fixup_filespec(filespec);

    if (detail) {
        output(_("Listing of "));
        outputnl(filespec);
    }
    for (rc = Fsfirst(filespec,0x17), n = 0; rc == 0; rc = Fsnext()) {
        if (constat())
            if (user_input(-1))
                return USER_BREAK;
        if (detail) {
            display_dta_detail();
        } else if (dta->d_fname[0] != '.') {
            padname(buf,dta->d_fname);
            output(buf);
            if (++n >= names_per_line) {
                outputnl("");
                n = 0;
            }
        }
    }
    if (n)
        outputnl("");

    if (rc == ENMFIL)
        rc = 0L;

    return rc;
}

PRIVATE LONG run_mkdir(WORD argc,char **argv)
{
    return Dcreate(argv[1]);
}

PRIVATE LONG run_mode(WORD argc,char **argv)
{
char buf[80];
WORD i, old;
WORD res = -1, rate = -1, delay = -1;

    argv++;
    if (strequal(*argv,"con") || strequal(*argv,"con:")) {
        if (argc == 2) {    /* just status wanted */
            outputnl(_("Status for CON:"));
            if (current_res >= 0) {
                sprintf(buf,"%s%3d",_("  Resolution:     "),current_res);
                outputnl(buf);
            }
            old = Kbrate(-1,-1);
            sprintf(buf,"%s%3d",_("  Keyboard delay: "),HIBYTE(old));
            outputnl(buf);
            sprintf(buf,"%s%3d",_("  Keyboard rate:  "),LOBYTE(old));
            outputnl(buf);
            return 0;
        }
        for (i = 2, argv++; i < argc; i++, argv++) {
            if (strncasecmp(*argv,"res=",4) == 0) {
                res = getword(*argv+4);
                if (!valid_res(res))
                    return INVALID_PARAM;
            } else if (strncasecmp(*argv,"delay=",6) == 0) {
                delay = getword(*argv+6);
                if (delay < 0)
                    return INVALID_PARAM;
            } else if (strncasecmp(*argv,"rate=",5) == 0) {
                rate = getword(*argv+5);
                if (rate < 0)
                    return INVALID_PARAM;
            } else return INVALID_PARAM;
        }
        if ((delay >= 0) || (rate >= 0))
            Kbrate(delay,rate);
        if ((res >= 0) && (res != current_res)) {
            requested_res = res;
            return CHANGE_RES;
        }
        return 0;
    }

    return INVALID_PARAM;
}

PRIVATE LONG run_more(WORD argc,char **argv)
{
    return output_files(argc,argv,1);
}

PRIVATE LONG run_mv(WORD argc,char **argv)
{
    return copy_move(argc,argv,1);
}

PRIVATE LONG run_path(WORD argc,char **argv)
{
char temp[MAXPATHLEN];
const char *p;
LONG rc = 0L;

    if (argc == 1) {
        p = user_path;
        if (!*p)
            p = _("(empty)");
        message(" ");
        messagenl(p);
        return 0L;
    }

    for (p = argv[1]; *p; ) {
        p = shellutl_find_next_path_component(p,temp);

        if (p == NULL)
            break;

        rc = check_path_component(temp);
        if (rc < 0L)
            break;
    }

    if (rc == 0L) {
        strcpy(user_path,argv[1]);
    } else {
        message(" ");
        message(temp);
        message(" ");
    }

    return rc;
}

PRIVATE LONG run_pwd(WORD argc,char **argv)
{
char buf[MAXPATHLEN];
LONG rc;

    rc = get_path(buf,0);
    outputnl(buf);

    return rc;
}

PRIVATE LONG run_ren(WORD argc,char **argv)
{
    return Frename(0,argv[1],argv[2]);
}

PRIVATE LONG run_rm(WORD argc,char **argv)
{
WORD prompt = 0;
LONG rc;

    argc--;
    argv++;

    if (strequal(*argv,"-q")) {
        prompt = 1;
        argc--;
        argv++;
    }

    if (argc == 0)
        return 0L;

    if (has_wildcard(*argv)) {
        message(_("Delete ALL matching files"));
        if (getyn() != 'y')
            return 0;
    }

    for (rc = Fsfirst(*argv,0x17); rc == 0; rc = Fsnext()) {
        if (prompt) {
            message(_("Delete file "));
            message(dta->d_fname);
            if (getyn() != 'y')
                continue;
        }
        rc = Fdelete(*argv);
        if (rc < 0L) {
            if (rc == EACCDN)
                rc = CANT_DELETE;
            break;
        }
    }

    if (rc == ENMFIL)
        rc = 0L;

    return rc;
}

PRIVATE LONG run_rmdir(WORD argc,char **argv)
{
LONG rc;

    rc = Ddelete(argv[1]);

    return (rc==EACCDN) ? DIR_NOT_EMPTY : rc;
}

PRIVATE LONG run_setdrv(WORD argc,char **argv)
{
    if (!is_valid_drive(argv[0][0]))
        return EDRIVE;

    Dsetdrv(shellutl_get_drive_number(argv[0][0]));

    return 0L;
}

PRIVATE LONG run_show(WORD argc,char **argv)
{
WORD drive;
LONG rc;
ULONG info[4], bpc;
char id[] = "X:";

    if (argc == 1)
        drive = Dgetdrv();
    else
        drive = shellutl_get_drive_number(argv[1][0]);

    rc = Dfree(info,drive+1);

    if (rc == 0) {
        bpc = info[2] * info[3];    /* bytes per cluster */

        output(_("Allocation info for drive "));
        id[0] = drive + 'A';
        outputnl(id);
        show_line(_("  Size (bytes):   "),info[1]*bpc);
        show_line(_("  Bytes used:     "),(info[1]-info[0])*bpc);
        show_line(_("  Bytes free:     "),info[0]*bpc);
        show_line(_("  Total clusters: "),info[1]);
        show_line(_("  Free clusters:  "),info[0]);
        show_line(_("  Sectors/cluster:"),info[3]);
        show_line(_("  Bytes/sector:   "),info[2]);
    } else rc = EDRIVE;

    return rc;
}

PRIVATE LONG run_version(WORD argc,char **argv)
{
UWORD n;
char version[] = "0.xx";

    n = Sversion() >> 8;
    version[2] = (n >> 4) + '0';
    version[3] = (n &0x0f) + '0';
    outputnl(version);

    return 0L;
}

PRIVATE LONG run_wrap(WORD argc,char **argv)
{
    if (argc == 1) {
        if (linewrap)
            messagenl("ON");
        else messagenl("OFF");
        return 0L;
    }

    if (strequal(argv[1],"ON")) {
        escape('v');
        linewrap = 1;
    } else if (strequal(argv[1],"OFF")) {
        escape('w');
        linewrap = 0;
    }

    return 0L;
}

/*
 *  subordinate functions
 */

/*
 *  output file(s), with optional screen paging
 */
PRIVATE LONG output_files(WORD argc,char **argv,WORD paging)
{
LONG bufsize, n, rc = 0L;
WORD handle, i;
char name[MAXPATHLEN];
char *iobuf, *p;

    bufsize = IOBUFSIZE;
    iobuf = (char *)Malloc(bufsize);
    if (!iobuf)
        return ENSMEM;

    for (i = 1; i < argc; i++, argv++) {
        p = extract_path(name,argv[i]);
        for (rc = Fsfirst(argv[i],0x07), n = 0; rc == 0; rc = Fsnext()) {
            strcpy(p,dta->d_fname);     /* add name to path */
            rc = Fopen(name,0);
            if (rc < 0L)
                break;
            handle = LOWORD(rc);
            linecount = 0L;
            do {
                n = rc = Fread(handle,bufsize,iobuf);
                if (rc < 0L)
                    break;
                rc = outputbuf(iobuf,n,paging);
                if (rc < 0L)
                    break;
                if (rc != n)
                    rc = DISK_FULL;
            } while(rc > 0L);
            Fclose(handle);
            if (rc < 0L)
                break;
        }
        if (rc == ENMFIL)   /* not really an error */
            rc = 0L;
        if (rc < 0L)
            break;
    }

    Mfree(iobuf);

    if (paging && (rc == USER_BREAK))   /* avoid 'interrupted' msg */
        rc = 0L;

    return rc;
}

/*
 *  extract next dirname from input path (includes any terminating backslash)
 *
 *  returns pointer to next starting point in input path
 */
PRIVATE char *next_dir(char *out,char *in)
{
    while(*in) {
        *out++ = *in++;
        if (*(in-1) == '\\')
            break;
    }
    *out = '\0';

    return in;
}

/*
 *  back up one level of directory in an (assumed) absolute path
 *
 *  returns pointer to char following the backslash at the end of
 *  the next higher level
 */
PRIVATE char *prev_dir(char *p)
{
    /* don't backup past root! */
    if (*--p == '\\')
        if (*(p-1) == ':')
            return p+1;

    while(*--p != '\\')
        ;

    return p+1;
}

/*
 *  convert name to absolute path
 */
PRIVATE LONG make_absolute(char *out,char *in)
{
char *p, *q, temp[MAXPATHLEN];
LONG rc = 0L;

    p = in;
    q = out;

    /* set up initial drive letter and : separator */
    if (*(p+1) == ':') {
        *q++ = toupper(*p);
        p += 2;
    } else {
        *q++ = Dgetdrv() + 'A';
    }
    *q++ = ':';

    /* insert current path if specified path is relative */
    if (*p != '\\') {
        if (Dgetpath(temp,out[0]-'A'+1) != 0)   /* e.g. invalid drive */
            temp[0] = '\0';
        strcpy(q,temp);
        q += strlen(q);
    } else p++;
    *q++ = '\\';

    /* copy dirnames one at a time, with special processing for . and .. */
    while(*p) {
        p = next_dir(temp,p);
        if (temp[0] == '.') {
            if ((temp[1] == '\\') || (temp[1] == '\0'))     /* got '.':       */
                continue;                                   /*  nothing to do */
            if ((temp[1] == '.')
             && ((temp[2] == '\\') || (temp[2] == '\0'))) { /* got '..':         */
                q = prev_dir(q);                            /* move up one level */
                continue;
            }
            rc = EPTHNF;
            break;
        }
        strcpy(q,temp);
        q += strlen(q);
    }

    *q = '\0';

    return rc;
}

/*
 *  copy_move
 */
PRIVATE LONG copy_move(WORD argc,char **argv,WORD delete)
{
char inname[MAXPATHLEN], outname[MAXPATHLEN], fullname[MAXPATHLEN];
char *inptr, *outptr;
WORD in, out, output_is_dir = 0;
char *iobuf;
LONG bufsize, n, rc;

    inptr = extract_path(inname,argv[1]);
    outptr = extract_path(outname,argv[2]);

    /* count files to be copied */
    for (rc = Fsfirst(inname,0x07), n = 0; rc == 0; rc = Fsnext())
        n++;

    /*
     * determine if target is a valid directory
     */
    rc = make_absolute(fullname,outname);   /* convert to absolute path */
    if (rc == 0L)
        rc = check_path_component(fullname);
    if (rc == 0L)                           /* it's an existing directory */
        output_is_dir = 1;
    else if ((rc == NOT_DIRECTORY) || (rc == EFILNF))   /* it's a file or non-existent */
        rc = 0L;

    /*
     * if error detected OR (multiple input files and output isn't a directory),
     * it's an error
     */
    if ((rc < 0L)
     || ((n > 1) && !output_is_dir)) {
        message(outname);
        messagenl(_(" is not a directory"));
        return 0;           /* because we already issued a message */
    }

    /*
     * for the remainder of processing, we use the converted output name
     */
    outptr = extract_path(outname,fullname);
    if (output_is_dir) {
        outptr += strlen(outptr);
        if ((*(outptr-1) != '\\') && (*(outptr-1) != ':'))
            *outptr++ = '\\';
        *outptr = '\0';
    }

    bufsize = IOBUFSIZE;
    iobuf = (char *)Malloc(bufsize);
    if (!iobuf)
        return ENSMEM;

    for (rc = Fsfirst(inname,0x07); rc == 0; rc = Fsnext()) {
        /* allow user to interrupt or pause before every file copy/move */
        if (constat()) {
            if (user_input(-1)) {
                rc = USER_BREAK;
                break;
            }
        }
        strcpy(inptr,dta->d_fname);     /* add name to paths */
        if (output_is_dir)
            strcpy(outptr,dta->d_fname);
        if (strequal(inname,outname)) {
            message(inname);
            message(_(" and "));
            message(outname);
            messagenl(" are the same file");
            continue;
        }

        message(_("Copying "));
        message(inname);
        message(_(" to "));
        message(outname);

        rc = Fopen(inname,0);
        if (rc < 0L)
            break;
        in = LOWORD(rc);

        rc = Fcreate(outname,0);
        if (rc < 0L) {
            Fclose(in);
            break;
        }
        out = LOWORD(rc);

        do {
            /* allow user to interrupt during file copy/move */
            if (constat()) {
                if (user_break()) {
                    rc = USER_BREAK;
                    break;
                }
            }
            n = rc = Fread(in,bufsize,iobuf);
            if (rc < 0L)
                break;
            rc = Fwrite(out,n,iobuf);
            if (rc < 0L)
                break;
            if (rc != n)
                rc = DISK_FULL;
        } while(rc > 0L);
        Fclose(in);
        Fclose(out);

        /* if the copy failed, delete the output to avoid an incomplete file */
        if (rc < 0L)
            Fdelete(outname);

        if (delete && (rc == 0L)) { /* don't delete unless copy successful */
            message(_(" ... deleting "));
            message(inptr);
            rc = Fdelete(inname);
        }

        if (rc < 0L) {
            message(" ... ");
            break;
        }

        messagenl(_(" ... done"));
    }
    if (rc == ENMFIL)   /* not really an error */
        rc = 0L;

    Mfree(iobuf);

    return rc;
}

/*
 *  get specified drive's current path (including drive letter) into buffer
 *
 *  Note: drive is specified as for Dgetpath(): for current drive,
 *  use 0, otherwise use the drive number + 1
 *
 *  returns error code from Dgetpath()
 */
LONG get_path(char *buf,WORD drive)
{
LONG rc;
char *p = buf;

    *p++ = 'A' + (drive ? drive-1 : Dgetdrv());
    *p++ = ':';
    *p = '\0';

    rc = Dgetpath(p,drive);
    if (!*p) {          /* the root */
        *p++ = '\\';
        *p = '\0';
    }

    return rc;
}

/*
 *  extract a path from a pathname
 *
 *  returns pointer to insertion point for name
 */
PRIVATE char *extract_path(char *dest,const char *src)
{
const char *p;
char *q, *sep = dest;

    for (p = src, q = dest; *p; ) {
        if ((*p == '\\') || (*p == ':'))
            sep = q + 1;
        *q++ = *p++;
    }
    *q = '\0';

    return sep;
}

PRIVATE void help_display(const COMMAND *p)
{
const char * const *s;

    output(p->name);
    if (p->synonym) {
        output(_(" or "));
        output(p->synonym);
    }
    output(" ");
    outputnl(p->help[0]);

    for (s = &p->help[1]; *s; s++) {
        output("  ");
        outputnl(gettext(*s));
    }
}

PRIVATE WORD help_lines(const COMMAND *p)
{
const char * const *s;
WORD lines;

    for (s = &p->help[0], lines = 0; *s; s++, lines++)
        ;

    return lines;
}

PRIVATE WORD help_pause(void)
{
WORD rc;
char c;

    message(_("CR to continue ..."));
    while(1) {
        c = LOBYTE(conin());
        if (c == '\r') {
            rc = 0;
            break;
        }
        if (c == CTL_C) {
            rc = -1;
            break;
        }
    }

    blank_line();   /* blank out the pause msg for neatness */

    return rc;
}

PRIVATE WORD help_wanted(const COMMAND *p,char *cmd)
{
    if (strequal(cmd,"all"))
        return 1;
    if (strequal(cmd,p->name))
        return 1;
    if (p->synonym)
        if (strequal(cmd,p->synonym))
            return 1;

    return 0;
}

PRIVATE void fixup_filespec(char *filespec)
{
char *p, *q;
WORD drive;

    if (!filespec[0])
        strcpy(filespec,"*.*");
    for (p = filespec; *p; p++)
        ;
    if (*(p-1) == ':') {        /* add current path for drive */
        drive = shellutl_get_drive_number(*(p-2));
        Dgetpath(p,drive+1);
        for ( ; *p; p++)
            ;
        *p++ = '\\';
        *p = '\0';
    }
    if (*(p-1) == '\\') {
        strcpy(p,"*.*");
        p += 3;
    }

    strupper(filespec);

    for (q = p; (*q != '\\') && (q >= filespec); q--)
        if (*q == '*')  /* wildcard, no more tweaks */
            return;

    /* no wildcards, see if we're pointing to a dir */
    if (Fsfirst(filespec,0xff) == 0)
        if (!(dta->d_attrib & 0x10))    /* not a dir */
            return;

    /* we assume the user wants to list the contents of the directory */
    strcpy(p,"\\*.*");
}

PRIVATE void padname(char *buf,const char *name)
{
WORD i;

    for (i = 0; *name; i++)
        *buf++ = *name++;

    for ( ; i < 13; i++)
        *buf++ = ' ';

    *buf = '\0';
}

PRIVATE void display_dta_detail(void)
{
char buf[80], *p = buf;

    p += sprintf(buf,"%-13.13s",dta->d_fname);
    p += decode_date_time(p,dta->d_date,dta->d_time);

    if (dta->d_attrib & 0x10) {
        strcpy(p,"<dir>");
    } else {
        sprintf(p,"%15lu",dta->d_length);
        p++;
        if (dta->d_attrib & 0x01)
            *p++= 'r';
        if (dta->d_attrib & 0x02)
            *p++ = 'h';
        if (dta->d_attrib & 0x04)
            *p = 's';
        if (!(dta->d_attrib & 0x07))
            *p = '-';
    }
    outputnl(buf);
}

PRIVATE LONG is_valid_drive(char drive_letter)
{
ULONG drvbits;
WORD drive_number;

    drive_number = shellutl_get_drive_number(drive_letter);

    if ((drive_number < 0) || (drive_number >= BLKDEVNUM))
        return 0;

    drvbits = Dsetdrv(Dgetdrv());
    return (drvbits & (1L << drive_number)) ? 1 : 0;
}

/*
 *  checks a pathname
 *
 *  returns 0 if it specifies a directory
 *          NOT_DIRECTORY if it specifies a file
 *          else a negative error code
 */
PRIVATE LONG check_path_component(char *component)
{
char *p;
WORD fixup;
LONG rc;
    /*
     * if drive specified, validate it and check
     * for "X:" and "X:\" directory specifications
     */
    if (component[1] == ':') {
        if (!is_valid_drive(*component))
            return EDRIVE;
        p = component + 2;
        if (*p == '\\')
            p++;
        if (!*p)              /* X: and X:\ are valid directories */
            return 0L;
    }

    for (p = component; *p; p++) {      /* scan thru string */
        if ((*p == '?') || (*p == '*'))
            return EPTHNF;
    }

    fixup = (*(p-1) == '\\');

    if (fixup)
        *--p = '\0';

    rc = Fsfirst(component,0x17);
    if ((rc == 0L) && ((dta->d_attrib&0x10) != 0x10))
        rc = NOT_DIRECTORY;     /* a file, not a directory */

    if (fixup)
        *p = '\\';

    return rc;
}

PRIVATE char getyn(void)
{
char c;

    message(" (y/N)?");
    c = LOBYTE(conin()) | 0x20;
    message("\r\n");

    return c;
}

PRIVATE void show_line(const char *title,ULONG n)
{
char buf[80];

    sprintf(buf,"%s%10lu",title,n);
    outputnl(buf);
}

/*
 *  output() and friends
 */
/*
 *  output a redirectable string
 */
PRIVATE void output(const char *s)
{
    if (redir_handle < 0L)
        message(s);
    else Fwrite((WORD)redir_handle,strlen(s),s);
}

/*
 *  output a redirectable string with a newline
 */
PRIVATE void outputnl(const char *s)
{
    output(s);
    output("\r\n");
}

/*
 *  output a redirectable fixed-length buffer
 *  with screen paging if requested
 */
PRIVATE LONG outputbuf(const char *s,LONG len,WORD paging)
{
LONG n, rc;
char c, cprev = 0, response;

    if (redir_handle < 0L) {
        n = len;
        while(n-- > 0) {
            if (constat())
                if (user_input(-1))
                    return USER_BREAK;
            c = *s++;
            /* convert Un*x-style text to TOS-style */
            if ((c == '\n') && (cprev != '\r'))
                conout('\r');
            conout(c);
            if (paging && (c == '\n')) {
                if (++linecount >= screen_rows-1) {
                    message(_("--More--"));
                    while(1) {
                        response = LOBYTE(conin());
                        if (response == '\r')   /* CR displays the next line */
                            break;
                        if (response == ' ') {  /* space displays the next page */
                            linecount = 0L;
                            break;
                        }
                        if (user_input(response)) {
                            blank_line();
                            return USER_BREAK;
                        }
                    }
                    blank_line();               /* overwrite the pause msg */
                }
            }
            cprev = c;
        }
        return len;
    }

    rc = Fwrite((WORD)redir_handle,len,s);
    if (rc >= 0) {
        if (constat())
            if (user_break())
                return USER_BREAK;
    }

    return rc;
}

/*
 *  handle control-C
 */
PRIVATE WORD user_break(void)
{
char c;

    c = LOBYTE(conin());
    if (c == CTL_C)         /* user wants to interrupt */
        return -1;

    return 0;
}

/*
 *  check for flow control or quit (ctl-C/Q/q)
 *
 *  a +ve argument is the character to check
 *  a -ve argument means get the character from conin
 */
PRIVATE WORD user_input(WORD c)
{
    if (c < 0)
        c = LOBYTE(conin());

    if ((c == CTL_C) || (c == 'Q') || (c == 'q'))   /* wants to quit */
        return -1;

    if (c == CTL_S) {       /* user wants to pause */
        while(1) {
            c = LOBYTE(conin());
            if (c == CTL_C)
                return -1;
            if (c == CTL_Q)
                break;
        }
    }

    return 0;
}
