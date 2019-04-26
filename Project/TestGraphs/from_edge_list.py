###
### Create directed graphs from an edgelist file.
### We give capacities of ONE to every edge.
###

import networkx as nx
import numpy as np
import sys

if len(sys.argv) != 3:
    print("USAGE: provide input TSV edge list filepath, followed by output filepath.")

file_path = sys.argv[1]
output_file_path = sys.argv[2]

# Create a directed graph from the edgelist file
G = nx.read_edgelist(file_path, create_using=nx.DiGraph())

# Remove self-loops
G.remove_edges_from(G.selfloop_edges())

# Consider only the largest component
G = max(nx.strongly_connected_component_subgraphs(G), key=len)

G = nx.convert_node_labels_to_integers(G)

graph_file = open(output_file_path, "w")
n = nx.number_of_nodes(G)
m = nx.number_of_edges(G)
graph_file.write(f"{n:d} {m:d}\n")

for u in range(n):
    # Add random capacity in the range [1,10]
    graph_file.write(" ".join(str(v)+" "+str(np.random.randint(1,11)) for v in set(nx.all_neighbors(G,u)))+"\n")

graph_file.close()
