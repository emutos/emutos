
WORD dr_fnode(UWORD last_state, UWORD curr_state, WORD x, WORD y,
              WORD w, WORD h, LONG psfcb);
WORD dr_code(LONG pparms);
WORD inf_show(LONG tree, WORD start);
WORD inf_file(BYTE *ppath, FNODE *pfnode);
WORD inf_folder(BYTE *ppath, FNODE *pf);
WORD inf_disk(BYTE dr_id);
WORD inf_pref(void);
WORD opn_appl(BYTE *papname, BYTE *papparms, BYTE *pcmd, BYTE *ptail);
