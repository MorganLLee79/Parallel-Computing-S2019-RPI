/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

// Making initial source code for now

//#include <graph.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

typedef pair<int, int> edge;
struct vertex {
  int id = -1; // Should match index

  // Lists of the edges, which are pairs of capacities and vertex IDs
  vector<edge> out_edges;
  vector<edge> in_edges;

  // vertex(int new_id) : id(new_id) {}
};

map<int, vertex *> graph; //<id, vertex>, vertex holds edges
int graph_size;

// Hold the queued nodes to read
// BFS: highest priority = lowest distance from source
// Send w/ .pop(), add with queue.push(make_pair(distance, vert_id));
// Define with vector<int> and greater<int> to define container (default) and
// reverse order? Or just save negative values for distances

// priority_queue<pair<int, int>, vector<int>, greater<int>> queue;
priority_queue<pair<int, int>> queue;

/*
 * Follow the file path and read in the edge list graph there
 * Reading from disk and loading into compute node's memory
 * TODO: should either be given graph size or have it as first file line
 */
void read_file(char *file_path, int input_graph_size) {
  FILE *file;
  file = fopen(file_path, "r");
  if (file == NULL) {
    printf("ERROR: Couldn't find file at %s\n", file_path);
  }

  graph_size = input_graph_size;

  // graph = (vertex *)calloc(graph_size, sizeof *graph);

  // Initialize all vertices with their IDs
  // for (int i = 0; i < graph_size; i++) {
  //   vertex *new_vertex = new vertex(i);
  //   // graph[i] = new_vertex;
  //   // test[i] = i;
  //   // graph[i].id = i;
  //   // graph[i].out_edges = new vector();
  //   // graph[i].in_edges = new vector();
  //   // graph[i] = {i, new vector<edge>(), new vector<edge>()};
  // }

  // Read in all the edges
  int to_id, from_id, capacity;
  int read = fscanf(file, "%d %d %d\n", &to_id, &from_id, &capacity);
  while (read != EOF) {
    edge new_edge = make_pair(capacity, from_id);

    // This vertex wasn't initialized yet, initialize it now
    if (graph[from_id]->id == -1) {
      graph[from_id]->id = from_id;
      // graph[from_id]->in_edges = new vector<edge>();
      // graph[from_id]->out_edges = new vector<edge>();
    }

    graph[from_id]->out_edges.push_back(new_edge);
    // vertex *from_vert = graph[from_id]->second;
    // from_vert->in_edges.push_back(new_edge);

    read = fscanf(file, "%d %d %d\n", &to_id, &from_id, &capacity);
  }
  fclose(file);
}

// Over the global graph, find the maximum flow
// Return: the network's maximum flow
int max_flow(int source_id, int sink_id) {

  // Store all flows? TODO. center rank managing? when sending back value to
  // root, set to calculated value.
  // To cover f(u, v) = x

  // Default all edges to flow = 0

  // Add initial vertices adjacent to source to queue

  // Iterate over queue until all paths tested
  // TODO: potential issue with max queue size? m * size pair<int,int>
  while (!queue.empty()) {
    // Lock each vertex access?

    // Push a maximum flow to the vertex

    // Go over each edge, add capacity of each edge

    // Add to queue the next vertices, with distance/priorty += 1
  }

  return -1;
}

// For now going to assume all ranks will load the entire graph
int main(int argc, char **argv) { return -1; }