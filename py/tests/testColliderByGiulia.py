# -*- coding: utf-8 -*-
# https://gitlab.com/yade-dev/trunk/issues/7
# https://answers.launchpad.net/yade/+question/220785
# https://bugs.launchpad.net/yade/+bug/1112763

import unittest

from builtins import range
from math import *
from minieigen import *
from yade import *
from yade import system
from yade._customConverters import *
from yade.gridpfacet import *
from yade.utils import *
from yade.wrapper import *
import random
import sys,re,os
import unittest
import yade, yade.utils, yade.config, yade.remote


# https://docs.python.org/3/library/unittest.html

class TestColliderByGiulia(unittest.TestCase):
	def setUp(self):
		# here prepare some internal variables used by this test
		self.wontFixIssue7 = [ ( -1.0 , True  , 2 , 0 ) , (  1.0 , False , 2 , 0 ) , ( -1.0 , True  , 0 , 1 ) , (  1.0 , False , 0 , 1 ) , ( -1.0 , True  , 1 , 2 ) , (  1.0 , False , 1 , 2 ) ]
	def tearDown(self):
		pass # here finalize the tests, maybe close an opened file, or something else
	def printOK(self,sign,ordering,axis,initialSortAxis):
		self.assertTrue(True)
		#axis = (2-axis)%3 # bacause the movement is along Z axis, while axis numbering referes to X. Better to print the axis which causes problems, than axis used in the loop numbering.
		#print("\033[92m OK, sign=",sign," ordering=",ordering," axis=",axis," initialSortAxis=",initialSortAxis,"\033[0m")
	def noteFail(self,sign,ordering,axis,initialSortAxis):
		axis = (2-axis)%3 # bacause the movement is along Z axis, while axis numbering referes to X. Better to print the axis which causes problems, than axis used in the loop numbering.
		self.assertTrue( (sign,ordering,axis,initialSortAxis) in self.wontFixIssue7)
		if( (sign,ordering,axis,initialSortAxis) in self.wontFixIssue7):
			self.assertTrue(True)
			print("\033[92m OK, sign=",sign," ordering=",ordering," axis=",axis," initialSortAxis=",initialSortAxis,"\033[0m")
		else:
			self.assertTrue(False)
			print("\033[91m failure, sign=",sign," ordering=",ordering," axis=",axis," initialSortAxis=",initialSortAxis,"\033[0m")
	def testColiderRegressions(self):
#		from minieigen import Vector3
#		from yade.utils import sphere,PWaveTimeStep,typedEngine
#		from yade.wrapper import ForceResetter,InsertionSortCollider,Bo1_Sphere_Aabb,Ig2_Sphere_Sphere_ScGeom,InteractionLoop,Ip2_FrictMat_FrictMat_MindlinPhys,Law2_ScGeom_MindlinPhys_Mindlin
#		from yade.wrapper import ForceEngine,NewtonIntegrator
		DbigSphere	= 1.
		radius_bigSphere= DbigSphere/2
		radius_sph	= DbigSphere/10
		# Periodic cell
		cellSizes=[DbigSphere*1. , DbigSphere*3. , DbigSphere*3. ]
		# NOTE, these settings do not trigger the bug and work fine:
		#cellSizes[0]= DbigSphere*1.000000000000001 # ok - Anything bigger  than that works fine
		#cellSizes[0]= DbigSphere*0.605             # ok - Anything smaller than that works fine
		for initialSortAxis in [0,1,2]:
			for axis in [0,1,2]:
				for sign in [-1.0,1.0]:
					for ordering in [True,False]:
						O.periodic  = True
						O.cell.setBox(Vector3(cellSizes[(0+axis)%3],cellSizes[(1+axis)%3],cellSizes[(2+axis)%3]))
						# Big blue sphere
						centerPos        =        [cellSizes[0]/2.,cellSizes[1]/2.,cellSizes[2]*(0.7 if sign>0 else 0.3)]
						center_bigSphere = Vector3(centerPos[(0+axis)%3],centerPos[(1+axis)%3],centerPos[(2+axis)%3])
						bigSphere        = sphere(center=center_bigSphere, radius=radius_bigSphere, color=(0,0,1))
						# Red small sphere
						centerSmall      =        [centerPos[0],	centerPos[1]+1./5.*DbigSphere,	centerPos[2]+sign*(-DbigSphere/2.-radius_sph)]
						center_sph	 = Vector3(centerSmall[(0+axis)%3],centerSmall[(1+axis)%3],centerSmall[(2+axis)%3])
						sphereSmall	 = sphere(center=center_sph, radius=radius_sph, color=(1,0,0))
						forceAxis        = [0,0,-sign*100]

						if(ordering==True):
							O.bodies.append((sphereSmall,bigSphere))
						else:
							O.bodies.append((bigSphere,sphereSmall))

						O.engines=[
							ForceResetter(),
							InsertionSortCollider ([Bo1_Sphere_Aabb()], allowBiggerThanPeriod=True),
							InteractionLoop ([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_MindlinPhys(),],[Law2_ScGeom_MindlinPhys_Mindlin()]),
							ForceEngine (force=(forceAxis[(0+axis)%3],forceAxis[(1+axis)%3],forceAxis[(2+axis)%3]),ids=[bigSphere.id]),
							NewtonIntegrator (damping=0.5),
						]
						O.dt=0.2*PWaveTimeStep()
						testedCollider=typedEngine("InsertionSortCollider")
						testedCollider.sortAxis=initialSortAxis

#
# uncomment line below if you run this script manually: yade ./scripts/checks-and-tests/checks/checkColliderByGiulia.py
#
						#input("\033[33mTo see the bug, uncomment line 388 in pkg/common/InsertionSortCollider.cpp and recompile.\033[0m\nYou can press ▶ (the start button), but to see something use x10 smaller timestep.\nWhen you finish examination press enter to continue this script. BTW: it will fail if you changed the timestep.\n")
						# at O.iter==2 is appears first time and i.isReal == False, at O.iter==4240, i.isReal==True and Overlap 1.1326899061803175e-05
						O.run(2,True)
						#print("dumpBounds():",testedCollider.dumpBounds()[(2-axis)%3])
						try:
							i=O.interactions[bigSphere.id,sphereSmall.id]
							if((i.isReal == False) and (O.iter==2)):
								O.run(4240-3,True)
								wasReal = i.isReal
								O.run(1,True)
								if((wasReal==False) and (i.isReal == True) and (O.iter==4240) and (abs(O.interactions[bigSphere.id,sphereSmall.id].geom.penetrationDepth - 1.1326899061803175e-05) < 1e-15 )):
									printOK(sign,ordering,axis,initialSortAxis)
									#print('iter first interaction\tStep',O.iter,'\tOverlap',O.interactions[bigSphere.id,sphereSmall.id].geom.penetrationDepth)
								else:
									noteFail(sign,ordering,axis,initialSortAxis)
							else:
								noteFail(sign,ordering,axis,initialSortAxis)
						except Exception as e:
							noteFail(sign,ordering,axis,initialSortAxis)

						O.reset()

