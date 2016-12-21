#!/bin/bash
#PBS -N game-of-life

#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=4:ppn=8

module load mpi/mpich3-x86_64
cd $PBS_O_WORKDIR
export OMP_NUM_THREADS=8
echo "=====test Run starts now ======= `date` "

mpiexec -np $PBS_NP ./../bin/game-of-life 40000 0.05 3 0 &> $PBS_JOBNAME.log


echo "====test Run ends now ======= `date` "
