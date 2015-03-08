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

MakePositionsVideo = False
MakeDistributionsVideo = False

galaxyNumPts = 2048

#=================================================================================
if createNewInitialConditions:
	
	print("compiling SimpleCPU NBody C++ code...")
	os.system("(cd NBodySim_SimpleCPU && make)")
	
	print("generating Plummer galaxy...")
	newGalaxy = PlummerGalaxy()
	newGalaxy.npts = galaxyNumPts
	newGalaxy.R = 1.0
	newGalaxy.timestep = 0.1
	newGalaxy.timemax = 12.0
	newGalaxy.ZeroVelocities_Bool = False
	
	newGalaxy.GenerateInitialConditions(0,0,0)
	newGalaxy.WriteToFile("plummer.data")
	
	print("Running compiled SimpleCPU NBody C++ code on Plummer initial conditions file")
	os.system("./NBodySim_SimpleCPU/nbodycpp plummer.data")
	

if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):
		
    print("beginning to make plots/video...")
		
    if MakePositionsVideo:
        MakeVideo(galaxyNumPts, "out_simplecpu.data", "video_cppnbody_positions.avi", True)
    if MakeDistributionsVideo:
        MakeVideo(galaxyNumPts, "out_simplecpu.data", "video_cppnbody_pos_distribution.avi", False)




