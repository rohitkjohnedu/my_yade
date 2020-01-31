# encoding: utf-8
from __future__ import print_function

from yade import pack,export,plot
import math,os,sys,shutil,subprocess,tempfile
print('checkVTKRecorder')

#### This is useful for printing the linenumber in the script
# import inspect
# print(inspect.currentframe().f_lineno)

if((opts.threads != None and opts.threads != 1) or (opts.cores != None and opts.cores != '1')):
	raise YadeCheckError("This test will only work on single core, because it must be fully reproducible, but -j "+str(opts.threads)+" or --cores "+str(opts.cores)+" is used.")

from yade import pack


#checksPath="." # this line was used for working on this script locally.
tmpSaveDir = tempfile.mkdtemp()
vtkSaveDir = tmpSaveDir+'/vtk_testing/'
if not os.path.exists(vtkSaveDir):
	os.makedirs(vtkSaveDir);

O.periodic=False
length=1.0
height=1.0
width=1.0
thickness=0.1

O.materials.append(FrictMat(density=1,young=1e5,poisson=0.3,frictionAngle=radians(30),label='boxMat'))
lowBox = box( center=(length/2.0,thickness*0.6,width/2.0), extents=(length*2.0,thickness/2.0,width*2.0) ,fixed=True,wire=False)
O.bodies.append(lowBox)

radius=0.01
O.materials.append(FrictMat(density=1000,young=1e4,poisson=0.3,frictionAngle=radians(30),label='sphereMat'))
sp=pack.SpherePack()
#sp.makeCloud((0.*length,height+1.2*radius,0.25*width),(0.5*length,2*height-1.2*radius,0.75*width),-1,.2,2000,periodic=True)
sp.load(checksPath+'/data/100spheres')
# 100 was not enough to have reasonable number of collisions, so I put 200 spheres.
O.bodies.append([sphere(s[0]+Vector3(0.0,0.2,0.0),s[1]) for s in sp])
O.bodies.append([sphere(s[0]+Vector3(0.1,0.3,0.0),s[1]) for s in sp])

O.dt=5e-4
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
	VTKRecorder(fileName=vtkSaveDir,recorders=['all'], firstIterRun=10, iterPeriod=2000 ,label="VtkRecorder", ascii=True, multiblock=True),
	newton
]

for b in O.bodies:
	b.shape.color=Vector3(b.id%8/8.0,b.id%8/8.0,b.id%8/8.0)

O.run( 20, True);

p=subprocess.Popen(["/usr/bin/diff", "-r" , vtkSaveDir , checksPath+"/data/vtk_reference"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
p.wait()
diffResult = p.returncode
diffOut, diffErr = p.communicate()
if(diffErr == None):
	diffErr = ""
else:
	diffErr = diffErr.decode()
if(diffOut == None):
	diffOut=""
else:
	diffOut = diffOut.decode()

if(diffResult==0):
	print("OK, removing temporary directory: ",vtkSaveDir)
	shutil.rmtree(vtkSaveDir, ignore_errors=True)
else:
	print('\033[91m --------------------------------------------- \033[0m')
	print('diffResult=',diffResult)
	print('\033[91m --------------------------------------------- \033[0m')
	print(diffOut)
	print('\033[91m --------------------------------------------- \033[0m')
	print(diffErr)
	print('\033[91m --------------------------------------------- \033[0m')
	print("Wrong result, keeping temporary directory: ",vtkSaveDir)
	raise YadeCheckError("checkVTKRecorder.py failed.")

