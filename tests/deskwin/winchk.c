/*
 * winchk.c - regression test for AES windows outliving the ROM desktop
 *
 * Copyright (C) 2026 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * checks that no window handle is left in use after the ROM desktop
 * terminates and a character-mode program is launched from it.
 *
 * meaningful only if EMUDESK.INF's '#W' lines actually reopened some
 * windows at boot, so count_saved_windows() verifies that first rather
 * than reporting a vacuous PASS.
 *
 * no gemlib to link against, so it hand-rolls the minimal AES bindings
 * it needs (trap #2, control/global/int_in/int_out/addr_in/addr_out),
 * copied from tests/expandstr/expstr.c.
 */

#include <stdio.h>

typedef short WORD;
typedef long LONG;

#define APPL_INIT   10
#define APPL_EXIT   19
#define WIND_CREATE 100
#define WIND_DELETE 103
#define WIND_GET    104
#define SHEL_GET    122

#define WF_TOP      10

#define NAME        0x0001
#define CLOSER      0x0002
#define MOVER       0x0008

#define SHELBUF_LEN 4192        /* SIZE_SHELBUF, see include/sysconf.h */

static WORD control[5], global[16], int_in[16], int_out[8];
static LONG addr_in[3], addr_out[1];
static void *aespb[6] = { control, global, int_in, int_out, addr_in, addr_out };

static char shelbuf[SHELBUF_LEN];

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

/*
 * count desktop windows open at termination, via the '#W' lines app_save()
 * shel_put()s on exit (an empty path field means the slot wasn't open).
 * scan byte by byte: the shell buffer starts with binary data that may
 * contain NULs.
 */
static WORD count_saved_windows(void)
{
    WORD i, j;
    WORD count = 0;

    addr_in[0] = (LONG)shelbuf;
    int_in[0] = SHELBUF_LEN;
    aes(SHEL_GET, 1, 1, 1);

    for (i = 0; i < SHELBUF_LEN-2; i++)
    {
        if ((shelbuf[i] != '#') || (shelbuf[i+1] != 'W'))
            continue;
        /* find the end of the line */
        for (j = i+2; j < SHELBUF_LEN; j++)
            if ((shelbuf[j] == '\r') || (shelbuf[j] == '\n'))
                break;
        /* the line ends in " <path>@": an empty path means "not open" */
        if ((j >= i+4) && (shelbuf[j-1] == '@') && (shelbuf[j-2] != ' '))
            count++;
    }

    return count;
}

static void write_result(WORD saved, WORD top, WORD first, const char *result)
{
    FILE *fh = fopen("WINCHK.OUT", "w");

    if (fh)
    {
        fprintf(fh, "SAVED: %ld TOP: %ld FIRST: %ld\n%s\n",
                (long)saved, (long)top, (long)first, result);
        fclose(fh);
    }
}

int main(void)
{
    WORD ap_id;
    WORD saved, top, first;
    int rc = 1;

    ap_id = aes(APPL_INIT, 0, 1, 0);
    if (ap_id < 0)
    {
        write_result(-1, -1, -1, "RESULT: FAIL: appl_init failed");
        return 1;
    }

    saved = count_saved_windows();

    int_in[0] = 0;              /* desktop window handle */
    int_in[1] = WF_TOP;
    aes(WIND_GET, 2, 5, 0);
    top = int_out[1];

    int_in[0] = NAME | CLOSER | MOVER;
    int_in[1] = 0;
    int_in[2] = 0;
    int_in[3] = 80;
    int_in[4] = 80;
    first = aes(WIND_CREATE, 5, 1, 0);
    if (first > 0)
    {
        int_in[0] = first;
        aes(WIND_DELETE, 1, 1, 0);
    }

    if (saved <= 0)
    {
        /* nothing was open, so this would pass vacuously even without the fix */
        write_result(saved, top, first,
                     "RESULT: FAIL: desktop had no window open, test is void");
    }
    else if ((top == 0) && (first == 1))
    {
        write_result(saved, top, first, "RESULT: PASS");
        rc = 0;
    }
    else
    {
        write_result(saved, top, first, "RESULT: FAIL: stale desktop window(s)");
    }

    aes(APPL_EXIT, 0, 1, 0);
    return rc;
}
