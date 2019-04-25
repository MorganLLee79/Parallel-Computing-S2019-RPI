#!/bin/sh
#SBATCH -D /gpfs/u/scratch/PCP8/PCP8lhrr/

#Pdf section 2.1 experiments with 4, 16, and 64 compute nodes for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition medium --nodes 128 --time 30 scratch/.../128_rank_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr
EXECUTABLE=$BARN/project.out
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/10000-3.adj 64 > $SCRATCH/128_rank_results/128r-small-3.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/10000-6.adj 64 > $SCRATCH/128_rank_results/128r-small-6.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/10000-12.adj 64 > $SCRATCH/128_rank_results/128r-small-12.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/100000-3.adj 64 > $SCRATCH/128_rank_results/128r-medium-3.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/100000-6.adj 64 > $SCRATCH/128_rank_results/128r-medium-6.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/100000-12.adj 64 > $SCRATCH/128_rank_results/128r-medium-12.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/1000000-3.adj 64 > $SCRATCH/128_rank_results/128r-large-3.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/1000000-6.adj 64 > $SCRATCH/128_rank_results/128r-large-6.log
srun -N128 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/1000000-12.adj 64 > $SCRATCH/128_rank_results/128r-large-12.log
