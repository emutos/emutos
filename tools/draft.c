/*
 *  draft: Delete Resource Alerts, Freestrings, and Trees
 *
 *  Copyright 2017-2019 Roger Burrows
 *
 *  This program is licensed under the GNU General Public License.
 *  Please see LICENSE.TXT for details.
 *
 *
 *  This program is designed to delete tree(s) and/or free string(s)
 *  and/or alert(s) from a resource file.  As of v1.1, it also deletes
 *  menu items.
 *
 *  Syntax: draft [-d] [-v] <RSCin> <RSCout>
 *
 *      where:
 *          -d          requests debugging output
 *          -v          requests verbose output
 *          <RSCin>     identifies the input RSC/definition files
 *          <RSCout>    identifies the output RSC/definition files
 *  Note: file extensions should be omitted for <RSCin> and <RSCout>
 *
 *
 *  Processing overview
 *  -------------------
 *  The program reads the file <RSCin>.rsc and the corresponding
 *  definition file.  Since there are different formats (and extensions)
 *  for the latter, it tries the possible extensions in this order:
 *      .HRD        most recent, supports mixed case names up to 16 bytes
 *      .DEF/.RSD   original, upper case names up to 8 bytes
 *      .DFN        slightly simplified version of .DEF/.RSD, with flag
 *                  values in a different sequence
 *
 *  It then deletes the trees/alerts/free strings/menu items specified
 *  in the exclude_items[] array (now maintained in its own file).
 *
 *  Finally, it writes the files <RSCout>.rsc and <RSCout>.def
 *
 *
 *  version history
 *  ---------------
 *  v1.0    roger burrows, august/2017
 *          initial release, based on erd v4.2
 *
 *  v1.1    roger burrows, august/2017
 *          . add support for menu item deletion
 *
 *  v1.2    roger burrows, october/2017
 *          . rename strdup() to avoid name conflicts
 *
 *  v1.3    roger burrows, december/2017
 *          . fix bug when deleting last item of menu
 *
 *  v1.4    roger burrows, june/2019
 *          . move exclude_items[] array to separate file for maintainability
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef ATARI
#include <getopt.h>
#endif

#define LOCAL   static  /* comment out for LatticeC debugging */
#define PRIVATE static

/*
 *  manifest RSC-related stuff
 */
#define NEWRSC_FORMAT   0x04        /* if this bit is set in the RSC version, it's TOS4 format (w/CICONBLKs) */
#define DEF             "def"       /* valid extensions for definition file */
#define DFN             "dfn"
#define HRD             "hrd"
#define RSD             "rsd"
#define MAXLEN_HRD      16          /* max length of name allowed in HRD entry */

/*
 *  our version of standard AES stuff, with changes to accommodate
 *  alternate architectures
 */
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

#define NIL             -1

typedef struct          /* big-endian short */
{
    signed char hi;
    unsigned char lo;
} SHORT;

typedef struct          /* big-endian unsigned short */
{
    unsigned char hi;
    unsigned char lo;
} USHORT;

typedef struct
{
    unsigned char b1, b2, b3, b4;
} OFFSET;

typedef struct text_edinfo
{
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

typedef struct icon_block
{
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

typedef struct bit_block
{
    OFFSET bi_pdata;
    SHORT bi_wb;
    SHORT bi_hl;
    SHORT bi_x;
    SHORT bi_y;
    SHORT bi_color;
} BITBLK;

typedef struct object
{
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

typedef struct
{
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
 * format of .DEF file entry
 */
typedef struct
{
    USHORT count;           /* # entries (in first entry, else zeros) */
    char language;          /* flag in first entry, else zeros */
    char unused;            /* always zero */
    char tree;
    char obj;
    char indicator;
    char type;
    char name[8];           /* name, not nul-terminated */
} DEF_EXT;


/*
 *  our own defines & structures
 */
#define PROGRAM_NAME    "draft"
#define VERSION         "v1.4"
#define MAX_STRLEN      300         /* max size for internal string areas */

#define OFFSET(item,base)    ((char *)item-(char *)base)

/*
 *  internal representation of resource header
 */
typedef struct
{
    unsigned short vrsn;            /* RCS version # */
    unsigned short object;          /* offset to object[] */
    unsigned short tedinfo;         /* offset to tedinfo[] */
    unsigned short iconblk;         /* offset to iconblk[] */
    unsigned short bitblk;          /* offset to bitblk[] */
    unsigned short frstr;           /* offset to free string index */
    unsigned short string;          /* offset to first string */
    unsigned short imdata;          /* offset to image data */
    unsigned short frimg;           /* offset to free image index */
    unsigned short trindex;         /* offset to object tree index */
    unsigned short nobs;            /* number of objects */
    unsigned short ntree;           /* number of trees */
    unsigned short nted;            /* number of tedinfos */
    unsigned short nib;             /* number of icon blocks */
    unsigned short nbb;             /* number of bit blocks */
    unsigned short nstring;         /* number of free strings */
    unsigned short nimages;         /* number of free images */
    unsigned short rssize;          /* total bytes in resource */
} MY_RSHDR;

/*
 *  internal representation of items in definition file
 */
typedef struct
{
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
    short indicator;
    char *name;
} DEF_ENTRY;


/*
 *  The exclude_items[] array is now maintained in draftexc.c
 */
extern char *exclude_items[];

/*
 *  other globals
 */
LOCAL const char *copyright = PROGRAM_NAME " " VERSION " copyright (c) 2017-2019 by Roger Burrows\n"
"This program is licensed under the GNU General Public License.\n"
"Please see LICENSE.TXT for details.\n";

LOCAL int debug = 0;                    /* options */
LOCAL int verbose = 0;

LOCAL char *rsc_root = NULL;            /* paths of input, output files */
LOCAL char *out_path = NULL;
LOCAL char defext[5];                   /* extension of definition file actually used */

LOCAL char inrsc[MAX_STRLEN] = "";      /* actual file names */
LOCAL char indef[MAX_STRLEN] = "";
LOCAL char outrsc[MAX_STRLEN] = "";
LOCAL char outdef[MAX_STRLEN] = "";

RSHDR *rschdr_in = NULL;                /* RSC header */
MY_RSHDR rsh_in;                        /* converted RSC header */
RSHDR *rschdr_out = NULL;               /* RSC header */
MY_RSHDR rsh_out;                       /* converted RSC header */

DEF_ENTRY *def = NULL;                  /* table of #defines from HRD/DEF/RSD/DFN */
LOCAL int num_defs = 0;                 /* entries in def */
LOCAL char langflag = 0x00;             /* language flag from def header */


/*
 *  function prototypes
 */
PRIVATE void analyse_resource(MY_RSHDR *rshout,RSHDR *in,MY_RSHDR *rshin);
PRIVATE int cmp_def(const void *a,const void *b);
PRIVATE void convert_header(RSHDR *hdr);
PRIVATE short convert_def_type(int deftype);
PRIVATE short convert_dfn_type(int dfntype,int dfnind);
PRIVATE void delete_freestr(int entry);
PRIVATE void delete_menuitem(int entry);
PRIVATE void delete_resource_item(char *name);
PRIVATE void delete_tree(int entry);
PRIVATE void display_defs(char *msg,int entries);
PRIVATE void display_header(char *msg,MY_RSHDR *hdr);
PRIVATE void error(char *s,char *t);
PRIVATE short get_short(SHORT *p);
PRIVATE unsigned short get_ushort(USHORT *p);
PRIVATE unsigned long get_offset(OFFSET *p);
PRIVATE int load_definition(char *file);
PRIVATE int load_def(FILE *fp);
PRIVATE int load_dfn(FILE *fp);
PRIVATE int load_hrd(FILE *fp);
PRIVATE RSHDR *load_rsc(char *path);
PRIVATE FILE *openfile(char *name,char *ext,char *mode);
PRIVATE void put_offset(OFFSET *p,unsigned long n);
PRIVATE void put_short(SHORT *p,short n);
PRIVATE void put_ushort(USHORT *p,unsigned short n);
PRIVATE void snip_item(OBJECT *tree,OBJECT *parent,OBJECT *item);
PRIVATE void sort_def_table(int n);
PRIVATE char *xstrdup(const char *string);
PRIVATE void usage(char *s);
PRIVATE int write_def_file(char *name,char *ext);
PRIVATE int write_rsc_file(char *name,char *ext);


#ifndef ATARI
/*
 *  max(): return maximum of two values
 */
static __inline__
short max(short a, short b)
{
    return (a > b) ? a : b;
}
#endif


int main(int argc,char *argv[])
{
char **p;
int n;

    while((n=getopt(argc,argv,"dv")) != -1)
    {
        switch(n)
        {
        case 'd':
            debug++;
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
        puts(copyright);            /* announce us ... */

    if (argc-optind != 2)
        usage("incorrect number of arguments");

    rsc_root = argv[optind++];
    out_path = argv[optind];

    sprintf(inrsc,"%s.rsc",rsc_root);
    if ((rschdr_in=load_rsc(inrsc)) == NULL)
        error("can't load resource file",inrsc);
    convert_header(rschdr_in);
    if (debug)
        display_header("Input",&rsh_in);

    num_defs = load_definition(rsc_root);
    sprintf(indef,"%s.%s",rsc_root,defext);

    if (num_defs < 0)
        error("can't load definition file",indef);

    /* the following is probably unnecessary */
    sort_def_table(num_defs);

    if (debug)
        display_defs("Input",num_defs);

    sprintf(outrsc,"%s.rsc",out_path);
    sprintf(outdef,"%s.def",out_path);

    if (debug)
    {
        printf("Input files: %s, %s\n",inrsc,indef);
        printf("Output files: %s, %s\n",outrsc,outdef);
    }

    /*
     * for safety, malloc the same memory size for output
     */
    if ((rschdr_out=calloc(1,rsh_in.rssize)) == NULL)
        error("can't malloc memory for output resource file",outrsc);

    /*
     * perform the deletions
     */
    for (p = exclude_items; *p; p++)
    {
        delete_resource_item(*p);
    }

    if (debug)
        display_defs("Output",num_defs);

    /*
     * count objects etc that will be put into output resource
     */
    analyse_resource(&rsh_out,rschdr_in,&rsh_in);
    if (debug)
        display_header("Output",&rsh_out);

    if (write_rsc_file(out_path,"rsc") < 0)
        error("can't create file",outrsc);

    if (write_def_file(out_path,DEF) < 0)
        error("can't create file",outdef);

    if (verbose)
        printf(PROGRAM_NAME " " VERSION " completed successfully\n");

    return 0;
}


/*****  deletion routines  *****/

PRIVATE void delete_tree(int entry)
{
OFFSET *trindex;
DEF_ENTRY *d = &def[entry], *d2;
short tree = d->tree;
int i;

    /*
     * remove the corresponding entry in trindex[]
     */
    trindex = (OFFSET *)((char *)rschdr_in+rsh_in.trindex) + tree;
    if (debug)
        printf("removing tree# %d from tree index\n",tree);
    memmove(trindex,trindex+1,(rsh_in.ntree-tree-1)*sizeof(OFFSET));
    rsh_in.ntree--;

    /*
     * delete all the DEF entries with a type of DIALOG or OBJECT & the same tree#
     */
    for (i = entry, d2 = d; i < num_defs; i++, d2++)
    {
        if ((d2->type != DEF_DIALOG) && (d2->type != DEF_OBJECT))
            break;
        if (d2->tree != tree)
            break;
    }
    memmove(d,d2,(num_defs-i)*sizeof(DEF_ENTRY));
    if (debug)
        printf("deleting entries %d-%d (tree# %d)\n",entry,i-1,tree);
    num_defs -= i-entry;

    /*
     * decrement the tree# of all subsequent trees
     */
    for ( ; i < num_defs; i++, d++)
    {
        if ((d->type == DEF_DIALOG) || (d->type == DEF_OBJECT))
            d->tree--;
    }
}

PRIVATE void delete_freestr(int entry)
{
OFFSET *freestr;
DEF_ENTRY *d = &def[entry];
short obj = d->obj;
int i;

    /*
     * remove the corresponding entry in the free string index
     */
    freestr = (OFFSET *)((char *)rschdr_in+rsh_in.frstr) + obj;
    if (debug)
        printf("removing obj# %d from free string index\n",obj);
    memmove(freestr,freestr+1,(rsh_in.nstring-obj-1)*sizeof(OFFSET));
    rsh_in.nstring--;

    /*
     * delete the matching DEF entry
     */
    if (debug)
        printf("deleting DEF entry %d (obj# %d)\n",entry,obj);
    memmove(d,d+1,(num_defs-entry-1)*sizeof(DEF_ENTRY));
    num_defs--;

    /*
     * decrement the object# of all subsequent alerts or free strings
     */
    for (i = entry; i < num_defs; i++, d++)
    {
        if ((d->type == DEF_ALERT) || (d->type == DEF_FREESTR))
            d->obj--;
    }
}

/*
 *  logically remove item from tree
 */
PRIVATE void snip_item(OBJECT *tree,OBJECT *parent,OBJECT *item)
{
short item_num = item-tree;
short next = get_short(&item->ob_next);
short head = get_short(&parent->ob_head);
short prev, temp;

    /*
     * handle item if at head of parent's list
     */
    if (head == item_num)
    {
        if (get_short(&parent->ob_tail) == item_num)
        {
            /* removing only child */
            next = NIL;
            put_short(&parent->ob_tail,NIL);
        }
        put_short(&parent->ob_head,next);
        return;
    }

    /*
     * handle item elsewhere:
     * find previous item and link it to next
     */
    for (prev = head; ; prev = temp)
    {
        temp = get_short(&(tree+prev)->ob_next);
        if (temp == item_num)
            break;
    }
    put_short(&(tree+prev)->ob_next,next);
    if (get_short(&parent->ob_tail) == item_num)
    {
        /* removing item at tail of list */
        put_short(&parent->ob_tail,prev);
    }
}

PRIVATE void delete_menuitem(int entry)
{
OFFSET *trindex, *trindex_end;
OBJECT *item, *parent, *tree, *obj;
DEF_ENTRY *d = &def[entry], *d2;
short curr, prev;
short delta, y, ysave, n;
int i, num_objs;

    if (debug)
        printf("removing obj# %d from tree# %d\n",d->obj,d->tree);

    /*
     * validate the menu item
     */
    trindex = (OFFSET *)((char *)rschdr_in+rsh_in.trindex) + d->tree;
    tree = (OBJECT *)((char *)rschdr_in+get_offset(trindex));
    item = tree + d->obj;
    if (get_ushort(&item->ob_type) != G_STRING)
        error("menu item is not a string:",d->name);
    if ((get_short(&item->ob_head) != NIL) || (get_short(&item->ob_tail) != NIL))
        error("menu item has children:",d->name);
    if (get_short(&item->ob_next) == NIL)
        error("menu item is root:",d->name);

    /*
     * find the parent (the enclosing box) & adjust its height
     */
    for (prev = d->obj; ; prev = curr)
    {
        curr = get_short(&(tree+prev)->ob_next);
        if (get_short(&(tree+curr)->ob_tail) == prev)
            break;
    }
    parent = tree + curr;
    delta = get_short(&item->ob_height);
    put_short(&parent->ob_height,get_short(&parent->ob_height) - delta);

    /*
     * all of the menu items below the one being deleted must be displayed
     * one line higher than before on the screen
     *
     * we accomplish that by adjusting the y position of all children whose
     * y position is greater than that of the item being deleted
     */
    ysave = get_short(&item->ob_y);
    for (curr = get_short(&parent->ob_head); ; curr = get_short(&obj->ob_next))
    {
        obj = tree + curr;
        y = get_short(&obj->ob_y);
        if (y > ysave)
            put_short(&obj->ob_y,y-delta);
        if (curr == get_short(&parent->ob_tail))
            break;
    }

    /*
     * snip the menu item out of the tree logically
     */
    snip_item(tree,parent,item);

    /*
     * we now have a one-integer gap in the object number sequence,
     * corresponding to the menu item that we are deleting
     *
     * we fix this by decrementing all next/head/tail numbers (within the
     * objects of the menu tree) that have a value greater than the gap
     */
    num_objs = (get_offset(trindex+1)-get_offset(trindex)) / sizeof(OBJECT);
    for (i = 0, obj = tree; i < num_objs; i++, obj++)
    {
        n = get_short(&obj->ob_next);
        if (n > d->obj)
            put_short(&obj->ob_next,n-1);

        n = get_short(&obj->ob_head);
        if (n > d->obj)
            put_short(&obj->ob_head,n-1);

        n = get_short(&obj->ob_tail);
        if (n > d->obj)
            put_short(&obj->ob_tail,n-1);
    }

    /*
     * then we update the DEF entries for the menu tree correspondingly
     */
    for (i = entry+1, d2 = d+1; (i < num_defs) && (d2->tree == d->tree); i++, d2++)
        d2->obj--;

    /*
     * if we are deleting the physically last object,
     * the previous item is now the last
     */
    if (get_ushort(&item->ob_flags) & LASTOB)
        put_ushort(&item[-1].ob_flags,get_ushort(&item[-1].ob_flags) | LASTOB);

    /*
     * now we delete the object itself and the corresponding DEF entry
     */
    n = item - (OBJECT *)((char *)rschdr_in+rsh_in.object);
    memmove(item,item+1,(rsh_in.nobs-n-1)*sizeof(OBJECT));
    rsh_in.nobs--;
    memmove(d,d+1,(num_defs-entry-1)*sizeof(DEF_ENTRY));
    num_defs--;

    /*
     * finally(!) we update the tree index entries for all tree numbers
     * above the current one, since all their objects have moved down one
     */
    trindex_end = (OFFSET *)((char *)rschdr_in+rsh_in.trindex) + rsh_in.ntree;
    for (trindex++; trindex < trindex_end; trindex++)
        put_offset(trindex,get_offset(trindex)-sizeof(OBJECT));
}

PRIVATE void delete_resource_item(char *name)
{
DEF_ENTRY *d;
int i;

    if (verbose)
        printf("deleting %s\n",name);

    for (i = 0, d = def; i < num_defs; i++, d++)
    {
        if (strcmp(d->name,name) == 0)
        {
            if (debug)
                printf("found: seq=%d type=%d tree=%d obj=%d ind=%d name=%s\n",
                        d->seq,d->type,d->tree,d->obj,d->indicator,d->name);
            switch(d->type)
            {
            case DEF_DIALOG:
                delete_tree(d-def);
                return;
                break;
            case DEF_ALERT:
            case DEF_FREESTR:
                delete_freestr(d-def);
                return;
                break;
            case DEF_OBJECT:
                if (def[d->tree].type == DEF_MENU)
                {
                    delete_menuitem(d-def);
                    return;
                }
                /* fall through */
            default:
                error("not a tree / alert / free string / menu item:",name);
            }
            break;
        }
    }
    if (i >= num_defs)
        error("item not found:",name);
}


/*****  input routines  *****/

/*
 *  load RSC file into memory
 *      returns NULL if there's a problem reading the file
 *      exits directly for other errors
 */
PRIVATE RSHDR *load_rsc(char *path)
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

    if (fread(base,fsize,1,rscfp) != 1)
    {
        free(base);
        fclose(rscfp);
        return NULL;
    }

    fclose(rscfp);

    rschdr = (RSHDR *)base;
    vrsn = get_ushort(&rschdr->rsh_vrsn);
    if ((vrsn&~NEWRSC_FORMAT) > 1)
    {
        sprintf(s,"unsupported version (0x%04x) found in RSC file",vrsn);
        error(s,inrsc);
    }

    rssize = get_ushort(&rschdr->rsh_rssize);
    if (((vrsn&NEWRSC_FORMAT) == 0)         /* old RSC format              */
     && (rssize != fsize))                  /*  but filesize doesn't match */
    {
        error("incorrect length in RSC file",inrsc);
    }

    return rschdr;
}

/*
 *  convert header to internal representation
 *  (save converting for each use)
 */
PRIVATE void convert_header(RSHDR *hdr)
{
    rsh_in.vrsn = get_ushort(&hdr->rsh_vrsn);
    rsh_in.object = get_ushort(&hdr->rsh_object);
    rsh_in.tedinfo = get_ushort(&hdr->rsh_tedinfo);
    rsh_in.iconblk = get_ushort(&hdr->rsh_iconblk);
    rsh_in.bitblk = get_ushort(&hdr->rsh_bitblk);
    rsh_in.frstr = get_ushort(&hdr->rsh_frstr);
    rsh_in.string = get_ushort(&hdr->rsh_string);
    rsh_in.imdata = get_ushort(&hdr->rsh_imdata);
    rsh_in.frimg = get_ushort(&hdr->rsh_frimg);
    rsh_in.trindex = get_ushort(&hdr->rsh_trindex);
    rsh_in.nobs = get_ushort(&hdr->rsh_nobs);
    rsh_in.ntree = get_ushort(&hdr->rsh_ntree);
    rsh_in.nted = get_ushort(&hdr->rsh_nted);
    rsh_in.nib = get_ushort(&hdr->rsh_nib);
    rsh_in.nbb = get_ushort(&hdr->rsh_nbb);
    rsh_in.nstring = get_ushort(&hdr->rsh_nstring);
    rsh_in.nimages = get_ushort(&hdr->rsh_nimages);
    rsh_in.rssize = get_ushort(&hdr->rsh_rssize);
}

/*
 *  load definition file into memory
 *  returns number of entries
 */
PRIVATE int load_definition(char *file)
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
PRIVATE int load_hrd(FILE *fp)
{
int i, n, rc, c;
DEF_ENTRY *d;
struct
{
    USHORT version;
    char dummy1;
    char language;
    char dummy2[4];
} hdr;
struct
{
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

    langflag = hdr.language;

    /*
     *  read through the file to determine number of entries
     */
    for (n = 0; ; n++)
    {
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

    for (i = 0, d = def; i < n; i++, d++)
    {
        if (fread(&entry,sizeof(entry),1,fp) != 1)
            return -1;
        d->type = entry.type;
        if ((d->type == DEF_ALERT)
         || (d->type == DEF_FREESTR)
         || (d->type == DEF_FREEBIT))
        {
            d->tree = get_ushort(&entry.obj);   /* swap for consistency */
            d->obj = get_ushort(&entry.tree);
        }
        else
        {
            d->tree = get_ushort(&entry.tree);
            d->obj = get_ushort(&entry.obj);
        }
        for (p = name; p < name+MAXLEN_HRD; p++)
        {
            c = fgetc(fp);
            if (c < 0)
                return -1;
            *p = c;
            if (c == 0)
                break;
        }
        d->name = xstrdup(name);
    }
    fclose(fp);

    return n;
}

/*
 *  load a .DEF/.RSD definition file
 */
PRIVATE int load_def(FILE *fp)
{
int i, n;
DEF_ENTRY *d;
DEF_EXT entry;
char temp[9];

    if (fread(&entry,sizeof(entry),1,fp) != 1)
        return -1;

    n = get_ushort(&entry.count);       /* number of entries (big-endian) */

    def = calloc(n,sizeof(DEF_ENTRY));
    if (!def)
        return -1;

    langflag = entry.language;
    temp[8] = '\0';                     /* ensure null-termination */

    if (fseek(fp,0,SEEK_SET) < 0)
        return -1;

    for (i = 0, d = def; i < n; i++, d++)
    {
        if (fread(&entry,sizeof(entry),1,fp) != 1)
            return -1;
        d->type = convert_def_type(entry.type);
        if ((d->type == DEF_DIALOG)
         || (d->type == DEF_MENU))
        {
            d->tree = entry.obj;        /* swap for consistency */
            d->obj = entry.tree;
        }
        else
        {
            d->tree = entry.tree;
            d->obj = entry.obj;
        }
        d->indicator = entry.indicator;
        memcpy(temp,entry.name,8);
        d->name = xstrdup(temp);
    }
    fclose(fp);

    return n;
}

/*
 *  load a .DFN definition file
 */
PRIVATE int load_dfn(FILE *fp)
{
int i, n;
DEF_ENTRY *d;
struct
{
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

    langflag = 0x00;                    /* not stored in .DFN */
    temp[8] = '\0';                     /* ensure null-termination */

    if (fseek(fp,0,SEEK_SET) < 0)
        return -1;

    for (i = 0, d = def; i < n; i++, d++)
    {
        if (fread(&entry,sizeof(entry),1,fp) != 1)
            return -1;
        d->type = convert_dfn_type(entry.type,entry.indicator);
        if ((d->type == DEF_DIALOG)
         || (d->type == DEF_MENU))
        {
            d->tree = entry.obj;        /* swap for consistency */
            d->obj = entry.tree;
        }
        else
        {
            d->tree = entry.tree;
            d->obj = entry.obj;
        }
        d->indicator = entry.indicator;
        memcpy(temp,entry.name,8);
        d->name = xstrdup(temp);
    }
    fclose(fp);

    return n;
}

/*
 *  convert the type from a .DEF/.RSD entry to the standard .HRD type
 */
PRIVATE short convert_def_type(int deftype)
{
short new;

    switch(deftype)
    {
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
 *  convert the type from a .DFN entry to the standard .HRD type
 */
PRIVATE short convert_dfn_type(int dfntype,int dfnind)
{
short new;

    switch(dfntype)
    {
    case 0:
        new = DEF_OBJECT;
        break;
    case 1:
        new = DEF_FREESTR;
        break;
    case 2:
        new = dfnind ? DEF_FREEBIT : DEF_MENU;
        break;
    case 3:
        new = DEF_DIALOG;
        break;
    case 4:
        new = DEF_ALERT;
        break;
    default:
        new = -1;
        break;
    }

    return new;
}

/*
 *  convert the type from a .HRD type to a .DEF/.RSD type
 */
PRIVATE short convert_hrd_type(int hrdtype)
{
short new;

    switch(hrdtype)
    {
    case DEF_OBJECT:
        new = 0;
        break;
    case DEF_MENU:
        new = 2;
        break;
    case DEF_DIALOG:
        new = 3;
        break;
    case DEF_ALERT:
        new = 4;
        break;
    case DEF_FREESTR:
        new = 5;
        break;
    case DEF_FREEBIT:
        new = 6;
        break;
    default:
        new = -1;
        break;
    }

    return new;
}


/*****  output routines *****/

PRIVATE short get_textlen(OFFSET *p)
{
unsigned long offset;

    offset = get_offset(p);
    return strlen((char *)rschdr_in+offset) + 1;
}

PRIVATE long get_iconsize(ICONBLK *ib)
{
    return (long)((get_short(&ib->ib_wicon) + 15) >> 4) * get_short(&ib->ib_hicon) * 2;
}

PRIVATE void analyse_resource(MY_RSHDR *rshout,RSHDR *in,MY_RSHDR *rshin)
{
OFFSET *p;
OBJECT *obj, *first;
TEDINFO *ted;
BITBLK *bb;
ICONBLK *ib;
int i;
unsigned long offset;
unsigned short ptextlen, pvalidlen;
unsigned short string_textlen = 0, frstr_textlen = 0;
unsigned short frimg_datalen = 0, image_datalen = 0;

    /*
     * process the trees, first doing just the string/button/title objects
     *
     * resources generated by Interface are observed to have the strings for
     * this type of object preceding the strings for TEDINFO objects
     */
    for (i = 0, p = (OFFSET *)((char *)in+rshin->trindex); i < rshin->ntree; i++, p++)
    {
        offset = get_offset(p);
        rshout->ntree++;
        first = (OBJECT *)((char *)in+offset);
        for (obj = first; ; obj++)
        {
            rshout->nobs++;
            switch(get_ushort(&obj->ob_type)&0xff)
            {
            case G_TEXT:
            case G_BOXTEXT:
            case G_FTEXT:
            case G_FBOXTEXT:
                rshout->nted++;
                ted = (TEDINFO *)((char *)in+get_offset(&obj->ob_spec));
                /*
                 * add up the lengths of strings used
                 *
                 * NOTE: using strlen() to find the te_ptext length is not reliable.
                 * the following kludge seems to work.
                 */
                ptextlen = get_textlen(&ted->te_ptext);
                pvalidlen = get_textlen(&ted->te_pvalid);
                string_textlen += max(ptextlen,pvalidlen);
                string_textlen += get_textlen(&ted->te_ptmplt);
                string_textlen += pvalidlen;    /* possibly we should use max() here */
                break;
            case G_STRING:
            case G_BUTTON:
            case G_TITLE:
                string_textlen += get_textlen(&obj->ob_spec);
                break;
            case G_IMAGE:
                rshout->nbb++;
                bb = (BITBLK *)((char *)in+get_offset(&obj->ob_spec));
                image_datalen += get_short(&bb->bi_wb) * get_short(&bb->bi_hl);
                break;
            case G_ICON:
                rshout->nib++;
                ib = (ICONBLK *)((char *)in+get_offset(&obj->ob_spec));
                string_textlen += get_textlen(&ib->ib_ptext);
                /* add mask + data lengths */
                image_datalen += 2 * get_iconsize(ib);
                break;
            }
            if (get_ushort(&obj->ob_flags)&LASTOB)
                break;
        }
    }

    /*
     * process the free strings
     */
    for (i = 0, p = (OFFSET *)((char *)in+rshin->frstr); i < rshin->nstring; i++, p++)
    {
        offset = get_offset(p);
        rshout->nstring++;
        frstr_textlen += strlen((char *)in+offset) + 1;
    }

    /*
     * process the free images
     */
    for (i = 0, p = (OFFSET *)((char *)in+rshin->frimg); i < rshin->nimages; i++, p++)
    {
        rshout->nbb++;
        rshout->nimages++;
        offset = get_offset(p);
        bb = (BITBLK *)((char *)in+offset);
        frimg_datalen += get_short(&bb->bi_wb) * get_short(&bb->bi_hl);
    }

#if 0
    printf("string textlen = %d\n",string_textlen);
    printf("frstr textlen = %d\n",frstr_textlen);
    printf("image datalen = %d\n",image_datalen);
    printf("frimg datalen = %d\n",frimg_datalen);
#endif

    rshout->trindex = sizeof(RSHDR);
    rshout->object = rshout->trindex + rshout->ntree * sizeof(OFFSET);
    rshout->tedinfo = rshout->object + rshout->nobs * sizeof(OBJECT);
    rshout->iconblk = rshout->tedinfo + rshout->nted * sizeof(TEDINFO);
    rshout->bitblk = rshout->iconblk + rshout->nib * sizeof(ICONBLK);
    rshout->frstr = rshout->bitblk + rshout->nbb * sizeof(BITBLK);
    rshout->string = rshout->frstr + rshout->nstring * sizeof(OFFSET) + frstr_textlen;
    rshout->frimg = rshout->string + string_textlen;
    /* align to word */
    rshout->frimg = (rshout->frimg + 1) & ~1;
    rshout->imdata = rshout->frimg + rshout->nimages * sizeof(OFFSET);
    rshout->rssize = rshout->imdata + image_datalen + frimg_datalen;
}


PRIVATE void build_rschdr(RSHDR *out,MY_RSHDR *rsh)
{
    put_ushort(&out->rsh_vrsn,0);
    put_ushort(&out->rsh_object,rsh->object);
    put_ushort(&out->rsh_tedinfo,rsh->tedinfo);
    put_ushort(&out->rsh_iconblk,rsh->iconblk);
    put_ushort(&out->rsh_bitblk,rsh->bitblk);
    put_ushort(&out->rsh_frstr,rsh->frstr);
    put_ushort(&out->rsh_string,rsh->string);
    put_ushort(&out->rsh_imdata,rsh->imdata);
    put_ushort(&out->rsh_frimg,rsh->frimg);
    put_ushort(&out->rsh_trindex,rsh->trindex);
    put_ushort(&out->rsh_nobs,rsh->nobs);
    put_ushort(&out->rsh_ntree,rsh->ntree);
    put_ushort(&out->rsh_nted,rsh->nted);
    put_ushort(&out->rsh_nib,rsh->nib);
    put_ushort(&out->rsh_nbb,rsh->nbb);
    put_ushort(&out->rsh_nstring,rsh->nstring);
    put_ushort(&out->rsh_nimages,rsh->nimages);
    put_ushort(&out->rsh_rssize,rsh->rssize);
}


/*
 * copy objects, updating the output tree index
 */
PRIVATE void copy_objects(RSHDR *out,MY_RSHDR *rshout,RSHDR *in,MY_RSHDR *rshin)
{
OFFSET *treein, *treeout;
OBJECT *objin, *objout;
unsigned long offset;
int i;

    treein = (OFFSET *)((char *)in + rshin->trindex);
    treeout = (OFFSET *)((char *)out + rshout->trindex);
    objout = (OBJECT *)((char *)out + rshout->object);
    for (i = 0; i < rshin->ntree; i++, treein++)
    {
        offset = get_offset(treein);
        put_offset(treeout++,OFFSET(objout,out));
        for (objin = (OBJECT *)((char *)in + offset); ; objin++)
        {
            *objout++ = *objin;
            if (get_ushort(&objin->ob_flags)&LASTOB)
                break;
        }
    }
}


/*
 * copy tedinfo/iconblk/bitblk, updating the output objects
 */
PRIVATE void copy_tib(RSHDR *out,MY_RSHDR *rshout,RSHDR *in)
{
OBJECT *objout;
TEDINFO *tedout;
BITBLK *bbout;
ICONBLK *ibout;
void *p;
int i;

    objout = (OBJECT *)((char *)out + rshout->object);
    tedout = (TEDINFO *)((char *)out + rshout->tedinfo);
    bbout = (BITBLK *)((char *)out + rshout->bitblk);
    ibout = (ICONBLK *)((char *)out + rshout->iconblk);
    for (i = 0; i < rshout->nobs; i++, objout++)
    {
        /*
         * the output object currently points to the input TEDINFO/ICONBLK/BITBLK
         */
        p = (char *)in + get_offset(&objout->ob_spec);
        switch(get_ushort(&objout->ob_type)&0xff)
        {
        case G_TEXT:
        case G_BOXTEXT:
        case G_FTEXT:
        case G_FBOXTEXT:
            put_offset(&objout->ob_spec,OFFSET(tedout,out));
            *tedout++ = *(TEDINFO *)p;
            break;
        case G_IMAGE:
            put_offset(&objout->ob_spec,OFFSET(bbout,out));
            *bbout++ = *(BITBLK *)p;
            break;
        case G_ICON:
            put_offset(&objout->ob_spec,OFFSET(ibout,out));
            *ibout++ = *(ICONBLK *)p;
            break;
        }
    }
}


/*
 * copy free string index & free strings
 */
PRIVATE void copy_freestr(RSHDR *out,MY_RSHDR *rshout,RSHDR *in,MY_RSHDR *rshin)
{
OFFSET *indexin, *indexout;
char *stringin, *stringout;
unsigned long offset;
int i;

    indexin = (OFFSET *)((char *)in + rshin->frstr);
    indexout = (OFFSET *)((char *)out + rshout->frstr);
    stringout = (char *)out + rshout->frstr + rshout->nstring * sizeof(OFFSET);
    for (i = 0; i < rshin->nstring; i++, indexin++)
    {
        offset = get_offset(indexin);
        stringin = (char *)in + offset;
        put_offset(indexout++,OFFSET(stringout,out));
        strcpy(stringout,stringin);
        stringout += strlen(stringout) + 1;
    }
}


/*
 * copy strings associated with objects:
 *      string/button/title/tedinfo
 */
PRIVATE void copy_strings(RSHDR *out,MY_RSHDR *rshout,RSHDR *in)
{
OBJECT *obj;
ICONBLK *ib;
TEDINFO *ted;
char *str;
unsigned long ptext, ptmplt, pvalid;
int ptextlen, pvalidlen;
int i, len;

    /*
     * process all output objects, first doing just the string/button/title objects
     *
     * resources generated by Interface are observed to have the strings for
     * this type of object preceding the strings for TEDINFO & ICONBLK objects
     */
    obj = (OBJECT *)((char *)out + rshout->object);
    str = (char *)out + rshout->string;
    for (i = 0; i < rshout->nobs; i++, obj++)
    {
        switch(get_ushort(&obj->ob_type)&0xff)
        {
        case G_STRING:
        case G_BUTTON:
        case G_TITLE:
            strcpy(str,(char *)in+get_offset(&obj->ob_spec));
            put_offset(&obj->ob_spec,OFFSET(str,out));
            str += strlen(str) + 1;
            break;
        }
    }

    /*
     * process just the tedinfo/iconblk objects
     */
    obj = (OBJECT *)((char *)out + rshout->object);
    for (i = 0; i < rshout->nobs; i++, obj++)
    {
        switch(get_ushort(&obj->ob_type)&0xff)
        {
        case G_TEXT:
        case G_BOXTEXT:
        case G_FTEXT:
        case G_FBOXTEXT:
            ted = (TEDINFO *)((char *)out+get_offset(&obj->ob_spec));
            ptext = get_offset(&ted->te_ptext);
            ptmplt = get_offset(&ted->te_ptmplt);
            pvalid = get_offset(&ted->te_pvalid);
            /*
             * see analyse_resource() for why we do the following
             */
            ptextlen = strlen((char *)in+ptext) + 1;
            pvalidlen = strlen((char *)in+pvalid) + 1;
            len = max(ptextlen,pvalidlen);
            memcpy(str,(char *)in+ptext,len);
            put_offset(&ted->te_ptext,OFFSET(str,out));
            str += len;
            strcpy(str,(char *)in+ptmplt);
            put_offset(&ted->te_ptmplt,OFFSET(str,out));
            str += strlen(str) + 1;
            strcpy(str,(char *)in+pvalid);
            put_offset(&ted->te_pvalid,OFFSET(str,out));
            str += pvalidlen;
            break;
        case G_ICON:
            ib = (ICONBLK *)((char *)out+get_offset(&obj->ob_spec));
            strcpy(str,(char *)in+get_offset(&ib->ib_ptext));
            put_offset(&ib->ib_ptext,OFFSET(str,out));
            str += strlen(str) + 1;
            break;
        }
    }
}


/*
 * copy free image index & free image BITBLKs
 */
PRIVATE void copy_frimg(RSHDR *out,MY_RSHDR *rshout,RSHDR *in,MY_RSHDR *rshin)
{
OFFSET *indexin, *indexout;
BITBLK *bbin, *bbout;
int i;

    indexin = (OFFSET *)((char *)in + rshin->frimg);
    indexout = (OFFSET *)((char *)out + rshout->frimg);
    /* skip BITBLKs that belong to objects, since these have already been output */
    bbout = (BITBLK *)((char *)out + rshout->bitblk + (rshin->nbb - rshin->nimages) * sizeof(BITBLK));
    for (i = 0; i < rshout->nimages; i++, indexin++)
    {
        bbin = (BITBLK *)((char *)in + get_offset(indexin));
        put_offset(indexout++,OFFSET(bbout,out));
        *bbout++ = *bbin;
    }
}


/*
 * copy image data, updating the BITBLK & ICONBLK pointers
 */
PRIVATE void copy_imdata(RSHDR *out,MY_RSHDR *rshout,RSHDR *in)
{
BITBLK *bb;
ICONBLK *ib;
char *data;
long size;
int i;

    data = (char *)out + rshout->imdata;
    bb = (BITBLK *)((char *)out + rshout->bitblk);
    for (i = 0; i < rshout->nbb; i++, bb++)
    {
        size = get_short(&bb->bi_wb) * get_short(&bb->bi_hl);
        memcpy(data,(char *)in+get_offset(&bb->bi_pdata),size);
        put_offset(&bb->bi_pdata,OFFSET(data,out));
        data += size;
    }

    ib = (ICONBLK *)((char *)out + rshout->iconblk);
    for (i = 0; i < rshout->nib; i++, ib++)
    {
        size = get_iconsize(ib);
        memcpy(data,(char *)in+get_offset(&ib->ib_pmask),size);
        put_offset(&ib->ib_pmask,OFFSET(data,out));
        data += size;
        memcpy(data,(char *)in+get_offset(&ib->ib_pdata),size);
        put_offset(&ib->ib_pdata,OFFSET(data,out));
        data += size;
    }
}


PRIVATE int write_rsc_file(char *name,char *ext)
{
FILE *fp;
int rc = 0;

    fp = openfile(name,ext,"wb");
    if (!fp)
        return -1;

    build_rschdr(rschdr_out,&rsh_out);
    copy_objects(rschdr_out,&rsh_out,rschdr_in,&rsh_in);
    copy_tib(rschdr_out,&rsh_out,rschdr_in);
    copy_freestr(rschdr_out,&rsh_out,rschdr_in,&rsh_in);
    copy_strings(rschdr_out,&rsh_out,rschdr_in);
    copy_frimg(rschdr_out,&rsh_out,rschdr_in,&rsh_in);
    copy_imdata(rschdr_out,&rsh_out,rschdr_in);

    /* write entire resource at once */
    if (fwrite(rschdr_out,rsh_out.rssize,1,fp) != 1)
        rc = -1;

    fclose(fp);

    return rc;
}


PRIVATE int write_def_file(char *name,char *ext)
{
FILE *fp;
DEF_ENTRY *d;
DEF_EXT entry;
int i, rc = 0;

    fp = openfile(name,ext,"wb");
    if (!fp)
        return -1;

    for (i = 0, d = def; (i < num_defs) && (rc == 0); i++, d++)
    {
        if (i == 0)
        {
            put_ushort(&entry.count,num_defs);
            entry.language = langflag;
        }
        else
        {
            put_ushort(&entry.count,0);
            entry.language = 0x00;
        }
        entry.unused = 0x00;
        if ((d->type == DEF_DIALOG)
         || (d->type == DEF_MENU))
        {
            entry.tree = d->obj;        /* unswap */
            entry.obj = d->tree;
        }
        else
        {
            entry.tree = d->tree;
            entry.obj = d->obj;
        }
        entry.indicator = d->indicator;
        entry.type = convert_hrd_type(d->type);
        memcpy(entry.name,d->name,8);
        if (fwrite(&entry,sizeof(entry),1,fp) != 1)
            rc = -1;
    }
    fclose(fp);

    return rc;
}


/*
 *  display info from converted RSC header
 */
PRIVATE void display_header(char *msg,MY_RSHDR *hdr)
{
    printf("\n%s RSC header (version %d)\n",msg,hdr->vrsn); /* version */
    printf("  Object offset  %5d\n",hdr->object);           /* offset to object[] */
    printf("  TEDINFO offset %5d\n",hdr->tedinfo);          /* offset to tedinfo[] */
    printf("  ICONBLK offset %5d\n",hdr->iconblk);          /* offset to iconblk[] */
    printf("  BITBLK offset  %5d\n",hdr->bitblk);           /* offset to bitblk[] */
    printf("  FRSTR offset   %5d\n",hdr->frstr);            /* offset to free strings index */
    printf("  STRING offset  %5d\n",hdr->string);           /* offset to string data */
    printf("  IMDATA offset  %5d\n",hdr->imdata);           /* offset to image data */
    printf("  FRIMG offset   %5d\n",hdr->frimg);            /* offset to free image index */
    printf("  TRINDEX offset %5d\n",hdr->trindex);          /* offset to object tree index */
    printf("  %5d objects\n",hdr->nobs);                    /* number of objects */
    printf("  %5d trees\n",hdr->ntree);                     /* number of trees */
    printf("  %5d TEDINFOs\n",hdr->nted);                   /* number of tedinfos */
    printf("  %5d ICONBLKs\n",hdr->nib);                    /* number of icon blocks */
    printf("  %5d BITBLKs\n",hdr->nbb);                     /* number of blt blocks */
    printf("  %5d free strings\n",hdr->nstring);            /* number of free strings */
    printf("  %5d free images\n",hdr->nimages);             /* number of free images */
    printf("  Resource size %5d bytes\n\n",hdr->rssize);    /* total bytes in resource */
}

/*
 *  sort the definition table
 */
PRIVATE void sort_def_table(int n)
{
int i;
DEF_ENTRY *d;

    /* insert sequencing number */
    for (i = 0, d = def; i < n; i++, d++)
    {
        switch(d->type)
        {
        case DEF_DIALOG:
        case DEF_MENU:
        case DEF_OBJECT:
            d->seq = 0;
            break;
        case DEF_ALERT:
        case DEF_FREESTR:
            d->seq = 1;
            break;
        case DEF_FREEBIT:
            d->seq = 2;
            break;
        default:
            d->seq = 3;
            break;
        }
    }

    qsort(def,n,sizeof(DEF_ENTRY),cmp_def);
}

PRIVATE int cmp_def(const void *a,const void *b)
{
const DEF_ENTRY *d1 = a;
const DEF_ENTRY *d2 = b;

    if (d1->seq != d2->seq)
        return d1->seq - d2->seq;

    if (d1->tree != d2->tree)
        return d1->tree - d2->tree;

    return d1->obj - d2->obj;
}

PRIVATE void display_defs(char *msg,int n)
{
int i;
DEF_ENTRY *d;

    printf("%s definitions table (%d entries)\n",msg,n);
    printf("  Seq  Type  Tree  Object  Ind  Name\n");
    for (i = 0, d = def; i < n; i++, d++)
        printf("   %2d   %2d    %2d     %2d    %2d    %s\n",
                d->seq,d->type,d->tree,d->obj,d->indicator,d->name);
    printf("\n");
}


/*****  miscellaneous support routines  *****/


/*
 *  convert big-endian short to short
 */
PRIVATE short get_short(SHORT *p)
{
    return (p->hi<<8) | p->lo;
}

/*
 *  convert big-endian unsigned short to unsigned short
 */
PRIVATE unsigned short get_ushort(USHORT *p)
{
    return (p->hi<<8) | p->lo;
}

/*
 *  convert big-endian offset to unsigned long
 */
PRIVATE unsigned long get_offset(OFFSET *p)
{
    return (p->b1<<24) | (p->b2<<16) | (p->b3<<8) | p->b4;
}

/*
 * convert short to big-endian
 */
PRIVATE void put_short(SHORT *p,short n)
{
    p->hi = (n>>8) & 0xff;
    p->lo = n & 0xff;
}

/*
 * convert unsigned short to big-endian
 */
PRIVATE void put_ushort(USHORT *p,unsigned short n)
{
    p->hi = (n>>8) & 0xff;
    p->lo = n & 0xff;
}

/*
 * convert unsigned long to big-endian offset
 */
PRIVATE void put_offset(OFFSET *p,unsigned long n)
{
    p->b1 = (n>>24) & 0xff;
    p->b2 = (n>>16) & 0xff;
    p->b3 = (n>>8) & 0xff;
    p->b4 = n & 0xff;
}

/*
 *  open a file, specified as name & extension separately
 */
PRIVATE FILE *openfile(char *name,char *ext,char *mode)
{
char s[MAX_STRLEN];

    sprintf(s,"%s.%s",name,ext);
    return fopen(s,mode);
}

/*
 *  allocate memory & copy string to it
 *
 *  this is the same as the quasi-standard function strdup();
 *  however, strdup() is not available when compiling under gcc
 *  with the -ansi option.  this used to be called strdup(), but
 *  the name was changed to avoid name conflicts when compiling
 *  with systems that do provide strdup().
 */
PRIVATE char *xstrdup(const char *string)
{
char *p;

    p = malloc(strlen(string)+1);
    if (p)
        strcpy(p,string);

    return p;
}

PRIVATE void error(char *s,char *t)
{
    printf("  error: %s",s);
    if (t)
        printf(" %s",t);
    printf("\n");
    exit(1);
}

PRIVATE void usage(char *s)
{
    if (*s)
        printf("%s %s: %s\n",PROGRAM_NAME,VERSION,s);
    printf("usage: %s [-d] [-v] <rsc_in> <rsc_out>\n",PROGRAM_NAME);

    exit(2);
}
