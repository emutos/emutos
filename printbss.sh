#!/bin/bash
# Determine the BSS usage of each subdirectory
# Written by Vincent Rivière, 2014.
# License: Public domain

total=0

for dir in aes bdos bios cli desk util vdi
do
  # Find all the .o corresponding to the sources of the directory (may exist or not)
  objects=$(find $dir -name '*.c' -or -name '*.S' |sed -n 's:[^/]*/\([^.]*\).*:obj/\1.o:p')

  # Get the total size of the .bss and COMMON sections
  dirsize=$(m68k-atari-mint-size -t --common $objects 2>/dev/null |grep '(TOTALS)' |awk '{print $3}')

  ((total += $dirsize))

  echo -e "$dir \\t $dirsize"
done

echo -e "----------------"
echo -e "Total: \\t $total"
