/*	ICONLO.H	10/05/84 - 06/11/85 	Lee Lorenzen		*/
/* added Desktop Publisher icons	03/27/87	Gregg Morris	*/
/* Added Scan,Mail icons		02/09/88        Chris Lusby Taylor*/

#define NUM_IBLKS 72
/*ICONBLK
   (L)ib_pmask, (L)ib_pdata, (L)ib_ptext,
   (W)ib_char, (W)ib_xchar, (W)ib_ychar,
   (W)ib_xicon, (W)ib_yicon, (W)ib_wicon, (W)ib_hicon,
   (W)ib_xtext, (W)ib_ytext, (W)ib_wtext, (W)ib_htext;
*/

EXTERN BYTE	*pi[];	    
BYTE	**gl_strs = {&pi[0]};
BYTE	***gl_start = {&gl_strs};

ICONBLK	gl_ilist[NUM_IBLKS] =
{
/* System Icons:	*/
	0x0L,  0x1L,  -1L, 0x1000,6,7, 15,0,48,24, 4,24,72,10,	/*IGHARD 0*/
	0x2L,  0x3L,  -1L, 0x1000,7,5,  15,0,48,24, 4,24,72,10,	/*IGFLOPPY 1*/
	0x4L,  0x5L,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IGFOLDER 2*/
	0x6L,  0x7L,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IGTRASH 3*/
	0x8L,  0x9L,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IG4RESV 4*/
	0x8L,  0x9L,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IG5RESV 5*/
	0x8L,  0x9L,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IG6RESV 6*/
	0x8L,  0x9L,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IG7RESV 7*/
/* Application Icons:	*/
	0x8L,  0x9L,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAGENER 8*/
	0x8L,  0xAL,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IASS 9*/
	0x8L,  0xBL,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAWP 10*/
	0x8L,  0xCL,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IADB 11*/
	0x8L,  0xDL,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IADRAW 12*/
	0x8L,  0xEL,  -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAPAINT 13*/
	0x8L,  0xFL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAPROJ 14*/
	0x8L,  0x10L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAGRAPH 15*/
	0x8L,  0x11L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAOUTL 16*/
	0x8L,  0x12L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAACCNT 17*/
	0x8L,  0x13L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAMULTI 18*/
	0x8L,  0x14L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAEDUC 19*/
	0x8L,  0x15L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IACOMM 20*/
	0x8L,  0x16L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IATOOL 21*/
	0x8L,  0x17L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAGAME 22*/
	0x8L,  0x18L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAOUTPT 23*/
	0x8L,  0x19L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IADPUB 24*/
	0x8L,  0x1AL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IASCAN 25*/
	0x8L,  0x1BL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IAMAIL 26*/
/* currently unused Application icons:	*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 4*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 5*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 6*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 7*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 8*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 9*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 10*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 11*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 12*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 13*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 14*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 15*/
	0x8L,  0x9L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IARSV 16*/
/* Document Icons:	*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDGENER 40*/
	0x1CL, 0x1EL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDSS 41*/
	0x1CL, 0x1FL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDWP 42*/
	0x1CL, 0x20L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDDB 43*/
	0x1CL, 0x21L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDDRAW 44*/
	0x1CL, 0x22L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDPAINT 45*/
	0x1CL, 0x23L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDPROJ 46*/
	0x1CL, 0x24L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDGRAPH 47*/
	0x1CL, 0x25L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDOUTLN 48*/
	0x1CL, 0x26L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDACCNT 49*/
	0x1CL, 0x27L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDMULTI 50*/
	0x1CL, 0x28L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDEDUC 51*/
	0x1CL, 0x29L, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDCOMM 52*/
	0x1CL, 0x2AL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDTOOL 53*/
	0x1CL, 0x2BL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDGAME 54*/
	0x1CL, 0x2CL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDOUTPT 55*/
	0x1CL, 0x2DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 1*/
/* currently unused Document icons:	*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 2*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 3*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 4*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 5*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 6*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 7*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 8*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 9*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 10*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 11*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 12*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 13*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 14*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10,	/*IDRSV 15*/
	0x1CL, 0x1DL, -1L, 0x1000,0,0,  15,0,48,24, 4,24,72,10	/*IDRSV 16*/
};

#include <IGHDSKML.ICN>	/*x0*/			/* System icons		*/
#include <IGHDSKDL.ICN>
#include <IGFDSKML.ICN>
#include <IGFDSKDL.ICN>
#include <IGFOLDML.ICN>
#include <IGFOLDDL.ICN>
#include <IGFOLD3L.ICN>
#include <IGFLOP3L.ICN>	/*x7*/
/* Placeholders for future icons
#include <IGRES4ML.ICN>
#include <IGRES4DL.ICN>
#include <IGRES5ML.ICN>
#include <IGRES5DL.ICN>
#include <IGRES6ML.ICN>
#include <IGRES6DL.ICN>
#include <IGRES7ML.ICN>
#include <IGRES7DL.ICN>
#include <IGRES8ML.ICN>
#include <IGRES8DL.ICN>
*/
#include <IAGENRML.ICN>	/*x8*/			/* Application Icons	*/
#include <IAGENRDL.ICN>
#include <IASPRDDL.ICN>
#include <IAWORDDL.ICN>
#include <IADBASDL.ICN>
#include <IADRAWDL.ICN>
#include <IAPANTDL.ICN>
#include <IAPROJDL.ICN>
#include <IAGRPHDL.ICN>	/*x10*/
#include <IAOUTLDL.ICN>
#include <IAACCTDL.ICN>
#include <IAMULTDL.ICN>
#include <IAEDUCDL.ICN>
#include <IACOMMDL.ICN>
#include <IATOOLDL.ICN>
#include <IAGAMEDL.ICN>
#include <IAOUTPDL.ICN>	/*x18*/
#include <IADPUBDL.ICN>
#include <IASCANDL.ICN>
#include <IAMAILDL.ICN>
/* Placeholders for future Application icons
#include <IARS04DL.ICN>
#include <IARS05DL.ICN>
#include <IARS06DL.ICN>
#include <IARS07DL.ICN>
#include <IARS08DL.ICN>
#include <IARS09DL.ICN>
#include <IARS10DL.ICN>
#include <IARS11DL.ICN>
#include <IARS12DL.ICN>
#include <IARS13DL.ICN>
#include <IARS14DL.ICN>
#include <IARS15DL.ICN>
#include <IARS16DL.ICN>
*/
#include <IDGENRML.ICN>				/* Document Icons	*/
#include <IDGENRDL.ICN>
#include <IDSPRDDL.ICN>
#include <IDWORDDL.ICN>
#include <IDDBASDL.ICN>
#include <IDDRAWDL.ICN>
#include <IDPANTDL.ICN>	/*x20*/
#include <IDPROJDL.ICN>
#include <IDGRPHDL.ICN>
#include <IDOUTLDL.ICN>
#include <IDACCTDL.ICN>
#include <IDMULTDL.ICN>
#include <IDEDUCDL.ICN>
#include <IDCOMMDL.ICN>
#include <IDTOOLDL.ICN>	/*x28*/
#include <IDGAMEDL.ICN>
#include <IDOUTPDL.ICN>
#include <IDDPUBDL.ICN>
/* Placeholders for future Document Icons
#include <IDRS02DL.ICN>
#include <IDRS03DL.ICN>
#include <IDRS04DL.ICN>
#include <IDRS05DL.ICN>
#include <IDRS06DL.ICN>
#include <IDRS07DL.ICN>
#include <IDRS08DL.ICN>
#include <IDRS09DL.ICN>
#include <IDRS10DL.ICN>
#include <IDRS11DL.ICN>
#include <IDRS12DL.ICN>
#include <IDRS13DL.ICN>
#include <IDRS14DL.ICN>
#include <IDRS15DL.ICN>
#include <IDRS16DL.ICN>
*/

/* Icon names for use in Desktop's Configure Application dialog	*/
BYTE	*pi[32]=
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
