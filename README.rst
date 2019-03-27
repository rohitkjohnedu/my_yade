===================================================
YADE - Yet Another Dynamic Engine - Python 3 branch
===================================================

This branch is used to migrate yade to python version 3. **It is under development, currently not working**.

**Installation**

The packages needed are the python3-versions of all packages listed in the official Yade doc. So the installation command becomes:

.. code::

sudo apt install cmake git freeglut3-dev libloki-dev libboost-all-dev fakeroot dpkg-dev build-essential g++ python3-dev ipython python3-matplotlib libsqlite3-dev python3-numpy python3-tk gnuplot libgts-dev python3-pygraphviz libvtk6-dev libeigen3-dev python3-xlib python3-pyqt5 pyqt5-dev-tools python3-pyqt5.qtwebkit gtk2-engines-pixbuf python3-pyqt5.qtsvg libqglviewer-dev-qt5 python3-pil libjs-jquery python3-sphinx python3-git libxmu-dev libxi-dev libcgal-dev help2man libbz2-dev zlib1g-dev python3-minieigen libopenblas-dev libsuitesparse-dev libmetis-dev

**Known bugs/todos**

- update CMakeLists.txt to make python3 and python2 optional (and one of them at least is necessary)
- pygts may not compile
- python3-bibtex package doesn't exist, and underlying _bibtex python module will not be found. As a result, the doc will not compile. We may use another python bibtex parser or use sphinxcontrib-bibtex: https://sphinxcontrib-bibtex.readthedocs.io/en/latest/usage.html
