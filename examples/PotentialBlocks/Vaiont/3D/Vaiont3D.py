#CW BOON
#Users need to update westBodyId, midBodyId, eastBodyID by displaying the IDs of the body and re-run. Otherwise will receive error. 

name ='fullJoint40degWetBasic80softeningKs1' # PLEASE CHECK THIS  

p=BlockGen()
p.maxRatio = 1000.0;
p.minSize =10.0;
p.density = 2395.5 # 23.6/9.81*1000.0 	#kg/m3
p.dampingMomentum = 0.8
p.damp3DEC = 0.0
p.viscousDamping = 0.0
p.Kn = 0.096e9  	#1GPa
p.Ks= 0.0384e9 	#1GPa
p.frictionDeg = 36.0 #degrees
p.traceEnergy = False
p.defaultDt = 1e-4
p.rForPP = 0.4 #0.04
p.kForPP = 0.55
p.RForPP = 1800.0
p.gravity = [0,0,9.81]
p.inertiaFactor = 1.0
p.initialOverlap = 1e-6 + 0.076e+06/p.Kn
p.numberOfGrids = 100
p.exactRotation = False
p.shrinkFactor = 1.0
p.boundarySizeXmin = 0.0; #South
p.boundarySizeXmax = 2970.0; #North
p.boundarySizeYmin = 0.0; #West
p.boundarySizeYmax = 4100; #East 4100
p.boundarySizeZmin = -1400.0; #Up
p.boundarySizeZmax = 400.0; #Down
p.extremeDist = 50.5;
p.subdivisionRatio = 0.05;
p.persistentPlanes = False
p.jointProbabilistic = True
p.opening = False
p.boundaries = True
p.slopeFace = False
p.calJointLength = False
p.twoDimension = False
p.unitWidth2D = 9.0
p.intactRockDegradation = True
p.calAreaStep = 20.0
p.useFaceProperties = True
p.neverErase = False # Must be used when tension is on
p.peakDisplacement = 0.1
p.brittleLength = 1.0
p.maxClosure = 0.0003 
p.sliceBoundaries = True
p.filenamePersistentPlanes = ''
p.filenameProbabilistic = './joints/jointProbabilistic40basic80Fast.csv'  #  PLEASE CHEK THIS  
p.filenameOpening = '' #'./joints/opening.csv'
p.filenameBoundaries = './joints/boundaries.csv'
p.filenameSlopeFace =''
p.filenameSliceBoundaries = './joints/sliceBoundaries.csv'
p.directionA=Vector3(1,0,0)
p.directionB=Vector3(0,1,0)
p.directionC=Vector3(0,0,1)
p.load()
O.engines[2].lawDispatcher.functors[0].initialOverlapDistance = p.initialOverlap - 1e-6
O.engines[2].lawDispatcher.functors[0].allowBreakage = False
#Gl1_PotentialBlock.sizeX = 50
#Gl1_PotentialBlock.sizeY = 50
#Gl1_PotentialBlock.sizeZ = 50

O.engines[2].physDispatcher.functors[0].ks_i = 0.1e9
O.engines[2].physDispatcher.functors[0].kn_i = 2e9

from yade import plot

rockFriction = 40.0
boundaryFriction = 40.0
targetFriction = 40.0
waterHeight = 460.0  # PLEASE CHEK THIS 460.0 # 
startCountingBrokenBonds = False
minTimeStep = 1000000.0
westBodyId=291 #PLEASE CHEK THIS  
midBodyId =652 #PLEASE CHEK THIS  
eastBodyId =887 #PLEASE CHEK THIS  
originalPositionW = O.bodies[westBodyId].state.pos
originalPositionE = O.bodies[eastBodyId].state.pos
originalPositionM = O.bodies[midBodyId].state.pos
velocityDependency = False

O.engines[3].label='integration'
O.dt = 10.0e-4 #10e-4
O.engines = O.engines + [PotentialBlockVTKRecorderTunnel(fileName='./vtk/Vaiont3d',iterPeriod=2000000,twoDimension=False,sampleX=50,sampleY=50,sampleZ=50,maxDimension=100.0,REC_INTERACTION=True,REC_VELOCITY=True,label='vtkRecorder')]
O.engines=O.engines[:3]+[Buoyancy(waterLevel=waterHeight, waterNormal=Vector3(0,0,-1),iterPeriod=1,label='CalculateBuoyancyEngine')] + O.engines[3:]
#O.engines = O.engines+ [PyRunner(iterPeriod=1,command='calTimeStep()')] 
O.engines=O.engines+[PyRunner(iterPeriod=500,command='myAddPlotData()')]

O.engines = O.engines+ [PyRunner(iterPeriod=200,command='goToNextStage2()',label='dispChecker')] 


O.step()

for b in O.bodies:
	if b.shape.isBoundary == True:
		if b.state.pos[1] >2690: #2172.0:
			b.shape.isBoundary =  False
#			b.shape.isEastBoundary =  True
	#	b.dynamic=False
	if b.shape.volume < 8000.0: #30x30x30
		if b.isClumpMember == True:
			O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
		elif b.isClump == False:
			O.bodies.erase(b.id)
	b.state.refPos = b.state.pos



######################################
## Define shape of excavation shape ##
######################################
def EastCleft(x,y,z):
	Xcentre = 931.0 #790.0
	Ycentre = 2922.0 #2300.0
	Zcentre = -1250.0 #1150.0
	dip = 30.0 #23.0
	dipdir = 40.0 #310.0
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
	###West###
	p1 = -y + 2172.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0 #-0.13917*x+0.99*y -(-0.13917*1683+0.99*2690 ) #- 2690.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 


def EastUpperSlope(x,y,z): 
	Xcentre = 689.0
	Ycentre = 2689.0
	Zcentre = -1380.0
	dip = 30.0
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
	###West###
	p1 =-y + 2172.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0 # -0.13917*x+0.99*y -(-0.13917*1683+0.99*2690 ) #- 2690.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 



def EastmostSlope(x,y,z):
	Xcentre = 1450.0 #931.0
	Ycentre = 2689.0 #2922.0
	Zcentre = -1020.0 #-1240
	dip = 45.0 #30.0
	dipdir = 40.0 #40.0
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
	###West###
	p1 = -y + 2172.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0 # -0.13917*x+0.99*y -(-0.13917*1683+0.99*2690 ) #- 2690.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 



def EastShaveSlope(x,y,z):
	Xcentre = 689.0
	Ycentre = 3589.0 
	Zcentre = -1350.0 
	dip = 34.0 
	dipdir = 10.0 
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
	###West###
	p1 = -y + 2172.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0 # -0.13917*x+0.99*y -(-0.13917*1683+0.99*2690 ) #- 2690.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 


def goToNextStage():
	removeDamping()
	activateCohesion()
	activateTension()
	reduceFric2(19.0)
	dispChecker.iterPeriod=10000000000


def EastBottomSlope(x,y,z):  
	Xcentre = 2040.0
	Ycentre = 2400.0
	Zcentre = -500.0
	dip = 55.0
	dipdir = 5.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = -y + 2172.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0 #-0.13917*x+0.99*y -(-0.13917*1683+0.99*2690 ) #- 2690.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 


def WestCrest(x,y,z):
	Xcentre = 1215
	Ycentre = 1089
	Zcentre = -1080
	dip = 23.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = 0.0 # 0.13917*x - 0.99*y  -(0.13917*1747 - 0.99*780) #+780.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y -1350.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x-1549.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 


def WestUpperSlope(x,y,z):
	Xcentre = 1540
	Ycentre = 1163
	Zcentre = -870
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
		###West###
	p1 = 0.0 #0.13917*x - 0.99*y  -(0.13917*1747 - 0.99*780) #+780.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y -1350.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x-1549.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 



def WestHorizontal(x,y,z):
	Xcentre = 1649
	Ycentre = 1163
	Zcentre = -860
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
		###West###
	p1 = 0.0 # 0.13917*x - 0.99*y  -(0.13917*1747 - 0.99*780) #+780.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y -1350.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = -x + 1549
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x-1769.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 


def WestBottomSlope(x,y,z):
	Xcentre = 1769
	Ycentre = 1163
	Zcentre = -860
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
		###West###
	p1 = 0.0 #0.13917*x - 0.99*y  -(0.13917*1747 - 0.99*780) #+780.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y -1350.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = -x + 1769
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 

def MidLeftUpperSlope(x,y,z):
	Xcentre = 1529
	Ycentre = 1452
	Zcentre = -850
	dip = 27.0
	dipdir = 20.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = -y  + 1350.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y  - 1557.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x-1549.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 
	


def MidCentreUpperSlope(x,y,z):
	Xcentre = 1529
	Ycentre = 1632
	Zcentre = -830
	dip = 22.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = -y  + 1557.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y   - 1724.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x-1549.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 


def MidRightUpperSlope(x,y,z):
	Xcentre = 1529
	Ycentre = 1822
	Zcentre = -800
	dip = 28.0
	dipdir = 340.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = -y  +1724.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y  - 2172.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x-1649.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 



def MidHorizontal(x,y,z):
	Xcentre = 1649
	Ycentre = 1622
	Zcentre = -830
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = -y  + 1350.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y  - 2172.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = -x + 1549.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x - 17690
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 


def MidBottomSlope(x,y,z):
	Xcentre = 1769.0
	Ycentre = 1622.0
	Zcentre = -830.0
	dip = 40.0
	dipdir = 8.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = -y  + 1350.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = y - 2172.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = -x + 1600
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 	




def Staircase(x,y,z):
	Xcentre = 584
	Ycentre = 2486
	Zcentre = -960
	dip = 24.0
	dipdir = 316.0
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
	plane = -a*x- b*y -c*z + d
	if plane < 0.0:
		plane = 0.0
	return plane 


def fixStaircase():
	for b in O.bodies:
		if Staircase(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			b.dynamic = False
			


def LeftBoundary(x,y,z):
	Xcentre = 1760
	Ycentre = 2400
	Zcentre = -800
	dip = 24.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = 0.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0 #-0.13917*x + 0.99*y -(-0.13917*1747 + 0.99*780 ) #- 780.0 
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x - 2195.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 	


def RightBoundary(x,y,z):
	Xcentre = 1760
	Ycentre = 2400
	Zcentre = -800
	dip = 24.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 = 0.0 #0.13917*x - 0.99*y -( 0.13917*1683 - 0.99*2690) # + 2690.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = 0.0
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = x - 2195.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 	




def OppositeReservoir(x,y,z):
	Xcentre = 2295
	Ycentre = 1667
	Zcentre = -590
	dip = 40.0
	dipdir = 178.0
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
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	###Bounding region#####
	###West###
	p1 =  0.0
	if p1 < 0.0:
		p1 = 0.0
	###East###
	p2 = 0.0
	if p2 < 0.0:
		p2 = 0.0
	###Back###
	p3 = -x + 2195
	if p3 < 0.0:
		p3= 0.0
	###Front###
	p4 = 0.0
	if p4 < 0.0:
		p4 = 0.0
	###Top###
	return plane + p1 + p2 +p3 + p4 		



def resetShearDisplacement():
	global originalPositionW
	global westBodyId
	for i in O.interactions:
		if i.isReal==True:
			i.phys.u_cumulative=0.0
	originalPositionW = O.bodies[westBodyId].state.pos[0]





def calTimeStep():
	global minTimeStep
	mkratio = 99999999.9
	maxK = 0.0
	minMass = 1.0e15
	for i in O.interactions:
		if i.isReal==True:
			dt1 = O.bodies[i.id1].state.mass/i.phys.Knormal_area
			dt2 = O.bodies[i.id2].state.mass/i.phys.Knormal_area
			if dt1 < dt2:
				presentDt = 0.2*sqrt(dt1)
				if minTimeStep > presentDt:
					minTimeStep = presentDt
					O.dt = minTimeStep
			else:
				presentDt = 0.2*sqrt(dt2)
				if minTimeStep > presentDt:
					minTimeStep = presentDt
					O.dt = minTimeStep
			


def openingValley(x,y,z):
	Xcentre1 = 2130.0 	
	Ycentre1 = 1653.0
	Zcentre1 = -590.0
	dip = 85.0
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
	d = a*Xcentre1 + b*Ycentre1 + c*Zcentre1
	plane = a*x+ b*y +c*z - d
	if plane < 0.0:
		plane = 0.0
	Xcentre2 = 2230.0 
	Ycentre2 = 1667.0
	Zcentre2 = -590.0
	dip = 75.0
	dipdir = 176.0
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
	d = a*Xcentre2 + b*Ycentre2 + c*Zcentre2
	plane2 = a*x+ b*y +c*z - d
	if plane2 < 0.0:
		plane2 = 0.0
	return plane + plane2


def openingRunOut(x,y,z):
	f = 1.0
	if x > 1870:
		f= 0.0
	return f


###############################
## Create excavation opening ##
###############################
def excavateAll():
	for b in O.bodies:
		if b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)

def excavate():
	for b in O.bodies:
		#if openingRunOut(b.state.pos[0],b.state.pos[1],b.state.pos[2]) ==0 and b.dynamic == True:
		#	O.bodies.erase(b.id)
		if openingValley(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if b.state.pos[2]>500:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		#if EastCleft(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
		#	if b.isClumpMember == True:
		#		O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
		#	elif b.isClump == False:
		#		O.bodies.erase(b.id)
		if EastBottomSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if EastUpperSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if EastShaveSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		#if EastmostSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
		#	if b.isClumpMember == True:
		#		O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
		#	elif b.isClump == False:
		#		O.bodies.erase(b.id)
		if WestBottomSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if WestUpperSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if WestHorizontal(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if WestCrest(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if MidRightUpperSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if MidCentreUpperSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if MidLeftUpperSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if MidHorizontal(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue
		if MidBottomSlope(b.state.pos[0],b.state.pos[1],b.state.pos[2]) <0.001 and b.dynamic == True:
			if b.isClumpMember == True:
				O.bodies.deleteClumpMember(O.bodies[b.clumpId], b)
			elif b.isClump == False:
				O.bodies.erase(b.id)
				continue


def countRealContacts():
	countReal = 0
	for i in O.interactions:
		if i.isReal == True:
			countReal = countReal + 1
	print countReal

def saveData():
	plot.saveDataTxt('Vaiont_section3D'+'.txt')

def removeDamping():
	for i in O.interactions:
		i.phys.viscousDamping = 0.1
		i.phys.cumulative_us = 0.0
	O.engines[2].physDispatcher.functors[0].viscousDamping = 0.1
	integration.damping= 0.0
	
def allowBreakage():
	O.engines[2].lawDispatcher.functors[0].allowBreakage = True



#Cohesion and Tension values will take the minimum of the two touching contact faces
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
	O.engines[2].lawDispatcher.functors[0].allowBreakage = True
	#O.engines[2].lawDispatcher.functors[0].neverErase = True
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
	
			

def setHeatCapacity(): 
	for i in O.interactions:
		if O.bodies[i.id1].shape.isBoundary == True or O.bodies[i.id2].shape.isBoundary == True:
			if O.bodies[i.id1].state.pos[1]<2690.0 and O.bodies[i.id2].state.pos[1]<2690.0:
				i.phys.heatCapacities = 3.0
	for b in O.bodies:
		if b.shape.isBoundary == True:
			if b.state.pos[1] <2690: #2172.0:
				temp = []
				for i in range(len(b.shape.heatCapacity)):
					temp.append(3.0)
				b.shape.shape.heatCapacity = temp
				




prevDistance = O.bodies[westBodyId].state.pos[0]
tolDistance = 0.003 #0.1
tolDistance2 = 0.05
SRinProgress = False
SRcounter = 0
checkIter = 0
prevKE = 0.0
currentKE = 0.0
tolKE =10e10
initBondedContacts = 0
initDispRate = -1.0
prevDispRate = 0

def changeKE(newKE):
	global tolKE
	tolKE = newKE

def changeTolDist(newTol):
	global tolDistance
	tolDistance = newTol


def goToNextStage2():
	global startCountingBrokenBonds
	global velocityDependency
	global waterHeight
	global boundaryFriction
	global rockFriction
	global targetFriction
	global prevDistance
	global originalPositionW
	global tolDistance
	global tolDistance2
	global checkIter
	global SRinProgress
	global SRcounter
	global prevKE
	global currentKE
	global tolKE
	global initDispRate
	global prevDispRate
	prevKE = currentKE
	KE = utils.kineticEnergy()
	currentKE = KE
	#record()	
	dPhi =0.000025
	if startCountingBrokenBonds == False:
		uf = utils.unbalancedForce()
		if O.iter>6000 and uf<0.005:
			print O.iter
			for b in O.bodies:
				b.state.vel = Vector3(0.0,0.0,0.0)
				b.state.angVel = Vector3(0.0,0.0,0.0)
			activateCohesion()
			activateTension()
			dispChecker.iterPeriod=1
			removeDamping()
			return
	else:
		dispChecker.iterPeriod=1
		if abs( (O.bodies[westBodyId].state.pos - originalPositionW).norm()  ) > 20.0:
			vtkRecorder.iterPeriod=1000000
		#if boundaryFriction > dPhi:
		#	boundaryFriction = boundaryFriction-dPhi			
		#reduceFric2(boundaryFriction)
		if waterHeight < 710.0:
			waterHeight = waterHeight + 0.5*0.000125 #0.0005 0.008
		reduceHw(waterHeight)
	if abs( (O.bodies[westBodyId].state.pos - originalPositionW).norm() ) >150.0:
		O.pause()
	prevDispRate = abs( (O.bodies[westBodyId].state.pos - originalPositionW).norm() - prevDistance)
	prevDistance = (O.bodies[westBodyId].state.pos  - originalPositionW).norm()

	


def goToNextStage3():
	global waterHeight
	waterHeight = waterHeight + 10.0
	reduceHw(waterHeight)
		




def reduceFric2(fric):
	global boundaryFriction
	boundaryFriction = fric
	for i in O.interactions:
		if O.bodies[i.id1].shape.isBoundary == True or O.bodies[i.id2].shape.isBoundary == True or i.phys.jointType==2:
			if O.bodies[i.id1].state.pos[1]<2690.0 and O.bodies[i.id2].state.pos[1]<2690.0:
				i.phys.phi_r = fric
				i.phys.phi_b = fric
	for b in O.bodies:
		if b.shape.isBoundary == True:
			if b.state.pos[1] <2690: #2172.0:
				temp = []
				for i in range(len(b.shape.phi_r)):
					temp.append(fric)
				b.shape.phi_r = temp
				b.shape.phi_b = temp
				del temp[:]

'''

		else:
			for i in range(len(b.shape.jointType)):
				if b.shape.jointType[i]==2:
					temp = []
					for i in range(len(b.shape.jointType)):
						if b.shape.jointType[i]==2:
							temp.append(fric)
						else:
							temp.append(b.shape.phi_b[i])
					b.shape.phi_r = temp
					b.shape.phi_b = temp
					del temp[:]

'''

'''
temp = []
			for i in range(len(b.shape.jointType)):
				if b.shape.jointType[i]==2:
					temp.append(fric)
				else:
					temp.append(b.shape.phi_b[i])
			b.shape.phi_r = temp
			b.shape.phi_b = temp
			del temp[:]
'''

def myAddPlotData():
	global westBodyId
	global midBodyId
	global eastBodyId
	global originalPositionW
	global originalPositionM
	global originalPositionE
	global waterHeight
	global boundaryFriction
	KE = utils.kineticEnergy()
	uf = utils.unbalancedForce()
	displacementWx = O.bodies[westBodyId].state.pos[0] - originalPositionW[0]
	displacementW = (O.bodies[westBodyId].state.pos - originalPositionW).norm()
	displacementMx = O.bodies[midBodyId].state.pos[0] - originalPositionM[0]
	displacementM = (O.bodies[midBodyId].state.pos - originalPositionM).norm()
	displacementEx = O.bodies[eastBodyId].state.pos[0] - originalPositionE[0]
	displacementE = (O.bodies[eastBodyId].state.pos - originalPositionE).norm()
	plot.addData(timeStep=O.iter,timeStep1=O.iter,timeStep2=O.iter,timeStep3=O.iter,timeStep4=O.iter,timeStep5=O.iter,kineticEn=KE,unbalancedForce=uf,waterLevel=waterHeight,boundary_phi=boundaryFriction,displacement=displacementW,displacementWest=displacementW,dispWx=displacementWx,displacementMid=displacementM,dispMx=displacementMx,displacementEast=displacementE,dispEx=displacementEx)
	

plot.plots={'waterLevel':('displacement'),'timeStep2':('kineticEn'),'timeStep3':(('displacementWest','ro-'),('dispWx','go-')),'timeStep1':(('displacementMid','ro-'),('dispMx','go-')),'timeStep5':(('displacementEast','ro-'),('dispEx','go-')),'timeStep4':('unbalancedForce')} #PLEASE CHECK



###################### BATCH FUNCTIONS-Giulia ###########################
os. makedirs('saveData/'+name+O.tags['d.id'])
f = open('./saveData/' +name+O.tags['d.id'] + '/Vaiont3Ddata.txt','w')
f.write('timeStep\ttime\tunbalancedForce\tkineticEnergy\twaterLevel\tboundary_phi\tdisplacementW\tdisplacementM\tdisplacementE\tdisplacementWx\tdisplacementMx\tdisplacementEx\tdisplacementWy\tdisplacementMy\tdisplacementEy\tvelocityW\tvelocityM\tvelocityE\tCohesionBrokenNo\tTensionBrokenNo\n')
f.close()

def record():
	global westBodyId
	global eastBodyId
	global midBodyId
	global originalPositionW
	global originalPositionE
	global originalPositionM
	global waterHeight
	global startCountingBrokenBonds
	global boundaryFriction
	global initBondedContacts
	uf=utils.unbalancedForce()
	KE = utils.kineticEnergy()
	cohesionBrokenCount = 0
	tensionBrokenCount = 0
	velW = O.bodies[westBodyId].state.vel.norm()
	velE = O.bodies[eastBodyId].state.vel.norm()
	velM = O.bodies[midBodyId].state.vel.norm()
	displacementWx = O.bodies[westBodyId].state.pos[0] - originalPositionW[0]
	displacementEx = O.bodies[eastBodyId].state.pos[0] - originalPositionE[0]
	displacementMx = O.bodies[midBodyId].state.pos[0] - originalPositionM[0]
	displacementWy = O.bodies[westBodyId].state.pos[1] - originalPositionW[1]
	displacementEy = O.bodies[eastBodyId].state.pos[1] - originalPositionE[1]
	displacementMy = O.bodies[midBodyId].state.pos[1] - originalPositionM[1]
	displacementW = (O.bodies[westBodyId].state.pos - originalPositionW).norm()
	displacementE = (O.bodies[eastBodyId].state.pos - originalPositionE).norm()
	displacementM = (O.bodies[midBodyId].state.pos- originalPositionM).norm()
	if startCountingBrokenBonds==True:
		for i in O.interactions:
			if O.bodies[i.id1].shape.isBoundary == False and O.bodies[i.id2].shape.isBoundary == False and i.isReal==True:
				if i.phys.cohesionBroken == False:
					cohesionBrokenCount = cohesionBrokenCount + 1
				if i.phys.tensionBroken == False:
					tensionBrokenCount = tensionBrokenCount + 1
		cohesionBrokenCount = initBondedContacts - cohesionBrokenCount
		tensionBrokenCount = initBondedContacts - tensionBrokenCount
	f = open('./saveData/'+name +O.tags['d.id'] + '/Vaiont3Ddata.txt','a')
	f.write(str(O.iter)+'\t'
+str(O.time) + '\t'
+str(uf) + '\t'
+str(KE) + '\t'
+str(waterHeight) + '\t'
+str(boundaryFriction) + '\t'
+str(displacementW) + '\t'
+str(displacementM) + '\t'
+str(displacementE) + '\t'
+str(displacementWx) + '\t'
+str(displacementMx) + '\t'
+str(displacementEx) + '\t'
+str(displacementWy) + '\t'
+str(displacementMy) + '\t'
+str(displacementEy) + '\t'
+str(velW) + '\t'
+str(velM) + '\t'
+str(velE) + '\t'
+str(cohesionBrokenCount) + '\t'
+str(tensionBrokenCount) 
+'\n')	
	f.close()



#O.engines = O.engines+ [PyRunner(command='saveToFile()',realPeriod=3600,label='saverEngine')]  ## save yade-xml and python plot every hour
O.engines = O.engines+ [PyRunner(command='record()',iterPeriod=500,label='txtRecorderEngine')] ##  save data into text file every 500 iterations
O.engines = O.engines+ [PyRunner(command='stopfunction()',iterPeriod=20000,label='checker')]  ##  pause when it fails


def stopfunction():
	displacement = O.bodies[westBodyId].state.pos[0] - originalPositionW[0]
	if displacement > 120.0:
		print '\n!!! Slope failed !!!'
		checker.command='pausefunction()'
		saveToFile()

def pausefunction():
	print 'Done, pausing now.'
	print 'Filename ',O.tags['d.id']
	print 'Comments: ',comments
	print yade.timing.stats()
	O.save('saveData/'+name+O.tags['d.id']+'/final.yade.gz')
	plot.saveDataTxt('saveData/'+name+O.tags['d.id']+'/final.data.bz2')
	O.pause()


def saveToFile():
	global boundaryFriction
	global waterHeight
	#if O.realtime<saverEngine.realPeriod:
	#	return 
	#elif O.realtime>saverEngine.realPeriod:
	O.save('saveData/'+name+O.tags['d.id']+'/Vaiont3Dmid'+str(boundaryFriction)+'.yade.gz')  #PLEASE CHECK
	plot.saveDataTxt('saveData/'+name+O.tags['d.id']+'/Vaiont3Dmid.data.bz2')
	return 	

'''
excavate()
O.step()
O.step()
#calTimeStep()

O.engines[2].physDispatcher.functors[0].ks_i = 0.1e9
O.engines[2].physDispatcher.functors[0].kn_i = 2e9
for i in O.interactions:
	i.phys.ks_i =0.1e9
	i.phys.kn_i =2e9


for b in O.bodies:
	if b.state.pos[1] >2690:
		b.dynamic=False

'''
reduceFric2(26.0)
O.run()


