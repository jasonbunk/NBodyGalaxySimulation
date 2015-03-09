import math
import numpy as np
import matplotlib.pyplot as plt
from InitialConditions import InitialConditions


class ToomreDiskGalaxy(InitialConditions):

	def __init__(self):
		InitialConditions.__init__(self)
		self.Mtot = 1e11  #just center
		self.CollisionRmin = 25.0  #kiloparsecs
		self.RingRmax = 0.7*self.CollisionRmin
		self.nRings = 11
		self.firstRingNumPts = 12
		self.NumAdditionalPtsPerRing = 3
		self.radiiStepsOfRings = 0.05*self.CollisionRmin
		
		self.timestep = 0.1
		self.timemax = 10.0
		self.Aarseth_eta = 0.01
		self.Aarseth_eps_sqd = 0.01
		
		#this is for the units kpc, solar masses, hundreds-of-millions-of-years
		self.NEWTONS_GRAVITY_CONSTANT = 4.498309551e-8
	
	
	def GenerateInitialConditions(self,  offset_x, offset_y, offset_z,  offset_vx, offset_vy, offset_vz):
		
		self.npts = 1 + (self.firstRingNumPts * self.nRings) + self.NumAdditionalPtsPerRing * (self.nRings*(self.nRings-1)/2)
		print("This Toomre galaxy has "+str(self.npts)+" points")
		
		self.masses = np.zeros(self.npts)
		self.masses[0] = self.Mtot
		
		self.ptsx = np.zeros(self.npts) + offset_x
		self.ptsy = np.zeros(self.npts) + offset_y
		self.ptsz = np.zeros(self.npts) + offset_z
		self.ptsvx = np.zeros(self.npts) + offset_vx
		self.ptsvy = np.zeros(self.npts) + offset_vy
		self.ptsvz = np.zeros(self.npts) + offset_vz
		
		ringMinRadius = self.RingRmax - (self.nRings-1)*self.radiiStepsOfRings
		
		ip = 1
		for j in range(self.nRings):
			nPtsThisRing = self.firstRingNumPts + j * self.NumAdditionalPtsPerRing
			thisRingRadius = ringMinRadius + j * self.radiiStepsOfRings
			deltaAngle = 6.28318530717958647693 / float(nPtsThisRing)
			
			for n in range(nPtsThisRing):
				
				self.ptsx[ip] = self.ptsx[ip] + thisRingRadius*math.cos(float(n)*deltaAngle)
				self.ptsy[ip] = self.ptsy[ip] + thisRingRadius*math.sin(float(n)*deltaAngle)
				
				circularOrbitVel = math.sqrt(self.masses[0] * self.NEWTONS_GRAVITY_CONSTANT / thisRingRadius)
				
				self.ptsvx[ip] = self.ptsvx[ip] + circularOrbitVel * math.sin(-1.0*float(n)*deltaAngle)
				self.ptsvy[ip] = self.ptsvy[ip] + circularOrbitVel * math.cos(float(n)*deltaAngle)
				
				ip = ip+1
		
	
	def WriteToFile(self, outfilename):
		
		header = str(self.npts)+"  "+str(self.Aarseth_eta)+"  "+str(self.timestep)+"  "+str(self.timemax)+"  "+str(self.Aarseth_eps_sqd)+" \n"
		
		self.WriteInitialConditionsToFile(outfilename, header)



