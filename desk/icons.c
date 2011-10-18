/*      COMPICON.H      10/05/84 - 10/10/84     Gregg Morris            */
/* taken from ICONHI.H  10/05/84 -      06/11/85        Lee Lorenzen    */
/* added Desktop Publisher icons        03/27/87        Gregg Morris    */
/* 2002/04/03 : Renamed to icons.c, it's now included in EmuTOS - THH   */
 
#include "config.h"
#include "portab.h"
#include "compat.h"
#include "obdefs.h"
#include "deskapp.h"

#if TOS_VERSION >= 0x200    /* Don't include icons in TOS 1.x to save space */


/*ICONBLK
   (L)ib_pmask, (L)ib_pdata, (L)ib_ptext,
   (W)ib_char, (W)ib_xchar, (W)ib_ychar,
   (W)ib_xicon, (W)ib_yicon, (W)ib_wicon, (W)ib_hicon,
   (W)ib_xtext, (W)ib_ytext, (W)ib_wtext, (W)ib_htext;
*/


const ICONBLK gl_ilist[NUM_IBLKS] =
{
/* System Icons:        */
    { 0x0L,  0x1L,  -1L, 0x1000,5,11, 23,0,32,32, 0,32,72,10}, /*IGHARD 0*/
    { 0x2L,  0x3L,  -1L, 0x1000,4,11, 23,0,32,32, 0,32,72,10}, /*IGFLOPPY 1*/
    { 0x4L,  0x5L,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IGFOLDER 2*/
    { 0x6L,  0x7L,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IGTRASH 3*/
    { 0x8L,  0x9L,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG4RESV 4*/
    { 0x8L,  0x9L,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG5RESV 5*/
    { 0x8L,  0x9L,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG6RESV 6*/
    { 0x8L,  0x9L,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IG7RESV 7*/
/* Application Icons:   */
    { 0x8L,  0x9L,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAGENER 8*/
    { 0x8L,  0xAL,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IASS 9*/
    { 0x8L,  0xBL,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAWP 10*/
    { 0x8L,  0xCL,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IADB 11*/
    { 0x8L,  0xDL,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IADRAW 12*/
    { 0x8L,  0xEL,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAPAINT 13*/
    { 0x8L,  0xFL,  -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAPROJ 14*/
    { 0x8L,  0x10L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAGRAPH 15*/
    { 0x8L,  0x11L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAOUTL 16*/
    { 0x8L,  0x12L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAACCNT 17*/
    { 0x8L,  0x13L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAMULTI 18*/
    { 0x8L,  0x14L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAEDUC 19*/
    { 0x8L,  0x15L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IACOMM 20*/
    { 0x8L,  0x16L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IATOOL 21*/
    { 0x8L,  0x17L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAGAME 22*/
    { 0x8L,  0x18L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAOUTPT 23*/
    { 0x8L,  0x19L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IADPUB 24*/
    { 0x8L,  0x1AL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IASCAN 25*/
    { 0x8L,  0x1BL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IAMAIL 26*/
/* currently unused Application icons:  */
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 4*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 5*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 6*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 7*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 8*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 9*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 10*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 11*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 12*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 13*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 14*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 15*/
    { 0x8L,  0x9L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10},  /*IARSV 16*/
/* Document Icons:      */
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDGENER 40*/
    { 0x1CL, 0x1EL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDSS 41*/
    { 0x1CL, 0x1FL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDWP 42*/
    { 0x1CL, 0x20L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDDB 43*/
    { 0x1CL, 0x21L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDDRAW 44*/
    { 0x1CL, 0x22L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDPAINT 45*/
    { 0x1CL, 0x23L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDPROJ 46*/
    { 0x1CL, 0x24L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDGRAPH 47*/
    { 0x1CL, 0x25L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDOUTLN 48*/
    { 0x1CL, 0x26L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDACCNT 49*/
    { 0x1CL, 0x27L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDMULTI 50*/
    { 0x1CL, 0x28L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDEDUC 51*/
    { 0x1CL, 0x29L, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDCOMM 52*/
    { 0x1CL, 0x2AL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDTOOL 53*/
    { 0x1CL, 0x2BL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDGAME 54*/
    { 0x1CL, 0x2CL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDOUTPT 55*/
    { 0x1CL, 0x2DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDDPUB 56*/
/* currently unused Document icons:     */
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 2*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 3*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 4*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 5*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 6*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 7*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 8*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 9*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 10*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 11*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 12*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 13*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 14*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}, /*IDRSV 15*/
    { 0x1CL, 0x1DL, -1L, 0x1000,0,0,  23,0,32,32, 0,32,72,10}  /*IDRSV 16*/
};

#define INCLUDE_ONLY_ICON_DATA
#define DATASIZE 0x0040
static const UWORD icondata[][DATASIZE] =
{
#include "icons/ighdskmh.icn" /*x0*/            /* System icons         */
,
#include "icons/ighdskdh.icn"
,
#include "icons/igfdskmh.icn"
,
#include "icons/igfdskdh.icn"
,
#include "icons/igfoldmh.icn"
,
#include "icons/igfolddh.icn"
,
#include "icons/igtrshmh.icn"
,
#include "icons/igtrshdh.icn" /*x7*/
,
/* Placeholders for future icons
#include "icons/igres4mh.icn"
,
#include "icons/igres4dh.icn"
,
#include "icons/igres5mh.icn"
,
#include "icons/igres5dh.icn"
,
#include "icons/igres6mh.icn"
,
#include "icons/igres6dh.icn"
,
#include "icons/igres7mh.icn"
,
#include "icons/igres7dh.icn"
,
#include "icons/igres8mh.icn"
,
#include "icons/igres8dh.icn"
,
*/
#include "icons/iagenrmh.icn" /*x8*/            /* application icons    */
,
#include "icons/iagenrdh.icn"
,
#include "icons/iasprddh.icn"
,
#include "icons/iaworddh.icn"
,
#include "icons/iadbasdh.icn"
,
#include "icons/iadrawdh.icn"
,
#include "icons/iapantdh.icn"
,
#include "icons/iaprojdh.icn"
,
#include "icons/iagrphdh.icn" /*x10*/
,
#include "icons/iaoutldh.icn"
,
#include "icons/iaacctdh.icn"
,
#include "icons/iamultdh.icn"
,
#include "icons/iaeducdh.icn"
,
#include "icons/iacommdh.icn"
,
#include "icons/iatooldh.icn"
,
#include "icons/iagamedh.icn"
,
#include "icons/iaoutpdh.icn"
,
#include "icons/iadpubdh.icn" /*x19*/
,
#include "icons/iascandh.icn"
,
#include "icons/iamaildh.icn"
,
/* Placeholders for future Application icons
#include "icons/iars04dh.icn"
,
#include "icons/iars05dh.icn"
,
#include "icons/iars06dh.icn"
,
#include "icons/iars07dh.icn"
,
#include "icons/iars08dh.icn"
,
#include "icons/iars09dh.icn"
,
#include "icons/iars10dh.icn"
,
#include "icons/iars11dh.icn"
,
#include "icons/iars12dh.icn"
,
#include "icons/iars13dh.icn"
,
#include "icons/iars14dh.icn"
,
#include "icons/iars15dh.icn"
,
#include "icons/iars16dh.icn"
,
*/
#include "icons/idgenrmh.icn"                   /* document icons       */
,
#include "icons/idgenrdh.icn"
,
#include "icons/idsprddh.icn"
,
#include "icons/idworddh.icn"
,
#include "icons/iddbasdh.icn"
,
#include "icons/iddrawdh.icn"
,
#include "icons/idpantdh.icn" /*x20*/
,
#include "icons/idprojdh.icn"
,
#include "icons/idgrphdh.icn"
,
#include "icons/idoutldh.icn"
,
#include "icons/idacctdh.icn"
,
#include "icons/idmultdh.icn"
,
#include "icons/ideducdh.icn"
,
#include "icons/idcommdh.icn"
,
#include "icons/idtooldh.icn" /*x28*/
,
#include "icons/idgamedh.icn"
,
#include "icons/idoutpdh.icn"
,
#include "icons/iddpubdh.icn"
,
/* Placeholders for future Document Icons
#include "icons/idrs02dh.icn"
,
#include "icons/idrs03dh.icn"
,
#include "icons/idrs04dh.icn"
,
#include "icons/idrs05dh.icn"
,
#include "icons/idrs06dh.icn"
,
#include "icons/idrs07dh.icn"
,
#include "icons/idrs08dh.icn"
,
#include "icons/idrs09dh.icn"
,
#include "icons/idrs10dh.icn"
,
#include "icons/idrs11dh.icn"
,
#include "icons/idrs12dh.icn"
,
#include "icons/idrs13dh.icn"
,
#include "icons/idrs14dh.icn"
,
#include "icons/idrs15dh.icn"
,
#include "icons/idrs16dh.icn"
,
*/
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


const void * const icondata_start = &icondata; /* Pointer to array = start of array */

const void * const icondata_end = &icondata + 1; /* Pointer to array + 1 = end of array */


#endif  /* TOS_VERSION >= 0x200 */
