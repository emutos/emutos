# Makefile for GEM-AES

CC = m68k-atari-mint-gcc
AS = m68k-atari-mint-as
LD = m68k-atari-mint-ld

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

CFLAGS = -mshort -I.
# -Wstrict-prototypes -Werror
LDFLAGS = -L/opt/st-cross/lib -lc16 \
          -L/opt/st-cross/lib/gcc-lib/m68k-atari-mint/2.95.2/mshort -lgcc

C_OBS= gemaplib.o gemasync.o gembase.o gemctrl.o gemdisp.o gemevlib.o \
   gemflag.o gemfmalt.o gemfmlib.o gemfslib.o gemglobe.o gemgraf.o gemgrlib.o \
   gemgsxif.o geminit.o geminput.o gemmnlib.o gemobed.o gemobjop.o gemoblib.o \
   gempd.o gemqueue.o gemrslib.o gemsclib.o gemshlib.o gemsuper.o gemwmlib.o \
   gemwrect.o optimize.o rectfunc.o gemdos.o

S_OBS= gemdosif.o gemasm.o gsx2.o large.o optimopt.o

aes: $(C_OBS) $(S_OBS) gemstart.o
	$(LD) gemstart.o $(S_OBS) $(C_OBS) $(LDFLAGS) -o aes.prg


clean:
	$(RM) *.o aes.prg

show: aes.prg
	$(OBJDUMP) --architecture=m68k -D aes.prg
