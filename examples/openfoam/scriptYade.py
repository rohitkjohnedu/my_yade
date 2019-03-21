#Deepak Kunhappan, deepak.kunhappan@3sr-grenoble.fr 
#Example script of Yade-OpenFOAM couplinfg.
#get the OpenFOAM solver at : https://github.com/dpkn31/Yade-OpenFOAM-coupling, (only tested on OpenFoam-6)
#get the latest version of Yade with the FoamCoupling engine :https://gitlab.com/yade-dev/trunk 
#compile the solver and libs from the git : ./Allwmake 
#create a symbolic link to your Yade Install, and a directiory for VTK dump.
#example video can be found here : https://youtu.be/J_V1ffx71To


import sys 
from libyade import *
from yade.utils import *
initMPI(); #Initialize the mpi environment, always required. 

fluidCoupling = FoamCoupling() #The coupling engine 
fluidCoupling.getRank()

#example of spheres in shear flow : two-way point force coupling
class simulation(): 
   
  def __init__(self):

    O.periodic = True; 
    O.cell.setBox(1,1,1); 
    
    
    numspheres=1000; 
    
    #Set the same particle density in the constant/transportProperties file in the OpenFOAM side. 
    young = 5e6; density = 1000;

    #materials for the spheres and the wall. 
    O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=radians(15),density=density,label='spheremat'))
    O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=0,density=0,label='wallmat'))
   
    #lower and upper walls 
    wall1 = wall(1e-08,1,0, material='wallmat'); O.bodies.append(wall1); 
    wall2 = wall(1-1e-08,1,0, material='wallmat'); O.bodies.append(wall2); 
    
 
    # create spheres. 
    mn, mx= Vector3(2e-08,2e-08,2e-08), Vector3(1-2e-08,1-2e-08, 1-2e-08)
    sp = pack.SpherePack();
    sp.makeCloud(mn,mx,-1,0.3333,numspheres,False, 0.95,seed=1) 
    O.bodies.append([sphere(center,rad,material='spheremat') for center,rad in sp]) 

    sphereIDs = [b.id for b in O.bodies if type(b.shape)==Sphere] 
    numparts = len(sphereIDs);     
    
    #Setting the fluid coupling engine. 
    fluidCoupling.setNumParticles(numparts)     #number of bodies/particles which will be affected by the flow (Walls are not included)
    fluidCoupling.setIdList(sphereIDs)          #list of ids of the bodies/particles.  
    fluidCoupling.isGaussianInterp=False # to use Gaussian averaging for field variables, use with pimpleFoamYade solver. 
                                         #(Although icoFoamYade will work, provided you switch it on in the icoFoamYade solver) 

    newton=NewtonIntegrator(damping=0.0, gravity = (0.0 ,0.0, 0.0)) # add small damping in case of stability issues.. ~ 0.1 max, also note : If using gravity,  make sure buoyancy force is switched on in the foam side, else the simulation will crash

    # The usual engines. 
    O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.7, label = "ts"), 
        fluidCoupling, #to be called after timestepper  
        PyRunner(command='sim.printMessage()', iterPeriod= 1000, label='hydroforce'), 
	newton, 
        VTKRecorder(fileName='yadep/3d-vtk-',recorders=['spheres'],iterPeriod=10000)
    ]

  def printMessage(self):
     print "********************************YADE-ITER = " + str(O.iter) +" **********************************" 


  def irun(self,num): 
      O.run(num,1)

if __name__=="__main__":
    sim = simulation()
    sim.irun(1000000)
    fluidCoupling.killmpi()

import __builtin__ 
__builtin__.sim=sim
