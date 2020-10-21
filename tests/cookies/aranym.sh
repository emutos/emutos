#!/bin/sh
# Copyright (C) 2019 The EmuTOS development team
#
# Authors:
#  THH   Thomas Huth
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.

echo "Cookie jar test (with Aranym):"

if ! command -v aranym >/dev/null 2>&1; then
    echo "ERROR: You must install aranym to run this test."
    exit 1
fi

if ! command -v mcopy >/dev/null 2>&1; then
    echo "ERROR: You must install mtools to run this test."
    exit 1
fi

if [ -z "$EMUTOS" ]; then
    export EMUTOS=../../emutos-aranym.img
fi

export SDL_VIDEODRIVER=dummy
export SDL_AUDIODRIVER=dummy

check_cookie() {
    if ! grep -q "$1 : $2" COOKIES.TXT ; then
        echo "ERROR: Cookie $1 does not have value $2."
        exit 1
    fi
}

set -e

disk=disk.st

trap "rm -rf $disk AUTO output.txt aranym.cfg COOKIES.TXT" INT TERM HUP EXIT

rm -f COOKIES.TXT
dd if=/dev/zero of=$disk bs=1024 count=720 status=none
mformat -a -f 720 -i $disk ::
mkdir AUTO
cp cj2txt.tos AUTO/CJ2TXT.PRG
mcopy -i $disk -s AUTO ::

echo '[GLOBAL]' > aranym.cfg
echo "Floppy = $disk" >> aranym.cfg
echo "EmuTOS = $EMUTOS" >> aranym.cfg

echo -n "- Checking Aranym settings ... "
aranym -c aranym.cfg 2> output.txt || \
    ( echo "ERROR:" ; cat output.txt ; rm output.txt ; exit 1 )
mcopy -i $disk ::/COOKIES.TXT .

if [ ! -f COOKIES.TXT ]; then
    echo "ERROR: COOKIES.TXT has not been created."
    exit 1
fi

check_cookie "_CPU" "0x00000028"
check_cookie "_VDO" "0x00030000"
check_cookie "_FPU" "0x00080000"
check_cookie "_MCH" "0x00050000"
echo "OK"

rm -f COOKIES.TXT

echo "All done."

exit 0
