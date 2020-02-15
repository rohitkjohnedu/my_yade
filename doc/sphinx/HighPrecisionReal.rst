.. _highPrecisionReal:

***************************
High precision calculations
***************************

Yade supports high and arbitrary precision ``Real`` type for performing calculations. All tests and checks pass but still the current support is considered experimental.
The backend library is `boost <https://github.com/boostorg/multiprecision>`__ `multiprecision <https://www.boost.org/doc/libs/1_72_0/libs/multiprecision/doc/html/index.html>`__
along with corresponding `boost <https://github.com/boostorg/math>`__ `math toolkit <https://www.boost.org/doc/libs/1_72_0/libs/math/doc/html/index.html>`__.

The supported types are following:

=============================================== =============== =============================== ==================================================================
type						bits		decimal places			notes
=============================================== =============== =============================== ==================================================================
  ``float``					``32``		``6``				hardware accelerated (not useful, it is only for testing purposes)
  ``double``					``64``		``15``				hardware accelerated
  ``long double``				``80``		``18``				hardware accelerated
  ``boost float128``				``128``		``33``				depending on processor type it may be hardware accelerated, `wrapped by boost <https://www.boost.org/doc/libs/1_72_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/float128.html>`__
  ``boost mpfr``				``Nbit``	``Nbit/(log(2)/log(10))``	uses external mpfr library, `wrapped by boost <https://www.boost.org/doc/libs/1_72_0/libs/multiprecision/doc/html/boost_multiprecision/tut/floats/mpfr_float.html>`__
  ``boost cpp_bin_float``			``Nbit``	``Nbit/(log(2)/log(10))``	uses boost only, but is slower
=============================================== =============== =============================== ==================================================================

The last two types are arbitrary precision, and their number of bits ``Nbit`` or decimal places is specified as argument during compilation.

.. note::
	See file :ysrc:`Real.hpp<lib/high-precision/Real.hpp>` for details. All ``Real`` types pass the :ysrccommit:`real type concept<1b4ae97583bd8a6efc74cb0d0/py/high-precision/_math.cpp#L197>` test from `boost concepts <https://www.boost.org/doc/libs/1_72_0/libs/math/doc/html/math_toolkit/real_concepts.html>`__.

Installation
===========================================

The precompiled Yade package uses ``double`` type by default. In order to use high precision type Yade has to be compiled and installed from source code by following the
regular :ref:`installation instructions <install-from-source-code>`. With extra following caveats:

1. Additional packages are required to be installed: ``python-mpmath`` ``libmpfr-dev`` ``libmpfrc++-dev`` (the ``mpfr`` related
   packages are necessary only to use ``boost::multiprecision::mpfr`` type).

.. comment: The debian/ubuntu package ``python-minieigen`` must be of version 0.50.4 or higher. This package hasn't been prepared yet.

2. The high precision `merge requests to minieigen <https://github.com/eudoxos/minieigen>`__ package were not submitted yet (they will be very soon), hence currently following solutions are possible:

	1. Patch the older ``python-minieigen`` debian package with `these patches <https://gitlab.com/cosurgi/minieigen-real/-/tree/master/patches>`__, the patches already included in the debian package are also required.
	2. Or copy `the patched minieigen files <https://gitlab.com/cosurgi/minieigen-real/tree/master/minieigen-local>`__ into local directory and pass the extra cmake argument ``MINIEIGEN_INCLUDE_PATH=…``, relative to this path the sources should reside inside ``minieigen`` subdirectory. The path ``/usr/include/minieigen`` is used by default to find the minieigen sources.

3. A g++ compiler version 9.2.1 or higher is required. It shall be noted that upgrading only the compiler on an existing linux installation is difficult and it is not recommended. A simpler solution is to upgrade entire linux installation.

4. During cmake invocation specify:

	1. either number of bits as ``REAL_PRECISION_BITS=……``,
	2. or number of requested decimal places as ``REAL_DECIMAL_PLACES=……``, but not both
	3. to use MPFR specify ``ENABLE_MPFR=ON``.

   The arbitrary precision (``mpfr`` or ``cpp_bin_float``) types are used only when more than 128 bits or more than 39 decimal places are requested. In such case if ``ENABLE_MPFR=OFF`` then
   the slower ``cpp_bin_float`` type is used. The difference in decimal places between 39 and 33 stems from the fact that `15 bits are used for exponent <https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format>`__.

.. FIXME : MINIEIGEN_PATH is not used by cmake currently


.. _supported-hp-modules:

Supported modules
===========================================

During :ref:`compilation <yadeCompilation>` several Yade modules can be enabled or disabled by passing an ``ENABLE_*`` command line argument to cmake.
The following table lists which modules are currently working with high precision (those marked with "maybe" were not tested):

=========================================== ============ ============================= ========================
``ENABLE_*`` module name                    HP support   cmake default setting         notes
=========================================== ============ ============================= ========================
``ENABLE_GUI``                              yes          ``ON``                        native support [#supp1]_
``ENABLE_CGAL``                             yes          ``ON``                        native support [#supp1]_
``ENABLE_VTK``                              yes          ``ON``                        supported [#supp3]_
``ENABLE_OPENMP``                           conditional  ``ON if Nbits<=128``          conditionally supported [#supp5]_
``ENABLE_MPI``                              maybe        ``OFF``                       not tested [#supp6]_
``ENABLE_GTS``                              yes          ``ON``                        supported [#supp2]_
``ENABLE_GL2PS``                            yes          ``ON``                        supported [#supp2]_
``ENABLE_LINSOLV``                          no           ``OFF``                       not supported [#supp7]_
``ENABLE_PFVFLOW``                          no           ``OFF``                       not supported [#supp7]_
``ENABLE_TWOPHASEFLOW``                     no           ``OFF``                       not supported [#supp7]_
``ENABLE_THERMAL``                          no           ``OFF``                       not supported [#supp7]_
``ENABLE_LBMFLOW``                          yes          ``ON``                        supported [#supp2]_
``ENABLE_SPH``                              maybe        ``OFF``                       not tested [#supp9]_
``ENABLE_LIQMIGRATION``                     maybe        ``OFF``                       not tested [#supp9]_
``ENABLE_MASK_ARBITRARY``                   maybe        ``OFF``                       not tested [#supp9]_
``ENABLE_PROFILING``                        maybe        ``OFF``                       not tested [#supp9]_
``ENABLE_POTENTIAL_BLOCKS``                 no           ``OFF``                       not supported [#supp8]_
``ENABLE_POTENTIAL_PARTICLES``              yes          ``ON``                        supported [#supp4]_
``ENABLE_DEFORM``                           maybe        ``OFF``                       not tested [#supp9]_
``ENABLE_OAR``                              maybe        ``OFF``                       not tested [#supp9]_
``ENABLE_FEMLIKE``                          yes          ``ON``                        supported [#supp2]_
``ENABLE_ASAN``                             yes          ``OFF``                       supported [#supp2]_
``ENABLE_MPFR``                             yes          ``OFF``                       native support [#supp1]_
=========================================== ============ ============================= ========================

The unsupported modules are automatically disabled during the cmake stage.

.. rubric:: Footnotes

.. [#supp1] This feature is supported natively, which means that specific numerical traits were written :ysrc:`for Eigen<lib/high-precision/EigenNumTraits.hpp>` and :ysrc:`for CGAL<lib/high-precision/CgalNumTraits.hpp>`, as well as :ysrc:`GUI<gui/qt5/SerializableEditor.py>` and :ysrc:`python support<lib/high-precision/ToFromPythonConverter.hpp>` was added.

.. [#supp3] VTK is supported via the :ysrc:`compatibility layer <lib/compatibility/VTKCompatibility.hpp>` which converts all numbers down to ``double`` type. See :ref:`below <vtk-real-compatibility>`.

.. [#supp5] Experimentally supported for ``long double`` and ``float128``. Not supported for ``mpfr`` and ``cpp_bin_float``. File :ysrc:`lib/base/openmp-accu.hpp` needs further testing. If in doubt, compile yade with ``ENABLE_OPENMP=OFF``.

.. [#supp6] MPI support has not been tested and sending data over network hasn't been tested yet.

.. [#supp2] The module was tested, the ``yade --test`` and ``yade --check`` pass, as well as most of examples are working. But it hasn't been tested extensively for all possible use cases.

.. [#supp7] Not supported, the code uses external cholmod library which supports only ``double`` type. To make it work a native Eigen solver for linear equations should be used.

.. [#supp9] This feature is ``OFF`` by default, the support of this feature has not been tested.

.. [#supp8] Potential blocks use external library coinor for linear programming, this library uses ``double`` type only. To make it work a linear programming routine has to be implemented using Eigen or coinor library should start using C++ templates or a converter/wrapper similar to :ysrc:`LAPACK library <lib/compatibility/LapackCompatibility.hpp>` should be used.

.. [#supp4] The module is enabled by default, the ``yade --test`` and ``yade --check`` pass, as well as most of examples are working. However the calculations are performed at lower ``double`` precision. A wrapper/converter layer for :ysrc:`LAPACK library <lib/compatibility/LapackCompatibility.hpp>` has been implemented. To make it work with full precision these routines should be reimplemented using Eigen.

Compatibility
===========================================

.. _python-hp-compatibility:

Python
----------------------------------------------

Python has :ysrc:`native support <lib/high-precision/ToFromPythonConverter.hpp>` for high precision types using ``mpmath`` package. Old Yade scripts that use :ref:`supported modules <supported-hp-modules>` can be immediately converted to high precision by switching to high precision version of minieigen. In order to do so, the following line:

.. code-block:: python

	from minieigen import *

has to be replaced with:

.. code-block:: python

	from yade.minieigenHP import *

Respectively ``import minieigen`` has to be replaced with ``import yade.minieigenHP as minieigen``. The :yref:`minieigenHP module<yade.minieigenHP>` has only these four lines inside:

.. code-block:: python

	if(yade.config.highPrecisionBits != 64):
		from yade._minieigenHP import *
	else:
		from minieigen import *

So that high precision (binary compatible) version of minieigen is used when non ``double`` type is used as ``Real``.


.. warning:: There may be still some parts of python code that were not migrated to high precision and may not work well with ``mpmath`` module. See :ref:`debugging section <hp-debugging>` for details.

.. _cpp-hp-compatibility:

C++
----------------------------------------------

Before introducing high precision it was assumed that ``Real`` is actually a `POD <https://en.cppreference.com/w/cpp/named_req/PODType>`__ ``double`` type. It was possible to use ``memset(…)``, ``memcpy(…)`` and similar functions on ``double``. This was not a good approach and even some compiler ``#pragma`` commands were used to silence the compilation warnings. To make ``Real`` work with other types, this assumption had `to be removed <https://gitlab.com/yade-dev/trunk/-/merge_requests/381>`__. A single ``memcpy(…)`` still remains in file :ysrccommit:`openmp-accu.hpp<de696763ea3ab8a88136976fb4d11eb3bd79fcbc/lib/base/openmp-accu.hpp#L42>` and will have to be removed. In future development such raw memory access functions are to be avoided.

All remaining ``double`` were replaced with ``Real`` and any attempts to use ``double`` type in the code will fail in the gitlab-CI pipeline.

Mathematical functions of all high precision types are wrapped using file :ysrc:`MathFunctions.hpp<lib/high-precision/MathFunctions.hpp>`, these are the inline redirections to respective functions of the type that Yade is currently being compiled with. The code will not pass the pipeline checks if ``std::`` is used. All functions that take ``Real`` argument should now call these functions in ``yade::math::`` namespace. Functions which take *only* ``Real`` arguments may omit ``math::`` specifier and use `ADL <https://en.cppreference.com/w/cpp/language/adl>`__ instead. Examples:

1. Call to ``std::min(a,b)`` is replaced with ``math::min(a,b)``, because ``a`` or ``b`` may be non ``Real`` the ``math::`` is necessary.
2. Call to ``std::sqrt(a)``  is replaced with ``math::sqrt(a)``. Since ``a`` is always ``Real`` the ``sqrt(a)`` may be written without ``math::`` as well, thanks to `ADL <https://en.cppreference.com/w/cpp/language/adl>`__.

If a new mathematical function is needed it has to be added in the following places:

1. :ysrc:`lib/high-precision/MathFunctions.hpp`
2. :ysrc:`py/high-precision/_math.cpp`, see :yref:`math module<yade.math>` for details.
3. :ysrc:`py/tests/testMath.py`
4. :ysrc:`py/tests/testMathHelper.py`

The tests for a new function are to be added in :ysrc:`py/tests/testMath.py` in one of these functions: ``oneArgMathCheck(…):``, ``twoArgMathCheck(…):``, ``threeArgMathCheck(…):``. A table of expected error tolerances in ``self.defaultTolerances`` is to be supplemented as well. To determine tolerances with better confidence it is recommended to temporarily increase number of tests in the :ysrccommit:`test loop<1b4ae97583bd8a6efc74cb0d0/py/tests/testMath.py#L338>`, but scale the arguments ``a`` and ``b`` accordingly to avoid infinities cropping up. To determine tolerances for currently implemented functions a ``range(2000)`` in both loops was used.

.. _hp-to-string:

String conversions
----------------------------------------------

It is recommended to use ``math::toString(…)`` and ``math::fromStringReal(…)`` conversion functions instead of ``boost::lexical_cast<std::string>(…)``. The ``toString`` function (in file :ysrc:`RealIO.hpp<lib/high-precision/RealIO.hpp>`) guarantees full precision during conversion. It is important to note that ``std::to_string`` does `not guarantee this <https://en.cppreference.com/w/cpp/string/basic_string/to_string>`__ and ``boost::lexical_cast`` does `not guarantee this either <https://www.boost.org/doc/libs/1_72_0/doc/html/boost_lexical_cast.html>`__.


Eigen and CGAL
----------------------------------------------

Eigen and CGAL libraries have native high precision support.

* All declarations required by Eigen are provided in files :ysrc:`EigenNumTraits.hpp<lib/high-precision/EigenNumTraits.hpp>` and :ysrc:`MathEigenTypes.hpp<lib/high-precision/MathEigenTypes.hpp>`
* All declarations required by CGAL  are provided in files :ysrc:`CgalNumTraits.hpp<lib/high-precision/CgalNumTraits.hpp>` and :ysrc:`AliasCGAL.hpp<lib/base/AliasCGAL.hpp>`


.. _vtk-real-compatibility:

VTK
-------------------------------------------

Since VTK is only used to record results for later viewing in other software, such as `paraview <https://www.paraview.org/>`__, the recording of all decimal places does not seem to be necessary.
Hence all recording commands in ``C++`` convert ``Real`` type down to ``double`` using ``static_cast<double>`` command. This has been implemented via classes ``vtkPointsReal``, ``vtkTransformReal`` and ``vtkDoubleArrayFromReal`` in file :ysrc:`VTKCompatibility.hpp<lib/compatibility/VTKCompatibility.hpp>`. Maybe VTK in the future will support non ``double`` types. If that will be needed, the interface can be updated there.


LAPACK
----------------------------------------------

Lapack is an external library which only supports ``double`` type. Since it is not templatized it is not possible to use it with ``Real`` type. Current solution is to `down-convert arguments <https://gitlab.com/yade-dev/trunk/-/merge_requests/379>`__ to ``double`` upon calling linear equation solver (and other functions), then convert them back to ``Real``. This temporary solution omits all benefits of high precision, so in the future Lapack is to be replaced with Eigen or other templatized libraries which support arbitrary floating point types.

.. _hp-debugging:

Debugging
===========================================

High precision is still in the experimental stages of implementation. Some errors may occur during use. Not all of these errors are caught by the checks and tests. Following examples may be instructive:

1. Trying to `use const references to Vector3r members <https://gitlab.com/yade-dev/trunk/-/merge_requests/406>`__ - a type of problem with results in a segmentation fault during runtime.
2. A part of python code `does not cooperate with mpmath <https://gitlab.com/yade-dev/trunk/-/merge_requests/414>`__ - the checks and tests do not cover all lines of the python code (yet), so more errors like this one are expected. The solution is to put the non compliant python functions into :ysrc:`py/high-precision/math.py`. Then replace original calls to this function with function in ``yade.math``, e.g. ``numpy.linspace(…)`` is replaced with ``yade.math.linspace(…)``.

The most flexibility in debugging is with the ``long double`` type, because special files :ysrc:`ThinRealWrapper.hpp<lib/high-precision/ThinRealWrapper.hpp>`, :ysrc:`ThinComplexWrapper.hpp<lib/high-precision/ThinComplexWrapper.hpp>` were written for that. They are implemented with `boost::operators <https://www.boost.org/doc/libs/1_72_0/libs/utility/operators.htm>`__, using `partially ordered field <https://www.boost.org/doc/libs/1_72_0/libs/utility/operators.htm#ordered_field_operators1>`__. Note that they `do not provide operator++ <https://gitlab.com/yade-dev/trunk/-/merge_requests/407>`__.

A couple of ``#defines`` were introduced in these two files to help debugging more difficult problems:

1. ``YADE_IGNORE_IEEE_INFINITY_NAN`` - it can be used to detect all occurrences when ``NaN`` or ``Inf`` are used. Also it is recommended to use this define when compiling Yade with ``-Ofast`` flag, without  ``-fno-associative-math -fno-finite-math-only -fsigned-zeros``
2. ``YADE_WRAPPER_THROW_ON_NAN_INF_REAL``, ``YADE_WRAPPER_THROW_ON_NAN_INF_COMPLEX`` - can be useful for debugging when calculations go all wrong for unknown reason.

Also refer to :ref:`address sanitizer section <address-sanitizer>`, as it is most useful for debugging in many cases.


