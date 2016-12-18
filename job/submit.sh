#!/bin/bash
#PBS -N game-of-life
#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=4:ppn=5

module load mpi/mpich3-x86_64
cd $PBS_O_WORKDIR

echo "=====test Run starts now ======= `date` "

mpiexec -np $PBS_NP ./../bin/game-of-life 10 0.2 3 1 &> $PBS_JOBNAME.log

echo "====test Run ends now ======= `date` "
