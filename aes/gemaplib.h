
#ifndef GEMAPLIB_H
#define GEMAPLIB_H

extern WORD     gl_play;
extern WORD     gl_recd;
extern WORD     gl_rlen;
extern LONG     gl_rbuf;

WORD ap_init(void);
void ap_rdwr(WORD code, REG PD *p, WORD length, LONG pbuff);
WORD ap_find(LONG pname);
void ap_tplay(REG LONG pbuff, WORD length, WORD scale);
WORD ap_trecd(REG LONG pbuff, REG WORD length);
void ap_exit(void);

#endif
