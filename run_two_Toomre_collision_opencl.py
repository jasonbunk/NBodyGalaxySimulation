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

DEGREESTORAD = 0.01745329251994329577

# Settings:

GravitationalConst = 4.498309551e-8

TotalNumPts = (298*2) #596

createNewInitialConditions = True

MakePositionsVideo = False
MakeDistributionsVideo = False

UseImageMagickForFancierVideo = False

initialVelocitiesFor25kpcSeparationFrom100kpc = 1.58
initialVelocitiesFor50kpcSeparationFrom100kpc = 2.12
initialVelocitiesFor25kpcSeparationFrom50kpc = 3.00

initialVelocities = initialVelocitiesFor25kpcSeparationFrom50kpc

#=================================================================================
if createNewInitialConditions:
	
	print("compiling OpenCL C++ code...")
	os.system("(cd NBodySim_OpenCL_N2 && make)")
	
	galaxy1 = ToomreDiskGalaxy()
	galaxy1.nRings = 11
	galaxy1.NEWTONS_GRAVITY_CONSTANT = GravitationalConst
	galaxy1.GenerateInitialConditions(0,0,0, 0,0,0)
	galaxy1.applyRotation(15.0*DEGREESTORAD, 0, 0)
	galaxy1.applyOffset(50,0,0, 0,initialVelocities,0)
	
	galaxy2 = ToomreDiskGalaxy()
	galaxy2.nRings = 11
	galaxy2.NEWTONS_GRAVITY_CONSTANT = GravitationalConst
	galaxy2.GenerateInitialConditions(0,0,0,  0,0,0)
	galaxy2.applyRotation(60.0*DEGREESTORAD, 0, 0)
	galaxy2.applyOffset(-50,0,0, 0,-1.0*initialVelocities,0)
	
	TotalNumPts = (galaxy1.npts + galaxy2.npts)
	
	timeStep = 0.07
	timeMax = 21.0
	epssqd = 0.01
	
	bothGalaxies = InitialConditions()
	bothGalaxies.extend(galaxy1)
	bothGalaxies.extend(galaxy2)
	AarsethHeader = str(TotalNumPts)+" 0.0 "+str(timeStep)+" "+str(timeMax)+" "+str(epssqd)+"\n"
	bothGalaxies.WriteInitialConditionsToFile("two_toomres_collision.data", AarsethHeader)
	
	print("Running compiled OpenCL C++ nbody code (on CPU) on initial conditions file")
	os.system("./NBodySim_OpenCL_N2/nbodyocl cpu two_toomres_collision.data NBodySim_OpenCL_N2/nbody_kernel_verlet.cl "+str(GravitationalConst))
	
	print("launching renderer...")
	os.system("Renderer3D/Renderer3D out_opencl.data "+str(TotalNumPts)+" 0 1 1 Renderer3D/cposfile.txt 0")


if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
    
	print("beginning to make plots/video...")
	
	if MakePositionsVideo:
		MakeVideo(TotalNumPts, "out_opencl.data", "video_twotoomre_collision.avi", True, 75, False, 200)
	if MakeDistributionsVideo:
		MakeVideo(TotalNumPts, "out_opencl.data", "video_twotoomre_collision_distributions.avi", False, 75)




