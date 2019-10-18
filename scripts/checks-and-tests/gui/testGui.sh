#!/bin/bash

# This check is to be called inside xvfb-run, so that it has a working Xserver in which a simple yade GUI session can be started, the
# testGui.py script is only a slightly modified simple-scene-energy-tracking.py example.
# the screenshots have to be taken inside yade session, while the GUI windows are open.

echo -e "\n\n=== Will now test inside xterm, all usefull output, including gdb crash backtrace, will be on screenshots ===\n\n"

echo -e "=== is xterm   present? We found this: "`which xterm`  ; ls -la /usr/bin/xterm
echo -e "=== is scrot   present? We found this: "`which scrot`  ; ls -la /usr/bin/scrot
echo -e "=== is xdotool present? We found this: "`which xdotool`; ls -la /usr/bin/xdotool
echo -e "=== is bash    present? We found this: "`which bash`   ; ls -la /bin/bash
echo -e "=== is gdb     present? We found this: "`which gdb `   ; ls -la /usr/bin/gdb

YADE_EXECUTABLE=install/bin/yade-ci
# You can test locally using this script, just change YADE_EXECUTABLE into something that works for you:
#   YADE_EXECUTABLE=./examples/yade
# then launch this command:
#   xvfb-run -a -s "-screen 0 1600x1200x16" scripts/checks-and-tests/gui/testGui.sh
# or just this command:
#   scripts/checks-and-tests/gui/testGui.sh

mkdir -p screenshots

# FIXME: this should be deduced automatically from the files matching pattern testGui*.py, see also testGui.py
declare -a TESTS=( "Empty" "Simple" )

for TestFile in ${TESTS[@]}; do

LOGFILE="screenshots/testGui_${TestFile}.txt"
tail -F ${LOGFILE} &
TAIL_PID=$!

echo -e "******************************************\n*** Testing file testGui${TestFile}.py ***\n******************************************\nLog in file: ${LOGFILE}\ntail pid:${TAIL_PID}\n"

/usr/bin/xterm -l -xrm "XTerm*logFile:${LOGFILE}" -geometry 100x48+5+560  -e /bin/bash -c "${YADE_EXECUTABLE} scripts/checks-and-tests/gui/testGui${TestFile}.py"

# FIXME: the idea is to have a screenshot from outside of yade. But taking a screenshot after it finished (crashed, or by normal exit)
#        will just produce an empty screenshot. It has to be done in a different way.
# scrot -z scrBash01.png

mv scr*.png screenshots

kill -9 ${TAIL_PID}

if [[ ! -f screenshots/scr_${TestFile}_12.png ]] ; then
    echo "File screenshots/scr_${TestFile}_12.png is missing, aborting."
    exit 1
fi

done

