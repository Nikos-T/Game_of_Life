#!/bin/bash
#PBS -N GoL-check_glider

#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=1:ppn=1

cd $PBS_O_WORKDIR

echo "=====test Run starts now ======= `date` "

./../bin/game-of-life 8 0.2 500 1 1 &> $PBS_JOBNAME.log

echo "====test Run ends now ======= `date` "
