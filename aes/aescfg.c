/*
 * EmuTOS AES configuration, including reading the config file.
 *
 * This file was created for the genxtos fork of EmuTOS
 * The configuration file follows the principle of the ATARI
 * DESKTOP.INF or NEWSDESK.INF files but now we could make it 
 * more user friendly.
 *
 * Copyright (C) 2002-2021 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "string.h" /* strcpy */
#include "gemdos.h" /* dos_xxx */
#include "gemshlib.h"
#include "struct.h"
#include "optimize.h" /* scan_2 */
#include "aescfg.h"

struct aes_configuration_t aes_configuration;


#define INF_SIZE 300 + 1        /*  for start of EMUDESK.INF file    */


/*
 * Read the AES's configuration file. On ATARI TOS, this is the same as the desktop app's
 * config file, but here we only care about AES-relevant settings
 */
void aescfg_read(void)
{
    LONG  n;
    char *pcurr;
    char *infbuf;
    WORD  v1,v2;
    int   i;

    aes_configuration.flags = 0;
 
    infbuf = (char*)dos_alloc_anyram(INF_SIZE+1);
    if (infbuf == NULL)
        return; /* fail silently */
    
    /* Read file into buffer */
    n = sh_readfile(INF_FILE_NAME, INF_SIZE, infbuf);
    if (n < 0L)
        n = 0L;
    infbuf[n] = '\0';

    for (pcurr = infbuf; *pcurr; )
    {
        /* Wait start of command */
        if (*pcurr++ != '#')
            continue;

        switch (*pcurr++)
        {
            case 'E':
                pcurr += 2;

                /* Set double click speed */
                pcurr = scan_2(pcurr, &aes_configuration.double_click_rate);
                aes_configuration.double_click_rate &= 0x07;
                aes_configuration.flags |= AES_CFG_PROVIDES_DCLICK_SPEED;

                /* Blitter on/off */
                pcurr = scan_2(pcurr, &aes_configuration.blitter_on);
                aes_configuration.blitter_on = (aes_configuration.blitter_on & 0x80) ? TRUE : FALSE; 
                aes_configuration.flags |= AES_CFG_PROVIDES_BLITTER;

                if (*pcurr == '\r')  /* The rest can be missing */
                    continue;

                /* Video mode */
                pcurr = scan_2(pcurr, &v1);
                pcurr = scan_2(pcurr, &v2);
                aes_configuration.videomode = MAKE_UWORD(v1, v2);
                aes_configuration.flags |= AES_CFG_PROVIDES_RESOLUTION;

#if CONF_WITH_CACHE_CONTROL
                pcurr = scan_2(pcurr, &aes_configuration.cpu_cache_on);            /* get desired cache state */
                aes_configuration.cpu_cache_on = (aes_configuration.cpu_cache_on & 0x08 ? FALSE : TRUE);
                aes_configuration.flags |= AES_CFG_PROVIDES_CPUCACHE;
#endif
                break;

#if CONF_WITH_BACKGROUNDS
            case 'Q':
                /* Desktop background colour, e.g. #Q 41 40 42 40 43 40 */
                for (i = 0; i < 3; i++)
                {
                    pcurr = scan_2(pcurr, &v1) + 3;
                    aes_configuration.desktop_background[i] = v1;
                }
                aes_configuration.flags |= AES_CFG_PROVIDES_BACKGROUND;
                break;
#endif

            case 'Z':
            {
                /* Auto exec file. Something like "#Z 01 C:\THING.APP@" */
                char *path;
                pcurr += 1;
                scan_2(pcurr, &v1);  /* 00 => not GEM, otherwise GEM */
                aes_configuration.autostart_is_gem = (v1 != 0);

                pcurr += 3;
                path = pcurr;
                while (*pcurr && (*pcurr != '@'))
                    ++pcurr;
                *pcurr++ = 0;

                strcpy(aes_configuration.autostart,path);
                aes_configuration.flags |= AES_CFG_PROVIDES_AUTOSTART;
                break;
            }

            default:
                break;
        }
    }

    dos_free(infbuf);
}