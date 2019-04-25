#!/bin/sh
#SBATCH -D /gpfs/u/scratch/PCP8/PCP8lhrr/

#Pdf section 2.1 experiments with 4, 16, and 64 compute nodes for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition small --nodes 32 --time 30 barn/4-5_scripts/2-1-small_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr

srun -N4 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/PA.adj 64 > $SCRATCH/4_rank_results/4r-PA.log &
srun -N4 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/TX.adj 64 > $SCRATCH/4_rank_results/4r-TX.log &
srun -N4 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/CA.adj 64 > $SCRATCH/4_rank_results/4r-CA.log &
srun -N4 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/social.adj 64 > $SCRATCH/4_rank_results/4r-social.log &
srun -N4 --ntasks-per-node=1 --overcommit -o $SCRATCH/project $SCRATCH/TestGraphs/webcrawl.adj 64 > $SCRATCH/4_rank_results/4r-webcrawl.log
