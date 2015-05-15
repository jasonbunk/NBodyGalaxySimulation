import os, time

npartspow = [10, 11, 12, 13, 14, 15, 16, 17]
npartsperthreadoptions = [1, 2, 4, 8]

for poww in npartspow:
	nparticles = 2**poww
	for partsperthread in npartsperthreadoptions:
		print("running: "+str(nparticles)+" parts, "+str(partsperthread)+" parts per thread, "+str(nparticles*partsperthread)+" interactions per thread, "+str(nparticles/partsperthread)+" threads")
		starttime = time.time() #time.clock()
		os.system("python run_plummer_benchmark.py "+str(nparticles)+" opencl-gpu 0 "+str(partsperthread)+" > /dev/null")
		endtime = time.time() #time.clock()
		print("     TIME TO RUN THAT BENCHMARK: "+str(endtime-starttime)+" seconds")

