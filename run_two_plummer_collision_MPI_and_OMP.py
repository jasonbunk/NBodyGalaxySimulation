import os, sys, time
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

if len(sys.argv) <= 1:
	print("usage:  {prefix}  {total num pts}  {numMPIprocs}  {numCacheGrps}  {Nburst}")
if len(sys.argv) >= 6:
	preffix=sys.argv[1]
	TotalNumPts = int(sys.argv[2])
	numMPIprocs = sys.argv[3]
	numMPImemgrps = sys.argv[4]
	nburstburst = sys.argv[5]
	print("using user-supplied arguments")
else:
	preffix="default"
	TotalNumPts = 600
	numMPIprocs = str( 6 )
	numMPImemgrps = str( 2 ) #will be forced to be 2
	nburstburst = str( 3 )
	quit()

GravitationalConst = 1.0

# Settings:

InitialConditionsFolder = "data/initialconditions/"
OutputResultsFolder = "data/results/"

createNewInitialConditions = True

MakePositionsVideo = False
UseImageMagickForFancierVideo = False

#=================================================================================

starttime = time.time()

try:
    os.system('rm NBodySim_CPU_MPI/nbodycpumpi')
    #os.system('rm data/initialconditions/*')
except:
    print("executable already deleted")


print("compiling C++ code...")
os.system("(cd NBodySim_CPU_MPI && make)")

if createNewInitialConditions:
    assert(numMPImemgrps == '2' or numMPImemgrps == '1')   
    
    timeStep = 0.15
    timeMax = 4.0
    epssqd = 0.1
    AarsethHeader = str(TotalNumPts)+" 0.01 "+str(timeStep)+" "+str(timeMax)+" "+str(epssqd)+" "+str(GravitationalConst)+"\n"
    
    galaxy1 = PlummerGalaxy()
    galaxy1.npts = (TotalNumPts/2)
    galaxy1.R = 1.0
    galaxy1.ZeroVelocities_Bool = False
    galaxy1.GenerateInitialConditions(-4, -4, 0)

    galaxy2 = PlummerGalaxy()
    galaxy2.npts = (TotalNumPts/2)
    galaxy2.R = 1.0
    galaxy2.ZeroVelocities_Bool = False
    galaxy2.GenerateInitialConditions(4, 4, 0)

    if numMPImemgrps == '2':
        galaxy1.WriteInitialConditionsToFile(InitialConditionsFolder+preffix+"0.data", AarsethHeader)
        galaxy2.WriteInitialConditionsToFile(InitialConditionsFolder+preffix+"1.data", AarsethHeader)
    else:
        bothGalaxies = InitialConditions()
        bothGalaxies.extend(galaxy1)
        bothGalaxies.extend(galaxy2)
        AarsethHeader = str(TotalNumPts)+" 0.01 "+str(timeStep)+" "+str(timeMax)+" "+str(epssqd)+" "+str(GravitationalConst)+"\n"
        bothGalaxies.WriteInitialConditionsToFile(InitialConditionsFolder+preffix+"0.data", AarsethHeader)
	
        
#args: {NumDataSplits}  {nburst-between-disksaves}  {InitialConditionsFilename-Prefix}  {OutputFilename-Prefix}

print("Running compiled MPI/OpenMP C++ nbody code on two-Plummer-collision initial conditions files")
os.system('mpirun -np '+numMPIprocs+' ./NBodySim_CPU_MPI/nbodycpumpi '+numMPImemgrps+' '+nburstburst+' '+InitialConditionsFolder+preffix+' '+OutputResultsFolder+preffix)


endtime = time.time()

print("that took "+ (starttime - endtime)+" seconds")

'''
if True:
    print("launching renderer...")
    os.system("Renderer3D/Renderer3D "+OutputResultsFolder+"out_mpi_twocollisions_.data "+str(TotalNumPts/2)+" 0 1 1")


if matplotlibAvailable and (MakePositionsVideo or MakeDistributionsVideo):

    print("beginning to make plots/video...")

    if MakePositionsVideo:
        MakeVideo(TotalNumPts, OutputResultsFolder+"out_MPI.data", "video_two_plummer_MPI_collision.avi", True, 8, UseImageMagickForFancierVideo)

'''



