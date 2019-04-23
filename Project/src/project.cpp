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

// Something to store vertices. array?
int local_vertex_count;
vector<vertex> network;

/*********** Zoltan Functions ***************/

// Return the byte size of the object
int user_return_obj_size(void *data, int num_global_ids,
                         ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id,
                         int *ierr) {
  return sizeof(vertex); // Accounting for edges?
}

// query function, returns the number of objects assigned to the processor
void user_return_num_obj(void *data, int *ierr) {
  int *result = (int *)data;

  *result = local_vertex_count;

  *ierr = ZOLTAN_OK;
}

// https://cs.sandia.gov/Zoltan/ug_html/ug_query_lb.html#ZOLTAN_OBJ_LIST_FN
void user_return_obj_list(void *data, int num_gid_entries, int num_lid_entries,
                          ZOLTAN_ID_PTR global_ids, ZOLTAN_ID_PTR local_ids,
                          int wgt_dim, float *obj_wgts, int *ierr) {
  // wgt_dim and obj_wgts are unused, left as default values

  int *result =
      (int *)data; // TODO set to full data set that is being split up (?)

  // Each global ID is a unsigned long long (2*sizeof(int)), local are ints
  // (array indices)
  if (num_gid_entries != 2 /*TODO verify*/ || num_lid_entries != 1) {
    *ierr = ZOLTAN_FATAL;
    return;
  }

  // ZOLTAN_ID_PTRs are arrays of the partitioned IDs, assigned objects

  // Use LB_Partition to allocate; call once, so after ran it?

  // Global IDs are vertex IDs, local IDs are array indices
  // TODO check, update all edges to have correct allocated ranks in them

  *result = local_vertex_count;

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
void user_pack(void *data, int num_global_ids, int num_local_ids,
               ZOLTAN_ID_PTR global_id, ZOLTAN_ID_PTR local_id, int destination,
               int size, char *buf, int *ierr) {
  // Cast data to vertex pointer type
  vertex *input_data = (vertex *)data;

  // Create a buffer

  // Send the buffer to the destination part/processor (MPI rank?)

  *ierr = ZOLTAN_OK;
}

// Copy all needed data for a single object into the application data structure
void user_unpack(void *data, int num_global_ids, ZOLTAN_ID_PTR global_id,
                 int size, char *buf, int *ierr) {
  // Cast data to vertex pointer type
  vertex *input_data = (vertex *)data;

  vertex new_vertex;
  new_vertex.id = (unsigned long long)global_id;

  // Need to pass in/out_edges, based on communication buffer in user_pack

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
  Zoltan_Set_Fn(zz, ZOLTAN_OBJ_SIZE_FN_TYPE, (void (*)())user_return_obj_size,
                NULL);

  // Pack/unpack for non-automigrate versions
  // Zoltan_Set_Fn(zz, ZOLTAN_PACK_OBJ_FN_TYPE, (void (*)())user_-- --, NULL);
  // Zoltan_Set_Fn(zz, ZOLTAN_UNPACK_OBJ_FN_TYPE, (void (*)())user_-- ---,
  // NULL); Zoltan_Set_Fn(zz, ...... _);

  /* Set Zoltan paramaters. */
  Zoltan_Set_Param(zz, "DEBUG?", "4?");
  Zoltan_Set_Param(zz, "LB_METHOD", "GRAPH");
  Zoltan_Set_Param(zz, "AUTO_MIGRATE",
                   "TRUE"); // Might need user-guided for edges?

  // Basing on https://cs.sandia.gov/Zoltan/ug_html/ug_examples_lb.html
  int num_changes; // Set to 1/True if decomposition was changed
  int num_imported, num_exported, *import_processors, *export_processors;
  int *import_to_parts, *export_to_parts;
  int num_gid_entries, num_lid_entries;
  ZOLTAN_ID_PTR import_global_ids, export_global_ids;
  ZOLTAN_ID_PTR import_local_ids, export_local_ids;

  // Parameters essential: global info (output), import info, export info
  Zoltan_LB_Partition(zz, &num_changes, &num_gid_entries, &num_lid_entries,
                      &num_imported, &import_global_ids, &import_local_ids,
                      &import_processors, &import_to_parts, &num_exported,
                      &export_global_ids, &export_local_ids, &export_processors,
                      &export_to_parts);
  // if (num_changes?)
  // perform_data_migration(...);

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