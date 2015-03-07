#ifndef ___INCLUDE_OPENCL_H_________
#define ___INCLUDE_OPENCL_H_________

//#define __NO_STD_VECTOR // Use std::vector instead of STL version

#ifdef __APPLE__
#include <OpenCL/cl.h>
#include "HeaderCL12ForAppleMavericks/cl.hpp"
#else
#include <CL/cl.hpp>
#endif


#endif