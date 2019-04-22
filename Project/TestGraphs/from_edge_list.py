###
### Create undirected (roadnet) graph from an edgelist file.
### We give capacities of ONE to every edge.
###   (But note, each undirected edge is technically 2 directed edges.)
###

import networkx as nx
import sys

if len(sys.argv) != 3:
    print("USAGE: provide input TSV edge list filepath, followed by output filepath.")

file_path = sys.argv[1]
output_file_path = sys.argv[2]

# We are just assuming undirected graphs, so technically the network
#   will have pairs of opposing directed edges.
G = nx.read_edgelist(file_path)

# Consider only the largest component
G = max(nx.connected_component_subgraphs(G), key=len)

G = nx.convert_node_labels_to_integers(G)

graph_file = open(output_file_path, "w")
n = nx.number_of_nodes(G)
m = nx.number_of_edges(G) * 2
graph_file.write(f"{n:d} {m:d}\n")

for u in range(n):
    graph_file.write(" ".join(str(v)+" "+str(1) for v in set(nx.all_neighbors(G,u)))+"\n")

graph_file.close()
