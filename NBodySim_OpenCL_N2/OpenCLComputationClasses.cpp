/*
   OpenCLComputationClasses.cpp
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

#include "OpenCLComputationClasses.h"
#include "ListOpenCLDeviceInfo.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
using std::cout; using std::endl;

void CheckCLErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		cout << "CLERROR: " << name << " -- (" << err << ")" << endl;
		exit(1);
	}
}
void CheckCLErr(cl_int err, std::string name) {
	CheckCLErr(err, name.c_str());
}

//=============================================================================================================
// Init context and devices
//=============================================================================================================

void OpenCLContextAndDevices::Init(std::string deviceType, bool profiling /*=false*/)
{
	if(myContext != nullptr || myDevices != nullptr || myQueue != nullptr) {
		cout << "OpenCLContextAndDevices::Init() -- already initialized"<<endl;
		return;
	}
	cl_int err;
	
	ListOpenCLDeviceInfo(); //print info
	//exit(0);
	
	
	cout<<"........ initializing context and devices..."<<endl;
	
	// handle command-line arguments
	int deviceTypeCode = (deviceType.compare("cpu") == 0 ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU);
	if(deviceTypeCode == CL_DEVICE_TYPE_CPU) {
		cout<<"will request CPU device"<<endl;
		isCPU = true;
	} else {
		isCPU = false;
	}
	
//try {
	//---------------------------------------------
	//Create OpenCL context
	std::vector<cl::Platform> platformList;
	cl::Platform::get(&platformList);
	CheckCLErr(platformList.size()!=0 ? CL_SUCCESS : -1, "cl::Platform::get");
	std::cout << "Platform number is: " << platformList.size() << std::endl;
	std::string platformVendor;
	platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
	std::cout << "Platform is by: " << platformVendor << "\n";
	
	cl_context_properties cprops[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
	
	myContext = new cl::Context(deviceTypeCode,
					cprops,
					NULL, //runtime callback for reporting errors
					NULL,
					&err);
	CheckCLErr(err, "Conext::Context()");
	
	//---------------------------------------------
	//OpenCL Devices
	myDevices = new std::vector<cl::Device>(myContext->getInfo<CL_CONTEXT_DEVICES>());
	CheckCLErr(myDevices->size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");
	cout << "Found " << ((int)myDevices->size()) << " OpenCL device(s)" << endl;
	
	DeviceNumUsed = 0;
	/*if(deviceTypeCode == CL_DEVICE_TYPE_CPU) {
		DeviceNumUsed = 1;
		cout<<"Choosing device 1, assuming that that is probably the CPU"<<endl;
	}*/
	
	cout << "Will use device "<<GetDeviceNumUsed()<<endl;
	
	//---------------------------------------------
	//OpenCL CommandQueue
	if(profiling) {
		cout<<"profiling ENABLED"<<endl;
		myQueue = new cl::CommandQueue(GetContext(), GetDevices()[GetDeviceNumUsed()], CL_QUEUE_PROFILING_ENABLE);
	} else {
		cout<<"profiling disabled"<<endl;
		myQueue = new cl::CommandQueue(GetContext(), GetDevices()[GetDeviceNumUsed()], 0);
	}
	
//} catch (cl::Error error) {
//	cout << "caught exception: " << error.what() << '(' << error.err() << ')' << endl;
//}
	cout<<"........ DONE initializing context and devices"<<endl;
}


//=============================================================================================================
// Load and build kernel
//=============================================================================================================

void OpenCLKernelComputationClass::LoadKernel(std::string kernelFilename, std::string kernelFunctionName)
{
	if(myKernel != nullptr) {
		cout << "OpenCLKernelComputationClass::LoadKernel() -- already loaded"<<endl;
		return;
	}
	cl_int err;
	
	cout<<"........ loading kernel..."<<endl;
	
	//---------------------------------------------
	//Open and load the OpenCL compute program
	std::ifstream file(kernelFilename.c_str());
	CheckCLErr(file.is_open() ? CL_SUCCESS:-1, kernelFilename);
	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()+1));
	cl::Program program(myParentContextAndDevices.GetContext(), source);
	
	//---------------------------------------------
	// Build (compile) it for the device(s)
	//try {
	err = program.build(myParentContextAndDevices.GetDevices());
	//} catch(cl::Error& err2) {
	if (err != CL_SUCCESS) {
		// Get the build log
		cout << "CLERROR: Program kernel build failed! (" << err << ")" << endl;
		cout << "retrieving  log ... " << endl;
		cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(myParentContextAndDevices.GetDevices()[myParentContextAndDevices.GetDeviceNumUsed()]) << endl;
		exit(1);
	}
	
	myKernel = new cl::Kernel(program, kernelFunctionName.c_str(), &err);
	CheckCLErr(err, "Kernel::Kernel()");
	
	//---------------------------------------------
	// Get the maximum work group size for executing the kernel on the device
	/*size_t WGS;
	err = myKernel->GetKernelWorkGroupInfo(myParentContextAndDevices.GetDevices()[myParentContextAndDevices.GetDeviceNumUsed()], 
						CL_KERNEL_WORK_GROUP_SIZE, 
						&WGS);
	CheckCLErr(err, "GetKernelWorkGroupInfo()");
	cout<<"~~~~~~~~ Maximum work group size for this kernel == "<<WGS<<endl;*/
	
	cout<<"........ DONE loading kernel"<<endl;
}

















