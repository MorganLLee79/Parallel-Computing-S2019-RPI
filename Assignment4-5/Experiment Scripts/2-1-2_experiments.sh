
#Second experiments set in pdf section 2.1 for HW 4-5
#4 compute nodes, 25% threshold, 256 ticks, standard 32768x32768 cell universe
#sbatch --partition small --nodes 4 --time 15 ~/barn/2-1-1_experiments.sh
srun -N1 --ntasks-per-node=64 --overcommit -o ~/scratch/2-1-1_256ranks-1thread.log ~/barn/assignment4-5.xl 1 0.25 256
srun -N1 --ntasks-per-node=16 --overcommit -o ~/scratch/2-1-1_64ranks-4thread.log ~/barn/assignment4-5.xl 4 0.25 256
srun -N1 --ntasks-per-node=4 --overcommit -o ~/scratch/2-1-1_16ranks-16thread.log ~/barn/assignment4-5.xl 16 0.25 256
srun -N1 --ntasks-per-node=2 --overcommit -o ~/scratch/2-1-1_8ranks-32thread.log ~/barn/assignment4-5.xl 32 0.25 256
srun -N1 --ntasks-per-node=1 --overcommit -o ~/scratch/2-1-1_4ranks-64thread.log ~/barn/assignment4-5.xl 64 0.25 256


#Outputting with i/o now