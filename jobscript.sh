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

python run_two_plummer_collision_MPI_and_OMP.py test1 2400 1 1 3
python run_two_plummer_collision_MPI_and_OMP.py test1 2400 1 2 3
python run_two_plummer_collision_MPI_and_OMP.py test1 2400 2 2 3
python run_two_plummer_collision_MPI_and_OMP.py test1 2400 3 2 3
python run_two_plummer_collision_MPI_and_OMP.py test1 2400 4 2 3


