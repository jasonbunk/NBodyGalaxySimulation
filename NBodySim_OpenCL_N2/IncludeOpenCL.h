#ifndef ___INCLUDE_OPENCL_H_________
#define ___INCLUDE_OPENCL_H_________

//#define __NO_STD_VECTOR // Use std::vector instead of STL version

#ifdef __APPLE__
#include <OpenCL/cl.h>
#include "HeaderCL12ForAppleMavericks/cl.hpp"
#elif defined __ANDROID_MALI_OPENCL_
#pragma warning "compiling for android!!! 222222222222222222"
#include "HeaderCL11forAndroid/cl.hpp"
#else
#include <CL/cl.hpp>
#endif


#endif
