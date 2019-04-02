#!/bin/sh

#Second experiments set in pdf section 2.1 for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition medium --nodes 128 --time 15 barn/4-5_scripts/2-1-medium_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr

#Clean up output files to ensure consistent running
rm $SCRATCH/2-1_64r-4t_io_output
rm $SCRATCH/2-1_16r4t_io_output
rm $SCRATCH/2-1_4r-16t_io_output
rm $SCRATCH/2-1_2r-32t_io_output
rm $SCRATCH/2-1_1r-64t_io_output

srun -N128 --ntasks-per-node=64 --overcommit -o $SCRATCH/2-1_128nodes-256ranks-1thread.log $BARN/assignment4-5 1 0.25 256 $SCRATCH/2-1_64r-4t_io_output
srun -N128 --ntasks-per-node=16 --overcommit -o $SCRATCH/2-1_128nodes-64ranks-4thread.log $BARN/assignment4-5 4 0.25 256 $SCRATCH/2-1_16r4t_io_output
srun -N128 --ntasks-per-node=4 --overcommit -o $SCRATCH/2-1_128nodes-16ranks-16thread.log $BARN/assignment4-5 16 0.25 256 $SCRATCH/2-1_4r-16t_io_output
srun -N128 --ntasks-per-node=2 --overcommit -o $SCRATCH/2-1_128nodes-8ranks-32thread.log $BARN/assignment4-5 32 0.25 256 $SCRATCH/2-1_2r-32t_io_output
srun -N128 --ntasks-per-node=1 --overcommit -o $SCRATCH/2-1_128nodes-4ranks-64thread.log $BARN/assignment4-5 64 0.25 256 $SCRATCH/2-1_1r-64t_io_output


#Outputting with i/o now