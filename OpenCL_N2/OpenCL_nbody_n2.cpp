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
//#define __NO_STD_VECTOR // Use std::vector instead of STL version
#include <CL/cl.hpp>
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


#define PROFILING // Define to see the time the kernel takes

//=====================================================================================================
// Miscellaneous

int RoundDoubleToInt(double x) {
	return static_cast<int>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
}

//=====================================================================================================
// Aarseth File I/O

static void readParameters(FILE* inputFile, int* numParticlesPtr, float* timeStepPtr, double* finalTimePtrDbl, float* epsilonSquaredPtr) {
    printf("Enter numParticles, eta, timeStep, finalTime and epsilonSquared: ");
    fscanf(inputFile, "%i", numParticlesPtr);
    if((*numParticlesPtr) % 2 != 0)
    {
        fprintf(stderr, "Error: you should use a multiple of 2 for numparticles.\n"
                        "...perhaps even a power of 2. Entered numparticles was: %i\n", *numParticlesPtr);
        exit(1);
    }
    float eta; fscanf(inputFile, "%f", &eta); //not used
    fscanf(inputFile, "%f", timeStepPtr);
    fscanf(inputFile, "%lf", finalTimePtrDbl);
    fscanf(inputFile, "%f", epsilonSquaredPtr);
    printf("\n");
}
static void readParticles(FILE* inputFile, std::vector<cl_float4> & positionsAndMasses, std::vector<cl_float4> & velocities) {
	int numParticles = positionsAndMasses.size();
	int i, j;
	for(i=0; i<numParticles; i++)
	{
		fscanf(inputFile, "%f", &(positionsAndMasses[i].s[3]));
		for(j=0; j<3; j++)
			fscanf(inputFile, "%f", &(positionsAndMasses[i].s[j]));
		for(j=0; j<3; j++)
			fscanf(inputFile, "%f", &(velocities[i].s[j]));
	}
}
static void writeParticles(FILE* outputFile, const std::vector<cl_float4> & positionsAndMasses) {
	int nparticles = positionsAndMasses.size();
	for(int i=0; i<nparticles; i++) {
		fprintf(outputFile, "%14.7g %14.7g %14.7g\n", positionsAndMasses[i].s[0], positionsAndMasses[i].s[1], positionsAndMasses[i].s[2]);
	}
}


//=====================================================================================================
// Main loop

int main(int argc, char* argv[])
{
	srand(time(NULL));
	cl_int err;
	if(argc < 3) {
		cout<<"Usage:  [cpu/gpu]  [InitialConditionsFilename]  [optional:specifykernel]"<<endl;
		exit(1);
	}
	std::string kernelFilename("nbody_n2_kernel.cl");
	if(argc >= 4) kernelFilename = std::string(argv[3]);
	
	
	OpenCLContextAndDevices ctxtAndDevcs(argv[1], false);
	
	OpenCLKernelComputationClass kernelClass(ctxtAndDevcs);
	kernelClass.LoadKernel(kernelFilename, "nbody_kern_func_main");
	
	//------------------------------------------------------- nbody settings
	int nparticles; // should be a nice power of two for simplicity
	int nsteps;
	
	int nburst = 4;  // this is the number of steps before reading back from GPU
			 // should divide the value of nstep without remainder...
			 // MUST be divisible by two because of the way we leapfrog pos_new / pos_old
	
	int nthreads = 32; // should be the maximum number of work-items per work-group
	float dt;
	float epssqd;
	
	std::string outputFileName("out_opencl.data"); //used later
	
	//------------------------------------------------------------
	
	FILE * inputFile = fopen(argv[2], "r");
	if(!inputFile) {
		cout<<"Error: unable to open initial conditions file for reading: \""<<argv[2]<<"\""<<endl;
		return 1;
	}
	
	double finalTimeGiven;
	readParameters(inputFile, &nparticles, &dt, &finalTimeGiven, &epssqd);
	dt /= float(nburst);
	nsteps = RoundDoubleToInt(finalTimeGiven / ((double)dt));
	
	cout<<"simulation will use "<<nparticles<<" particles"<<endl;
	
	std::vector<cl_float4> positions_host(nparticles);
	std::vector<cl_float4> velocitys_host(nparticles);
	
	readParticles(inputFile, positions_host, velocitys_host);
	
	//save initial conditions as first frame of output
	FILE * outputFile = fopen(outputFileName.c_str(), "w");
	if(!outputFile) {
		cout<<"Error: unable to open output file for saving output: \""<<outputFileName<<"\""<<endl;
		return 1;
	}
	writeParticles(outputFile, positions_host);
	//------------------------------------------------------------
	
	cout<<"allocating video memory..."<<endl;
	
	const int sizeOfPosArrays = ((int)sizeof(cl_float4)) * nparticles;
	const int sizeOfPCache =    ((int)sizeof(cl_float4)) * nthreads;
	
	cl::Buffer* positionsAABuf = kernelClass.CreateBufForKernel(sizeOfPosArrays, CL_MEM_READ_WRITE, &err);	CheckCLErr(err, "cl::Buffer::Buffer()");
	cl::Buffer* positionsBBBuf = kernelClass.CreateBufForKernel(sizeOfPosArrays, CL_MEM_READ_WRITE, &err);	CheckCLErr(err, "cl::Buffer::Buffer()");
	cl::Buffer* velocitysBuf = kernelClass.CreateBufForKernel(sizeOfPosArrays, CL_MEM_READ_WRITE, &err);	CheckCLErr(err, "cl::Buffer::Buffer()");
	
	cout<<"setting kernel args"<<endl;
	
	err = kernelClass.GetKernel().setArg(0, dt);				CheckCLErr(err, "kernelClass.GetKernel().setArg(0)");
	err = kernelClass.GetKernel().setArg(1, epssqd);			CheckCLErr(err, "kernelClass.GetKernel().setArg(1)");
	err = kernelClass.GetKernel().setArg(2, *positionsAABuf); /*"old"*/	CheckCLErr(err, "kernelClass.GetKernel().setArg(2)");
	err = kernelClass.GetKernel().setArg(3, *positionsBBBuf); /*"new"*/	CheckCLErr(err, "kernelClass.GetKernel().setArg(3)");
	err = kernelClass.GetKernel().setArg(4, *velocitysBuf);			CheckCLErr(err, "kernelClass.GetKernel().setArg(4)");
	err = kernelClass.GetKernel().setArg(5, cl::Local(sizeOfPCache));			CheckCLErr(err, "kernelClass.GetKernel().setArg(5)");
	
	cout<<"writing video memory to GPU (i.e. queueing write buffer)"<<endl;
	
	//the CL_TRUE or CL_FALSE indicates whether the write is blocking
	err = ctxtAndDevcs.GetQueue().enqueueWriteBuffer(*positionsAABuf, CL_TRUE, 0, sizeOfPosArrays, &(positions_host[0]));
	CheckCLErr(err, "enqueueWriteBuffer() with positions_host");
	err = ctxtAndDevcs.GetQueue().enqueueWriteBuffer(*velocitysBuf,   CL_TRUE, 0, sizeOfPosArrays, &(velocitys_host[0]));
	CheckCLErr(err, "enqueueWriteBuffer() with velocitys_host");
	
	
	cout<<"beginning main loop"<<endl;
	//------------------------------------------------------------
	cl::Event event;
	
	for(int step=0; step<nsteps; step+=nburst) {
		for(int burst=0; burst<nburst; burst+=2) {
			
			err = kernelClass.GetKernel().setArg(2, *positionsAABuf); CheckCLErr(err, "kernel.setArg(intermediate-old)");
			err = kernelClass.GetKernel().setArg(3, *positionsBBBuf); CheckCLErr(err, "kernel.setArg(intermediate-new)");
			
			err = ctxtAndDevcs.GetQueue().enqueueNDRangeKernel(kernelClass.GetKernel(),//kernel
									cl::NullRange,		   //offset
									cl::NDRange(nparticles,1), //global
									cl::NullRange,		   //local
									nullptr,		   //events to finish before being queued
									&event);		   //event representing this execution instance
			CheckCLErr(err, "enqueueNDRangeKernel()");
			event.wait(); //wait until that processing is done
			
			kernelClass.GetKernel().setArg(2, *positionsBBBuf); //do another update: set "old" to the "intermediate-new"
			kernelClass.GetKernel().setArg(3, *positionsAABuf); //set "new" to the "intermediate-old"
			
			err = ctxtAndDevcs.GetQueue().enqueueNDRangeKernel(kernelClass.GetKernel(),//kernel
									cl::NullRange,		   //offset
									cl::NDRange(nparticles,1), //global
									cl::NDRange(1,1),	   //local
									nullptr,		   //events to finish before being queued
									&event);		   //event representing this execution instance
			CheckCLErr(err, "enqueueNDRangeKernel()");
			event.wait(); //wait until that processing is done
		}
		
		ctxtAndDevcs.GetQueue().enqueueReadBuffer(*positionsAABuf, CL_TRUE, 0, sizeOfPosArrays, &(positions_host[0]));
		ctxtAndDevcs.GetQueue().enqueueReadBuffer(*velocitysBuf,   CL_TRUE, 0, sizeOfPosArrays, &(velocitys_host[0]));
		
		//save to disk
		writeParticles(outputFile, positions_host);
		
		//cout<<"so far, completed "<<(1+(step/nburst))<<" bursts"<<endl;
		cout<<"simulation time elapsed: "<<(dt*((float)nburst)*((float)(1+(step/nburst))))<<endl;
	}
	//------------------------------------------------------------
	
	fclose(outputFile);
	
	
	return 0;
}

