#!/bin/sh
#SBATCH -D /gpfs/u/scratch/PCP8/PCP8lhrr/

#Pdf section 2.1 experiments with 4, 16, and 64 compute nodes for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition small --nodes 64 --time 30 barn/4-5_scripts/2-1-small_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/10000-3.adj 64 > $SCRATCH/16_rank_results/16r-small-3.log &
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/10000-6.adj 64 > $SCRATCH/16_rank_results/16r-small-6.log &
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/10000-12.adj 64 > $SCRATCH/16_rank_results/16r-small-12.log &
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/100000-3.adj 64 > $SCRATCH/16_rank_results/16r-medium-3.log
wait

srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/100000-6.adj 64 > $SCRATCH/16_rank_results/16r-medium-6.log &
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/100000-12.adj 64 > $SCRATCH/16_rank_results/16r-medium-12.log &
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/1000000-3.adj 64 > $SCRATCH/16_rank_results/16r-large-3.log &
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/1000000-6.adj 64 > $SCRATCH/16_rank_results/16r-large-6.log
wait

srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/1000000-12.adj 64 > $SCRATCH/16_rank_results/16r-large-12.log
