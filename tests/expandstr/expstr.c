/*
 * expstr.c - regression test for the AES's unbounded client strings
 *
 * Copyright (C) 2026 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * hands the AES ~4000-character strings via G_STRING, G_BOXTEXT,
 * G_FBOXTEXT and a G_FTEXT objc_edit(), to exercise every place where
 * the AES used to copy a client-supplied string into a fixed-size
 * buffer of its own with no bound, overrunning its globals and
 * ultimately its supervisor stack.
 *
 * no gemlib to link against, so it hand-rolls the minimal AES bindings
 * it needs (trap #2, control/global/int_in/int_out/addr_in/addr_out).
 */

#include <stdio.h>
#include <string.h>

typedef short WORD;
typedef unsigned short UWORD;
typedef long LONG;

typedef struct
{
    WORD  ob_next, ob_head, ob_tail;
    UWORD ob_type, ob_flags, ob_state;
    LONG  ob_spec;
    WORD  ob_x, ob_y, ob_width, ob_height;
} OBJECT;

typedef struct
{
    char *te_ptext, *te_ptmplt, *te_pvalid;
    WORD te_font, te_junk1, te_just, te_color, te_junk2;
    WORD te_thickness, te_txtlen, te_tmplen;
} TEDINFO;

#define G_BOX       20
#define G_BOXTEXT   22
#define G_STRING    28
#define G_FTEXT     29
#define G_FBOXTEXT  30
#define EDITABLE    0x0008
#define LASTOB      0x0020
#define ROOT        0
#define TE_LEFT     0
#define IBM         1

#define EDINIT      1
#define EDCHAR      2

#define KEY_Z       0x2c5a      /* scancode + ASCII, as the AES expects */

#define APPL_INIT   10
#define APPL_EXIT   19
#define OBJC_DRAW   42
#define OBJC_EDIT   46
#define GRAF_HANDLE 77
#define WIND_GET    104
#define WF_WXYWH    4           /* window work area x,y,w,h */

#define STRLEN 4000             /* mirrors the 2838-byte field crash */

/*
 * MAX_LEN, i.e. the size of the AES's D.g_rawstr et al (see aes/gemlib.h).
 * the AES must never hand us back more text than that, however large a
 * te_txtlen we give it.
 */
#define AES_MAX_LEN 81

#define KEYSTROKES  200         /* enough to overrun D.g_rawstr, pre-fix */

static WORD control[5], global[16], int_in[16], int_out[8];
static LONG addr_in[3], addr_out[1];
static void *aespb[6] = { control, global, int_in, int_out, addr_in, addr_out };

static WORD aes(WORD op, WORD nin, WORD nout, WORD nain)
{
    register long d0 __asm__("d0") = 200;
    register void *d1 __asm__("d1") = aespb;

    control[0] = op;
    control[1] = nin;
    control[2] = nout;
    control[3] = nain;
    control[4] = 0;
    __asm__ volatile ("trap #2" : "+r"(d0) : "r"(d1)
                      : "d2", "a0", "a1", "a2", "memory");
    return int_out[0];
}

static char longstr[STRLEN + 1];    /* over-long text     */
static char longtmpl[STRLEN + 1];   /* over-long template */
static char longvalid[STRLEN + 1];  /* over-long validation string */
/*
 * the text of the object we edit.  it is deliberately far larger than
 * the AES's own buffer, so that an unbounded ins_char() writes into our
 * own bss (where we can see it) rather than off the end of it
 */
static char edittext[512] = "AB";
static TEDINFO ted, fted, eted;
static OBJECT tree[5];

/*
 * the AES's state before we hand it anything over-long, so that we can
 * tell afterwards whether an overrun corrupted it even though it did not
 * actually fault
 */
static WORD handle0, wchar0, hchar0, wbox0, hbox0;

static void snapshot_aes(void)
{
    aes(GRAF_HANDLE, 0, 5, 0);
    handle0 = int_out[0];
    wchar0 = int_out[1];
    hchar0 = int_out[2];
    wbox0 = int_out[3];
    hbox0 = int_out[4];
}

/*
 * returns NULL if the AES still looks intact, else a failure message
 */
static const char *check_aes(void)
{
    aes(GRAF_HANDLE, 0, 5, 0);
    if ((int_out[0] != handle0) || (int_out[1] != wchar0) ||
        (int_out[2] != hchar0) || (int_out[3] != wbox0) ||
        (int_out[4] != hbox0))
        return "graf_handle() changed";

    int_in[0] = 0;              /* desktop window handle */
    int_in[1] = WF_WXYWH;
    aes(WIND_GET, 2, 5, 0);
    if (!int_out[0] ||
        (int_out[3] <= 0) || (int_out[4] <= 0) ||
        (int_out[3] > 4096) || (int_out[4] > 4096))
        return "wind_get(WF_WXYWH) implausible";

    return NULL;
}

static void set_tedinfo(TEDINFO *p, char *text, char *tmplt, char *valid,
                        WORD txtlen, WORD tmplen)
{
    p->te_ptext = text;
    p->te_ptmplt = tmplt;
    p->te_pvalid = valid;
    p->te_font = IBM;
    p->te_junk1 = 0;
    p->te_just = TE_LEFT;
    p->te_color = 4352;         /* border/fill/text colour word */
    p->te_junk2 = 0;
    p->te_thickness = 0;
    p->te_txtlen = txtlen;
    p->te_tmplen = tmplen;
}

static void set_object(WORD obj, WORD next, UWORD type, UWORD flags, LONG spec,
                       WORD x, WORD y, WORD w, WORD h)
{
    tree[obj].ob_next = next;
    tree[obj].ob_head = -1;
    tree[obj].ob_tail = -1;
    tree[obj].ob_type = type;
    tree[obj].ob_flags = flags;
    tree[obj].ob_state = 0;
    tree[obj].ob_spec = spec;
    tree[obj].ob_x = x;
    tree[obj].ob_y = y;
    tree[obj].ob_width = w;
    tree[obj].ob_height = h;
}

static void write_result(const char *result)
{
    FILE *fh = fopen("EXPSTR.OUT", "w");

    if (fh)
    {
        fprintf(fh, "%s\n", result);
        fclose(fh);
    }
}

static void fail(const char *what, const char *why)
{
    char msg[128];

    sprintf(msg, "FAIL: %s: %s", what, why);
    write_result(msg);
}

int main(void)
{
    WORD i;
    WORD ap_id;
    WORD idx;
    WORD hchar;
    WORD wx, wy, ww, wh;
    const char *why;
    int rc = 1;

    for (i = 0; i < STRLEN; i++)
    {
        longstr[i] = 'A' + (i % 26);
        longtmpl[i] = '_';
        longvalid[i] = 'X';
    }
    longstr[STRLEN] = longtmpl[STRLEN] = longvalid[STRLEN] = '\0';

    ap_id = aes(APPL_INIT, 0, 1, 0);
    if (ap_id < 0)
    {
        write_result("FAIL: appl_init failed");
        return 1;
    }

    snapshot_aes();
    hchar = hchar0;

    int_in[0] = 0;              /* desktop window handle */
    int_in[1] = WF_WXYWH;
    aes(WIND_GET, 2, 5, 0);
    wx = int_out[1];
    wy = int_out[2];
    ww = int_out[3];
    wh = int_out[4];

    /*
     * build a small tree: a G_BOX root with four children, one per
     * over-long string path listed at the top of this file.
     */
    set_object(0, -1, G_BOX, 0, 0, wx, wy, ww, wh);
    tree[0].ob_head = 1;
    tree[0].ob_tail = 4;

    set_object(1, 2, G_STRING, 0, (LONG)longstr, 0, 0, ww, hchar);

    set_tedinfo(&ted, longstr, "", "", STRLEN, 0);
    set_object(2, 3, G_BOXTEXT, 0, (LONG)&ted, 0, hchar, ww, hchar);

    set_tedinfo(&fted, longstr, longtmpl, "", STRLEN, STRLEN);
    set_object(3, 4, G_FBOXTEXT, 0, (LONG)&fted, 0, 2*hchar, ww, hchar);

    /* text kept short: find_pos() needs one '_' per character, unrelated to what we're testing */
    set_tedinfo(&eted, edittext, longtmpl, longvalid, 3, 2);
    set_object(4, 0, G_FTEXT, LASTOB|EDITABLE, (LONG)&eted, 0, 3*hchar, ww, hchar);

    int_in[0] = ROOT;
    int_in[1] = 3;               /* depth */
    int_in[2] = wx;
    int_in[3] = wy;
    int_in[4] = ww;
    int_in[5] = wh;
    addr_in[0] = (LONG)tree;
    aes(OBJC_DRAW, 6, 1, 1);     /* the expand_string()/ob_format() overruns */

    why = check_aes();
    if (why)
    {
        fail("objc_draw", why);
        goto out;
    }

    /*
     * pass 1: an over-long te_pvalid, which ob_edit() copies into
     * D.g_valstr
     */
    int_in[0] = 4;               /* object */
    int_in[1] = 0;               /* character */
    int_in[2] = 0;               /* index */
    int_in[3] = EDINIT;
    addr_in[0] = (LONG)tree;
    aes(OBJC_EDIT, 4, 2, 1);

    why = check_aes();
    if (why)
    {
        fail("objc_edit with long te_pvalid", why);
        goto out;
    }

    /*
     * pass 2: a short te_pvalid but an over-long te_tmplen, which is how
     * far ob_edit() expands the validation string in D.g_valstr
     */
    eted.te_pvalid = "X";
    eted.te_tmplen = STRLEN;

    int_in[0] = 4;
    int_in[1] = 0;
    int_in[2] = 0;
    int_in[3] = EDINIT;
    addr_in[0] = (LONG)tree;
    aes(OBJC_EDIT, 4, 2, 1);

    why = check_aes();
    if (why)
    {
        fail("objc_edit with long te_tmplen", why);
        goto out;
    }

    /*
     * pass 3: over-long te_txtlen, which bounds how far ins_char() grows the
     * text. pins the contract (AES never returns more than AES_MAX_LEN-1
     * chars) rather than reproducing a crash, since the D.g_valstr bound from
     * pass 2 already stops the cursor going further.
     */
    strcpy(edittext, "AB");
    eted.te_pvalid = "X";       /* accept any character... */
    eted.te_tmplen = STRLEN;    /* ...at every cursor position */
    eted.te_txtlen = STRLEN;

    idx = 2;
    for (i = 0; i < KEYSTROKES; i++)
    {
        int_in[0] = 4;
        int_in[1] = KEY_Z;
        int_in[2] = idx;
        int_in[3] = EDCHAR;
        addr_in[0] = (LONG)tree;
        aes(OBJC_EDIT, 4, 2, 1);
        idx = int_out[1];
    }

    if (strlen(edittext) >= AES_MAX_LEN)
    {
        fail("objc_edit with long te_txtlen", "text grew past the AES buffer");
        goto out;
    }

    why = check_aes();
    if (why)
    {
        fail("objc_edit with long te_txtlen", why);
        goto out;
    }

    /*
     * pass 4: template with fewer '_' than the text has characters. a
     * liveness check only: find_pos() used to run off the template's end and
     * might never terminate, so all we can observe is whether the AES
     * comes back.
     */
    strcpy(edittext, "ABCDE");
    eted.te_ptmplt = "";        /* no '_' at all */
    eted.te_pvalid = "X";
    eted.te_txtlen = sizeof(edittext);
    eted.te_tmplen = 1;

    int_in[0] = 4;
    int_in[1] = 0;
    int_in[2] = 0;
    int_in[3] = EDINIT;
    addr_in[0] = (LONG)tree;
    aes(OBJC_EDIT, 4, 2, 1);

    why = check_aes();
    if (why)
    {
        fail("objc_edit with a template short of '_'", why);
        goto out;
    }

    write_result("RESULT: PASS");
    rc = 0;

out:
    aes(APPL_EXIT, 0, 1, 0);
    return rc;
}
