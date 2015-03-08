import numpy as np
import matplotlib.pyplot as plt
from InitialConditions import InitialConditions


class ToomreDiskGalaxy(InitialConditions):

	def __init__(self):
		InitialConditions.__init__(self)
		self.npts = 1
		self.R = 1.0
		self.Mtot = 1.0 #currently, must be 1
		self.timestep = 0.1
		self.timemax = 10.0
		self.Aarseth_eta = 0.01
		self.Aarseth_eps_sqd = 0.01
	
	
	def GenerateInitialConditions(self,  offset_x, offset_y, offset_z,  offset_vx, offset_vy, offset_vz):
		
		self.masses = np.ones(self.npts) * (float(self.Mtot) / float(self.npts))
		
		self.ptsx = np.zeros(self.npts) + offset_x
		self.ptsy = np.zeros(self.npts) + offset_y
		self.ptsz = np.zeros(self.npts) + offset_z
		
		self.ptsvx = np.zeros(self.npts) + offset_vx
		self.ptsvy = np.zeros(self.npts) + offset_vy
		self.ptsvz = np.zeros(self.npts) + offset_vz
	
	
	def WriteToFile(self, outfilename):
		
		header = str(self.npts)+"  "+str(self.Aarseth_eta)+"  "+str(self.timestep)+"  "+str(self.timemax)+"  "+str(self.Aarseth_eps_sqd)+" \n"
		
		self.WriteInitialConditionsToFile(outfilename, header)



