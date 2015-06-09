#!/bin/bash
#PBS -q normal
#PBS -l nodes=4:ppn=16:native
#PBS -l walltime=0:15:00
#PBS -N gravitystrongscaling
#PBS -o my.out
#PBS -e my.err
#PBS -A csd453
#PBS -M jason.bunk@gmail.com
#PBS -m jason
#PBS -V
# Start of user commands - comments start with a hash sign (#)

cd $PBS_O_WORKDIR
echo currentworkdir= $PBS_O_WORKDIR

mpirun -np 1 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/t2400p_1c_1m_ data/results/t2400p_1c_1m_
mpirun -np 2 ./NBodySim_CPU_MPI/nbodycpumpi 2 3 data/initialconditions/t2400p_2c_2m_ data/results/t2400p_2c_2m_
mpirun -np 3 ./NBodySim_CPU_MPI/nbodycpumpi 3 3 data/initialconditions/t2400p_2c_4m_ data/results/t2400p_2c_4m_
mpirun -np 4 ./NBodySim_CPU_MPI/nbodycpumpi 4 3 data/initialconditions/t2400p_1c_4m_ data/results/t2400p_1c_4m_

