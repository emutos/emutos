##############################################################################
# EmuTOS Makefile
##############################################################################

TOP = $(shell pwd)

######################## Additional Flags section ############################


# Linker with relocation information and binary output (image)
LD = m68k-atari-mint-ld
LDFLAGS = -Ttext=0xfc0000 -Tdata=0xfd0000 -Tbss=0x000000 -L/usr/lib/gcc-lib/m68k-atari-mint/2.95.3/mshort -lgcc
#LD = ld
#LDFLAGS =

BIOSOBJS =	tosvars.o startup.o  charblit.o \
		kbd.o midi.o kbq.o mfp.o clock.o \
		bios.o conio.o kprint.o xbios.o \
                fnt8x12.o version.o lineavars.o

BDOSOBJS = bdosinit.o bdosts.o console.o fsbuf.o \
	fsdir.o fsdrive.o fsfat.o fsglob.o \
	fshand.o fsio.o fsmain.o fsopnclo.o \
	iumem.o kpgmld.o osmem.o proc.o \
	bdosmain.o time.o umem.o rwa.o


gemdos.img: $(BIOSOBJS) $(BDOSOBJS)
	${LD} -oformat binary -o $@ $? ${LDFLAGS}

$(BIOSOBJS):
	cp ../bios/*.o .

$(BDOSOBJS):
	cp ../bdos/*.o .

clean:
	rm -f gemdos.img *.o *~

