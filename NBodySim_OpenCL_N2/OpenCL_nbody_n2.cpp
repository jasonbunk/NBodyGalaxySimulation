/*
   OpenCL_nbody_n2.cpp
   An O(N^2) nbody simulator using OpenCL for portable GPU computing.
   
   Copyright 2015 Jason Bunk
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
       http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <utility>
#include "IncludeOpenCL.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <stdint.h>
#include <math.h>
#include "OpenCLComputationClasses.h"
using std::cout; using std::endl;


//#define _USE_DOUBLE_PRECISION___ 1

#ifdef _USE_DOUBLE_PRECISION___
#define myflt cl_double
#define myflt4 cl_double4
#else
#define myflt cl_float
#define myflt4 cl_float4
#endif


//#define PROFILING // Define to see the time the kernel takes

static myflt NEWTONS_GRAVITATIONAL_CONSTANT = (myflt)1.0;


//=====================================================================================================
// Miscellaneous

int RoundDoubleToInt(double x) {
	return static_cast<int>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
}

bool IsPowerOfTwo(unsigned int x) {
    return (x & (x - 1)) == 0;
}

//=====================================================================================================
// Aarseth File I/O

#define DBLBYTES 8
#define FLTBYTES 4

static void readParameters(FILE* inputFile, int* numParticlesPtr, myflt* timeStepPtr, double* finalTimePtrDbl, myflt* epsilonSquaredPtr) {
	if(sizeof(double) != DBLBYTES){cout<<"WARNING: unexpected sizeof(double) == "<<sizeof(double)<<endl;}
	double temp;
	cout<<"Initial conditions file header should be: (0)numParticles, (1)eta, (2)timeStep, (3)finalTime, (4)epsilonSquared, (5)gravityconst_G"<<endl;
	
	//read numparticles
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 0"<<endl; exit(1);}
	(*numParticlesPtr) = RoundDoubleToInt(temp);
	
	//read eta... not used
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 1"<<endl; exit(1);}
	
	//read timestep
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 2"<<endl; exit(1);}
	(*timeStepPtr) = (myflt)temp;
	
	//read finalTime
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 3"<<endl; exit(1);}
	(*finalTimePtrDbl) = temp;
	
	//read epsilonSquared
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 4"<<endl; exit(1);}
	(*epsilonSquaredPtr) = (myflt)temp;
	
	//read G
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 5"<<endl; exit(1);}
	NEWTONS_GRAVITATIONAL_CONSTANT = (myflt)temp;
}
static void readParticles(FILE* inputFile, int nparticles, std::vector<myflt4> & positionsAndMasses, std::vector<myflt4> & velocities) {
	if(sizeof(double) != DBLBYTES){cout<<"WARNING: unexpected sizeof(double) == "<<sizeof(double)<<endl;}
	double temp;
	int i, j;
	for(i=0; i<nparticles; i++)
	{
		if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 6 (read "<<nparticles<<" particles before failing)"<<endl; exit(1);}
		positionsAndMasses[i].s[3] = (myflt)temp; assert(isnan(positionsAndMasses[i].s[3])==false);
		for(j=0; j<3; j++) {
			if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 7 (read "<<nparticles<<" particles before failing)"<<endl; exit(1);}
			positionsAndMasses[i].s[j] = (myflt)temp; assert(isnan(positionsAndMasses[i].s[j])==false);
		}
		for(j=0; j<3; j++) {
			if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 8 (read "<<nparticles<<" particles before failing)"<<endl; exit(1);}
			velocities[i].s[j] = temp; assert(isnan(velocities[i].s[j])==false);
		}
	}
}
static void writeParticles(FILE* outputFile, int nparticles, const std::vector<myflt4> & positionsAndMasses) {
	if(sizeof(float) != FLTBYTES){cout<<"WARNING: unexpected sizeof(float) == "<<sizeof(float)<<endl;}
	float writeme;
	size_t nwritten;
	for(int i=0; i<nparticles; i++) {
		writeme=(float)positionsAndMasses[i].s[0];   nwritten  = fwrite(&writeme, FLTBYTES, 1, outputFile);
		writeme=(float)positionsAndMasses[i].s[1];   nwritten += fwrite(&writeme, FLTBYTES, 1, outputFile);
		writeme=(float)positionsAndMasses[i].s[2];   nwritten += fwrite(&writeme, FLTBYTES, 1, outputFile);
		if(nwritten < 3){cout<<"Error writing to output file"<<endl; exit(1);}
	}
}


//=====================================================================================================
// Main loop

int main(int argc, char* argv[])
{
#ifdef _USE_DOUBLE_PRECISION___
	cout<<"NOTE: this version of nbodyocl was compiled for DOUBLE PRECISION 64-bit doubles"<<endl;
#else
	cout<<"NOTE: this version of nbodyocl was compiled for SINGLE PRECISION 32-bit floats"<<endl;
#endif
	
	cl_int err;
	if(argc < 3) {
		cout<<"Usage:  [cpu/gpu]  [InitialConditionsFilename]  [optional:specifykernel]"<<endl;
		exit(1);
	}
	std::string kernelFilename("nbody_kernel_verlet.cl");
	if(argc >= 4) kernelFilename = std::string(argv[3]);
	
	
#ifdef PROFILING
	OpenCLContextAndDevices ctxtAndDevcs(argv[1], true);
#else
	OpenCLContextAndDevices ctxtAndDevcs(argv[1], false);
#endif
	
	OpenCLKernelComputationClass kernelClass(ctxtAndDevcs);
	kernelClass.LoadKernel(kernelFilename, "nbody_kern_func_main");
	
	//------------------------------------------------------- nbody settings
	int nparticles; // needs to be be a power of two
	int nsteps;
	
	int nburst = 6;  // this is the number of steps before reading back from GPU
			 // should divide the value of nstep without remainder...
			 // MUST be divisible by three
	
	int nthreads = 32; // should be the maximum number of work-items per work-group
	myflt dt;
	myflt epssqd;
	
	std::string outputFileName("out_opencl.data"); //used later
	
	//------------------------------------------------------------
	
	FILE * inputFile = fopen(argv[2], "r");
	if(!inputFile) {
		cout<<"Error: unable to open initial conditions file for reading: \""<<argv[2]<<"\""<<endl;
		return 1;
	}
	
	double finalTimeGiven;
	readParameters(inputFile, &nparticles, &dt, &finalTimeGiven, &epssqd);
	dt /= myflt(nburst);
	nsteps = RoundDoubleToInt(finalTimeGiven / ((double)dt));
	
	int numPaddingPts = 0;
	
	if(true){//ctxtAndDevcs.IsCPU() == false) {
		while(IsPowerOfTwo((unsigned int)(nparticles+numPaddingPts)) == false) {
			numPaddingPts++;
		}
		if(numPaddingPts > 0) {
			cout<<"simulation will use "<<nparticles<<" real particles, and "<<numPaddingPts<<" padding particles"<<endl;
		} else {
			cout<<"simulation will use "<<nparticles<<" particles"<<endl;
		}
	}
	
	std::vector<myflt4> positions_host(nparticles+numPaddingPts);
	
	std::vector<myflt4> velocitysInitial(nparticles+numPaddingPts);
	std::vector<myflt4> positionsOneStepBackForVerlet(nparticles+numPaddingPts);
	
	readParticles(inputFile, nparticles, positionsOneStepBackForVerlet, velocitysInitial);
	
	myflt4 accelerationsInitial;
	myflt atemp0, atemp1, atemp2;
	myflt temp;
	for(int i=0; i<nparticles; i++) {
		accelerationsInitial.s[0] = accelerationsInitial.s[1] = accelerationsInitial.s[2] = accelerationsInitial.s[3] = ((myflt)0.0);
		for(int j=0; j<nparticles; j++) {
			if(i != j) {
				atemp0 = positionsOneStepBackForVerlet[j].s[0] - positionsOneStepBackForVerlet[i].s[0];
				atemp1 = positionsOneStepBackForVerlet[j].s[1] - positionsOneStepBackForVerlet[i].s[1];
				atemp2 = positionsOneStepBackForVerlet[j].s[2] - positionsOneStepBackForVerlet[i].s[2];
				temp = sqrt(atemp0*atemp0 + atemp1*atemp1 + atemp2*atemp2);
				temp = (positionsOneStepBackForVerlet[j].s[3] * (NEWTONS_GRAVITATIONAL_CONSTANT)) / (temp*temp*temp); //G==1
				
				accelerationsInitial.s[0] += atemp0*temp;
				accelerationsInitial.s[1] += atemp1*temp;
				accelerationsInitial.s[2] += atemp2*temp;
			}
		}
		positions_host[i].s[0] = positionsOneStepBackForVerlet[i].s[0] + dt*velocitysInitial[i].s[0] + ((myflt)0.5)*dt*dt*accelerationsInitial.s[0];
		positions_host[i].s[1] = positionsOneStepBackForVerlet[i].s[1] + dt*velocitysInitial[i].s[1] + ((myflt)0.5)*dt*dt*accelerationsInitial.s[1];
		positions_host[i].s[2] = positionsOneStepBackForVerlet[i].s[2] + dt*velocitysInitial[i].s[2] + ((myflt)0.5)*dt*dt*accelerationsInitial.s[2];
		positions_host[i].s[3] = positionsOneStepBackForVerlet[i].s[3];
	}
	for(int i=0; i<numPaddingPts; i++) {
		int j = (nparticles + i);
		positions_host[j].s[0] = positions_host[j].s[1] = positions_host[j].s[2] = positions_host[j].s[3] = ((myflt)0.0);
		positionsOneStepBackForVerlet[j].s[0] = positionsOneStepBackForVerlet[j].s[1] = positionsOneStepBackForVerlet[j].s[2] = positionsOneStepBackForVerlet[j].s[3] = ((myflt)0.0);
		positions_host[j].s[2] = ((myflt)i)*((myflt)0.1);
		positionsOneStepBackForVerlet[j].s[2] = ((myflt)i)*((myflt)0.1);
	}
	
	//create output file
	FILE * outputFile = fopen(outputFileName.c_str(), "w");
	if(!outputFile) {
		cout<<"Error: unable to open output file for saving output: \""<<outputFileName<<"\""<<endl;
		return 1;
	}
	//write output header: first 8 bytes are number of particles; next 8 bytes are currently unused
	int64_t NPARTICLESWRITEME = (int64_t)nparticles;
	fwrite(&NPARTICLESWRITEME, 8, 1, outputFile);
	NPARTICLESWRITEME = 0;
	fwrite(&NPARTICLESWRITEME, 8, 1, outputFile);
	//write the first two timesteps
	writeParticles(outputFile, nparticles, positionsOneStepBackForVerlet);
	writeParticles(outputFile, nparticles, positions_host);
	
	//------------------------------------------------------------
	
	cout<<"allocating video memory..."<<endl;
	
	const int sizeOfPosArrays = ((int)sizeof(myflt4)) * (nparticles+numPaddingPts);
	const int sizeOfPCache =    ((int)sizeof(myflt4)) * nthreads;
	
	cl::Buffer* positionsAABuf = kernelClass.CreateBufForKernel(sizeOfPosArrays, CL_MEM_READ_WRITE, &err);	CheckCLErr(err, "cl::Buffer::Buffer()");
	cl::Buffer* positionsBBBuf = kernelClass.CreateBufForKernel(sizeOfPosArrays, CL_MEM_READ_WRITE, &err);	CheckCLErr(err, "cl::Buffer::Buffer()");
	cl::Buffer* positionsCCBuf = kernelClass.CreateBufForKernel(sizeOfPosArrays, CL_MEM_READ_WRITE, &err);	CheckCLErr(err, "cl::Buffer::Buffer()");
	
	cout<<"setting kernel args"<<endl;
	
	err = kernelClass.GetKernel().setArg(0, dt);				CheckCLErr(err, "kernelClass.GetKernel().setArg(0)");
	err = kernelClass.GetKernel().setArg(1, epssqd);			CheckCLErr(err, "kernelClass.GetKernel().setArg(1)");
	err = kernelClass.GetKernel().setArg(2, NEWTONS_GRAVITATIONAL_CONSTANT);CheckCLErr(err, "kernelClass.GetKernel().setArg(2)");
	err = kernelClass.GetKernel().setArg(3, numPaddingPts);			CheckCLErr(err, "kernelClass.GetKernel().setArg(3)");
	err = kernelClass.GetKernel().setArg(4, *positionsAABuf);		CheckCLErr(err, "kernelClass.GetKernel().setArg(4)");
	err = kernelClass.GetKernel().setArg(5, *positionsBBBuf);		CheckCLErr(err, "kernelClass.GetKernel().setArg(5)");
	err = kernelClass.GetKernel().setArg(6, *positionsCCBuf);		CheckCLErr(err, "kernelClass.GetKernel().setArg(6)");
	err = kernelClass.GetKernel().setArg(7, cl::__local(sizeOfPCache));	CheckCLErr(err, "kernelClass.GetKernel().setArg(7)");
	
	cout<<"writing video memory to device (i.e. queueing write buffer)"<<endl;
	
	//the CL_TRUE or CL_FALSE indicates whether the write is blocking
	err = ctxtAndDevcs.GetQueue().enqueueWriteBuffer(*positionsAABuf, CL_TRUE, 0, sizeOfPosArrays, &(positionsOneStepBackForVerlet[0]));
	CheckCLErr(err, "enqueueWriteBuffer() with positionsOneStepBackForVerlet");
	err = ctxtAndDevcs.GetQueue().enqueueWriteBuffer(*positionsBBBuf, CL_TRUE, 0, sizeOfPosArrays, &(positions_host[0]));
	CheckCLErr(err, "enqueueWriteBuffer() with positions_host");
	err = ctxtAndDevcs.GetQueue().enqueueWriteBuffer(*positionsCCBuf, CL_TRUE, 0, sizeOfPosArrays, &(positions_host[0]));
	CheckCLErr(err, "enqueueWriteBuffer() with positions_host on buf CC to save masses");
	
	
	cout<<"beginning main loop"<<endl;
	//------------------------------------------------------------
	cl::Event event;
	
	for(int step=0; step<nsteps; step+=nburst) {
		for(int burst=0; burst<nburst; burst+=3) {
			
			err = kernelClass.GetKernel().setArg(4, *positionsAABuf); CheckCLErr(err, "kernel.setArg(4) loop-0");
			err = kernelClass.GetKernel().setArg(5, *positionsBBBuf); CheckCLErr(err, "kernel.setArg(5) loop-0");
			err = kernelClass.GetKernel().setArg(6, *positionsCCBuf); CheckCLErr(err, "kernel.setArg(6) loop-0");
			
			err = ctxtAndDevcs.GetQueue().enqueueNDRangeKernel(kernelClass.GetKernel(),//kernel
									cl::NullRange,		   //offset
									cl::NDRange(nparticles+numPaddingPts),   //global
									cl::NDRange(nthreads),	   //local
									nullptr,		   //events to finish before being queued
									&event);		   //event representing this execution instance
			CheckCLErr(err, "enqueueNDRangeKernel()");
			event.wait(); //wait until that processing is done
			
			err = kernelClass.GetKernel().setArg(4, *positionsBBBuf); CheckCLErr(err, "kernel.setArg(4) loop-1");
			err = kernelClass.GetKernel().setArg(5, *positionsCCBuf); CheckCLErr(err, "kernel.setArg(5) loop-1");
			err = kernelClass.GetKernel().setArg(6, *positionsAABuf); CheckCLErr(err, "kernel.setArg(6) loop-1");
			
			err = ctxtAndDevcs.GetQueue().enqueueNDRangeKernel(kernelClass.GetKernel(),//kernel
									cl::NullRange,		   //offset
									cl::NDRange(nparticles+numPaddingPts),   //global
									cl::NDRange(nthreads),	   //local
									nullptr,		   //events to finish before being queued
									&event);		   //event representing this execution instance
			CheckCLErr(err, "enqueueNDRangeKernel()");
			event.wait(); //wait until that processing is done
			
			err = kernelClass.GetKernel().setArg(4, *positionsCCBuf); CheckCLErr(err, "kernel.setArg(4) loop-2");
			err = kernelClass.GetKernel().setArg(5, *positionsAABuf); CheckCLErr(err, "kernel.setArg(5) loop-2");
			err = kernelClass.GetKernel().setArg(6, *positionsBBBuf); CheckCLErr(err, "kernel.setArg(6) loop-2");
			
			err = ctxtAndDevcs.GetQueue().enqueueNDRangeKernel(kernelClass.GetKernel(),//kernel
									cl::NullRange,		   //offset
									cl::NDRange(nparticles+numPaddingPts),   //global
									cl::NDRange(nthreads),	   //local
									nullptr,		   //events to finish before being queued
									&event);		   //event representing this execution instance
			CheckCLErr(err, "enqueueNDRangeKernel()");
			event.wait(); //wait until that processing is done
			
#ifdef PROFILING
			cl_ulong start= event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
			cl_ulong end  = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
			double time = 1.e-9 * (((double)end)-((double)start));
			cout << "Time for kernel to execute " << time << endl;
#endif
		}
		
		ctxtAndDevcs.GetQueue().enqueueReadBuffer(*positionsBBBuf, CL_TRUE, 0, sizeOfPosArrays, &(positions_host[0]));
		
		//save to disk
		writeParticles(outputFile, nparticles, positions_host);
		
		//cout<<"so far, completed "<<(1+(step/nburst))<<" bursts"<<endl;
		cout<<"simulation time elapsed: "<<(dt*((myflt)nburst)*((myflt)(1+(step/nburst))))<<endl;
	}
	//------------------------------------------------------------
	
	fclose(outputFile);
	
	return 0;
}

