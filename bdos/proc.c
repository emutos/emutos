/*
 * proc.c - process management routines
 *
 * Copyright (c) 2001 Lineo, Inc. and Authors:
 *
 *  KTB     Karl T. Braun (kral)
 *  MAD     Martin Doering
 *  ACH     ???
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "asm.h"
#include "fs.h"
#include "bios.h"
#include "mem.h"
#include "proc.h"
#include "gemerror.h"
#include "biosbind.h"
#include "string.h"
#include "../bios/kprint.h"
#include "../bios/processor.h"

#define DBGPROC 0

#if DBGPROC
#define D(a) kprintf a
#else
#define D(a)
#endif

/*
 * forward prototypes
 */

static void ixterm( PD *r );
static WORD envsize( char *env );
static void init_pd_fields(PD *p, char *tail, long max, MD *env_md);
static void init_pd_files(PD *p);
static MD *alloc_env(char *v);
static void proc_go(PD *p);

/*
 * global variables
 */

PD      *run;           /* ptr to PD for current process */
WORD    supstk[SUPSIZ]; /* common sup stack for all processes*/

/*
 * internal variables
 */

static jmp_buf bakbuf;         /* longjmp buffer */


/*
 * memory internal routines
 * 
 * These violate the encapsulation of the memory internal structure.
 * Could perhaps better go in the memory part.
 */

static MPB *find_mpb(void *addr);
static void free_all_owned(PD *p, MPB *mpb);
static void set_owner(void *addr, PD *p, MPB *mpb);
static void reserve_block(void *addr, MPB *mpb);

static MPB *find_mpb(void *addr)
{
    if(((long)addr) >= start_stram && ((long)addr) <= end_stram) {
        return &pmd;
    } else if(has_ttram) {
        return &pmdtt;
    } else {
        /* returning NULL would mean check for NULL in all mpb functions */
        return &pmd;
    }
}

/* reserve a block, i.e. remove it from the allocated list */
static void reserve_block(void *addr, MPB *mpb)
{
    MD *m,**q;

    for (m = *(q = &mpb->mp_mal); m ; m = *q) {
        if (m->m_own == run) {
            *q = m->m_link; /* pouf ! like magic */
            xmfreblk(m);
        } else {
            q = &m->m_link;
        }
    }
}

/* free each item in the allocated list, that is owned by 'p' */
static void free_all_owned(PD *p, MPB *mpb)
{
    MD *m, **q;

    for( m = *( q = &mpb->mp_mal ) ; m ; m = *q ) {
        if (m->m_own == p) {
            *q = m->m_link;
            freeit(m,mpb);
        } else {
            q = &m->m_link;
        }
    }
}
    
/* change the memory owner based on the block address */
static void set_owner(void *addr, PD *p, MPB *mpb)
{
    MD *m;
    for( m = mpb->mp_mal ; m ; m = m->m_link ) {
        if(m->m_start == (long)addr) {
            m->m_own = p;
            return;
        }  
    }
}

/**
 * ixterm - terminate a process
 *
 * terminate process with PD 'r'.
 *
 * @r: PD of process to terminate
 */

static void     ixterm( PD *r )
{
    register WORD h;
    register WORD i;

    /* check the standard devices in both file tables  */

    for( i = 0 ; i < NUMSTD ; i++ )
        if( (h = r->p_uft[i]) > 0 )
            xclose(h);

    for (i = 0; i < OPNFILES; i++)
        if (r == sft[i].f_own)
            xclose(i+NUMSTD);


    /* check directory usage  M01.01.07 */

    for( i = 0 ; i < NUMCURDIR ; i++ )
    {
        if( (h = r->p_curdir[i]) != 0 )
            diruse[h]-- ;
    }

    /* free each item in the allocated list, that is owned by 'r' */

    free_all_owned(r, &pmd);
    if(has_ttram) 
        free_all_owned(r, &pmdtt);
}


/*
 * envsize - determine size of env area
 *
 * counts bytes starting at 'env' upto and including the terminating
 * double null.
 */

static  WORD envsize( char *env )
{
    register char       *e ;
    register WORD       cnt ;

    for( e = env, cnt = 0 ; !(*e == '\0' && *(e+1) == '\0') ; ++e, ++cnt )
        ;

    return( cnt + 2 ) ;         /*  count terminating double null  */
}



/** xexec - (p_exec - 0x4b) execute a new process
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
static MD *cur_m;
static MD *cur_env_md;

long xexec(WORD flag, char *path, char *tail, char *env)
{
    PD *p;
    PGMHDR01 hdr;
    MD *m, *env_md;
    LONG rc;
    long max, needed;
    FH fh;

    D(("BDOS: xexec - flag or mode = %d\n", flag));

    /* first branch - actions that do not require loading files */
    switch(flag) {
    case PE_BASEPAGE:        
        /* just create a basepage */
        env_md = alloc_env(env);
        if(env_md == NULL) {
            D(("xexec: Not Enough Memory!\n"));
            return(ENSMEM);
        }
        max = (long) ffit(-1L, &pmd); 
        if(max >= sizeof(PD)) {
            m = ffit(max, &pmd);
            p = (PD *) m->m_start;
        } else {
            /* not even enough memory for basepage */
            freeit(env_md, &pmd);
            D(("xexec: No memory for TPA\n"));
            return(ENSMEM);
        }
        /* memory ownership */
        m->m_own = env_md->m_own = run;

        /* initialize the PD */
        init_pd_fields(p, tail, max, env_md);
        init_pd_files(p);

        return (long) p;
    case PE_GOTHENFREE:
        /* set the owner of the memory to be this process */
        p = (PD *) tail;
        set_owner(p, p, find_mpb(p));
        set_owner(p->p_env, p, find_mpb(p->p_env));
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
    D(("BDOS: xexec - trying to find the command ...\n"));
    if (ixsfirst(path,0,0L)) {
        D(("BDOS: Command %s not found!!!\n", path));
        return(EFILNF);     /*  file not found      */
    }

    /* load the header - if IO error occurs now, the longjmp in rwabs will
     * jump directly back to bdosmain.c, which is not a problem because
     * we haven't allocated anything yet.
     */
    rc = kpgmhdrld(path, &hdr, &fh);
    if(rc) {
        D(("BDOS: xexec - kpgmhdrld returned %ld (0x%lx)\n", rc, rc));
        return(rc);
    }

    /* allocate the environment first, always in ST RAM */
    env_md = alloc_env(env);
    if ( env_md == NULL ) {
        D(("xexec: Not Enough Memory!\n"));
        return(ENSMEM);
    }
    
    /* allocate the basepage depending on memory policy */
    needed = hdr.h01_tlen + hdr.h01_dlen + hdr.h01_blen + sizeof(PD);
    max = 0;
        
    /* first try */
    p = NULL;
    m = NULL;
    if(has_ttram && (hdr.h01_flags & PF_TTRAMLOAD)) {
        /* use ttram preferably */
        max = (long) ffit(-1L, &pmdtt); 
        if(max >= needed) {
            m = ffit(max, &pmdtt);
            p = (PD *) m->m_start;
        } 
    }
    /* second try */
    if(p == NULL) {
        max = (long) ffit(-1L, &pmd); 
        if(max >= needed) {
            m = ffit(max, &pmd);
            p = (PD *) m->m_start;
        } 
    }
    /* still failed? free env_md and return */
    if(p == NULL) {
        D(("xexec: No memory for TPA\n"));
        freeit(env_md, &pmd);
        return(ENSMEM);
    }
    assert(m != NULL);

    /* memory ownership - the owner is either the new process being created,
     * or the parent 
     */
    if(flag == PE_LOADGO) {
        m->m_own = env_md->m_own = p;
    } else {
        m->m_own = env_md->m_own = run;
    }   

    /* initialize the fields in the PD structure */
    init_pd_fields(p, tail, max, env_md);
    
    /* set the flags (must be done after init_pd) */
    p->p_flags = hdr.h01_flags;

    /* use static variable to avoid the obscure longjmp warning */
    cur_p = p;
    cur_m = m;
    cur_env_md = env_md;

    /* we have now allocated memory, so we need to intercept longjmp. */
    memcpy(bakbuf, errbuf, sizeof(errbuf));
    if ( setjmp(errbuf) ) {

        kprintf("Error and longjmp in xexec()!");

        /* free any memory allocated yet */
        freeit(cur_env_md, &pmd);
        freeit(cur_m, find_mpb((void *)cur_m->m_start));
        
        /* we still have to jump back to bdosmain.c so that the proper error
         * handling can occur.
         */
        longjmp(bakbuf, 1);
    }

    /* now, load the rest of the program and perform relocation */
    rc = kpgmld(cur_p, fh, &hdr);
    if ( rc ) {
        D(("BDOS: xexec - kpgmld returned %ld (0x%lx)\n", rc, rc));
        /* free any memory allocated yet */
        freeit(cur_env_md, &pmd);
        freeit(cur_m, find_mpb((void *)cur_m->m_start));
    
        return rc;
    }

    /* at this point the program has been correctly loaded in memory, and 
     * more IO errors cannot occur, so it is safe now to finish initializing
     * the new process.
     */
    init_pd_files(cur_p);
    
    /* invalidate instruction cache for the TEXT segment only
     * programs that jump into their DATA, BSS or HEAP are kindly invited 
     * to do their cache management themselves.
     */
    invalidate_icache(((char *)cur_p) + sizeof(PD), hdr.h01_tlen);

    if(flag != PE_LOAD)
        proc_go(cur_p);
    return (long) cur_p;
}

/* initialize the structure fields */
static void init_pd_fields(PD *p, char *tail, long max, MD *env_md)
{
    int i;
    char *b;
    
    /* first, zero it out */
    bzero(p, sizeof(PD)) ;

    /* memory values */
    p->p_lowtpa = (long) p;                /*  M01.01.06   */
    p->p_hitpa  = (long) p  +  max;        /*  M01.01.06   */
    p->p_xdta = &p->p_cmdlin[0];   /* default p_xdta is p_cmdlin */
    p->p_env = (char *) env_md->m_start;

    /* copy tail */
    b = &p->p_cmdlin[0];
    for( i = 0 ; (i < PDCLSIZE)  && (*tail) ; i++ )
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
        if ( h > 0 )
            ixforce(i, h, p);
        else
            p->p_uft[i] = h;
    }

    /* and current directory set */
    for (i = 0; i < 16; i++)
        ixdirdup(i,run->p_curdir[i],p);

    /* and current drive */
    p->p_curdrv = run->p_curdrv;
}

/* allocate the environment, always in ST RAM */
static MD *alloc_env(char *env)
{
    MD *env_md;
    int size;

    /* determine the env size */
    if (env == NULL)
        env = run->p_env;
    size = (envsize(env) + 1) & ~1;  /* must be even */
 
    /* allocate it */
    env_md = ffit((long) size, &pmd);
    if ( env_md == NULL ) {
        return NULL;
    }

    /* copy it */
    memcpy((void *)(env_md->m_start), env, size);
    
    return env_md;
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
 *   bne     retsys          // a6 is (user-supplied) system stack
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

    D(("BDOS: xexec - trying to load (and execute) a command ...\n"));
    p->p_parent = run;
        
    /* create a stack at the end of the TPA */
    sp = (struct gouser_stack *) (p->p_hitpa - sizeof(struct gouser_stack));
    
    sp->basepage = p;      /* the stack contains the basepage */
       
    sp->retaddr = p->p_tbase;    /* return address a3 is text start */
    sp->sr = 0;                  /* the process will start in user mode */
    
    /* the other stack is the supervisor stack */
    sp->other_sp = (long) &supstk[SUPSIZ];
    
    /* store this new stack in the saved a7 field of the PD */
    p->p_areg[7-3] = (long) sp;
    
#if 0
    /* the following settings are not documented, and hence theoretically 
     * the assignments below are not necessary.
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
    run = (PD *) p;

    gouser();
}



/*
 * x0term - (p_term0 - 0x00)Terminate Current Process
 *
 * terminates the calling process and returns to the parent process
 * without a return code
 */

void    x0term(void)
{
    xterm(0);
}

/*
 * xterm - terminate a process
 *
 * terminate the current process and transfer control to the colling
 * process.  All files opened by the terminating process are closed.
 *
 * Function 0x4C        p_term
 */

void    xterm(UWORD rc)
{
    PD *p = run;

    (* (WORD(*)()) Setexc(0x102, (long)-1L))(); /*  call user term handler */

    run = run->p_parent;
    ixterm(p);
    run->p_dreg[0] = rc;
    gouser();
}


/*      
 * xtermres - Function 0x31   p_termres
 */

WORD    xtermres(long blkln, WORD rc)
{
    xsetblk(0,run,blkln);

    reserve_block(run, find_mpb(run));

    xterm(rc);
}

