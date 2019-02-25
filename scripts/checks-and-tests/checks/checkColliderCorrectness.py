# encoding: utf-8
from yade import pack,utils,export,plot
import math,os,sys
print 'checkColliderCorrectness for InsertionSortCollider'

failCollider=False

if((opts.threads != None and opts.threads != 1) or (opts.cores != None and opts.cores != '1')):
	print "This test will only work on single core, because it must be fully reproducible, but -j "+str(opts.threads)+" or --cores "+str(opts.cores)+" is used."
	failCollider=True

typedEngine("NewtonIntegrator").gravity=(0,0,-9.81)
typedEngine("NewtonIntegrator").damping=0.1
testedCollider=typedEngine("InsertionSortCollider")

##This is unnecessary now. But if we wanted to test different colliders later that might be useful.
#if(testedCollider.__class__.__name__ not in yade.system.childClasses("Collider")):
#	print "testedCollider is not derived from collider class?"
#	failCollider=True

#Typical structure of a checkTest:

#do something and get a result...

if failCollider: #put a condition on the result here, is it the expected result? else:
	print "InsertionSortCollider Dummy failed (we know it will not happen here, you get the idea)." 
	resultStatus+=1
