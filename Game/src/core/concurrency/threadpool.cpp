#include "threadpool.h"

namespace gengine2d
{
  ThreadPool::ThreadPool() :
    m_threadJoiner(m_threads),
    m_done(false)
  {
  }

  ThreadPool::~ThreadPool()
  {
    m_done = true;
  }

  void ThreadPool::doJob()
  {
    while(!m_done) {
      std::function<void()> job;
      if(m_workQueue.try_pop(job)) {
        job();
      } else {
        std::this_thread::yield();
      }
    }
  }
}
