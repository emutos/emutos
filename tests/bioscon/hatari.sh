#!/bin/sh
# Copyright (C) 2019 The EmuTOS development team
#
# Authors:
#  THH   Thomas Huth
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.

echo "Bioscon test:"

if ! command -v hatari >/dev/null 2>&1; then
    echo "ERROR: You must install hatari to run this test."
    exit 1
fi

if [ -z "$EMUTOS" ]; then
    export EMUTOS=../../etos512k.img
fi

export SDL_VIDEODRIVER=dummy
export SDL_AUDIODRIVER=dummy

run_hatari() {
    outtxt=$(mktemp)
    hatari --log-level fatal --sound off --fast-forward on --run-vbls 2000 \
        --fast-boot on --tos "$EMUTOS" -d . "$@"  >"$outtxt" 2>&1
    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to run hatari:"
        cat "$outtxt"
        rm "$outtxt"
        exit 1
    fi
    rm "$outtxt"
}

dd if=/dev/urandom of=in.dat bs=200 count=1 status=none

echo -n "- Checking Bconin/out 1 on ST ... "
rm -f out.dat
run_hatari --machine st --cpulevel 0 --rs232-in in.dat --rs232-out out.dat
if ! diff -q in.dat out.dat ; then
    echo "ERROR: output does not match input."
    exit 1
fi
echo "OK"

rm -f in.dat out.dat
echo "All done."
