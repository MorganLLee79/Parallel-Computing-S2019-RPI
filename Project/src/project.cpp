/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

#include "zoltan.h"
#include <sstream>
#include <vector>

using namespace std;

/************MPI Variables *********************/
int mpi_rank;
int mpi_size;
/************Zoltan Library Variables **********/
struct Zoltan_Struct *zz;
float version;
/***********************************************/

struct in_edge {
  unsigned long long source_node_id;
  int rank_location;
};

struct out_edge {
  unsigned long long destination_node_id;
  int rank_location;
  int flow;
  int capacity;
};

struct label {
  unsigned long long previous_node; // Should be a global ID
  int value;
};

struct vertex {
  unsigned long long id = -1; // Should match index

  // Lists of the edges, which are pairs of capacities and vertex IDs
  vector<struct out_edge> out_edges;
  vector<struct in_edge> in_edges;
};

// Something to store vertices
// int local_vertex_count; replaced with vector.size()
vector<vertex> network;
unsigned long long vertex_id_to_ranks[];
// Indices represent vertex IDs, values represent the rank they're on

// struct network {
//   vertex vertices[32]; // Temp maximum
//   int n;               // Number vertices held
//   int m;               // Number edges held
// };

// network local_network;

/********** Graph Management Functions ************/

// Take the two network IDs, add an edge between them.
// Assuming this rank has both nodes
// Sets flow = 0
void add_edge(unsigned long long start_id, unsigned long long end_id,
              int edge_capacity) {
  // Initialize the two edges, fill them
  out_edge new_out_edge;
  new_out_edge.destination_node_id = end_id;
  // new_out_edge.rank_location = !!!export_processors[end_id];
  new_out_edge.rank_location = -1;
  new_out_edge.flow = 0; // Default value
  new_out_edge.capacity = edge_capacity;

  in_edge new_in_edge;
  new_in_edge.source_node_id = start_id;
  new_in_edge.rank_location = -1;
  // new_in_edge.rank_location = export_processors[start_id];

  // Add the two new edges to their respective vertices
  // Just adding to the end; shouldn't matter?
  network[start_id].out_edges.push_back(new_out_edge);
  network[end_id].in_edges.push_back(new_in_edge);
  // network[end_id].in_edges.push_back(new_in_edge);
}

/*********** Zoltan Query Functions ***************/

// query function, returns the number of objects assigned to the processor
int user_return_num_obj(void *data, int *ierr) {

  printf("RAN num obj\n");
  // int *result = (int *)data;
  // *result = network.size();

  *ierr = ZOLTAN_OK;
  return network.size();
}

// https://cs.sandia.gov/Zoltan/ug_html/ug_query_lb.html#ZOLTAN_OBJ_LIST_FN
void user_return_obj_list(void *data, int num_gid_entries, int num_lid_entries,
                          ZOLTAN_ID_PTR global_ids, ZOLTAN_ID_PTR local_ids,
                          int wgt_dim, float *obj_wgts, int *ierr) {
  printf("RAN obj list\n");
  int i;
  for (i = 0; i < network.size(); ++i) {
    global_ids[i * num_gid_entries] = network[i].id;
    local_ids[i * num_lid_entries] = i;
  }

  *ierr = ZOLTAN_OK;
}

/////////// Zoltan Graph Query Functions

// Return the number of edges in the given vertex
int user_num_edges(void *data, int num_gid_entries, int num_lid_entries,
                   ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id, int *ierr) {
  printf("RAN num edges on node %d, size of %d+%d=%d\n", local_id[0],
         network[local_id[0]].in_edges.size(),
         network[local_id[0]].out_edges.size(),
         (network[local_id[0]].in_edges.size() +
          network[local_id[0]].out_edges.size()));

  *ierr = ZOLTAN_OK;

  return (network[local_id[0]].in_edges.size() +
          network[local_id[0]].out_edges.size());
}

// Return list of global ids, processor ids for nodes sharing an edge with the
// given object
void user_return_edge_list(void *data, int num_gid_entries, int num_lid_entries,
                           ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
                           ZOLTAN_ID_PTR nbor_global_id, int *nbor_procs,
                           int wgt_dim, float *ewgts, int *ierr) {
  // printf("-------%d, %d-%d; g:%d,l:%d\n", network.size(), num_gid_entries,
  //        num_lid_entries, *global_id, *local_id);
  vertex curr_vertex = network[(unsigned long long)*local_id];

  // printf("step 1\n");
  // go through all neighboring edges. in then out edges
  for (int i = 0; i < curr_vertex.in_edges.size(); ++i) {
    nbor_global_id[i] = curr_vertex.in_edges[i].source_node_id;
    nbor_procs[i] = curr_vertex.in_edges[i].rank_location;
  }
  // printf("step 2\n");

  int offset = curr_vertex.in_edges.size();
  for (int i = 0; i < curr_vertex.out_edges.size(); ++i) {
    nbor_global_id[i + offset] = curr_vertex.out_edges[i].destination_node_id;
    nbor_procs[i + offset] = curr_vertex.out_edges[i].rank_location;
  }
  // printf("step 3\n");

  *ierr = ZOLTAN_OK;
}

/////////// Zoltan Migration Functions:
// Copy all needed data for a single object into a communication buffer

// Return the byte size of the object
int user_return_obj_size(void *data, int num_global_ids,
                         ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
                         int *ierr) {
  // vertex *input_vertex = (vertex *)data;

  // Need to get the size for the given object, as not all of them are the same
  // size due to vectors

  // sizeof(MyVector) + (sizeof(MyVector[0]) * MyVector.size())
  /*
    return sizeof(vertex) * 2 + sizeof(unsigned long long) +
           sizeof(in_edge) * network[(unsigned long)*local_id].in_edges.size() +
           sizeof(out_edge) * network[(unsigned
    long)*local_id].out_edges.size();
  */
  // return sizeof(struct vertex);
  *ierr = ZOLTAN_OK;
  printf(
      "r%d: Got size of gid:%d lid:%d: in size:%d, %d; out size: %d, %d\n",
      mpi_rank, global_id, *local_id,
      sizeof(network[(unsigned long)*local_id].in_edges),
      network[(unsigned long)*local_id].in_edges.size(),
      (sizeof(network[(unsigned long)*local_id]) +
       sizeof(in_edge) * network[(unsigned long)*local_id].in_edges.size() +
       sizeof(out_edge) * network[(unsigned long)*local_id].out_edges.size()),
      network[(unsigned long)*local_id].out_edges.size());

  return (sizeof(network[(unsigned long)*local_id]) +
          sizeof(in_edge) * network[(unsigned long)*local_id].in_edges.size() +
          sizeof(out_edge) *
              network[(unsigned long)*local_id].out_edges.size());
}

void user_pack(void *data, int num_global_ids, int num_local_ids,
               ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id, int destination,
               int size, char *buf, int *ierr) {
  // Cast data to vertex pointer type and create the buffer
  vertex *vertex_buffer = (vertex *)buf;

  // Fill the buffer with the correct information
  vertex source_vertex = network[global_id[0]];
  vertex_buffer->id = source_vertex.id;
  // sprintf(buf, "%llu", buf, source_vertex.id);
  printf("r%d: RAN pack, gid:%d,lid:%d, id=%d. sending to rank %d\n", mpi_rank,
         global_id[0], local_id[0], -1, destination);

  // Concatenate lengths of edge lists
  memcpy(buf + sizeof(buf), source_vertex.out_edges.size(),
         sizeof(source_vertex.out_edges.size()));
  memcpy(buf + sizeof(buf), source_vertex.out_edges.size(),
         sizeof(source_vertex.out_edges.size()));

  // Concatenate raw binary from out and in edges
  memcpy(buf + sizeof(buf), source_vertex.out_edges.data(),
         sizeof(source_vertex.out_edges.data()));
  memcpy(buf + sizeof(buf), source_vertex.in_edges.data(),
         sizeof(source_vertex.in_edges.data()));
  // ostringstream ss;
  // ss << buf;
  // // sprintf(buf, "%s %d", buf, source_vertex.out_edges.size());
  // // Go through all the edges and add them to the buffer, and their counts
  // ss << " " << 2; // source_vertex.out_edges.size();
  // for (int i = 0; i < source_vertex.out_edges.size(); i++) {
  //   out_edge new_out_edge;
  //   new_out_edge.destination_node_id =
  //       source_vertex.out_edges[i].destination_node_id;
  //   new_out_edge.rank_location = source_vertex.out_edges[i].rank_location;
  //   new_out_edge.flow = source_vertex.out_edges[i].flow;
  //   new_out_edge.capacity = source_vertex.out_edges[i].capacity;
  //   // vertex_buffer->out_edges.push_back(new_out_edge);

  //   // Also add to the buffer new_in_edge's value
  //   // buf = buf + (char *)new_out_edge;
  //   // sprintf(buf, "%s %d %d %d %d", buf, new_out_edge.destination_node_id,
  //   //         new_out_edge.rank_location, new_out_edge.flow,
  //   //         new_out_edge.capacity);
  //   // ss << " " << new_out_edge.destination_node_id << " "
  //   //    << new_out_edge.rank_location << " " << new_out_edge.flow << " "
  //   //    << new_out_edge.capacity;
  // }

  // Separate for loops for when not same sizes/degrees in/out
  // sprintf(buf, "%s %d", buf, source_vertex.in_edges.size());
  // ss << " " << 4; // source_vertex.in_edges.size();
  // for (int i = 0; i < source_vertex.in_edges.size(); i++) {
  //   in_edge new_in_edge;
  //   new_in_edge.source_node_id = source_vertex.in_edges[i].source_node_id;
  //   new_in_edge.rank_location = source_vertex.in_edges[i].rank_location;
  //   // vertex_buffer->in_edges.push_back(new_in_edge);

  //   // Also add to the buffer new_in_edge's value
  //   // strcat(buf, new_in_edge);
  //   // sprintf(buf, "%s %d %d", buf, new_in_edge.source_node_id,
  //   //         new_in_edge.rank_location);
  //   // ss << " " << new_in_edge.source_node_id << " " <<
  //   // new_in_edge.rank_location;
  // }
  // buf = (char *)ss.str().c_str();

  // vertex_buffer->in_edges = new
  // vector<in_edge>(network[local_id[0]].in_edges); vertex_buffer->out_edges =
  //     new vector<out_edge>(network[local_id[0]].out_edges);
  // buf = (char *)ss.str().c_str();
  // printf("%s\n", ss.str());

  *ierr = ZOLTAN_OK;
}

// Copy all needed data for a single object into the application data structure
void user_unpack(void *data, int num_global_ids, ZOLTAN_ID_PTR global_id,
                 int size, char *buf, int *ierr) {
  // Cast data to vertex pointer type
  // char *raw_vertex;
  // sscanf(buf, "%s ", raw_vertex);
  // vertex *vertex_buffer = (vertex *)raw_vertex;

  // vertex *vertex_buffer = (vertex *)buf;

  // vertex new_vertex;
  // new_vertex.id = /*vertex_buffer->id;*/ (unsigned long long)*global_id;
  // // sscanf(buf, "%llu ", &new_vertex.id);
  vertex new_vertex;
  memcpy(&new_vertex, buf, sizeof(vertex));

  printf("r%d: RAN unpack, node id:%d, gid:%d,%d; %d in, %d out.\n", mpi_rank,
         new_vertex.id, *global_id, num_global_ids, new_vertex.in_edges.size(),
         new_vertex.out_edges.size());

  // Need to pass in/out_edges, based on communication buffer in user_pack
  // Go through all the edges and add them. Vector's copy constructor failed
  int out_count;
  // for (int i = 0; i < out_count; i++) {
  //   out_edge new_out_edge;
  //   new_out_edge.destination_node_id =
  //       vertex_buffer->out_edges[i].destination_node_id;
  //   new_out_edge.rank_location = vertex_buffer->out_edges[i].rank_location;
  //   new_out_edge.flow = vertex_buffer->out_edges[i].flow;
  //   new_out_edge.capacity = vertex_buffer->out_edges[i].capacity;
  //   new_vertex.out_edges.push_back(new_out_edge);

  //   printf("--%d, edge to %d\n", new_vertex.id,
  //          new_vertex.out_edges.back().destination_node_id);
  // }

  // Separate for loops for when not same sizes/degrees in/out
  int in_count;
  // ss >> temp;
  // strings
  // stream(temp) >> in_count;
  // ss >> in_count;

  // char *junk;
  // sscanf(buf, "%s %d", junk, &out_count);
  // buf += sizeof(junk) + 1 + sizeof(out_count);
  // sscanf(buf, "%d", &in_count);

  // sscanf(buf, "%*s %d %d", &out_count, &in_count);
  // sscanf(buf, "%*s %d %d", &out_count, &in_count);

  // for (int i = 0; i < in_count; i++) {
  //   in_edge new_in_edge;
  //   new_in_edge.source_node_id = vertex_buffer->in_edges[i].source_node_id;
  //   new_in_edge.rank_location = vertex_buffer->in_edges[i].rank_location;
  //   new_vertex.in_edges.push_back(new_in_edge);

  //   printf("--%d, edge from %d\n", new_vertex.id,
  //          new_vertex.in_edges.back().source_node_id);
  // }
  // istringstream ss(buf);

  // // stringstream test_ss("raw_vertex 9 8");
  // string junk;
  // ss >> junk;
  // ss >> out_count;
  // ss >> in_count;

  printf("r%d: vert %d out_count=%d, in_count=%d---------------------------\n",
         mpi_rank, new_vertex.id, out_count, in_count);

  network.push_back(new_vertex);

  *ierr = ZOLTAN_OK;
}

/********************************************/

int max_flow(int source_id, int sink_id) { return -1; }

// For now going to assume all ranks will load the entire graph
int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  // Zoltan Initialization
  Zoltan_Initialize(argc, argv, &version);
  zz = Zoltan_Create(MPI_COMM_WORLD);
  // zz = Zoltan_Create(MPI_COMM_WORLD);

  /* Register query functions */
  // zz->Set_Fn(ZOLTAN_NUM_)
  Zoltan_Set_Fn(zz, ZOLTAN_NUM_OBJ_FN_TYPE, (void (*)())user_return_num_obj,
                NULL);
  Zoltan_Set_Fn(zz, ZOLTAN_OBJ_LIST_FN_TYPE, (void (*)())user_return_obj_list,
                NULL);

  // Graph query functions
  Zoltan_Set_Fn(zz, ZOLTAN_NUM_EDGES_FN_TYPE, (void (*)())user_num_edges, NULL);
  Zoltan_Set_Fn(zz, ZOLTAN_EDGE_LIST_FN_TYPE, (void (*)())user_return_edge_list,
                NULL);

  // Pack/unpack for data migration
  Zoltan_Set_Fn(zz, ZOLTAN_OBJ_SIZE_FN_TYPE, (void (*)())user_return_obj_size,
                NULL);
  Zoltan_Set_Fn(zz, ZOLTAN_PACK_OBJ_FN_TYPE, (void (*)())user_pack, NULL);
  Zoltan_Set_Fn(zz, ZOLTAN_UNPACK_OBJ_FN_TYPE, (void (*)())user_unpack, NULL);
  // Zoltan_Set_Fn(zz, ...... _);

  /* Set Zoltan paramaters. */
  // Zoltan_Set_Param(zz, "DEBUG?", "4?");
  Zoltan_Set_Param(zz, "LB_METHOD", "GRAPH");
  Zoltan_Set_Param(zz, "AUTO_MIGRATE",
                   "TRUE"); // Might need user-guided for edges?
  Zoltan_Set_Param(zz, "RETURN_LISTS", "PARTS");

  // Initialize network
  int testing_limit = 0;
  if (mpi_rank == 0) {
    testing_limit = 8;
  }
  // testing_limit = 4;

  for (int i = 0; i < testing_limit; i++) {
    vertex new_vertex;
    new_vertex.id = (unsigned long long)i + (mpi_rank * testing_limit);
    network.push_back(new_vertex);
    test_net2[i] = i;
  }
  printf("r%d: Initial size=%lu\n", mpi_rank, network.size());
  for (int i = 0; i < network.size(); i++) {
    printf("r%d: network[%lu]=%lu; %d\n", mpi_rank, i, network[i].id,
           test_net2[i]);
  }
  if (mpi_rank == 0) {
    add_edge(4, 5, 3);
    // add_edge(4, 5, 3);
    // add_edge(4, 5, 3);
    // add_edge(4, 5, 3);
    // add_edge(4, 5, 3);
    // add_edge(4, 5, 3);
    // add_edge(4, 5, 3);
    // add_edge(4, 5, 3);
    // add_edge(0, 0, 5);
    // add_edge(1, 1, 6);
    // add_edge(2, 2, 9);
    // add_edge(3, 3, 8);
    // add_edge(4, 4, 1);
    // add_edge(5, 5, 2);
    // add_edge(6, 6, 4);
    // add_edge(7, 7, 3);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  // Basing on https://cs.sandia.gov/Zoltan/ug_html/ug_examples_lb.html
  int num_changes; // Set to 1/True if decomposition was changed
  int num_imported, num_exported, *import_processors, *export_processors;
  int *import_to_parts, *export_to_parts;
  int num_gid_entries, num_lid_entries;
  ZOLTAN_ID_PTR import_global_ids, export_global_ids;
  ZOLTAN_ID_PTR import_local_ids, export_local_ids;
  // Parameters essentially: global info(output), import info, export info
  // printf("r%d: Entering lb partition. n=%d\n", mpi_rank, network.size());
  Zoltan_LB_Partition(zz, &num_changes, &num_gid_entries, &num_lid_entries,
                      &num_imported, &import_global_ids, &import_local_ids,
                      &import_processors, &import_to_parts, &num_exported,
                      &export_global_ids, &export_local_ids, &export_processors,
                      &export_to_parts);
  // Also handles data migration as AUTO_MIGRATE was set to true

  MPI_Barrier(MPI_COMM_WORLD);
  /* Begin Algorithm */

  printf("r%d: imported %d, exported %d. num_changes=%d Final size=%lu; g/l id "
         "entries:%d, %d\n",
         mpi_rank, num_imported, num_exported, num_changes, network.size(),
         num_gid_entries, num_lid_entries);
  for (int i = 0; i < network.size(); i++) {

    if (num_exported == 0) {
      printf("r%d: network[%lu]=%lu\n", mpi_rank, i, network[i].id);
    } else {
      printf("r%d: network[%lu]=%lu; exported to rank %d\n", mpi_rank, i,
             network[i].id, export_processors[i]);
    }
  }

  // Process the map of where vertices went and remove exported vertices
  unsigned long long id_rank_map[network.size()];
  if (mpi_rank == 0) {
    vertex_id_to_ranks = export_processors;

    for (int i = network.size() - 1; i >= 0; i--) { // Iterate in reverse

      // Map all export_processors to id_rank_map
      // Wait, export_processors already it?

      // Remove from this rank if it was exported
      if (export_processors[i] != mpi_rank) {
        printf("r%d: removing exported network[%lu]=%lu. Was exported to %d\n",
               mpi_rank, i, network[i].id, export_processors[i]);
        network.erase(network.begin() + i);
      }
    }
  }
  int total_network_size = -1;
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&total_network_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  // Broadcast array of export_processors essentially.
  // Indices represent vertex IDs, values represent the rank they're on
  MPI_Bcast(vertex_id_to_ranks, total_network_size, MPI_INT, 0, MPI_COMM_WORLD);

  // Fill out edges' rank_location values everywhere using vertex_id_to_ranks
  for (int i = 0; i < network.size(); i++) {
    for (int j = 0; j < network[i].out_edges.size(); j++) {
      network[i].out_edges[j].rank_location =
          vertex_id_to_ranks[network[i].out_edges[j].destination_node_id];
    }
    for (int j = 0; j < network[i].in_edges.size(); j++) {
      network[i].in_edges[j].rank_location =
          vertex_id_to_ranks[network[i].in_edges[j].source_node_id];
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  /* Set up finished, can begin running */

  /*Begin closing/freeing things*/
  Zoltan_LB_Free_Part(&import_global_ids, &import_local_ids, &import_processors,
                      &import_to_parts);
  Zoltan_LB_Free_Part(&export_global_ids, &export_local_ids, &export_processors,
                      &export_to_parts);

  Zoltan_Destroy(&zz);
  MPI_Finalize();

  return -1;
}