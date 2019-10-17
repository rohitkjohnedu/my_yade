#!/usr/bin/python
# -*- coding: utf-8 -*-

############################################
##### interesting parameters          #####
############################################
# Cundall non-viscous damping
# try zero damping and watch total energy...
from __future__ import print_function
damping = 0.2
# initial angular velocity
angVel = 3.0
# use two spheres?
two_spheres =True
# sphere rotating more?
rotate_in_two_directions = True

############################################
##### material                         #####
############################################

import matplotlib
matplotlib.use('TkAgg')
O.materials.append(CohFrictMat(
	young=3e8,
	poisson=0.3,
	frictionAngle=radians(30),
	density=2600,
	isCohesive=False,
	alphaKr=0.031,
	alphaKtw=0.031,
	momentRotationLaw=False,
	etaRoll=5.0,
	label='granular_material'))

############################################
##### calculation loop                 #####
############################################
law=Law2_ScGeom6D_CohFrictPhys_CohesionMoment(always_use_moment_law=False)
g=9.81

guiIterPeriod=500000

O.trackEnergy=True
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom6D(),Ig2_Box_Sphere_ScGeom6D()],	
		[Ip2_CohFrictMat_CohFrictMat_CohFrictPhys()],
		[law]
	),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=50,timestepSafetyCoefficient=.0001),
	NewtonIntegrator(damping=damping,kinSplit=True,gravity=(0,0,-g)),
	PyRunner(iterPeriod=20,command='myAddPlotData()')
	,PyRunner(iterPeriod=guiIterPeriod,command='nextGuiTest()')
]

O.bodies.append(box(center=[0,0,0],extents=[.5,.5,.5],fixed=True,color=[1,0,0],material='granular_material'))
O.bodies.append(sphere([0,0,2],1,color=[0,1,0],material='granular_material'))
if(two_spheres):
	O.bodies.append(sphere([0,0,4],1,color=[0,1,0],material='granular_material'))
O.dt=.002*PWaveTimeStep()
O.bodies[1].state.angVel[1]=angVel
if(rotate_in_two_directions):
	O.bodies[1].state.angVel[2]=angVel

############################################
##### now the part pertaining to plots #####
############################################

from yade import plot
## we will have 2 plots:
## 1. t as function of i (joke test function)
## 2. i as function of t on left y-axis ('|||' makes the separation) and z_sph, v_sph (as green circles connected with line) and z_sph_half again as function of t


plot.labels={'t':'time [s]', 
	'normal_Work':'Normal work: W=kx^2/2', 
	'shear_Work':'Shear work: W=kx^2/2',
	'E_kin_translation':'Translation energy: E_kin=m*V^2/2',
	'E_kin_rotation':'Rotation energy: E_kin=I*$\omega$^2/2',
	'E_pot':'Gravitational potential: E_pot=m*g*h',
	'E_plastic':'Plastic dissipation on shearing: E_pl=F*$\Delta$F/k',
	'total':'total',
	'total_plus_damp':'total + damping'}

plot.plots={'t':(
		('normal_Work','b-'),
		('shear_Work','r-'),
		('E_kin_translation','b-.'),
		('E_kin_rotation','r-.'),
		('E_plastic','c-'),
		('E_pot','y-'),
		('total','k:'),
		('total_plus_damp','k-')
		)}

## this function is called by plotDataCollector
## it should add data with the labels that we will plot
## if a datum is not specified (but exists), it will be NaN and will not be plotted
def myAddPlotData():
	normal_Work	  = law.normElastEnergy()
	shear_Work	  = law.shearElastEnergy()
	E_kin_translation = 0
	E_kin_rotation    = 0
	E_pot		  = 0
	E_plastic	  = 0
	E_tracker	  = dict(list(O.energy.items()))

	if(two_spheres):## for more bodies we better use the energy tracker, because it's tracking all bodies
		E_kin_translation = E_tracker['kinTrans']
		E_kin_rotation    = E_tracker['kinRot']
		E_pot             = E_tracker['gravWork']

	else: ## for one sphere we can just calculate, and it will be correct
		sph=O.bodies[1]
		h=sph.state.pos[2]
		V=sph.state.vel.norm()
		w=sph.state.angVel.norm()
		m=sph.state.mass
		I=sph.state.inertia[0]
		E_kin_translation = m*V**2.0/2.0
		E_kin_rotation    = I*w**2.0/2.0
		E_pot		  = m*g*h

	if('plastDissip' in E_tracker):
		E_plastic	  = E_tracker['plastDissip']

	total = normal_Work + shear_Work + E_plastic + E_kin_translation + E_kin_rotation + E_pot
	total_plus_damp	  = 0
	if(damping!=0):
		total_plus_damp	  = total + E_tracker['nonviscDamp']
	else:	
		total_plus_damp	  = total
	plot.addData(
		t=O.time,
		normal_Work	  = normal_Work	 ,
		shear_Work	  = shear_Work	 ,
		E_kin_translation = E_kin_translation,
		E_kin_rotation    = E_kin_rotation   ,
		E_pot		  = E_pot		 ,
		E_plastic	  = E_plastic ,
		total		  = total		 ,
		total_plus_damp	  = total_plus_damp	 ,
	)

#print("Now calling plot.plot() to show the figures. The timestep is artificially low so that you can watch graphs being updated live.")

############################################################################################################################################
############################################################# test GUI #####################################################################
############################################################################################################################################
# here start changes of script simple-scene-energy-tracking.py, maybe later this duplication of code above can be removed.
# The code below, takes screenshot before and after each GUI action. And yade is hopefully not crashing in between.
# The test runs on debug build, so anyway we should get a useful backtrace from gitlab-CI

scrNum=0;
plot.liveInterval=0
def makeNextScreenshot():
	import subprocess,time
	global scrNum
	time.sleep(1)
	subprocess.run(["scrot", "-m" , "-z" , "scr"+str(scrNum)+".png"])
	time.sleep(1)

def nextGuiTest():
	from yade import qt
	import time
	global scrNum
	scrNum += 1
	intro = "="*20+" "+"stage:"+str(scrNum)+" iter:"+str(O.iter)
	if(scrNum == 0):
		makeNextScreenshot();
		print(intro)
		return
	if(scrNum == 1):
		makeNextScreenshot();
		print(intro+" opening yade.qt.View()")
		yade.qt.View();
		vv=yade.qt.views()[0]
		vv.axes=True
		vv.eyePosition=(2.8717367257539266,-3.2498802823394684,11.986065750108025)
		vv.upVector  =(-0.786154130840315,-0.2651054185084529,0.558283798475441)
		vv.center()
		return
	if(scrNum == 2):
		makeNextScreenshot();
		print(intro+" opening yade.qt.Inspector()")
		yade.qt.Inspector()
		yade.qt.controller.inspector.setGeometry(1050,20,500,1100)
		yade.qt.controller.inspector.show()
		return
	if(scrNum == 3):
		makeNextScreenshot();
		print(intro+" changing tab to bodies")
		yade.qt.controller.inspector.close()
		yade.qt.controller.inspector.setGeometry(1050,20,500,1100)
		yade.qt.controller.inspector.tabWidget.setCurrentIndex(1)
		yade.qt.controller.inspector.show()
		return
	if(scrNum == 4):
		makeNextScreenshot();
		print(intro+" changing tab to interactions")
		yade.qt.controller.inspector.close()
		yade.qt.controller.inspector.setGeometry(1050,20,500,1100)
		yade.qt.controller.inspector.tabWidget.setCurrentIndex(2)
		yade.qt.controller.inspector.show()
		return
	if(scrNum == 5):
		makeNextScreenshot();
		print(intro+" changing tab to cell")
		yade.qt.controller.inspector.close()
		yade.qt.controller.inspector.setGeometry(1050,20,500,1100)
		yade.qt.controller.inspector.tabWidget.setCurrentIndex(3)
		yade.qt.controller.inspector.show()
		return
	if(scrNum == 6):
		makeNextScreenshot();
		print(intro+" changing tab to bodies")
		yade.qt.controller.inspector.close()
		yade.qt.controller.inspector.setGeometry(1050,20,500,1100)
		yade.qt.controller.inspector.tabWidget.setCurrentIndex(1)
		yade.qt.controller.inspector.show()
		print(intro+" moving yade.qt.Controller()")
		yade.qt.Controller()
		yade.qt.controller.setGeometry(550,5,500,1100)
		return
	if(scrNum == 7):
		makeNextScreenshot();
		print(intro+" changing tab to display")
		yade.qt.controller.setTabActive('display')
		return
	if(scrNum == 8):
		makeNextScreenshot();
		print(intro+" changing tab to generator")
		yade.qt.controller.setTabActive('generator')
		return
	if(scrNum == 9):
		makeNextScreenshot();
		print(intro+" changing tab to python")
		yade.qt.controller.setTabActive('python')
		return
	if(scrNum == 10):
		makeNextScreenshot();
		print(intro+" changing tab to simulation")
		yade.qt.controller.setTabActive('simulation')
		return
	if(scrNum == 11):
		makeNextScreenshot();
		# FIXME: I couldn't get matplotlib to draw the plot, while screenshotting is going on.
		# makeNextScreenshot();
		# O.pause()
		# print(intro+" opening yade.plot.plot()")
		# plot.liveInterval=0
		# plot.plot(subPlots=False)
		# fig=yade.plot.plot();
		# time.sleep(5)
		# matplotlib.pyplot.draw()
		# time.sleep(5)
		# makeNextScreenshot();
		# matplotlib.pyplot.close(fig)
		return
	if(scrNum == 12):
		makeNextScreenshot();
		print(intro+" exiting")
		O.pause()
		makeNextScreenshot();
		vv=yade.qt.views()[0]
		vv.close()
		yade.qt.controller.inspector.close()
		yade.qt.controller.close()
#		sys.exit(0)
		os._exit(0)
#		quit()
		return

O.dt = O.dt*0.0001
O.run(guiIterPeriod*12+1)

