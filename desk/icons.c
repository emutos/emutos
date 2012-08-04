/*      COMPICON.H      10/05/84 - 10/10/84     Gregg Morris            */
/* taken from ICONHI.H  10/05/84 -      06/11/85        Lee Lorenzen    */
/* added Desktop Publisher icons        03/27/87        Gregg Morris    */
/* 2002/04/03 : Renamed to icons.c, it's now included in EmuTOS - THH   */
 
#include "config.h"
#include "portab.h"
#include "obdefs.h"
#include "deskapp.h"

#if CONF_WITH_DESKTOP_ICONS

/*
 * System icons
 */
#include "icons/ighdskmh.icn" /*x0*/
#include "icons/ighdskdh.icn"
#include "icons/igfdskmh.icn"
#include "icons/igfdskdh.icn"
#include "icons/igfoldmh.icn"
#include "icons/igfolddh.icn"
#include "icons/igtrshmh.icn"
#include "icons/igtrshdh.icn" /*x7*/

/*
 * Application icons
 */
#include "icons/iagenrmh.icn" /*x8*/
#include "icons/iagenrdh.icn"
#include "icons/iasprddh.icn"
#include "icons/iaworddh.icn"
#include "icons/iadbasdh.icn"
#include "icons/iadrawdh.icn"
#include "icons/iapantdh.icn"
#include "icons/iaprojdh.icn"
#include "icons/iagrphdh.icn" /*x10*/
#include "icons/iaoutldh.icn"
#include "icons/iaacctdh.icn"
#include "icons/iamultdh.icn"
#include "icons/iaeducdh.icn"
#include "icons/iacommdh.icn"
#include "icons/iatooldh.icn"
#include "icons/iagamedh.icn"
#include "icons/iaoutpdh.icn" /*x18*/
#include "icons/iadpubdh.icn"
#include "icons/iascandh.icn"
#include "icons/iamaildh.icn"

/*
 * Document icons
 */
#include "icons/idgenrmh.icn" /*x1C*/
#include "icons/idgenrdh.icn"
#include "icons/idsprddh.icn"
#include "icons/idworddh.icn"
#include "icons/iddbasdh.icn" /*x20*/
#include "icons/iddrawdh.icn"
#include "icons/idpantdh.icn"
#include "icons/idprojdh.icn"
#include "icons/idgrphdh.icn"
#include "icons/idoutldh.icn"
#include "icons/idacctdh.icn"
#include "icons/idmultdh.icn"
#include "icons/ideducdh.icn" /*x28*/
#include "icons/idcommdh.icn"
#include "icons/idtooldh.icn"
#include "icons/idgamedh.icn"
#include "icons/idoutpdh.icn"
#include "icons/iddpubdh.icn" /*x2D*/



/*ICONBLK
   (L)ib_pmask, (L)ib_pdata, (L)ib_ptext,
   (W)ib_char, (W)ib_xchar, (W)ib_ychar,
   (W)ib_xicon, (W)ib_yicon, (W)ib_wicon, (W)ib_hicon,
   (W)ib_xtext, (W)ib_ytext, (W)ib_wtext, (W)ib_htext;
*/

const ICONBLK gl_ilist[NUM_IBLKS] =
{
/* System Icons:        */
    { (LONG)ighdskmh, (LONG)ighdskdh, -1L, 0x1000,5,11, 23,0,32,32, 0,32,72,10}, /*IGHARD 0*/
    { (LONG)igfdskmh, (LONG)igfdskdh, -1L, 0x1000,4,11, 23,0,32,32, 0,32,72,10}, /*IGFLOPPY 1*/
    { (LONG)igfoldmh, (LONG)igfolddh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IGFOLDER 2*/
    { (LONG)igtrshmh, (LONG)igtrshdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IGTRASH 3*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG4RESV 4*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG5RESV 5*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG6RESV 6*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG7RESV 7*/
/* Application Icons:   */
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAGENER 8*/
    { (LONG)iagenrmh, (LONG)iasprddh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IASS 9*/
    { (LONG)iagenrmh, (LONG)iaworddh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAWP 10*/
    { (LONG)iagenrmh, (LONG)iadbasdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IADB 11*/
    { (LONG)iagenrmh, (LONG)iadrawdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IADRAW 12*/
    { (LONG)iagenrmh, (LONG)iapantdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAPAINT 13*/
    { (LONG)iagenrmh, (LONG)iaprojdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAPROJ 14*/
    { (LONG)iagenrmh, (LONG)iagrphdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAGRAPH 15*/
    { (LONG)iagenrmh, (LONG)iaoutldh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAOUTL 16*/
    { (LONG)iagenrmh, (LONG)iaacctdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAACCNT 17*/
    { (LONG)iagenrmh, (LONG)iamultdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAMULTI 18*/
    { (LONG)iagenrmh, (LONG)iaeducdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAEDUC 19*/
    { (LONG)iagenrmh, (LONG)iacommdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IACOMM 20*/
    { (LONG)iagenrmh, (LONG)iatooldh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IATOOL 21*/
    { (LONG)iagenrmh, (LONG)iagamedh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAGAME 22*/
    { (LONG)iagenrmh, (LONG)iaoutpdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAOUTPT 23*/
    { (LONG)iagenrmh, (LONG)iadpubdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IADPUB 24*/
    { (LONG)iagenrmh, (LONG)iascandh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IASCAN 25*/
    { (LONG)iagenrmh, (LONG)iamaildh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAMAIL 26*/
/* currently unused Application icons:  */
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 4*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 5*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 6*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 7*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 8*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 9*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 10*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 11*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 12*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 13*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 14*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 15*/
    { (LONG)iagenrmh, (LONG)iagenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IARSV 16*/
/* Document Icons:      */
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDGENER 40*/
    { (LONG)idgenrmh, (LONG)idsprddh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDSS 41*/
    { (LONG)idgenrmh, (LONG)idworddh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDWP 42*/
    { (LONG)idgenrmh, (LONG)iddbasdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDDB 43*/
    { (LONG)idgenrmh, (LONG)iddrawdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDDRAW 44*/
    { (LONG)idgenrmh, (LONG)idpantdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDPAINT 45*/
    { (LONG)idgenrmh, (LONG)idprojdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDPROJ 46*/
    { (LONG)idgenrmh, (LONG)idgrphdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDGRAPH 47*/
    { (LONG)idgenrmh, (LONG)idoutldh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDOUTLN 48*/
    { (LONG)idgenrmh, (LONG)idacctdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDACCNT 49*/
    { (LONG)idgenrmh, (LONG)idmultdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDMULTI 50*/
    { (LONG)idgenrmh, (LONG)ideducdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDEDUC 51*/
    { (LONG)idgenrmh, (LONG)idcommdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDCOMM 52*/
    { (LONG)idgenrmh, (LONG)idtooldh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDTOOL 53*/
    { (LONG)idgenrmh, (LONG)idgamedh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDGAME 54*/
    { (LONG)idgenrmh, (LONG)idoutpdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDOUTPT 55*/
    { (LONG)idgenrmh, (LONG)iddpubdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDDPUB 56*/
/* currently unused Document icons:     */
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 2*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 3*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 4*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 5*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 6*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 7*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 8*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 9*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 10*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 11*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 12*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 13*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 14*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 15*/
    { (LONG)idgenrmh, (LONG)idgenrdh, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}  /*IDRSV 16*/
};


/* Icon names for use in Desktop's Configure Application dialog */
const BYTE * const cfg_icons_txt[32] =
{
        " Generic ",
        " Spreadsheet ",
        " Word Processor ",
        " Database ",
        " Draw ",
        " Paint ",
        " Project ",
        " Graph ",
        " Outline ",
        " Accounting ",
        " Multi-Function ",
        " Education ",
        " Communications ",
        " Programmer's Tool ",
        " Game ",
        " Output ",
        " Desktop Publisher ",
        " Scan ",
        " Mail ",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
};

#endif /* CONF_WITH_DESKTOP_ICONS */
