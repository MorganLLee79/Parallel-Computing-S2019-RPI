#!/bin/sh
#SBATCH -D /gpfs/u/scratch/PCP8/PCP8lhrr/

#Pdf section 2.1 experiments with 4, 16, and 64 compute nodes for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition small --nodes 16 --time 30 barn/4-5_scripts/2-1-small_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr
EXECUTABLE=$SCRATCH/project.out

srun -N4 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/email-enron.adj 64 > $SCRATCH/4_rank_results/4r-email.log &
srun -N4 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/p2p-gnutella.adj 64 > $SCRATCH/4_rank_results/4r-p2p.log &
srun -N4 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/social-slashdot.adj 64 > $SCRATCH/4_rank_results/4r-social.log &
srun -N4 --ntasks-per-node=1 --overcommit $EXECUTABLE $SCRATCH/TestGraphs/web-google.adj 64 > $SCRATCH/4_rank_results/4r-webcrawl.log &
wait