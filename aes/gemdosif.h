
#ifndef GEMDOSIF_H
#define GEMDOSIF_H


extern LONG     drwaddr;

extern LONG     tikaddr;
extern LONG     tiksav;

extern LONG     NUM_TICK;                       /* number of ticks      */
                                                /*   since last sample  */
                                                /*   while someone was  */
                                                /*   waiting            */
extern LONG     CMP_TICK;                       /* indicates to tick    */
                                                /*   handler how much   */
                                                /*   time to wait before*/
                                                /*   sending the first  */
                                                /*   tchange            */


extern void cli();
extern void sti();

extern WORD far_bcha();
extern WORD far_mcha();
extern WORD justretf();


#endif
