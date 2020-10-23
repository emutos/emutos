/*
 * bdosmain.c - GEMDOS main function dispatcher
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2020 The EmuTOS development team
 *
 * Authors:
 *  EWF  Eric W. Fleischman
 *  JSL  Jason S. Loveman
 *  SCC  Steven C. Cavender
 *  LTG  Louis T. Garavaglia
 *  KTB  Karl T. Braun (kral)
 *  ACH  Anthony C. Hay (DR UK)
 *  MAD  Martin Doering
 *  THH  Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "fs.h"
#include "biosdefs.h"
#include "mem.h"
#include "proc.h"
#include "console.h"
#include "time.h"
#include "gemerror.h"
#include "biosbind.h"
#include "string.h"
#include "bdosstub.h"
#include "tosvars.h"

/*
**  externals
*/

/*
 * in rwa.S
 */

void enter(void);       /* defined in rwa.S */
void bdos_trap2(void);  /* defined in rwa.S */
PFVOID old_trap2; /* Old trap #2 handler, also used by rwa.S */

/*
 *  prototypes / forward declarations
 */

static long ni(void);
static long xgetver(void);


/*
 *  defines for some standard GEMDOS calls
 */
#define GEMDOS_FCREATE  0x3c
#define GEMDOS_FOPEN    0x3d
#define GEMDOS_FREAD    0x3f
#define GEMDOS_FWRITE   0x40


/*
 *  MiNT-compatible EOF indicator for character
 *  devices redirected to files
 */
#define EOF_INDICATOR   0x0000ff1aL


/*
 * the basepage for the initial process
 *
 * this used to be obtained via MGET, but that was a bit pointless,
 * since it was never freed
 */
static PD initial_basepage;

/* initial environment string */
static const char double_nul[2] __attribute__ ((aligned (2))) = { 0, 0 };


/*
 * SPECNAME - special name descriptor
 *
 * Each entry in the special name table (below) contains a special
 * name with the corresponding handle
 */
typedef struct {
    char *name;
    long handle;
} SPECNAME;


/*
 * table of special names, used by Fopen()/Fcreate() to access
 * a character device.  note that special names can be upper or
 * lower case, but NOT mixed case.
 */
static const SPECNAME specname_table[] =
{
    { "CON:", 0x0000ffffL },
    { "con:", 0x0000ffffL },
    { "AUX:", 0x0000fffeL },
    { "aux:", 0x0000fffeL },
    { "PRN:", 0x0000fffdL },
    { "prn:", 0x0000fffdL },
};
#define SN_ENTRIES  ARRAY_SIZE(specname_table)


/*
 * FND - Function Descriptor
 *
 * Each entry in the function table (below) consists of the address of
 * the function which corresponds to the function number, and a function
 * type.
 */
typedef struct
{
    union {
        long  (*v)(void);
        long  (*w)(short);
        long  (*ww)(short, short);
        long  (*www)(short, short, short);
        long  (*wwww)(short, short, short, short);
        long  (*wwwww)(short, short, short, short, short);
        long  (*wwwwwww)(short, short, short, short, short, short, short);
    } fncall;
    UBYTE stdio_typ;    /* Standard I/O channel (highest bit must be set, too) */
    UBYTE wparms;       /* Size of parameters in WORDs */
} FND;


/*
 * funcs - table of os functions, indexed by function number
 *
 * Each entry is for an FND structure. NI is used
 * as the address for functions not implemented.
 */
static const FND funcs[] =
{
#define F(x) { (PFLONG)(x) }
#define NI F(ni)

     { F(x0term), 0, 0 },       /* 0x00 */

    /*
     * console functions
     *
     * on these functions, the 0x80 flag indicates std file used
     * 0x80 is std in, 0x81 is stdout, 0x82 is stdaux, 0x83 stdprn
     */

    { F(xconin),   0x80, 0 },   /* 0x01 */
    { F(xconout),  0x81, 1 },   /* 0x02 */
    { F(xauxin),   0x82, 0 },   /* 0x03 */
    { F(xauxout),  0x82, 1 },   /* 0x04 */
    { F(xprtout),  0x83, 1 },   /* 0x05 */
    { F(xrawio),   0,    1 },   /* 0x06 */
    { F(xrawcin),  0x80, 0 },   /* 0x07 */
    { F(xnecin),   0x80, 0 },   /* 0x08 */
    { F(xconws),   0x81, 2 },   /* 0x09 */
    { F(xconrs),   0x80, 2 },   /* 0x0A */
    { F(xconstat), 0x80, 0 },   /* 0x0B */

    /*
     * disk functions
     *
     * on these functions the 0x80 flag indicates whether a handle
     * is required, the low bits represent the parameter ordering,
     * as usual.
     */

    { NI, 0, 0 },
    { NI, 0, 0 },

    { F(xsetdrv),  0, 1 },      /* 0x0E */

    { NI, 0, 0 },

    /*
     * extended console functions
     *
     * Here the 0x80 flag indicates std file used, as above
     */

    { F(xconostat), 0x81, 0 },  /* 0x10 */
    { F(xprtostat), 0x83, 0 },  /* 0x11 */
    { F(xauxistat), 0x82, 0 },  /* 0x12 */
    { F(xauxostat), 0x82, 0 },  /* 0x13 */

#if CONF_WITH_ALT_RAM
    { F(xmaddalt),  0, 4 },     /* 0x14 */
#else
    { NI, 0, 0 },               /* 0x14 */
#endif
    { F(srealloc),  0, 2 },     /* 0x15 */
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },

    { F(xgetdrv),  0, 0 },      /* 0x19 */
    { F(xsetdta),  0, 2 },      /* 0x1A */

    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },

    /* xgsps */

    { NI, 0, 0 },               /* 0x20 */
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },

    { F(xgetdate), 0, 0 },      /* 0x2A */
    { F(xsetdate), 0, 1 },      /* 0x2B */
    { F(xgettime), 0, 0 },      /* 0x2C */
    { F(xsettime), 0, 1 },      /* 0x2D */

    { NI, 0, 0 },

    { F(xgetdta),  0, 0 },      /* 0x2F */
    { F(xgetver),  0, 0 },      /* 0x30 */
    { F(xtermres), 0, 3 },      /* 0x31 */

    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },

    { F(xgetfree), 0, 3 },      /* 0x36 */

    { NI, 0, 0 },
    { NI, 0, 0 },

    { F(xmkdir),   0, 2 },      /* 0x39 */
    { F(xrmdir),   0, 2 },      /* 0x3A */
    { F(xchdir),   0, 2 },      /* 0x3B */
    { F(xcreat),   0, 3 },      /* 0x3C */
    { F(xopen),    0, 3 },      /* 0x3D */
    { F(xclose),   0, 1 },      /* 0x3E - will handle its own redirection */
    { F(xread),    0x82, 5 },   /* 0x3F */
    { F(xwrite),   0x82, 5 },   /* 0x40 */
    { F(xunlink),  0, 2 },      /* 0x41 */
    { F(xlseek),   0x81, 4 },   /* 0x42 */
    { F(xchmod),   0, 4 },      /* 0x43 */
    { F(xmxalloc), 0, 3 },      /* 0x44 */
    { F(xdup),     0, 1 },      /* 0x45 */
    { F(xforce),   0, 2 },      /* 0x46 */
    { F(xgetdir),  0, 3 },      /* 0x47 */
    { F(xmalloc),  0, 2 },      /* 0x48 */
    { F(xmfree),   0, 2 },      /* 0x49 */
    { F(xsetblk),  0, 5 },      /* 0x4A */
    { F(xexec),    0, 7 },      /* 0x4B */
    { F(xterm),    0, 1 },      /* 0x4C */

    { NI, 0, 0 },

    { F(xsfirst),  0, 3 },      /* 0x4E */
    { F(xsnext),   0, 0 },      /* 0x4F */

    { NI, 0, 0 },               /* 0x50 */
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },
    { NI, 0, 0 },

    { F(xrename),  0, 5 },      /* 0x56 */
    { F(xgsdtof),  0, 4 }       /* 0x57 */
#undef F
#undef NI
};
#define MAX_FNCALL (ARRAY_SIZE(funcs) - 1)


/*
 *  xgetver -
 *      return current version number
 */
static long xgetver(void)
{
    return (long)GEMDOS_VERSION;
}


/*
 *  ni -
 */
static long ni(void)
{
    return EINVFN;
}


/*
 *  osinit - the bios calls this routine to initialize the os
 */

void osinit_before_xmaddalt(void)
{
    /* take over the handling of TRAP #1 */
    Setexc(0x21, (long)enter);

    /*
     * intercept TRAP #2 only for xterm(), keeping the old value
     * so that our trap handler can call the old one
     */
    old_trap2 = (PFVOID) Setexc(0x22, (long)bdos_trap2);

    bufl_init();    /* initialize BDOS buffer list */

    osmem_init();
    umem_init();
}

/* BIOS may call xmaddalt() between those two calls */

void osinit_after_xmaddalt(void)
{
    /* Set up initial process. Required by Malloc() */
    run = &initial_basepage;
    run->p_flags = PF_STANDARD;
    run->p_env = CONST_CAST(char *,double_nul);

    time_init();

    KDEBUG(("BDOS: address of basepage = %p\n", run));

    stdhdl_init();  /* set up system initial standard handles */

    KDEBUG(("BDOS: cinit - osinit successful ...\n"));
}


/*
 *  freetree -  free the directory node tree
 */
static void freetree(DND *d)
{
    DIRTBL_ENTRY *p;
    int i;

    if (d->d_left)
        freetree(d->d_left);
    if (d->d_right)
        freetree(d->d_right);
    if (d->d_ofd)
    {
        xmfreblk(d->d_ofd);
    }
    for (i = 1, p = dirtbl+1; i < NCURDIR; i++, p++)
    {
        if (p->dnd == d)
        {
            p->dnd = NULL;
            p->use = 0;
        }
    }
    xmfreblk(d);
}


/*
 *  offree -
 */
static void offree(DMD *d)
{
    int i;
    OFD *f;

    for (i = 0; i < OPNFILES; i++)
    {
        if (((long) (f = sft[i].f_ofd)) > 0L)
        {
            if (f->o_dmd == d)
            {
                xmfreblk(f);
                sft[i].f_ofd = 0;
                sft[i].f_own = 0;
                sft[i].f_use = 0;
            }
        }
    }
}


/*
 *  osif - C implementation of trap #1. Called by _enter.
 */
long osif(short *pw);   /* called only from rwa.S */
long osif(short *pw)
{
    char **pb, *pb2, *p, ctmp;
    BPB *b;
    BCB *bx;
    DND *dn;
    int typ, h, i, fn;
    int num, max;
    long rc, numl;
    const FND *f;

restrt:
    fn = pw[0];
    if (fn > MAX_FNCALL)
        return EINVFN;

    KDEBUG(("BDOS (fn=0x%04x)\n",fn));

    if (setjmp(errbuf))
    {
        rc = errcode;
        /* hard error processing */
        KDEBUG(("Error code gotten from some longjmp(), back in osif(): %ld\n",rc));

        /* is this a media change ? */
        if (rc == E_CHNG)
        {
            /* first, out with the old stuff */
            dn = drvtbl[errdrv]->m_dtl;
            offree(drvtbl[errdrv]);
            xmfreblk(drvtbl[errdrv]);
            drvtbl[errdrv] = 0;

            if (dn)
                freetree(dn);

            for (i = 0; i < 2; i++)
                for (bx = bufl[i]; bx; bx = bx->b_link)
                    if (bx->b_bufdrv == errdrv)
                        bx->b_bufdrv = -1;

            /* then, in with the new */
            b = (BPB *)Getbpb(errdrv);
            if ((long)b <= 0)
            {
                drvsel &= ~(1L<<errdrv);
                if (b)
                    return (long)b;
                return rc;
            }

            if (log_media(b,errdrv))
                return ENSMEM;

            rwerr = 0;
            errdrv = 0;
            goto restrt;
        }

        /* else handle as hard error on disk for now */

        for (i = 0; i < 2; i++)
            for (bx = bufl[i]; bx; bx = bx->b_link)
                if (bx->b_bufdrv == errdrv)
                    bx->b_bufdrv = -1;
        return rc;
    }

    f = &funcs[fn];
    typ = f->stdio_typ;

    if (typ && fn && ((fn<12) || ((fn>=16) && (fn<=19)))) /* std funcs */
    {
        h = run->p_uft[typ & 0x7f];
        if (h > 0)
        {   /* handle standard device functions redirected to a file */
            switch(fn)
            {
            case 6:                 /* Crawio() */
                if (pw[1] != 0xFF)
                    goto rawout;
                FALLTHROUGH;
            case 1:                 /* Cconin() */
            case 3:                 /* Cauxin() */
            case 7:                 /* Crawcin() */
            case 8:                 /* Cnecin() */
                /*
                 * if an error occurs when reading the file, we handle it
                 * the same way as MiNT (standard TOS returns garbage here)
                 */
                if (xread(h,1L,&ctmp) != 1L)
                    return EOF_INDICATOR;
                return ctmp;

            case 2:                 /* Cconout */
            case 4:                 /* Cauxout() */
            case 5:                 /* Cprnout() */
                /*  M01.01.07  */
                /*  write the char in the int at pw[1]  */
            rawout:
                xwrite(h , 1L , ((char*) &pw[1])+1);
                return 0; /* dummy */

            case 9:                 /* Cconws() */
                pb2 = *((char **) &pw[1]);
                xwrite(h,strlen(pb2),pb2);
                return 0; /* dummy */

            case 10:                /* Cconrs() */
                pb2 = *((char **) &pw[1]);
                max = *pb2++;
                p = pb2 + 1;
                for (i = 0; max--; i++, p++)
                {
                    if (xread(h,1L,p) == 1)
                    {
                        if (*p == 0x0d)
                        {       /* eat the lf */
                            xread(h,1L,&ctmp);
                            break;
                        }
                    }
                    else
                        break;
                }
                *pb2 = i;
                return 0;

            case 11:                /* Cconis() */
            case 18:                /* Cauxis() */
                if (eof(h))
                    return 0L;
                FALLTHROUGH;

            case 16:                /* Cconos() */
            case 17:                /* Cprnos() */
            case 19:                /* Cauxos() */
                return -1L;
            }
        }

        typ = 0;
    }

    if (typ & 0x80)
    {
        if (typ == 0x81)
            h = pw[3];
        else
            h = pw[1];

        if (h >= NUMSTD)
            numl = (long) sft[h-NUMSTD].f_ofd;
        else if (h >= 0)
        {
            h = run->p_uft[h];
            if (h > 0)
                numl = (long) sft[h-NUMSTD].f_ofd;
            else
                numl = h;
        }
        else
            numl = h;

        if (!numl)
            return EIHNDL;  /* invalid handle: media change, etc */

        if (numl < 0)
        {       /* prn, aux, con */
                /* -3   -2   -1  */

            num = numl;

            /*  check for valid handle  */ /* M01.01.0528.01 */
            if (num < -3)
                return EIHNDL;

            pb = (char **) &pw[4];

            /* only do things on read and write */

            if (fn == GEMDOS_FREAD)     /* read */
            {
                if (pw[2])              /* disallow HUGE reads      */
                    return 0;

                if (pw[3] == 1)
                {
                    **pb = conin(HXFORM(num));
                    return 1;
                }

                return cgets(HXFORM(num),pw[3],*pb);
            }

            if (fn == GEMDOS_FWRITE)    /* write */
            {
                long n, count = *(long *)&pw[2];

                pb2 = *pb;      /* char * is buffer address */

                for (n = 0; n < count; n++)
                {
                    if (num == H_Console)
                        tabout(HXFORM(num), (unsigned char)*pb2++);
                    else
                    {           /* M01.01.1029.01 */
                        if (Bconout(HXFORM(num), (unsigned char)*pb2++) == 0)
                            return n;
                    }
                }

                return count;
            }

            return 0;
        }
    }


    /*
     * for Fopen(), Fcreate() we check for special names
     */
    rc = 0;
    if ((fn == GEMDOS_FOPEN) || (fn == GEMDOS_FCREATE)) /* open, create */
    {
        const SPECNAME *entry;
        p = *((char **) &pw[1]);
        for (i = 0, entry = specname_table; i < SN_ENTRIES; i++, entry++)
        {
            if (strcmp(p,entry->name) == 0)
            {
                rc = entry->handle;
                break;
            }
        }
    }

    if (!rc)
    {
        switch(f->wparms)
        {
        case 0:
            rc = (*f->fncall.v)();
            break;

        case 1:
            rc = (*f->fncall.w)(pw[1]);
            break;

        case 2:
            rc = (*f->fncall.ww)(pw[1],pw[2]);
            break;

        case 3:
            rc = (*f->fncall.www)(pw[1],pw[2],pw[3]);
            break;

        case 4:
            rc = (*f->fncall.wwww)(pw[1],pw[2],pw[3],pw[4]);
            break;

        case 5:
            rc = (*f->fncall.wwwww)(pw[1],pw[2],pw[3],pw[4],pw[5]);
            break;

        case 7:
            rc = (*f->fncall.wwwwwww)(pw[1],pw[2],pw[3],pw[4],pw[5],pw[6],pw[7]);
            break;

        default:
            rc = EINTRN;    /* Internal error */
        }
    }

    KDEBUG(("BDOS returns: 0x%08lx\n",rc));

    return rc;
}
