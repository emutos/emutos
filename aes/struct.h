/*	STRUCT.H	1/28/84 - 01/18/85	Lee Jay Lorenzen	*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1986			Digital Research Inc.
*	-------------------------------------------------------------
*/

#define PD	struct pd		/* process descriptor		*/
#define UDA	struct uda		/* user stack data area		*/
#define CDA	struct cdastr		/* console data area structure	*/
#define QPB	struct qpb		/* queue parameter block	*/
#define EVB 	struct evb		/* event block 			*/
#define CQUEUE	struct cqueue		/* console kbd queue		*/
#define MFORM	struct mform		/* mouse form			*/
#define SPB	struct spb		/* sync parameter block 	*/
#define FPD	struct fpd		/* fork process descriptor 	*/

typedef UWORD	EVSPEC;

#define NUM_PDS (NUM_ACCS + 2)		/* acc's + ctrlpd + dos appl.	*/
#define NUM_IEVBS (2 * 5)		/* 5 * the number of internalPDs*/
#define NUM_EEVBS (NUM_ACCS * 5)	/* 5 * the number of externalPDs*/
#define KBD_SIZE 8
#define QUEUE_SIZE 128
#define STACK_SIZE 448
#define NFORKS 32

CQUEUE
{
	WORD	c_buff[KBD_SIZE];
	WORD	c_front;
	WORD	c_rear;
	WORD	c_cnt;
};


MFORM
{
	WORD	mf_xhot;
	WORD	mf_yhot;
	WORD	mf_nplanes;
	WORD	mf_fg;
	WORD	mf_bg;
	WORD	mf_mask[16];
	WORD	mf_data[16];
} ;


#define C_KOWNER 0x0001
#define C_MOWNER 0x0002

CDA
{
	UWORD	c_flags;
	EVB	*c_iiowait;	/* Waiting for Input		*/
	EVB	*c_msleep;	/* wait for mouse rect		*/
	EVB	*c_bsleep;	/* wait for button		*/
	CQUEUE	c_q;		/* input queue 			*/
};


#if I8086
UDA
{
	WORD	u_insuper;		/* in supervisor flag		*/ 
	WORD	*u_spsuper;		/* supervisor stack offset	*/
	UWORD	u_sssuper;		/* supervisor stack segment	*/
	WORD	*u_spuser;		/* user stack offset		*/
	UWORD	u_ssuser;		/* user stack segment		*/
	UWORD	u_regs[9];		/* ds,es,ax,bx,cx,dx,si,di,bp	*/
	WORD	u_super[STACK_SIZE];
	WORD	u_supstk;
} ;
#endif
#if MC68K
UDA
{
	WORD	u_insuper;		/* in supervisor flag		*/ 
	ULONG	u_regs[15];		/* d0-d7, a0-a6			*/
	ULONG	*u_spsuper;		/* supervisor stack 		*/
	ULONG	*u_spuser;		/* user stack 			*/
	ULONG	u_super[STACK_SIZE];
	ULONG	u_supstk;
} ;
#endif


#define NOCANCEL 0x0001		/* event is occuring */
#define COMPLETE 0x0002		/* event completed */
#define EVDELAY  0x0004		/* event is delay event */
#define EVMOUT   0x0008		/* event flag for mouse wait outside of rect*/

EVB		/* event block structure */
{
	EVB	*e_nextp;	/* link to next event on PD event list */
	EVB	*e_link;	/* link to next block on event chain */
	EVB	*e_pred;	/* link to prev block on event chain */
	BYTE	*e_pd;		/* owner PD (data for fork) */
	LONG	e_parm;		/* parm for request -> event comm */
	WORD	e_flag;
	EVSPEC	e_mask;		/* mask for event notification */
	LONG	e_return;
} ;

/* pd defines */
/* p_stat */
#define		WAITIN		0x0001
#define		SWITCHIN	0x8000

PD 
{
	PD	*p_link;
	PD	*p_thread;
	UDA	*p_uda;

	BYTE	p_name[8];

	CDA	*p_cda;		/* cio data area 	*/
	LONG	p_ldaddr;	/* long addr. of load	*/
	WORD 	p_pid;
	WORD	p_stat;

	EVSPEC	p_evbits;	/* event bits in use 	*/
	EVSPEC	p_evwait;	/* event wait mask 	*/
	EVSPEC	p_evflg;	/* event flags 		*/

	EVB	*p_evlist;
	EVB	*p_qdq;
	EVB	*p_qnq;
	LONG	p_qaddr;
	WORD	p_qindex;	
	BYTE	p_queue[QUEUE_SIZE];
} ;



QPB
{
	PD	*qpb_ppd;
	WORD	qpb_cnt;
	LONG	qpb_buf;
} ;

SPB
{
	WORD	sy_tas;
	PD	*sy_owner;
	EVB	*sy_wait;
} ;

FPD
{
	WORD	(*f_code)();
	LONG	f_data;
} ;


