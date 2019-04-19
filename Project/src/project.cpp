/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

// Making initial source code for now

#include <stdio.h>
#include <stdlib.h>

#include "zoltan.h"

using namespace std;

/************MPI Stuff ***************/

int mpi_rank;
int mpi_size;

/************Zoltan Library stuff*****/
struct Zoltan_Strcut *zz;
float version;
/*************************************/

struct edge {
  unsigned long long destination_node_id;
  int rank_location;
};

struct label {
  unsigned long long previous_node;
  int value;
};

struct vertex {
  unsigned long long id = -1; // Should match index

  // Lists of the edges, which are pairs of capacities and vertex IDs
  vector<edge> out_edges;
  vector<edge> in_edges;
  // edge[] out_edges;
  // edge[] in_edges;

  // Match indices with out_edges
  int[] flows;
  int[] capacities;
};

// Something to store vertices. array?
int local_vertex_count;

/*********** Zoltan Functions ***************/

// query function, returns the number of objects assigned to the processor
void user_return_num_obj(void *data, int *ierr) {
  int *result = (int *)data;

  *result = local_vertex_count;

  *ierr = ZOLTAN_OK;
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

  // ZOLTAN_ID_PTRs are arrays of the partitioned global IDs, assigned objects

  // Global IDs are vertex IDs, local IDs are array indices
  // TODO check, update all edges to have correct allocated ranks in them

  *result = local_vertex_count;

  *ierr = ZOLTAN_OK;
}

/********************************************/

int max_flow(int source_id, int sink_id) { return -1; }

// For now going to assume all ranks will load the entire graph
int main(int argc, char **argv) {

  MPI::Init(argc, argv);
  mpi_rank = MPI : COMM_WORLD.Get_rank();
  mpi_size = MPI::COMM_WORLD.Get_size();

  // Zoltan Initialization
  Zoltan_Initialize(argc, argv, &version);
  zz = new Zoltan(MPI::COMM_WORLD);
  // zz = Zoltan_Create(MPI_COMM_WORLD);

  /* Register query functions */
  // zz->Set_Fn(ZOLTAN_NUM_)
  Zoltan_Set_Fn(zz, ZOLTAN_NUM_OBJ_FN_TYPE, (void (*)())user_return_num_obj,
                NULL);
  Zoltan_Set_Fn(zz, ZOLTAN_OBJ_LIST_FN_TYPE, (void (*)())user_return_obj_list,
                NULL);
  Zoltan_Set_Fn(zz, ...... _);

  /* Set Zoltan paramaters. */
  Zoltan_SetParam(zz, "DEBUG?", "4?");

  /*begin alogirhtm?*/

  /*Begin closing/freeing things*/
  Zoltan_Destroy(&zz);
  MPI::Finalize();

  return -1;
}