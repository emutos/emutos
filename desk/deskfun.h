

WORD fun_alert(WORD defbut, WORD stnum, WORD pwtemp[]);
void fun_msg(WORD type, WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
void fun_rebld(WNODE *pwin);
WORD fun_mkdir(WNODE *pw_node);
WORD fun_op(WORD op, PNODE *pspath, BYTE *pdest, WORD dulx, WORD duly,
            WORD from_disk, WORD src_ob);
void fun_drag(WORD src_wh, WORD dst_wh, WORD dst_ob, WORD dulx, WORD duly);
void fun_del(WNODE *pdw);
