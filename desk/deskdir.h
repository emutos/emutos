
void show_hide(WORD fmd, LONG tree);
void draw_fld(LONG tree, WORD obj);
BYTE *scan_slsh(BYTE *path);
void add_fname(BYTE *path, BYTE *new_name);
void del_fname(BYTE *pstr);
WORD d_errmsg(void);
WORD d_doop(WORD op, LONG tree, WORD obj, BYTE *psrc_path, BYTE *pdst_path,
            WORD *pfcnt, WORD *pdcnt, WORD flag);
WORD par_chk(BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path);
WORD dir_op(WORD op, BYTE *psrc_path, FNODE *pflist, BYTE *pdst_path,
            WORD *pfcnt, WORD *pdcnt, LONG *psize,
            WORD dulx, WORD duly, WORD from_disk, WORD src_ob);
