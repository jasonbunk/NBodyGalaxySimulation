import os
import math
import numpy as np
from PlummerGalaxy import PlummerGalaxy
from plot_or_make_video import MakeVideo
import imp
try:
  imp.find_module('matplotlib')
  matplotlibAvailable = True
  import matplotlib.pyplot as plt
  from mpl_toolkits.mplot3d import Axes3D
except ImportError:
  matplotlibAvailable = False



# Settings:

createNewInitialConditions = True

MakePositionsVideo = True
MakeDistributionsVideo = True

galaxyNumPts = 2048

#=================================================================================
if createNewInitialConditions:
	
	print("compiling OpenCL C++ code...")
	os.system("(cd OpenCL_N2 && make)")
	
	print("generating Plummer galaxy...")
	newGalaxy = PlummerGalaxy()
	newGalaxy.npts = galaxyNumPts
	newGalaxy.R = 1.0
	newGalaxy.timestep = 0.1
	newGalaxy.timemax = 12.0
	newGalaxy.ZeroVelocities_Bool = False
	
	newGalaxy.GenerateInitialConditions(0,0,0)
	newGalaxy.WriteToFile("plummer.data")
	
	print("Running compiled OpenCL C++ nbody code (on GPU) on Plummer initial conditions file")
	os.system("./OpenCL_N2/nbodyocl gpu plummer.data OpenCL_N2/nbody_n2_kernel.cl")
	

if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
		
    print("beginning to make plots/video...")
		
    if MakePositionsVideo:
        MakeVideo(galaxyNumPts, "out_opencl.data", True)
    if MakeDistributionsVideo:
        MakeVideo(galaxyNumPts, "out_opencl.data", False)




