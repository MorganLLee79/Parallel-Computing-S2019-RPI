/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

// Making initial source code for now

#include <stdio.h>
#include <stdlib.h>

using namespace std;

typedef pair<unsigned long long, int>
    edge; // Node ID, foreign mpi rank stored at
typedef ? pair ? <unsigned long long, int> label; // Node ID, +/- value

struct vertex {
  unsigned long long id = -1; // Should match index

  // Lists of the edges, which are pairs of capacities and vertex IDs
  // vector<edge> out_edges;
  // vector<edge> in_edges;
  edge[] out_edges;
  edge[] in_edges;

  // Match indices with out_edges
  int[] flows;
  int[] capacities;
};

int max_flow(int source_id, int sink_id) { return -1; }

// For now going to assume all ranks will load the entire graph
int main(int argc, char **argv) { return -1; }