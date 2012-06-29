/*
 *       Copyright 2002 The EmuTOS development team
 *
 *       This software is licenced under the GNU Public License.
 *       Please see LICENSE.TXT for further information.
 */
#define FSELECTR        0
#define FSTITLE         1
#define FSDIRECT        2
#define FSSELECT        3
#define FSOK            4
#define FSCANCEL        5
#define FCLSBOX         7
#define FTITLE          8
#define SCRLBAR         9
#define FUPAROW         10
#define FDNAROW         11
#define FSVSLID         12
#define FSVELEV         13
#define FILEBOX         14
#define F1NAME          15
#define F2NAME          16
#define F3NAME          17
#define F4NAME          18
#define F5NAME          19
#define F6NAME          20
#define F7NAME          21
#define F8NAME          22
#define F9NAME          23

#define DIALERT         1
#define MSGOFF          2
#define BUTOFF          7

#define DESKTOP         2


#define STPATH          0

#define STDESKTP        1

#define STGEM           2

#define STACC           3

#define ST9VAL          4

#define STAVAL          5

#define STNVAL          6

#define STPVAL          7

#define STLPVAL         8

#define STFVAL          9

#define STLFAVAL        10

#define STLAVAL         11

#define STLNVAL         12

#define STGDOS          13

#define STINPATH        14

#define STSCDIR         15

#define STVERSN         16

#define STAVAIL         17

#define STSCREEN        18

#define STINFPAT        19

#define STSCRAP         20    /* currently unused */

#define AL00CRT         21

#define AL01CRT         22

#define AL02CRT         23

#define AL03CRT         24

#define AL04CRT         25

#define AL05CRT         26

#define AL18ERR         27

#define ALNOFIT         28

#define AL04ERR         29

#define AL05ERR         30

#define AL15ERR         31

#define AL16ERR         32

#define AL08ERR         33

#define ALXXERR         34

#define ALNOFUNC        35

#define ALOKDESK        36


#define NOTEBB          0

#define QUESTBB         1

#define STOPBB          2

#define MICE00          3

#define MICE01          4

#define MICE02          5

#define MICE03          6

#define MICE04          7

#define MICE05          8

#define MICE06          9

#define MICE07          10


#define RS_NOBS         37
#define RS_NTREE        3
#define RS_NTED         13
#define RS_NIB          0
#define RS_NBB          11


/*
 * parameters for form_alert()
 */
#define MAX_LINENUM 5
#define MAX_LINELEN 40
#define MAX_BUTNUM  3
#define MAX_BUTLEN  20


/* The following arrays live in RAM */
extern OBJECT  rs_obj[];
extern TEDINFO rs_tedinfo[];

/* This array lives in ROM and points to RAM data */
extern OBJECT * const rs_tree[];

/* The following resource data live in ROM */
extern const char * const rs_fstr[];
extern const BITBLK       rs_fimg[];


extern void gem_rsc_init(void);
extern void gem_rsc_fixit(void);

