#!/bin/bash
# Determine the BSS usage of each subdirectory
# Originally written by Vincent Rivière, 2014.
# License: Public domain

total=0

for dir in aes bdos bios cli desk util vdi
do
  # Find all the .o corresponding to the sources of the directory (may exist or not)
  objects=$(find $dir -name '*.c' -or -name '*.S' |sed -n 's:[^/]*/\([^.]*\).*:obj/\1.o:p')

  # Compute the sum of the sizes of the .bss sections
  bsssize=$(( $(m68k-atari-mint-objdump -h $objects 2>/dev/null |grep '\.bss' |awk '{print "+ 0x" $3}' |xargs) ))

  # Compute the sum of the sizes of all the COMMON symbols
  # This is not 100% accurate because we miss a few bytes of alignments and fillers
  commonsize=$(( $(m68k-atari-mint-objdump -t $objects 2>/dev/null |grep '\*COM\*' |awk '{print " + 0x" $1}' |xargs) ))

  # The final .bss segment contains all the .bss and COMMON sections
  dirsize=$(($bsssize + $commonsize))
  ((total += $dirsize))

  echo -e "$dir \\t $dirsize"
done

echo -e "----------------"
echo -e "Total: \\t $total"
