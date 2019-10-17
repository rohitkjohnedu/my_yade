#! /bin/bash

# This check is to be called inside xvfb-run, so that it has a working Xserver in which a simple yade GUI session can be started, the
# testGui.py script is only a slightly modified simple-scene-energy-tracking.py example.
# the screenshots have to be taken inside yade session, while the GUI windows are open.

mkdir -p screenshots

install/bin/yade-ci scripts/checks-and-tests/testGui.py

mv scr*.png screenshots

if [[ ! -f screenshots/scr12.png ]] ; then
    echo 'File "screenshots/scr12.png" is missing, aborting.'
    exit 1
fi

