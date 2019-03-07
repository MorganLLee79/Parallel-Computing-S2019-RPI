#!/bin/sh

#salloc --nodes 1 --time 15 --partition debug
#srun --partition debug --time 15 --ntasks 8 --overcommit ~/barn/leeh17_hw3.xl
#exit

#Run with: 
#sbatch --partition debug --nodes 1 --time 15 ~/barn/run-hw3.sh
srun --ntasks 64 --overcommit -o ~/scratch/output.log ~/barn/leeh17_hw3.xl