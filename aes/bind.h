/*  BIND.H   */

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

extern	VOID	v_opnwk();
extern	WORD	v_clswk();
extern	WORD	v_clrwk();
extern	WORD	v_clswk();
extern	WORD	vq_chcells();
extern	WORD	v_exit_cur();
extern	WORD	v_enter_cur();
extern	WORD	v_curup();
extern	WORD	v_curdown();
extern	WORD	v_curright();
extern	WORD	v_curleft();
extern	WORD	v_curhome();
extern	WORD	v_eeos();
extern	WORD	v_eeol();
extern	WORD	vs_curaddress();
extern	WORD	v_curtext();
extern	WORD	v_rvon();
extern	WORD	v_rvoff();
extern	WORD	vq_curaddress();
extern	WORD	vq_tabstatus();
extern	WORD	v_hardcopy ();
extern	WORD	v_dspcur();
extern	WORD	v_rmcur ();
extern	VOID	v_pline();
extern	WORD	v_pmarker();
extern	WORD	v_gtext();
extern	WORD	v_fillarea();

extern	WORD	v_bar();
extern	WORD	v_circle();
extern	WORD	v_arc();
extern	WORD	v_pieslice();
extern	WORD	v_ellipse();
extern	WORD	v_ellarc();
extern	WORD	v_ellpie();

extern	VOID	vst_height();
extern	WORD	vst_rotation();
extern	WORD	vs_color();
extern	WORD	vsl_type();
extern	VOID    vsl_width();
extern	WORD	vsl_color();
extern	WORD	vsm_type();
extern	WORD	vsm_height();
extern	WORD	vsm_color();
extern	WORD	vst_font();
extern	WORD	vst_color();
extern	WORD	vsf_interior();
extern	WORD	vsf_style();
extern	WORD	vsf_color();
extern	WORD	vq_color();

extern	WORD	vrq_locator();
extern	WORD	vsm_locator();
extern	WORD	vrq_valuator();
extern	WORD	vsm_valuator();
extern	WORD	vrq_choice();
extern	WORD	vsm_choice();
extern	WORD	vrq_string();
extern	WORD	vsm_string();
extern	WORD	vswr_mode();
extern	WORD	vsin_mode();

extern	WORD	vsf_perimeter();

extern	WORD	vr_cpyfm();
extern	WORD	vr_trnfm();
extern	WORD	vsc_form();
extern	WORD	vsf_udpat();
extern	WORD	vsl_udsty();
extern	VOID	vr_recfl();
extern	WORD	v_show_c();
extern	WORD	v_hide_c();
extern	WORD	vq_mouse();
extern	WORD	vex_butv();
extern	WORD	vex_motv();
extern	WORD	vex_curv();
extern	WORD	vq_key_s();			

