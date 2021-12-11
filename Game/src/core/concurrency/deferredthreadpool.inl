#include "deferredthreadpool.h"

#include "../3rdparty/high_resolution_clock.h"

namespace gengine2d
{
  template<class DebuggerT>
  DeferredThreadPool::DeferredThreadPool(DebuggerT& debugger) :
      m_threadJoiner(m_threads),
      m_run(false),
      m_done(false)
  {
    // subtract one, so that the main thread can also do work while the thread pool
    // is doing some work
    auto threadCount = std::thread::hardware_concurrency();
    m_threads.resize(threadCount);
    for(decltype(threadCount) i = 0u; i < threadCount; ++i) {
        m_threads[i] = std::thread(static_cast<void(DeferredThreadPool::*)(DebuggerT&, unsigned int)>(&DeferredThreadPool::doJob), this, std::ref(debugger), i + 1);
    }
  }

  template<class FuncT>
  void DeferredThreadPool::addJob(unsigned int id, FuncT&& func)
  {
    m_workQueue.push(JobDescription(id, std::forward<FuncT>(func)));
  }

  template<class DebuggerT>
  void DeferredThreadPool::run(DebuggerT& debugger, unsigned int jobID)
  {
    m_run = true;
    m_runCondition.notify_all();

//        std::function<void()> work;
    JobDescription jobDes;
    while(m_workQueue.try_pop(jobDes)) {
      mar::high_resolution_clock clock;
      jobDes.job();
      debugger.addValue(jobDes.id, jobID, clock.get_elapsed().as_milliseconds());
    }

    m_run = false;
  }

  template<class DebuggerT>
  void DeferredThreadPool::doJob(DebuggerT& debugger, unsigned int jobID)
  {
    while(!m_done) {
      mutex_t m;
      std::unique_lock<mutex_t> l(m);
      m_runCondition.wait(l, [this] { return m_run || m_done;  });

      while(m_run) {
//                std::function<void()> job;
        JobDescription jobDesc;
        if(m_workQueue.try_pop(jobDesc)) {
          mar::high_resolution_clock clock;
          jobDesc.job();
          debugger.addValue(jobDesc.id, jobID, clock.get_elapsed().as_milliseconds());
        } else {
          std::this_thread::yield();
        }
      }
    }
  }
}
