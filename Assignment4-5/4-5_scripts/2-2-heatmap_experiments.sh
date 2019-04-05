#!/bin/sh

#Heat map experiments set in pdf section 2.2 for HW 4-5
#sbatch --partition medium --nodes 128 --time 30 barn/4-5_scripts/2-2-heatmap_experiments.sh
BARN=/gpfs/u/barn/PCP8/PCP8lhrr
SCRATCH=/gpfs/u/scratch/PCP8/PCP8lhrr

#Clean up output files to ensure consistent running
# rm $SCRATCH/2-1_64r-4t_io_output
# rm $SCRATCH/2-1_16r4t_io_output
# rm $SCRATCH/2-1_4r-16t_io_output
# rm $SCRATCH/2-1_2r-32t_io_output
# rm $SCRATCH/2-1_1r-64t_io_output

#128 nodes 32 tasks 2 threas worked best
srun -N128 --ntasks-per-node=32 --overcommit -o $SCRATCH/2-2_heatmap-0thresh.log $BARN/assignment4-5 2 0 128 --heatmap $SCRATCH/2-2_heatmap-0thresh

srun -N128 --ntasks-per-node=32 --overcommit -o $SCRATCH/2-2_heatmap-25thresh.log $BARN/assignment4-5 2 0.25 128 --heatmap $SCRATCH/2-2_heatmap-25thresh

srun -N128 --ntasks-per-node=32 --overcommit -o $SCRATCH/2-2_heatmap-50thresh.log $BARN/assignment4-5 2 0.5 128 --heatmap $SCRATCH/2-2_heatmap-50thresh

srun -N128 --ntasks-per-node=32 --overcommit -o $SCRATCH/2-2_heatmap-75thresh.log $BARN/assignment4-5 2 0.75 128 --heatmap $SCRATCH/2-2_heatmap-75thresh


#Outputting with i/o now