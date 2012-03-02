/*
 *  erd: the EmuTOS Resource Decompiler
 *
 *  Copyright 2012 by Roger Burrows
 *
 *  This program is licensed under the GNU Public License.
 *  Please see LICENSE.TXT for details.
 *
 *
 *  This program is designed to decompile the EmuTOS desktop RSC file
 *  to .c and .h files, which are input to the EmuTOS make process.
 *  It is very similar to a general-purpose resource decompiler, but
 *  has a number of EmuTOS-specific features.
 *
 *  Syntax: erd [-d] [-p<prefix>] [-v] <RSCfile> <Cfile>
 *
 *      where:
 *          -d          requests debugging output
 *          -p          specifies the prefix to apply to the names
 *                      of all generated data items
 *          -v          requests verbose output (start & end messages)
 *          <RSCfile>   identifies the input RSC/definition files
 *          <Cfile>     identifies the output .c/.h files
 *  Note: file extensions should be omitted for <RSCfile> and <Cfile>
 *
 *
 *  Processing overview
 *  -------------------
 *  The program reads the file <RSCfile>.rsc and the corresponding
 *  definition file.  Since there are different formats (and extensions)
 *  for the latter, it tries the possible extensions in this order:
 *      .HRD        most recent, supports mixed case names up to 16 bytes
 *      .DEF/.RSD   original, upper case names up to 8 bytes
 *      .DFN        slightly simplified version of .DEF/.RSD, with flag
 *                  values in a different sequence
 *  It then generates the files <Cfile>.c and <Cfile>.h, based on the
 *  contents of the input files.
 *
 *  The .c file contains arrays of structures for the following types of
 *  data, in this order: TEDINFO, ICONBLK, BITBLK, OBJECT, tree, free
 *  string (the latter includes alerts).  The .c file also contains the
 *  code for a minimal routine to copy data items that will be modified
 *  (TEDINFOs, OBJECTs) to globally-visible, modifiable structures.
 *
 *  The .h file contains the #defines for all OBJECTS, trees, and free
 *  strings.  It also contains externs for the structures in the .c file
 *  that are to be globally-visible.
 *
 *  For compatibility with the EmuTOS DESK1 compilation option, any free
 *  string or alert that contains the word "trash" will be surrounded by
 *  #ifdef DESK1 and #endif directives, as will the corresponding #define
 *  in the .h file.  This will reduce ROM memory requirements when DESK1
 *  is not #defined.
 *
 *  In order to save space in the generated resource file, the following
 *  additional steps are taken:
 *    . duplicate image data is automatically eliminated; multiple
 *      BITBLKs may point to the same image data.
 *    . duplicate icon data is automatically eliminated; multiple
 *      ICONBLKs may point to the same mask/data.
 *    . strings in objects are trimmed of trailing spaces.
 *    . strings that occur in more than one object (such as "OK") may
 *      optionally be assigned to separate variables which are then
 *      pointed to by more than one object.  This is done through the
 *      "shared string" array in the program.  Each entry in the array
 *      specifies a complete string; a match causes the corresponding
 *      string to be assigned to a separate variable.
 *    . the te_ptext pointer in TEDINFOs is always set to NULL; this field
 *      is initialised properly by the resource fixup code elsewhere in
 *      EmuTOS.
 *    . the TEDINFO validation string, pointed to by te_pvalid, is
 *      optimized by removing duplicate characters from the end of the
 *      validation string.
 *
 *  For compatibility with EmuTOS multi-language support, text strings
 *  (strings containing any character other than a space, a digit or
 *  punctuation) in the .c file are normally enclosed in the N_() macro.
 *  Additionally, strings consisting of all dashes (which are used to
 *  separate items in menus) are also enclosed in N_().  These actions
 *  may be overridden via the "no-translate" array in the program.
 *  Each entry in the array specifies a partial string to match; any
 *  text string beginning with any of the match strings will _not_ be
 *  enclosed by N_().
 *  
 *  The user may also specify (via the -p option) a prefix to be applied
 *  to the names of all the arrays in the .c file.  A prefix of e.g.
 *  "desk" will prefix each of the array names with "desk_".  If no
 *  prefix is supplied, but the input definition file is an HRD file,
 *  and it contains a prefix specification, that prefix will be used.
 *
 *
 *  version history
 *  ---------------
 *  v1.0    roger burrows, february/2012
 *          initial release
 *
 *  v1.1    roger burrows, march/2012
 *          . added comments to identify trees, objects and free strings
 *            by name
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef ATARI                        /* i.e. running under TOS */
#define DIRSEP  '\\'
#else
#include <getopt.h>
#define DIRSEP  '/'
#endif

/*
 *  manifest RSC-related stuff
 */
#define NEWRSC_FORMAT   0x04        /* if this bit is set in the RSC version, it's TOS4 format (w/CICONBLKs) */
#define DEF             "def"       /* valid extensions for definition file */
#define DFN             "dfn"
#define HRD             "hrd"
#define RSD             "rsd"
#define MAXLEN_HRD      16          /* max length of name allowed in HRD entry */
#define MAX_SUBSTR      5           /* maximum number of "substrings" in a free string (see getlen()) */

/*
 *  our version of standard AES stuff, with changes to accommodate
 *  alternate architectures
 */
#define GDOS_PROP       0   /* font types */
#define GDOS_MONO       1
#define GDOS_BITM       2
#define IBM             3
#define SMALL           5

#define G_BOX           20  /* object types */
#define G_TEXT          21
#define G_BOXTEXT       22
#define G_IMAGE         23
#define G_PROGDEF       24
#define G_USERDEF       G_PROGDEF
#define G_IBOX          25
#define G_BUTTON        26
#define G_BOXCHAR       27
#define G_STRING        28
#define G_FTEXT         29
#define G_FBOXTEXT      30
#define G_ICON          31
#define G_TITLE         32
#define G_CICON         33

#define NONE            0x0000  /* Object flags */
#define SELECTABLE      0x0001
#define DEFAULT         0x0002
#define EXIT            0x0004
#define EDITABLE        0x0008
#define RBUTTON         0x0010
#define LASTOB          0x0020
#define TOUCHEXIT       0x0040
#define HIDETREE        0x0080
#define INDIRECT        0x0100
#define FL3DMASK        0x0600
#define     FL3DNONE        0x0000
#define     FL3DIND         0x0200
#define     FL3DBAK         0x0400
#define     FL3DACT         0x0600
#define SUBMENU         0x0800

#define NORMAL          0x0000  /* Object states */
#define SELECTED        0x0001
#define CROSSED         0x0002
#define CHECKED         0x0004
#define DISABLED        0x0008
#define OUTLINED        0x0010
#define SHADOWED        0x0020
#define SPECIAL         0x0040  /* user defined object state */

#define TE_LEFT         0   /* editable text justification */
#define TE_RIGHT        1
#define TE_CNTR         2

typedef struct {        /* big-endian short */
    signed char hi;
    unsigned char lo;
} SHORT;

typedef struct {        /* big-endian unsigned short */
    unsigned char hi;
    unsigned char lo;
} USHORT;

typedef struct {
    unsigned char b1, b2, b3, b4;
} OFFSET;

typedef struct text_edinfo {
    OFFSET te_ptext;
    OFFSET te_ptmplt;
    OFFSET te_pvalid;
    SHORT te_font;
    SHORT te_fontid;
    SHORT te_just;
    SHORT te_color;
    SHORT te_fontsize;
    SHORT te_thickness;
    SHORT te_txtlen;
    SHORT te_tmplen;
} TEDINFO;

typedef struct icon_block {
    OFFSET ib_pmask;
    OFFSET ib_pdata;
    OFFSET ib_ptext;
    SHORT ib_char;
    SHORT ib_xchar;
    SHORT ib_ychar;
    SHORT ib_xicon;
    SHORT ib_yicon;
    SHORT ib_wicon;
    SHORT ib_hicon;
    SHORT ib_xtext;
    SHORT ib_ytext;
    SHORT ib_wtext;
    SHORT ib_htext;
} ICONBLK;

typedef struct bit_block {
    OFFSET bi_pdata;
    SHORT bi_wb;
    SHORT bi_hl;
    SHORT bi_x;
    SHORT bi_y;
    SHORT bi_color;
} BITBLK;

typedef struct object {
    SHORT ob_next;
    SHORT ob_head;
    SHORT ob_tail;
    USHORT ob_type;
    USHORT ob_flags;
    USHORT ob_state;
    OFFSET ob_spec;
    SHORT ob_x;
    SHORT ob_y;
    SHORT ob_width;
    SHORT ob_height;
} OBJECT;

typedef struct {
    USHORT rsh_vrsn;        /* RCS version # */
    USHORT rsh_object;      /* offset to object[] */
    USHORT rsh_tedinfo;     /* offset to tedinfo[] */
    USHORT rsh_iconblk;     /* offset to iconblk[] */
    USHORT rsh_bitblk;      /* offset to bitblk[] */
    USHORT rsh_frstr;       /* offset to free string index */
    USHORT rsh_string;      /* offset to first string */
    USHORT rsh_imdata;      /* offset to image data */
    USHORT rsh_frimg;       /* offset to free image index */
    USHORT rsh_trindex;     /* offset to object tree index */
    USHORT rsh_nobs;        /* number of objects */
    USHORT rsh_ntree;       /* number of trees */
    USHORT rsh_nted;        /* number of tedinfos */
    USHORT rsh_nib;         /* number of icon blocks */
    USHORT rsh_nbb;         /* number of bit blocks */
    USHORT rsh_nstring;     /* number of free strings */
    USHORT rsh_nimages;     /* number of free images */
    USHORT rsh_rssize;      /* total bytes in resource */
} RSHDR;


/*
 *  our own defines & structures
 */
#define PROGRAM_NAME    "erd"
#define VERSION         "v1.1"
#define MAX_STRLEN      200         /* max size for internal string areas */
#define NLS             "N_("       /* the macro used in EmuTOS for NLS support*/

/*
 *  internal representation of items in definition file
 */
typedef struct {
    short type;                     /* we use the HRD definitions */
#define DEF_DIALOG  0
#define DEF_MENU    1
#define DEF_ALERT   2
#define DEF_FREESTR 3               /* free string or text */
#define DEF_FREEBIT 4               /* free BITBLK or image */
#define DEF_OBJECT  5
#define DEF_EOF     6               /* EOF marker */
#define DEF_PREFIX  7               /* record indicates prefix */
    short seq;                      /* major sort sequence */
    short tree;
    short obj;
    char indicator;
    char desk1;                     /* 1 => conditional on DESK1 (DEF_ALERT/DEF_FREESTR only) */
    char *name;
} DEF_ENTRY;

/*
 *  shared strings
 */
typedef struct {
    char *string;
} SHARED_ENTRY;

/*
 *  no-translate prefixes
 */
typedef struct {
    short length;                   /* length of prefix */
    char *string;                   /* prefix string */
} NOTRANS_ENTRY;

/*
 *  globals
 */
char *copyright = PROGRAM_NAME " " VERSION " copyright (c) 2012 by Roger Burrows\n"
"This program is licensed under the GNU Public License. Please see LICENSE.TXT for details.\n";

int debug = 0;                          /* options */
char prefix[MAX_STRLEN] = "";
int verbose = 0;

char *rsc_root = NULL;                  /* paths of input, output files */
char *out_path = NULL;
char defext[5];                         /* extension of definition file actually used */

char inrsc[MAX_STRLEN] = "";            /* actual file names */
char indef[MAX_STRLEN] = "";
char cfile[MAX_STRLEN] = "";
char hfile[MAX_STRLEN] = "";


RSHDR *rschdr = NULL;                   /* RSC header */

DEF_ENTRY *def = NULL;                  /* table of #defines from HRD/DEF/RSD/DFN */
int num_defs = 0;                       /* entries in def */

/*
 *  table of complete strings that will have a shared data item
 */
SHARED_ENTRY shared[] = {
    { "OK" },
    { "Cancel" },
    { "Install" },
    { "Remove" },
    { "Yes" },
    { "No" }
};
int num_shared = sizeof(shared) / sizeof(SHARED_ENTRY);

/*
 *  table of string prefixes for text that should not be translated
 */
NOTRANS_ENTRY notrans[] = {
    { 0, "- EmuTOS -" },
    { 0, "http://" },
    { 0, "GEM" },
    { 0, "TOS" },
    { 0, "TTP" },
    { 0, "#" },
    { 0, "A:" },
    { 0, "am" },
    { 0, "pm" },
    { 0, "(c)" }
};
int num_notrans = sizeof(notrans) / sizeof(NOTRANS_ENTRY);

/*
 *  table for decoding ob_flags
 */
typedef struct {
    short mask;
    char *desc;
} FLAGS;

FLAGS flaglist[] = {
    { SELECTABLE, "SELECTABLE" }, { DEFAULT,  "DEFAULT" },
    { EXIT,       "EXIT" },       { EDITABLE, "EDITABLE" },
    { RBUTTON,    "RBUTTON" },    { LASTOB,   "LASTOB" },
    { TOUCHEXIT,  "TOUCHEXIT" },  { HIDETREE, "HIDETREE" },
    { INDIRECT,   "INDIRECT" },   { FL3DBAK,  "FL3DBAK" },
    { FL3DIND,    "FL3DIND" },    { FL3DACT,  "FL3DACT" },
    { SUBMENU,    "SUBMENU" },    { 0,        NULL } };

/*
 *  table for decoding ob_state
 */
typedef struct {
    short mask;
    char *desc;
} STATE;

STATE statelist[] = {
    { SELECTED, "SELECTED" }, { CROSSED,  "CROSSED" },
    { CHECKED,  "CHECKED" },  { DISABLED, "DISABLED" },
    { OUTLINED, "OUTLINED" }, { SHADOWED, "SHADOWED" },
    { 0,        NULL } };

/*
 *  table for decoding ob_type
 */
typedef struct {
    short type;
    char *desc;
} TYPE;

TYPE typelist[] = {
    { G_BOX,      "G_BOX" },      { G_TEXT,    "G_TEXT" },
    { G_BOXTEXT,  "G_BOXTEXT" },  { G_IMAGE,   "G_IMAGE" },
    { G_PROGDEF,  "G_PROGDEF" },  { G_IBOX,    "G_IBOX" },
    { G_BUTTON,   "G_BUTTON" },   { G_BOXCHAR, "G_BOXCHAR" },
    { G_STRING,   "G_STRING" },   { G_FTEXT,   "G_FTEXT" },
    { G_FBOXTEXT, "G_FBOXTEXT" }, { G_ICON,    "G_ICON" },
    { G_TITLE,    "G_TITLE" },    { G_CICON,   "G_CICON" },
    { 0,          NULL } };


/*
 *  function prototypes
 */
int all_dashes(char *string);
int compare_icons(ICONBLK *b1,ICONBLK *b2);
int compare_images(BITBLK *b1,BITBLK *b2);
short convert_type(int deftype);
int copycheck(char *dest,char *src,int len);
void copyfix(char *dest,char *src,int len);
char *decode_flags(short flags);
char *decode_font(short font);
char *decode_ib_char(short iconchar);
char *decode_just(short just);
char *decode_state(short state);
char *decode_type(short type);
void display_defs(int entries);
void display_header(RSHDR *rschdr);
void display_notrans(int n);
void display_shared(int n);
void error(char *s,char *t);
int find(char *string,char *substring);
void fixshared(char *dest,char *src);
int getlen(int *length,char *s);
short get_short(SHORT *p);
unsigned short get_ushort(USHORT *p);
unsigned long get_offset(OFFSET *p);
void init_notrans(int n);
int load_definition(char *file);
int load_def(FILE *fp);
int load_dfn(FILE *fp);
int load_hrd(FILE *fp);
RSHDR *load_rsc(char *path);
DEF_ENTRY *lookup_freestr(int freestr);
DEF_ENTRY *lookup_object(int tree,int obj);
DEF_ENTRY *lookup_tree(int tree);
int markdef(int number);
int notranslate(char *text);
FILE *openfile(char *name,char *ext,char *mode);
void shrink_valid(char *dest,char *src);
void sort_table(int n);
char *strdup(const char *string);
void trim_spaces(char *string);
void usage(char *s);
int write_c_file(char *name,char *ext);
int write_data(FILE *fp,int words,USHORT *data);
int write_freestr(FILE *fp);
int write_general_prologue(FILE *fp,char *name,char *ext);
int write_h_file(char *name,char *ext);
int write_h_define(FILE *fp);
int write_h_extern(FILE *fp);
int write_bitblk(FILE *fp);
int write_iconblk(FILE *fp);
int write_include(FILE *fp,char *name);
int write_object(FILE *fp);
int write_obspec(FILE *fp,OBJECT *obj);
int write_rs_init(FILE *fp);
int write_shared(FILE *fp);
int write_tedinfo(FILE *fp);
int write_tree(FILE *fp);



int main(int argc,char *argv[])
{
int n;

    while((n=getopt(argc,argv,"dp:v")) != -1) {
        switch(n) {
        case 'd':
            debug++;
            break;
        case 'p':
            strcpy(prefix,optarg);
            strcat(prefix,"_");
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
        fputs(copyright,stderr);        /* announce us ... */

    if (argc-optind != 2)
        usage("incorrect number of arguments");

    if (debug)
        display_shared(num_shared);

    init_notrans(num_notrans);
    if (debug)
        display_notrans(num_notrans);

    rsc_root = argv[optind++];
    out_path = argv[optind];

    sprintf(inrsc,"%s.rsc",rsc_root);
    if ((rschdr=load_rsc(inrsc)) == NULL)
        error("can't load resource file",inrsc);
    if (debug)
        display_header(rschdr);

    num_defs = load_definition(rsc_root);
    sprintf(indef,"%s.%s",rsc_root,defext);

    if (num_defs < 0)
        error("can't load definition file",indef);

    sort_table(num_defs);
    if (debug)
        display_defs(num_defs);

    sprintf(cfile,"%s.c",out_path);
    sprintf(hfile,"%s.h",out_path);

    if (verbose) {
        printf("Input files: %s, %s\n",inrsc,indef);
        printf("Output files: %s, %s\n",cfile,hfile);
    }

    /*
     *  we write the C file first, because it figures out what free strings
     *  are conditional on DESK1, so that we can make the corresponding
     *  defines conditional too.
     */
    if (write_c_file(out_path,"c") < 0)
        error("can't create file",cfile);

    if (write_h_file(out_path,"h") < 0)
        error("can't create file",hfile);

    if (verbose)
        printf(PROGRAM_NAME " " VERSION " completed successfully\n");

    return 0;
}

/*****  input routines  *****/

/*
 *  load RSC file into memory
 *      returns NULL if there's a problem reading the file
 *      exits directly for other errors
 */
RSHDR *load_rsc(char *path)
{
long fsize;
unsigned short vrsn, rssize;
FILE *rscfp;
char *base;
RSHDR *rschdr;
char s[MAX_STRLEN];

    if (!(rscfp=fopen(path,"rb")))
        return NULL;

    if (fseek(rscfp,0L,SEEK_END) < 0)
        return NULL;
    if ((fsize=ftell(rscfp)) < 0)
        return NULL;
    if (fseek(rscfp,0L,SEEK_SET) < 0)
        return NULL;

    if (fsize < (long)sizeof(RSHDR))
        error("invalid RSC file (too small)",inrsc);

    if (!(base=malloc(fsize)))
        error("not enough memory to store RSC file",inrsc);

    if (fread(base,fsize,1,rscfp) != 1) {
        free(base);
        fclose(rscfp);
        return NULL;
    }

    fclose(rscfp);

    rschdr = (RSHDR *)base;
    vrsn = get_ushort(&rschdr->rsh_vrsn);
    if ((vrsn&~NEWRSC_FORMAT) > 1) {
        sprintf(s,"unsupported version (0x%04x) found in RSC file",vrsn);
        error(s,inrsc);
    }

    rssize = get_ushort(&rschdr->rsh_rssize);
    if (((vrsn&NEWRSC_FORMAT) == 0)         /* old RSC format              */
     && (rssize != fsize)) {                /*  but filesize doesn't match */
        error("incorrect length in RSC file",inrsc);
    }

    return rschdr;
}

/*
 *  load definition file into memory
 *  returns number of entries
 */
int load_definition(char *file)
{
FILE *fp = NULL;

    strcpy(defext,HRD);
    if ((fp=openfile(file,defext,"rb")))
        return load_hrd(fp);

    strcpy(defext,DEF);
    if ((fp=openfile(file,defext,"rb")))
        return load_def(fp);

    strcpy(defext,RSD);
    if ((fp=openfile(file,defext,"rb")))
        return load_def(fp);

    strcpy(defext,DFN);
    if ((fp=openfile(file,defext,"rb")))
        return load_dfn(fp);

    return -1;
}

/*
 *  load a .HRD definition file
 *  returns number of entries
 */
int load_hrd(FILE *fp)
{
int i, n, rc, c;
DEF_ENTRY *d;
struct {
    USHORT version;
    char dummy[6];
} hdr;
struct {
    char type;
    char dummy;
    USHORT tree;
    USHORT obj;
} entry;
char name[MAXLEN_HRD+1], *p;

    if (fread(&hdr,sizeof(hdr),1,fp) != 1)
        return -1;

    if (get_ushort(&hdr.version) != 1)
        return -1;

    /*
     *  read through the file to determine number of entries
     */
    for (n = 0; ; n++) {
        rc = fread(&entry,sizeof(entry),1,fp);
        if (rc < 0)
            return -1;
        if (entry.type == DEF_EOF)
            break;
        while((c=fgetc(fp)))
            if (c < 0)
                return -1;
    }

    def = calloc(n,sizeof(DEF_ENTRY));
    if (!def)
        return -1;

    if (fseek(fp,sizeof(hdr),SEEK_SET) < 0)
        return -1;

    for (i = 0, d = def; i < n; i++, d++) {
        if (fread(&entry,sizeof(entry),1,fp) != 1)
            return -1;
        d->type = entry.type;
        if ((d->type == DEF_ALERT)
         || (d->type == DEF_FREESTR)) {
            d->tree = get_ushort(&entry.obj);   /* swap for consistency in write_h_define() */
            d->obj = get_ushort(&entry.tree);
        } else {
            d->tree = get_ushort(&entry.tree);
            d->obj = get_ushort(&entry.obj);
        }
        for (p = name; p < name+MAXLEN_HRD; p++) {
            c = fgetc(fp);
            if (c < 0)
                return -1;
            *p = c;
            if (c == 0)
                break;
        }
        d->name = strdup(name);
        if (d->type == DEF_PREFIX) {
            if (!prefix[0]) {           /* use prefix from file if not already specified */
                strcpy(prefix,d->name);
                strcat(prefix,"_");
            }
        }
    }
    fclose(fp);

    return n;
}

/*
 *  load a .DEF/.RSD definition file
 */
int load_def(FILE *fp)
{
int i, n;
DEF_ENTRY *d;
struct {
    char dummy[4];
    char tree;
    char obj;
    char indicator;
    char type;
    char name[8];
} entry;
char temp[9];

    n = (fgetc(fp)<<8) | fgetc(fp);     /* number of entries (big-endian) */
    if (n < 0)
        return -1;

    def = calloc(n,sizeof(DEF_ENTRY));
    if (!def)
        return -1;

    temp[8] = '\0';                     /* ensure null-termination */

    if (fseek(fp,0,SEEK_SET) < 0)
        return -1;

    for (i = 0, d = def; i < n; i++, d++) {
        if (fread(&entry,sizeof(entry),1,fp) != 1)
            return -1;
        d->type = convert_type(entry.type);
        if ((d->type == DEF_DIALOG)
         || (d->type == DEF_MENU)) {
            d->tree = entry.obj;        /* swap for consistency in write_h_define() */
            d->obj = entry.tree;
        } else {
            d->tree = entry.tree;
            d->obj = entry.obj;
        }
        d->indicator = entry.indicator;
        memcpy(temp,entry.name,8);
        d->name = strdup(temp);
    }
    fclose(fp);

    return n;
}

/*
 *  load a .DFN definition file
 */
int load_dfn(FILE *fp)
{
int i, n;
DEF_ENTRY *d;
struct {
    char dummy[2];
    char obj;
    char tree;
    char type;
    char indicator;
    char name[8];
} entry;
char temp[9];

    n = fgetc(fp) | (fgetc(fp)<<8);     /* number of entries (little-endian) */
    if (n < 0)
        return -1;

    def = calloc(n,sizeof(DEF_ENTRY));
    if (!def)
        return -1;

    temp[8] = '\0';                     /* ensure null-termination */

    if (fseek(fp,0,SEEK_SET) < 0)
        return -1;

    for (i = 0, d = def; i < n; i++, d++) {
        if (fread(&entry,sizeof(entry),1,fp) != 1)
            return -1;
        d->type = convert_type(entry.type);
        if ((d->type == DEF_DIALOG)     /* fixup of bad(?) DFN */
         && (entry.tree == 0)
         && (entry.indicator == 1))
            d->type = DEF_FREESTR;
        if ((d->type == DEF_DIALOG)
         || (d->type == DEF_MENU)) {
            d->tree = entry.obj;        /* swap for consistency in write_h_define() */
            d->obj = entry.tree;
        } else {
            d->tree = entry.tree;
            d->obj = entry.obj;
        }
        d->indicator = entry.indicator;
        memcpy(temp,entry.name,8);
        d->name = strdup(temp);
    }
    fclose(fp);

    return n;
}

/*
 *  convert the type from a .DEF/.RSD/.DFN entry to the standard .HRD type
 */
short convert_type(int deftype)
{
short new;

    switch(deftype) {
    case 0:
        new = DEF_OBJECT;
        break;
    case 1:
        new = DEF_DIALOG;
        break;
    case 2:
        new = DEF_MENU;
        break;
    case 3:
        new = DEF_DIALOG;
        break;
    case 4:
        new = DEF_ALERT;
        break;
    case 5:
        new = DEF_FREESTR;
        break;
    case 6:
        new = DEF_FREEBIT;
        break;
    default:
        new = -1;
        break;
    }

    return new;
}

/*
 *  mark definition entry as conditional
 */
int markdef(int number)
{
int i;
DEF_ENTRY *d;

    for (i = 0, d = def; i < num_defs; i++, d++) {
        if ((d->type == DEF_ALERT) || (d->type == DEF_FREESTR)) {
            if (d->obj == number) {
                d->desk1 = 1;
                return 0;
            }
        }
    }

    return -1;
}

/*
 *  lookup object in definition table
 *
 *  if found, return pointer to entry
 *  otherwise, return NULL
 */
DEF_ENTRY *lookup_object(int tree,int obj)
{
int i;
DEF_ENTRY *d;

    for (i = 0, d = def; i < num_defs; i++, d++) {
        if (d->type != DEF_OBJECT)
            continue;
        if (d->tree > tree)
            break;
        if (d->tree < tree)
            continue;
        if (d->obj > obj)
            break;
        if (d->obj < obj)
            continue;
        return d;
    }

    return NULL;
}

/*
 *  lookup tree in definition table
 *
 *  if found, return pointer to entry
 *  otherwise, return NULL
 */
DEF_ENTRY *lookup_tree(int tree)
{
int i;
DEF_ENTRY *d;

    for (i = 0, d = def; i < num_defs; i++, d++) {
        if ((d->type != DEF_DIALOG) && (d->type != DEF_MENU))
            continue;
        if (d->tree > tree)
            break;
        if (d->tree < tree)
            continue;
        return d;
    }

    return NULL;
}

/*
 *  lookup free string in definition table
 *
 *  if found, return pointer to entry
 *  otherwise, return NULL
 */
DEF_ENTRY *lookup_freestr(int freestr)
{
int i;
DEF_ENTRY *d;

    for (i = 0, d = def; i < num_defs; i++, d++) {
        if ((d->type != DEF_ALERT) && (d->type != DEF_FREESTR))
            continue;
        if (d->obj > freestr)
            break;
        if (d->obj < freestr)
            continue;
        return d;
    }

    return NULL;
}


/*****  output routines *****/

int write_c_file(char *name,char *ext)
{
FILE *fp;
char *basename;

    fp = openfile(name,ext,"w");
    if (!fp)
        return -1;

    basename = strrchr(name,DIRSEP);
    if (!basename)
        basename = name;
    else basename++;
    if (write_general_prologue(fp,basename,ext))
        return -1;
    if (write_include(fp,basename))
        return -1;
    if (write_shared(fp))
        return -1;
    if (write_tedinfo(fp))
        return -1;
    if (write_iconblk(fp))
        return -1;
    if (write_bitblk(fp))
        return -1;
    if (write_object(fp))
        return -1;
    if (write_tree(fp))
        return -1;
    if (write_freestr(fp))
        return -1;
    if (write_rs_init(fp))
        return -1;

    fclose(fp);

    return 0;
}

int write_h_file(char *name,char *ext)
{
FILE *fp;
char *basename;

    fp = openfile(name,ext,"w");
    if (!fp)
        return -1;

    basename = strrchr(name,DIRSEP);
    if (!basename)
        basename = name;
    else basename++;
    if (write_general_prologue(fp,basename,"h"))
        return -1;
    if (write_h_define(fp))
        return -1;
    if (write_h_extern(fp))
        return -1;

    fclose(fp);

    return 0;
}

/*
 *  this creates the general stuff at the start of both output files
 */
int write_general_prologue(FILE *fp,char *name,char *ext)
{
char *basersc;

    /* strip path from resource/definition */
    basersc = strrchr(rsc_root,DIRSEP);
    if (!basersc)
        basersc = rsc_root;
    else basersc++;

    fprintf(fp,"/*\n");
    fprintf(fp," * %s.%s\n",name,ext);
    fprintf(fp," *\n");
    fprintf(fp," * Generated from %s.rsc and %s.%s by %s %s\n",basersc,basersc,defext,PROGRAM_NAME,VERSION);
    fprintf(fp," *\n");
    fprintf(fp," * Copyright by the EmuTOS development team\n");
    fprintf(fp," *\n");
    fprintf(fp," * This software is licenced under the GNU Public License.\n");
    fprintf(fp," * Please see LICENSE.TXT for further information.\n");
    fprintf(fp," */\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the #defines of the .h file
 */
int write_h_define(FILE *fp)
{
int i, n;
DEF_ENTRY *d;
short old_tree = -1;

    for (i = 0, d = def; i < num_defs; i++, d++) {
        if ((d->tree != old_tree)
         || (d->seq == 1))
            fprintf(fp,"\n");
        if (d->desk1)
            fprintf(fp,"#ifdef DESK1\n");
        if ((d->type == DEF_DIALOG)
         || (d->type == DEF_MENU))
            n = d->tree;
        else n = d->obj;
        fprintf(fp,"#define %-16s%d\n",d->name,n);
        if (d->desk1)
            fprintf(fp,"#endif\n");
        old_tree = d->tree;
    }
    fprintf(fp,"\n\n");

    fprintf(fp,"#define %-16s%d\n","RS_NOBS",get_ushort(&rschdr->rsh_nobs));
    fprintf(fp,"#define %-16s%d\n","RS_NTREE",get_ushort(&rschdr->rsh_ntree));
    fprintf(fp,"#define %-16s%d\n","RS_NTED",get_ushort(&rschdr->rsh_nted));
    fprintf(fp,"#define %-16s%d\n","RS_NIB",get_ushort(&rschdr->rsh_nib));
    fprintf(fp,"#define %-16s%d\n\n\n","RS_NBB",get_ushort(&rschdr->rsh_nbb));


    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the externs of the .h file
 */
int write_h_extern(FILE *fp)
{
    fprintf(fp,"extern const BITBLK %srs_bitblk[];\n",prefix);
    fprintf(fp,"extern const char * const %srs_fstr[];\n",prefix);
    fprintf(fp,"extern const ICONBLK %srs_iconblk[];\n",prefix);
    fprintf(fp,"extern OBJECT %srs_obj[RS_NOBS];\n",prefix);
    fprintf(fp,"extern TEDINFO %srs_tedinfo[RS_NTED];\n",prefix);
    fprintf(fp,"extern OBJECT * const %srs_trees[];\n\n",prefix);

    fprintf(fp,"extern void %srs_init(void);\n\n",prefix);

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the includes at the beginning of the .c file
 */
int write_include(FILE *fp,char *name)
{
    fprintf(fp,"#include \"config.h\"\n");
    fprintf(fp,"#include \"string.h\"\n");
    fprintf(fp,"#include \"portab.h\"\n");
    fprintf(fp,"#include \"obdefs.h\"\n");
    fprintf(fp,"#include \"%s.h\"\n",name);
    fprintf(fp,"#include \"nls.h\"\n\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the shared strings for the .c file
 */
int write_shared(FILE *fp)
{
int i;
SHARED_ENTRY *e;
char temp[MAX_STRLEN];

    for (i = 0, e = shared; i < num_shared; i++, e++) {
        fixshared(temp,e->string);  /* replace any non-alphanumeric char with underscore */
        fprintf(fp,"static const char rs_str_%s[] = ",temp);
        if (copycheck(temp,e->string,MAX_STRLEN-1) == 0)
            fprintf(fp,"\"%s\";\n",temp);
        else fprintf(fp,"%s\"%s\");\n",NLS,temp);
    }
    fprintf(fp,"\n\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the TEDINFO stuff for the .c file
 */
int write_tedinfo(FILE *fp)
{
int i, nted;
TEDINFO *ted;
char temp[MAX_STRLEN];
char *base = (char *)rschdr;

    fprintf(fp,"TEDINFO %srs_tedinfo[RS_NTED];\n\n",prefix);

    fprintf(fp,"static const TEDINFO %srs_tedinfo_rom[] = {\n",prefix);
    nted = get_ushort(&rschdr->rsh_nted);
    for (i = 0, ted = (TEDINFO *)(base+get_ushort(&rschdr->rsh_tedinfo)); i < nted; i++, ted++) {
        fprintf(fp,"    {0L,\n");
        if (copycheck(temp,base+get_offset(&ted->te_ptmplt),get_short(&ted->te_tmplen)) == 0)
            fprintf(fp,"     (LONG) \"%s\",\n",temp);
        else fprintf(fp,"     (LONG) %s\"%s\"),\n",NLS,temp);
        shrink_valid(temp,(char *)(base+get_offset(&ted->te_pvalid)));
        fprintf(fp,"     (LONG) \"%s\",\n",temp);
        sprintf(temp,"     %s, %d, %s, %d, %d, %d, %d, %d}%s",
                decode_font(get_short(&ted->te_font)),get_short(&ted->te_fontid),
                decode_just(get_short(&ted->te_just)),get_short(&ted->te_color),
                get_short(&ted->te_fontsize),get_short(&ted->te_thickness),
                get_short(&ted->te_txtlen),get_short(&ted->te_tmplen),
                (i==nted-1)?"":",");
        fprintf(fp,"%-48.48s/* %d */\n",temp,i);
        if (i != nted-1)
            fprintf(fp,"\n");
    }

    fprintf(fp,"};\n\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the ICONBLK stuff for the .c file
 */
int write_iconblk(FILE *fp)
{
int i, j, n, nib;
short iconchar;
char temp[MAX_STRLEN];
int *map;
ICONBLK *iconblk;
char *base = (char *)rschdr;

    nib = get_ushort(&rschdr->rsh_nib);
    if (nib == 0)
        return 0;

    iconblk = (ICONBLK *)(base + get_ushort(&rschdr->rsh_iconblk));

    /*
     * first we figure out the duplicate icon stuff: if map[j]
     * contains i (i not equal to -1), then the jth icon is the
     * same as the ith, and the ith icon is the "master" icon
     * (and map[i] will contain -1)
     */
    map = malloc(nib*sizeof(int));      /* allocate mapping array */
    for (i = 0; i < nib; i++)
        map[i] = -1;

    for (i = 0; i < nib; i++) {         /* populate mapping array */
        if (map[i] >= 0)
            continue;                   /* already mapped */
        for (j = i+1; j < nib; j++) {
            if (map[j] >= 0)
                continue;               /* already mapped */
            if (compare_icons(iconblk+i,iconblk+j))
                continue;                       /* different icons */
            map[j] = i;                         /* the same, so map */
        }
    }

    /*
     * then we create the actual icon mask/data arrays
     */
    for (i = 0; i < nib; i++, iconblk++) {
        if (map[i] >= 0)        /* don't create icon data for a "mapped" icon */
            continue;
        n = get_short(&iconblk->ib_hicon) * get_short(&iconblk->ib_wicon) / 16;
        fprintf(fp,"static const WORD rs_iconmask%d[] = {\n",i);        /* output mask */
        write_data(fp,n,(USHORT *)(base+get_offset(&iconblk->ib_pmask)));
        fprintf(fp,"};\n");
        fprintf(fp,"static const WORD rs_icondata%d[] = {\n",i);        /* output data */
        write_data(fp,n,(USHORT *)(base+get_offset(&iconblk->ib_pdata)));
        fprintf(fp,"};\n\n");
    }
    fprintf(fp,"\n");

    /*
     * finally we create the array of ICONBLKs with pointers to mask/data
     */
    fprintf(fp,"const ICONBLK %srs_iconblk[] = {\n",prefix);
    iconblk = (ICONBLK *)(base + get_ushort(&rschdr->rsh_iconblk));
    for (i = 0; i < nib; i++, iconblk++) {
        iconchar = get_short(&iconblk->ib_char);
        copyfix(temp,base+get_offset(&iconblk->ib_ptext),MAX_STRLEN-1);
        fprintf(fp,"    { (LONG) rs_iconmask%d, (LONG) rs_icondata%d, \"%s\", %s,\n",
                (map[i]==-1)?i:map[i],(map[i]==-1)?i:map[i],
                temp,decode_ib_char(iconchar));
        fprintf(fp,"      %d, %d, %d, %d, %d, %d, %d, %d, %d, %d },\n",
                get_short(&iconblk->ib_xchar),get_short(&iconblk->ib_ychar),
                get_short(&iconblk->ib_xicon),get_short(&iconblk->ib_yicon),
                get_short(&iconblk->ib_wicon),get_short(&iconblk->ib_hicon),
                get_short(&iconblk->ib_xtext),get_short(&iconblk->ib_ytext),
                get_short(&iconblk->ib_wtext),get_short(&iconblk->ib_htext));
    }
    fprintf(fp,"};\n\n\n");

    free(map);

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the BITBLK stuff for the .c file
 */
int write_bitblk(FILE *fp)
{
int i, j, n, nbb;
int *map;
BITBLK *bitblk;
char *base = (char *)rschdr;

    nbb = get_ushort(&rschdr->rsh_nbb);
    if (nbb == 0)
        return 0;

    bitblk = (BITBLK *)(base + get_ushort(&rschdr->rsh_bitblk));

    /*
     * first we figure out the duplicate image stuff: if map[j]
     * contains i (i not equal to -1), then the jth image is the
     * same as the ith, and the ith image is the "master" image
     * (and map[i] will contain -1)
     */
    map = malloc(nbb*sizeof(int));      /* allocate mapping array */
    for (i = 0; i < nbb; i++)
        map[i] = -1;

    for (i = 0; i < nbb; i++) {         /* populate mapping array */
        if (map[i] >= 0)
            continue;                   /* already mapped */
        for (j = i+1; j < nbb; j++) {
            if (map[j] >= 0)
                continue;               /* already mapped */
            if (compare_images(bitblk+i,bitblk+j))
                continue;                       /* different images */
            map[j] = i;                         /* the same, so map */
        }
    }

    /*
     * then we create the actual images
     */
    for (i = 0; i < nbb; i++, bitblk++) {
        if (map[i] >= 0)        /* don't create image data for a "mapped" image */
            continue;
        n = get_short(&bitblk->bi_hl) * get_short(&bitblk->bi_wb) / 2;
        fprintf(fp,"static const WORD rs_bitblk%d[] = {\n",i);
        write_data(fp,n,(USHORT *)(base+get_offset(&bitblk->bi_pdata)));
        fprintf(fp,"};\n\n");
    }
    fprintf(fp,"\n");

    /*
     * finally we create the array of BITBLKs with pointers to images
     */
    fprintf(fp,"const BITBLK %srs_bitblk[] = {\n",prefix);
    bitblk = (BITBLK *)(base + get_ushort(&rschdr->rsh_bitblk));
    for (i = 0; i < nbb; i++, bitblk++)
        fprintf(fp,"    { (LONG) rs_bitblk%d, %d, %d, %d, %d, %d },\n",
                (map[i]==-1)?i:map[i],get_short(&bitblk->bi_wb),
                get_short(&bitblk->bi_hl),get_short(&bitblk->bi_x),
                get_short(&bitblk->bi_y),get_short(&bitblk->bi_color));
    fprintf(fp,"};\n\n\n");

    free(map);

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the OBJECT stuff for the .c file
 */
int write_object(FILE *fp)
{
int i, j, nobs, tree, ntree;
unsigned short type, ext_type;
OBJECT *obj;
OFFSET *trindex;
DEF_ENTRY *d;
char temp[MAX_STRLEN];
char *p;
char *base = (char *)rschdr;

    fprintf(fp,"OBJECT %srs_obj[RS_NOBS];\n\n",prefix);

    fprintf(fp,"static const OBJECT %srs_obj_rom[] = {\n",prefix);

    /* trindex points to an array of long _offsets_ */
    trindex = (OFFSET *)(base+get_ushort(&rschdr->rsh_trindex));
    obj = (OBJECT *)(base + get_ushort(&rschdr->rsh_object));
    nobs = get_ushort(&rschdr->rsh_nobs);
    ntree = get_ushort(&rschdr->rsh_ntree);
    for (i = 0, j = 0, tree = 0; i < nobs; i++, j++, obj++) {
        if (tree < ntree) {
            if ((OBJECT *)(base+get_offset(&trindex[tree])) == obj) {
                fprintf(fp,"#define TR%d %d\n",tree,i);
                fprintf(fp,"/* TREE %d */\n\n",tree);
                tree++;
                j = 0;                              /* relative object number */
            }
        }
        type = get_ushort(&obj->ob_type);
        p = decode_type(type&0xff);
        ext_type = type & 0xff00;
        if (ext_type)
            sprintf(temp,"   { %d, %d, %d, %s|0x%04x,",get_short(&obj->ob_next),
                    get_short(&obj->ob_head),get_short(&obj->ob_tail),p,ext_type);
        else sprintf(temp,"   { %d, %d, %d, %s,",get_short(&obj->ob_next),
                    get_short(&obj->ob_head),get_short(&obj->ob_tail),p);

        fprintf(fp,"%-44s/*** %d ***/",temp,j);
        if ((d=lookup_object(tree-1,j)))
            fprintf(fp,"  /* %s */",d->name);
        fprintf(fp,"\n");
        fprintf(fp,"     %s,\n",decode_flags(get_ushort(&obj->ob_flags)));
        fprintf(fp,"     %s,\n",decode_state(get_ushort(&obj->ob_state)));
        write_obspec(fp,obj);
        fprintf(fp,"     %d, %d, %d, %d}",
                get_short(&obj->ob_x),get_short(&obj->ob_y),
                get_short(&obj->ob_width),get_short(&obj->ob_height));
        if (i != nobs-1)
            fprintf(fp,",\n\n");
        else fprintf(fp,"\n");
    }
    fprintf(fp,"};\n\n\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the TREE stuff for the .c file
 */
int write_tree(FILE *fp)
{
int i, ntree;
DEF_ENTRY *d;
char temp[MAX_STRLEN];

    fprintf(fp,"OBJECT * const %srs_trees[] = {\n",prefix);

    ntree = get_ushort(&rschdr->rsh_ntree);
    for (i = 0; i < ntree; i++) {
        sprintf(temp,"    &%srs_obj[TR%d]%s",prefix,i,(i==ntree-1)?"":",");
        d = lookup_tree(i);
        fprintf(fp,"%-44s/* %s */\n",temp,d?d->name:"???");
    }
    fprintf(fp,"};\n\n\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the free string stuff for the .c file
 */
int write_freestr(FILE *fp)
{
int i, j, n, nstring;
int desk1, xlate, numstr, len;
int length[MAX_SUBSTR];
char *s;
OFFSET *strptr;
DEF_ENTRY *d;
char temp[MAX_STRLEN];
char *base = (char *)rschdr;

    fprintf(fp,"const char * const %srs_fstr[] = {\n",prefix);

    nstring = get_ushort(&rschdr->rsh_nstring);
    strptr = (OFFSET *)(base + get_ushort(&rschdr->rsh_frstr));
    for (i = 0; i < nstring; i++, strptr++) {
        s = (char *) (base + get_offset(strptr));

        /* save space in the generated code if DESK1 is not defined */
        desk1 = find(s,"trash");    /* set flag if magic word is present */
        if (desk1) {
            fprintf(fp,"#ifdef DESK1\n");       /* string is only generated conditionally */
            markdef(i);                         /* mark DEF_ENTRY so #define is conditional too */
        }

        xlate = copycheck(temp,s,MAX_STRLEN-1); /* copy string, fixing up special characters */
        numstr = getlen(length,temp);           /* get lengths of substrings */
        for (j = 0, s = temp; j < numstr; j++, s += len) {
            n = fprintf(fp,"    ");
            if (xlate)
                n += fprintf(fp,"%s",(j==0)?NLS:"   ");
            len = length[j];
            n += fprintf(fp,"\"%*.*s\"",len,len,s);
            if (j == numstr-1)
                n += fprintf(fp,"%s,",xlate?")":"");
            if (j == 0) {
                if ((d=lookup_freestr(i))) {
                    while(n++ < 56)
                        fprintf(fp," ");
                    fprintf(fp,"/* %s */",d->name);
                }
            }
            fprintf(fp,"\n");
        }

        if (desk1)
            fprintf(fp,"#endif\n");
    }
    fprintf(fp,"};\n\n\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  this creates the minimal initialisation code in the .c file
 */
int write_rs_init(FILE *fp)
{
    fprintf(fp,"void %srs_init(void)\n",prefix);
    fprintf(fp,"{\n");
    fprintf(fp,"    /* Copy data from ROM to RAM: */\n");
    fprintf(fp,"    memcpy(%srs_obj, %srs_obj_rom, RS_NOBS * sizeof(OBJECT));\n",prefix,prefix);
    fprintf(fp,"    memcpy(%srs_tedinfo, %srs_tedinfo_rom,\n",prefix,prefix);
    fprintf(fp,"           RS_NTED * sizeof(TEDINFO));\n");
    fprintf(fp,"}\n\n");

    return ferror(fp) ? -1 : 0;
}

/*
 *  copies string, returns non-zero if it may need translating
 */
int copycheck(char *dest,char *src,int len)
{
char *s;
int i;

    copyfix(dest,src,len);

    /* if _not_ no-translate, then see if we need to */
    if (!notranslate(src))
        for (i = 0, s = src; (i < len) && *s; i++, s++)
            if (!isdigit((unsigned char)*s) && !ispunct((unsigned char)*s) && !isspace((unsigned char)*s))
                return 1;

    return 0;
}

/*
 *  copies string, fixing unprintable characters
 */
void copyfix(char *dest,char *src,int len)
{
char *d, *s;
int i;

    for (i = 0, d = dest, s = src; (i < len) && *s; i++, s++) {
        if (*s == '"')          /* we need to escape double quotes */
            *d++ = '\\';
        if (!isprint((unsigned char)*s))        /* or convert to octal if not printable */
            d += sprintf(d,"\\%03o",*s);
        else *d++ = *s;
    }
    *d = '\0';
}

/*
 *  shrinks TEDINFO validate string to minimum
 */
void shrink_valid(char *dest,char *src)
{
char *d, *s, *new;

    for (new = s = src; *s; s++)
        if (*s != *(s+1))       /* if the next char differs, */
            if (*(s+1))         /*  & isn't end of string,   */
                new = s+1;      /*   remember where it is    */

    for (s = src, d = dest; s <= new; )
        *d++ = *s++;
    *d = '\0';
}

/*
 *  initialises length fields in notrans table
 */
void init_notrans(int n)
{
int i;
NOTRANS_ENTRY *e;

    for (i = 0, e = notrans; i < n; i++, e++)
        e->length = strlen(e->string);
}

/*
 *  looks for match between supplied string and notranslate entries
 *  returns 1 iff match found
 */
int notranslate(char *text)
{
int i;
NOTRANS_ENTRY *e;

    for (i = 0, e = notrans; i < num_notrans; i++, e++)
        if (strncmp(text,e->string,e->length) == 0)
            return 1;

    return 0;
}

/*
 *  looks for match between supplied string and shared string entries
 *  returns 1 iff match found
 */
int isshared(char *text)
{
int i;
SHARED_ENTRY *e;

    for (i = 0, e = shared; i < num_shared; i++, e++)
        if (strcmp(text,e->string) == 0)
            return 1;

    return 0;
}

/*
 *  look for substring in string
 *  returns 1 if substring is found, 0 otherwise
 */
int find(char *string,char *substring)
{
char *s = string;
char *t = substring;
int len, stop;

    len = strlen(substring);
    stop = strlen(string) - len;

    while (stop-- >= 0)
        if (strncmp(s++,t,len) == 0)
            return 1;

    return 0;
}

/*
 *  scan free string
 *  returns number of substrings & length of each substring
 *
 *  in this case, a substring is defined in form_alert() fashion,
 *  i.e. it ends with a vertical bar |, or a null character.
 *  because this is designed for alert-style strings, a maximum
 *  of 5 substrings is allowed.
 *  note: we only look for vertical bars between the first
 *  and second occurrences of ']'
 */
int getlen(int *length,char *s)
{
int i;
char *start;
char right_brackets = 0;

    if (!*s) {              /* handle empty string */
        *length = 0;
        return 1;
    }

    for (i = 0, start = s; *s && (i < MAX_SUBSTR); i++, start = s) {
        while(*s) {
            if (*s == ']')
                right_brackets++;
            if (*s++ == '|')
                if (right_brackets == 1)
                    break;
        }
        *length++ = s - start;
    }

    return i;
}

/*
 *  returns pointer to string containing decoded data from ib_char
 */
char *decode_ib_char(short iconchar)
{
static char temp[20];
char *t;
unsigned char c;

    t = temp + sprintf(temp,"0x%04hX|'",iconchar&0xff00);
    c = iconchar & 0xff;
    if (isprint(c))
        *t++ = c;
    else t += sprintf(t,"\\%03o",c);
    strcpy(t,"'");

    return temp;        
}

/*
 *  returns pointer to string defining flags
 */
char *decode_flags(short flags)
{
static char temp[MAX_STRLEN];
FLAGS *f;

    if (flags == 0)
        return "NONE";

    temp[0] = '\0';
    for (f = flaglist; f->mask; f++) {
        if ((flags&f->mask) == f->mask) {
            if (temp[0])
                strcat(temp," | ");
            strcat(temp,f->desc);
        }
    }

    return temp;        
}

/*
 *  returns pointer to string defining font
 */
char *decode_font(short font)
{
static char temp[10], *p;

    switch(font) {
    case IBM:
        p = "IBM";
        break;
    case SMALL:
        p = "SMALL";
        break;
    default:
        sprintf(temp,"%d",font);
        p = temp;
        break;
    }

    return p;       
}

/*
 *  returns pointer to string defining justification
 */
char *decode_just(short just)
{
static char temp[10], *p;

    switch(just) {
    case TE_LEFT:
        p = "TE_LEFT";
        break;
    case TE_RIGHT:
        p = "TE_RIGHT";
        break;
    case TE_CNTR:
        p = "TE_CNTR";
        break;
    default:
        sprintf(temp,"%d",just);
        p = temp;
        break;
    }

    return p;       
}

/*
 *  returns pointer to string defining state
 */
char *decode_state(short state)
{
static char temp[MAX_STRLEN];
STATE *s;

    if (state == 0)
        return "NORMAL";

    temp[0] = '\0';
    for (s = statelist; s->mask; s++) {
        if ((state&s->mask) == s->mask) {
            if (temp[0])
                strcat(temp," | ");
            strcat(temp,s->desc);
        }
    }

    return temp;
}


/*
 *  returns pointer to string defining type
 */
char *decode_type(short type)
{
TYPE *t;
static char temp[10];

    for (t = typelist; t->type; t++)
        if (t->type == type)
            return t->desc;

    sprintf(temp,"%d",type);
    return temp;
}

/*
 *  write array of USHORT data items, 4 per line
 */
int write_data(FILE *fp,int words,USHORT *data)
{
int i;

    for (i = 0; i < words; i++) {
        if ((i%4) == 0)
            fprintf(fp,"   ");
        fprintf(fp," 0x%04hX,",get_ushort(data++));
        if (((i%4) == 3) || (i == words-1))
            fprintf(fp,"\n");
    }

    return ferror(fp) ? -1 : 0;
}

/*
 *  writes formatted obspec to output
 */
int write_obspec(FILE *fp,OBJECT *obj)
{
int xlate, type;
char *p;
char temp[MAX_STRLEN];
char *base = (char *)rschdr;

    fprintf(fp,"     (LONG) ");

    type = get_ushort(&obj->ob_type) & 0xff;
    switch(type) {
    case G_BOX:
    case G_IBOX:
        fprintf(fp,"%ldL,\n",get_offset(&obj->ob_spec));
        break;
    case G_BOXCHAR:
        fprintf(fp,"0x%08lxL,\n",get_offset(&obj->ob_spec));
        break;
    case G_STRING:
    case G_BUTTON:
    case G_TITLE:
        p = base + get_offset(&obj->ob_spec);
        if (isshared(p)) {
            fixshared(temp,p);
            fprintf(fp,"rs_str_%s,\n",temp);
            break;
        }
        xlate = copycheck(temp,p,MAX_STRLEN-1);
        if (type == G_STRING)
            trim_spaces(temp);
        if (all_dashes(temp) && !notranslate(temp))     /* handle menu separators */
            xlate = 1;
        if (xlate == 0)
            fprintf(fp,"\"%s\",\n",temp);
        else fprintf(fp,"%s\"%s\"),\n",NLS,temp);
        break;
    case G_TEXT:
    case G_BOXTEXT:
    case G_FTEXT:
    case G_FBOXTEXT:
        fprintf(fp,"&%srs_tedinfo[%ld],\n",prefix,
            (get_offset(&obj->ob_spec)-get_ushort(&rschdr->rsh_tedinfo))/sizeof(TEDINFO));
        break;
    case G_IMAGE:
        fprintf(fp,"&%srs_bitblk[%ld],\n",prefix,
            (get_offset(&obj->ob_spec)-get_ushort(&rschdr->rsh_bitblk))/sizeof(BITBLK));
        break;
    case G_PROGDEF:
        fprintf(fp,"%ldL, /* generate number for unsupported PROGDEF ob_type */\n",
                get_offset(&obj->ob_spec));
        break;
    case G_ICON:
        fprintf(fp,"&%srs_iconblk[%ld],\n",prefix,
            (get_offset(&obj->ob_spec)-get_ushort(&rschdr->rsh_iconblk))/sizeof(ICONBLK));
        break;
    case G_CICON:
        fprintf(fp,"%ldL, /* generate number for unsupported CICONBLK ob_type */\n",
                get_offset(&obj->ob_spec));
        break;
    default:
        fprintf(fp,"%ldL, /* generate number for unknown ob_type 0x%02x */\n",
                get_offset(&obj->ob_spec),type);
        break;
    }

    return ferror(fp) ? -1 : 0;
}

/*
 *  copies string, changing non-alphanumeric characters to underscore
 */
void fixshared(char *dest,char *src)
{
char *d, *s;

    for (d = dest, s = src; *s; d++, s++)
        *d = isalnum((unsigned char)*s) ? *s : '_';
    *d = '\0';
}

/*
 *  check if line is all dashes, return 1 iff so
 *
 *  normally, such a line would not be enclosed with N_(), because
 *  it's just punctuation.  however, this kind of line is used in
 *  the menu to separate different sections, and since menu items
 *  may change in length, we must detect such strings so that we
 *  can make them visible to the nls support, and therefore capable
 *  of being changed in length.
 */
int all_dashes(char *string)
{
char *p;

    if (!*string)       /* empty strings are not "all dashes"! */
        return 0;

    for (p = string; *p; p++)
        if (*p != '-')
            return 0;

    return 1;
}

/*
 *  trims trailing spaces from string
 */
void trim_spaces(char *string)
{
char *p;

    for (p = string+strlen(string)-1; p >= string; p--)
        if (*p != ' ')
            break;
    *(p+1) = '\0';
}

/*
 *  compare images, return 0 iff identical size & image data
 */
int compare_images(BITBLK *b1,BITBLK *b2)
{
int i, size1, size2;
char *p1, *p2;
char *base = (char *)rschdr;

    /* calculate sizes in bytes */
    size1 = get_short(&b1->bi_hl) * get_short(&b1->bi_wb);
    size2 = get_short(&b2->bi_hl) * get_short(&b2->bi_wb);

    if (size1 != size2)
        return 1;

    p1 = base + get_offset(&b1->bi_pdata);
    p2 = base + get_offset(&b2->bi_pdata);

    for (i = 0; i < size1; i++)
        if (*p1++ != *p2++)
            return 1;

    return 0;
}

/*
 *  compare icons, return 0 iff identical size & mask/data
 */
int compare_icons(ICONBLK *b1,ICONBLK *b2)
{
int i, size1, size2;
char *p1, *p2;
char *base = (char *)rschdr;

    /* calculate sizes in bytes */
    size1 = get_short(&b1->ib_hicon) * get_short(&b1->ib_wicon) / 8;
    size2 = get_short(&b2->ib_hicon) * get_short(&b2->ib_wicon) / 8;
    if (size1 != size2)
        return 1;

    p1 = base + get_offset(&b1->ib_pmask);
    p2 = base + get_offset(&b2->ib_pmask);
    for (i = 0; i < size1; i++)
        if (*p1++ != *p2++)
            return 1;

    p1 = base + get_offset(&b1->ib_pdata);
    p2 = base + get_offset(&b2->ib_pdata);
    for (i = 0; i < size1; i++)
        if (*p1++ != *p2++)
            return 1;

    return 0;
}


/*****  dump routines for debugging *****/

/*
 *  display info from RSC header
 */
void display_header(RSHDR *rschdr)
{
    printf("RSC header (version %d), loaded from %s\n",get_ushort(&rschdr->rsh_vrsn),inrsc);
    printf("  Object offset  %5d\n",get_ushort(&rschdr->rsh_object));   /* offset to object[] */
    printf("  TEDINFO offset %5d\n",get_ushort(&rschdr->rsh_tedinfo));  /* offset to tedinfo[] */
    printf("  ICONBLK offset %5d\n",get_ushort(&rschdr->rsh_iconblk));  /* offset to iconblk[] */
    printf("  BITBLK offset  %5d\n",get_ushort(&rschdr->rsh_bitblk));   /* offset to bitblk[] */
    printf("  FRSTR offset   %5d\n",get_ushort(&rschdr->rsh_frstr));    /* offset to free strings index */
    printf("  STRING offset  %5d\n",get_ushort(&rschdr->rsh_string));   /* offset to string data */
    printf("  IMDATA offset  %5d\n",get_ushort(&rschdr->rsh_imdata));   /* offset to image data */
    printf("  FRIMG offset   %5d\n",get_ushort(&rschdr->rsh_frimg));    /* offset to free image index */
    printf("  TRINDEX offset %5d\n",get_ushort(&rschdr->rsh_trindex));  /* offset to object tree index */
    printf("  %5d objects\n",get_ushort(&rschdr->rsh_nobs));            /* number of objects */
    printf("  %5d trees\n",get_ushort(&rschdr->rsh_ntree));             /* number of trees */
    printf("  %5d TEDINFOs\n",get_ushort(&rschdr->rsh_nted));           /* number of tedinfos */
    printf("  %5d ICONBLKs\n",get_ushort(&rschdr->rsh_nib));            /* number of icon blocks */
    printf("  %5d BITBLKs\n",get_ushort(&rschdr->rsh_nbb));             /* number of blt blocks */
    printf("  %5d free strings\n",get_ushort(&rschdr->rsh_nstring));    /* number of free strings */
    printf("  %5d free images\n",get_ushort(&rschdr->rsh_nimages));     /* number of free images */
    printf("  Resource size %5d bytes\n\n",get_ushort(&rschdr->rsh_rssize));    /* total bytes in resource */
}

int cmp(const void *a,const void *b)
{
const DEF_ENTRY *d1 = a;
const DEF_ENTRY *d2 = b;

    if (d1->seq != d2->seq)
        return d1->seq - d2->seq;

    if (d1->tree != d2->tree)
        return d1->tree - d2->tree;

    return d1->obj - d2->obj;
}

void sort_table(int n)
{
int i;
DEF_ENTRY *d;

    /* insert sequencing number */
    for (i = 0, d = def; i < n; i++, d++)
        if ((d->type == DEF_DIALOG)
         || (d->type == DEF_MENU)
         || (d->type == DEF_OBJECT))
            d->seq = 0;
        else d->seq = 1;

    qsort(def,n,sizeof(DEF_ENTRY),cmp);
}

void display_defs(int n)
{
int i;
DEF_ENTRY *d;

    printf("Definitions table, loaded from %s.%s\n",rsc_root,defext);
    printf("  Seq  Type  Tree  Object  Ind  Name\n");
    for (i = 0, d = def; i < n; i++, d++)
        printf("   %2d   %2d    %2d     %2d    %2d  %s\n",d->seq,d->type,d->tree,d->obj,d->indicator,d->name);
    printf("\n");
}

void display_notrans(int n)
{
int i;
NOTRANS_ENTRY *e;

    if (n == 0) {
        printf("0 no-translate entries\n");
        return;
    }

    printf("%d no-translate entr%s:\n",n,(n==1)?"y":"ies");
    printf("  Length  Prefix string\n");
    for (i = 0, e = notrans; i < n; i++, e++)
        printf("    %2d    '%s'\n",e->length,e->string);
    printf("\n");
}

void display_shared(int n)
{
int i;
SHARED_ENTRY *e;

    if (n == 0) {
        printf("0 shared string entries\n");
        return;
    }

    printf("%d shared string entr%s:\n",n,(n==1)?"y":"ies");
    for (i = 0, e = shared; i < n; i++, e++)
        printf("  '%s'\n",e->string);
    printf("\n");
}


/*****  miscellaneous support routines  *****/

/*
 *  convert big-endian short to short
 */
short get_short(SHORT *p)
{
    return (p->hi<<8) | p->lo;
}

/*
 *  convert big-endian short to short
 */
unsigned short get_ushort(USHORT *p)
{
    return (p->hi<<8) | p->lo;
}

/*
 *  convert big-endian offset to unsigned long
 */
unsigned long get_offset(OFFSET *p)
{
    return (p->b1<<24) | (p->b2<<16) | (p->b3<<8) | p->b4;
}

/*
 *  open a file, specified as name & extension separately
 */
FILE *openfile(char *name,char *ext,char *mode)
{
char s[MAX_STRLEN];

    sprintf(s,"%s.%s",name,ext);
    return fopen(s,mode);
}

/*
 *  allocate memory & copy string to it
 */
char *strdup(const char *string)
{
char *p;

    p = malloc(strlen(string)+1);
    if (p)
        strcpy(p,string);

    return p;
}

void error(char *s,char *t)
{
    fprintf(stderr,"  error: %s",s);
    if (t)
        fprintf(stderr," %s",t);
    fputc('\n',stderr);
    exit(1);
}

void usage(char *s)
{
    if (*s)
        fprintf(stderr,"%s %s: %s\n",PROGRAM_NAME,VERSION,s);
    fprintf(stderr,"usage: %s [-d] [-p<prefix>] [-v] <rsc_file> <c_file>\n",PROGRAM_NAME);

    exit(2);
}

