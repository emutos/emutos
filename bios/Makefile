
# Setup for crosscompiling under Linux only at this time :-(


# Assembler with options for Motorola like syntax (68000 cpu)
AS = m68k-atari-mint-as
ASFLAGS = --register-prefix-optional -m68000

# C compiler for MiNT
CC = m68k-atari-mint-gcc
CFLAGS = -c -O -mshort -I./include -Wall
#CC = gcc
#CFLAGS = -c -O -I./include -Wall

# Archiver with options for Motorola like syntax (68000 cpu)
AR = m68k-atari-mint-ar
ARFLAGS = rs
# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump


#OBJECTS	=	biosc.o disk.o sioerr.o siomain.o \
#		sioque.o ikb.o kbq.o vt52.o \
#                conio.o siochr.o sioint.o siophysc.o \
#                ikba.o ikba.o biosa.o mouse.o \
#                siophysa.o startup.o

OBJECTS	=	startup.o tosvars.o charblit.o \
		kbd.o midi.o kbq.o mfp.o clock.o \
		bios.o conio.o kprint.o xbios.o \
                fnt8x12.o version.o lineavars.o

all:	libbios.a

libbios.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^


bios.o:
	${CC} ${CFLAGS} -c bios.c

xbios.o:
	${CC} ${CFLAGS} -c xbios.c

kbq.o:
	${CC} ${CFLAGS} -c kbq.c

kprint.o:
	${CC} ${CFLAGS} -c kprint.c

version.o: version.c
	${CC} ${CFLAGS} -c $?

kbd.o: kbd.c
	${CC} ${CFLAGS} -c $?

conio.o: conio.c
	${CC} ${CFLAGS} -c $?

clock.o: clock.c
	${CC} ${CFLAGS} -c $?

mfp.o: mfp.c
	${CC} ${CFLAGS} -c $?

charblit.o: charblit.c
	${CC} ${CFLAGS} -c $?

fnt8x12.o: fnt8x12.c
	${CC} ${CFLAGS} -c $?


# ==== Now all the assembler stuff ==========================================
tosvars.o: tosvars.s
	$(AS) $(ASFLAGS) $? -o $@

gsxvars.o: gsxvars.s
	$(AS) $(ASFLAGS) $? -o $@

startup.o: startup.s
	$(AS) $(ASFLAGS) $? -o $@


clean:
	rm -f *.o *~ lib*.a core
