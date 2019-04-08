===================================================
YADE - Yet Another Dynamic Engine - Python 3 branch
===================================================

This branch is used to migrate yade to python version 3. **It is under development**.

**Installation**

The packages needed are the python3-versions of all packages listed in the official Yade doc. So the installation command becomes:

.. code::

sudo apt install cmake git freeglut3-dev libloki-dev libboost-all-dev fakeroot dpkg-dev build-essential g++ python3-dev python3-ipython python3-matplotlib libsqlite3-dev python3-numpy python3-tk gnuplot libgts-dev python3-pygraphviz libvtk6-dev libeigen3-dev python3-xlib python3-pyqt5 pyqt5-dev-tools python3-pyqt5.qtwebkit gtk2-engines-pixbuf python3-pyqt5.qtsvg libqglviewer-dev-qt5 python3-pil libjs-jquery python3-sphinx python3-git libxmu-dev libxi-dev libcgal-dev help2man libbz2-dev zlib1g-dev python3-minieigen libopenblas-dev libsuitesparse-dev libmetis-dev python3-bibtexparser python-bibtexparser

Installing, running, testing looks now ok, at least for ubuntu 18.04. You can try it with:

.. code::

cmake -DCMAKE_INSTALL_PREFIX=/path/to/your/install/target -DUSE_PYTHON3=ON /path/to/sources

**Implementation status**

- cmake and yade compilation ok with python3
- yade launch ok with python3, generating/launching triax ok
- yade --test all ok
- yade --test all ok with python2 (-DUSE_PYTHON3=OFF)
- yade --checks almost ok with python2 and python3 (but the only issue is not related to python version)

**Known bugs/todos**

- (DONE) update CMakeLists.txt to make python3 and python2 optional (and one of them at least is necessary)
- (FIXED by Vaclav) pygts may not compile
- python3-bibtex package doesn't exist, and underlying _bibtex python module will not be found. As a result, the doc will not compile. We may use another python bibtex parser or use sphinxcontrib-bibtex: https://sphinxcontrib-bibtex.readthedocs.io/en/latest/usage.html
- (PARTIALLY FIXED) fix checks/tests (checks neither work in the master branch)
- (DONE) try/fix python2 retro-compatibility
- try/fix examples
