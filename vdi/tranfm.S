/*
 * tranfm.S - transform raster area from standard to device-specific format
 *
 * Copyright (c) 1991 L. Pruessner, S. Behne, W. Behne
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#if 1

/* Global Declarations */

        .globl  _vr_trnfm       		// entry point

/* Global references */

        .xdef  _CONTRL         			// CONTRL array address

/* MFDB address offsets in CONTRL array */

#define 	s_addr	        0x0e            // pointer to source MFDB
#define 	d_addr	        0x12            // pointer to destination MFDB

/* Memory form definition block offsets */

#define 	fd_h		0x06            // form height in pixels
#define 	fd_wdwidth	0x08            // form width in words
#define 	fd_stand	0x0a            // form format flag
#define 	fd_nplanes	0x0c            // number of memory planes

        .text



/*
 * vr_trnfm - Transform form
 *
 * This function transforms a raster area from standard format to
 * device-specific format or from device-specific to standard format.
 * The operation is a toggle, changing the current state.
 *
 * The number of planes specified in the source MFDB determines the
 * number of planes transformed. The source format flag is toggled and
 * placed in the destination. The user is required to ensure that the
 * other parameters in the destination MFDB are correct.
 *
 *
 * C BINDING
 *
 * Procedure Name  vr_trnfm( handle, psrcMFDB, pdesMFDB )
 *
 * Data Types      WORD vr_trnfm()
 *                 WORD handle
 *                 WORD *psrcMFDB
 *                 WORD *pdesMFDB
 *
 * Input Arguments handle = contrl[6]
 *                 psrcMFDB = contrl[7-8]
 *                 pdesMFDB = contrl[9-10]
 * Registers used  -  D0/d7, a0/a2
 */

_vr_trnfm:
	movem.l	d0-d4/d7-a2, -(sp)    

/* Get source and destination MFDB pointers from the user CONTROL array */

        movea.l _CONTRL,a2              // get CONTROL array address
        move.l  s_addr(a2),d0           // get source MFDB address
        bmi     exit                    // for an invalid address
        move.l  #0xfffffffe,d1          // get mask 
        and.l   d1,d0                   // force source ptr. even word alignment
        movea.l d0,a0                   // source MFDB pointer
        move.l  d_addr(a2),d0           // get source MFDB address
        bmi     exit                    // for an invalid address
        and.l   d1,d0                   // force dest. ptr. even word alignment
        movea.l d0,a1                   // destination MFDB pointer

	moveq	#0, d0			// clear long
	move.w	fd_nplanes(a0), d0	// source plane count
	move.w	fd_h(a0), d1		// source line count
	mulu	fd_wdwidth(a0), d1	// source word count per plane
	tst.w	fd_stand(a0)		// source device dependant?
	bne.s	s_physcl_dev            // yes, jump...
	move.w	#1, fd_stand(a1)	// set standard format flag
	bra.s	addr

/* Source is device dependent( physical device ). */

s_physcl_dev:
	clr.w	fd_stand(a1)		// source is device dependant!
	exg	d0, d1			// exchanges planes and word count

addr:
	movea.l	(a0), a0		// source block address
	movea.l	(a1), a1		// destination block address
	subq.l	#1, d0			// at least one plane/word?
	bmi  	exit
	move.l	d1, d4
	subq.l	#1, d4			// word/plane counter
	bmi.s	exit
	cmpa.l	a0, a1			// source and dest address same?
	beq.s	in_place

	add.l	d1, d1			// distance to next word/plane

/* source and destination address are different */

out_of_place:
	movea.l	a1, a2
	move.l	d0, d3
d_copy:
	move.w	(a0)+, (a2)
	adda.l	d1, a2			// next dest word
	subq.l	#1, d3
	bpl.s	d_copy
	addq.l	#2, a1			// next plane
	subq.l	#1, d4
	bpl.s	out_of_place
	rts

/* source and destination address are the same */

in_place:
	subq.l	#1, d4			// just one word/plane?
	bmi.s	exit

s_loop:
	moveq	#0, D2
	move.l	d4, d1			// word/plane counter

bls:
	adda.l	d0, a0
	lea	2(a0, d0.l), a0
	move.w	(a0), D5
	movea.l	a0, a1
	movea.l	a0, a2
	add.l	d0, D2
	move.l	D2, d3
	bra.s	s_next

s_copy:
	movea.l	a1, a2
	move.l	-(a1), (a2)             // shift all by one word

s_next:
	subq.l	#1, d3
	bpl.s	s_copy
	move.w	D5, (a1)

dbfs:
	subq.l	#1, d1
	bpl.s	bls
	movea.l	a2, a0
	subq.l	#1, d0
	bpl.s	s_loop

exit:
	movem.l	(sp)+, d0-d4/d7-a2
	rts

        .end

#endif
