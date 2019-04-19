/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

#include <vector>

using namespace std;

struct in_edge {
  unsigned long long destination_node_id;
  int rank_location;
};

struct out_edge {
  unsigned long long destination_node_id;
  int rank_location;
  int flow;
  int capacity;
};

struct label {
  unsigned long long previous_node;
  int value;
};

struct vertex {
  unsigned long long id = -1; // Should match index

  // Lists of the edges, which are pairs of capacities and vertex IDs
  vector<struct out_edge> out_edges;
  vector<struct in_edge> in_edges;
};

int max_flow(int source_id, int sink_id) { return -1; }

// For now going to assume all ranks will load the entire graph
int main(int argc, char **argv) { return -1; }