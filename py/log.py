# encoding: utf-8
# 2019 Janek Kozicki
# hint: follow changes in d067b0696a8 to add new modules. 

"""
The ``yade.log`` module serves as an interface to yade logging framework implemented on top of `boost::log <https://www.boost.org/doc/libs/release/libs/log/>`_.
For full documentation see :ref:`debugging section<debugging>`. Example usage in python is as follows:

.. code-block:: python

	import yade.log
	yade.log.setLevel('PeriTriaxController',yade.log.TRACE)

Example usage in C++ is as follows:

.. code-block:: c++

	LOG_WARN("Something: "<<something)
	// also a logger can be obtained with example command:
	YadeLogger logger = Logger::getLogger("yade.pack.predicates");

.. warning:: This documentation is incomplete.
"""

# all C++ functions are accessible now:
from yade._log import *

def test():
	"""
	.. warning:: This is a test.
	"""
	print("Test OK")


# Maybe add here python commands that would respect the filtering levels?
# They would just test the filter level and call the print(…) command.
# Their names would be LOG_FATAL(…), LOG_WARN(…) etc...
# Suppelement it with a python testAllLevelsPython(), like it is written in _log.cpp.
# And mention them in prog.rst.
# search if there is some kind of python logging library and use for this maybe?

