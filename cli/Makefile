# Makefile for EmuCON2

TARGET = emucon2.tos
LIBS = -nostartfiles -nostdlib -lgcc
CC = m68k-atari-mint-gcc
CPPFLAGS = -I../include -DUSE_STATIC_INLINES=0 -DSTANDALONE_CONSOLE
CFLAGS = $(CPPFLAGS) -mshort -O -Wall -Wundef -Wold-style-definition

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = cmdasm.o cmdmain.o cmdedit.o cmdexec.o cmdint.o cmdparse.o cmdutil.o
HEADERS = cmd.h

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
