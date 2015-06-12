#!/bin/bash
#PBS -q normal
#PBS -l nodes=2:ppn=16:native
#PBS -l walltime=0:20:00
#PBS -N gravitystrongscaling
#PBS -o my4.out
#PBS -e my4.err
#PBS -A csd453
#PBS -M jason.bunk@gmail.com
#PBS -m abe
#PBS -V
# Start of user commands - comments start with a hash sign (#)

cd $PBS_O_WORKDIR
echo currentworkdir= $PBS_O_WORKDIR

(cd NBodySim_CPU_MPI && make)

#mpirun -np 1 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/2400pt_1cg_100tsteps_ data/results/2400pt_1cg_100tsteps_1mpi_
#mpirun -np 2 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/2400pt_1cg_100tsteps_ data/results/2400pt_1cg_100tsteps_2mpi_
mpirun -np 4 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/2400pt_1cg_100tsteps_ data/results/2400pt_1cg_100tsteps_4mpi_

#mpirun -np 2 ./NBodySim_CPU_MPI/nbodycpumpi 2 3 data/initialconditions/2400pt_2cg_100tsteps_ data/results/2400pt_2cg_100tsteps_2mpi_
mpirun -np 4 ./NBodySim_CPU_MPI/nbodycpumpi 2 3 data/initialconditions/2400pt_2cg_100tsteps_ data/results/2400pt_2cg_100tsteps_4mpi_

#mpirun -np 1 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/4800pt_1cg_100tsteps_ data/results/4800pt_1cg_100tsteps_1mpi_
#mpirun -np 2 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/4800pt_1cg_100tsteps_ data/results/4800pt_1cg_100tsteps_2mpi_
mpirun -np 4 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/4800pt_1cg_100tsteps_ data/results/4800pt_1cg_100tsteps_4mpi_

#mpirun -np 2 ./NBodySim_CPU_MPI/nbodycpumpi 2 3 data/initialconditions/4800pt_2cg_100tsteps_ data/results/4800pt_2cg_100tsteps_2mpi_
mpirun -np 4 ./NBodySim_CPU_MPI/nbodycpumpi 2 3 data/initialconditions/4800pt_2cg_100tsteps_ data/results/4800pt_2cg_100tsteps_4mpi_

#mpirun -np 1 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/7200pt_1cg_100tsteps_ data/results/7200pt_1cg_100tsteps_1mpi_
#mpirun -np 2 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/7200pt_1cg_100tsteps_ data/results/7200pt_1cg_100tsteps_2mpi_
mpirun -np 4 ./NBodySim_CPU_MPI/nbodycpumpi 1 3 data/initialconditions/7200pt_1cg_100tsteps_ data/results/7200pt_1cg_100tsteps_4mpi_

#mpirun -np 2 ./NBodySim_CPU_MPI/nbodycpumpi 2 3 data/initialconditions/7200pt_2cg_100tsteps_ data/results/7200pt_2cg_100tsteps_2mpi_
mpirun -np 4 ./NBodySim_CPU_MPI/nbodycpumpi 2 3 data/initialconditions/7200pt_2cg_100tsteps_ data/results/7200pt_2cg_100tsteps_4mpi_




