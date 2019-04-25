===================================
YADE - Yet Another Dynamic Engine
===================================

Extensible open-source framework for discrete numerical models, focused on 
Discrete Element Method. The computation parts are written in c++ using flexible
object model, allowing independent implementation of new algorithms and interfaces. 
Python is used for rapid and concise scene construction, simulation control, 
postprocessing and debugging.

**Documentation:**

- https://www.yade-dem.org/doc/ (includes c++ and python reference)
- https://yade-dem.org/doc/Yade.pdf (PDF-file with documentation)
- https://yade-dem.org/doc/Yade.epub (EPUB-file with documentation)
 
**Downloads:**

- Packages for Ubuntu and Debian:
  
  - https://yade-dem.org/doc/installation.html#packages
  - External packages: https://launchpad.net/~yade-users/+archive/external

- Tarballs of different versions: 

  - https://gitlab.com/yade-dev/trunk/tags
  - Repository on gitlab.com: https://gitlab.com/yade-dev/trunk/-/archive/master/trunk-master.tar.gz
  - Repository on git.debian.org: http://git.debian.org/?p=debian-science/packages/yade.git

**Teams involved:**

- General users: https://launchpad.net/~yade-users
- Developers and advanced users: https://launchpad.net/~yade-dev
- Developers for distributed memory parallelization of Yade: https://launchpad.net/~yade-mpi
- Infrastructure maintainers: https://launchpad.net/~yade-pkg

**Miscellaneous:**

- `Wiki <https://yade-dem.org/wiki/>`_
- `Screenshots <https://www.yade-dem.org/wiki/Screenshots_and_videos>`_
- `Tutorial <https://yade-dem.org/doc/tutorial-examples.html>`_
- `Videos <https://yade-dem.org/doc/tutorial-more-examples-fast.html>`_
- `Yade page on Open Hub <http://www.openhub.net/p/yade/>`_
- `Yade on Debian PTS <http://packages.qa.debian.org/y/yade.html>`_
- `Yade on Debian salsa <https://salsa.debian.org/science-team/yade/commits/master>`_
- `Yade in Ubuntu <https://launchpad.net/ubuntu/+source/yade>`_
- `Launchpad page <http://www.launchpad.net/yade>`_


=========================================================================================
YADE - Yet Another Dynamic Engine - Python 3 branch - remove this when merge is complete.
=========================================================================================

The packages needed are the python3-versions of all packages listed in the official Yade doc. So the installation command becomes:

.. code::

sudo apt install cmake git freeglut3-dev libloki-dev libboost-all-dev fakeroot dpkg-dev build-essential g++ python3-dev python3-ipython python3-matplotlib libsqlite3-dev python3-numpy python3-tk gnuplot libgts-dev python3-pygraphviz libvtk6-dev libeigen3-dev python3-xlib python3-pyqt5 pyqt5-dev-tools python3-pyqt5.qtwebkit gtk2-engines-pixbuf python3-pyqt5.qtsvg libqglviewer-dev-qt5 python3-pil libjs-jquery python3-sphinx python3-git libxmu-dev libxi-dev libcgal-dev help2man libbz2-dev zlib1g-dev python3-minieigen libopenblas-dev libsuitesparse-dev libmetis-dev python3-bibtexparser python3-future


On ubuntu 16.04 python3-ipython is to be replaced by ipython3, and libqglviewer-dev-qt5 by libqglviewer-dev.

Additional dependency for backward python 2.7 compatibility: python-future (in addition to all python-xxx packages).

Installing, running, testing, doc look now ok, on all current and future debian/ubuntu versions. You can try it with:

.. code::

cmake -DCMAKE_INSTALL_PREFIX=/path/to/your/install/target -DPYTHON_VERSION=3 /path/to/sources

**Implementation status**

- cmake and yade compilation ok with python3
- yade launch ok with python3, generating/launching triax ok
- yade --test all ok with python3 (-DPYTHON_VERSION=3)
- yade --test all ok with python2 (-DPYTHON_VERSION=2)
- yade --checks almost ok with python2 and python3 (but the only issue is not related to python version)

**Known bugs/todos**

- (DONE) update CMakeLists.txt to make python3 and python2 optional (and one of them at least is necessary)
- (FIXED by Vaclav) pygts may not compile
- (FIXED) python3-bibtex package doesn't exist, and underlying _bibtex python module will not be found. As a result, the doc will not compile. We may use another python bibtex parser or use sphinxcontrib-bibtex: https://sphinxcontrib-bibtex.readthedocs.io/en/latest/usage.html
- (PARTIALLY FIXED) fix checks/tests (checks neither work in the master branch)
- (DONE) try/fix python2 retro-compatibility
- try/fix examples


