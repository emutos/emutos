
#ifndef GEMDOSIF_H
#define GEMDOSIF_H


extern LONG     drwaddr;

extern void *   tikaddr;
extern void *   tiksav;

extern LONG     NUM_TICK;                       /* number of ticks      */
                                                /*   since last sample  */
                                                /*   while someone was  */
                                                /*   waiting            */
extern LONG     CMP_TICK;                       /* indicates to tick    */
                                                /*   handler how much   */
                                                /*   time to wait before*/
                                                /*   sending the first  */
                                                /*   tchange            */


extern void cli(void);
extern void sti(void);

extern void hcli(void);
extern void hsti(void);

extern WORD far_bcha(void);
extern WORD far_mcha(void);
extern WORD justretf(void);

extern void givecpm(void);
extern void takecpm(void);

extern void takeerr(void);
extern void giveerr(void);
extern void retake(void);

extern void drawrat(WORD newx, WORD newy);


#endif
