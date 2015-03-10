import os
import sys
from PlummerGalaxy import PlummerGalaxy


def printargs():
	print("arguments:   {numpts}   {nbody: aarseth|simplecpu|opencl-cpu|opencl-gpu}   {optional:render?}")

if len(sys.argv) <= 2:
	printargs()
	quit()

# ==================== Argument 1: number of points in Plummer sphere

galaxyNumPts = int(sys.argv[1])
if galaxyNumPts <= 0:
	printargs()
	quit()


# ==================== Argument 2: nbody code type

NBODYCOMPILER = ""
NBODYCALCULATOR = ""

if str(sys.argv[2]) == "aarseth":
	print("python script was told to use aarseth code (nbody0-lab.c)")
	NBODYCOMPILER = "(cd NBodySim_Aarseth && make)"
	NBODYCALCULATOR = "./NBodySim_Aarseth/aarseth initialconditions.data"

elif str(sys.argv[2]) == "simplecpu":
	print("python script was told to use simplecpu code (single-threaded C++)")
	NBODYCOMPILER = "(cd NBodySim_SimpleCPU && make)"
	NBODYCALCULATOR = "./NBodySim_SimpleCPU/nbodycpp initialconditions.data"

elif str(sys.argv[2]) == "opencl-cpu":
	print("python script was told to use opencl code (cpu)")
	NBODYCOMPILER = "(cd NBodySim_OpenCL_N2 && make)"
	NBODYCALCULATOR = "./NBodySim_OpenCL_N2/nbodyocl cpu initialconditions.data NBodySim_OpenCL_N2/nbody_kernel_verlet.cl"

elif str(sys.argv[2]) == "opencl-gpu":
	print("python script was told to use opencl code (gpu)")
	NBODYCOMPILER = "(cd NBodySim_OpenCL_N2 && make)"
	NBODYCALCULATOR = "./NBodySim_OpenCL_N2/nbodyocl gpu initialconditions.data NBodySim_OpenCL_N2/nbody_kernel_verlet.cl"

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
newGalaxy.timestep = 0.1
newGalaxy.timemax = 10.0
newGalaxy.ZeroVelocities_Bool = False
newGalaxy.GenerateInitialConditions(0,0,0)
newGalaxy.WriteToFile("initialconditions.data")


# ==================== simulate

if len(NBODYCALCULATOR) > 1:
	print("Running nbody simulation...")
	os.system(NBODYCALCULATOR)


# ==================== render

if doRender3D:
	import glob
	latestDatafile = max(glob.iglob('*.data'), key=os.path.getctime)
	print("will render file: \""+str(latestDatafile)+"\"")
	os.system("./Renderer3D/Renderer3D "+str(latestDatafile)+" "+str(galaxyNumPts)+" 0 1 1")









