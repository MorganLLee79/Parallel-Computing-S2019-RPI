/* Parallel Computing Project S2019
 * Eric Johnson, Chris Jones, Harrison Lee
 */

#include <mpi.h>

#include <cstdlib>
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
  /// Sent to rank 0 after the algorithm finishes, contains the flow through
  /// the graph
  TOTAL_FLOW,
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
 * Maps global IDs to local IDs. Needs to be fast for border nodes.
 *
 * @param id The global ID to lookup
 * @return The local ID of the given node, or @c (local_id)-1 if not found.
 */
local_id lookup_global_id(global_id id) {
  // FIXME: very slow implementation for testing; replace with std::map, at
  //  least for border nodes.
  for (local_id i = 0; i < vertices.size(); ++i) {
    if (vertices[i].id == id) {
      return i;
    }
  }
  return -1;
}

/**
 * Inserts edges between @c vertices[vert_idx] and neighboring unlabelled nodes
 * into the edge queue.
 *
 * @param vert_idx The local index of a newly labelled node.
 */
void insert_edges(local_id vert_idx) {
  const struct vertex &v = vertices[vert_idx];
  for (unsigned int i = 0; i < v.out_edges.size(); ++i) {
    const out_edge &edge = v.out_edges[i];
    if (edge.rank_location == mpi_rank && labels[edge.vert_index].value != 0) {
      continue; // already has a label, skip it
    }
    if (edge.dest_node_id == labels[vert_idx].prev_node) {
      continue; // we came from here, so skip it
    }
    edge_entry temp = {
        vert_idx, // vertex_index
        true,     // is_outgoing
        i,        // edge_index
    };
    edge_queue.push(temp);
  }

  for (unsigned int i = 0; i < v.in_edges.size(); ++i) {
    const in_edge &edge = v.in_edges[i];
    if (edge.rank_location == mpi_rank && !labels[edge.vert_index].value) {
      continue; // already has a label, skip it
    }
    if (edge.dest_node_id == labels[vert_idx].prev_node) {
      continue; // we came from here, so skip it
    }
    edge_entry temp = {
        vert_idx, // vertex_index
        false,    // is_outgoing
        i,        // edge_index
    };
    edge_queue.push(temp);
  }
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
bool set_label(global_id prev_node, int prev_rank, local_id prev_idx,
               local_id curr_idx, int value);

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
      local_id i = lookup_global_id(source_id);
      if (i != (local_id)-1) {
        set_label(source_id, mpi_rank, i, i,
                  numeric_limits<decltype(labels[i].value)>::max());
      }
    }

    /**
     * In step 3, holds the local index of the node that the backtracking
     * algorithm is currently processing. Local to each thread.
     *
     * Set to @c (local_id)-1 if the current backtracking node is not on this
     * rank.
     */
    local_id bt_idx = -1;
    /// Only the thread that will handle step 3 sets this flag.
    bool do_step_3 = false;
    /// Label value of sink node
    int sink_value = 0;

    // all threads must wait until everything is initialized
    barrier.wait();

    /*--------*
     | Step 2 |
     *--------*/
    // Thread 0 handles all incoming messages, while the other threads run the
    // actual algorithm
    if (tid == 0) {
      struct message_data msg = {};
      local_id vert_idx;
      int curr_flow;
      while (!sink_found) {
        // if message tag is SINK_FOUND, set do_step_3 and sink_found to true,
        // so thread 0 on this rank will do step 3.
        MPI_Status stat;
        MPI_Recv(&msg, 1, MPI_MESSAGE_TYPE, MPI_ANY_SOURCE, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &stat);
        switch (stat.MPI_TAG) {
        case SET_TO_LABEL:
          // try to set label of "to" node
          vert_idx = lookup_global_id(msg.receivers_node);
          if (vert_idx == (local_id)-1) {
            cerr << "Warning: SET_TO_LABEL sent to wrong rank\n";
            continue;
          }
          if (set_label(msg.senders_node, stat.MPI_SOURCE, -1, vert_idx,
                        msg.label_value)) {
            // found sink!
            bt_idx = vert_idx;
            do_step_3 = true;
            sink_found = true;
          }
          break;
        case COMPUTE_FROM_LABEL:
          // compute and set label of "from" node
          vert_idx = lookup_global_id(msg.receivers_node); // from_id
          if (vert_idx == (local_id)-1) {
            cerr << "Warning: COMPUTE_FROM_LABEL sent to wrong rank\n";
            continue;
          }

          // find edge for the sender's node, and get the flow through it
          curr_flow = 0;
          for (auto it = vertices[vert_idx].out_edges.begin();
               it != vertices[vert_idx].out_edges.end(); ++it) {
            if (it->dest_node_id == msg.senders_node) {
              curr_flow = it->flow;
            }
          }
          if (curr_flow <= 0) {
            continue; // discard edge
          }

          // set label and add edges
          if (set_label(msg.senders_node, stat.MPI_SOURCE, -1, vert_idx,
                        -min(abs(msg.label_value), curr_flow))) {
            // found sink!
            cerr << "Warning: outgoing edge from sink!\n";
            bt_idx = vert_idx;
            do_step_3 = true;
            sink_found = true;
          }
          break;
        case SINK_FOUND:
          do_step_3 = true;
          sink_found = true;
          break;
        default:
          cerr << "Error: got invalid tag in step 2: " << stat.MPI_TAG << endl;
          break;
        }
      }
    } else {
      struct edge_entry entry = {0, false, 0};
      while (!sink_found) {
        {
          ScopedLock l(edge_queue.h_lock);
          // wait for the queue to become non-empty
          while (!edge_queue.pop(entry) && !sink_found)
            ;
          // release the lock on edge_queue now, so other threads can get edges
        }

        if (sink_found)
          break;

        // process edge
        if (entry.is_outgoing) {
          bt_idx = handle_outgoing_edge(entry);
        } else {
          bt_idx = handle_incoming_edge(entry);
        }
        if (bt_idx != (local_id)-1) {
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
    if (bt_idx != (local_id)-1) {
      sink_value = labels[bt_idx].value;
    }
    // tell the next rank to stop
    MPI_Send(NULL, 0, MPI_MESSAGE_TYPE, (mpi_rank + 1) % mpi_size, SINK_FOUND,
             MPI_COMM_WORLD);
    // TODO: may need to wait for message to make it all the way around before
    //  starting?
    // start backtracking
    bool wait_for_sink_found = bt_idx != (local_id)-1;
    bool wait_for_source_found = false;
    while (do_step_3) {
      if (bt_idx != (local_id)-1) {
        // update flow in local nodes
        struct label &l = labels[bt_idx];
        // TODO: looping over all edges will be slow for dense graphs
        if (l.value > 0 && l.prev_rank_loc == mpi_rank) {
          // bt_idx is a "from" node and previous node is local
          // let f(y, x) += sink_value
          for (auto it = vertices[l.prev_vert_index].out_edges.begin();
               it != vertices[l.prev_vert_index].out_edges.end(); ++it) {
            if (it->dest_node_id == vertices[bt_idx].id)
              it->flow += sink_value;
          }
        } else if (l.value < 0) {
          // let f(x, y) -= sink_value
          for (auto it = vertices[bt_idx].out_edges.begin();
               it != vertices[bt_idx].out_edges.end(); ++it) {
            if (it->dest_node_id == l.prev_node)
              it->flow -= sink_value;
          }
        }

        // if the previous node is not on this rank, send the next rank an
        // UPDATE_FLOW message, then wait for incoming messages
        if (l.prev_rank_loc != mpi_rank) {
          // previous node is remote, send an UPDATE_FLOW message
          struct message_data msg = {
              vertices[bt_idx].id, // sender's node
              l.prev_node,         // receiver's node
              sink_value,          // label value
          };
          MPI_Send(&msg, 1, MPI_MESSAGE_TYPE, l.prev_rank_loc, UPDATE_FLOW,
                   MPI_COMM_WORLD);
          bt_idx = -1;
        } else {
          // check for source node
          if (bt_idx == l.prev_vert_index && l.prev_node == source_id) {
            // source node was already processed, exit the loop
            wait_for_source_found = true;
            break;
          }
          // otherwise, keep following back-pointers
          bt_idx = l.prev_vert_index;
        }
      } else {
        // wait for incoming messages
        struct message_data msg = {};
        MPI_Status stat;
        MPI_Recv(&msg, 1, MPI_MESSAGE_TYPE, MPI_ANY_SOURCE, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &stat);
        switch (stat.MPI_TAG) {
        case SINK_FOUND:
          wait_for_sink_found = false;
          break;
        case SOURCE_FOUND:
          // if SOURCE_FOUND is received, break and forward it the next rank
          wait_for_source_found = false;
          do_step_3 = false;
          break;
        case UPDATE_FLOW: {
          // find our local node
          sink_value = msg.label_value;
          local_id vert_idx = lookup_global_id(msg.receivers_node);
          auto it = vertices[vert_idx].out_edges.begin();
          // find the remote node in the local node's edge list
          for (; it != vertices[vert_idx].out_edges.end(); ++it) {
            if (it->dest_node_id == msg.senders_node)
              it->flow += sink_value;
          }
          // if the sender's node is not found in out_edges, then vert_idx must
          // be the "to" node and we don't need to do anything
          bt_idx = vert_idx; // continue with the previous node
        } break;
        default:
          cerr << "Error: got invalid tag in step 3: " << stat.MPI_TAG << endl;
          break;
        }
      }
    }

    // send SOURCE_FOUND message to next rank
    MPI_Send(NULL, 0, MPI_MESSAGE_TYPE, (mpi_rank + 1) % mpi_size, SOURCE_FOUND,
             MPI_COMM_WORLD);

    // wait to receive the SINK_FOUND and SOURCE_FOUND messages from previous
    // rank if necessary
    vector<MPI_Request> requests;
    vector<MPI_Status> statuses;
    if (wait_for_sink_found) {
      requests.push_back(MPI_Request());
      MPI_Irecv(NULL, 0, MPI_MESSAGE_TYPE, (mpi_rank - 1 + mpi_size) % mpi_size,
                SINK_FOUND, MPI_COMM_WORLD, &requests.back());
      statuses.push_back(MPI_Status());
    }
    if (wait_for_source_found) {
      requests.push_back(MPI_Request());
      MPI_Irecv(NULL, 0, MPI_MESSAGE_TYPE, (mpi_rank - 1 + mpi_size) % mpi_size,
                SOURCE_FOUND, MPI_COMM_WORLD, &requests.back());
      statuses.push_back(MPI_Status());
    }
    MPI_Waitall(requests.size(), requests.data(), statuses.data());
  }

  return NULL;
}

bool set_label(global_id prev_node, int prev_rank, local_id prev_idx,
               local_id curr_idx, int value) {
  // atomically set label, only if it was unset before
  if (__sync_bool_compare_and_swap(&labels[curr_idx].value, 0, value)) {
    // label was unset before, so go ahead and set prev pointer
    labels[curr_idx].prev_node = prev_node;
    labels[curr_idx].prev_rank_loc = prev_rank;
    labels[curr_idx].prev_vert_index = prev_idx;
    if (vertices[curr_idx].id == sink_id) {
      // set flag
      sink_found = true;
      return true;
    } else {
      // add edges to queue
      insert_edges(curr_idx);
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

  int label_val = min(abs(labels[from_id].value), flow_diff);
  // check if "to" node is on another rank
  if (edge.rank_location == mpi_rank) {
    // set label and add edges
    if (set_label(vertices[from_id].id, mpi_rank, from_id, edge.vert_index,
                  label_val)) {
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
    local_id from_id = rev_edge.vert_index;
    int curr_flow = vertices[from_id].out_edges[to_id].flow;
    if (curr_flow <= 0) {
      return -1; // discard edge
    }

    int label_val = -min(abs(labels[to_id].value), curr_flow);

    // set label and add edges
    if (set_label(vertices[to_id].id, mpi_rank, to_id, from_id, label_val)) {
      cerr << "Warning: outgoing edge from sink!\n";
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

int calc_max_flow() {
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

  // sum up flow out of source node
  local_id src_idx = lookup_global_id(source_id);
  int total_flow = -1;
  if (src_idx != (local_id)-1) {
    total_flow = 0;
    for (local_id i = 0; i < vertices[src_idx].out_edges.size(); ++i) {
      total_flow += vertices[src_idx].out_edges[i].flow;
    }
  }
  // send to rank 0
  if (mpi_rank == 0) {
    if (total_flow == -1) {
      // need to receive the total flow from another node
      MPI_Status stat;
      MPI_Recv(&total_flow, 1, MPI_INT, MPI_ANY_SOURCE, TOTAL_FLOW,
               MPI_COMM_WORLD, &stat);
      // TODO: check status?
    }
    // otherwise, we have already have the flow
  } else {
    if (total_flow != -1) {
      MPI_Send(&total_flow, 1, MPI_INT, 0, TOTAL_FLOW, MPI_COMM_WORLD);
      total_flow = -1;
    }
  }

  return total_flow;
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
    MPI_Type_commit(&MPI_MESSAGE_TYPE);
  }

  // do setup

  graph_node_count = 10;
  source_id = 0;
  sink_id = graph_node_count - 1;

  int max_flow = calc_max_flow();
  if (mpi_rank == 0) {
    cout << "Max flow: " << max_flow << endl;
  }

  MPI_Finalize();
  return 0;
}
