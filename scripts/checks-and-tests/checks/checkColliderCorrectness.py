# encoding: utf-8
from yade import pack,utils,export,plot
import math,os,sys
print 'checkColliderCorrectness for InsertionSortCollider'

failCollider=False

#### This is useful for printing the linenumber innn the script
# import inspect
# print inspect.currentframe().f_lineno

if((opts.threads != None and opts.threads != 1) or (opts.cores != None and opts.cores != '1')):
	print "This test will only work on single core, because it must be fully reproducible, but -j "+str(opts.threads)+" or --cores "+str(opts.cores)+" is used."
	print inspect.currentframe().f_lineno
	failCollider=True

# print inspect.currentframe().f_lineno
# typedEngine("NewtonIntegrator").gravity=(0,0,-9.81)
# print inspect.currentframe().f_lineno
# typedEngine("NewtonIntegrator").damping=0.1
# print inspect.currentframe().f_lineno
# testedCollider=typedEngine("InsertionSortCollider")
# print inspect.currentframe().f_lineno

#### This is unnecessary now. But if we wanted to test different colliders later that might be useful.
# if(testedCollider.__class__.__name__ not in yade.system.childClasses("Collider")):
#	print "testedCollider is not derived from collider class?"
#	failCollider=True


from yade import pack
from pylab import rand
from yade import qt

O.periodic=True
length=1.0
height=1.0
width=1.0
thickness=0.1

O.cell.hSize=Matrix3(length, 0, 0,
		     0 ,3.*height, 0,
		    0, 0, width)

O.materials.append(FrictMat(density=1,young=1e5,poisson=0.3,frictionAngle=radians(30),label='boxMat'))
lowBox = box( center=(length/2.0,thickness*0.6,width/2.0), extents=(length*2.0,thickness/2.0,width*2.0) ,fixed=True,wire=False)
O.bodies.append(lowBox)

radius=0.01
O.materials.append(FrictMat(density=1000,young=1e4,poisson=0.3,frictionAngle=radians(30),label='sphereMat'))
sp=pack.SpherePack()
sp.makeCloud((0.*length,height+1.2*radius,0.25*width),(0.5*length,2*height-1.2*radius,0.75*width),-1,.2,2000,periodic=True)
checksPath="."
sp.load(checksPath+'/data/100spheres')
O.bodies.append([sphere(s[0]+Vector3(0.0,0.2,0.0),s[1],color=(0.6+0.15*rand(),0.5+0.15*rand(),0.15+0.15*rand())) for s in sp])
O.bodies.append([sphere(s[0]+Vector3(0.1,0.3,0.0),s[1],color=(0.6+0.15*rand(),0.5+0.15*rand(),0.15+0.15*rand())) for s in sp])

O.dt=1e-5
O.usesTimeStepper=False
newton=NewtonIntegrator(damping=0.6,gravity=(0,-10,0))

O.engines=[
	ForceResetter(),
	#(1) This is where we allow big bodies, else it would crash due to the very large bottom box:
	InsertionSortCollider([Bo1_Box_Aabb(),Bo1_Sphere_Aabb()],allowBiggerThanPeriod=True),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	newton
]

Gl1_Sphere.stripes=1

from yade import qt
qt.View()
print('Press PLAY button')






if failCollider: #put a condition on the result here, is it the expected result? else:
	print "InsertionSortCollider failed."
	resultStatus+=1
