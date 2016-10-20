#CW BOON
#Users need to update outermostBodyId, midBodyId, upperBodyId by displaying the IDs of the body and re-run. Otherwise will receive error. 

p=BlockGen()
p.maxRatio = 5000.0;
p.minSize = 0.1;
p.density = 2395.5 	#kg/m3
p.dampingMomentum = 0.2
p.damp3DEC = 0.0
p.viscousDamping = 0.0
p.Kn = 0.192e9   	#1GPa please check this
p.Ks= 0.0768e9  	#1GPa please check this
p.frictionDeg = 40.0 #degrees
p.traceEnergy = False
p.defaultDt = 1e-4
p.rForPP = 0.4 #0.4
p.kForPP = 0.55
p.RForPP = 5800.0
p.gravity = [0,0,9.81]
p.inertiaFactor = 1.0
p.initialOverlap = 1e-6 + 0.076e6/p.Kn 
p.numberOfGrids = 100
p.exactRotation = False
p.shrinkFactor = 1.0
p.boundarySizeXmax = 1215.0; #North 1200
p.boundarySizeXmin = 0.0; #South
p.boundarySizeYmax = 0.5-p.rForPP; #East
p.boundarySizeYmin = -0.5+p.rForPP; #West
p.boundarySizeZmax = 200.0; #Down
p.boundarySizeZmin = -575.0; #Up
p.extremeDist = 50.5;
p.subdivisionRatio = 0.05;
p.persistentPlanes = False
p.jointProbabilistic = True
p.opening = False
p.boundaries = True
p.slopeFace = True
p.calJointLength = True
p.twoDimension = True
p.unitWidth2D = 1.0
p.intactRockDegradation = True
p.calAreaStep = 30
p.useFaceProperties = True
p.neverErase = False # Must be used when tension is on
p.peakDisplacement = 0.1
p.brittleLength = 1.0
p.maxClosure = 0.0003 
p.filenamePersistentPlanes = ''
p.filenameProbabilistic = './jointsSec2spacing/jointProbabilisticMatch40.csv'
p.filenameOpening = '' 
p.filenameBoundaries = './jointsSec2spacing/boundariesMatch.csv'
p.filenameSlopeFace ='' 
p.directionA=Vector3(1,0,0)
p.directionB=Vector3(0,1,0)
p.directionC=Vector3(0,0,1)
p.load()
O.engines[2].lawDispatcher.functors[0].initialOverlapDistance =  p.initialOverlap - 1e-6
O.engines[2].lawDispatcher.functors[0].allowBreakage = False

from yade import qt
from yade import plot
averageMass = 0.0
bodyNo = 0

O.engines[2].physDispatcher.functors[0].ks_i = 0.1e9
O.engines[2].physDispatcher.functors[0].kn_i = 2e9
O.engines[3].label='integration'

for b in O.bodies:
	if b.dynamic == True:
		averageMass += b.state.mass
		bodyNo = bodyNo+1
	b.state.blockedDOFs='yXZ'
	b.state.blockedDOFs='yXZ'

O.step()
for b in O.bodies:
	if b.state.mass < 0.045*averageMass/bodyNo:
		if b.isClumpMember == True:
			O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
		else:
			O.bodies.erase(b.id)



#O.engines=O.engines[:3]+[Buoyancy(waterLevel=90.0, waterNormal=Vector3(0,0,-1),iterPeriod=1,label='CalculateBuoyancyEngine')] + O.engines[3:]



O.engines = O.engines + [PotentialBlockVTKRecorderTunnel(fileName='./vtk/Vaiont2d',iterPeriod=20000,twoDimension=True,REC_INTERACTION=True,REC_VELOCITY=True,sampleX=50,sampleY=1,sampleZ=50,maxDimension=0.5,label='vtkRecorder')]

O.dt = 5.0e-4 #4.*3e-4
#O.engines = O.engines+ [PyRunner(iterPeriod=1,command='calTimeStep()')] 
O.engines = O.engines+ [PyRunner(iterPeriod=200,command='goToNextStage2()',label='dispChecker')]  #833 3333
#O.engines = O.engines+ [PyRunner(iterPeriod=250,command='goToNextStage2()',label='strengthReduction')] 
O.engines=O.engines+[PyRunner(iterPeriod=100,command='myAddPlotData()')]


targetFriction = 40.0
boundaryFriction = 40
outermostBodyId =16 #PLEASE CHECK
midBodyId = 149 #PLEASE CHECK
upperBodyId = 63  #PLEASE CHECK
originalPosition =  O.bodies[outermostBodyId].state.pos[0]
waterHeight = 90.0 #PLEASE CHECK
startCountingBrokenBonds = False


b0=Body()
b1=Body()
b2=Body()
b3=Body()
b4=Body()
wire=False
color=[0,0,1]
highlight=False
kPP = 0.01 #KnLin 0.35  #KnDiscrete 0.25
rPP = 4.0
RPP = 100.0
aPP0 = [-0.98962,1,0,0,0,0]
cPP0 = [-0.087,0,-1,1,0,0]
bPP0 = [0,0,0,0,1,-1]
dPP0 = [55,46,56,136,0.5,0.5] #[55,46,56,136,0.5,0.5]
aPP1 = [-1,1,-0.707,0,0,0]
cPP1 = [0,0,-0.707,1,0,0]
bPP1 = [0,0,0,0,1,-1]
dPP1 = [46,46,74,136,0.5,0.5]
aPP2 = [-1,1,-0.342,0,0,0]
cPP2 = [0,0,-0.93953,1,0,0]
bPP2 = [0,0,0,0,1,-1]
dPP2 = [46,46,163,136,0.5,0.5]
aPP3 = [-1,1,-0.7071,0,0,0]
cPP3 = [0,0,-0.7071,1,0,0]
bPP3 = [0,0,0,0,1,-1]
dPP3 = [46,46,170,136,0.5,0.5]
aPP4 = [-1,1,-0.4225,0,0,0]
cPP4 = [0,0,-0.906,1,0,0]
bPP4 = [0,0,0,0,1,-1]
dPP4 = [46,46,289,136,0.5,0.5]
isBoundaryPlane =[True,True,True,True,True,True]
minmaxAabb = Vector3(80,5.0,400.0)
phiR=[36,36,36,36,36,36]
phiB=[36,36,36,36,36,36]
blockJRC=[0,0,0,0,0,0]
blockJCS=[0,0,0,0,0,0]
blockAsperity=[0,0,0,0,0,0]
blockSigmaC=[0,0,0,0,0,0]
blockCohesion=[0,0,0,0,0,0]
blockTension=[0,0,0,0,0,0]
blockLambda=[0,0,0,0,0,0]
blockHC=[0,0,0,0,0,0]
blockHwater=[0,0,0,0,0,0]
jointNo=[0,0,0,0,0,0]
b0.shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP0, b=bPP0, c=cPP0, d=dPP0,id=1000,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,phi_r=phiR,phi_b=phiB,JRC=blockJRC,JCS=blockJCS,asperity=blockAsperity,sigmaC=blockSigmaC,cohesion=blockCohesion,tension=blockTension,lambda0=blockLambda,heatCapacity=blockHC,hwater=blockHwater,jointType=jointNo)
b1.shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP1, b=bPP1, c=cPP1, d=dPP1,id=1001,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,phi_r=phiR,phi_b=phiB,JRC=blockJRC,JCS=blockJCS,asperity=blockAsperity,sigmaC=blockSigmaC,cohesion=blockCohesion,tension=blockTension,lambda0=blockLambda,heatCapacity=blockHC,hwater=blockHwater,jointType=jointNo)
b2.shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP2, b=bPP2, c=cPP2, d=dPP2,id=1002,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,phi_r=phiR,phi_b=phiB,JRC=blockJRC,JCS=blockJCS,asperity=blockAsperity,sigmaC=blockSigmaC,cohesion=blockCohesion,tension=blockTension,lambda0=blockLambda,heatCapacity=blockHC,hwater=blockHwater,jointType=jointNo)
b3.shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP3, b=bPP3, c=cPP3, d=dPP3,id=1003,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,phi_r=phiR,phi_b=phiB,JRC=blockJRC,JCS=blockJCS,asperity=blockAsperity,sigmaC=blockSigmaC,cohesion=blockCohesion,tension=blockTension,lambda0=blockLambda,heatCapacity=blockHC,hwater=blockHwater,jointType=jointNo)
b4.shape=PotentialBlock(k=kPP, r=rPP , R=RPP,a=aPP4, b=bPP4, c=cPP4, d=dPP4,id=1004,isBoundary=True,color=color ,wire=wire,highlight=highlight,AabbMinMax=True,minAabb=minmaxAabb ,maxAabb=minmaxAabb,minAabbRotated=minmaxAabb ,maxAabbRotated=minmaxAabb,phi_r=phiR,phi_b=phiB,JRC=blockJRC,JCS=blockJCS,asperity=blockAsperity,sigmaC=blockSigmaC,cohesion=blockCohesion,tension=blockTension,lambda0=blockLambda,heatCapacity=blockHC,hwater=blockHwater,jointType=jointNo)

length=100.0
V=1.0e6
geomInert=(1./6.)*V*length*length
young = 600.0e6 # [N/m^2]
poisson = 0.6 
density = 2395
O.materials.append(FrictMat(young=young,poisson=poisson,density=density,frictionAngle=radians(36), label='wall'))
utils._commonBodySetup(b0,V,Vector3(geomInert,geomInert,geomInert), material='wall', pos=[0,0,0], dynamic=True, fixed=True)
utils._commonBodySetup(b1,V,Vector3(geomInert,geomInert,geomInert), material='wall', pos=[0,0,0],  dynamic=True, fixed=True)
utils._commonBodySetup(b2,V,Vector3(geomInert,geomInert,geomInert), material='wall', pos=[0,0,0],  dynamic=True, fixed=True)
utils._commonBodySetup(b3,V,Vector3(geomInert,geomInert,geomInert), material='wall', pos=[0,0,0],  dynamic=True, fixed=True)
utils._commonBodySetup(b4,V,Vector3(geomInert,geomInert,geomInert), material='wall', pos=[0,0,0],  dynamic=True, fixed=True)
b0.dynamic=False
b1.dynamic=False
b2.dynamic=False
b3.dynamic=False
b4.dynamic=False

b0.state.pos =[1375-85,0,60]
b1.state.pos =[1475-85,0,60]
b2.state.pos = [1575-85,0.0,60]
b3.state.pos = [1675-85,0.0,60]
b4.state.pos = [1775-85,0.0,60]
O.bodies.append(b0) 
O.bodies.append(b1) 
O.bodies.append(b2) 
O.bodies.append(b3) 
O.bodies.append(b4)

######################################
## Define shape of excavation shape ##
######################################
def openingTop1(x,z):
	centreX = 200
	centreZ = -700
	p1 =-(z-centreZ) -1000.0 #175
	if p1<0.0: 
		p1=0
	p2 = -(x-centreX) -200.0
	if p2<0.0: 
		p2=0
	p3 = (x-centreX) -180.0 #400
	if p3<0.0: 
		p3=0
	p4 = -0.3090*(x-centreX) + 0.9511*(z-centreZ) - 195.21 #196.41
	if p4<0.0: 
		p4=0
	return p1+p2+p3+p4


def openingTop2(x,z):
	centreX = 500
	centreZ = -700
	p1 =-(z-centreZ) -1000.0 #175
	if p1<0.0: 
		p1=0
	p2 = -(x-centreX) -120.0
	if p2<0.0: 
		p2=0
	p3 = (x-centreX) -240.0 #400
	if p3<0.0: 
		p3=0
	#p4 = -0.4695*(x-centreX) + 0.8829*(z-centreZ) - 297.9
	p4 = -0.5373*(x-centreX) + 0.8434*(z-centreZ) - 285.9
	if p4<0.0: 
		p4=0
	return p1+p2+p3+p4


def openingBtm1(x,z):
	centreX = 800
	centreZ = -700
	p1 =-(z-centreZ) -1000.0 #175
	if p1<0.0: 
		p1=0
	p2 = -(x-centreX) -60.0
	if p2<0.0: 
		p2=0
	p3 = (x-centreX) -175.0 #400
	if p3<0.0: 
		p3=0
	p4 =  (z-centreZ) - 465.0
	if p4<0.0: 
		p4=0
	return p1+p2+p3+p4



def openingBtm2(x,z):
	centreX = 1100
	centreZ = -700
	p1 =-(z-centreZ) -1000.0 #175
	if p1<0.0: 
		p1=0
	p2 = -(x-centreX) -125.0
	if p2<0.0: 
		p2=0
	p3 = (x-centreX) -200.0 #400
	if p3<0.0: 
		p3=0
	#p4 = -0.6157*(x-centreX) + 0.778*(z-centreZ) - 443.8
	p4 = -0.6293*(x-centreX) + 0.777*(z-centreZ) - 437
	if p4<0.0: 
		p4=0
	return p1+p2+p3+p4


###############################
## Create excavation opening ##
###############################
def excavate():
	for b in O.bodies:
		if b.dynamic==True:
			if BottomSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) ==0:
				if b.isClumpMember == True:
					O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
				elif b.isClump == False:
					O.bodies.erase(b.id)
			if HorizontalSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2])==0:
				if b.isClumpMember == True:
					O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
				elif b.isClump == False:
					O.bodies.erase(b.id)
			if MidSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) ==0:
				if b.isClumpMember == True:
					O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
				elif b.isClump == False:
					O.bodies.erase(b.id)
			if CrestSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) ==0:
				if b.isClumpMember == True:
					O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
				elif b.isClump == False:
					O.bodies.erase(b.id)
			if b.state.pos[0]>1500.0:
				if b.isClumpMember == True:
					O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
				elif b.isClump == False:
					O.bodies.erase(b.id)
			if b.state.pos[2]>200.0:
				if b.isClumpMember == True:
					O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
				elif b.isClump == False:
					O.bodies.erase(b.id)
			if b.state.pos[2]<-600.0:
				if b.isClumpMember == True:
					O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
				elif b.isClump == False:
					O.bodies.erase(b.id)
			#O.bodies.erase(50) #49
			

def reduceFric(fric):
	global boundaryFriction
	boundaryFriction = fric
	for i in O.interactions:
		if i.id1 == 2 or i.id2 == 2 or i.id1 ==1 or i.id2==1 or i.id1 == 3 or i.id2 ==3:
			i.phys.phi_r = fric
			i.phys.phi_b = fric
	temp = []
	for i in range(len(O.bodies[2].shape.phi_r)):
		temp.append(fric)
	O.bodies[2].shape.phi_r = temp
	temp = []
	for i in range(len(O.bodies[1].shape.phi_r)):
		temp.append(fric)
	O.bodies[1].shape.phi_r = temp	
	temp = []
	for i in range(len(O.bodies[3].shape.phi_r)):
		temp.append(fric)
	O.bodies[3].shape.phi_r = temp	
	temp = []
	for i in range(len(O.bodies[2].shape.phi_b)):
		temp.append(fric)
	O.bodies[2].shape.phi_b = temp
	temp = []
	for i in range(len(O.bodies[1].shape.phi_b)):
		temp.append(fric)
	O.bodies[1].shape.phi_b = temp	
	temp = []
	for i in range(len(O.bodies[3].shape.phi_b)):
		temp.append(fric)
	O.bodies[3].shape.phi_b = temp	



def reduceCohesion(cohesion):
	for i in O.interactions:
		if i.id1 != 2 and i.id2 != 2 and i.id1 !=1 and i.id2!=1 and i.id1 !=3 and i.id2 !=3:
			i.phys.cohesion = cohesion
			temp = []
			for j in range(len(O.bodies[i.id1].shape.cohesion)):
				temp.append(cohesion)
			O.bodies[i.id1].shape.cohesion = temp
			temp = []
			for j in range(len(O.bodies[i.id2].shape.cohesion)):
				temp.append(cohesion)
			O.bodies[i.id2].shape.cohesion = temp
	
	
def reduceTension(tension):
	for i in O.interactions:
		if i.id1 != 2 and i.id2 != 2 and i.id1 !=1 and i.id2!=1 and i.id1 !=3 and i.id2 !=3:
			i.phys.tension = tension
			temp = []
			for j in range(len(O.bodies[i.id1].shape.tension)):
				temp.append(tension)
			O.bodies[i.id1].shape.tension = temp
			temp = []
			for j in range(len(O.bodies[i.id2].shape.tension)):
				temp.append(tension)
			O.bodies[i.id2].shape.tension = temp	


def removeDamping():
	for i in O.interactions:
		i.phys.viscousDamping = 0.1
	O.engines[2].physDispatcher.functors[0].viscousDamping = 0.1
	integration.damping= 0.0
	for i in O.interactions:
		i.phys.cumulative_us = 0.0


def addDamping():
	for i in O.interactions:
		i.phys.viscousDamping = 0.2
	O.engines[2].physDispatcher.functors[0].viscousDamping = 0.2
	O.engines[4].damping= 0.0


def activateCohesion():
	#O.engines[2].lawDispatcher.functors[0].neverErase = True
	O.engines[2].lawDispatcher.functors[0].allowBreakage = True
	global startCountingBrokenBonds
	global initBondedContacts
	initBondedContacts = 0
	startCountingBrokenBonds = True
	for i in O.interactions:
		if O.bodies[i.id1].shape.isBoundary == False and O.bodies[i.id2].shape.isBoundary == False and i.isReal==True:
			i.phys.cohesionBroken = False
			initBondedContacts = initBondedContacts + 1
	#O.engines[2].physDispatcher.functors[0].cohesionBroken = False


def activateTension():
	#O.engines[2].lawDispatcher.functors[0].neverErase = True
	O.engines[2].lawDispatcher.functors[0].allowBreakage = True
	global startCountingBrokenBonds
	startCountingBrokenBonds = True
	for i in O.interactions:
		if O.bodies[i.id1].shape.isBoundary == False and O.bodies[i.id2].shape.isBoundary == False and i.isReal==True:
			i.phys.tensionBroken = False
	#O.engines[2].physDispatcher.functors[0].tensionBroken = False


def reduceHw(hw):
	global waterHeight 
	waterHeight = hw
	CalculateBuoyancyEngine.waterLevel = hw


def setHeatCapacity(heatCapacity): 
	for i in O.interactions:
		if i.id1 == 2 or i.id2 == 2:
			i.phys.heatCapacities = heatCapacity
	temp = []
	for i in range(len(O.bodies[2].shape.heatCapacity)):
		temp.append(heatCapacity)
	O.bodies[2].shape.heatCapacity = temp


def calTimeStep():
	mkratio = 99999999.9
	maxK = 0.0
	minMass = 1.0e15
	for i in O.interactions:
		if i.isReal==True:
			if i.phys.Knormal_area > maxK:
				maxK = i.phys.Knormal_area 
			if i .phys.Kshear_area > maxK:
				maxK = i.phys.Kshear_area
	for b in O.bodies:
		if b.state.mass < minMass:
			minMass = b.state.mass
	O.dt = 0.2*sqrt(minMass/maxK) 


def reduceFricSharp(fric):
	global boundaryFriction
	boundaryFriction = fric
	for i in O.interactions:
		if i.id1 == 2 or i.id2 == 2 or i.id1 ==1 or i.id2==1:
			i.phys.phi_r = fric
			i.phys.phi_b = fric
	temp = []
	for i in range(len(O.bodies[2].shape.phi_r)):
		temp.append(fric)
	O.bodies[2].shape.phi_r = temp
	temp = []
	for i in range(len(O.bodies[1].shape.phi_r)):
		temp.append(fric)
	O.bodies[1].shape.phi_r = temp	
	temp = []
	for i in range(len(O.bodies[2].shape.phi_b)):
		temp.append(fric)
	O.bodies[2].shape.phi_b = temp
	temp = []
	for i in range(len(O.bodies[1].shape.phi_b)):
		temp.append(fric)
	O.bodies[1].shape.phi_b = temp	



def countRealContacts():
	countReal = 0
	for i in O.interactions:
		if i.isReal == True:
			countReal = countReal + 1
	print countReal

def resetShearDisplacement():
	global originalPosition
	global outermostBodyId
	for i in O.interactions:
		if i.isReal==True:
			i.phys.cumulative_us=0.0
	originalPosition = O.bodies[outermostBodyId].state.pos[0]

def goToNextStage():
	removeDamping()
	#activateCohesion()
	#activateTension()
	reduceFric(17.2)
	strengthReduction.iterPeriod=10000000000


prevDistance = O.bodies[outermostBodyId].state.pos[0]
prevDispRate = 0.0
tolDistance = 0.003
tolDelKE = 1.0
prevKE = 0.0
currentKE = 0.0
iterWait = 0
prevKErate = 0.0
initBondedContacts = 0 

def changeTolDistance(tol):
	global tolDistance
	tolDistance = tol

def changeTolDelKE(tol):
	global tolDelKE
	tolDelKE = tol



def goToNextStage2():
	global outermostBodyId
	global midBodyId
	global upperBodyId
	global boundaryFriction		
	global targetFriction
	global prevDistance
	global originalPosition
	global tolDistance
	global startCountingBrokenBonds
	global checkInterval
	global prevKE
	global currentKE
	global tolDelKE
	global iterWait
	global prevDispRate
	global prevKErate
	prevKE = currentKE
	KE = utils.kineticEnergy()
	currentKE = KE
	dPhi = 0.0000125
	if startCountingBrokenBonds == False:
		if O.iter>12000: #5s
		#if abs(O.bodies[outermostBodyId].state.pos[0] - originalPosition - prevDistance) <tolDistance:
			activateCohesion()
			activateTension()
			dispChecker.iterPeriod=1
			removeDamping()
			return
	else:	
		dispChecker.iterPeriod=1
		if abs(O.bodies[outermostBodyId].state.pos[0] - originalPosition ) > 20.0:
			vtkRecorder.iterPeriod=5000
		if abs(O.bodies[outermostBodyId].state.pos[0] - originalPosition ) >120.0:
			O.pause()
		if boundaryFriction > 10.0:
			boundaryFriction = boundaryFriction-dPhi
			reduceFric(boundaryFriction)
			#if boundaryFriction < 35.000001 and boundaryFriction > 34.999999:
			#	dispChecker.iterPeriod=20000
			#elif boundaryFriction < 30.000001 and boundaryFriction > 29.999999:
			#	dispChecker.iterPeriod=20000
			#elif boundaryFriction < 25.000001 and boundaryFriction > 24.999999:
			#	dispChecker.iterPeriod=20000
		else:
			O.pause()
	prevDispRate = O.bodies[outermostBodyId].state.pos[0] - originalPosition - prevDistance
	prevKErate = currentKE-prevKE
	prevDistance = O.bodies[outermostBodyId].state.pos[0]  - originalPosition



def goToNextStage3():
	global waterHeight
	global boundaryFriction
	KE = 0.0
	normKE = 0.0
	for b in O.bodies:
		if b.dynamic==True:
			KE = KE+0.5*b.state.mass*b.state.vel.squaredNorm()
			normKE = normKE+0.5*b.state.mass
	normalizedKE = KE/normKE
	if utils.unbalancedForce()<0.005 and normalizedKE<1e-7:
		waterHeight = waterHeight + 10.0
		reduceHw(waterHeight)



def BottomSlope(x,y,z): 
	Xcentre = 920.0
	Ycentre = 0.0
	Zcentre = -230.0
	dip = 38.0
	dipdir = 0.0
	dipdirN = 0.0
	dipN = 90.0-dip
	if dipdir > 180.0:
		dipdirN = dipdir - 180.0
	else:
		dipdirN = dipdir + 180.0
	dipRad = dipN/180.0*pi
	dipdirRad = dipdirN/180.0*pi
	a = cos(dipdirRad)*cos(dipRad)
	b = sin(dipdirRad)*cos(dipRad)
	c = sin(dipRad)
	l = sqrt(a*a + b*b +c*c)
	a = a/l
	b = b/l
	c = c/l
	d = a*Xcentre + b*Ycentre + c*Zcentre
	plane = a*x +b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###Back###
	p1 = -x+920.0
	if p1 < 0.0:
		p1= 0.0
	###Front###
	p2 = x-1500.0
	if p2 < 0.0:
		p2 = 0.0
	###Top###
	p3 = -z-400.0
	if p3 < 0.0:
		p3 = 0.0
	return plane + p1 + p2 +p3


def HorizontalSlope(x,y,z): 
	Xcentre = 920.0
	Ycentre = 0.0
	Zcentre = -230.0
	dip = 0.0
	dipdir = 0.0
	dipdirN = 0.0
	dipN = 90.0-dip
	if dipdir > 180.0:
		dipdirN = dipdir - 180.0
	else:
		dipdirN = dipdir + 180.0
	dipRad = dipN/180.0*pi
	dipdirRad = dipdirN/180.0*pi
	a = cos(dipdirRad)*cos(dipRad)
	b = sin(dipdirRad)*cos(dipRad)
	c = sin(dipRad)
	l = sqrt(a*a + b*b +c*c)
	a = a/l
	b = b/l
	c = c/l
	d = a*Xcentre + b*Ycentre + c*Zcentre
	plane = a*x +b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###Back###
	p1 = -x+685 #697.0
	if p1 < 0.0:
		p1= 0.0
	###Front###
	p2 = x-1500.0
	if p2 < 0.0:
		p2 = 0.0
	return plane + p1 + p2 


def MidSlope(x,y,z): 
	Xcentre = 352.0
	Ycentre = 0.0
	Zcentre = -445.0
	dip = 33.0
	dipdir = 0.0
	dipdirN = 0.0
	dipN = 90.0-dip
	if dipdir > 180.0:
		dipdirN = dipdir - 180.0
	else:
		dipdirN = dipdir + 180.0
	dipRad = dipN/180.0*pi
	dipdirRad = dipdirN/180.0*pi
	a = cos(dipdirRad)*cos(dipRad)
	b = sin(dipdirRad)*cos(dipRad)
	c = sin(dipRad)
	l = sqrt(a*a + b*b +c*c)
	a = a/l
	b = b/l
	c = c/l
	d = a*Xcentre + b*Ycentre + c*Zcentre
	plane = a*x +b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###Back###
	p1 = -x+352.0
	if p1 < 0.0:
		p1= 0.0
	###Front###
	p2 = x-685.0 #697.0
	if p2 < 0.0:
		p2 = 0.0
	###Top###
	return plane + p1 + p2 



def CrestSlope(x,y,z): 
	Xcentre = 352.0
	Ycentre = 0.0
	Zcentre = -445.0
	dip = 19.0
	dipdir = 0.0
	dipdirN = 0.0
	dipN = 90.0-dip
	if dipdir > 180.0:
		dipdirN = dipdir - 180.0
	else:
		dipdirN = dipdir + 180.0
	dipRad = dipN/180.0*pi
	dipdirRad = dipdirN/180.0*pi
	a = cos(dipdirRad)*cos(dipRad)
	b = sin(dipdirRad)*cos(dipRad)
	c = sin(dipRad)
	l = sqrt(a*a + b*b +c*c)
	a = a/l
	b = b/l
	c = c/l
	d = a*Xcentre + b*Ycentre + c*Zcentre
	plane = a*x +b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###Front###
	p1 = x-352.0
	if p1 < 0.0:
		p1 = 0.0
	###Top###
	return plane + p1 


def saveData():
	plot.saveDataTxt('Vaiont_section2'+'.txt')


def afterIterFunc():
	O.wait()
	O.run()

def myAddPlotData():
	global outermostBodyId
	global midBodyId
	global upperBodyId 
	global originalPosition
	global waterHeight
	global startCountingBrokenBonds
	global initBondedContacts
	global boundaryFriction
	O.pause()  # wait till the iteration really finishes
	O.wait()
	uf=utils.unbalancedForce()
	KE=utils.kineticEnergy()
	cohesionBrokenCount = 0
	tensionBrokenCount = 0
	velExtreme = O.bodies[outermostBodyId].state.vel.norm()
	angVelExtreme = O.bodies[outermostBodyId].state.angVel.norm()
	displacement = O.bodies[outermostBodyId].state.pos[0] - originalPosition
	us = 0.0
	mobilizedShear = 0.0
	mobilizedShearMid = 0.0
	mobilizedShearTop = 0.0
	if O.interactions[1,outermostBodyId].isReal==True:
		#us= O.interactions[1,outermostBodyId].phys.cumulative_us
		mobilizedShear = O.interactions[1,outermostBodyId].phys.mobilizedShear
	if O.interactions[2,midBodyId].isReal==True:
		mobilizedShearMid = O.interactions[2,midBodyId].phys.mobilizedShear
	if O.interactions[3,upperBodyId].isReal==True:
		mobilizedShearTop = O.interactions[3,upperBodyId].phys.mobilizedShear
	if startCountingBrokenBonds==True:
		for i in O.interactions:
			if i.id1 != 1 and i.id2 !=1 and i.id1 !=2 and i.id2 !=2 and i.id1!=3 and i.id2 !=3:
				if i.phys.cohesionBroken == False:
					cohesionBrokenCount = cohesionBrokenCount + 1
				if i.phys.tensionBroken == False:
					tensionBrokenCount = tensionBrokenCount + 1
		cohesionBrokenCount = initBondedContacts - cohesionBrokenCount
		tensionBrokenCount = initBondedContacts  - tensionBrokenCount 
	plot.addData(timeStep=O.iter,unbalancedForce=uf,timeStep2=O.iter,timeStep3=O.iter,timeStep4=O.iter,kineticEn=KE,time=O.time,angVel=angVelExtreme,velocity=velExtreme,time2=O.time,slopeDisplacement=displacement,distanceTravelled=displacement,phys_us=us,waterLevel=waterHeight,cohesion_broken_no=cohesionBrokenCount,cohesion_broken_no2=cohesionBrokenCount,tension_broken_no=tensionBrokenCount,waterLevel2=waterHeight,mobilized_phi=mobilizedShear,boundary_phi=boundaryFriction,boundary_phi2=boundaryFriction,mobilized_shear_bottom=mobilizedShear,mobilized_shear_mid=mobilizedShearMid,mobilized_shear_top=mobilizedShearTop)
	O.run()

plot.plots={'timeStep':('unbalancedForce'),'timeStep2':('kineticEn'),'time2':('cohesion_broken_no2'),'boundary_phi2':('slopeDisplacement'),'time':(('velocity'),('angVel','go-')),'timeStep3':(('distanceTravelled'),('phys_us','go-')),'waterLevel':('cohesion_broken_no'),'waterLevel2':(('mobilized_phi','go-'),),'boundary_phi':(('mobilized_shear_bottom','go-'),('mobilized_shear_mid','bo-'),('mobilized_shear_top','ro-'),)}


'''
f = open('./'+'investigateDamping.txt','w')
f.write('timeStep\tid1\tid2\tshearForce\tnormalForce\tmobilisedShear\tcalculatedMS\n')
f.close()
def record():
	for i in O.interactions:
		if (O.bodies[i.id1].shape.isBoundary == True or O.bodies[i.id2].shape.isBoundary == True) and i.isReal==True:
			shearForce = i.phys.shearForce.norm()
			normalForce = i.phys.normalForce.norm()
			mobilisedShear = i.phys.mobilizedShear
			calculatedMS = shearForce/(normalForce*tan(radians(40.0)))
			f = open('./'+'investigateDamping.txt','a')
			f.write(str(O.iter)+'\t'
		+str(i.id1)+'\t'
		+str(i.id2) + '\t'
		+str(shearForce) + '\t'
		+str(normalForce) + '\t'
		+str(mobilisedShear) + '\t'
		+str(calculatedMS) + '\t'
		+'\n')	
			f.close()

'''


O.bodies[1].shape.fixedNormal = True
O.bodies[2].shape.fixedNormal = True
O.bodies[3].shape.fixedNormal = True
O.bodies[1].shape.boundaryNormal = Vector3(0,0,-1)
O.bodies[2].shape.boundaryNormal = Vector3(0.2334,0,-0.9724) 
O.bodies[3].shape.boundaryNormal = Vector3(0.609,0,-0.794)
O.step()
excavate()
O.step()
O.step()
#calTimeStep()
O.engines[2].physDispatcher.functors[0].ks_i = 0.1e9
O.engines[2].physDispatcher.functors[0].kn_i = 2e9
for i in O.interactions:
	i.phys.ks_i =0.1e9
	i.phys.kn_i =2e9

O.run(3500000)
#O.run(6000)
