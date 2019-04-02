#!/bin/sh
#SBATCH -D /gpfs/u/scratch/PCP8/PCP8lhrr/

#Pdf section 2.1 experiments with 4, 16, and 64 compute nodes for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition small --nodes 64 --time 30 barn/4-5_scripts/2-1-small_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr
srun -N4 --ntasks-per-node=64 --overcommit -o $SCRATCH/2-1_4nodes-256ranks-1thread.log $BARN/assignment4-5 1 0.25 256 &
srun -N4 --ntasks-per-node=16 --overcommit -o $SCRATCH/2-1_4nodes-64ranks-4thread.log $BARN/assignment4-5 4 0.25 256 &
srun -N4 --ntasks-per-node=4 --overcommit -o $SCRATCH/2-1_4nodes-16ranks-16thread.log $BARN/assignment4-5 16 0.25 256 &
srun -N4 --ntasks-per-node=2 --overcommit -o $SCRATCH/2-1_4nodes-8ranks-32thread.log $BARN/assignment4-5 32 0.25 256 &
srun -N4 --ntasks-per-node=1 --overcommit -o $SCRATCH/2-1_4nodes-4ranks-64thread.log $BARN/assignment4-5 64 0.25 256 &

#16 Compute nodes
srun -N16 --ntasks-per-node=64 --overcommit -o $SCRATCH/2-1_16nodes-256ranks-1thread.log $BARN/assignment4-5 1 0.25 256 &
srun -N16 --ntasks-per-node=16 --overcommit -o $SCRATCH/2-1_16nodes-64ranks-4thread.log $BARN/assignment4-5 4 0.25 256 &
wait

srun -N16 --ntasks-per-node=4 --overcommit -o $SCRATCH/2-1_16nodes-16ranks-16thread.log $BARN/assignment4-5 16 0.25 256 &
srun -N16 --ntasks-per-node=2 --overcommit -o $SCRATCH/2-1_16nodes-8ranks-32thread.log $BARN/assignment4-5 32 0.25 256 &
srun -N16 --ntasks-per-node=1 --overcommit -o $SCRATCH/2-1_16nodes-4ranks-64thread.log $BARN/assignment4-5 64 0.25 256 &
wait

#64 Compute nodes
srun -N64 --ntasks-per-node=64 --overcommit -o $SCRATCH/2-1_64nodes_256ranks-1thread.log $BARN/assignment4-5 1 0.25 256
srun -N64 --ntasks-per-node=16 --overcommit -o $SCRATCH/2-1_64nodes_64ranks-4thread.log $BARN/assignment4-5 4 0.25 256
srun -N64 --ntasks-per-node=4 --overcommit -o $SCRATCH/2-1_64nodes_16ranks-16thread.log $BARN/assignment4-5 16 0.25 256
srun -N64 --ntasks-per-node=2 --overcommit -o $SCRATCH/2-1_64nodes_8ranks-32thread.log $BARN/assignment4-5 32 0.25 256
srun -N64 --ntasks-per-node=1 --overcommit -o $SCRATCH/2-1_64nodes_4ranks-64thread.log $BARN/assignment4-5 64 0.25 256