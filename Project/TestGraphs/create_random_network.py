###
### This script creates a graph file for a roughly random graph.
###
### The first line of the output file is
###   the number of vertices and number of directed edges.
### Vertex IDs are ZERO-indexed.
### Each line of the file corresponds to a vertex:
###   Each pair of values (v,c) is an outgoing edge to vertex
###     v with a capacity of c.
###
### We assign capacities to edges uniformly randomly from a
###   range [1,C]
###
### The graph is not completely random because we make sure
###   the graph is weakly connected, and that there is a 
###   feasible flow.
###
### The Source vertex is ID=0
### The Sink vertex is ID=n-1
###
### The name of the graph file is
###   <n>-<m>-<C>-<average out degree>.adj
###

import numpy as np
import sys

if len(sys.argv) != 4:
    print("USAGE: provide 3 parameters: number of vertices, average out degree, capacity limit")
    exit(0)


np.random.seed(914736)

n = int(sys.argv[1])
avg_out_deg = float(sys.argv[2])
# Randpm capacities generated in range [1,C]
C = int(sys.argv[3])

# roughly 2*n edges will be added later, 
#   so limit how many random edges we make now
m = int(n * avg_out_deg) - 2*n + 2

np_edges = np.random.randint( 0, n, (m, 2) )

# Remove self-loops
edges = []
for edge in np_edges:
    if edge[0] != edge[1]:
        edges.append((edge[0], edge[1]))

# For feasibility, we add at most the following 2*(n-1) edges:
#
# Have every vertex but the first be the dest of at least one edge,
#  where the source is of a lower ID/index
for v in range(1, n):
    u = np.random.randint(0, v)
    edges.append((u,v))
#
# Have every vertex but the last be the src of at least one edge
for u in range(0, n-1):
    v = u 
    while v == u:
        v = np.random.randint(0, n)
    edges.append((u,v))

# Remove duplicate edges
edges = set(edges)

m = len(edges)
avg_out_deg = float(m)/float(n)
print(f"\nm = {m:d}")
print(f"average out degree = {avg_out_deg:f}")

# Construct adjacency list
adj_list = [[] for v in range(n)]
for edge in edges:
    adj_list[edge[0]].append(edge[1])

# Create the graph file
graph_file = open(f"random-{n:d}-{m:d}-{C:d}-{avg_out_deg:f}.adj", "w")
graph_file.write(f"{n:d} {m:d}\n")
for u in range(n):
    graph_file.write(" ".join(str(v)+" "+str(np.random.randint(1,C+1)) for v in adj_list[u]) + "\n")

graph_file.close()
