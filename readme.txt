Installing:

You will need the (cross-)gcc toolchain from the sparemint pages to
build this stuff. 

Also you need to get it (more or less) running is STonX. But you have
to do some changes in STonX. Possibly this will change later.

EmuTOS now needs no change in STonX any longer. It will run on a
normal STonX. 

But, if you want to see debugging output in STonX, you need to
implement a write_native function in gemdos.c of STonX:

/* Print string from emulated side */
void write_native(char * addr)
{
    char buf[1024];
    int n;

    buf[0]=0;   /* to be save */
    buf[80]=0;   /* to be save */

    for(n=0;n<1024;n++) {       /* Fill string char by char */
        if ((buf[n]=LM_UB(MEM(addr++))) == 0) break;
    }
    fprintf(stderr,"%s", buf );
}

void call_native(UL as, UL func)
{
#if 0
	fprintf(stderr,"Calling native %d\n",func);
#endif
	switch (func)
	{
        case 0: /* Get the address, where string begins*/
		write_native((char *)LM_UL(MEM(as)));
		break;
	case 1:
.....


You will find no compiled command.prg in this archive. The source of it
now is ok (it works a bit). It is in the cli sub-directory. It may have
lots of bugs and debugging code in it!


Hope this is all. AND: EmuTOS is not thought for the public right now.
It is alpha alpha!!!
