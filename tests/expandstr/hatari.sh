#!/bin/sh
# Copyright (C) 2026 The EmuTOS development team
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.

echo "AES over-long client string test (with Hatari):"

if ! command -v hatari >/dev/null 2>&1; then
    echo "ERROR: You must install hatari to run this test."
    exit 1
fi

if [ -z "$EMUTOS" ]; then
    export EMUTOS=../../etos1024k.img
fi

export SDL_VIDEODRIVER=dummy
export SDL_AUDIODRIVER=dummy

rm -f EXPSTR.OUT

outtxt=$(mktemp)
hatari --log-level fatal --sound off --fast-forward on --run-vbls 2000 \
    --fast-boot on --tos "$EMUTOS" -d . --machine st --cpulevel 0 \
    >"$outtxt" 2>&1
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to run hatari:"
    cat "$outtxt"
    rm "$outtxt"
    exit 1
fi
rm "$outtxt"

if [ ! -f EXPSTR.OUT ]; then
    echo "ERROR: EXPSTR.OUT not created (EmuTOS crashed?)"
    exit 1
fi

if ! grep -q "RESULT: PASS" EXPSTR.OUT; then
    echo "ERROR: test did not pass:"
    cat EXPSTR.OUT
    exit 1
fi

rm -f EXPSTR.OUT

echo "OK"
