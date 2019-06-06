
CC = m68k-atari-mint-gcc
CFLAGS = -Wall -mshort -I../include

all: bconio1.tos

bconio1.tos: bconio.c
	$(CC) $(CFLAGS) bconio.c -DCHANNEL=1 -o bconio1.tos

clean:
	$(RM) bconio*.tos in.dat out.dat

.PHONY : test
test: all
	@if command -v hatari >/dev/null 2>&1; then \
		./hatari.sh || exit 1; \
	else \
		echo "Skipped cookie test with Hatari (not installed)."; \
	fi
