// File:    data-structures.h
// Purpose:
#ifndef PARALLEL_PROJECT_DATA_STRUCTURES_H
#define PARALLEL_PROJECT_DATA_STRUCTURES_H

#include <vector>

#include "pthread-wrappers.h"

/// Holds a global node ID.
typedef unsigned long long global_id;
typedef size_t local_id;
#define GLOBAL_ID_TYPE MPI_UNSIGNED_LONG_LONG

struct in_edge {
  const global_id dest_node_id;
  const int rank_location;
  const local_id vert_index;
};

struct out_edge {
  const global_id dest_node_id;
  const int rank_location;
  const local_id vert_index;
  const int capacity;
  int flow;
};

struct label {
  global_id previous_node;
  int value;
};
extern const struct label EMPTY_LABEL;

struct vertex {
  global_id id; // Should match index

  // Lists of the edges, which are pairs of capacities and vertex IDs
  std::vector<struct out_edge> out_edges;
  std::vector<struct in_edge> in_edges;
};

struct edge_entry {
  /// Index of the src node in SimData::vertices (and in SimData::labels)
  local_id vertex_index;
  /// Whether the corresponding edge is in `out_edges` or `in_edges`
  bool is_outgoing;
  /// The index of the edge in its edge list
  int edge_index;
};

class EdgeQueue;

class QueueNode {
  friend class EdgeQueue;

  const struct edge_entry value;
  QueueNode *next;

  QueueNode() : value(), next(NULL) {}
  explicit QueueNode(const struct edge_entry &value)
      : value(value), next(NULL) {}
};

/**
 * MPMC queue, based on the two-lock concurrent queue introduced by Michael and
 * Scott in https://dl.acm.org/citation.cfm?id=248106.
 *
 * Head locking is moved out of the pop function and into the main edge-
 * processing loop, so a single thread can spin without having to contend with
 * other threads for the lock.
 */
class EdgeQueue {
public:
private:
  QueueNode *head;
  QueueNode *tail;
  Mutex t_lock;

public:
  Mutex h_lock;

  EdgeQueue();
  ~EdgeQueue();

  void push(const struct edge_entry &value);

  /**
   * Try to remove an entry from the front of the queue and store it in
   * @p entry. If the queue is empty, @p entry is not modified.
   *
   * N.B. This function must only be called while holding @a h_lock.
   *
   * @return @c true if an entry was retrieved, @c false if the queue is empty
   */
  bool pop(struct edge_entry &entry);
};

#endif // PARALLEL_PROJECT_DATA_STRUCTURES_H
