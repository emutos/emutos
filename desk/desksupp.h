
void desk_clear(WORD wh);
void desk_verify(WORD wh, WORD changed);
void do_wredraw(WORD w_handle, WORD xc, WORD yc, WORD wc, WORD hc);
ICONBLK  *get_spec(OBJECT olist[], WORD obj);
void do_xyfix(WORD *px, WORD *py);
void do_wopen(WORD new_win, WORD wh, WORD curr, WORD x, WORD y, WORD w, WORD h);
void do_wfull(WORD wh);
WORD do_diropen(WNODE *pw, WORD new_win, WORD curr_icon, WORD drv,
                BYTE *ppath, BYTE *pname, BYTE *pext, GRECT *pt, WORD redraw);
void do_fopen(WNODE *pw, WORD curr, WORD drv, BYTE *ppath, BYTE *pname,
              BYTE *pext, WORD chkall, WORD redraw);
WORD do_open(WORD curr);
WORD do_info(WORD curr);
void do_format(WORD curr);
void do_chkall(WORD redraw);
