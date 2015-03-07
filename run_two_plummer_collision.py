import os
import math
import numpy as np
from PlummerGalaxy import PlummerGalaxy
from InitialConditions import InitialConditions
import imp
try:
    imp.find_module('matplotlib')
    matplotlibAvailable = True
    import matplotlib.pyplot as plt
    from mpl_toolkits.mplot3d import Axes3D
except ImportError:
    matplotlibAvailable = False


# Settings:

TotalNumPts = 6000

createNewInitialConditions = True

MakePositionsVideo = True
MakeDistributionsVideo = False

UseImageMagickForFancierVideo = False

#=================================================================================
if createNewInitialConditions:
	
	galaxy1 = PlummerGalaxy()
	galaxy1.npts = (TotalNumPts/2)
	galaxy1.R = 1.0
	galaxy1.ZeroVelocities_Bool = False
	galaxy1.GenerateInitialConditions(-2.5, -2.5, 0)
	
	galaxy2 =PlummerGalaxy()
	galaxy2.npts = (TotalNumPts/2)
	galaxy2.R = 1.0
	galaxy2.ZeroVelocities_Bool = False
	galaxy2.GenerateInitialConditions(2.5, 2.5, 0)
	
	bothGalaxies = InitialConditions()
	bothGalaxies.extend(galaxy1)
	bothGalaxies.extend(galaxy2)
	AarsethHeader = str(TotalNumPts)+"  0.05  0.15  30.0  0.10\n"
	bothGalaxies.WriteInitialConditionsToFile("two_plummers_collision.data", AarsethHeader)
	
	print("compiling Aarseth c code...")
	os.system("(cd Aarseth && make)")
	
	print("Running compiled Aarseth nbody code on Plummer initial conditions file")
	os.system("./Aarseth/aarseth two_plummers_collision.data")


if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
    
	print("beginning to make plots/video...")
	
	if MakePositionsVideo:
		MakeVideo(galaxyNumPts, "out_aarseth_npts_"+str(TotalNumPts)+".data", True)
	if MakeDistributionsVideo:
		MakeVideo(galaxyNumPts, "out_aarseth_npts_"+str(TotalNumPts)+".data", False)




