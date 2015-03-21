import math
import numpy as np
import sys
import os
from math import cos
from math import sin
import struct
from struct import pack as packbinary

def printargs():
	print("arguments:   {inputFileToConvert}   {outputFilename}")
	print("note: jphys141 files end with .data")
if len(sys.argv) <= 2:
    printargs()
    quit()

inputFileToConvert = str(sys.argv[1])
outputFilename = str(sys.argv[2])

fin = open(inputFileToConvert, 'rb')
fo  = open(outputFilename, 'w')

INTSIZE = 4
DBLSIZE = 8


#--------------------------------------------------------------
# first convert header

#AarsethHeader = str(TotalNumPts)+" 0.01 "+str(timeStep)+" "+str(timeMax)+" "+str(epssqd)+" "+str(GravitationalConst)+"\n"

nbodies = int(struct.unpack('d',fin.read(DBLSIZE))[0])
fo.write(str(nbodies))
for i in range(5):
	fo.write(" "+str(struct.unpack('d',fin.read(DBLSIZE))[0]))
fo.write("\n")

for i in range(nbodies):
	for j in range(7):
		if j > 0:
			fo.write(" ")
		fo.write(str(struct.unpack('d',fin.read(DBLSIZE))[0]))
	fo.write("\n")


fo.close()
fin.close()





