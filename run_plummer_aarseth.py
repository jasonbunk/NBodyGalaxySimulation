import os
import math
import numpy as np
from PlummerGalaxy import PlummerGalaxy
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
MakeDistributionsVideo = False

galaxyNumPts = 1000


#=================================================================================
if createNewInitialConditions:
	
	# Generate Plummer galaxy
	newGalaxy = PlummerGalaxy()
	newGalaxy.npts = galaxyNumPts
	newGalaxy.R = 1.0
	newGalaxy.timestep = 0.1
	newGalaxy.timemax = 2.0
	newGalaxy.ZeroVelocities_Bool = True
	
	newGalaxy.GenerateInitialConditions(0,0,0)
	newGalaxy.WriteToFile("plummer.data")
	
	print("compiling Aarseth c code...")
	os.system("(cd Aarseth && make nbody0-lab)")
    
	print("Running compiled Aarseth nbody code on Plummer initial conditions file")
	os.system("./Aarseth/nbody0-lab plummer.data")


if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
	
	print("beginning to make plots/video...")	
	
	if MakePositionsVideo:
		MakeVideo(galaxyNumPts, "out_aarseth_npts_"+str()+".data", True)
	if MakeDistributionsVideo:
		MakeVideo(galaxyNumPts, "out_aarseth_npts_"+str()+".data", False)


