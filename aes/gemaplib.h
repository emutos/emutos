
#ifndef GEMAPLIB_H
#define GEMAPLIB_H

EXTERN WORD	gl_play;
EXTERN WORD	gl_recd;
EXTERN WORD	gl_rlen;
EXTERN LONG	gl_rbuf;

WORD ap_init(void);
VOID ap_rdwr(WORD code, REG PD *p, WORD length, LONG pbuff);
WORD ap_find(LONG pname);
VOID ap_tplay(REG LONG pbuff, WORD length, WORD scale);
WORD ap_trecd(REG LONG pbuff, REG WORD length);
void ap_exit(void);

#endif
