#!/bin/bash
#PBS -q normal
#PBS -l nodes=1:ppn=16:native
#PBS -l walltime=0:10:00
#PBS -N gravitystrongscaling
#PBS -o myOMP.out
#PBS -e myOMP.err
#PBS -A csd453
#PBS -M jason.bunk@gmail.com
#PBS -m abe
#PBS -V
# Start of user commands - comments start with a hash sign (#)

cd $PBS_O_WORKDIR
echo currentworkdir= $PBS_O_WORKDIR

(cd NBodySim_SimpleCPU && make)

./NBodySim_SimpleCPU/nbodycpp 16 data/initialconditions/2400pt_1cg_100tsteps_0.data data/results/2400pt_1cg_100tsteps_simpleomp16_
./NBodySim_SimpleCPU/nbodycpp 12 data/initialconditions/2400pt_1cg_100tsteps_0.data data/results/2400pt_1cg_100tsteps_simpleomp12_
./NBodySim_SimpleCPU/nbodycpp 8 data/initialconditions/2400pt_1cg_100tsteps_0.data data/results/2400pt_1cg_100tsteps_simpleomp8_
./NBodySim_SimpleCPU/nbodycpp 4 data/initialconditions/2400pt_1cg_100tsteps_0.data data/results/2400pt_1cg_100tsteps_simpleomp4_
./NBodySim_SimpleCPU/nbodycpp 2 data/initialconditions/2400pt_1cg_100tsteps_0.data data/results/2400pt_1cg_100tsteps_simpleomp2_
./NBodySim_SimpleCPU/nbodycpp 1 data/initialconditions/2400pt_1cg_100tsteps_0.data data/results/2400pt_1cg_100tsteps_simpleomp1_

./NBodySim_SimpleCPU/nbodycpp 16 data/initialconditions/4800pt_1cg_100tsteps_0.data data/results/4800pt_1cg_100tsteps_simpleomp16_
./NBodySim_SimpleCPU/nbodycpp 12 data/initialconditions/4800pt_1cg_100tsteps_0.data data/results/4800pt_1cg_100tsteps_simpleomp12_
./NBodySim_SimpleCPU/nbodycpp 8 data/initialconditions/4800pt_1cg_100tsteps_0.data data/results/4800pt_1cg_100tsteps_simpleomp8_
./NBodySim_SimpleCPU/nbodycpp 4 data/initialconditions/4800pt_1cg_100tsteps_0.data data/results/4800pt_1cg_100tsteps_simpleomp4_
./NBodySim_SimpleCPU/nbodycpp 2 data/initialconditions/4800pt_1cg_100tsteps_0.data data/results/4800pt_1cg_100tsteps_simpleomp2_
./NBodySim_SimpleCPU/nbodycpp 1 data/initialconditions/4800pt_1cg_100tsteps_0.data data/results/4800pt_1cg_100tsteps_simpleomp1_

./NBodySim_SimpleCPU/nbodycpp 16 data/initialconditions/7200pt_1cg_100tsteps_0.data data/results/7200pt_1cg_100tsteps_simpleomp16_
./NBodySim_SimpleCPU/nbodycpp 12 data/initialconditions/7200pt_1cg_100tsteps_0.data data/results/7200pt_1cg_100tsteps_simpleomp12_
./NBodySim_SimpleCPU/nbodycpp 8 data/initialconditions/7200pt_1cg_100tsteps_0.data data/results/7200pt_1cg_100tsteps_simpleomp8_
./NBodySim_SimpleCPU/nbodycpp 4 data/initialconditions/7200pt_1cg_100tsteps_0.data data/results/7200pt_1cg_100tsteps_simpleomp4_
./NBodySim_SimpleCPU/nbodycpp 2 data/initialconditions/7200pt_1cg_100tsteps_0.data data/results/7200pt_1cg_100tsteps_simpleomp2_
./NBodySim_SimpleCPU/nbodycpp 1 data/initialconditions/7200pt_1cg_100tsteps_0.data data/results/7200pt_1cg_100tsteps_simpleomp1_



