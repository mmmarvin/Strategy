#include "threadpool.h"

namespace gengine2d
{
  template<class Func>
  void ThreadPool::addJob(Func&& func)
  {
    m_workQueue.push(std::forward<Func>(func));
  }
}
