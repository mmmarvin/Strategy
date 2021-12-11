#include "deferredthreadpool.h"

#include <mutex>
#include "thread_algorithm.h"

namespace gengine2d
{
  DeferredThreadPool::DeferredThreadPool() :
    m_threadJoiner(m_threads),
    m_run(false),
    m_done(false)
  {
    // subtract one, so that the main thread can also do work while the thread pool
    // is doing some work
    auto threadCount = std::thread::hardware_concurrency();
    m_threads.resize(threadCount);
    for(decltype(threadCount) i = 0u; i < threadCount; ++i)
        m_threads[i] = std::thread(static_cast<void(DeferredThreadPool::*)()>(&DeferredThreadPool::doJob), this);
  }

  DeferredThreadPool::~DeferredThreadPool()
  {
    close();
  }

  void DeferredThreadPool::close()
  {
    m_done = true;
    m_runCondition.notify_all();
  }

  void DeferredThreadPool::run()
  {
    m_run = true;
    m_runCondition.notify_all();

//        std::function<void()> work;
    JobDescription jobDes;
    while(m_workQueue.try_pop(jobDes)) {
      jobDes.job();
    }

    m_run = false;
  }

  void DeferredThreadPool::doJob()
  {
    while(!m_done) {
      mutex_t m;
      std::unique_lock<mutex_t> l(m);
      m_runCondition.wait(l, [this] { return m_run || m_done;  });

      while(m_run) {
        JobDescription jobDesc;
        if(m_workQueue.try_pop(jobDesc)) {
          jobDesc.job();
        } else {
          std::this_thread::yield();
        }
      }
    }
  }
}
