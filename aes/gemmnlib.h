
#ifndef GEMMNLIB_H
#define GEMMNLIB_H


extern LONG     gl_mntree;
extern PD       *gl_mnppd;

extern LONG     desk_acc[];
extern PD       *desk_ppd[];
extern LONG     menu_tree[];

extern BYTE     menu_name[];

extern BYTE     *desk_str[];

extern WORD     gl_dacnt;
extern WORD     gl_dabox;
extern LONG     gl_datree;

extern OBJECT   M_DESK[];


void mn_clsda(void);
UWORD do_chg(LONG tree, WORD iitem, UWORD chgvalue,
             WORD dochg, WORD dodraw, WORD chkdisabled);
WORD mn_do(WORD *ptitle, WORD *pitem);


#endif
