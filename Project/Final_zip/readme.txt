Parallel Network Flow Project Readme
Eric Johnson, Christopher Jones, Harrison Lee
26Apr2019
Rensselaer Polytechnic Institute: CSCI-6360/4320 Parallel Computing


Dependencies:
Zoltan
On Amos Blue Gene/Q run: module load xl; module load zoltan/xl


Locally running:
mpirun -np <number ranks> ./project.out <input graph> <number threads>
ex: mpirun -np 4 ./project.out /TestGraphs/small-3.adj

Generating graphs with create_random_network.py
python3 create_random_network.py <number nodes> <average degree> <maximum capacity> <output file name>
ex: python3 create_random_network.py 10000 3.001 10 TestGraphs/small-3.adj

Input graph format:
The first line is the number of nodes then number of edges.
Each following line represents a node, starting with node ID 0.
Each node line consists of it's out edges. These edges are pairs of destination node ID then capacity.