#!/bin/bash
#PBS -N GoL-4Nodes

#PBS -q pdlab
#PBS -j oe
#PBS -l nodes=1:ppn=1


cd $PBS_O_WORKDIR

echo "=====test Run starts now ======= `date` "

./../bin/game-of-life 40000 0.5 3 0 0&> $PBS_JOBNAME.log

echo "====test Run ends now ======= `date` "
