// File:    data-structures.cpp
// Purpose:
#include "data-structures.h"

const struct label EMPTY_LABEL = {(global_id)-1, 0};

EdgeQueue::EdgeQueue() {
  auto *node = new QueueNode(); // Allocate a new node
  node->next = NULL;            // Make it the only node in the linked list
  head = tail = node;           // Both head and tail point to it
}

EdgeQueue::~EdgeQueue() {
  QueueNode *node = head;
  // delete rest of queue
  while (node != NULL) {
    head = node->next;
    delete node;
    node = head;
  }
}

void EdgeQueue::push(const struct edge_entry &value) {
  auto *node = new QueueNode(value); // Allocate a new node
  ScopedLock l(t_lock);              // Acquire t_lock in order to access tail
  tail->next = node;                 // Link node at the end of the linked list
  tail = node;                       // Swing tail to node
}

bool EdgeQueue::pop(struct edge_entry &entry) {
  QueueNode *node = head;           // Read head
  QueueNode *new_head = node->next; // Read next pointer
  if (new_head == NULL) {           // Is queue empty?
    return false;                   // Queue was empty
  }
  entry = new_head->value; // Queue not empty. Read value before release
  head = new_head;         // Swing head to next node
  delete node;             // Free node
  return true;             // Queue was not empty, dequeue succeeded
}
