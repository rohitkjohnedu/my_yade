
'''
# Possible executions of this script
### Parallel:
# mpiexec -n np yade-mpi -n -x testMPIxNxM.py
# mpiexec -n np yade-mpi  -n -x testMPIxN.py L M N# (np-1) subdomains with LxMxN spheres each
'''


NSTEPS=1000 #turn it >0 to see time iterations, else only initilization TODO!HACK
L=10; M=10; N=10; #default size per thread
if len(sys.argv)>1: #we then assume L,M,N are provided as as cmd line arguments
	L=int(sys.argv[1]); N=int(sys.argv[3]); M=int(sys.argv[2])

Nx=Ny=Nz=1 #the number of subD in each direction of space

import mpy as mp
numThreads = 4
np = numThreads-1  #remember to set odd number of cores to make the number of domains even

# stack max. 3 domains vertically, if possible
#if np%3==0: Ny=3; np=np/3   
#elif np%2==0: Ny=2; np=np/2
#else: Ny=1

# now determine the how many domains along x
while np%4==0: 
	Nx*=2; Nz*=2; np/=4
if np%2==0: 
	Nx*=2; np/=2
Nz*=np

# sequential grain colors
import colorsys
colorScale = (Vector3(colorsys.hsv_to_rgb(value*1.0/numThreads, 1, 1)) for value in range(0, numThreads))

#add spheres
subdNo=0
import itertools
for x,y,z in itertools.product(range(int(Nx)),range(int(Ny)),range(int(Nz))):
	subdNo+=1
	col = next(colorScale)
	if mp.rank!=subdNo: continue
	ids=[]
	_id = 0
	for i in range(L):#(numThreads-1) x N x M x L spheres, one thread is for master and will keep only the wall, others handle spheres
		for j in range(M):
			for k in range(N):
				id = O.bodies.insertAtId(sphere((x*L+i+j/30.+k/15.0,y*M+j,z*N+k),0.500,color=col),_id+(N*M*L*(subdNo-1))) #a small shift in x-positions of the rows to break symmetry
				_id+=1
				ids.append(id)
	for id in ids: O.bodies[id].subdomain = subdNo
	
#if rank==0:
WALL_ID=O.bodies.insertAtId(box(center=(Nx*L/2,-0.5,Nz*N/2),extents=(2*Nx*L,0,2*Nz*N),fixed=True),(N*M*L*(numThreads-1)))
print("WALL_ID=",WALL_ID)
if mp.rank!=0:
	O.bodies.erase(WALL_ID)

collider.verletDist = 0.25
newton.gravity=(0,-10,0) #else nothing would move
tsIdx=O.engines.index(timeStepper) #remove the automatic timestepper. Very important: we don't want subdomains to use many different timesteps...
O.engines=O.engines[0:tsIdx]+O.engines[tsIdx+1:]
O.dt=0.001 #this very small timestep will make it possible to run 2000 iter without merging
#O.dt=0.1*PWaveTimeStep() #very important, we don't want subdomains to use many different timesteps...


#import yade's mpi module
#from yade import mpy as mp
# customize
mp.MAX_RANK_OUTPUT=4
mp.VERBOSE_OUTPUT=True
mp.YADE_TIMING=True
mp.DISTRIBUTED_INSERT=True
mp.mpirun(1,numThreads,True) #this is to eliminate initialization overhead in Cundall number and timings
#from yade import timing
#timing.reset()
#t1=time.time()
#mp.mpirun(NSTEPS)
#t2=time.time()
#mp.mprint("num. bodies:",len([b for b in O.bodies])," ",len(O.bodies))
#if rank==0:
	#mp.mprint( "Total force on floor="+str(O.forces.f(WALL_ID)[1]))
	#mp.mprint("CPU wall time for ",NSTEPS," iterations:",t2-t1,"; Cundall number = ",N*M*(numThreads-1)*NSTEPS/(t2-t1))
	#collectTiming()
#else: mp.mprint( "Partial force on floor="+str(O.forces.f(WALL_ID)[1]))
#mp.mergeScene()
#if rank==0: O.save('mergedScene.yade')
mp.MPI.Finalize()
exit()
