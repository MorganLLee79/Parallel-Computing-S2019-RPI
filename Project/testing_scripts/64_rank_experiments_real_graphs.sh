#!/bin/sh
#SBATCH -D /gpfs/u/scratch/PCP8/PCP8lhrr/

#Pdf section 2.1 experiments with 4, 16, and 64 compute nodes for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition medium --nodes 128 --time 30 barn/4-5_scripts/2-1-small_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr
EXECUTABLE=$SCRATCH/project.out
srun -N64 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/roadnet-PA.adj 64 > $SCRATCH/64_rank_results/64r-PA.log &
srun -N64 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/roadnet-TX.adj 64 > $SCRATCH/64_rank_results/64r-TX.log &
wait

srun -N64 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/roadnet-CA.adj 64 > $SCRATCH/64_rank_results/64r-CA.log &
srun -N64 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/social-slashdot.adj 64 > $SCRATCH/64_rank_results/64r-social.log &
wait

srun -N64 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/web-google.adj 64 > $SCRATCH/64_rank_results/64r-webcrawl.log
