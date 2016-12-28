#!/bin/bash
#PBS -N GoL-check_glider

#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=2:ppn=8

module load mpi/mpich3-x86_64
cd $PBS_O_WORKDIR
export OMP_NUM_THREADS=8

echo "=====test Run starts now ======= `date` "

mpiexec -np $PBS_NP ./../bin/game-of-life 16 0.2 500 1 1 &> $PBS_JOBNAME.log

echo "====test Run ends now ======= `date` "
