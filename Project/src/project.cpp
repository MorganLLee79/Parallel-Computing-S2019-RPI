/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

#include "zoltan.h"
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

// struct network {
//   vertex vertices[32]; // Temp maximum
//   int n;               // Number vertices held
//   int m;               // Number edges held
// };

// network local_network;

/*********** Zoltan Query Functions ***************/

// query function, returns the number of objects assigned to the processor
void user_return_num_obj(void *data, int *ierr) {
  int *result = (int *)data;

  *result = network.size();

  *ierr = ZOLTAN_OK;
}

// https://cs.sandia.gov/Zoltan/ug_html/ug_query_lb.html#ZOLTAN_OBJ_LIST_FN
void user_return_obj_list(void *data, int num_gid_entries, int num_lid_entries,
                          ZOLTAN_ID_PTR global_ids, ZOLTAN_ID_PTR local_ids,
                          int wgt_dim, float *obj_wgts, int *ierr) {
  // wgt_dim and obj_wgts are unused, left as default values
  /*
    int *result =
        (int *)data; // TODO set to full data set that is being split up (?)

    // Each global ID is a unsigned long long (2*sizeof(int)), local are ints
    // (array indices)
    if (num_gid_entries != 2 TODO verify || num_lid_entries != 1) {
      *ierr = ZOLTAN_FATAL;
      return;
    }

    // ZOLTAN_ID_PTRs are arrays of the partitioned IDs, assigned objects

    // Use LB_Partition to allocate; call once, so after ran it?

    // Global IDs are vertex IDs, local IDs are array indices
    // TODO check, update all edges to have correct allocated ranks in them

    *result = local_vertex_count; */

  int i;
  for (i = 0; i < network.size(); ++i) {
    global_ids[i * num_gid_entries] = network[i].id;
    local_ids[i * num_lid_entries] = i;
  }

  *ierr = ZOLTAN_OK;
}

// // Copied from PDF, temporary
// /* Return coordinates for objects requested by Zoltan in globalIDs array. */
// void user_geom_multi_fn(void *data, int nge, int nle, int numObj,
//                         ZOLTAN_ID_PTR globalIDs, ZOLTAN_ID_PTR localIDs,
//                         int dim, double *geomVec, int *err) {
//   /* Cast data pointer provided in Zoltan_Set_Fn to application data type. */
//   /* Application data is an array of Particle structures. */
//   struct Particle *user_particles = (struct Particle *)data; // Source data

//   /* Assume for this example that each globalID and localID is one integer.
//   */
//   /* Each globalID is a global particle number; each localID is an index */
//    into the user’s array of Particles.
//   if (nge != 1 || nle != 1) {
//     *err = ZOLTAN_FATAL;
//     return;
//   }
//   /* Loop over objects for which coordinates are requested */
//   int i, j = 0;
//   for (i = 0; i < numObj; i++) { // Iterate over localIDs, things we have
//     /* Copy the coordinates for the object globalID[i] (with localID[i]) */
//     /* into the geomVec vector. Note that Zoltan allocates geomVec. */
//     // geomVec being set: output?; using data, data=input?
//     geomVec[j++] = user_particles[localIDs[i]].x;
//     if (dim > 1)
//       geomVec[j++] = user_particles[localIDs[i]].y;
//     if (dim > 2)
//       geomVec[j++] = user_particles[localIDs[i]].z;
//   }
//   *err = ZOLTAN_OK;
// }

/////////// Zoltan Migration Functions:
// Copy all needed data for a single object into a communication buffer

// Return the byte size of the object
int user_return_obj_size(void *data, int num_global_ids,
                         ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
                         int *ierr) {
  printf("TEST------\n");
  vertex *input_vertex = (vertex *)data;

  // Need to get the size for the given object, as not all of them are the same
  // size due to vectors

  // sizeof(MyVector) + (sizeof(MyVector[0]) * MyVector.size())

  return sizeof(vertex) + sizeof(in_edge) * input_vertex->in_edges.size() +
         sizeof(out_edge) * input_vertex->out_edges.size();
  *ierr = ZOLTAN_OK;
}

void user_pack(void *data, int num_global_ids, int num_local_ids,
               ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id, int destination,
               int size, char *buf, int *ierr) {
  printf("TEST------\n");
  // Cast data to vertex pointer type and create the buffer
  vertex *vertex_buffer = (vertex *)buf;

  // Fill the buffer with the correct information
  vertex source_vertex = network[local_id[0]];
  vertex_buffer->id = source_vertex.id;

  // Go through all the edges and add them. Vector's copy constructor failed
  for (int i = 0; i < source_vertex.out_edges.size(); i++) {
    out_edge new_out_edge;
    new_out_edge.destination_node_id =
        source_vertex.out_edges[i].destination_node_id;
    new_out_edge.rank_location = source_vertex.out_edges[i].rank_location;
    new_out_edge.flow = source_vertex.out_edges[i].flow;
    new_out_edge.capacity = source_vertex.out_edges[i].capacity;
    vertex_buffer->out_edges.push_back(new_out_edge);
  }

  // Separate for loops for when not same sizes/degrees in/out
  for (int i = 0; i < source_vertex.in_edges.size(); i++) {
    in_edge new_in_edge;
    new_in_edge.source_node_id = source_vertex.in_edges[i].source_node_id;
    new_in_edge.rank_location = source_vertex.in_edges[i].rank_location;
    vertex_buffer->in_edges.push_back(new_in_edge);
  }

  // vertex_buffer->in_edges = new
  // vector<in_edge>(network[local_id[0]].in_edges); vertex_buffer->out_edges
  // =
  //     new vector<out_edge>(network[local_id[0]].out_edges);

  *ierr = ZOLTAN_OK;
}

// Copy all needed data for a single object into the application data structure
void user_unpack(void *data, int num_global_ids, ZOLTAN_ID_PTR global_id,
                 int size, char *buf, int *ierr) {
  printf("TEST------\n");
  // Cast data to vertex pointer type
  vertex *vertex_buffer = (vertex *)buf;

  vertex new_vertex;
  new_vertex.id = (unsigned long long)global_id;

  // Need to pass in/out_edges, based on communication buffer in user_pack
  // Go through all the edges and add them. Vector's copy constructor failed
  for (int i = 0; i < vertex_buffer->out_edges.size(); i++) {
    out_edge new_out_edge;
    new_out_edge.destination_node_id =
        vertex_buffer->out_edges[i].destination_node_id;
    new_out_edge.rank_location = vertex_buffer->out_edges[i].rank_location;
    new_out_edge.flow = vertex_buffer->out_edges[i].flow;
    new_out_edge.capacity = vertex_buffer->out_edges[i].capacity;
    new_vertex.out_edges.push_back(new_out_edge);
  }

  // Separate for loops for when not same sizes/degrees in/out
  for (int i = 0; i < vertex_buffer->in_edges.size(); i++) {
    in_edge new_in_edge;
    new_in_edge.source_node_id = vertex_buffer->in_edges[i].source_node_id;
    new_in_edge.rank_location = vertex_buffer->in_edges[i].rank_location;
    new_vertex.in_edges.push_back(new_in_edge);
  }

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
  // Pack/unpack for non-automigrate versions
  // Zoltan_Set_Fn(zz, ZOLTAN_OBJ_SIZE_FN_TYPE, (void
  // (*)())user_return_obj_size,
  //               NULL);
  // Zoltan_Set_Fn(zz, ZOLTAN_PACK_OBJ_FN_TYPE, (void (*)())user_-- --, NULL);
  // Zoltan_Set_Fn(zz, ZOLTAN_UNPACK_OBJ_FN_TYPE, (void (*)())user_-- ---,
  // NULL); Zoltan_Set_Fn(zz, ...... _);

  /* Set Zoltan paramaters. */
  // Zoltan_Set_Param(zz, "DEBUG?", "4?");
  Zoltan_Set_Param(zz, "LB_METHOD", "GRAPH");
  Zoltan_Set_Param(zz, "AUTO_MIGRATE",
                   "TRUE"); // Might need user-guided for edges?

  // Initialize network
  for (int i = 0; i < 20; i++) {
    vertex new_vertex;
    new_vertex.id = i;
    network.push_back(new_vertex);
  }

  // Basing on https://cs.sandia.gov/Zoltan/ug_html/ug_examples_lb.html
  int num_changes; // Set to 1/True if decomposition was changed
  int num_imported, num_exported, *import_processors, *export_processors;
  int *import_to_parts, *export_to_parts;
  int num_gid_entries, num_lid_entries;
  ZOLTAN_ID_PTR import_global_ids, export_global_ids;
  ZOLTAN_ID_PTR import_local_ids, export_local_ids;
  // Parameters essentially: global info(output), import info, export info
  printf("r%d: Entering lb partition. n=%d\n", mpi_rank, network.size());
  Zoltan_LB_Partition(zz, &num_changes, &num_gid_entries, &num_lid_entries,
                      &num_imported, &import_global_ids, &import_local_ids,
                      &import_processors, &import_to_parts, &num_exported,
                      &export_global_ids, &export_local_ids, &export_processors,
                      &export_to_parts);
  // Also handles data migration as AUTO_MIGRATE was set to true

  /*begin algorithm?*/

  printf("r%d: imported %d, exported %d\n", mpi_rank, num_imported,
         num_exported);

  // Temporary testing: distribute and stuff, print w/ rank

  /*Begin closing/freeing things*/
  Zoltan_LB_Free_Part(&import_global_ids, &import_local_ids, &import_processors,
                      &import_to_parts);
  Zoltan_LB_Free_Part(&export_global_ids, &export_local_ids, &export_processors,
                      &export_to_parts);

  Zoltan_Destroy(&zz);
  MPI_Finalize();

  return -1;
}