# encoding: utf-8
##########################################################################
#  2019      Janek Kozicki                                               #
#                                                                        #
#  This program is free software; it is licensed under the terms of the  #
#  GNU General Public License v2 or later. See file LICENSE for details. #
##########################################################################
"""
This python module exposes all C++ math functions for Real and Complex types to python.
In fact it sort of duplicates ``import math``, ``import cmath`` or ``import mpmath``.
Also it facilitates migration of old python scripts to high precision calculations.

This module has following purposes:

1. To reliably :ysrc:`test<py/tests/testMath.py>` all C++ math functions of arbitrary precision Real and Complex types against mpmath.
2. To act as a "migration helper" for python scripts which call python mathematical functions that do not work
   well with mpmath. As an example see :yref:`yade.math.linspace` below and `this merge request <https://gitlab.com/yade-dev/trunk/-/merge_requests/414>`__
3. To allow writing python math code in a way that mirrors C++ math code in Yade. As a bonus it
   will be faster than mpmath because mpmath is a purely python library (which was one of the main
   difficulties when writing :ysrc:`lib/high-precision/ToFromPythonConverter.hpp`)
4. To test Eigen NumTraits
5. To test CGAL NumTraits

If another ``C++`` :ref:`math function<cpp-hp-compatibility>` is needed it should be added to following files:

1. :ysrc:`lib/high-precision/MathFunctions.hpp`
2. :ysrc:`py/high-precision/_math.cpp`
3. :ysrc:`py/tests/testMath.py`
4. :ysrc:`py/tests/testMathHelper.py`

If another ``python`` math function does not work well with ``mpmath`` it should be added below, and original
calls to this function should call this function instead, e.g. ``numpy.linspace(…)`` is replaced with ``yade.math.linspace(…)``.

"""
# all C++ functions are accessible now:
from yade._math import *

import yade.config

def linspace(a,b,num):
	"""
	This function calls ``numpy.linspace(…)`` or ``mpmath.linspace(…)``, because ``numpy.linspace`` function does not work with mpmath.
	"""
	if(yade.config.highPrecisionMpmath):
		import mpmath
		return mpmath.linspace(a,b,num)
	else:
		import numpy
		return numpy.linspace(a,b,num=num)

