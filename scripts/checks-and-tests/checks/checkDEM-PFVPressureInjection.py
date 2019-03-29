from yade import pack
from yade import export
from yade import timing
from yade import plot

num_spheres=1000# number of spheres
young=1e6
compFricDegree = 3 # initial contact friction during the confining phase
finalFricDegree = 30 # contact friction during the deviatoric loading
mn,mx=Vector3(0,0,0),Vector3(1,1,0.4) # corners of the initial packing
graindensity=2600
toleranceWarning =1.e-11
toleranceCritical=1.e-6

O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=radians(compFricDegree),density=graindensity,label='spheres'))
O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=0,density=0,label='walls'))
walls=aabbWalls([mn,mx],thickness=0,material='walls')
wallIds=O.bodies.append(walls)

sp=pack.SpherePack()
sp.makeCloud(mn,mx,-1,0.3333,num_spheres,False, 0.95,seed=1) #"seed" make the "random" generation always the same
sp.toSimulation(material='spheres')

triax=TriaxialStressController(
	maxMultiplier=1.+2e4/young, # spheres growing factor (fast growth)
	finalMaxMultiplier=1.+2e3/young, # spheres growing factor (slow growth)
	thickness = 0,
	stressMask = 7,
	max_vel = 0.005,
	internalCompaction=True, # If true the confining pressure is generated by growing particles
)

newton=NewtonIntegrator(damping=0.2)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],label="iloop"
	),
	FlowEngine(dead=1,label="flow"),#introduced as a dead engine for the moment, see 2nd section
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=100,timestepSafetyCoefficient=0.8),
	triax,
	newton
]

triax.goal1=triax.goal2=triax.goal3=-10000

while 1:
  O.run(1000, True)
  unb=unbalancedForce()
  if unb<0.001 and abs(-10000-triax.meanStress)/10000<0.001:
    break

setContactFriction(radians(finalFricDegree))

radius=0
for b in O.bodies:
  if b.state.mass==0:
      b.state.blockedDOFs='xyzXYZ'
      b.state.vel=(0,0,0)
      b.state.angVel=(0,0,0)
  if b.state.mass>0:
      radius+=b.shape.radius
  #    b.state.blockedDOFs='xyz'
   #   b.state.vel=(0,0,0)
radius=radius/num_spheres

triax.dead=True
while 1:
  O.run(1000, True)
  unb=unbalancedForce()
  if unb<0.001:
    break

press=1000.    
O.run(10,1)
flow.dead=0
flow.meshUpdateInterval=-1
flow.useSolver=3
flow.permeabilityFactor=1
flow.viscosity=0.1
flow.bndCondIsPressure=[0,0,1,0,0,0]
flow.bndCondValue=[0,0,press,0,0,0]
flow.boundaryUseMaxMin=[0,0,0,0,0,0]
flow.iniVoidVolumes=True
newton.damping=0.1
GlobalStiffnessTimeStepper.dead=True
O.dt=min(0.8*PWaveTimeStep(),0.8*1./1200.*pi/flow.viscosity*graindensity*radius**2)
O.dynDt=False

O.run(1,1)

voidvol=0.0
voidvoltot=0.0
nvoids=flow.nCells()
totalflux=[0] * (nvoids)
initialvol=[0] * (nvoids)
bar=[0] * (nvoids)
initiallevel=O.bodies[flow.wallIds[flow.ymin]].state.pos[1]+(O.bodies[flow.wallIds[flow.ymax]].state.pos[1]-O.bodies[flow.wallIds[flow.ymin]].state.pos[1])/3
initialymax=O.bodies[flow.wallIds[flow.ymax]].state.pos[1]

for ii in range(nvoids):
   initialvol[ii]=1./flow.getCellInvVoidVolume(ii)
   bar[ii]=flow.getCellBarycenter(ii)[1]
   voidvoltot+=initialvol[ii]
   if bar[ii]>=initiallevel:
      voidvol+=initialvol[ii]
      flow.setCellPImposed(ii,True)
      flow.setCellPressure(ii,0)

O.run(1,1)
iterini=O.time 
flow.saveVtk("./vtk",True)
for ii in range(nvoids):
  if bar[ii]>=initiallevel:
    if flow.getCellPImposed(ii)==True:
      #totalflux[ii]=flow.getCellFluxFromId(ii)*O.dt
      totalflux[ii]=0.0*O.dt
#Qx=(flow.getBoundaryFlux(flow.wallIds[flow.xmin]))*O.dt+(flow.getBoundaryFlux(flow.wallIds[flow.xmax]))*O.dt
#Qz=(flow.getBoundaryFlux(flow.wallIds[flow.zmin]))*O.dt+(flow.getBoundaryFlux(flow.wallIds[flow.zmax]))*O.dt
#Qin=(flow.getBoundaryFlux(flow.wallIds[flow.ymin]))*O.dt
#Qout=(flow.getBoundaryFlux(flow.wallIds[flow.ymax]))*O.dt
Qin=0.0*O.dt
Qout=0.0*O.dt
Qx=0.0*O.dt
Qz=0.0*O.dt
 
bubble=0   
deltabubble=0
deltaovercross=0
deltaagain=0
def pressureImbibition():
   global bubble,deltabubble,deltaovercross,deltaagain
   global Qin,Qout,Qz,Qx,totalflux,total
   Qin+=(flow.getBoundaryFlux(flow.wallIds[flow.ymin]))*O.dt
   Qout+=(flow.getBoundaryFlux(flow.wallIds[flow.ymax]))*O.dt
   Qx+=(flow.getBoundaryFlux(flow.wallIds[flow.xmin]))*O.dt+(flow.getBoundaryFlux(flow.wallIds[flow.xmax]))*O.dt
   Qz+=(flow.getBoundaryFlux(flow.wallIds[flow.zmin]))*O.dt+(flow.getBoundaryFlux(flow.wallIds[flow.zmax]))*O.dt
   
   for ii in range(nvoids):
      if bar[ii]>=initiallevel:
	 if flow.getCellPImposed(ii)==True:
	    totalflux[ii]+=flow.getCellFluxFromId(ii)*O.dt
	    
   for ii in range(nvoids):
      if bar[ii]>=initiallevel:
	 if flow.getCellPImposed(ii)==True:
	    if (-totalflux[ii])>initialvol[ii]:
	       deltaflux=-totalflux[ii]-initialvol[ii]
	       kk=0
	       neigh=[nvoids+2,nvoids+2,nvoids+2,nvoids+2]
               neighok=[nvoids+2,nvoids+2,nvoids+2,nvoids+2]
	       for jj in range(4):
		 if jj<=len(flow.getNeighbors(ii))-1:
		   if flow.getCellPImposed(flow.getNeighbors(ii)[jj])==True:
		     if (-totalflux[flow.getNeighbors(ii)[jj]])<initialvol[flow.getNeighbors(ii)[jj]]:
		       neigh[kk]=flow.getNeighbors(ii)[jj]
		       kk=kk+1
	       if kk==0:
		 totalflux[ii]+=deltaflux
		 bubble=bubble+1
		 deltabubble+=deltaflux
	       if kk!=0:
		 totalflux[ii]+=deltaflux
	         deltafluxEach=deltaflux/kk
	         deltadelta=0
	         for xx in range(4):
		   if kk!=0:
		     if neigh[xx]<=nvoids:
		       jj=neigh[xx]
		       if (-totalflux[jj]+deltafluxEach)>initialvol[jj]:
			 deltadelta+=-totalflux[jj]-initialvol[jj]
		         totalflux[jj]+=-(-totalflux[jj]-initialvol[jj])
		         kk=kk-1
		         neighok[kk]=jj		        		         
		 if kk==0:
		   deltaflux2=deltaflux-deltadelta
	           kk2=0
	           neigh2=[nvoids+2]*(4*4)
                   neighok2=[nvoids+2]*(4*4)
	           for xx in range(4):
                      if neigh[xx]<=nvoids:
			jj=neigh[xx]
		        for gg in range(4):
			  if gg<=len(flow.getNeighbors(jj))-1:
		           if flow.getCellPImposed(flow.getNeighbors(jj)[gg])==True:
		               if (-totalflux[flow.getNeighbors(jj)[gg]])<initialvol[flow.getNeighbors(jj)[gg]]:
		                  neigh2[kk2]=flow.getNeighbors(jj)[gg]
		                  kk2=kk2+1
	           if kk2==0:
		      bubble=bubble+1
		      deltabubble+=deltaflux2
	           if kk2!=0:
 	              deltafluxEach2=deltaflux2/kk2
	              for xx in range(16):
		         if kk2!=0:
		           if neigh2[xx]<=nvoids:
			      gg=neigh2[xx]
		              if (-totalflux[gg]+deltafluxEach2)>initialvol[gg]: 
				 print 'check'
		              if (-totalflux[gg]+deltafluxEach2)<=initialvol[gg]: 
			         deltadelta+=deltafluxEach2
		                 totalflux[gg]+=-deltafluxEach2	
		                 kk2=kk2-1
		                 neighok2[kk2]=gg	
	              if deltaflux!=deltadelta:
		          print 'overcross'
		          deltaovercross+=+(deltaflux-deltadelta)
		 if kk!=0:
		   deltafluxEach2=(deltaflux-deltadelta)/kk
	           for xx in range(4):
		    if kk!=0:
		     if neigh[xx]<=nvoids:
		       jj=neigh[xx]
		       if jj!=neighok[0] and jj!=neighok[1] and jj!=neighok[2] and jj!=neighok[3]:
		          if (-totalflux[jj]+deltafluxEach2)<=initialvol[jj]:
		              totalflux[jj]+=-deltafluxEach2
		              deltadelta+=deltafluxEach2
		              kk=kk-1		              
		              neighok[kk]=jj
		   if deltaflux!=deltadelta:
		    print 'again'	
		    deltaagain+=+(deltaflux-deltadelta)
		    
   total=0		        
   for ii in range(nvoids):
      total+=totalflux[ii]
      if bar[ii]>=initiallevel:
	 if flow.getCellPImposed(ii)==True:
            if -(totalflux[ii])==initialvol[ii]:
	       flow.setCellPImposed(ii,False)
               #print 'pp_',ii
            if -(totalflux[ii])>initialvol[ii]:
	       flow.setCellPImposed(ii,False)
               print 'error_',ii
   total=abs(total)
   
 #  for ii in range(nvoids):
  #    if bar[ii]>=initiallevel:
#	 if flow.getCellPImposed(ii)==False:
  #          if -(totalflux[ii])!=initialvol[ii]:
   #            print 'error_',ii
               

file=open('Test.txt',"w")
checkdifference=0
def equilibriumtest():
   global F33,F22,checkdifference,errors
   #unbalanced=unbalancedForce()
   F33=abs(O.forces.f(flow.wallIds[flow.ymax])[1])
   F22=abs(O.forces.f(flow.wallIds[flow.ymin])[1])
   #F11 =abs(O.forces.f(flow.wallIds[flow.xmax])[0]),
   #F00=abs(O.forces.f(flow.wallIds[flow.xmin])[0]),
   #F44=abs(O.forces.f(flow.wallIds[flow.zmin])[2]),
   #F55=abs(O.forces.f(flow.wallIds[flow.zmax])[2]),
   deltaF=abs(F33-F22)
   file.write(str(O.iter)+" "+str(deltaF)+"\n")
   if O.time>=iterini+1.5:
     if checkdifference==0:
       print('check F done')
       if deltaF>0.01*press:
         raise YadeCheckError('Error: too high difference between forces acting at the bottom and upper walls')
         #O.pause()
       checkdifference=1
 

once=0
def fluxtest():  
   global once,QinOk
   no=0
   QinOk=-Qin-deltabubble
   error=QinOk-total
   if error>toleranceWarning:
      print "Warning: difference between total water volume flowing through bottom wall and water loss due to air bubble generations",QinOk," vs. total water volume flowing inside dry or partially saturated cells",total
   if error>toleranceCritical:
      raise YadeCheckError("The difference is more, than the critical tolerance!")
   file.write(str(O.time)+" "+str(QinOk)+" "+str(error)+" \n")
   for ii in range(nvoids):
     if bar[ii]>=initiallevel:
      if flow.getCellPImposed(ii)==True:
         no=1
   if once==0:
      if no==0:
         imbtime=O.time-iterini
         print(imbtime,voidvol,total,QinOk)
         if voidvol-total>toleranceWarning:
           print"Warning: initial volume of dry voids",voidvol," vs. total water volume flowing inside dry or partially saturated cells",total
         if voidvol-total>toleranceCritical:
           raise YadeCheckError("The difference is more, than the critical tolerance!")
         file.write(str(imbtime)+" "+str(voidvol)+" "+str(total)+" "+str(QinOk)+"\n")
         once=1
         timing.stats()
   

def addPlotData():
   global F33,F22,QinOk,total
   plot.addData(i1=O.iter,
      t=O.time,
      Fupper=F33,
      Fbottom=F22,
      Q=QinOk,
      T=total
   )
plot.live=True
plot.plots={'t':('Fbottom','Fupper'),' t ':('Q','T')} 
plot.plot()
      

def pl():
   flow.saveVtk("./vtk",True)

O.engines=O.engines+[PyRunner(iterPeriod=100,command='pl()')]
#O.engines=O.engines+[VTKRecorder(iterPeriod=100,recorders=['spheres'],fileName='./exp')]
O.engines=O.engines+[PyRunner(iterPeriod=1,command='equilibriumtest()')]
O.engines=O.engines+[PyRunner(iterPeriod=1,command='pressureImbibition()')]
O.engines=O.engines+[PyRunner(iterPeriod=1,command='fluxtest()')]
O.engines=O.engines+[PyRunner(iterPeriod=1,command='addPlotData()')]

O.timingEnabled=True
#file.close()
#plot.saveDataTxt('plots.txt',vars=('i1','t','Fupper','Fbottom','Q','T'))

import tempfile, shutil
dirpath = tempfile.mkdtemp()
for fileName in ['./vtk', './Test.txt' ]:
  if (os.path.exists(fileName)): shutil.move(fileName,dirpath)
  print("File %s moved into %s/ directory"%(fileName,dirpath))

