
#ifndef OPTIMIZE_H
#define OPTIMIZE_H

WORD sound(WORD isfreq, WORD freq, WORD dura);
void rc_constrain(GRECT *pc, GRECT *pt);
void rc_union(GRECT *p1, GRECT *p2);
WORD rc_intersect(GRECT *p1, GRECT *p2);
BYTE *strscn(BYTE *ps, BYTE *pd, BYTE stop);
void fmt_str(BYTE *instr, BYTE *outstr);
void unfmt_str(BYTE *instr, BYTE *outstr);
void fs_sset(LONG tree, WORD obj, LONG pstr, LONG *ptext, WORD *ptxtlen);
void inf_sset(LONG tree, WORD obj, BYTE *pstr);
void fs_sget(LONG tree, WORD obj, LONG pstr);
void inf_sget(LONG tree, WORD obj, BYTE *pstr);
void inf_fldset(LONG tree, WORD obj, UWORD testfld, UWORD testbit,
                UWORD truestate, UWORD falsestate);
WORD inf_gindex(LONG tree, WORD baseobj, WORD numobj);
WORD inf_what(LONG tree, WORD ok, WORD cncl);
WORD wildcmp(BYTE *pwld, BYTE *ptst);
void ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len);

#endif
