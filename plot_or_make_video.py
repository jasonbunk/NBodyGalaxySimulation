import os
import numpy as np
import imp
import struct
try:
  imp.find_module('matplotlib')
  matplotlibAvailable = True
  import matplotlib.pyplot as plt
  from mpl_toolkits.mplot3d import Axes3D
except ImportError:
  matplotlibAvailable = False

def MakeVideo(npts, datafilename, videoOutFilename, boolTrueIfPositionsVideo_FalseIfDistributionsVideo,
		AxisLimitsKPC = 5, UseImageMagickForFancierPositionsVideo = False, ScatterPlotPointSizeGiven=-1):
	
	if matplotlibAvailable == False:
		print("can't make video without matplotlib")
		return
	
	print("beginning to make plots/video...")	
	#=================================================================================
	# Plot the results using matplotlib
	
	fin = open(datafilename, 'rb')
	FLOATSIZE = 4
	FLOATTYPE = 'f'
        nparticles = int(struct.unpack('q',fin.read(8))[0])  # read int64_t type
        secondheadernum = int(struct.unpack('q',fin.read(8))[0])  # read int64_t type
	
	ptsx = np.zeros(npts)
	ptsy = np.zeros(npts)
	ptsz = np.zeros(npts)
	
	if UseImageMagickForFancierPositionsVideo and boolTrueIfPositionsVideo_FalseIfDistributionsVideo:
		# plot axis -- we'll hide it in all future plots because we want to post-process the stars
		fig = plt.figure()
		ax = fig.add_subplot(1,1,1)
		ax.set_xlim(-1*AxisLimitsKPC, AxisLimitsKPC)
		ax.set_ylim(-1*AxisLimitsKPC, AxisLimitsKPC)
		ax.set_aspect(1)
		ax.set_xlabel('x (kpc)')
		ax.set_ylabel('y (kpc)')
		plt.savefig("data/videoframes/_aoutlineframe.png")
		plt.clf()
		plt.close()
		os.system("convert data/videoframes/_aoutlineframe.png -negate data/videoframes/_aoutlineframe.png")
	
	partnum = 0
	timestepint = 0
	keepiterating = True
	while keepiterating: #keep iterating until end of file
		
		try:
			ptsx[partnum] = float(struct.unpack(FLOATTYPE,fin.read(FLOATSIZE))[0])
			ptsy[partnum] = float(struct.unpack(FLOATTYPE,fin.read(FLOATSIZE))[0])
			ptsz[partnum] = float(struct.unpack(FLOATTYPE,fin.read(FLOATSIZE))[0])
		except struct.error:
			print("done reading file")
			keepiterating = False
		
		#####ptsx[partnum], ptsy[partnum], ptsz[partnum] = line.split()   # split line by whitespace
		#print("pt_"+str(partnum)+" == ("+str(ptsx[partnum])+", "+str(ptsy[partnum])+", "+str(ptsz[partnum])+")")
		
		if partnum >= (npts-1):
			if boolTrueIfPositionsVideo_FalseIfDistributionsVideo:
				fig = plt.figure()
				ax = fig.add_subplot(1,1,1, projection='3d')
				
				if ScatterPlotPointSizeGiven > 0:
					ax.scatter(ptsx, ptsy, ptsz, c='b', marker='o', s=ScatterPlotPointSizeGiven)
				else:
					ax.scatter(ptsx, ptsy, ptsz, c='b', marker='o', s=3)
				
				ax.set_xlim(-1*AxisLimitsKPC, AxisLimitsKPC)
				ax.set_ylim(-1*AxisLimitsKPC, AxisLimitsKPC)
				ax.set_zlim(-1*AxisLimitsKPC, AxisLimitsKPC)
				ax.set_aspect(1)
				
				if UseImageMagickForFancierPositionsVideo:
					ax.get_xaxis().set_visible(False)
					ax.get_yaxis().set_visible(False)
					ax.set_frame_on(False)
				else:
					ax.set_title("time: "+str(timestepint))
					ax.set_xlabel('x (kpc)')
					ax.set_ylabel('y (kpc)')
				
				fname = "data/videoframes/_tmp%03d.png"%timestepint
				plt.savefig(fname)
				plt.clf()
				plt.close()
				
				if UseImageMagickForFancierPositionsVideo:
					os.system("convert "+fname+" -negate -gaussian-blur 7x2 -contrast "+fname)
					os.system("composite -compose plus "+fname+" data/videoframes/_aoutlineframe.png "+fname)
			
			else:
				radiiForPlotting = np.sqrt(np.power(ptsx,2.0) + np.power(ptsy,2.0) + np.power(ptsz,2.0))
				radiiForPlotting[radiiForPlotting > 9.9] = 9.9
				truthx = np.linspace(0,10,npts)
				
				fig = plt.figure()
				ax = fig.add_subplot(1,1,1)
				ax.plot(truthx, 3.0*np.power(truthx,2.0)*np.power(1.0+np.power(truthx,2.0),-5.0/2.0))
				ax.hist(radiiForPlotting, 100, normed=1, alpha=0.75, facecolor='green')
				ax.set_xlim(0, 10)
				ax.set_ylim(0, 1)
				ax.set_xlabel('r (kpc)')
				ax.set_ylabel('radial mass density: p(r) * 4 pi r^2   units: 10^11 Msol/kpc')
				ax.set_title("time: "+str(timestepint))
				fname = "data/videoframes/_tmp%03d.png"%timestepint
				plt.savefig(fname)
				plt.clf()
				plt.close()
		
			partnum = -1
			timestepint = (timestepint + 1)
		
		partnum = (partnum + 1)
	
	fin.close()
	
	print("Making movie "+str(videoOutFilename)+"video.avi - this make take a while")
	os.system("ffmpeg -r 30 -f image2 -i 'data/videoframes/_tmp%03d.png' -qscale 0 '"+str(videoOutFilename)+"'")
	######os.system("rm data/videoframes/*.png")


