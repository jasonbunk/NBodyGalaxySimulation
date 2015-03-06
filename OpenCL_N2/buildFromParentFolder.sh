#!/bin/bash

rm *.o
rm nbodyocl

g++ -c OpenCL_N2/OpenCLComputationClasses.cpp -o OpenCL_N2/OpenCLComputationClasses.o -std=c++11 -Ipath-OpenCL-include
g++ -c OpenCL_N2/OpenCL_nbody_n2.cpp -o OpenCL_N2/OpenCL_nbody_n2.o -std=c++11 -Ipath-OpenCL-include

g++ -o OpenCL_N2/nbodyocl OpenCL_N2/OpenCLComputationClasses.o OpenCL_N2/OpenCL_nbody_n2.o -Lpath-OpenCL-libdir -lOpenCL

