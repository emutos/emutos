/*
 *  draftexc: exclude_items[] array for the draft program
 *
 *  Copyright 2019-2024 Roger Burrows
 *
 *  This program is licensed under the GNU General Public License.
 *  Please see LICENSE.TXT for details.
 */
#include "stdio.h"
#include "../include/config.h"

/*
 *  list of items to be deleted from resource before processing by erd
 *
 *  This must be updated when a configurable alert/freestring/tree
 *  is added to the resource
 */
char *exclude_items[] =
{
                                /* configurable items under 'File' */
#if !CONF_WITH_SEARCH
    "SRCHITEM",
    "ADSEARCH",
    "STCNSRCH",
    "STNOMORE",
#endif
#if !CONF_WITH_BOTTOMTOTOP
    "BTOPITEM",
#endif
#if !CONF_WITH_SELECTALL
    "SLCTITEM",
#endif
#if !CONF_WITH_FILEMASK
    "MASKITEM",
    "ADFMASK",
#endif
#if !CONF_WITH_FORMAT
    "FORMITEM",
    "ADFORMAT",
    "STFMTERR",
    "STFMTINF",
#endif
#if !CONF_WITH_EJECT
    "EJCTITEM",
#endif
#if !WITH_CLI && !CONF_WITH_SHUTDOWN
    "SEP_FL2",
#endif
#if !WITH_CLI
    "CLIITEM",
#endif
#if !CONF_WITH_SHUTDOWN
    "QUITITEM",
#endif

                                /* configurable items under 'View' */
#if !CONF_WITH_SIZE_TO_FIT
    "FITITEM",
    "SEP_VW1",
#endif
#if !CONF_WITH_BACKGROUNDS
    "BACKGRND",
    "SEP_VW2",
    "ADBKGND",
#endif

                                /* configurable items under 'Options' */
#if !CONF_WITH_DESKTOP_CONFIG
    "CONFITEM",
    "ADDESKCF",
    "STDUPCUT",
#endif
#if !CONF_WITH_READ_INF
    "READITEM",
    "SEP_OP0",
    "STRDINF",
    "STINVINF",
#endif
#if !CONF_WITH_BLITTER
    "BLITITEM",
    "SEP_OP1",
#endif
#if !CONF_WITH_CACHE_CONTROL
    "CACHITEM",
    "SEP_OP2",
#endif

                                /* other configurable items */
#if !CONF_WITH_SHOW_FILE && !CONF_WITH_PRINTER_ICON /* print/show file */
    "ADPRINT",
    "STPRTERR",
#endif
#if !CONF_WITH_SHOW_FILE
    "STMORE",
    "STEOF",
    "STFRE",
    "STSHOW",
#else
    "STNOAPPL",
#endif
#if !CONF_WITH_PRINTER_ICON
    "STPRINT",
    "STPRINFO",
#endif

#if !CONF_WITH_TT_SHIFTER           /* resolution setting */
    "ADTTREZ",
#endif
#if !CONF_WITH_VIDEL
    "ADFALREZ",
    "STREZ1",
    "STREZ2",
    "STREZ3",
    "STREZ4",
#endif
#if !defined(MACHINE_AMIGA)
    "ADAMIREZ",
#endif

#if !CONF_WITH_WINDOW_ICONS         /* icon-related */
    "ADINSWIN",
    "STICNTYP",
    "STNOMTCH",
#endif
#if !CONF_WITH_DESKTOP_SHORTCUTS
    "STLOCATE",
    "STRMVLOC",
#else
    "STNODRA1",
#endif

    NULL                            /* end marker */
};
