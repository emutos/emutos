/*      COMPICON.H      10/05/84 - 10/10/84     Gregg Morris            */
/* taken from ICONHI.H  10/05/84 -      06/11/85        Lee Lorenzen    */
/* added Desktop Publisher icons        03/27/87        Gregg Morris    */
/* 2002/04/03 : Renamed to icons.c, it's now included in EmuTOS - THH   */
 
#include "config.h"
#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "deskapp.h"

#if TOS_VERSION >= 0x200    /* Don't include icons in TOS 1.x to save space */


/*ICONBLK
   (L)ib_pmask, (L)ib_pdata, (L)ib_ptext,
   (W)ib_char, (W)ib_xchar, (W)ib_ychar,
   (W)ib_xicon, (W)ib_yicon, (W)ib_wicon, (W)ib_hicon,
   (W)ib_xtext, (W)ib_ytext, (W)ib_wtext, (W)ib_htext;
*/


ICONBLK gl_ilist[NUM_IBLKS] =
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

#include <ighdskmh.icn> /*x0*/                  /* System icons         */
#include <ighdskdh.icn>
#include <igfdskmh.icn>
#include <igfdskdh.icn>
#include <igfoldmh.icn>
#include <igfolddh.icn>
#include <igfold3h.icn>
#include <igflop3h.icn> /*x7*/
/* Placeholders for future icons
#include <IGRES4MH.ICN>
#include <IGRES4DH.ICN>
#include <IGRES5MH.ICN>
#include <IGRES5DH.ICN>
#include <IGRES6MH.ICN>
#include <IGRES6DH.ICN>
#include <IGRES7MH.ICN>
#include <IGRES7DH.ICN>
#include <IGRES8MH.ICN>
#include <IGRES8DH.ICN>
*/
#include <iagenrmh.icn> /*x8*/                  /* application icons    */
#include <iagenrdh.icn>
#include <iasprddh.icn>
#include <iaworddh.icn>
#include <iadbasdh.icn>
#include <iadrawdh.icn>
#include <iapantdh.icn>
#include <iaprojdh.icn>
#include <iagrphdh.icn> /*x10*/
#include <iaoutldh.icn>
#include <iaacctdh.icn>
#include <iamultdh.icn>
#include <iaeducdh.icn>
#include <iacommdh.icn>
#include <iatooldh.icn>
#include <iagamedh.icn>
#include <iaoutpdh.icn>
#include <iadpubdh.icn> /*x19*/
#include <iascandh.icn>
#include <iamaildh.icn>
/* Placeholders for future Application icons
#include <IARS04DH.ICN>
#include <IARS05DH.ICN>
#include <IARS06DH.ICN>
#include <IARS07DH.ICN>
#include <IARS08DH.ICN>
#include <IARS09DH.ICN>
#include <IARS10DH.ICN>
#include <IARS11DH.ICN>
#include <IARS12DH.ICN>
#include <IARS13DH.ICN>
#include <IARS14DH.ICN>
#include <IARS15DH.ICN>
#include <IARS16DH.ICN>
*/
#include <idgenrmh.icn>                         /* document icons       */
#include <idgenrdh.icn>
#include <idsprddh.icn>
#include <idworddh.icn>
#include <iddbasdh.icn>
#include <iddrawdh.icn>
#include <idpantdh.icn> /*x20*/
#include <idprojdh.icn>
#include <idgrphdh.icn>
#include <idoutldh.icn>
#include <idacctdh.icn>
#include <idmultdh.icn>
#include <ideducdh.icn>
#include <idcommdh.icn>
#include <idtooldh.icn> /*x28*/
#include <idgamedh.icn>
#include <idoutpdh.icn>
#include <iddpubdh.icn>
/* Placeholders for future Document Icons
#include <IDRS02DH.ICN>
#include <IDRS03DH.ICN>
#include <IDRS04DH.ICN>
#include <IDRS05DH.ICN>
#include <IDRS06DH.ICN>
#include <IDRS07DH.ICN>
#include <IDRS08DH.ICN>
#include <IDRS09DH.ICN>
#include <IDRS10DH.ICN>
#include <IDRS11DH.ICN>
#include <IDRS12DH.ICN>
#include <IDRS13DH.ICN>
#include <IDRS14DH.ICN>
#include <IDRS15DH.ICN>
#include <IDRS16DH.ICN>
*/

/*WORD icondata_end;*/

/* Icon names for use in Desktop's Configure Application dialog */
BYTE    *cfg_icons_txt[32]=
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


void *icondata_start = &ighdskmh[0];

void *icondata_end = &iddpubdh[DATASIZE];


#endif  /* TOS_VERSION >= 0x200 */
