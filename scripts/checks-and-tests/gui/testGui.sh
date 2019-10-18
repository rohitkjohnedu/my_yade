#! /bin/bash

# This check is to be called inside xvfb-run, so that it has a working Xserver in which a simple yade GUI session can be started, the
# testGui.py script is only a slightly modified simple-scene-energy-tracking.py example.
# the screenshots have to be taken inside yade session, while the GUI windows are open.

echo -e "\n\n=== Will now test inside xterm, all usefull output, including gdb crash backtrace, will be on screenshots ===\n\n"

mkdir -p screenshots

# FIXME: this should be deduced automatically from the files matching pattern testGui*.py, see also testGui.py
TESTS=( "Empty" "Simple" )

for TestFile in ${TESTS}; do

xterm -geometry 100x48+5+560  -e bash -c "install/bin/yade-ci scripts/checks-and-tests/gui/testGui${TestFile}.py"

# FIXME: the idea is to have a screenshot from outside of yade. But taking a screenshot after it finished (crashed, or by normal exit)
#        will just produce an empty screenshot. It has to be done in a different way.
# scrot -z scrBash01.png

mv scr*.png screenshots

if [[ ! -f screenshots/scr12_${TestFile}.png ]] ; then
    echo "File screenshots/scr12_${TestFile}.png is missing, aborting."
    exit 1
fi

done

