#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include "../../type.h"

namespace gengine2d
{
  // TODO: implement thread-safe lock-free queue
  template<class T>
  class ThreadSafeQueue
  {
  public:
    ThreadSafeQueue() {}

    void push(const T& t);
    void push(T&& t);

    std::unique_ptr<T> pop();
    void pop(T& t);

    std::unique_ptr<T> try_pop();
    bool try_pop(T& t);

    bool empty() const;
    std::size_t size() const;

  private:
    queue_t<T>      m_queue;
    mutable mutex_t m_queueMutex;
  };
}

// definitions
#include "threadsafequeue.inl"

#endif // THREADSAFEQUEUE_H
