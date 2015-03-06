import os
import math
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from PlummerGalaxy import PlummerGalaxy
from plot_or_make_video import MakeVideo

# Settings:

createNewInitialConditions = True

MakePositionsVideo = True
MakeDistributionsVideo = True

galaxyNumPts = 2048

#=================================================================================
if createNewInitialConditions:
	
	# Generate Plummer galaxy
	newGalaxy = PlummerGalaxy()
	newGalaxy.npts = galaxyNumPts
	newGalaxy.R = 1.0
	newGalaxy.timestep = 0.1
	newGalaxy.timemax = 12.0
	newGalaxy.ZeroVelocities_Bool = False
	
	newGalaxy.GenerateInitialConditions(0,0,0)
	newGalaxy.WriteToFile("plummer.data")
	
	print("compiling OpenCL C++ code...")
	os.system("./OpenCL_N2/buildFromParentFolder.sh")
	
	print("Running compiled OpenCL C++ nbody code (on GPU) on Plummer initial conditions file")
	os.system("./OpenCL_N2/nbodyocl gpu plummer.data OpenCL_N2/nbody_n2_kernel.cl")
	


if MakePositionsVideo or MakeDistributionsVideo:
	
	print("beginning to make plots/video...")	
	
	if MakePositionsVideo:
		MakeVideo(galaxyNumPts, "out_opencl.data", True)
	if MakeDistributionsVideo:
		MakeVideo(galaxyNumPts, "out_opencl.data", False)




