// File:    pthread-wrappers.cpp
// Purpose: C++ style wrappers for pthread functions

#include "pthread-wrappers.h"

// Mutex wrapper
Mutex::Mutex() : mtx() { pthread_mutex_init(&mtx, NULL); }
Mutex::~Mutex() { pthread_mutex_destroy(&mtx); }
void Mutex::lock() { pthread_mutex_lock(&mtx); }
bool Mutex::try_lock() { return pthread_mutex_trylock(&mtx) == 0; }
void Mutex::unlock() { pthread_mutex_unlock(&mtx); }

// Barrier wrapper
Barrier::Barrier(unsigned int thread_count) : barrier() {
  pthread_barrier_init(&barrier, NULL, thread_count);
}
Barrier::~Barrier() { pthread_barrier_destroy(&barrier); }
void Barrier::wait() { pthread_barrier_wait(&barrier); }

// Condition variable wrapper
CondVar::CondVar() : cond() { pthread_cond_init(&cond, NULL); }
CondVar::~CondVar() { pthread_cond_destroy(&cond); }
void CondVar::wait(Mutex &m) { pthread_cond_wait(&cond, &m.mtx); }
void CondVar::notify() { pthread_cond_signal(&cond); }
void CondVar::notify_all() { pthread_cond_broadcast(&cond); }

// Scoped lock implementation
ScopedLock::ScopedLock(Mutex &m) : mtx(&m) { mtx->lock(); }
ScopedLock::~ScopedLock() { mtx->unlock(); }
