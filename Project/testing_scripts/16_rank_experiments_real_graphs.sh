#!/bin/sh
#SBATCH -D /gpfs/u/scratch/PCP8/PCP8lhrr/

#Pdf section 2.1 experiments with 4, 16, and 64 compute nodes for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition medium --nodes 128 --time 30 barn/4-5_scripts/2-1-small_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr
EXECUTABLE=$BARN/project.out
srun -N16 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/PA.adj 64 > $SCRATCH/16_rank_results/16r-PA.log &
srun -N16 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/TX.adj 64 > $SCRATCH/16_rank_results/16r-TX.log &
srun -N16 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/CA.adj 64 > $SCRATCH/16_rank_results/16r-CA.log &
srun -N16 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/social.adj 64 > $SCRATCH/16_rank_results/16r-social.log &
srun -N16 --ntasks-per-node=1 --overcommit EXECUTABLE $SCRATCH/TestGraphs/webcrawl.adj 64 > $SCRATCH/16_rank_results/16r-webcrawl.log
