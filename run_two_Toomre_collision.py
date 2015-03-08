import os
import math
import numpy as np
from InitialConditions import InitialConditions
from ToomreDiskGalaxy import ToomreDiskGalaxy
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

TotalNumPts = 2

createNewInitialConditions = True

MakePositionsVideo = True
MakeDistributionsVideo = False

UseImageMagickForFancierVideo = False

#=================================================================================
if createNewInitialConditions:
	
	galaxy1 = ToomreDiskGalaxy()
	galaxy1.GenerateInitialConditions(62.5, 0, 0,  0, 20, 0)
	
	galaxy2 = ToomreDiskGalaxy()
	galaxy2.GenerateInitialConditions(-62.5, 0, 0,  0, -20, 0)
	
	bothGalaxies = InitialConditions()
	bothGalaxies.extend(galaxy1)
	bothGalaxies.extend(galaxy2)
	AarsethHeader = str(TotalNumPts)+"  0.01  0.1  30.0  0.01\n"
	bothGalaxies.WriteInitialConditionsToFile("two_toomres_collision.data", AarsethHeader)
	
	print("compiling Aarseth c code...")
	os.system("(cd NBodySim_Aarseth && make)")
	
	print("Running compiled Aarseth nbody code on Plummer initial conditions file")
	os.system("./NBodySim_Aarseth/aarseth two_toomres_collision.data")


if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
    
	print("beginning to make plots/video...")
	
	if MakePositionsVideo:
		MakeVideo(TotalNumPts, "out_aarseth_npts_"+str(TotalNumPts)+".data", "video_twotoomre_collision.avi", True, 75, False, 200)
	if MakeDistributionsVideo:
		MakeVideo(TotalNumPts, "out_aarseth_npts_"+str(TotalNumPts)+".data", "video_twotoomre_collision_distributions.avi", False, 75)




