/*
 *  draftexc: exclude_items[] array for the draft program
 *
 *  Copyright 2019 Roger Burrows
 *
 *  This program is licensed under the GNU General Public License.
 *  Please see LICENSE.TXT for details.
 */
#include "stdio.h"
#include "../include/config.h"

/*
 *  list of items to be deleted from resource before processing by erd
 *
 *  This will need to be updated when a configurable alert/freestring/tree
 *  is added to the resource
 */
char *exclude_items[] =
{
#if !CONF_WITH_BACKGROUNDS
    "BACKGRND",
    "SEP_VW1",
    "ADBKGND",
#endif
#if !CONF_WITH_BLITTER
    "BLITITEM",
    "SEP_OP1",
#endif
#if !CONF_WITH_DESKTOP_CONFIG
    "CONFITEM",
    "ADDESKCF",
#endif
#if !CONF_WITH_DESKTOP_SHORTCUTS
    "STLOCATE",
    "STRMVLOC",
#else
    "STNODRA1",
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
#if !CONF_WITH_PRINTER_ICON
    "STPRINFO",
#endif
#if !CONF_WITH_SHOW_FILE
    "ADPRINT",
    "STMORE",
    "STEOF",
    "STFRE",
    "STSHOW",
    "STPRTERR",
#else
    "STNOAPPL",
#endif
#if !CONF_WITH_SHUTDOWN
    "QUITITEM",
#endif
#if !CONF_WITH_TT_SHIFTER
    "ADTTREZ",
#endif
#if !CONF_WITH_VIDEL
    "ADFALREZ",
    "STREZ1",
    "STREZ2",
    "STREZ3",
    "STREZ4",
#endif
#if !CONF_WITH_WINDOW_ICONS
    "ADINSWIN",
    "STICNTYP",
    "STNOMTCH",
#endif
#if !defined(MACHINE_AMIGA)
    "ADAMIREZ",
#endif
#if !WITH_CLI
    "CLIITEM",
#endif
#if !WITH_CLI && !CONF_WITH_SHUTDOWN
    "SEP_FL2",
#endif
    NULL                            /* end marker */
};
