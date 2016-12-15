#!/bin/bash
#PBS -N hello_MPI
#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=4:ppn=5

module load mpi/mpich3-x86_64
cd $PBS_O_WORKDIR

echo "=====test Run starts now ======= `date` "

mpiexec -np $PBS_NP ./../bin/game-of-life &> $PBS_JOBNAME.log

echo "====test Run ends now ======= `date` "
