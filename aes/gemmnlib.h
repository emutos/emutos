
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


UWORD do_chg(LONG tree, WORD iitem, UWORD chgvalue,
             WORD dochg, WORD dodraw, WORD chkdisabled);
WORD mn_do(WORD *ptitle, WORD *pitem);

void mn_bar(LONG tree, WORD showit, WORD pid);
void mn_clsda(void);
WORD mn_register(WORD pid, LONG pstr);
void mn_unregister(WORD da_id);


#endif
