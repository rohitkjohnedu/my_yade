.. _mpy:

MPI parallelization
===================

The module mpy (link) implements parallelization by domain decomposition (distributed memory) using the Message Passing Interface (MPI) implemented by OpenMPI.
It aims at exploiting large numbers of cores, where shared memory techniques (OpenMP) are helpless.
The shared memory and the distributed memory approaches are compatible, i.e. it is possible to run hybrid jobs using both, as shown below (and it may well be the optimal solution in many cases).

Disclaimer: even though the mpy module provides the function mpirun(), which may seem as a simple replacement for O.run(), setting up a simulation with MPI can be deceptively triavial.
It is anticipated that, in general, a simple replacement of "run" by "mpirun" in an arbitrary script will not work. To understand why, and to tackle the problems, it is recommended to acquire at least basic knowledge of how MPI works (this is partly covered below), and to understand Yade's implementation through basic examples.



Concepts
________


.. _fig-subdomains:
.. figure:: fig/subdomains.*
	:width: 12cm
	
	Two overlapping subdomains and their intersections. In this situation we have subD1.intersections[id_sd2]==[id4,id5] and subD1.mirrorIntersections[id_sd2]==[id1], with id_sd2 the index of subdomain 2. 


Execution
_________

Passive mode
------------

Interactive mode
----------------
 - Automatic initialization
 - Explicit initialization


Centralized scene construction
------------------------------

Distributed scene construction
------------------------------


Various remarks
---------------
- omca_oversubscribe = 1