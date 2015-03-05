
class InitialConditions:
	
	def __init__(self):
		self.masses = []
		
		self.ptsx = []
		self.ptsy = []
		self.ptsz = []
		
		self.ptsvx = []
		self.ptsvy = []
		self.ptsvz = []
	
	def extend(self, other_initial_conditions):
		self.masses.extend(other_initial_conditions.masses)
		
		self.ptsx.extend(other_initial_conditions.ptsx)
		self.ptsy.extend(other_initial_conditions.ptsy)
		self.ptsz.extend(other_initial_conditions.ptsz)
		
		self.ptsvx.extend(other_initial_conditions.ptsvx)
		self.ptsvy.extend(other_initial_conditions.ptsvy)
		self.ptsvz.extend(other_initial_conditions.ptsvz)
	
	
	def WriteInitialConditionsToFile(self, outfilename, header):
		
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
		fo.write(header)

		# body:
		# mass, x, y, z, vx, vy, vz
		for i in range(len(self.masses)):
			fo.write(str(self.masses[i])+"\t"+str(self.ptsx[i])+"\t"+str(self.ptsy[i])+"\t"+str(self.ptsz[i])+"\t"+str(self.ptsvx[i])+"\t"+str(self.ptsvy[i])+"\t"+str(self.ptsvz[i])+"\n")
		
		fo.close()
	
	
