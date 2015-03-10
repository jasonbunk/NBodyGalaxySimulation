#ifndef ____OPENCL_COMPUTATION_CLASSES_H____
#define ____OPENCL_COMPUTATION_CLASSES_H____
/*
   OpenCLComputationClasses.h
   Manages initialization of OpenCL stuff.
   
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

#include "IncludeOpenCL.h"
#include <assert.h>
#ifndef nullptr
#define nullptr NULL
#endif

void CheckCLErr(cl_int err, const char * name);
void CheckCLErr(cl_int err, std::string name);

//==============================================================================

class OpenCLContextAndDevices
{
	cl::Context * myContext;
	std::vector<cl::Device> * myDevices;
	cl::CommandQueue * myQueue;
	int DeviceNumUsed; //single-GPU
	bool isCPU;
public:
	cl::Context & GetContext() const             {assert(myContext!=nullptr); return (*myContext);}
	std::vector<cl::Device> & GetDevices() const {assert(myDevices!=nullptr); return (*myDevices);}
	cl::CommandQueue & GetQueue() const          {assert(myQueue!=nullptr); return (*myQueue);}
	int GetDeviceNumUsed() const {return DeviceNumUsed;}
	bool IsCPU() const {return isCPU;}
	
	
	//	Init()
	//	need to initialize context and devices first
	void Init(std::string deviceType, bool profiling = false);
	
	
	//	Constructors
	//	can initialize immediately, or later
	OpenCLContextAndDevices() : myContext(nullptr), myDevices(nullptr), myQueue(nullptr) {}
	OpenCLContextAndDevices(std::string deviceType, bool profiling) : myContext(nullptr), myDevices(nullptr), myQueue(nullptr) {
		Init(deviceType, profiling);
	}
};

//==============================================================================

class OpenCLKernelComputationClass
{
	//OpenCLKernelComputationClass() : myKernel(nullptr) {}
	
	const OpenCLContextAndDevices & myParentContextAndDevices;
	cl::Kernel * myKernel;
public:
	
	//	GetKernel()
	//	retrieve the loaded kernel, for passing memory to or retrieving memory from
	cl::Kernel & GetKernel()  {assert(myKernel!=nullptr); return (*myKernel);}
	
	
	//	Constructor
	//	needs the OpenCL context and list of devices
	OpenCLKernelComputationClass(const OpenCLContextAndDevices & myParent) : myParentContextAndDevices(myParent), myKernel(nullptr) {}
	
	
	//	LoadKernel()
	//	call this first
	void LoadKernel(std::string kernelFilename, std::string kernelFunctionName);
	
	
	//	CreateBufForKernel(kernelArgNum, bufSize, bufmemflags)
	//	use to allocate memory on the device of a specific size
	//	bufmemflags can be: CL_MEM_READ_ONLY, CL_MEM_READ_WRITE, ...
	cl::Buffer * CreateBufForKernel(int bufSize, cl_mem_flags bufmemflags, cl_int* err=nullptr) const {
		return new cl::Buffer(myParentContextAndDevices.GetContext(), bufmemflags, bufSize, nullptr, err);
	}
	
	
/*
	//	AllocBufferForKernel(kernelArgNum, givenvec, bufmemflags)
	//	use this to allocate memory on the device; returns the buffer pointer so you can set or retrieve data
	//	bufmemflags can be: CL_MEM_READ_ONLY, CL_MEM_READ_WRITE, ...
	template<typename T>
	cl::Buffer * AllocBufferForKernel(int kernelArgNum, std::vector<T> & xvec, cl_mem_flags bufmemflags) {
		cl::Buffer* newbuff = new cl::Buffer(myParentContextAndDevices.GetContext(), bufmemflags, ((int)sizeof(T))*((int)xvec.size()));
		kernel.setArg(kernelArgNum, newbuff);
		return newbuff;
	}
	template<typename T>
	cl::Buffer * AllocBufferForKernel(int kernelArgNum, T & xval, cl_mem_flags bufmemflags) {
		cl::Buffer* newbuff = new cl::Buffer(myParentContextAndDevices.GetContext(), bufmemflags, (int)sizeof(T));
		kernel.setArg(kernelArgNum, newbuff);
		return newbuff;
	}
	
	
	//	AllocBufferOfSizeForKernel(kernelArgNum, bufSize, bufmemflags)
	//	use to allocate memory on the device of a specific size
	//	bufmemflags can be: CL_MEM_READ_ONLY, CL_MEM_READ_WRITE, ...
	cl::Buffer * AllocBufferOfSizeForKernel(int kernelArgNum, int bufSize, cl_mem_flags bufmemflags) {
		cl::Buffer* newbuff = new cl::Buffer(myParentContextAndDevices.GetContext(), bufmemflags, bufSize);
		kernel.setArg(kernelArgNum, newbuff);
		return newbuff;
	}
*/

};





#endif
