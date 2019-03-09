#!/bin/sh

#salloc --nodes 1 --time 15 --partition debug
#srun --partition debug --time 15 --ntasks 8 --overcommit ~/barn/leeh17_hw3.xl
#exit

#Run with: 
#sbatch --partition debug --nodes 1 --time 15 ~/barn/run-hw3.sh
#--mail-type and --mail-user values?
srun --ntasks 64 --overcommit -o ~/scratch/output64.log ~/barn/leeh17_hw3.xl

#Running batch of additional tests; full test requires more nodes
#sbatch --partition large --nodes 128 --time 150 ~/barn/run-hw3.sh
srun --ntasks 128 --overcommit -o ~/scratch/output128.log ~/barn/leeh17_hw3.xl
srun --ntasks 256 --overcommit -o ~/scratch/output256.log ~/barn/leeh17_hw3.xl
srun --ntasks 512 --overcommit -o ~/scratch/output512.log ~/barn/leeh17_hw3.xl
srun --ntasks 1024 --overcommit -o ~/scratch/output1024.log ~/barn/leeh17_hw3.xl
srun --ntasks 2048 --overcommit -o ~/scratch/output2048.log ~/barn/leeh17_hw3.xl
#srun --ntasks 4096 --overcommit -o ~/scratch/output4096.log ~/barn/leeh17_hw3.xl
#srun --ntasks 8192 --overcommit -o ~/scratch/output8192.log ~/barn/leeh17_hw3.xl


srun --ntasks 64 --overcommit -o ~/scratch/output64_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl
srun --ntasks 128 --overcommit -o ~/scratch/output128_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl
srun --ntasks 256 --overcommit -o ~/scratch/output256_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl
srun --ntasks 512 --overcommit -o ~/scratch/output512_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl
srun --ntasks 1024 --overcommit -o ~/scratch/output1024_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl
srun --ntasks 2048 --overcommit -o ~/scratch/output2048_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl
#srun --ntasks 4096 --overcommit -o ~/scratch/output4096_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl
#srun --ntasks 8192 --overcommit -o ~/scratch/output8192_mpiReduce.log ~/barn/leeh17_hw3_mpiReduce.xl

