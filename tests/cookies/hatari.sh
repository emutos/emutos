#!/bin/sh
# Copyright (C) 2019 The EmuTOS development team
#
# Authors:
#  THH   Thomas Huth
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.

echo "Cookie jar test (with Hatari):"

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
    rm -f COOKIES.TXT
    outtxt=$(mktemp)
    hatari --log-level fatal --sound off --fast-forward on --run-vbls 400 \
        --fast-boot on --natfeats on --tos "$EMUTOS" -d . "$@" >"$outtxt" 2>&1
    if [ $? -ne 0 ]; then
        echo "ERROR: Failed to run hatari:"
        cat "$outtxt"
        rm "$outtxt"
        exit 1
    fi
    rm "$outtxt"
    if [ ! -f COOKIES.TXT ]; then
        echo "ERROR: COOKIES.TXT has not been created."
        exit 1
    fi
}

check_cookie() {
    if ! grep -q "$1 : $2" COOKIES.TXT ; then
        echo "ERROR: Cookie $1 does not have value $2."
        exit 1
    fi
}

echo -n "- Checking ST ... "
run_hatari --machine st --cpulevel 0
check_cookie "_CPU" "0x00000000"
check_cookie "_VDO" "0x00000000"
check_cookie "_FPU" "0x00000000"
check_cookie "_MCH" "0x00000000"
check_cookie "_SND" "0x00000001"
echo "OK"

echo -n "- Checking STE ... "
run_hatari --machine ste --cpulevel 1
check_cookie "_CPU" "0x0000000a"
check_cookie "_VDO" "0x00010000"
check_cookie "_FPU" "0x00000000"
check_cookie "_MCH" "0x00010000"
check_cookie "_SND" "0x00000003"
echo "OK"

echo -n "- Checking TT ... "
run_hatari --machine tt --cpulevel 3
check_cookie "_CPU" "0x0000001e"
check_cookie "_VDO" "0x00020000"
check_cookie "_MCH" "0x00020000"
check_cookie "_SND" "0x00000003"
echo "OK"

echo -n "- Checking Falcon ... "
run_hatari --machine falcon --cpulevel 4
check_cookie "_CPU" "0x00000028"
check_cookie "_VDO" "0x00030000"
check_cookie "_MCH" "0x00030000"
echo "OK"

# Mega-ST and Mega-STE are only available with newer versions of Hatari
if ! hatari --version | grep -q "Hatari v1"; then
    echo -n "- Checking Mega-ST ... "
    run_hatari --machine megast --cpulevel 2 --fpu 68881
    check_cookie "_CPU" "0x00000014"
    check_cookie "_VDO" "0x00000000"
    check_cookie "_FPU" "0x00040000"
    check_cookie "_MCH" "0x00000000"
    check_cookie "_SND" "0x00000001"
    echo "OK"

    echo -n "- Checking Mega-STE ... "
    run_hatari --machine megaste --cpulevel 6 --fpu internal
    check_cookie "_CPU" "0x0000003c"
    check_cookie "_VDO" "0x00010000"
    check_cookie "_FPU" "0x00100000"
    check_cookie "_MCH" "0x00010010"
    check_cookie "_SND" "0x00000003"
    echo "OK"
fi

rm -f COOKIES.TXT

echo "All done."
