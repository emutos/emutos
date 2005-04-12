
#ifndef GEMCTRL_H
#define GEMCTRL_H

extern MOBLK    gl_ctwait;
extern WORD     gl_ctmown;
extern WORD     appl_msg[8];
extern WORD     gl_wa[];

void ct_chgown(PD *mpd, GRECT *pr);
void ct_mouse(WORD grabit);
void ctlmgr(void);

#endif
