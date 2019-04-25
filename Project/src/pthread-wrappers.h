// File:    pthread-wrappers.h
// Purpose: C++ style wrappers for pthread functions
#ifndef PARALLEL_PROJECT_PTHREAD_WRAPPERS_H
#define PARALLEL_PROJECT_PTHREAD_WRAPPERS_H

#include <pthread.h>

/**
 * C++ wrapper for a pthreads mutex.
 */
class Mutex {
  // Let CondVar::wait use the `mtx` variable in pthread_cond_wait.
  friend class CondVar;

private:
  pthread_mutex_t mtx;

public:
  Mutex();
  ~Mutex();
  /**
   * Locks the mutex, potentially blocking if it is currently locked.
   */
  void lock();
  /**
   * Tries to lock the mutex without blocking.
   *
   * @return @c true if the lock was successfully acquired, @c false otherwise
   */
  bool try_lock();
  /**
   * Unlocks the mutex.
   */
  void unlock();
};

/**
 * C++ wrapper for a pthreads barrier.
 */
class Barrier {
private:
  pthread_barrier_t barrier;

public:
  explicit Barrier(unsigned int thread_count);
  ~Barrier();
  void wait();
};

/**
 * C++ wrapper for a pthreads condition variable.
 */
class CondVar {
private:
  pthread_cond_t cond;

public:
  CondVar();
  ~CondVar();
  /**
   * Block on this condition variable.
   *
   * @p mutex must be locked by the calling thread.
   * This function will release @p mutex, then block until another thread calls
   * notify() or notify_all(). It will then acquire @p mutex, and return.
   *
   * This function is susceptible to spurious wakeups, so be sure to check the
   * relevant boolean predicate after this function returns.
   */
  void wait(Mutex &mutex);
  /**
   * Unblock at least one of the threads that are blocked on this condition
   * variable.
   *
   * May be called by a thread even if that thread doesn't hold the relevant
   * mutex, and will not unlock that mutex if it is held.
   */
  void notify();
  /**
   * Unblock all of the threads that are blocked on this condition variable.
   *
   * May be called by a thread even if that thread doesn't hold the relevant
   * mutex, and will not unlock that mutex if it is held.
   */
  void notify_all();
};

/**
 * RAII scoped lock using the Mutex wrapper class.
 */
class ScopedLock {
private:
  bool locked;
  Mutex *mtx;

public:
  /**
   * Acquires a lock on @p mutex, and holds it until this object goes out of
   * scope, or unlock() is called.
   */
  explicit ScopedLock(Mutex &mutex);
  /**
   * Releases the lock acquired in the constructor, unless unlock() was
   * called on this object.
   */
  ~ScopedLock();

  /**
   * Unlocks the mutex early.
   */
  void unlock();
};

#endif // PARALLEL_PROJECT_PTHREAD_WRAPPERS_H
