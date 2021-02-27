/*
 *  localise: an EmuTOS localisation tool
 *
 *  Copyright 2021 Roger Burrows
 *
 *  This program is licensed under the GNU General Public License.
 *  Please see LICENSE.TXT for details.
 */

/*
 *  This program is designed to manage country-specific aspects of the
 *  EmuTOS build process; these aspects are defined in a control file.
 *  The control file contains a series of text lines, one for each
 *  country supported by EmuTOS.  Lines beginning with '#' are treated
 *  as comments and ignored; other lines are assumed to be a sequence
 *  of text strings separated by white space.  The sequence of strings
 *  in each line is as follows:
 *      . country (two-character abbreviation, case is ignored)
 *      . language (two-character abbreviation, case is ignored)
 *      . keyboard (two-character abbreviation, case is ignored)
 *      . character set (two-character abbreviation, case is ignored)
 *      . IDT (date/time) info (string using #define'd strings, case is important)
 *      . group (1 or 2 characters, case is ignored).  note that a special
 *        value of * matches all values in the -g argument
 *  Input arguments (see below) are used to extract the relevant data.
 *
 *  The following files are created:
 *      . ctables.h, used by bios/country.c
 *      . i18nconf.h, used by multi-language support
 *      . LINGUAS, used by 'bug make'
 *
 *
 *  Syntax: localise [-v] [-u[<unique>]] [-g[<group>]] <country> <ctlfile> <tblfile> <i18nfile> <LINGfile>
 *
 *      where:
 *          -v          is optional; if set, the program is more talkative
 *          -u[<unique>] is optional and specifies a unique country name; if set,
 *                      the generated files will be set up to produce a version
 *                      of EmuTOS that supports only country <unique>.  if the
 *                      value <unique> is omitted, it will be treated as though
 *                      the whole option was omitted.
 *          -g[<group>] is optional and specifies a group name; countries with
 *                      matching values in the control file will be included
 *                      (only when multi-country output is produced).  if the
 *                      value <group> is omitted, it will be treated as though
 *                      the whole option was omitted.
 *          <ctlfile>   is the name of the control file
 *          <tblfile>   is the name of the ctables.h file
 *          <i18nfile>  is the name of the i18nconf.h file
 *          <LINGfile>  is the name of the LINGUAS file
 *
 *
 *  version history
 *  ---------------
 *  v1.0    roger burrows, february/2021
 *          initial release
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>

#define BOOL    int
#define TRUE    1
#define FALSE   0


/*
 *  our own defines & structures
 */
#define PROGRAM_NAME    "localise"
#define VERSION         "v1.0"

/*
 * base language: the language used in the resources & message texts
 */
#define BASE_LANGUAGE   "us"

#define MAX_CTL_LINES   100             /* max number of non-comment control lines */
#define MAX_LINELEN     255             /* (excluding terminator) for control file lines */
#define NUM_CTL_FIELDS  6               /* fields per line */
#define MAX_FLDLEN      2               /* (excluding terminator) for all fields except idt */
#define MAX_STRLEN      (MAX_FLDLEN+1)

typedef struct                      /* for input control file */
{
    BOOL ignore;                        /* marks entries to temporarily ignore */
    char country[MAX_STRLEN];
    char language[MAX_STRLEN];
    char keyboard[MAX_STRLEN];
    char charset[MAX_STRLEN];
    char *idt;
    char group[MAX_STRLEN];
} CONTROL;

/*
 *  font names
 */
#define FNT_ST_SMALL    "fnt_st_6x6"
#define FNT_ST_MEDIUM   "fnt_st_8x8"
#define FNT_ST_LARGE    "fnt_st_8x16"
#define FNT_L2_SMALL    "fnt_l2_6x6"
#define FNT_L2_MEDIUM   "fnt_l2_8x8"
#define FNT_L2_LARGE    "fnt_l2_8x16"
#define FNT_GR_SMALL    "fnt_gr_6x6"
#define FNT_GR_MEDIUM   "fnt_gr_8x8"
#define FNT_GR_LARGE    "fnt_gr_8x16"
#define FNT_RU_SMALL    "fnt_ru_6x6"
#define FNT_RU_MEDIUM   "fnt_ru_8x8"
#define FNT_RU_LARGE    "fnt_ru_8x16"


/*
 *  other globals
 */
const char *copyright = PROGRAM_NAME " " VERSION " copyright (c) 2021 by Roger Burrows\n"
"This program is licensed under the GNU General Public License.\n"
"Please see LICENSE.TXT for details.\n";

int verbose = 0;

char unique[MAX_STRLEN] = "";       /* input */
char group[MAX_STRLEN] = "";
char *ctl_file = NULL;

char *tbl_file = NULL;              /* output files */
char *i18n_file = NULL;
char *LING_file = NULL;

CONTROL control[MAX_CTL_LINES];
int control_count = 0;              /* number of entries in above array */
CONTROL *unique_ptr = NULL;         /* pointer to entry for country 'unique' */

int needed_charsets = 0;            /* charsets required */
#define NEED_ST_CSET    0x01        /* internal bit encoding for 'needed_charsets' */
#define NEED_L2_CSET    0x02
#define NEED_GR_CSET    0x04
#define NEED_RU_CSET    0x08

char keytable[MAX_CTL_LINES][MAX_STRLEN];   /* holds unique keyboard names from control[] */
int keytable_count = 0;

char now[26];                       /* for current date/time */



/*
 ***** general-purpose functions *****
 */

/*
 * check if control record group matches group argument
 */
static BOOL group_matches(CONTROL *entry)
{
    /* an empty group specification always matches */
    if (!group[0])
        return TRUE;

    /* an asterisk in the control record always matches */
    if (strcmp(entry->group, "*") == 0)
        return TRUE;

    if (strcmp(entry->group, group) == 0)
        return TRUE;

    return FALSE;
}

/*
 * convert a character set string to a flag bit
 */
static int decode_charset(char *charset)
{
    if (strcmp(charset, "st") == 0)
        return NEED_ST_CSET;

    if (strcmp(charset, "l2") == 0)
        return NEED_L2_CSET;

    if (strcmp(charset, "gr") == 0)
        return NEED_GR_CSET;

    if (strcmp(charset, "ru") == 0)
        return NEED_RU_CSET;

    return -1;
}

/*
 * return a bitmap of character sets needed
 */
static int get_charsets(void)
{
    int i;
    CONTROL *entry;

    if (unique_ptr)
    {
        needed_charsets = decode_charset(unique_ptr->charset);
    }
    else
    {
        for (i = 0, entry = control; i < control_count; i++, entry++)
        {
            if (group_matches(entry))
                needed_charsets |= decode_charset(entry->charset);
        }
    }

    if (needed_charsets <= 0)
        fprintf(stderr, "unknown character set specification in control file\n");

    return needed_charsets;
}

/*
 * return bug-style destination charset name
 */
static char *get_dest_charset(char *charset)
{
    char *p;

    switch(decode_charset(charset))
    {
    default:
        p = "atarist";
        break;
    case NEED_L2_CSET:
        p = "latin2";
        break;
    case NEED_GR_CSET:
        p = "cp737";
        break;
    case NEED_RU_CSET:
        p = "russian-atarist";
        break;
    }

    return p;
}

/*
 * clear the 'ignore' flag in all control table entries
 */
static void clear_ignore_flags(void)
{
    int i;
    CONTROL *entry;

    for (i = 0, entry = control; i < control_count; i++, entry++)
        entry->ignore = FALSE;
}

/*
 * set the 'ignore' flag in all entries with a matching keyboard value
 */
static void mark_matching_keyboard(char *keyboard)
{
    int i;
    CONTROL *entry;

    for (i = 0, entry = control; i < control_count; i++, entry++)
    {
        if (strcmp(keyboard, entry->keyboard) == 0)
            entry->ignore = TRUE;
    }
}

/*
 * set the 'ignore' flag in all entries with a matching language value
 */
static void mark_matching_language(char *language)
{
    int i;
    CONTROL *entry;

    for (i = 0, entry = control; i < control_count; i++, entry++)
    {
        if (strcmp(language, entry->language) == 0)
            entry->ignore = TRUE;
    }
}

/*
 * convert string to lowercase in situ
 *
 * returns ptr to string
 */
static char *strlwr(char *str)
{
    char *p;

    for (p = str; *p; p++)
        *p = tolower(*p);

    return str;
}

/*
 * copy up to MAX_FLDLEN characters, lowercasing them
 */
static void copy2lwr(char *p, char *q)
{
    strncpy(p, q, MAX_FLDLEN);
    p[MAX_FLDLEN] = '\0';
    strlwr(p);
}

/*
 * convert string to uppercase in situ
 *
 * returns ptr to string
 */
static char *strupr(char *str)
{
    char *p;

    for (p = str; *p; p++)
        *p = toupper(*p);

    return str;
}

/*
 * copy up to MAX_FLDLEN characters, uppercasing them
 */
static void copy2upr(char *p, char *q)
{
    strncpy(p, q, MAX_FLDLEN);
    p[MAX_FLDLEN] = '\0';
    strupr(p);
}

/*
 *  allocate memory & copy string to it
 *
 *  this is the same as the quasi-standard function strdup();
 *  however, strdup() is not available when compiling under gcc
 *  with the -ansi option.
 */
static char *xstrdup(const char *string)
{
char *p;

    p = malloc(strlen(string)+1);
    if (p)
        strcpy(p,string);

    return p;
}

/*
 * issue 'usage' message
 */
static void usage(char *s)
{
    if (*s)
        fprintf(stderr,"%s %s: %s\n", PROGRAM_NAME, VERSION, s);
    fprintf(stderr,"usage: %s [-v] [-u[<unique>]] [-g[<group>]] <ctlfile> <tblfile> <i18nfile> <LINGfile>\n", PROGRAM_NAME);

    exit(1);
}



/*
 ***** input routines *****
 */

/*
 *  read control file
 *
 *  returns -1 iff if there's a problem reading the file
 */
static int read_control_file(char *path)
{
    FILE *fp;
    int n, rc = 0;
    char s[MAX_LINELEN+1], idt[MAX_LINELEN+1];
    CONTROL *p, entry;

    fp = fopen(path, "r");
    if (!fp)
    {
        fprintf(stderr, "can't open control file %s\n", path);
        return -1;
    }

    p = control;
    while(fgets(s, MAX_LINELEN, fp))
    {
        if (s[0] == '#')            /* comment */
            continue;

        s[MAX_LINELEN] = '\0';      /* ensure nul termination */
        n = sscanf(s, "%2s %2s %2s %2s %s %2s", entry.country, entry.language,
                    entry.keyboard, entry.charset, idt, entry.group);

        if (n <= 0)                 /* empty line, treat as comment */
            continue;

        if (n != NUM_CTL_FIELDS)
        {
            fprintf(stderr, "expected %d fields in control line, found %d\n",
                    NUM_CTL_FIELDS, n);
            rc = -1;
            break;
        }

        if (++control_count > MAX_CTL_LINES)
        {
            fprintf(stderr, "too many lines in control file\n");
            rc = -1;
            break;
        }

        p->ignore = FALSE;
        strcpy(p->country, strlwr(entry.country));
        strcpy(p->language, strlwr(entry.language));
        strcpy(p->keyboard, strlwr(entry.keyboard));
        strcpy(p->charset, strlwr(entry.charset));
        p->idt = xstrdup(idt);
        strcpy(p->group, strlwr(entry.group));

        if (!p->idt)
        {
            fprintf(stderr, "not enough memory to save idt\n");
            rc = -1;
            break;
        }

        p++;
    }

    return rc;
}

static int validate_control_file(void)
{
    int i, rc = 0;
    CONTROL *entry;
    BOOL found_group = group[0] ? FALSE : TRUE;
    BOOL found_unique = unique[0] ? FALSE : TRUE;

    for (i = 0, entry = control; i < control_count; i++, entry++)
    {
        if (strcmp(unique, entry->country) == 0)
        {
            if (found_unique)
            {
                fprintf(stderr, "duplicate entry for unique country %s in control file\n", unique);
                rc = -1;
            }
            found_unique = TRUE;
            unique_ptr = entry;
        }
        if (strcmp(group, entry->group) == 0)
            found_group = TRUE;
        if (decode_charset(entry->charset) < 0)
        {
            fprintf(stderr, "invalid character set %s specified for country %s in control file\n",
                    entry->charset, entry->country);
            rc = -1;
        }
    }

    if (!found_unique)
    {
        fprintf(stderr, "specified unique country %s not found in control file\n", unique);
        rc = -1;
    }
    if (!found_group)
    {
        fprintf(stderr, "specified group %s not found in control file\n", group);
        rc = -1;
    }

    return rc;
}

static void build_keytable(void)
{
    int i;
    CONTROL *entry;

    for (i = 0, entry = control; i < control_count; i++, entry++)
    {
        if (group_matches(entry))
        {
            if (!entry->ignore)
            {
                strcpy(keytable[keytable_count++], entry->keyboard);
                /* make sure we only include the keyboard once */
                mark_matching_keyboard(entry->keyboard);
            }
        }
    }
}


/*
 ***** output ctables.h *****
 */
static void write_tbl_header(FILE *fp)
{
    fprintf(fp, "/*\n");
    fprintf(fp, " * ctables.h - country tables for bios/country.c\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * This file was automatically generated by %s %s\n",
                PROGRAM_NAME, VERSION);
    fprintf(fp, " * on %s - do not alter!\n", now);
    fprintf(fp, " */\n\n");
}

static void write_font_stuff(FILE *fp)
{
    int index = 0;

    /*
     * create defines for indexes into font sets
     */
    fprintf(fp, "/* Indexes of font sets inside font_sets[] */\n");
    if (needed_charsets & NEED_ST_CSET)
        fprintf(fp, "#define CHARSET_ST %d\n", index++);
    if (needed_charsets & NEED_L2_CSET)
        fprintf(fp, "#define CHARSET_L2 %d\n", index++);
    if (needed_charsets & NEED_GR_CSET)
        fprintf(fp, "#define CHARSET_GR %d\n", index++);
    if (needed_charsets & NEED_RU_CSET)
        fprintf(fp, "#define CHARSET_RU %d\n", index++);
    fprintf(fp, "\n");

    /*
     * create externs
     */
    fprintf(fp, "/* Externs for font sets inside font_sets[] */\n");
    if (needed_charsets & NEED_ST_CSET)
    {
        fprintf(fp,"extern const Fonthead %s;\n", FNT_ST_SMALL);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_ST_MEDIUM);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_ST_LARGE);
    }
    if (needed_charsets & NEED_L2_CSET)
    {
        fprintf(fp,"extern const Fonthead %s;\n", FNT_L2_SMALL);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_L2_MEDIUM);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_L2_LARGE);
    }
    if (needed_charsets & NEED_GR_CSET)
    {
        fprintf(fp,"extern const Fonthead %s;\n", FNT_GR_SMALL);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_GR_MEDIUM);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_GR_LARGE);
    }
    if (needed_charsets & NEED_RU_CSET)
    {
        fprintf(fp,"extern const Fonthead %s;\n", FNT_RU_SMALL);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_RU_MEDIUM);
        fprintf(fp,"extern const Fonthead %s;\n", FNT_RU_LARGE);
    }
    fprintf(fp, "\n");

    /*
     * create font_sets[]
     */
    fprintf(fp, "static const struct charset_fonts font_sets[] = {\n");
    if (needed_charsets & NEED_ST_CSET)
        fprintf(fp, "    { &%s, &%s, &%s },\n", FNT_ST_SMALL, FNT_ST_MEDIUM, FNT_ST_LARGE);
    if (needed_charsets & NEED_L2_CSET)
        fprintf(fp, "    { &%s, &%s, &%s },\n", FNT_L2_SMALL, FNT_L2_MEDIUM, FNT_L2_LARGE);
    if (needed_charsets & NEED_GR_CSET)
        fprintf(fp, "    { &%s, &%s, &%s },\n", FNT_GR_SMALL, FNT_GR_MEDIUM, FNT_GR_LARGE);
    if (needed_charsets & NEED_RU_CSET)
        fprintf(fp, "    { &%s, &%s, &%s },\n", FNT_RU_SMALL, FNT_RU_MEDIUM, FNT_RU_LARGE);
    fprintf(fp, "};\n\n");
}

static void write_keyb_stuff(FILE *fp)
{
    int i;
    char uc_keytable[MAX_STRLEN];

    /*
     * create #defines for indexes into keytables[]
     */
    if (!unique_ptr)
    {
        fprintf(fp, "/* Indexes of keyboard entries inside keytables[] */\n");
        for (i = 0; i < keytable_count; i++)
        {
            copy2upr(uc_keytable, keytable[i]);
            fprintf(fp, "#define KEYB_%s %d\n", uc_keytable, i);
        }
        fprintf(fp, "\n");
    }

    /*
     * create #includes for keyboard tables
     */
    if (unique_ptr)
    {
        fprintf(fp, "#include \"keyb_%s.h\"\n", unique_ptr->keyboard);
    }
    else
    {
        for (i = 0; i < keytable_count; i++)
            fprintf(fp, "#include \"keyb_%s.h\"\n", keytable[i]);
    }
    fprintf(fp, "\n");

    /*
     * create keytables[]
     */
    fprintf(fp, "static const struct keytbl *const keytables[] = {\n");

    if (unique_ptr)
    {
        fprintf(fp, "    &keytbl_%s\n", unique_ptr->keyboard);
    }
    else
    {
        for (i = 0; i < keytable_count; i++)
            fprintf(fp, "    &keytbl_%s,\n", keytable[i]);
    }
    fprintf(fp, "};\n\n");
}

static void write_countries(FILE *fp)
{
    int i;
    CONTROL *entry;
    char uc_country[MAX_STRLEN], uc_keyboard[MAX_STRLEN], uc_charset[MAX_STRLEN];

    /*
     * there is no countries[] array for a unique country
     */
    if (unique_ptr)
        return;

    fprintf(fp, "static const struct country_record countries[] = {\n");
    for (i = 0, entry = control; i < control_count; i++, entry++)
    {
        if (group_matches(entry))
        {
            copy2upr(uc_country, entry->country);
            copy2upr(uc_keyboard, entry->keyboard);
            copy2upr(uc_charset, entry->charset);
            fprintf(fp, "    { COUNTRY_%s, \"%s\", KEYB_%s, CHARSET_%s, %s },\n",
                    uc_country, entry->language, uc_keyboard, uc_charset, entry->idt);
        }
    }
    fprintf(fp, "};\n\n");
}

static int write_tbl_file(char *path)
{
    FILE *fp;

    fp = fopen(path, "w");
    if (!fp)
        return -1;

    write_tbl_header(fp);

    /*
     * create defines, externs, and array for font_sets[]
     */
    write_font_stuff(fp);

    /*
     * create #defines, #includes, and array for keytables[]
     */
    write_keyb_stuff(fp);

    /*
     * create countries[] array
     */
    write_countries(fp);

    fclose(fp);

    return 0;
}


/*
 ***** output i18nconf.h *****
 */
static void write_i18n_header(FILE *fp)
{
    fprintf(fp, "/*\n");
    fprintf(fp, " * i18nconf.h - internationalisation header\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * This file was automatically generated by %s %s\n",
                PROGRAM_NAME, VERSION);
    fprintf(fp, " * on %s - do not alter!\n", now);
    fprintf(fp, " */\n");
}

static int write_i18n_file(char *path)
{
    FILE *fp;

    fp = fopen(path, "w");
    if (!fp)
        return -1;

    write_i18n_header(fp);

    if (unique_ptr)
    {
        fprintf(fp, "#define CONF_MULTILANG 0\n");
        fprintf(fp, "#define CONF_WITH_NLS 0\n");

        /*
         * because we automatically generate the correct entries in
         * keytables[] and fontsets[], rather than using the preprocessor,
         * we no longer need to generate CONF_KEYB & CONF_CHARSET here
         */

        fprintf(fp, "#define CONF_IDT (%s)\n", unique_ptr->idt);
    }
    else
    {
        fprintf(fp, "#define CONF_MULTILANG 1\n");
        fprintf(fp, "#define CONF_WITH_NLS 1\n");
    }

    fclose(fp);

    return 0;
}


/*
 ***** output LINGUAS *****
 */
static void write_LING_header(FILE *fp)
{
    fprintf(fp, "#\n");
    fprintf(fp, "# LINGUAS - languages file for 'bug'\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# This file contains a mapping of languages to character sets.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# For multi-language flavours of EmuTOS, The translations for\n");
    fprintf(fp, "# all languages listed here will be included in langs.c by\n");
    fprintf(fp, "# 'bug make'.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# For single-language flavours of EmuTOS, 'bug translate' will\n");
    fprintf(fp, "# use this file to determine the appropriate character set.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# Each line consists of a two-character language specification\n");
    fprintf(fp, "# followed by the destination character set.\n");
    fprintf(fp, "#\n");
    fprintf(fp, "# This file was automatically generated by %s %s\n",
                PROGRAM_NAME, VERSION);
    fprintf(fp, "# on %s - do not alter!\n", now);
    fprintf(fp, "#\n");
}

static int write_LING_file(char *path)
{
    int i;
    CONTROL *entry;
    FILE *fp;

    fp = fopen(path, "w");
    if (!fp)
        return -1;

    write_LING_header(fp);

    clear_ignore_flags();   /* reset all ignore flags in table */

    for (i = 0, entry = control; i < control_count; i++, entry++)
    {
        /* LINGUAS never includes the 'base' language */
        if (strcmp(entry->language, BASE_LANGUAGE) == 0)
            continue;

        if (group_matches(entry))
        {
            if (entry->ignore)
                continue;

            fprintf(fp, "%s %s\n", entry->language, get_dest_charset(entry->charset));

            /* make sure we only include the language once */
            mark_matching_language(entry->language);
        }
    }

    fclose(fp);

    return 0;
}



int main(int argc,char *argv[])
{
    int n;
    time_t t;

    while((n=getopt(argc, argv, "g::u::v")) != -1)
    {
        switch(n) {
        case 'g':
            if (optarg)
                copy2lwr(group, optarg);
            break;
        case 'u':
            if (optarg)
                copy2lwr(unique, optarg);
            break;
        case 'v':
            verbose++;
            break;
        default:
            usage("invalid option");
            break;
        }
    }

    if (verbose)
        fputs(copyright, stderr);       /* announce us ... */

    if (argc-optind != 4)
        usage("incorrect number of arguments");

    ctl_file = argv[optind++];
    tbl_file = argv[optind++];
    i18n_file = argv[optind++];
    LING_file = argv[optind];

    if (verbose) {
        fprintf(stderr, "Unique: %s / Group: %s\n",
                unique[0]?unique:"(none)", group[0]?group:"(none)");
        fprintf(stderr, "Input file: %s\n", ctl_file);
        fprintf(stderr, "Output files: %s, %s, %s\n",
                tbl_file, i18n_file, LING_file);
    }

    if (read_control_file(ctl_file) < 0)
        exit(1);    /* message was already issued */

    if (validate_control_file() < 0)
        exit(1);

    if (get_charsets() <= 0)
        exit(1);

    build_keytable();

    /* get date/time for output file headers */
    time(&t);
    strcpy(now, asctime(localtime(&t)));
    now[24] = '\0'; /* remove trailing newline */

    if (write_tbl_file(tbl_file) < 0)
        exit(1);
    if (write_i18n_file(i18n_file) < 0)
        exit(1);
    if (write_LING_file(LING_file) < 0)
        exit(1);

    if (verbose)
        fprintf(stderr, PROGRAM_NAME " " VERSION " completed successfully\n");

    return 0;
}
