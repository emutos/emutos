/*
 * proc.c - process management routines
 *
 * Copyright (C) 2001 Lineo, Inc. and Authors:
 *               2002-2017 The EmuTOS development team
 *
 *  KTB     Karl T. Braun (kral)
 *  MAD     Martin Doering
 *  ACH     ???
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "fs.h"
#include "mem.h"
#include "proc.h"
#include "gemerror.h"
#include "biosbind.h"
#include "string.h"
#include "kprint.h"
#include "biosext.h"
#include "asm.h"
#include "../bios/tosvars.h"


/*
 * defines
 */
#define TPASIZE_QUANTUM (128*1024L)     /* see alloc_tpa() */

/*
 * forward prototypes
 */

static void ixterm( PD *r );
static WORD envsize( char *env );
static void init_pd_fields(PD *p, char *tail, long max, char *envptr);
static void init_pd_files(PD *p);
static char *alloc_env(ULONG flags, char *v);
static UBYTE *alloc_tpa(ULONG flags,LONG needed,LONG *avail);
static void proc_go(PD *p);

/*
 * global variables
 */

PD      *run;           /* ptr to PD for current process */

/*
 * internal variables
 */

static WORD    supstk[SUPSIZ]; /* common sup stack for all processes */
static jmp_buf bakbuf;         /* longjmp buffer */


/*
 * memory internal routines
 *
 * These violate the encapsulation of the memory internal structure.
 * Could perhaps better go in the memory part.
 */
static void free_all_owned(PD *p, MPB *mpb);
static void reserve_blocks(PD *pd, MPB *mpb);

/* reserve blocks, i.e. remove them from the allocated list
 *
 * the memory associated with these blocks will remain permanently
 * allocated - this is used by Ptermres()
 */
static void reserve_blocks(PD *p, MPB *mpb)
{
    MD *m, **q;

    for (m = *(q = &mpb->mp_mal); m; m = *q) {
        if (m->m_own == p) {
            *q = m->m_link; /* pouf ! like magic */
            xmfremd(m);
        } else {
            q = &m->m_link;
        }
    }
}

/* free each item in the allocated list, that is owned by 'p' */
static void free_all_owned(PD *p, MPB *mpb)
{
    MD *m, *next;

    for (m = mpb->mp_mal; m; m = next) {
        next = m->m_link;
        if (m->m_own == p)
            freeit(m,mpb);
    }
}

/*
 * ixterm - terminate a process
 *
 * terminate process with PD 'r'.
 *
 * @r: PD of process to terminate
 */
static void ixterm(PD *r)
{
    WORD h;
    WORD i;

    /* call process termination vector (last chance for user cleanup) */

    etv_term();

    /* check the standard devices in both file tables  */

    for (i = 0; i < NUMSTD; i++)
        if ((h = r->p_uft[i]) > 0)
            xclose(h);

    for (i = 0; i < OPNFILES; i++)
        if (r == sft[i].f_own)
            xclose(i+NUMSTD);


    /* decrement usage counts for current directories */

    for (i = 0; i < NUMCURDIR; i++)
    {
        if ((h = r->p_curdir[i]) != 0)
            decr_curdir_usage(h);
    }

    /* free each item in the allocated list that is owned by 'r' */

    free_all_owned(r, &pmd);
#if CONF_WITH_ALT_RAM
    if(has_alt_ram)
        free_all_owned(r, &pmdalt);
#endif
}


/*
 * envsize - determine size of env area
 *
 * counts bytes starting at 'env' up to and including the terminating
 * double null.
 */
static WORD envsize(char *env)
{
    char *e;
    WORD cnt;

    for (e = env, cnt = 0; !(*e == '\0' && *(e+1) == '\0'); ++e, ++cnt)
        ;

    return cnt + 2;         /*  count terminating double null  */
}


/*
 * xexec - (Pexec - 0x4b) execute a new process
 *
 * load&go(cmdlin,cmdtail), load/nogo(cmdlin,cmdtail), justgo(psp)
 * create psp - user receives a memory partition
 *
 * @flg: 0: load&go, 3: load/nogo, 4: justgo, 5: create psp, 6: ???
 * @s:   command
 * @t:   tail
 * @v:   environment
 */

/* these variables are used to avoid the following warning:
 * variable `foo' might be clobbered by `longjmp' or `vfork'
 */
static PD *cur_p;

long xexec(WORD flag, char *path, char *tail, char *env)
{
    PD *p, *owner;
    PGMHDR01 hdr;
    char *env_ptr;
    ULONG hdrflags;
    LONG rc;
    long max, needed;
    FH fh;

    KDEBUG(("BDOS xexec: flag or mode = %d\n",flag));

    /* first branch - actions that do not require loading files */
    switch(flag) {
#if DETECT_NATIVE_FEATURES
    case PE_RELOCATE:   /* internal use only, see bootstrap() in bios/bios.c */
        p = (PD *) tail;
        rc = kpgm_relocate(p, (long)path);
        if (rc) {
            KDEBUG(("BDOS xexec: kpgm_reloc returned %ld (0x%lx)\n",rc,rc));
            return rc;
        }

        /* invalidate instruction cache for the TEXT segment only
         * programs that jump into their DATA, BSS or HEAP are kindly invited
         * to do their cache management themselves.
         */
        invalidate_instruction_cache( p+1, p->p_tlen);

        return (long)p;
#endif
    case PE_BASEPAGE:           /* just create a basepage */
        path = (char *) 0L;     /* (same as basepage+flags with flags set to zero) */
        /* drop thru */
    case PE_BASEPAGEFLAGS:      /* create a basepage, respecting the flags */
        hdrflags = (ULONG)path;
        env_ptr = alloc_env(hdrflags, env);
        if (env_ptr == NULL) {
            KDEBUG(("BDOS xexec: no memory for environment\n"));
            return ENSMEM;
        }
        p = (PD *)alloc_tpa((ULONG)path,sizeof(PD),&max);

        if (p == NULL) {    /* not even enough memory for basepage */
            xmfree(env_ptr);
            KDEBUG(("BDOS xexec: No memory for basepage\n"));
            return ENSMEM;
        }

        /* memory ownership */
        set_owner(p, run);
        set_owner(env_ptr, run);

        /* initialize the PD */
        init_pd_fields(p, tail, max, env_ptr);
        p->p_flags = (ULONG)path;   /* set the flags */
        init_pd_files(p);

        return (long)p;
    case PE_GOTHENFREE:
        /* set the owner of the memory to be this process */
        p = (PD *) tail;
        set_owner(p, p);
        set_owner(p->p_env, p);
        /* fall through */
    case PE_GO:
        p = (PD *) tail;
        proc_go(p);
        /* should not return ? */
        return (long)p;
    case PE_LOADGO:
    case PE_LOAD:
        break;
    default:
        return EINVFN;
    }

    /* we now need to load a file */
    KDEBUG(("BDOS xexec: trying to find %s\n",path));
    if (ixsfirst(path,0,0L)) {
        KDEBUG(("BDOS xexec: command %s not found!!!\n",path));
        return EFILNF;      /*  file not found      */
    }

    /* load the header - if I/O error occurs now, the longjmp in rwabs will
     * jump directly back to bdosmain.c, which is not a problem because
     * we haven't allocated anything yet.
     */
    rc = kpgmhdrld(path, &hdr, &fh);
    if (rc) {
        KDEBUG(("BDOS xexec: kpgmhdrld returned %ld (0x%lx)\n",rc,rc));
        return rc;
    }

    /* allocate the environment first, depending on memory policy */
    env_ptr = alloc_env(hdr.h01_flags, env);
    if (env_ptr == NULL) {
        KDEBUG(("BDOS xexec: no memory for environment\n"));
        return ENSMEM;
    }

    /* allocate the basepage depending on memory policy */
    needed = hdr.h01_tlen + hdr.h01_dlen + hdr.h01_blen + sizeof(PD);
    p = (PD *)alloc_tpa(hdr.h01_flags,needed,&max);

    /* if failed, free env_ptr and return */
    if (p == NULL) {
        KDEBUG(("BDOS xexec: no memory for TPA\n"));
        xmfree(env_ptr);
        return ENSMEM;
    }

    /* memory ownership - the owner is either the new process being created,
     * or the parent
     */
    owner = (flag == PE_LOADGO) ? p : run;
    set_owner(p, owner);
    set_owner(env_ptr, owner);

    /* initialize the fields in the PD structure */
    init_pd_fields(p, tail, max, env_ptr);

    /* set the flags (must be done after init_pd) */
    p->p_flags = hdr.h01_flags;

    /* use static variable to avoid the obscure longjmp warning */
    cur_p = p;

    /* we have now allocated memory, so we need to intercept longjmp. */
    memcpy(bakbuf, errbuf, sizeof(errbuf));
    if (setjmp(errbuf)) {

        KDEBUG(("Error and longjmp in xexec()!\n"));

        /* free any memory allocated yet */
        xmfree(cur_p->p_env);
        xmfree(cur_p);

        /* we still have to jump back to bdosmain.c so that the proper error
         * handling can occur.
         */
        longjmp(bakbuf, 1);
    }

    /* now, load the rest of the program and perform relocation */
    rc = kpgmld(cur_p, fh, &hdr);
    if (rc) {
        KDEBUG(("BDOS xexec: kpgmld returned %ld (0x%lx)\n",rc,rc));
        /* free any memory allocated yet */
        xmfree(cur_p->p_env);
        xmfree(cur_p);

        return rc;
    }

    /* at this point the program has been correctly loaded in memory, and
     * more I/O errors cannot occur, so it is safe now to finish initializing
     * the new process.
     */
    init_pd_files(cur_p);

    /* invalidate instruction cache for the TEXT segment only
     * programs that jump into their DATA, BSS or HEAP are kindly invited
     * to do their cache management themselves.
     */
    invalidate_instruction_cache(((char *)cur_p) + sizeof(PD), hdr.h01_tlen);

    if (flag != PE_LOAD)
        proc_go(cur_p);
    return (long)cur_p;
}

/* initialize the structure fields */
static void init_pd_fields(PD *p, char *tail, long max, char *envptr)
{
    int i;
    char *b;

    /* first, zero it out */
    bzero(p, sizeof(PD)) ;

    /* memory values */
    p->p_lowtpa = (long) p;                /*  M01.01.06   */
    p->p_hitpa  = (long) p  +  max;        /*  M01.01.06   */
    p->p_xdta = (DTA *) p->p_cmdlin;       /* default p_xdta is p_cmdlin */
    p->p_env = envptr;

    /* copy tail */
    b = &p->p_cmdlin[0];
    for (i = 0; (i < PDCLSIZE) && (*tail); i++)
        *b++ = *tail++;

    *b++ = 0;
}

/* duplicate files */
static void init_pd_files(PD *p)
{
    int i;

    /* inherit standard files from me */
    for (i = 0; i < NUMSTD; i++) {
        WORD h = run->p_uft[i];
        if (h > 0)
            ixforce(i, h, p);
        else
            p->p_uft[i] = h;
    }

    /* and current directory set */
    for (i = 0; i < NUMCURDIR; i++) {
        int dn = run->p_curdir[i];
        p->p_curdir[i] = dn;
        if (dn)
            dirtbl[dn].use++;
    }

    /* and current drive */
    p->p_curdrv = run->p_curdrv;
}

/* allocate the environment, in ST RAM or alternate RAM, according to the header flags */
static char *alloc_env(ULONG flags, char *env)
{
    char *new_env;
    int size;

    /* determine the env size */
    if (env == NULL)
        env = run->p_env;
    size = (envsize(env) + 1) & ~1;  /* must be even */

    /* allocate it */
    new_env = xmxalloc(size, (flags&PF_TTRAMLOAD) ? MX_PREFTTRAM : MX_STRAM);
    if (new_env)
    {
        memcpy(new_env, env, size);     /* copy it */
    }

    return new_env;
}

/*
 * allocate the TPA
 *
 * we first determine if ST RAM and/or alternate RAM is available for
 * allocation, based on the flags, the amount of RAM required and
 * the presence or absence of TT RAM.
 *
 * if no types are available (the requested amount is too large), we
 * return NULL.
 *
 * if only one type of RAM is available, we allocate it & return a
 * pointer to it.
 *
 * if both types are available, we normally allocate in alternate RAM
 * *except* if the amount of ST RAM is greater than the amount of
 * alternate RAM.  In this case, we use a tiebreaker: bits 31-27
 * of the flags field plus 1 is taken as a 4-bit number, which is
 * multiplied by 128K and added to the base amount needed to get a
 * "would like to have" amount.  If this amount is larger than the
 * amount of alternate RAM, then we allocate in ST RAM.
 *
 * Reference: TT030 TOS Release Notes, Third Edition, 6 September 1991,
 * pages 29-30.
 *
 * returns: ptr to allocated memory (NULL => failed)
 *          updates 'avail' with the size of allocated memory
 */
static UBYTE *alloc_tpa(ULONG flags,LONG needed,LONG *avail)
{
    MD *md;
    LONG st_ram_size;
    BOOL st_ram_available = FALSE;

    st_ram_size = (LONG) ffit(-1L, &pmd);
    if (st_ram_size >= needed)
        st_ram_available = TRUE;

#if CONF_WITH_ALT_RAM
    {
        LONG alt_ram_size = 0L, tpasize;
        BOOL alt_ram_available = FALSE;

        if (has_alt_ram && (flags & PF_TTRAMLOAD)) {
            alt_ram_size = (LONG) ffit(-1L, &pmdalt);
            if (alt_ram_size >= needed)
                alt_ram_available = TRUE;
        }

        if (st_ram_available && alt_ram_available && (st_ram_size > alt_ram_size)) {
            tpasize = (((flags >> 28) & 0x0f) + 1) * TPASIZE_QUANTUM;
            if (needed+tpasize > alt_ram_size)
                alt_ram_available = FALSE;  /* force allocation in ST RAM */
        }

        if (alt_ram_available) {
            *avail = alt_ram_size;
            md = ffit(alt_ram_size, &pmdalt);
            return md->m_start;
        }
    }
#endif

    if (st_ram_available) {
        *avail = st_ram_size;
        md = ffit(st_ram_size, &pmd);
        return md->m_start;
    }

    return NULL;
}

/* proc_go launches the new process by creating the right data
 * structure in memory, then pretending resuming from an ordinary
 * BDOS call by calling gouser().
 *
 * Here is an excerpt of gouser() from rwa.S:
 * _gouser:
 *   move.l  _run,a5
 *   move.l  d0,0x68(a5)
 *   move.l  0x7c(a5),a6     // stack pointer (maybe usp, maybe ssp)
 *   move.l  (a6)+,a4        // other stack pointer
 *   move.w  (a6)+,d0
 *   move.l  (a6)+,a3        // retadd
 *   movem.l (a6)+,d1-d7/a0-a2
 *   btst    #13,d0
 *   jne     retsys          // a6 is (user-supplied) system stack
 *   move.l  a4,sp
 *   move.l  a6,usp
 * gousr:
 *   move.l  a3,-(sp)
 *   move    d0,-(sp)
 *   movem.l 0x68(a5),d0/a3-a6
 *
 */

struct gouser_stack {
  LONG other_sp;   /* a4, the other stack pointer */
  WORD sr;         /* d0, the status register */
  LONG retaddr;    /* a3, the return address */
  LONG fill[11];   /* 10 registers d1-d7/a0-a2 and one dummy so that ... */
  PD * basepage;   /* ... upon startup the basepage is in 4(sp) */
};

static void proc_go(PD *p)
{
    struct gouser_stack *sp;

    KDEBUG(("BDOS xexec: trying to load (and execute) a process on 0x%lx...\n",p->p_tbase));
    p->p_parent = run;

    /* create a stack at the end of the TPA */
    sp = (struct gouser_stack *) (p->p_hitpa - sizeof(struct gouser_stack));

    sp->basepage = p;      /* the stack contains the basepage */

    sp->retaddr = p->p_tbase;    /* return address a3 is text start */
    sp->sr = get_sr() & 0x0700;  /* the process will start in user mode, same IPL */

    /* the other stack is the supervisor stack */
    sp->other_sp = (long) &supstk[SUPSIZ];

    /* store this new stack in the saved a7 field of the PD */
    p->p_areg[7-3] = (long) sp;

#if 1
    /* the following settings are not documented, and hence theoretically
     * the assignments below are not necessary.
     * However, many programs test if A0 = 0 to check if they are running
     * as a normal program or as an accessory, so we need to clear at least
     * this register!
     */
    {   /* d1-d7/a0-a2 and dummy return address set to zero */
        int i;
        for(i = 0; i < 11 ; i++)
            sp->fill[i] = 0;
    }
    p->p_areg[6-3] = (long) sp;    /* a6 to hold a copy of the stack */
    p->p_areg[5-3] = p->p_dbase;   /* a5 to point to the DATA segt */
    p->p_areg[4-3] = p->p_bbase;   /* a4 to point to the BSS segt */
#endif

    /* the new process is the one to run */
    run = (PD *)p;

    gouser();
}



/*
 * x0term - (Pterm0) terminate current process
 *
 * terminates the calling process and returns to the parent process
 * with a return code of zero
 */
void x0term(void)
{
    xterm(0);
}

/*
 * xterm - (Pterm) terminate current process
 *
 * terminate the current process and transfer control to the colling
 * process.  All files opened by the terminating process are closed.
 *
 * Function 0x4C        p_term
 */
void xterm(UWORD rc)
{
    PD *p = run;

    (* (WORD(*)(void)) Setexc(0x102, (long)-1L))(); /*  call user term handler */

    run = run->p_parent;
    ixterm(p);
    /* gouser() will store the current value of D0 in the active PD
     * so it cannot be used here. See proc_go() above.
     * termuser() will enter the gouser() code at the proper place.
     * sep 2005 RCL
     */
    run->p_dreg[0] = rc;
    termuser();
}


/*
 * xtermres - Function 0x31 (Ptermres)
 */
WORD xtermres(long blkln, WORD rc)
{
    xsetblk(0,run,blkln);

    reserve_blocks(run, &pmd);
#if CONF_WITH_ALT_RAM
    if (has_alt_ram)
        reserve_blocks(run, &pmdalt);
#endif
    xterm(rc);
}
