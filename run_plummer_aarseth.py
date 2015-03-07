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
	newGalaxy.Aarseth_eta = 0.05
	newGalaxy.Aarseth_eps_sqd = 0.05
	newGalaxy.ZeroVelocities_Bool = True
	
	newGalaxy.GenerateInitialConditions(0,0,0)
	newGalaxy.WriteToFile("plummer.data")
	
	print("compiling Aarseth c code...")
	os.system("(cd Aarseth && make)")
    
	print("Running compiled Aarseth nbody code on Plummer initial conditions file")
	os.system("./Aarseth/aarseth plummer.data")


if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
	
	print("beginning to make plots/video...")	
	
	if MakePositionsVideo:
		MakeVideo(galaxyNumPts, "out_aarseth_npts_"+str(galaxyNumPts)+".data", True)
	if MakeDistributionsVideo:
		MakeVideo(galaxyNumPts, "out_aarseth_npts_"+str(galaxyNumPts)+".data", False)


