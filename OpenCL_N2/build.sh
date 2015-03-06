#!/bin/bash

rm *.o
rm nbodyocl

g++ -c OpenCLComputationClasses.cpp -o OpenCLComputationClasses.o -std=c++11 -Ipath-OpenCL-include
g++ -c OpenCL_nbody_n2.cpp -o OpenCL_nbody_n2.o -std=c++11 -Ipath-OpenCL-include

g++ -o nbodyocl OpenCLComputationClasses.o OpenCL_nbody_n2.o -Lpath-OpenCL-libdir -lOpenCL

