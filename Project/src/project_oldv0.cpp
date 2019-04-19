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

// map<int, vertex *> graph; //<id, vertex>, vertex holds edges

struct edge {
  int from_id;
  int to_id;
  int capacity;
};
using graph = map<int, vector<edge>>;
// typedef map<int, vector<edge>> graph;
graph board; // = map<int, vector<edge>>;
int graph_size;

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
  for (int i = 0; i < graph_size; i++) {
    board[i] = new vector<edge>();
    // vertex *new_vertex = new vertex(i);
    // graph[i] = new_vertex;
    // test[i] = i;
    // graph[i].id = i;
    // graph[i].out_edges = new vector();
    // graph[i].in_edges = new vector();
    // graph[i] = {i, new vector<edge>(), new vector<edge>()};
  }

  // Read in all the edges
  int to_id, from_id, capacity;
  int read = fscanf(file, "%d %d %d\n", &to_id, &from_id, &capacity);
  while (read != EOF) {
    // if(graph[from_id] != s)
    // edge new_edge = make_pair(capacity, from_id);

    // vertex *from_vert = graph.find(from_id)->second;
    // from_vert->in_edges.push_back(new_edge);

    read = fscanf(file, "%d %d %d\n", &to_id, &from_id, &capacity);
  }
  fclose(file);
}

// For now going to assume all ranks will load the entire graph

int main(int argc, char **argv) { return -1; }