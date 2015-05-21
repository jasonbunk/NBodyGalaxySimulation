import os, sys, time
from PlummerGalaxy import PlummerGalaxy


def printargs():
	print("arguments:   {numpts}   {nbody: aarseth|simplecpu|opencl-cpu|opencl-gpu}   {optional:render?}   {optional:opencl-particles-per-thread}")

if len(sys.argv) <= 2:
	printargs()
	quit()

# ==================== Argument 1: number of points in Plummer sphere

galaxyNumPts = int(sys.argv[1])
if galaxyNumPts <= 0:
	printargs()
	quit()

particlesPerOpenCLThread = 0
if len(sys.argv) > 4:
	particlesPerOpenCLThread = str(sys.argv[4])

InitialConditionsFolder = "data/initialconditions/"
OutputResultsFolder = "data/results/"

# ==================== Argument 2: nbody code type

OUTFILENAME = ""
NBODYCOMPILER = ""
NBODYCALCULATOR = ""

if str(sys.argv[2]) == "aarseth":
	print("python script was told to use aarseth code (nbody0-lab.c)")
	OUTFILENAME = OutputResultsFolder+"out_aarseth_plumbench.data"
	NBODYCOMPILER = "(cd NBodySim_Aarseth && make clean && make)"
	NBODYCALCULATOR = "./NBodySim_Aarseth/aarseth "+InitialConditionsFolder+"initialconditions.data "+OUTFILENAME

elif str(sys.argv[2]) == "simplecpu":
	print("python script was told to use simplecpu code (single-threaded C++)")
	OUTFILENAME = OutputResultsFolder+"out_simplecpu_plumbench.data"
	NBODYCOMPILER = "(cd NBodySim_SimpleCPU && make clean && make)"
	NBODYCALCULATOR = "./NBodySim_SimpleCPU/nbodycpp "+InitialConditionsFolder+"initialconditions.data "+OUTFILENAME

elif str(sys.argv[2]) == "opencl-cpu":
	print("python script was told to use opencl code (cpu)")
	OUTFILENAME = OutputResultsFolder+"out_opencl_cpu_plumbench.data"
	NBODYCOMPILER = "(cd NBodySim_OpenCL_N2 && make clean && make)"
	NBODYCALCULATOR = "./NBodySim_OpenCL_N2/nbodyocl cpu "+InitialConditionsFolder+"initialconditions.data "+OUTFILENAME+" "+particlesPerOpenCLThread+" NBodySim_OpenCL_N2/nbody_kernel_verlet.cl"

elif str(sys.argv[2]) == "opencl-gpu":
	print("python script was told to use opencl code (gpu)")
	OUTFILENAME = OutputResultsFolder+"out_opencl_gpu_plumbench.data"
	NBODYCOMPILER = "(cd NBodySim_OpenCL_N2 && make clean && make)"
	NBODYCALCULATOR = "./NBodySim_OpenCL_N2/nbodyocl gpu "+InitialConditionsFolder+"initialconditions.data "+OUTFILENAME+" "+particlesPerOpenCLThread+" NBodySim_OpenCL_N2/nbody_kernel_verlet.cl"

else:
	print("unknown nbody code type, see arguments")
	printargs()
	print("will still create initial conditions for later use")


# ==================== Argument 3 - render? (optional, default is False)

doRender3D = False
if len(sys.argv) > 3:
	if str(sys.argv[3]) == "True" or str(sys.argv[3]) == "true" or str(sys.argv[3]) == "1":
		doRender3D = True


# ==================== compile (run Makefile)

if len(NBODYCOMPILER) > 1:
	print("compiling nbody simulator...")
	os.system(NBODYCOMPILER)


# ==================== create initial conditions

print("Generating Plummer galaxy initial conditions...")
newGalaxy = PlummerGalaxy()
newGalaxy.npts = galaxyNumPts
newGalaxy.R = 1.0
newGalaxy.timestep = 0.07
newGalaxy.timemax = 50.0
newGalaxy.ZeroVelocities_Bool = False
newGalaxy.GenerateInitialConditions(0,0,0)
newGalaxy.WriteToFile(InitialConditionsFolder+"initialconditions.data")


# ==================== simulate

if len(NBODYCALCULATOR) > 1:
	print("Running nbody simulation...")
	starttime = time.time() #time.clock()
	os.system(NBODYCALCULATOR)
	endtime = time.time() #time.clock()
	print("TIME TO RUN BENCHMARK: "+str(endtime-starttime)+" seconds")


# ==================== render

if doRender3D:
	if False:
		import glob
		latestDatafile = max(glob.iglob(OutputResultsFolder+"*.data"), key=os.path.getctime)
	else:
		latestDatafile = OUTFILENAME
	print("will render file: \""+str(latestDatafile)+"\"")
	os.system("./Renderer3D/Renderer3D "+str(latestDatafile)+" "+str(galaxyNumPts)+" 0 1 1")









