
#ifndef GEMFSLIB_H
#define GEMFSLIB_H

extern BYTE     gl_fsobj[];
extern LONG     ad_fstree;
extern LONG     ad_fsnames;
extern LONG     ad_fsdta;
extern GRECT    gl_rfs;

extern LONG     ad_tmp1;
extern BYTE     gl_tmp1[];
extern LONG     ad_tmp2;
extern BYTE     gl_tmp2[];

extern WORD     gl_shdrive;
extern WORD     gl_fspos;

WORD fs_input(LONG pipath, LONG pisel, WORD *pbutton);


#endif
