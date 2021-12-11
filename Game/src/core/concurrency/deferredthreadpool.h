#ifndef DEFERREDTHREADPOOL_H
#define DEFERREDTHREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <thread>
#include "../singleton.h"
#include "../type.h"
#include "containers/threadsafequeue.h"
#include "threadjoiner.h"

namespace gengine2d
{
  class DeferredThreadPool
  {
  public:
    DeferredThreadPool();
    ~DeferredThreadPool();
    template<class DebuggerT> explicit DeferredThreadPool(DebuggerT& debugger);

  public:
    struct JobDescription
    {
      JobDescription() {}
      template<class Func> JobDescription(unsigned int id_,
                                          Func&& job_) :
        job(std::forward<Func>(job_)),
        id(id_)
      {
      }

      std::function<void()>   job;
      unsigned int            id;
    };

    void close();

    template<class FuncT> void addJob(unsigned int id, FuncT&& func);

    void run();
    template<class DebuggerT> void run(DebuggerT& debugger, unsigned int jobID);

    std::size_t getThreadsCount() const { return m_threads.size(); }

  private:
    friend class Singleton<DeferredThreadPool>;

    void doJob();
    template<class DebuggerT> void doJob(DebuggerT& debugger, unsigned int jobID);

    ThreadSafeQueue<JobDescription>         m_workQueue;
    vector_t<std::thread>                   m_threads;

    ThreadJoiner<decltype(m_threads)>       m_threadJoiner;

    std::condition_variable                 m_runCondition;
    std::atomic_bool                        m_run;
    bool                                    m_done;
  };
}

// definitions
#include "deferredthreadpool.inl"

#endif // DEFERREDTHREADPOOL_H
