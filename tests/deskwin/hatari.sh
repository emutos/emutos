#!/bin/sh
# Copyright (C) 2026 The EmuTOS development team
#
# This file is distributed under the GPL, version 2 or at your
# option any later version.  See doc/license.txt for details.

echo "AES desktop window leak test (with Hatari):"

if ! command -v hatari >/dev/null 2>&1; then
    echo "ERROR: You must install hatari to run this test."
    exit 1
fi

# unlike the other tests we need to press a key inside the emulator, which
# means driving Hatari from hconsole rather than just starting it
if ! command -v python3 >/dev/null 2>&1; then
    echo "Skipped (python3 not installed)."
    exit 0
fi

HCONSOLE=
for dir in /usr/share/hatari/hconsole /usr/local/share/hatari/hconsole \
           /usr/share/doc/hatari/hconsole /opt/local/share/hatari/hconsole \
           /opt/homebrew/share/hatari/hconsole ; do
    if [ -f "$dir/hconsole.py" ]; then
        HCONSOLE="$dir/hconsole.py"
        break
    fi
done
if [ -z "$HCONSOLE" ]; then
    echo "Skipped (hconsole not installed)."
    exit 0
fi

if [ -z "$EMUTOS" ]; then
    export EMUTOS=../../etos1024k.img
fi

export SDL_VIDEODRIVER=dummy
export SDL_AUDIODRIVER=dummy

rm -f WINCHK.OUT

#
# scancode 59 is F1, which EMUDESK.INF assigns to WINCHK.TOS.
#
# the delays are wall clock seconds, not emulated time: the desktop must be
# fully up before we press F1, and WINCHK.TOS must have run to completion
# before we kill the emulator.  --fast-forward speeds up emulation so these
# can be shorter than real boot/run time would need; still generous enough
# to tolerate a loaded CI box.
#
cmds=$(mktemp)
cat > "$cmds" <<'EOF'
sleep 6
keypress 59
sleep 4
kill
EOF

outtxt=$(mktemp)
python3 "$HCONSOLE" "$cmds" --exit -- \
    --tos "$EMUTOS" -d . --machine st --cpulevel 0 \
    --log-level fatal --sound off --fast-boot on --fast-forward on --confirm-quit off \
    >"$outtxt" 2>&1
rc=$?
rm -f "$cmds"
if [ $rc -ne 0 ]; then
    echo "ERROR: Failed to run hatari via hconsole:"
    cat "$outtxt"
    rm -f "$outtxt"
    exit 1
fi
rm -f "$outtxt"

if [ ! -f WINCHK.OUT ]; then
    echo "ERROR: WINCHK.OUT not created (EmuTOS crashed, or F1 was not delivered?)"
    exit 1
fi

if ! grep -q "RESULT: PASS" WINCHK.OUT; then
    echo "ERROR: test did not pass:"
    cat WINCHK.OUT
    exit 1
fi

rm -f WINCHK.OUT

echo "OK"
