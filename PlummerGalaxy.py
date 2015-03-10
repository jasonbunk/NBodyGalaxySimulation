import numpy as np
from InitialConditions import InitialConditions
import imp
try:
  imp.find_module('matplotlib')
  matplotlibAvailable = True
  import matplotlib.pyplot as plt
  from mpl_toolkits.mplot3d import Axes3D
except ImportError:
  matplotlibAvailable = False


class PlummerGalaxy(InitialConditions):
	
	def __init__(self):
		InitialConditions.__init__(self)
		self.npts = 1000
		self.R = 1.0
		self.Mtot = 1.0 #currently, must be 1
		self.ZeroVelocities_Bool = False
		self.timestep = 0.1
		self.timemax = 10.0
		self.Aarseth_eta = 0.04
		self.Aarseth_eps_sqd = 0.01
		self.NEWTONS_GRAVITY_CONSTANT = 1.0
	
	
	def GenerateInitialConditions(self, offset_x, offset_y, offset_z):
		
		self.masses = np.ones(self.npts) * (float(self.Mtot) / float(self.npts))
		
		#=================================================================================
		# Step one: generate random radii
		#
		# the complicated-looking equation for generating radii comes from solving
		# the cumulative density function P(r) == r and solving for r in terms of P
		# P(r) = Integrate[p(r')*4*pi*(r')^2, {r',0,r}]
		# then when you have r(P) you can generate random numbers X from 0 to 1 and get the radii r(X)
		# I used Mathematica to generate this; substitution variable: rhat == (r^2/R^2)
		
		randnums = np.random.uniform(0,1,self.npts)
		
		P = np.power(randnums,2.0)
		
		R = self.R
		bigtermR24to13 = -1.0*np.power(np.absolute((-1.0*R**6)*np.power(P,2.0) - ((R**12.0)*P) + np.sqrt(np.power(P,4.0)*(R**12.0) - (2.0*(R**18))*np.power(P,3.0) + (R**24)*np.power(P,2.0))), 1.0/3.0)
		rhat = np.divide(P, R**6.0-P) + 1.25992104989487316477*(R**6.0)*np.divide(P, np.multiply(P-R**6, bigtermR24to13)) + np.divide(bigtermR24to13, 1.25992104989487316477*(P-R**6))
		radii = R*np.sqrt(rhat)
		
	
		# this was given by the paper in lecture 5
		#radii = np.power(np.power(randnums,-2/3)-1, -1/2)
		
		#=================================================================================
		if matplotlibAvailable and False:
			# Plot radii to ensure we match the desired distribution
			
			radiiForPlotting = radii.copy()
			radiiForPlotting[radiiForPlotting > 9.9] = 9.9
			
			truthx = np.linspace(0,10,self.npts)
			plt.plot(truthx, 3.0*np.power(truthx,2.0)*np.power(1.0+np.power(truthx,2.0),-5.0/2.0))
			plt.hist(radiiForPlotting, 100, normed=1, alpha=0.75, facecolor='green')
			plt.grid(True)
			plt.show()
		
		#=================================================================================
		# Get cartesian 3D coordinates by placing the points onto spheres at their radii
		
		RandomThetas = np.random.uniform(0,6.28318530717958647693,self.npts) #angle around the XY plane
		RandomPhis = np.arccos(np.random.uniform(-1,1,self.npts)) #angle from z axis
		
		self.ptsx = np.multiply(radii, np.multiply(np.cos(RandomThetas), np.sin(RandomPhis))) + float(offset_x)
		self.ptsy = np.multiply(radii, np.multiply(np.sin(RandomThetas), np.sin(RandomPhis))) + float(offset_y)
		self.ptsz = np.multiply(radii, np.cos(RandomPhis)) + float(offset_z)
		
		#=================================================================================
		# Get cartesian velocities
		
		RandomQs = np.random.uniform(0, 1, self.npts)
		RandomGs = np.random.uniform(0, 0.093, self.npts)

		for i in range(self.npts):
			while RandomGs[i] > (RandomQs[i]**2.0 * ((1.0 - RandomQs[i]**2.0)**(7.0/2.0))):
				RandomQs[i] = np.random.uniform(0,1)
				RandomGs[i] = np.random.uniform(0,0.093)

		velsmagnitude = np.multiply(RandomQs, 1.4142135623730950488*np.power(1 + np.power(radii,2.0), -0.25))
		
		RandomThetas = np.random.uniform(0,6.28318530717958647693,self.npts) #angle around the XY plane
		RandomPhis = np.arccos(np.random.uniform(-1,1,self.npts)) #angle from z axis
		
		self.ptsvx = np.multiply(velsmagnitude, np.multiply(np.cos(RandomThetas), np.sin(RandomPhis)))
		self.ptsvy = np.multiply(velsmagnitude, np.multiply(np.sin(RandomThetas), np.sin(RandomPhis)))
		self.ptsvz = np.multiply(velsmagnitude, np.cos(RandomPhis))
		
		if self.ZeroVelocities_Bool:
			self.ptsvx = (self.ptsvx * 0.0)
			self.ptsvy = (self.ptsvy * 0.0)
			self.ptsvz = (self.ptsvz * 0.0)
		
		#print("avg velocity == "+str(np.mean(velsmagnitude)))
		#exit()
	
	
	def WriteToFile(self, outfilename):
		
		header = str(self.npts)+" "+str(self.Aarseth_eta)+" "+str(self.timestep)+" "+str(self.timemax)+" "+str(self.Aarseth_eps_sqd)+" "+str(self.NEWTONS_GRAVITY_CONSTANT)+"\n"
		self.WriteInitialConditionsToFile(outfilename, header)


