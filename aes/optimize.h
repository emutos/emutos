
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
void fs_sget(LONG tree, WORD obj, LONG pstr);
WORD inf_what(LONG tree, WORD ok, WORD cncl);
void merge_str(BYTE *pdst, BYTE *ptmp, UWORD parms[]);
WORD wildcmp(BYTE *pwld, BYTE *ptst);
void ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len);
BYTE *op_gname(WORD index);

#endif
