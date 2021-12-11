#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include "../type.h"
#include "../singleton.h"
#include "containers/threadsafequeue.h"
#include "threadjoiner.h"

namespace gengine2d
{
  class ThreadPool
  {
  public:
    ThreadPool();
    ~ThreadPool();

  public:
    template<class Func> void addJob(Func&& func);

  private:
    friend class Singleton<ThreadPool>;

    void doJob();

    ThreadSafeQueue<std::function<void()>> m_workQueue;
    vector_t<std::thread>                   m_threads;

    ThreadJoiner<decltype(m_threads)>      m_threadJoiner;

    bool                                    m_done;
  };
}

// definitions
#include "threadpool.inl"

#endif // THREADPOOL_H
