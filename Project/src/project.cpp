/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

#include <mpi.h>

#include <iostream>
#include <limits>
#include <vector>

#include "data-structures.h"
#include "pthread-wrappers.h"

using namespace std;

/************MPI Variables *********************/
int mpi_rank;
int mpi_size;
MPI_Datatype MPI_MESSAGE_TYPE;

struct message_data {
  /// The ID of the node belonging to the sender
  global_id senders_node;
  /// The ID of the node belonging to the receiver
  global_id receivers_node;
  /// The relevant label value (identity depends on message type)
  int label_value;
};

enum message_tags : int {
  /// Set the label on a node, generated from an incoming edge
  SET_TO_LABEL = 1,
  /// Compute and set the label on a node, generated from an outgoing edge
  COMPUTE_FROM_LABEL,
  /// Another rank found the sink node in step 2, move on to step 3. Pass on to
  /// the next rank.
  SINK_FOUND,
  /// Used during step 3
  UPDATE_FLOW,
  /// Another rank found the source node in step 3; go back to step 1. Pass on
  /// to the next rank.
  SOURCE_FOUND,
};

/****************** Globals ********************/

size_t num_threads = 64;
global_id graph_node_count;

// source and sink ids
global_id source_id = -1;
global_id sink_id = -1;

// entries in `vertices` and entries in `labels` must correspond one-to-one
vector<struct vertex> vertices;
vector<struct label> labels;
/// Set to true when the sink node is found in step 2.
bool sink_found = false;

/// Set to true when no valid paths can be found through the graph.
bool algorithm_complete = false;

EdgeQueue edge_queue;

struct thread_params {
  int tid;
  Barrier &barrier;
};

/**
 * Inserts edges between @p vert and neighboring unlabelled nodes into the edge
 * queue.
 *
 * @param vert A newly labelled node.
 */
void insert_edges(const vertex &vert) {
  // TODO: implement
}

/**
 * Sets @c sink_found and returns the local id of the sink node if it was found;
 * otherwise returns (local_id)-1.
 *
 * @param entry The edge to process.
 */
local_id handle_outgoing_edge(const struct edge_entry &entry);
/**
 * Sets @c sink_found and returns the local id of the sink node if it was found;
 * otherwise returns (local_id)-1.
 *
 * @param entry The edge to process.
 */
local_id handle_incoming_edge(const struct edge_entry &entry);

/**
 * Returns @c true if @p curr_idx is the sink node and we successfully set its
 * label.
 */
bool set_label(global_id prev_node, local_id curr_idx, int value);

void *run_algorithm(struct thread_params *params) {
  int tid = params->tid;
  Barrier &barrier = params->barrier;

  // TODO: figure out how to detect deadlocks
  while (!algorithm_complete) {
    // synchronize all threads before each iteration
    barrier.wait();

    /*--------*
     | Step 1 |
     *--------*/
    if (tid == 0) {
      // wipe previous labels
      fill(labels.begin(), labels.end(), EMPTY_LABEL);
      // setup globals
      sink_found = false;
      // empty out edge queue
      edge_entry entry = {};
      while (edge_queue.pop(entry))
        ;
      // find source node
      // TODO: spread this work between all threads? Or maybe store the index if
      //  we ever have to loop over the entire graph anyway
      for (size_t i = 0; i < vertices.size(); i++) {
        if (vertices[i].id == source_id) {
          set_label(source_id, i,
                    numeric_limits<decltype(labels[i].value)>::max());
          break;
        }
      }
    }

    /**
     * In step 3, holds the local index of the node that the backtracking
     * algorithm is currently processing. Local to each thread.
     *
     * Set to @c (local_id)-1 if the current backtracking node is not on this
     * rank.
     */
    local_id backtrack_idx = -1;
    /// Only the thread that will handle step 3 sets this flag.
    bool do_step_3 = false;

    // all threads must wait until everything is initialized
    barrier.wait();

    /*--------*
     | Step 2 |
     *--------*/
    // Thread 0 handles all incoming messages, while the other threads run the
    // actual algorithm
    if (tid == 0) {
      // TODO: handle messages
      //  may need a lookup table from global ids of border nodes to local ids
      while (!sink_found) {
        // if message tag is SINK_FOUND, set do_step_3 and sink_found to true,
        // so thread 0 on this rank will do step 3.
      }
    } else {
      struct edge_entry entry = {0, false, 0};
      while (!sink_found) {
        {
          ScopedLock l(edge_queue.h_lock);
          // wait for the queue to become non-empty
          while (!edge_queue.pop(entry))
            ;
          // release the lock on edge_queue now, so other threads can get edges
        }

        // process edge
        if (entry.is_outgoing) {
          backtrack_idx = handle_outgoing_edge(entry);
        } else {
          backtrack_idx = handle_incoming_edge(entry);
        }
        if (backtrack_idx != (local_id)-1) {
          do_step_3 = true;
        }
      }
    }

    /*--------*
     | Step 3 |
     *--------*/
    // go to the beginning of the loop and wait if not handling step 3.
    if (!do_step_3) {
      continue;
    }
    // tell the next rank to stop
    MPI_Send(NULL, 0, MPI_INT, (mpi_rank + 1) % mpi_size, SINK_FOUND,
             MPI_COMM_WORLD);
    // TODO: may need to wait for message to make it all the way around before
    //  starting?
    // start backtracking
    while (true) {
      if (backtrack_idx == (local_id)-1) {
        // wait for incoming messages
        // if SOURCE_FOUND is received, forward it the next rank and break
      } else {
        // update flow in local nodes
      }
    }

    // wait to receive the SINK_FOUND message from `mpi_rank - 1` if necessary
  }

  return NULL;
}

bool set_label(global_id prev_node, local_id curr_idx, int value) {
  // atomically set label, only if it was unset before
  if (__sync_bool_compare_and_swap(&labels[curr_idx].value, 0, value)) {
    // label was unset before, so go ahead and set prev pointer
    labels[curr_idx].previous_node = prev_node;
    if (vertices[curr_idx].id == sink_id) {
      // set flag
      sink_found = true;
      return true;
    } else {
      // add edges to queue
      insert_edges(vertices[curr_idx]);
    }
  }
  return false;
}

local_id handle_outgoing_edge(const struct edge_entry &entry) {
  local_id from_id = entry.vertex_index;
  struct out_edge &edge = vertices[from_id].out_edges[entry.edge_index];

  // always compute label locally
  int flow_diff = edge.capacity - edge.flow;
  if (flow_diff <= 0) {
    return -1; // discard edge
  }

  int label_val = min(labels[from_id].value, flow_diff);
  // check if "to" node is on another rank
  if (edge.rank_location == mpi_rank) {
    // set label and add edges
    if (set_label(vertices[from_id].id, edge.vert_index, label_val)) {
      return edge.vert_index;
    }
  } else {
    // send message to the owner of the "to" node
    struct message_data msg = {
        vertices[from_id].id, // sender's node
        edge.dest_node_id,    // receiver's node
        label_val,            // label value
    };
    // TODO: Make non-blocking somehow? give it to thread 0, maybe?
    MPI_Send(&msg, 1, MPI_MESSAGE_TYPE, edge.rank_location, SET_TO_LABEL,
             MPI_COMM_WORLD);
  }
  return -1;
}

local_id handle_incoming_edge(const struct edge_entry &entry) {
  local_id to_id = entry.vertex_index;
  struct in_edge &rev_edge = vertices[to_id].in_edges[entry.edge_index];

  // check if "from" node (which holds the flow) is on another rank
  if (rev_edge.rank_location == mpi_rank) {
    // c
    local_id from_id = rev_edge.vert_index;
    int curr_flow = vertices[from_id].out_edges[to_id].flow;
    if (curr_flow <= 0) {
      return -1; // discard edge
    }

    // equivalent to -min(|l(from)|, curr_flow)
    int label_val = max(labels[to_id].value, -curr_flow);

    // set label and add edges
    if (set_label(vertices[to_id].id, from_id, label_val)) {
      return from_id;
    }
  } else {
    // send message to the owner of the "from" node
    struct message_data msg = {
        vertices[to_id].id,    // sender's node
        rev_edge.dest_node_id, // receiver's node
        labels[to_id].value,   // label value
    };
    // TODO: Make non-blocking somehow? give it to thread 0?
    MPI_Send(&msg, 1, MPI_MESSAGE_TYPE, rev_edge.rank_location,
             COMPUTE_FROM_LABEL, MPI_COMM_WORLD);
  }
  return -1;
}

int max_flow() {
  Barrier barrier(num_threads);
  pthread_t threads[num_threads];
  struct thread_params shared_params = {-1, barrier};

  // initialize vector of labels
  labels = vector<struct label>(graph_node_count, EMPTY_LABEL);

  // spawn threads
  for (size_t i = 0; i < num_threads; i++) {
    auto *params = new struct thread_params(shared_params);
    params->tid = i;
    pthread_create(&threads[i], NULL, (void *(*)(void *))run_algorithm,
                   (void *)params);
  }
  // wait for threads to finish
  for (size_t i = 0; i < num_threads; ++i) {
    pthread_join(threads[i], NULL);
  }

  // TODO: sum up flow from source node
  // must be done from the rank that has the source node, and sent to rank 0

  return 0;
}

// For now going to assume all ranks will load the entire graph
int main(int argc, char **argv) {
  int mpi_thread_support;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &mpi_thread_support);
  if (mpi_thread_support != MPI_THREAD_MULTIPLE) {
    cerr << "Error: MPI_THREAD_MULTIPLE not supported!" << endl;
    MPI_Abort(MPI_COMM_WORLD, 1);
    return 1;
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  {
    // create MPI datatype for inter-rank messages
    const int count = 3;
    int block_lengths[count] = {1, 1, 1};
    MPI_Datatype types[count] = {GLOBAL_ID_TYPE, GLOBAL_ID_TYPE, MPI_INT};
    MPI_Aint offsets[count] = {offsetof(message_data, senders_node),
                               offsetof(message_data, receivers_node),
                               offsetof(message_data, label_value)};
    MPI_Type_create_struct(count, block_lengths, offsets, types,
                           &MPI_MESSAGE_TYPE);
  }

  // do setup

  graph_node_count = 10;
  source_id = 0;
  sink_id = graph_node_count - 1;

  cout << "Max flow: " << max_flow() << endl;

  MPI_Finalize();
  return 0;
}
