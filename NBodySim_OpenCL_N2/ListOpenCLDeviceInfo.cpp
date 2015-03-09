#include "ListOpenCLDeviceInfo.h"
#include "IncludeOpenCL.h"
#include <stdio.h>
#include <stdlib.h>

void ListOpenCLDeviceInfo()
{
	int i, j;
	char* value;
	size_t valueSize;
	cl_uint platformCount;
	cl_platform_id* platforms;
	cl_uint deviceCount;
	cl_device_id* devices;
	cl_uint maxComputeUnits;

	// get all platforms
	clGetPlatformIDs(0, NULL, &platformCount);
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL);

	for (i = 0; i < platformCount; i++) {

		// get all devices
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
		devices = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);
		
		// for each device print critical attributes
		for (j = 0; j < deviceCount; j++) {

			// print device name
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
			value = (char*) malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
			printf("%d. Device: %s\n", j+1, value);
			free(value);

			// print hardware device version
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
			value = (char*) malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
			printf(" %d Hardware version: %s\n", j+1, value);
			free(value);

			// print software driver version
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
			value = (char*) malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
			printf(" %d Software version: %s\n", j+1, value);
			free(value);

			// print c version supported by compiler for device
			clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
			value = (char*) malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
			printf(" %d OpenCL C version: %s\n", j+1, value);
			free(value);
			
			// device type
			cl_device_type devtype;
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(devtype), &devtype, NULL);
			if(devtype == CL_DEVICE_TYPE_CPU)		printf(" %d Device type: CPU\n",j+1);
			else if(devtype == CL_DEVICE_TYPE_GPU)		printf(" %d Device type: GPU\n",j+1);
			else if(devtype == CL_DEVICE_TYPE_ACCELERATOR)	printf(" %d Device type: ACCELERATOR\n",j+1);
			else						printf(" %d Device type: UNKNOWN\n",j+1);
			
			// print parallel compute units
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
			printf(" %d Parallel compute units: %d\n", j+1, maxComputeUnits);
			
			// max work group size
			size_t devcores2;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(devcores2), &devcores2, NULL);
			printf(" %d Max work group size: %u\n", j+1, (cl_uint)devcores2);
			
			// max clock frequency
			cl_uint devfreq;
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(devfreq), &devfreq, NULL);
			printf(" %d Max clock frequency: %u\n", j+1, devfreq);
			
			// memory in MB
			cl_ulong devmem;
			clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(devmem), &devmem, NULL);
			cl_uint devmemMB = (cl_uint)(devmem / 1000000);
			printf(" %d Device global memory (MB): %u\n", j+1, devmemMB);
			
			// double precision support?
			cl_int supported;
			clGetDeviceInfo(devices[j], CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, sizeof(supported), &supported, NULL);
			printf(" %d Double precision supported: %s\n", j+1, supported?"YES":"NO");
			
			printf("--------------------------------------------------------\n");
		}
		free(devices);
	}
	free(platforms);
}
