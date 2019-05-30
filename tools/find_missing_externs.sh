#!/bin/bash
# Find missing .extern declarations in assembler files.
# Although those declarations are ignored by gas,
# they are useful for documentation.
# Written by Vincent Rivière, 2019.
# License: Public domain

# Check if there are some object files
if [ -z "$(ls obj/*.o 2>/dev/null)" ]
then
  echo "error: Please do \"make ...\" first." >&2
  exit 1
fi

# For each assembler source file
find . -name '*.S' | while read srcfile
do
  ofile=obj/$(basename -s .S  $srcfile).o

  if [ ! -f $ofile ]
  then
    continue
  fi

  # Display differences between actual extern symbols and .extern declarations
  echo $srcfile:
  comm -3 -2 \
    <(m68k-atari-mint-objdump -t $ofile | grep '\*UND\*' | tr -s ' ' | cut -d ' ' -f 6 | sort) \
    <(grep '\.extern' $srcfile | tr -s ' ' | cut -d ' ' -f 3 | sort)
done
