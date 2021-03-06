import math
import numpy as np
from math import cos
from math import sin
from struct import pack as packbinary

class InitialConditions:
	
	def __init__(self):
		self.masses = []
		
		self.ptsx = []
		self.ptsy = []
		self.ptsz = []
		
		self.ptsvx = []
		self.ptsvy = []
		self.ptsvz = []
	
	
	def applyOffset(self,  offset_x, offset_y, offset_z,  offset_vx, offset_vy, offset_vz):
		for i in range(len(self.masses)):
			self.ptsx[i] = self.ptsx[i] + offset_x
			self.ptsy[i] = self.ptsy[i] + offset_y
			self.ptsz[i] = self.ptsz[i] + offset_z
			self.ptsvx[i] = self.ptsvx[i] + offset_vx
			self.ptsvy[i] = self.ptsvy[i] + offset_vy
			self.ptsvz[i] = self.ptsvz[i] + offset_vz
	
	
	def applyRotation(self, thetaXaxis, thetaYaxis, thetaZaxis):
		
		rotMatX = np.matrix([[1,0,0], [0,cos(thetaXaxis),-1.0*sin(thetaXaxis)], [0,sin(thetaXaxis),cos(thetaXaxis)]])
		rotMatY = np.matrix([[cos(thetaYaxis),0,sin(thetaYaxis)], [0,1,0], [-1.0*sin(thetaYaxis),0,cos(thetaYaxis)]])
		rotMatZ = np.matrix([[cos(thetaZaxis),-1.0*sin(thetaZaxis),0], [sin(thetaZaxis),cos(thetaZaxis),0], [0,0,1]])
		
		for i in range(len(self.masses)):
			posvec = np.zeros((3,1))
			posvec[0] = self.ptsx[i]
			posvec[1] = self.ptsy[i]
			posvec[2] = self.ptsz[i]
			posvec = (rotMatZ * rotMatY * rotMatX) * posvec
			self.ptsx[i] = posvec[0]
			self.ptsy[i] = posvec[1]
			self.ptsz[i] = posvec[2]
			
			velvec = np.zeros((3,1))
			velvec[0] = self.ptsvx[i]
			velvec[1] = self.ptsvy[i]
			velvec[2] = self.ptsvz[i]
			velvec = (rotMatZ * rotMatY * rotMatX) * velvec
			self.ptsvx[i] = velvec[0]
			self.ptsvy[i] = velvec[1]
			self.ptsvz[i] = velvec[2]
	
	
	def applyRotationAboutAxis(self, th, axX, axY, axZ):
		
		a = 1 - cos(th)
		s = sin(th)
		c = cos(th)
		
		rotMat = np.matrix([[a*axX*axX+c, a*axX*axY-s*axZ, a*axX*axZ+s*axY], [a*axX*axY+s*axX, a*axY*axY+c, a*axY*axX-s*axX], [a*axX*axZ-s*axY, a*axY*axZ+s*axX, a*axZ*axZ+c]])					
		
		for i in range(len(self.masses)):
			posvec = np.zeros((3,1))
			posvec[0] = self.ptsx[i]
			posvec[1] = self.ptsy[i]
			posvec[2] = self.ptsz[i]
			posvec = rotMat * posvec
			self.ptsx[i] = posvec[0]
			self.ptsy[i] = posvec[1]
			self.ptsz[i] = posvec[2]
			
			velvec = np.zeros((3,1))
			velvec[0] = self.ptsvx[i]
			velvec[1] = self.ptsvy[i]
			velvec[2] = self.ptsvz[i]
			velvec = rotMat * velvec
			self.ptsvx[i] = velvec[0]
			self.ptsvy[i] = velvec[1]
			self.ptsvz[i] = velvec[2]
	
	
	def extend(self, other_initial_conditions):
		self.masses.extend(other_initial_conditions.masses)
		
		self.ptsx.extend(other_initial_conditions.ptsx)
		self.ptsy.extend(other_initial_conditions.ptsy)
		self.ptsz.extend(other_initial_conditions.ptsz)
		
		self.ptsvx.extend(other_initial_conditions.ptsvx)
		self.ptsvy.extend(other_initial_conditions.ptsvy)
		self.ptsvz.extend(other_initial_conditions.ptsvz)
	
	
	def WriteInitialConditionsToFile(self, outfilename, header):
		
		headerNums = map(float, header.split())
		if len(headerNums) != 6:
			print("error: initial conditions header is not 6 numbers")
			quit()
		
		if len(self.masses) != len(self.ptsx) or len(self.ptsx) != len(self.ptsy) or len(self.ptsy) != len(self.ptsz) or len(self.ptsz) != len(self.ptsvx) or len(self.ptsvx) != len(self.ptsvy) or len(self.ptsvy) != len(self.ptsvz):
			print("error in WriteInitialConditionsToFile() -- not all arrays are the same length")
			print("len(self.masses) == "+str(len(self.masses)))
			print("len(self.ptsx) == "+str(len(self.ptsx)))
			print("len(self.ptsy) == "+str(len(self.ptsy)))
			print("len(self.ptsz) == "+str(len(self.ptsz)))
			print("len(self.ptsvx) == "+str(len(self.ptsvx)))
			print("len(self.ptsvy) == "+str(len(self.ptsvy)))
			print("len(self.ptsvz) == "+str(len(self.ptsvz)))
			return
		if len(self.masses) < 1:
			print("error in WriteInitialConditionsToFile() -- no bodies to print")
			return
		
		fo = open(outfilename, "w")
		
		# header:
		for headerNum in headerNums:
			fo.write(packbinary('d', headerNum)) #write as double precision binary
		
		# body:
		# mass, x, y, z, vx, vy, vz
		for i in range(len(self.masses)):
			
			#write as double precision binary
			fo.write(packbinary('d', self.masses[i]))
			fo.write(packbinary('d', self.ptsx[i]))
			fo.write(packbinary('d', self.ptsy[i]))
			fo.write(packbinary('d', self.ptsz[i]))
			fo.write(packbinary('d', self.ptsvx[i]))
			fo.write(packbinary('d', self.ptsvy[i]))
			fo.write(packbinary('d', self.ptsvz[i]))
		
		
		fo.close()
	




