
#ifndef GEMASM_H
#define GEMASM_H

extern void psetup(PD *p, void *codevalue);
extern void gotopgm() /*NORETURN*/ ;
extern void dsptch() /*NORETURN*/ ;
extern void switchto(UDA *puda) NORETURN ;

#endif
