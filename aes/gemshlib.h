
#ifndef GEMSHLIB_H
#define GEMSHLIB_H

extern SHELL    sh[];

extern BYTE     sh_apdir[];
extern LONG     ad_scmd;
extern LONG     ad_stail;
extern LONG     ad_ssave;
extern LONG     ad_dta;
extern LONG     ad_path;

extern LONG     ad_pfile;

extern WORD     gl_shgem;


WORD sh_find(LONG pspec);

#endif
