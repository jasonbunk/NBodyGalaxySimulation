import os
import math
import numpy as np
from PlummerGalaxy import PlummerGalaxy
from InitialConditions import InitialConditions
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

InitialConditionsFolder = "data/initialconditions/"
OutputResultsFolder = "data/results/"

GravitationalConst = 1.0

TotalNumPts = 4096
particlesPerOpenCLThread = str( 2 )

createNewInitialConditions = True

MakePositionsVideo = True
UseImageMagickForFancierVideo = True

#=================================================================================
if createNewInitialConditions:
	
	print("compiling OpenCL C++ code...")
	os.system("(cd NBodySim_OpenCL_N2 && make)")
	
	galaxy1 = PlummerGalaxy()
	galaxy1.npts = (TotalNumPts/2)
	galaxy1.R = 1.0
	galaxy1.ZeroVelocities_Bool = False
	galaxy1.GenerateInitialConditions(-4, -4, 0)
	
	galaxy2 =PlummerGalaxy()
	galaxy2.npts = (TotalNumPts/2)
	galaxy2.R = 1.0
	galaxy2.ZeroVelocities_Bool = False
	galaxy2.GenerateInitialConditions(4, 4, 0)
	
	timeStep = 0.15
	timeMax = 30.0
	epssqd = 0.1
	
	bothGalaxies = InitialConditions()
	bothGalaxies.extend(galaxy1)
	bothGalaxies.extend(galaxy2)
	AarsethHeader = str(TotalNumPts)+" 0.01 "+str(timeStep)+" "+str(timeMax)+" "+str(epssqd)+" "+str(GravitationalConst)+"\n"
	bothGalaxies.WriteInitialConditionsToFile(InitialConditionsFolder+"two_plummers_collision.data", AarsethHeader)
	
	print("Running compiled OpenCL C++ nbody code (on GPU) on Plummer initial conditions file")
	os.system("./NBodySim_OpenCL_N2/nbodyocl gpu "+InitialConditionsFolder+"two_plummers_collision.data "+OutputResultsFolder+"out_opencl.data "+particlesPerOpenCLThread+" NBodySim_OpenCL_N2/nbody_kernel_verlet.cl")
	


if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
    
	print("beginning to make plots/video...")
	
	if MakePositionsVideo:
		MakeVideo(TotalNumPts, OutputResultsFolder+"out_opencl.data", "video_two_plummer_collision.avi", True, 8, UseImageMagickForFancierVideo)





