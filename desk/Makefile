# Makefile for GEM-DESKTOP

CC = m68k-atari-mint-gcc
AS = m68k-atari-mint-as

# The objdump utility (disassembler)
OBJDUMP=m68k-atari-mint-objdump

LDFLAGS = -m68000 -mshort
CFLAGS =  -O2 -fomit-frame-pointer -I. -Iicons -I../aes -I../include \
    -I../bios -mshort -m68000 -DNO_ROM -Wall #-Werror

C_SRC = deskact.c deskfpd.c deskgraf.c deskinf.c deskpro.c deskmain.c \
    deskapp.c deskfun.c deskgsx.c deskins.c deskrsrc.c deskwin.c  \
    deskdir.c deskglob.c deskobj.c desksupp.c desk_rsc.c gembind.c icons.c
    #rectfunc.c gemdos.c optimize.c string.c

C_OBS= $(C_SRC:%.c=%.o)

GEM_OBS= ../obj/gemdos.o ../obj/string.o ../obj/rectfunc.o ../obj/optimize.o \
    ../obj/gsx2.o ../obj/optimopt.o ../obj/large.o ../obj/memmove.o

desktop.app: $(C_OBS) $(S_OBS) deskstart.o
	$(CC) -nostartfiles -nostdlib -o desktop.app \
          deskstart.o $(S_OBS) $(C_OBS) $(GEM_OBS) $(LDFLAGS) -lgcc

deskstart.o: deskstart.S
	$(CC) $(CFLAGS) -c deskstart.S -o deskstart.o

clean:
	$(RM) *.o *~ desktop.app

show: desktop.app
	$(OBJDUMP) --architecture=m68k -D desktop.app
