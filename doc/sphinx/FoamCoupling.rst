.. _FoamCouplingEngine:

CFD-DEM coupled simulations with Yade and OpenFOAM
==================================================
The yadeFoamCoupling engines provides a framework for Euler-Lagrange fluid-particle
simulation with the open source finite volume solver OpenFOAM. The coupling
relies on the Message Passing Interface library (MPI), as OpenFOAM is
a parallel solver, furthermore communication between the solvers are realised by MPI messages. 
Yade sends the partilce information (particle position, velocity, etc. ) to all the OpenFOAM processes. Each OpenFOAM process searches the particle in the local mesh, 
if the particle is found, the hydrodynamic drag force and torque are calculated using the fluid velocity at the particle position (two interpolation methods are available) and the particle velocity.
The hydroynamic force is sent to the Yade process and it is added  to the force container.  The negative of the particle hydrodynamic force is set as source term in the Navier-Stokes equations. 
The OpenFOAM solver can be found at: https://github.com/dpkn31/Yade-OpenFOAM-coupling. The coupling methodology can be found in [Kunhappan2017_] and [Kunhappan2018_].

Simulation Types
================
Two simulation methods (unresolved) are currently available : simple point
force coupling and full CFD-DEM coupling which includes the particle volume fraction
effects. For the point force coupling the following equations are solved in the 
OpenFOAM solver icoFoamYade, momentum equation : 

.. math:: \frac{\partial \vec{U}}{\partial t} + \nabla \cdot (\vec{U}\vec{U}) = -\frac{\nabla p}{\rho} + \nabla \bar{\bar \tau} + \vec{f}_{h}

Along with the continuity equation: 

.. math:: \nabla \cdot \vec{U} = 0 

where $\vec{f}_{h}$ is the hydrodynamic force set as the volumetric source term. This hydrodynamic force is calculated using the Stokes equation and is only 
valid for particle Reynolds numbers less than 1. Furthermore the size of the particles have to be smaller than the grid size.  

In full CFD-DEM approach the following eqautions are solved : 

.. math:: \frac{\partial (\epsilon \vec{U})}{\partial t} + \nabla \cdot ( \epsilon \vec{U}\vec{U}) = -\frac{\nabla p}{\rho}+ \epsilon \nabla \bar{\bar \tau} + \vec{f}_{h}

Along with the continuity equation: 

.. math:: \frac{\partial \epsilon}{\partial t} + \nabla \cdot (\epsilon \vec{U}) = 0  

where $\epsilon$ is the fluid volume fraction$


Usage
=====
The example in examples/openfoam/scriptYade.py demonstrates the coupling.
A symbolic link to Yade is created and it is imported in the script. The MPI environment
is initialized by the initMPI() function ::


    fluidCoupling = FoamCoupling()
    fluidCoupling.initMPI()

A list of the particle ids and number of particle is passed to the coupling engine ::
  

    sphereIDs = [b.id for b in O.bodies if type(b.shape)==Sphere] 
    numparts = len(sphereIDs);     
    
    fluidCoupling.setNumParticles(numparts)
    fluidCoupling.setIdList(sphereIDs)
    fluidCoupling.isGaussian = False 

The type of force/velocity interpolation mode has to be set. For Gaussian   envelope interpolation, the isGaussian flag has to be set, also  the solver
pimpleFoamYade (under validation) is required.  The engine is added to the O.engines after the timestepper ::


      O.engines = [
      ForceResetter(),
      ..., 
      GlobalStiffnessTimeStepper, 
      fluidCoupling ...
      newton ]
    
Substepping/data exchange interval is set automatically based on the
foamDt/yadeDt.

To execute the script ::

    mpiexec -n 1 python scriptYade.py : -n NUMPROCS icoFoamYade -parallel 
