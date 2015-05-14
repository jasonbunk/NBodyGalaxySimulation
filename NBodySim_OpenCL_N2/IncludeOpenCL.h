#ifndef ___INCLUDE_OPENCL_H_________
#define ___INCLUDE_OPENCL_H_________

//#define __NO_STD_VECTOR // Use std::vector instead of STL version



#ifdef __APPLE__
#include <OpenCL/cl.h>
#endif



// include C++ OpenCL header

#ifdef __ANDROID_MALI_OPENCL_
#pragma warning "compiling for android!!! 222222222222222222"
#include "HeaderCL11forAndroid/cl.hpp"

#elif defined USE_HEADER_CL12_FOR_CPP
#include "HeaderCL12ForCPP/cl.hpp"

#else
#include <CL/cl.hpp>

#endif






#endif
