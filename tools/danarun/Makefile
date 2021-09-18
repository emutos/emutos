tools/danarun/danarun.prc: obj/danarun.exe Makefile
	rm -f obj/*.danarun.exe.grc
	(cd obj && m68k-palmos-obj-res danarun.exe)
	build-prc \
		--output $@ \
		--name "DanaRun" \
		--creator DNRN \
		obj/*.danarun.exe.grc

obj/danarun.exe: tools/danarun/danarun.c obj/palmboot.h
	m68k-palmos-gcc -Os -g $< -o $@

obj/palmboot.o: tools/danarun/palmboot.S
	m68k-atari-mint-as -g $< -o $@

obj/palmboot.img: obj/palmboot.o
	m68k-atari-mint-ld --oformat binary $< -o $@

obj/palmboot.h: obj/palmboot.img
	xxd -i < $< > $@

